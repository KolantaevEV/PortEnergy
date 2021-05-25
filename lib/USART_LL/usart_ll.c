#include "usart_ll.h"

static circularBuff_t uartRxCircularBuff = {{0}, 0, DATA_BUF_SIZE};
static buff_t buffForCan = {{0}, 0};

extern volatile xSemaphoreHandle UART1_semphr;
extern volatile xQueueHandle queue_to_CAN;

uint8_t *getUartRxBuffAdr(void)
{
    return uartRxCircularBuff.buff;
}

void resetBuffersServiceData(void)
{
    uartRxCircularBuff.beginIndx = 0;
    uartRxCircularBuff.dmaRemainBytes = DATA_BUF_SIZE;
    buffForCan.cnt = 0;
}

void uart2Init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_MODE_OUTPUT_50MHz;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
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
    LL_USART_Init(USART1, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_EnableIT_IDLE(USART1);
    LL_USART_EnableDMAReq_RX(USART1);
    LL_USART_EnableDMAReq_TX(USART1);
    LL_USART_Enable(USART1);
}

buff_t str2Char(float temp, float volt)
{
    static buff_t strData2Tx = {{"Temperatura: xx C | Voltage: y.yy V\x0D\x0A"}, 37};

    strData2Tx.data[13] = ((int)temp / 10) + '0';
    strData2Tx.data[14] = ((int)temp % 10) + '0';

    strData2Tx.data[29] = (int)volt + '0';
    int fractional = (int)(volt * 100) - ((int)volt * 100);
    strData2Tx.data[31] = (fractional / 10) + '0';
    strData2Tx.data[32] = (fractional % 10) + '0';

    return strData2Tx;
}

void USART1_IRQHandler(void)
{
    LL_USART_ClearFlag_IDLE(USART1);

    int numOfData2BeTransfered = DMA1_Channel5->CNDTR;
    int numOfNewBytes = 0;

    if (numOfData2BeTransfered < uartRxCircularBuff.dmaRemainBytes)
    {
        numOfNewBytes = DATA_BUF_SIZE - (numOfData2BeTransfered + uartRxCircularBuff.beginIndx);
    } else {
        numOfNewBytes = (DATA_BUF_SIZE - uartRxCircularBuff.beginIndx) +  (DATA_BUF_SIZE - numOfData2BeTransfered);
    }
    uartRxCircularBuff.dmaRemainBytes = numOfData2BeTransfered;

    int uartBuffIndx = 0;
    for (int indx = 0; indx < numOfNewBytes; indx++)
    {
        uartBuffIndx = indx + uartRxCircularBuff.beginIndx;
        if (uartBuffIndx > (DATA_BUF_SIZE - 1)) uartBuffIndx -= DATA_BUF_SIZE;
        buffForCan.data[indx] = uartRxCircularBuff.buff[uartBuffIndx];
    }
    uartRxCircularBuff.beginIndx = ++uartBuffIndx;

    buffForCan.cnt = numOfNewBytes;

    xQueueSendFromISR(queue_to_CAN, &buffForCan, pdFALSE);
    xSemaphoreGiveFromISR(UART1_semphr, pdFALSE);
}