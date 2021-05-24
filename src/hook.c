#include "hook.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName) {
  taskDISABLE_INTERRUPTS(); // game over
  while(1); //Error
}