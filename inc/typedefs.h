#ifndef INC_TYPEDEFS
#define INC_TYPEDEFS

#define DATA_BUF_SIZE 256
#define TEMP_25 1.52F
#define AVG_SLOPE 0.0043F
#define V_REF 3.0F

#include <stdint.h>

typedef struct 
{
    char chars[36];
    int size;
} strData_t;

/*
typedef struct
{
    char buff[DATA_BUF_SIZE];
    int amountOfData;
} orderedBuff_t;
*/

typedef struct 
{
    uint8_t data[DATA_BUF_SIZE];
    int16_t cnt;
} buff_t;

typedef struct 
{
    uint32_t stid;
    uint32_t exid;
    uint32_t ide;
    uint32_t rtr;
    uint32_t dlc;
    buff_t msg;
} can_msg_t;

typedef struct 
{
    uint16_t voltage_ch14;
    uint16_t temp_ch16;
} adc_data_t;

#endif  /*INC_TYPEDEFS*/