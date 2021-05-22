#ifndef INC_TYPEDEFS
#define INC_TYPEDEFS

#define DATA_BUF_SIZE 256

typedef struct 
{
    char chars[36];
    int size;
} strData_t;

typedef struct
{
    char buff[DATA_BUF_SIZE];
    int amountOfData;
} orderedBuff_t;

#endif  /*INC_TYPEDEFS*/
