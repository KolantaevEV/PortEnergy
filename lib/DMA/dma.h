#ifndef LIB_DMA
#define LIB_DMA

#include "stm32f1xx.h"
#include "string.h"

void DMA1_Ch1_init(volatile void *addr_perif, volatile void *addr_buff);
//void DMA1_Ch2_init(volatile void *addr_perif, volatile void *addr_buff);
//void DMA1_Ch3_init(volatile void *addr_perif, volatile void *addr_buff);
void DMA1_Ch4_init(volatile void *addr_perif, volatile void *addr_buff);
void DMA1_Ch5_init(volatile void *addr_perif, volatile void *addr_buff);
void DMA1_Ch6_init(volatile void *addr_perif, volatile void *addr_buff);
void DMA1_Ch7_init(volatile void *addr_perif, volatile void *addr_buff);

#endif  /*LIB_DMA*/