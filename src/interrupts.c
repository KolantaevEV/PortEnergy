#include "interrupts.h"

extern volatile xSemaphoreHandle CAN1_semphr;
extern volatile xSemaphoreHandle DMA_Ch4_semphr;

void IRQ_init(void)
{
    NVIC_SetPriority(USART1_IRQn, 0xF0);
    NVIC_EnableIRQ(USART1_IRQn);

    NVIC_SetPriority(DMA1_Channel4_IRQn, 0xF0);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0xF0);
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    CAN1->IER &= ~CAN_IER_FMPIE0;
    xSemaphoreGiveFromISR(CAN1_semphr, pdFALSE);
}

void DMA1_Channel4_IRQHandler(void)
{
    DMA1->IFCR |= DMA_IFCR_CGIF4;
    DMA1_Channel4->CCR &= ~DMA_CCR_EN;
    xSemaphoreGiveFromISR(DMA_Ch4_semphr, pdFALSE);
}