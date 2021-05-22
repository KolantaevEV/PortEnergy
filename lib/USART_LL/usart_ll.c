#include "usart_ll.h"

typedef struct
{
    char buff[DATA_BUF_SIZE];
    int beginIndx;
    int dmaRemainBytes;
} circularBuff_t;

static circularBuff_t uartRxCircularBuff = {{0}, 0, DATA_BUF_SIZE};
static orderedBuff_t buffForCan = {{0}, 0};

uint8_t newDataFlag = 0;

static void setNewDataFlag(void);

int *getUartRxBuffAdr(void)
{
    return uartRxCircularBuff.buff;
}

orderedBuff_t getDataFromUart(void)
{
    return buffForCan;
}

void setNewDataFlag(void)
{
    newDataFlag = 1;
}

uint8_t getNewDataFlag(void)
{
    return newDataFlag;
}

void clearNewDataFlag(void)
{
    newDataFlag = 0;
}

void resetBuffersServiceData(void)
{
    clearNewDataFlag();
    uartRxCircularBuff.beginIndx = 0;
    uartRxCircularBuff.dmaRemainBytes = DATA_BUF_SIZE;
    buffForCan.amountOfData = 0;

}

void uart2Init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
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

strData_t str2Char(float temp, float volt)
{
    static strData_t strData2Tx = {{"Temperatura: xx C | Voltage: y.yy V\n"}, 36};

    strData2Tx.chars[13] = ((int)temp / 10) + '0';
    strData2Tx.chars[14] = ((int)temp % 10) + '0';

    strData2Tx.chars[29] = (int)volt + '0';
    int fractional = (int)(volt * 100) - ((int)volt * 100);
    strData2Tx.chars[31] = (fractional / 10) + '0';
    strData2Tx.chars[32] = (fractional % 10) + '0';

    return strData2Tx;
}

void USART2_IRQHandler(void)
{
    LL_USART_ClearFlag_IDLE(USART2);

    int numOfData2BeTransfered = DMA1_Channel6->CNDTR;
    int numOfNewBytes = 0;

    if (numOfData2BeTransfered < uartRxCircularBuff.dmaRemainBytes)
    {
        numOfNewBytes = DATA_BUF_SIZE - numOfData2BeTransfered;
    } else {
        numOfNewBytes = (DATA_BUF_SIZE - uartRxCircularBuff.beginIndx) +  (DATA_BUF_SIZE - numOfData2BeTransfered);
    }
    uartRxCircularBuff.dmaRemainBytes = numOfData2BeTransfered;

    int uartBuffIndx = 0;
    for (int indx = 0; indx < numOfNewBytes; indx++)
    {
        uartBuffIndx = indx + uartRxCircularBuff.beginIndx;
        if (uartBuffIndx > (DATA_BUF_SIZE - 1)) uartBuffIndx -= DATA_BUF_SIZE;
        buffForCan.buff[indx] = uartRxCircularBuff.buff[uartBuffIndx];
    }
    uartRxCircularBuff.beginIndx = ++uartBuffIndx;

    buffForCan.amountOfData = numOfNewBytes;

    setNewDataFlag();
}