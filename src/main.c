#include "main.h"

volatile xSemaphoreHandle CAN1_semphr;
volatile xSemaphoreHandle DMA_Ch7_semphr;
volatile xSemaphoreHandle UART1_semphr;
volatile xQueueHandle queue_to_DMA;
volatile xQueueHandle queue_to_CAN;
volatile xTimerHandle th_ADC_convert;

int main(void)
{
    RCC_init();
    GPIO_init();
    uart2Init();
    DMA1_Ch6_init(&USART2->DR, getUartRxBuffAdr());
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
    DMA_Ch7_semphr = xSemaphoreCreateBinary();
        if (DMA_Ch7_semphr == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
    CAN1_semphr = xSemaphoreCreateBinary();
        if (CAN1_semphr == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
    UART1_semphr = xSemaphoreCreateBinary();
        if (UART1_semphr == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
//============Add_Mutex=============

//============Add_Tasks=============
    xTaskCreate(Task_ADC_to_UART, 
                "ADC_to_UART", 
                configMINIMAL_STACK_SIZE * 2, 
                NULL, 
                2,
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
    volatile adc_data_t adc1 = {0};
    DMA1_Ch1_init(&ADC1->DR, &adc1.voltage_ch14);    

    float temp_C = 0.0F;
    float voltage = 0.0F;

    TickType_t curr_time = xTaskGetTickCount();

    while(1)
    {
        xTaskDelayUntil(&curr_time, pdMS_TO_TICKS(5000));
        
        if (xTimerStop(th_ADC_convert, 0) != pdPASS) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
        temp_C = ((float)adc1.temp_ch16 * V_REF) / 4095.0F;
        temp_C = (TEMP_25 - temp_C) / AVG_SLOPE + 25.0F;
        voltage = ((float)adc1.voltage_ch14 * V_REF) / 4095.0F;
        if (xTimerStart(th_ADC_convert, 0) != pdPASS) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
        
        data = str2Char(temp_C, voltage);
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
        if (xSemaphoreTake(CAN1_semphr, portMAX_DELAY) == pdTRUE)
        {
            CAN_rx_data(CAN1, &can_msg);
            xQueueSend(queue_to_DMA, &can_msg.msg, portMAX_DELAY);
        }
    }
}

void Task_DMA_to_UART(void *pvParameters)
{
    buff_t data = {0};
    DMA1_Ch7_init(&USART2->DR, data.data);

    while(1)
    {
        xQueueReceive(queue_to_DMA, &data, portMAX_DELAY);
        DMA1_Channel7->CNDTR = data.cnt;
        DMA1_Channel7->CCR |= DMA_CCR_EN;
        xSemaphoreTake(DMA_Ch7_semphr, portMAX_DELAY);
    }
}

void tc_ADC_convert(xTimerHandle pxTimer)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;   //Start ADC1
}


