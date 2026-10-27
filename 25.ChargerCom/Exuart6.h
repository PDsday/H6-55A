#ifndef __EXUART6_H
#define __EXUART6_H

#include "string.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
//#include "ExternPrototype.h"

#define	RS485_RDE6 	    	GPIOG	
#define	RS485_RDE6_PIN 		GPIO_Pin_6	
#define RS485_RDE6_RX       GPIO_WriteBit(RS485_RDE6, RS485_RDE6_PIN, (BitAction)1)  
#define RS485_RDE6_TX      	GPIO_WriteBit(RS485_RDE6, RS485_RDE6_PIN, (BitAction)0)   
#define READ_RDE6      		GPIO_ReadInputDataBit(RS485_RDE6, RS485_RDE6_PIN) 

void ExUart6_Configuration(void);
void Stars_Exuart6_Test(void);
void Customer_usart6_task(void);	
void USART6_DMA_Begin_Send(uint8_t *send_buffer , uint16_t nSendCount);
#endif

