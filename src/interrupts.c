#include "interrupts.h"

extern volatile xSemaphoreHandle CAN1_semphr;
extern volatile xSemaphoreHandle DMA_Ch7_semphr;
extern volatile xSemaphoreHandle UART1_semphr;
extern volatile xQueueHandle queue_to_CAN;
extern volatile circularBuff_t uartRxCircularBuff;
extern volatile buff_t buffForCan;

void IRQ_init(void)
{
    NVIC_SetPriority(USART2_IRQn, 0xF0);
    NVIC_EnableIRQ(USART2_IRQn);

    NVIC_SetPriority(DMA1_Channel7_IRQn, 0xF0);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);

    NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0xF0);
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    CAN1->IER &= ~CAN_IER_FMPIE0;
    xSemaphoreGiveFromISR(CAN1_semphr, pdFALSE);
}

void DMA1_Channel7_IRQHandler(void)
{
    DMA1->IFCR |= DMA_IFCR_CGIF7;
    DMA1_Channel7->CCR &= ~DMA_CCR_EN;
    xSemaphoreGiveFromISR(DMA_Ch7_semphr, pdFALSE);
}

void USART2_IRQHandler(void)
{
    LL_USART_ClearFlag_IDLE(USART2);

    //DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    
    int numOfData2BeTransfered = DMA1_Channel6->CNDTR;
    int numOfNewBytes = 0;

    if (numOfData2BeTransfered < uartRxCircularBuff.dmaRemainBytes)
    {
        numOfNewBytes = DATA_BUF_SIZE - numOfData2BeTransfered;
    } else {
        numOfNewBytes = (DATA_BUF_SIZE - uartRxCircularBuff.beginIndx) +  (DATA_BUF_SIZE - numOfData2BeTransfered);
    }
    uartRxCircularBuff.dmaRemainBytes = numOfData2BeTransfered;

    int uartBuffIndx = 0;
    for (int indx = 0; indx < numOfNewBytes; indx++)
    {
        uartBuffIndx = indx + uartRxCircularBuff.beginIndx;
        if (uartBuffIndx > (DATA_BUF_SIZE - 1)) uartBuffIndx -= DATA_BUF_SIZE;
        buffForCan.data[indx] = uartRxCircularBuff.buff[uartBuffIndx];
    }
    uartRxCircularBuff.beginIndx = ++uartBuffIndx;

    buffForCan.cnt = numOfNewBytes;
    
    setNewDataFlag();

    xQueueSendFromISR(queue_to_CAN, getDataFromUart(), pdFALSE);
    xSemaphoreGiveFromISR(UART1_semphr, pdFALSE);

    //DMA1_Channel6->CCR |= DMA_CCR_EN;
}