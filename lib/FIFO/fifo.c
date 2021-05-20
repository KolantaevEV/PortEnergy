#include "fifo.h"

void fifo_init(fifo *fifo_buff, void *buff, uint16_t buff_size)
{
    fifo_buff->head = (uint8_t *)buff;
    fifo_buff->tail = (uint8_t *)buff;
    fifo_buff->size = buff_size;
    fifo_buff->end = (uint8_t *)buff + fifo_buff->size;
    fifo_buff->cnt = 0;
}

uint8_t fifo_write(fifo *fifo_buff, uint8_t data)
{
    if (fifo_buff->tail != fifo_buff->head || fifo_buff->cnt == 0)
    {
        *fifo_buff->tail = data;
        fifo_buff->tail++;
        fifo_buff->cnt++;
        if (fifo_buff->tail == fifo_buff->end) fifo_buff->tail -= fifo_buff->size;
    }
    else return 1;

    return 0;
}

uint8_t fifo_read(fifo *fifo_buff)
{
    uint8_t byte = 0;

    if (fifo_buff->cnt > 0)
    {
        byte = *fifo_buff->head;
        fifo_buff->head++;
        fifo_buff->cnt--;
        if (fifo_buff->head == fifo_buff->end) fifo_buff->head -= fifo_buff->size;
    }

    return byte;
}
