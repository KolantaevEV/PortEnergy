#include "main.h"

volatile message msg_to_uart = {0};
volatile xSemaphoreHandle DMA_Ch7_mutex;
volatile xSemaphoreHandle CAN1_semphr;
volatile xSemaphoreHandle DMA_Ch7_semphr;
volatile xSemaphoreHandle UART1_semphr;

int main(void)
{
    RCC_init();
    GPIO_init();
    uart2Init();
    DMA1_Ch7_init(&USART2->DR, msg_to_uart.msg.data);
    ADC1_init();
    CAN1_init();
    IRQ_init();

//============Add_Timers============

//============Add_Semaphores========
    DMA_Ch7_semphr = xSemaphoreCreateBinary();
        if (DMA_Ch7_semphr == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
    CAN1_semphr = xSemaphoreCreateBinary();
        if (CAN1_semphr == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
    UART1_semphr = xSemaphoreCreateBinary();
        if (UART1_semphr == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
//============Add_Mutex=============
    DMA_Ch7_mutex = xSemaphoreCreateMutex();
        if (DMA_Ch7_mutex == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
//============Add_Tasks=============
    xTaskCreate(Task_ADC_to_UART, 
                "ADC_to_UART", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                2,
                (xTaskHandle *) NULL);
    xTaskCreate(Task_UART_to_CAN, 
                "UART_to_CAN", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                3,
                (xTaskHandle *) NULL);
    xTaskCreate(Task_CAN_to_UART, 
                "CAN_to_UART", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                4,
                (xTaskHandle *) NULL);

    vTaskStartScheduler();

    while (1)
    {
        
    }

    return 0;
}


void Task_ADC_to_UART(void *pvParameters)
{
    volatile adc_data adc1 = {0};
    DMA1_Ch1_init(&ADC1->DR, &adc1.voltage_ch14);

    xTimerHandle th_ADC_convert;
    th_ADC_convert = xTimerCreate("ADC_convert",
                                pdMS_TO_TICKS(50),
                                pdTRUE,
                                NULL,
                                tc_ADC_convert);
    if (th_ADC_convert == NULL) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
    if (xTimerStart(th_ADC_convert, 0) != pdPASS) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}

    TickType_t curr_time = xTaskGetTickCount();

    float temp_C = 0.0F;
    float voltage = 0.0F;

    while(1)
    {
        xTaskDelayUntil(&curr_time, pdMS_TO_TICKS(5000));
        if (xSemaphoreTake(DMA_Ch7_mutex, portMAX_DELAY) == pdTRUE)
        {
            if (xTimerStop(th_ADC_convert, 0) != pdPASS) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
            temp_C = ((float)adc1.temp_ch16 * V_REF) / 4095.0F;
            temp_C = (TEMP_25 - temp_C) / AVG_SLOPE + 25.0F;
            voltage = ((float)adc1.voltage_ch14 * V_REF) / 4095.0F;
            if (xTimerStart(th_ADC_convert, 0) != pdPASS) {GPIOC->BSRR = GPIO_BSRR_BS13; while(1);}
            msg_to_uart.msg = str2Char(temp_C, voltage);
            DMA1_Channel7->CNDTR = msg_to_uart.msg.cnt;
            DMA1_Channel7->CCR |= DMA_CCR_EN;
            xSemaphoreTake(DMA_Ch7_semphr, portMAX_DELAY);
            xSemaphoreGive(DMA_Ch7_mutex);
        }
    }
}

void Task_UART_to_CAN(void *pvParameters)
{
    volatile message can_msg_tx = {0};
    DMA1_Ch6_init(&USART2->DR, can_msg_tx.msg.data);

    can_msg_tx.stid = CAN_ID_0;
    can_msg_tx.exid = 0;
    can_msg_tx.ide = 0;
    can_msg_tx.rtr = 0;

    while(1)
    {
        if (can_msg_tx.msg.cnt != 0)
        {
            CAN_tx_data(CAN1, &can_msg_tx);
        }
        else
        {
            if (!(DMA1_Channel6->CCR & DMA_CCR_EN))
                DMA1_Channel6->CCR |= DMA_CCR_EN;
            xSemaphoreTake(UART1_semphr, portMAX_DELAY);
            can_msg_tx.msg.cnt = DATA_BUF_SIZE - DMA1_Channel6->CNDTR;
            DMA1_Channel6->CNDTR = DATA_BUF_SIZE;
        }
    }
}

void Task_CAN_to_UART(void *pvParameters)
{
    while(1)
    {
        if (xSemaphoreTake(CAN1_semphr, portMAX_DELAY) == pdTRUE)
        {
            CAN_rx_data(CAN1, &msg_to_uart);
            if (xSemaphoreTake(DMA_Ch7_mutex, portMAX_DELAY) == pdTRUE)
            {                
                DMA1_Channel7->CNDTR = msg_to_uart.msg.cnt;
                DMA1_Channel7->CCR |= DMA_CCR_EN;
                xSemaphoreTake(DMA_Ch7_semphr, portMAX_DELAY);
                xSemaphoreGive(DMA_Ch7_mutex);
            }
        }
    }
}

void tc_ADC_convert(xTimerHandle pxTimer)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;   //Start ADC1
}