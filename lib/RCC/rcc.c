#include "rcc.h"

void RCC_init(void)
{
//----------Config for max clocks without USB and ADC----------------------
    FLASH->ACR |= FLASH_ACR_LATENCY_1;  //FLASH latency = 2 (because 48 - 72MHz)

    RCC->CR |= RCC_CR_HSEON;    //HSE ON
    while (!(RCC->CR & RCC_CR_HSERDY)); //HSE is READY?

    RCC->CFGR |= RCC_CFGR_PLLMULL9; //PLL prescaler
    RCC->CFGR |= RCC_CFGR_PLLSRC;   //PLL sourse = HSE
    RCC->CR |= RCC_CR_PLLON;    //PLL enable
    while (!(RCC->CR & RCC_CR_PLLRDY)); //PLL is READY?

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1; //AHB divider
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; //APB1 divider
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; //APB2 divider
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6; //ADC Div 6 (12 MHz)

    RCC->CFGR |= RCC_CFGR_SW_PLL; // Switch to PLL
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)); //is switched to PLL?

//    RCC->CR &= ~RCC_CR_HSION; //HSI OFF if needed....(for energy saving)
}
