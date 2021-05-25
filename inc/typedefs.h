#ifndef INC_TYPEDEFS
#define INC_TYPEDEFS

#define DATA_BUF_SIZE 256
#define TEMP_25 1.52F
#define AVG_SLOPE 0.0043F
#define V_REF 3.0F

#include <stdint.h>

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

#endif  /*INC_TYPEDEFS*/