#define DEBUG_WAR_CONSOLE_ON 0
#define DEBUG_ERR_CONSOLE_ON 0
#include "stm32f4xx_conf.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_rcc.h"
#include "can_STM32.h"
#include "CANopen.h"
#include "FD_ObjDict.h"
#include "ExternPrototype.h"

#define CAN_TSR_TME0     ((u32)0x04000000)    /* Transmit mailbox 0 empty */
#define CAN_TSR_TME1     ((u32)0x08000000)    /* Transmit mailbox 1 empty */
#define CAN_TSR_TME2     ((u32)0x10000000)    /* Transmit mailbox 2 empty */
#define TxMessageFIFO_NUM  32

CanTxMsg TxMessageFIFO[TxMessageFIFO_NUM];  //CAN发送FIFO
CanTxMsg *pReadFIFO  = TxMessageFIFO; 		//读FIFO指针
CanTxMsg *pWriteFIFO = TxMessageFIFO; 		//写FIFO指针
vu16 iFIFO = 0;								//CAN发送FIFO栈顶指针，0为没有待发送的TxMessage
volatile unsigned char msg_received = 0;
unsigned char CANopen_LED_ERROR;
unsigned char CANopen_LED_RUN;
u8	ExCAN_RxBuf[7]; 						
u16	ExCANTxOKFlag = 1;
u16	ExCANRxFullFlag;
u16 CanLineOffCnt = 0;
u8 TxMailbox;
u8 CANOpenOffLineFlag = 1;

u8 Tpdo1_Data[8] = 0;
u8 Tpdo2_Data[8] = 0;
u8 Tpdo3_Data[8] = 0;
u8 Tpdo4_Data[8] = 0;
u8 get_tpdo_len[4] = 0;

extern UNS32 pdoNum_X;
extern UNS32 pdoNum0Cnt;
extern UNS32 pdoNum1Cnt;
extern UNS32 pdoNum2Cnt;
extern UNS32 pdoNum3Cnt;

u16 CAN_IAP_FLAG[4];

unsigned short Boot_Version = 0;
//Boot版本固化在Flash中
void Get_BootVersion()
{
	unsigned char i, temp[7] = 0;
	for(i = 0;i<7;i++)
	{
		temp[i] = *(volatile const unsigned char*)(0x08004000+i);
  }
	if(	temp[0] == 'H' 
		&& temp[1]== '6' 
		&& temp[2] == '_' 
		&& temp[3] == 'B' 
		&& temp[4] == 'o' 
		&& temp[5] == 'o'
		&& temp[6] == 't' 
		//&& temp[7] == ':' 
		)
	{
		//ASCI转码
		temp[0] = *(volatile const unsigned char*)(0x08004000+8);
		temp[1] = *(volatile const unsigned char*)(0x08004000+9);
		temp[2] = *(volatile const unsigned char*)(0x08004000+10);
		
		Boot_Version = (temp[0] - '0')*100 + (temp[1] - '0')*10 + (temp[2] - '0');
		
	}

	else
		Boot_Version = 0xFFFF;
	
	ReadPara(ARM_RESERVE_PARA_828) = Boot_Version;	//2026-10-09新增：ARM Boot版本上报（客户读ID828；=0xFFFF 表示未读到有效Boot版本串）
}

void CANopen_Jump_bootloader()
{
    CPU_CRITICAL_ENTER();          // 关总中断
    EraseFlash_IAP(0x0800C000, 1); // 先擦除刷机标志位所在16K扇区
    CAN_IAP_FLAG[0] = 0Xaa55;
    CAN_IAP_FLAG[1] = 0Xaa55;
    CAN_IAP_FLAG[2] = (((unsigned short)ModbusAddr) << 8 | 0x0077);
    // 需要保持用户的DO状态?
    // 硬件设计上只要不掉电，DO状态就能锁存，掉电重新上电后也要保存端口？
    WriteHalfWordToFlash(0x0800C000, CAN_IAP_FLAG, 4); // 再写入刷机标志位
    NVIC_SystemReset();
}

unsigned char canInit(unsigned int bitrate)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	u32 PCLK1 = 36000000;	  //APB时钟PCLK1：36MHz	MAX

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 , ENABLE);
	
	/* Connect CAN pins to AF9 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1); 

	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//CX
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* enabling CAN TX interrupt */
	NVIC_InitStructure.NVIC_IRQChannel=CAN1_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CAN1_TX_PREPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = CAN1_TX_SBUPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* enabling CAN RX interrupt */
	NVIC_InitStructure.NVIC_IRQChannel=CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CAN1_RX_PREPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = CAN1_RX_SBUPRIORITY;//2017-2-7修改 解决通讯闪断问题
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM=DISABLE;
	CAN_InitStructure.CAN_ABOM=ENABLE;   // 自动离线管理
	CAN_InitStructure.CAN_AWUM=DISABLE;
	CAN_InitStructure.CAN_NART=DISABLE;  // 发送重传功能
	CAN_InitStructure.CAN_RFLM=DISABLE;
	CAN_InitStructure.CAN_TXFP=DISABLE;
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal; //CAN_Mode_LoopBack;
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		//-------- 
	CAN_InitStructure.CAN_BS1=CAN_BS1_6tq;		//-------- 
	CAN_InitStructure.CAN_BS2=CAN_BS2_5tq;		//-------- total 12 tq of 1 bit
	//CAN_InitStructure.CAN_Prescaler=6;		//tq freq =	PCLK / CAN_Prescaler
	CAN_InitStructure.CAN_Prescaler=PCLK1/12/1000/bitrate;			// PCLK1= CAN_Prescale * 12 * bitrate * 1000	   (bitrate: ***K bps)
	CAN_Init(CAN1, &CAN_InitStructure);

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=1;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* CAN FIFO0 message pending interrupt enable */ 
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);     // 允许FIFO0消息挂号中断

	if(CANopen_EN==0)	 //------CAN458 Protocol---------------------
	{
	CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);      // 允许发送邮箱空中断
	}
	CANOpenOffLineFlag = 1;//CAN离线标志位
	return 1;
}

u8 CAN_CheckEmptyTXMB(void)  //查询空闲MailBox
{
	u8 TransmitMailbox = CAN_NO_MB;

	/* Select one empty transmit mailbox */
	if ((CAN1->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
	{
	TransmitMailbox = 0;
	}
	else if ((CAN1->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
	{
	TransmitMailbox = 1;
	}
	else if ((CAN1->TSR&CAN_TSR_TME2) == CAN_TSR_TME2)
	{
	TransmitMailbox = 2;
	}
	else
	{
	TransmitMailbox = CAN_NO_MB;
	}

	return TransmitMailbox;
}

void CAN_TXFrame(u8* ExCAN_TxBuf)		 //CAN485发送
{ 
	CanTxMsg TxMessage;
	u8 TransmitMailbox;
	u8 i;
	u8* pExCAN_TxBuf = ExCAN_TxBuf;

	TxMessage.RTR   = CAN_RTR_DATA;
	TxMessage.IDE   = CAN_ID_STD;
	TxMessage.DLC   = 7;
	TxMessage.StdId = CommAddr; //

	for (i=0;i<7;i++)
	{
	TxMessage.Data[i] = pExCAN_TxBuf[i]; // ExCAN_TxBuf[i];  
	}
	ExCANTxOKFlag=0;
	TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
	TxMailbox=TransmitMailbox;
}

unsigned char canSend(CAN_PORT notused, Message *m)
/******************************************************************************
The driver send a CAN message passed from the CANopen stack
INPUT	CAN_PORT is not used (only 1 avaiable)
	Message *m pointer to message to send
OUTPUT	1 if  hardware -> CAN frame
******************************************************************************/
{
	CanTxMsg TxMessage; 
	u8 TransmitMailbox;
	u8 j,i;
	u16 k;  

	TxMessage.IDE = 0;
	TxMessage.StdId = m->cob_id;
	//  TxMessage.RTR = m->rtr;
	if(m->rtr==1)								 //CAN_RTR_REMOTE= ((u32)0x00000002)  /* Remote frame */
	{
		TxMessage.RTR = CAN_RTR_REMOTE;
	}
	else
	{
		TxMessage.RTR = CAN_RTR_DATA;
	}

	TxMessage.DLC = m->len;

	if ((m->len) >= 1) //发送数据
	{
	for(j=0; j<(m->len); j++)
	{
		TxMessage.Data[j] = m->data[j]; 
	}
	}

	if ((CAN_CheckEmptyTXMB() != CAN_NO_MB) && (pWriteFIFO == pReadFIFO))  //有空邮箱 且 无排队时直接发
	{
		TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);		//Send	CAN
		TransmitMailbox=TransmitMailbox;
	}
	else  // 写入FIFO等待发送
	{
		CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);      // 允许发送邮箱空中断

		/* 写FIFO处理 ***********************************************/
		pWriteFIFO[0] = TxMessage;     // 写入FIFO
		if (pWriteFIFO == (&TxMessageFIFO[TxMessageFIFO_NUM-1]))
		{
			pWriteFIFO = TxMessageFIFO;
		}
		else
		{
			pWriteFIFO++;
		}
		if (pWriteFIFO == pReadFIFO)
		{
			if (pReadFIFO == (&TxMessageFIFO[TxMessageFIFO_NUM-1]))
			{
				pReadFIFO = TxMessageFIFO;
			}
			else
			{
				pReadFIFO++;
			}
		}
	}

    i = 0;
	k=0;

	return 1;	 // succesful
}

unsigned char canSend1(CAN_PORT notused, Message *m)
{
	CanTxMsg TxMessage; 
	u8 TransmitMailbox;
	u8 j,i;
	u16 k;  

	TxMessage.IDE = 0;
	TxMessage.StdId = m->cob_id;
	//  TxMessage.RTR = m->rtr;
	if(m->rtr==1)								 //CAN_RTR_REMOTE= ((u32)0x00000002)  /* Remote frame */
	{
		TxMessage.RTR = CAN_RTR_REMOTE;
	}
	else
	{
		TxMessage.RTR = CAN_RTR_DATA;
	}

	TxMessage.DLC = m->len;

	if ((m->len) >= 1) //发送数据
	{
		switch (pdoNum_X)
		{
			case 0: 			
			{
				get_tpdo_len[0] = m->len;
				for(j=0; j<(m->len); j++)
				{
					TxMessage.Data[j] = Tpdo1_Data[j]; 
				}            
				break;
			}

			case 1: 			
			{
				get_tpdo_len[1] = m->len;
				for(j=0; j<(m->len); j++)
				{
					TxMessage.Data[j] = Tpdo2_Data[j]; 
				}            
				break;
			}
			
			case 2: 			
			{
				get_tpdo_len[2] = m->len;
				for(j=0; j<(m->len); j++)
				{
					TxMessage.Data[j] = Tpdo3_Data[j]; 
				}            
				break;
			}

			case 3: 			
			{
				get_tpdo_len[3] = m->len;
				for(j=0; j<(m->len); j++)
				{
					TxMessage.Data[j] = Tpdo4_Data[j]; 
				}            
				break;
			}
			
			default:
			{
				break;
			}
		}
	}

	if ((CAN_CheckEmptyTXMB() != CAN_NO_MB) && (pWriteFIFO == pReadFIFO))  //有空邮箱 且 无排队时直接发
	{
		TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);		//Send	CAN
		TransmitMailbox=TransmitMailbox;
	}
	else  // 写入FIFO等待发送
	{
		CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);      // 允许发送邮箱空中断

		/* 写FIFO处理 ***********************************************/
		pWriteFIFO[0] = TxMessage;     // 写入FIFO
		if (pWriteFIFO == (&TxMessageFIFO[TxMessageFIFO_NUM-1]))
		{
			pWriteFIFO = TxMessageFIFO;
		}
		else
		{
			pWriteFIFO++;
		}
		if (pWriteFIFO == pReadFIFO)
		{
			if (pReadFIFO == (&TxMessageFIFO[TxMessageFIFO_NUM-1]))
			{
				pReadFIFO = TxMessageFIFO;
			}
			else
			{
				pReadFIFO++;
			}
		}
	}

	i = 0;
	k=0;

	return 1;	 // succesful
}

unsigned char canReceive(Message *m)
/******************************************************************************
The driver pass a received CAN message to the stack
INPUT	Message *m pointer to received CAN message
OUTPUT	1 if a message received
******************************************************************************/
{
	CanRxMsg RxMessage; 
	u8 j;  

	RxMessage.StdId=0x00;
	RxMessage.ExtId=0x00;
	RxMessage.IDE=0;
	RxMessage.RTR=0;
	RxMessage.DLC=0;
	RxMessage.FMI=0;
	RxMessage.Data[0]=0x00;
	RxMessage.Data[1]=0x00;
	RxMessage.Data[2]=0x00;
	RxMessage.Data[3]=0x00;
	RxMessage.Data[4]=0x00;
	RxMessage.Data[5]=0x00;
	RxMessage.Data[6]=0x00;
	RxMessage.Data[7]=0x00;

	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

	m->cob_id = RxMessage.StdId;
	//m->rtr =  RxMessage.RTR;
	if(RxMessage.RTR == CAN_RTR_REMOTE)		 //CAN_RTR_REMOTE= ((u32)0x00000002)  /* Remote frame */
	{
		m->rtr =  1;
	}
	else
	{
	m->rtr =  0;
	}

	m->len = RxMessage.DLC;

	if (RxMessage.DLC >= 1) //接收数据
	{
		for(j=0; j<RxMessage.DLC; j++)
		{
		m->data[j] = RxMessage.Data[j]; 
		}
	}

	return 1;	   // succesful
}

unsigned char canChangeBaudRate( CAN_HANDLE fd, char* baud)
{
	unsigned int bitrate;
	u8 string[]="     ";
	u8 *ptr;

	ptr= string;

	do
	{
		*ptr++ = *baud++;
	}while (*baud != 0);

	if(string =="1M")        bitrate=1000;
	else if(string =="800K") bitrate=800;
	else if(string =="500K") bitrate=500;
	else if(string =="250K") bitrate=250;
	else if(string =="125K") bitrate=125;
	else if(string =="100K") bitrate=100;
	else if(string =="50K") bitrate=50;
	else if(string =="20K") bitrate=20;
	else if(string =="10K") bitrate=10;
	else bitrate=500;

	canInit(bitrate);	  //init CAN with baud

	return 0;
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_TX_IRQHandler(void)
{
    u8 TransmitMailbox;

	if(CANopen_EN==0)
	{
		if((CAN_TransmitStatus(CAN1, TxMailbox) == CANTXOK))
		{
		 ExCANTxOKFlag = 1;
		}
        CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP0);
        CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP1);
        CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP2);
	}
	else				//CANopen
	{
		if(CAN_CheckEmptyTXMB() != CAN_NO_MB)		//发送邮箱空 中断  且有待发送的TxMessage
		{
			/* 发送FIFO中数据 ********************************************************/
			if (pReadFIFO != pWriteFIFO)    // 从FIFO中读出数据发送给DSP
			{   			
				TransmitMailbox = CAN_Transmit(CAN1, pReadFIFO); // 发送数据
				//TransmitMailbox = TransmitMailbox;
				if (pReadFIFO == (&TxMessageFIFO[TxMessageFIFO_NUM-1]))
				{
					pReadFIFO = TxMessageFIFO;
				}
				else
				{
					pReadFIFO++;
				}

				if (TransmitMailbox == 0)
				{
					CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP0);
				}
				else if (TransmitMailbox == 1)
				{
					CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP1);
				}
				else if (TransmitMailbox == 2)
				{
					CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP2);
				}
			}
			else
			{
				CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);      // 允许发送邮箱空中断
				CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP0);
				CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP1);
				CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP2);
			}
		}
		else
		{
		   CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP0);
		   CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP1);
		   CAN_ClearITPendingBit(CAN1, CAN_IT_RQCP2);
		}
	}
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_RX0_IRQHandler(void)
{ 
    Message m;
    u8 j;

	//CanLineOffCnt = 0;
	
   if (canReceive(&m))			// a message reveived
   {
        if ((m.len == 7) && (m.cob_id == 0) && ((m.data[0] == CommAddr) || (m.data[0] == 201) || (m.data[0] == 0xFF)))
        {
            for(j=0; j < 7; j++)
            {
                ExCAN_RxBuf[j] = m.data[j]; 
            }

            ExCANRxFullFlag = 1;
            /*if(ExCAN_RxBuf[0] == CommAddr)		//通讯地址判断
            {
                ExCANRxFullFlag = 1;
            }else
            {
                ExCANRxFullFlag = 0;
            }*/
        }
        else
        {
            ExCANRxFullFlag = 0;
            canDispatch(&FD_ObjDict_Data, &m);         // 收到CANopen数据 process it
        }
    }
}