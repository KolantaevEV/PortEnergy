#ifndef INC_TYPEDEFS
#define INC_TYPEDEFS

<<<<<<< HEAD
#define TEMP_25 1.43F
#define AVG_SLOPE 4.3F
#define V_COEFF 1.0F

#include <stdint.h>
typedef struct 
{
    volatile uint8_t data[256];
    volatile int16_t cnt;
} buff;
=======
#define DATA_BUF_SIZE 256

typedef struct 
{
    char chars[36];
    int size;
} strData;
>>>>>>> origin/Roma_dev

typedef struct 
{
    uint32_t stid;
    uint32_t exid;
    uint32_t ide;
    uint32_t rtr;
    uint32_t dlc;
    buff msg;
} can_message;

typedef struct 
{
    uint16_t voltage_ch14;
    uint16_t temp_ch16;
} adc_data;

#endif  /*INC_TYPEDEFS*/