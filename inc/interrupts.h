#ifndef INC_INTERRUPTS
#define INC_INTERRUPTS

#include "stm32f1xx.h"
#include "typedefs.h"
#include "FreeRTOS.h"
#include "semphr.h"

void IRQ_init(void);
void USB_LP_CAN1_RX0_IRQHandler(void);
void DMA1_Channel7_IRQHandler(void);

#endif  /*INC_INTERRUPTS*/
