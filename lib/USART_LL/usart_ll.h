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

void uart2Init(void);
buff str2Char(float temp, float volt);
char *getUartRxBuffAdr(void);
buff getDataFromUart(void);
uint8_t getNewDataFlag(void);
void clearNewDataFlag(void);
void resetBuffersServiceData(void);

#endif  /*LIB_USART_LL*/
