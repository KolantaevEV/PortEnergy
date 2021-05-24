#include "gpio.h"

void GPIO_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    //------------PC13 to output-------------
    GPIOC->CRH &= ~(GPIO_CRH_CNF | GPIO_CRH_MODE); //reset CRH
    GPIOC->CRH &= ~GPIO_CRH_CNF13;  // PC13 to push-pull
    GPIOC->CRH |= GPIO_CRH_MODE13;  // PC13 to output max speed (50MHz)
    GPIOC->BSRR = GPIO_BSRR_BR13;   // SET bit
}