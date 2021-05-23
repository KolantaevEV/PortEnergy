#include "usart_ll.h"


void uart2Init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.BaudRate = 57600;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    LL_USART_Init(USART2, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(USART2);
    LL_USART_EnableIT_IDLE(USART2);
    LL_USART_EnableDMAReq_RX(USART2);
    LL_USART_EnableDMAReq_TX(USART2);
    LL_USART_Enable(USART2);
}

strData str2Char(float temp, float volt)
{
    static strData strData2Tx = {"Temperatura: xx C | Voltage: y.yy V\n", 36};

    strData2Tx.chars[13] = ((int)temp / 10) + '0';
    strData2Tx.chars[14] = ((int)temp % 10) + '0';

    strData2Tx.chars[29] = (int)volt + '0';
    int fractional = (int)(volt * 100) - ((int)volt * 100);
    strData2Tx.chars[29] = (fractional / 10) + '0';
    strData2Tx.chars[29] = (fractional % 10) + '0';

    return strData2Tx;
}

void USART2_IRQHandler(void)
{
    LL_USART_ClearFlag_IDLE(USART2);
}