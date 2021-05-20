#include "tim.h"

void TIM2_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 7200 - 1;
    TIM2->ARR = 1000 - 1;
    TIM2->DIER |= TIM_DIER_UIE; //enable interrupt
    TIM2->CR1 |= TIM_CR1_CEN; //Enable tim2
}

void TIM3_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 7200 - 1;
    TIM3->ARR = 1000 - 1;
    TIM3->DIER |= TIM_DIER_UIE; //enable interrupt
    TIM3->CR1 |= TIM_CR1_CEN; //Enable tim2
}

//=================Other_Func==================

void timer_restart(TIM_TypeDef *TIMx, uint16_t *timer_TIMx)
{
        TIMx->CNT = 0;
        *timer_TIMx = 0;
}