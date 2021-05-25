#ifndef LIB_USART_LL
#define LIB_USART_LL

#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_cortex.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "typedefs.h"

typedef struct
{
    uint8_t buff[DATA_BUF_SIZE];
    int32_t beginIndx;
    int32_t dmaRemainBytes;
} circularBuff_t;

void uart2Init(void);
buff_t str2Char(float temp, float volt);
uint8_t *getUartRxBuffAdr(void);
buff_t *getDataFromUart(void);
uint8_t getNewDataFlag(void);
void clearNewDataFlag(void);
void resetBuffersServiceData(void);
void setNewDataFlag(void);

#endif  /*LIB_USART_LL*/
