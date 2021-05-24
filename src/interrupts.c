#include "interrupts.h"


void IRQ_init(void)
{
    //NVIC_SetPriority();
    //NVIC_EnableIRQ();
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    //NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
    NVIC_EnableIRQ(USART2_IRQn);
}

