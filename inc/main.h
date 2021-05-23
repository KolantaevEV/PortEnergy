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

#endif  /*INC_MAIN*/