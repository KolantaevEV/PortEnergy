#ifndef LIB_CAN
#define LIB_CAN

#define CAN_DATA_SIZE 8 //Max 8 byte
#define CAN_ID_0 0xABU
#define CAN_ID_NONE 0x0U

#include <stdint.h>

#include "stm32f1xx.h"
#include "typedefs.h"

void CAN1_init(void);
void CAN_tx_data(CAN_TypeDef *CANx, volatile can_msg_t *can_msg);
void CAN_rx_data(CAN_TypeDef *CANx, volatile can_msg_t *can_msg);

#endif /*LIB_CAN*/