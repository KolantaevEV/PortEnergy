#ifndef LIB_ADC
#define LIB_ADC

#include <stdint.h>

#include "stm32f1xx.h"

void ADC1_init(void);
static void ADC1_Ch8_init(void);
static void ADC1_Ch16_init(void);

#endif /*LIB_ADC*/