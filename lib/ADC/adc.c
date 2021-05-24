#include "adc.h"

void ADC1_init(void)
{
    if (!(RCC->APB2ENR & RCC_APB2ENR_ADC1EN))
        RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //Enable RCC on ADC1

    ADC1->CR2 &= ~ADC_CR2_ADON;  //ADC1 disable
/*===============ADC1_config===================*/
    //ADC1->JSQR |= ADC_JSQR_JL; //Number of conversions in inject channels
    ADC1->SQR1 |= ADC_SQR1_L_0; //Number of conversions in regular channels
    ADC1->CR2 |= ADC_CR2_EXTSEL;    //select trigger to start
    ADC1->CR2 |= ADC_CR2_EXTTRIG;   //enable trigger for regular Channels
    ADC1->CR2 &= ~ADC_CR2_CONT;   //Continue Mode
    ADC1->CR1 |= ADC_CR1_SCAN;   //Scan Mode
    ADC1->CR2 &= ~ADC_CR2_ALIGN;  //Right/left alignment
    ADC1->CR2 |= ADC_CR2_DMA;  //DMA enable

/*===============ADC1_Ch_config===================*/
    ADC1_Ch8_init();    //Voltage input
    ADC1_Ch16_init();   //Temp internal input

/*===============ADC1_calibration===================*/
    ADC1->CR2 |= ADC_CR2_ADON;  //ADC enable
    for (uint8_t i = 100; i != 0; i--); //Wait some time
    ADC1->CR2 |= ADC_CR2_RSTCAL;   //Initialize calibration
    while(ADC1->CR2 & ADC_CR2_RSTCAL); //Wait for Initialize end
    ADC1->CR2 |= ADC_CR2_CAL;   //Start calibration
    while(ADC1->CR2 & ADC_CR2_CAL); //Wait for calibration end

//    ADC1->CR2 |= ADC_CR2_SWSTART;   //Start ADC1
}

void ADC1_Ch8_init(void)
{
    if (!(RCC->APB2ENR & RCC_APB2ENR_IOPBEN))
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

/*===============ADC1_Ch8_GPIO_PB0===================*/
    GPIOB->CRL &= ~GPIO_CRL_CNF0;   //Analog
    GPIOB->CRL &= ~GPIO_CRL_MODE0;  //Input

/*===============ADC1_Ch8_config===================*/
    ADC1->SMPR2 |= ADC_SMPR2_SMP8_0 | ADC_SMPR2_SMP8_1; //Sample time
    //ADC1->JSQR |= ADC_JSQR_JSQ; //Set inject channel
    ADC1->SQR1 |= ADC_SQR3_SQ1_3; //Set regular channel
}

void ADC1_Ch16_init(void)
{
/*===============ADC1_Ch16_config===================*/
    ADC1->SMPR1 |= ADC_SMPR1_SMP16; //Sample time
    //ADC1->JSQR |= ADC_JSQR_JSQ; //Set inject channel
    ADC1->SQR1 |= ADC_SQR3_SQ2_4;   //Set regular channel
    ADC1->CR2 |= ADC_CR2_TSVREFE;  //Enable internal temp sensor
}
