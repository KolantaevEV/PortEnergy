#ifndef LIB_TIM
#define LIB_TIM

#include "stm32f1xx.h"

void TIM2_Init(void);
void TIM2_switch(uint16_t time, uint8_t state);
void TIM3_Init(void);
void TIM3_switch(uint16_t time, uint8_t state);
void timer_restart(TIM_TypeDef *TIMx, uint16_t *timer_TIMx);

#endif  /*LIB_TIM*/