#ifndef LIB_USART_LL
#define LIB_USART_LL

#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_gpio.h"
#include "typedefs.h"

void uart2Init(void);
buff str2Char(float temp, float volt);
void USART2_IRQHandler(void);

#endif  /*LIB_USART_LL*/