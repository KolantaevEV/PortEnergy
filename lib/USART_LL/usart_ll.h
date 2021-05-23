#ifndef LIB_USART_LL
#define LIB_USART_LL
#define USE_FULL_LL_DRIVER

#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_gpio.h"
#include "typedefs.h"

void uart2Init(void);
strData str2Char(float temp, float volt);

#endif  /*LIB_USART_LL*/