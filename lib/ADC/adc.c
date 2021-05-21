#include "adc.h"

void ADC_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //Enable RCC on ADC
    /*==========ADC_channel_14_PC4=========*/
    GPIOC->CRL &= ~GPIO_CRL_CNF4;   //Analog
    GPIOC->CRL &= ~GPIO_CRL_MODE4;  //Input
    /*==========ADC_config=================*/
    ADC1->SMPR1 |= ADC_SMPR1_SMP14_0 | ADC_SMPR1_SMP14_1; //Sample time
    ADC1->JSQR |= ADC_JSQR_JL_0; //Number of conversions in inlect channels
    ADC1->JSQR |= ADC_JSQR_JSQ3_1 | ADC_JSQR_JSQ3_2 | ADC_JSQR_JSQ3_3; //Set channel to jsq3
    ADC1->JSQR |= ADC_JSQR_JSQ4_4; //Set channel to jsq4
    ADC1->CR2 |= ADC_CR2_ADON;  //ADC on
    while (!())
}