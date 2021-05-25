#include "usart_ll.h"
typedef struct
{
    char buff[DATA_BUF_SIZE];
    int beginIndx;
    int dmaRemainBytes;
} circularBuff_t;

static circularBuff_t uartRxCircularBuff = {{0}, 0, DATA_BUF_SIZE};
static buff_t buffForCan = {{0}, 0};

uint8_t newDataFlag = 0;

//static void setNewDataFlag(void);

char *getUartRxBuffAdr(void)
{
    return uartRxCircularBuff.buff;
}

buff_t getDataFromUart(void)
{
    return buffForCan;
}
/*
void setNewDataFlag(void)
{
    newDataFlag = 1;
}
*/
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
    buffForCan.cnt = 0;

}

void uart2Init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_MODE_OUTPUT_50MHz;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
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
    LL_USART_Init(USART2, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(USART2);
    LL_USART_EnableIT_IDLE(USART2);
    LL_USART_EnableDMAReq_RX(USART2);
    LL_USART_EnableDMAReq_TX(USART2);
    LL_USART_Enable(USART2);
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

