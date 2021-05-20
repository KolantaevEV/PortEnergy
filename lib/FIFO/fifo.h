#ifndef LIB_FIFO
#define LIB_FIFO

#include <stdint.h>

typedef struct {
    uint8_t *head;
    uint8_t *tail;
    uint8_t *end;
    uint16_t cnt;
    uint16_t size;
} fifo;

void fifo_init(fifo *fifo_buff, void *buff, uint16_t buff_size);
uint8_t fifo_write(fifo *fifo_buff, uint8_t data);
uint8_t fifo_read(fifo *fifo_buff);

#endif  /*LIB_FIFO*/