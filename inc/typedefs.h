#ifndef INC_TYPEDEFS
#define INC_TYPEDEFS

#define TEMP_25 1.43F
#define AVG_SLOPE 4.3F
#define V_COEFF 1.0F
#define DATA_BUF_SIZE 256

#include <stdint.h>
typedef struct 
{
    volatile uint8_t data[256];
    volatile int16_t cnt;
} buff;
typedef struct 
{
    uint32_t stid;
    uint32_t exid;
    uint32_t ide;
    uint32_t rtr;
    uint32_t dlc;
    buff msg;
} message;

typedef struct 
{
    uint16_t voltage_ch14;
    uint16_t temp_ch16;
} adc_data;

#endif  /*INC_TYPEDEFS*/