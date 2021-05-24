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

/*============Add_tasks=============*/
    xTaskCreate(Task_ADC_to_UART, ( signed char * ) "ADC_to_UART", configMINIMAL_STACK_SIZE, NULL, 2,
                        ( xTaskHandle * ) NULL);

    while (1)
    {

    }

    return 0;
}

void Task_ADC_to_UART(void *pvParameters)
{
    volatile adc_data adc1 = {0};
    DMA1_Ch1_init(&ADC1->DR, &adc1.voltage_ch14);
    DMA1_Channel1->CCR |= DMA_CCR_EN;  //Enable DMA

    TickType_t curr_time;
    curr_time = xTaskGetTickCount();

    const TickType_t wait_time = 5000;    //in freertos Ticks(5sec in this proj)
    BaseType_t delay_err = 0;

    while(1)
    {
        delay_err = xTaskDelayUntil(&curr_time, wait_time);
        while (DMA1_Channel7->CCR & DMA_CCR_EN);
        DMA1_Channel1->CCR &= ~DMA_CCR_EN;  //Disable DMA
        while (DMA1_Channel1->CCR & DMA_CCR_EN); //Check disable

        if (!delay_err)
        {
            float temp_C = (TEMP_25 - (float)adc1.temp_ch16) / AVG_SLOPE + 25.0F;
            float voltage = ((float)adc1.voltage_ch14 * 3.3F)/4095.0F;
            msg_to_uart.msg = str2Char(temp_C, voltage);
            DMA1_Channel7->CNDTR = msg_to_uart.msg.cnt;
            DMA1_Channel7->CCR |= DMA_CCR_EN;
        }
        else while(1);  //Maximization of error to debug

        DMA1_Channel1->CCR |= DMA_CCR_EN; //Enable DMA
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
        if (!(DMA1_Channel7->CCR & DMA_CCR_EN))
        {            
            CAN_rx_data(CAN1, &msg_to_uart);
            DMA1_Channel7->CNDTR = msg_to_uart.msg.cnt;
            DMA1_Channel7->CCR |= DMA_CCR_EN;
        }
        else ;//wait semaphore
    }
}

void ADC_convertion(void)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;   //Start ADC1
}