#include "interrupts.h"

extern volatile xSemaphoreHandle CAN1_semphr;
extern volatile xSemaphoreHandle DMA_Ch7_semphr;

void IRQ_init(void)
{
    //NVIC_SetPriority();
    //NVIC_EnableIRQ();
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    //NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
    NVIC_SetPriority(USART2_IRQn, 0xA0);
    NVIC_EnableIRQ(USART2_IRQn);

    NVIC_SetPriority(DMA1_Channel7_IRQn, 0xA0);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);

    NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0xA0);
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    CAN1->IER &= ~CAN_IER_FMPIE0;
    xSemaphoreGive(CAN1_semphr);
}

void DMA1_Channel7_IRQHandler(void)
{
    DMA1->IFCR |= DMA_IFCR_CGIF7;
    DMA1_Channel7->CCR &= ~DMA_CCR_EN;
    xSemaphoreGive(DMA_Ch7_semphr);
}