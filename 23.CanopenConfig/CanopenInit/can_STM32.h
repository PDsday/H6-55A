#ifndef __can_STM32__
#define __can_STM32__

#include "config.h"
#include "stm32f4xx_conf.h"
#include "can.h"

extern u16 CANopen_EN;
extern u8	ExCAN_RxBuf[7]; 
extern u16	ExCANTxOKFlag;
extern u16	ExCANRxFullFlag;
extern u16	CommAddr;
extern u16	CANAddr;

void CAN_TXFrame(u8* ExCAN_TxBuf);	//CAN485·¢ËÍ

unsigned char canInit(unsigned int bitrate);
unsigned char canSend(CAN_PORT notused, Message *m);
unsigned char canReceive(Message *m);
unsigned char canChangeBaudRate( CAN_HANDLE fd, char* baud);

#endif
