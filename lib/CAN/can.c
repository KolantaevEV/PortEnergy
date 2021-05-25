#include "can.h"

void CAN1_init(void)
{
/*---------------RCC---------------*/
    if (!(RCC->APB1ENR & RCC_APB1ENR_CAN1EN)) //Enable RCC on CAN1
        RCC->APB1ENR |= RCC_APB1ENR_CAN1EN; 
    if (!(RCC->APB2ENR & RCC_APB2ENR_IOPAEN)) //Enable RCC on CAN1 GPIO
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    if (!(RCC->APB2ENR & RCC_APB2ENR_AFIOEN)) //Enable RCC on GPIO AF
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    AFIO->MAPR |= AFIO_MAPR_CAN_REMAP_1; //to PB8 PB9
/*----------------TX---------------*/
    GPIOB->CRH &= ~GPIO_CRH_CNF9;   //reset CNF
    GPIOB->CRH |= GPIO_CRH_CNF9_1;  //Pin to alt. func. push-pull
    GPIOB->CRH |= GPIO_CRH_MODE9;   //Pin to output max speed (50MHz)
/*----------------RX---------------*/
    GPIOB->CRH &= ~GPIO_CRH_CNF8;   //reset CNF
    GPIOB->CRH |= GPIO_CRH_CNF8_1;  //Pin to input with pull_up
    GPIOB->CRH &= ~GPIO_CRH_MODE8;   //Pin to input
    GPIOB->BSRR = GPIO_BSRR_BS8;   //Set pull_up
/*----------------CAN1_config---------------*/
    CAN1->MCR |= CAN_MCR_INRQ;  //Enter init mode
        while(!(CAN1->MSR & CAN_MSR_INAK)); //Check init mode
    CAN1->MCR &= ~CAN_MCR_TXFP; //Priority by chronologically
    CAN1->MCR &= ~CAN_MCR_RFLM; //FIFO overrun mode
    CAN1->MCR &= ~CAN_MCR_NART; //Message repeat transfer
    CAN1->MCR |= CAN_MCR_AWUM; //Auto wakeup on RX
    CAN1->MCR &= ~CAN_MCR_ABOM; //Bus-Off exit on soft request
    CAN1->MCR &= ~CAN_MCR_TTCM; //Save or not time stamp
    CAN1->MCR |= CAN_MCR_DBF; //Freeze CAN on debug
    CAN1->BTR = 0x001c001f; //TS1, TS2, BRP - calculated from web (62,5kbps, 32MHz clk)
    CAN1->BTR &= ~CAN_BTR_SILM; //Silent mode
    CAN1->BTR |= CAN_BTR_LBKM; //Loopback mode
    CAN1->BTR &= ~CAN_BTR_SJW; // Resync jump (+1)
    //---------Filters_setup----------
    CAN1->FMR |= CAN_FMR_FINIT; //Enter filter init mode
    CAN1->FM1R |= CAN_FM1R_FBM0; //List or Mask mode filter
    CAN1->FS1R &= ~CAN_FS1R_FSC0; //32 or 16x2 bit scale config
    CAN1->sFilterRegister[0].FR1 = 0UL;
    CAN1->sFilterRegister[0].FR2 = 0UL;
    CAN1->sFilterRegister[0].FR1 |= (CAN_ID_NONE << 5) | (CAN_ID_0 << 21); //Set ID list R1
    CAN1->sFilterRegister[0].FR2 |= (CAN_ID_NONE << 5) | (CAN_ID_NONE << 21); //Set ID list R2
    CAN1->FFA1R &= ~CAN_FFA1R_FFA0; //Assign filter_0 to fifo
    CAN1->FMR &= ~CAN_FMR_FINIT; //Exit filter init mode
    CAN1->FA1R |= CAN_FA1R_FACT0; //Activate filter
    //---------interrupts_setup----------
    CAN1->IER |= CAN_IER_FMPIE0; //IRQ if fifo receive data

    CAN1->MCR &= ~CAN_MCR_INRQ;  //Exit init mode
        while(CAN1->MSR & CAN_MSR_INAK); //Check init mode
}
    
void CAN_tx_data(CAN_TypeDef *CANx, volatile can_msg_t *can_msg)
{
    static uint16_t index = 0;  //saves current index in data array

    if (CAN1->TSR & CAN_TSR_TME0) //check mailbox_0 empty
    {
        CAN1->sTxMailBox[0].TIR = ((can_msg->stid << CAN_TI0R_STID_Pos) | \
                                  (can_msg->exid << CAN_TI0R_EXID_Pos) | \
                                  (can_msg->ide << CAN_TI0R_IDE_Pos) | \
                                  (can_msg->rtr << CAN_TI0R_RTR_Pos));

        if (can_msg->msg.cnt > CAN_DATA_SIZE)  //check if message bigger than 8 byte
        {
            can_msg->dlc = CAN_DATA_SIZE;
            CAN1->sTxMailBox[0].TDTR &= 0xFFFFFFF0;
            CAN1->sTxMailBox[0].TDTR |= (can_msg->dlc << CAN_TDT0R_DLC_Pos);
            
            CAN1->sTxMailBox[0].TDLR = 0UL;
            CAN1->sTxMailBox[0].TDHR = 0UL;
            for (int i = 0; i < (can_msg->dlc / 2); i++)
            {
                CAN1->sTxMailBox[0].TDLR |= can_msg->msg.data[index + i] << (8 * i);
                CAN1->sTxMailBox[0].TDHR |= can_msg->msg.data[index + i + 4] << (8 * i);
            }
            CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ; //Transmit request

            can_msg->msg.cnt -= can_msg->dlc;
            index += can_msg->dlc;
        }
        else if (can_msg->msg.cnt > 0) //check if message lesser than 8 byte
        {
            can_msg->dlc = can_msg->msg.cnt;
            CAN1->sTxMailBox[0].TDTR &= 0xFFFFFFF0;
            CAN1->sTxMailBox[0].TDTR |= (can_msg->dlc << CAN_TDT0R_DLC_Pos);
            
            CAN1->sTxMailBox[0].TDLR = 0UL;
            CAN1->sTxMailBox[0].TDHR = 0UL;
            for (int i = 0; i < can_msg->dlc; i++)
            {
                if (i < 4)
                    CAN1->sTxMailBox[0].TDLR |= can_msg->msg.data[index + i] << (8 * i);
                else
                    CAN1->sTxMailBox[0].TDHR |= can_msg->msg.data[index + i] << (8 * (i - 4));
            }
            CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ; //Transmit request

            can_msg->msg.cnt = 0;
            index = 0;
        }
    }
}

void CAN_rx_data(CAN_TypeDef *CANx, volatile can_msg_t *can_msg)
{
    can_msg->stid = ((CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_STID_Msk) >> CAN_RI0R_STID_Pos);
    can_msg->exid = ((CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_EXID_Msk) >> CAN_RI0R_EXID_Pos);
    can_msg->ide = ((CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_IDE_Msk) >> CAN_RI0R_IDE_Pos);
    can_msg->rtr = ((CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_RTR_Msk) >> CAN_RI0R_RTR_Pos);
    can_msg->dlc = ((CAN1->sFIFOMailBox[0].RDTR & CAN_RDT0R_DLC_Msk) >> CAN_RDT0R_DLC_Pos);
    can_msg->msg.cnt = can_msg->dlc;

    for (int i = 0; i < can_msg->dlc; i++)
    {
        if (i < 4)
            can_msg->msg.data[i] = ((CAN1->sFIFOMailBox[0].RDLR >> (8 * i)) & 0xFFUL);
        else
            can_msg->msg.data[i] = ((CAN1->sFIFOMailBox[0].RDHR >> (8 * (i - 4))) & 0xFFUL);
    }

    CAN1->RF0R |= CAN_RF0R_RFOM0; //Clear current fifo_0
    CAN1->IER |= CAN_IER_FMPIE0;
}