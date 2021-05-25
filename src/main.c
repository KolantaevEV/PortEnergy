#include "main.h"

volatile xSemaphoreHandle CAN1_semphr;
volatile xSemaphoreHandle DMA_Ch4_semphr;
volatile xSemaphoreHandle UART1_semphr;
volatile xQueueHandle queue_to_DMA;
volatile xQueueHandle queue_to_CAN;
volatile xTimerHandle th_ADC_convert;

int main(void)
{
    RCC_init();
    GPIO_init();
    uart2Init();
    ADC1_init();
    CAN1_init();
    IRQ_init();

//============Add_Timers============
    th_ADC_convert = xTimerCreate("ADC_convert",
                                pdMS_TO_TICKS(50),
                                pdTRUE,
                                NULL,
                                tc_ADC_convert);
    if (th_ADC_convert == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
    if (xTimerStart(th_ADC_convert, 0) != pdPASS) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
//============Add_Queue============
    queue_to_DMA = xQueueCreate(5 , sizeof(buff_t));
        if (queue_to_DMA == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
    queue_to_CAN = xQueueCreate(5 , sizeof(buff_t));
        if (queue_to_DMA == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
//============Add_Semaphores========
    DMA_Ch4_semphr = xSemaphoreCreateBinary();
        if (DMA_Ch4_semphr == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
    CAN1_semphr = xSemaphoreCreateBinary();
        if (CAN1_semphr == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
    UART1_semphr = xSemaphoreCreateBinary();
        if (UART1_semphr == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
//============Add_Tasks=============
    xTaskCreate(Task_ADC_to_UART, 
                "ADC_to_UART", 
                configMINIMAL_STACK_SIZE * 2, 
                NULL, 
                4,
                (xTaskHandle *) NULL);
    xTaskCreate(Task_UART_to_CAN, 
                "UART_to_CAN", 
                configMINIMAL_STACK_SIZE * 2, 
                NULL, 
                3,
                (xTaskHandle *) NULL);
    xTaskCreate(Task_CAN_to_UART, 
                "CAN_to_UART", 
                configMINIMAL_STACK_SIZE * 2, 
                NULL, 
                3,
                (xTaskHandle *) NULL);
    xTaskCreate(Task_DMA_to_UART, 
                "CAN_to_UART",
                configMINIMAL_STACK_SIZE * 2, 
                NULL, 
                2,
                (xTaskHandle *) NULL);

    vTaskStartScheduler();

    while (1)
    {
        GPIOC->BSRR = GPIO_BSRR_BS13; //Error led
    }

    return 0;
}


void Task_ADC_to_UART(void *pvParameters)
{
    buff_t data = {0};
    volatile int16_t adc1_data[101] = {0};
        for (int i = 0; i < 101; i++) adc1_data[i] = -1;
    DMA1_Ch1_init(&ADC1->DR, adc1_data);

    float avg_temp = 0.0F;
    float avg_voltage = 0.0F;
    int i = 0;

    TickType_t curr_time = xTaskGetTickCount();

    while(1)
    {
        xTaskDelayUntil(&curr_time, pdMS_TO_TICKS(5000));

        avg_voltage = 0.0F;
        avg_temp = 0.0F;

        for (i = 0; adc1_data[i] != -1; i++)
        {
            if (i % 2 == 0) avg_voltage += adc1_data[i];
            else avg_temp += adc1_data[i];
            adc1_data[i] = -1;
        }
        avg_voltage /= ((float)i / 2.0F);
        avg_temp /= ((float)i / 2.0F);

        if (xTimerStop(th_ADC_convert, 0) != pdPASS) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
        avg_temp = ((float)avg_temp * V_REF) / 4095.0F;
        avg_temp = (TEMP_25 - avg_temp) / AVG_SLOPE + 25.0F;
        avg_voltage = ((float)avg_voltage * V_REF) / 4095.0F;
        if (xTimerStart(th_ADC_convert, 0) != pdPASS) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
        
        data = str2Char(avg_temp, avg_voltage);
        xQueueSend(queue_to_DMA, &data, portMAX_DELAY);
    }
}

void Task_UART_to_CAN(void *pvParameters)
{
    can_msg_t can_msg = {0};    
    can_msg.stid = CAN_ID_0;
    can_msg.exid = 0;
    can_msg.ide = 0;
    can_msg.rtr = 0;
    
    DMA1_Ch5_init(&USART1->DR, getUartRxBuffAdr());

    while(1)
    {
        if (can_msg.msg.cnt)
            CAN_tx_data(CAN1, &can_msg);
        else
            xQueueReceive(queue_to_CAN, &can_msg.msg, portMAX_DELAY);
    }
}

void Task_CAN_to_UART(void *pvParameters)
{
    can_msg_t can_msg = {0};

    while(1)
    {
        memmove(can_msg.msg.data, "ECHO from CAN: ", 15);
        can_msg.msg.cnt = 15;

        if (xSemaphoreTake(CAN1_semphr, portMAX_DELAY) == pdTRUE)
        {            
            CAN_rx_data(CAN1, &can_msg);            
            memmove(&can_msg.msg.data[can_msg.msg.cnt], "\x0A\x0D", 2);
            can_msg.msg.cnt += 2;
            xQueueSend(queue_to_DMA, &can_msg.msg, portMAX_DELAY);
        }
    }
}

void Task_DMA_to_UART(void *pvParameters)
{
    buff_t data = {0};
    DMA1_Ch4_init(&USART1->DR, data.data);

    while(1)
    {
        xQueueReceive(queue_to_DMA, &data, portMAX_DELAY);
        DMA1_Channel4->CNDTR = data.cnt;
        DMA1_Channel4->CCR |= DMA_CCR_EN;
        xSemaphoreTake(DMA_Ch4_semphr, portMAX_DELAY);
    }
}

void tc_ADC_convert(xTimerHandle pxTimer)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;   //Start ADC1
}


