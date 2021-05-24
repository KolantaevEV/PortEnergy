#ifndef LIB_USART_LL
#define LIB_USART_LL
#define USE_FULL_LL_DRIVER

#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_cortex.h"
#include "typedefs.h"

void uart2Init(void);
strData_t str2Char(float temp, float volt);
int *getUartRxBuffAdr(void);
orderedBuff_t getDataFromUart(void);
uint8_t getNewDataFlag(void);
void clearNewDataFlag(void);
void resetBuffersServiceData(void);

#endif  /*LIB_USART_LL*/
