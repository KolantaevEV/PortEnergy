#include "main.h"

volatile message msg_to_uart = {0};

int main(void)
{
    RCC_init();
    GPIO_init();
    
    DMA1_Ch7_init(&USART2->DR, &msg_to_uart.msg.data);
    ADC1_init();
    CAN1_init();
    uart2Init();
    IRQ_init();

/*============Add_Tasks=============*/
    xTaskCreate(Task_ADC_to_UART, 
                "ADC_to_UART", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                tskIDLE_PRIORITY + 2,
                (xTaskHandle *) NULL);
    xTaskCreate(Task_UART_to_CAN, 
                "UART_to_CAN", 
                configMINIMAL_STACK_SIZE, 
                NULL, 
                tskIDLE_PRIORITY + 2,
                (xTaskHandle *) NULL);
/*============Add_Timers=============*/    




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
    if (th_ADC_convert == NULL) while(1);
    if (xTimerStart(th_ADC_convert, 0) != pdPASS) while(1);

    BaseType_t delay_err = 0;

    TickType_t curr_time;
    curr_time = xTaskGetTickCount();

    while(1)
    {
        delay_err = xTaskDelayUntil(&curr_time, pdMS_TO_TICKS(5000));
        if (!delay_err)
        {
            //GET MUTEX!!! DMA7
            if (xTimerStop(th_ADC_convert, 0) != pdPASS) while(1);
            float temp_C = (TEMP_25 - (float)adc1.temp_ch16) / AVG_SLOPE + 25.0F;
            float voltage = ((float)adc1.voltage_ch14 * 3.3F)/4095.0F;
            if (xTimerStart(th_ADC_convert, 0) != pdPASS) while(1);
            msg_to_uart.msg = str2Char(temp_C, voltage);
            DMA1_Channel7->CNDTR = msg_to_uart.msg.cnt;
            DMA1_Channel7->CCR |= DMA_CCR_EN;
            //wait semaphore from dma interrupt
            //!!!Release MUTEX DMA7???
        }
        else while(1);  //Error - maximization of error to debug

    }
}

void Task_UART_to_CAN(void *pvParameters)
{
    volatile message can_msg_tx = {0};
    DMA1_Ch6_init(&USART2->DR, &can_msg_tx.msg.data);

    while(1)
    {
        if (can_msg_tx.msg.cnt != 0)
        {
            CAN_tx_data(CAN1, &can_msg_tx);
        }
        else
        {
            DMA1_Channel6->CCR |= DMA_CCR_EN;
            //wait semaphore 1
            can_msg_tx.msg.cnt = DATA_BUF_SIZE - DMA1_Channel6->CNDTR;
        }
    }
}

void Task_CAN_to_UART(void *pvParameters)
{
    while(1)
    {
        //!!!GET MUTEX DMA7
        CAN_rx_data(CAN1, &msg_to_uart);
        DMA1_Channel7->CNDTR = msg_to_uart.msg.cnt;
        DMA1_Channel7->CCR |= DMA_CCR_EN;
        //wait semaphore from dma interrupt
        //!!!Release MUTEX DMA7???
        //wait semaphore
}

void tc_ADC_convert(xTimerHandle pxTimer)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;   //Start ADC1
}