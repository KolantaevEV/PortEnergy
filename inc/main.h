#ifndef INC_MAIN
#define INC_MAIN

/*=========C_std_lib==========*/
#include <stdint.h>
/*=========CMSIS===========*/
#include "stm32f1xx.h"
/*=========HAL_LL===========*/

/*=========FreeRTOS===========*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/*========User_Libs==========*/
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "gpio.h"
#include "rcc.h"
#include "tim.h"
#include "usart_ll.h"
/*========Other=============*/
#include "typedefs.h"
#include "interrupts.h"

void Task_ADC_to_UART(void *pvParameters);
void Task_UART_to_CAN(void *pvParameters);
void Task_CAN_to_UART(void *pvParameters);
void Task_ADC_convertion(void *pvParameters);

#endif  /*INC_MAIN*/