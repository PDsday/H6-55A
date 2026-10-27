#include "Exuart6.h"

#define H6_USART6_TIMER 1				//软件定时器驱动放入500ms硬件中断，超时判断时间500
#define H6_USART6_RX_MAXLENTH 122
#define H6_USART6_TX_MAXLENTH 46
uint8_t Exuart6_RX_buffer[H6_USART6_RX_MAXLENTH] = {0};
uint8_t Exuart6_TX_buffer[H6_USART6_TX_MAXLENTH] = {0};

#define __weak   __attribute__((weak))

__weak void customer_usart6_irqhandle(void)
{
	return ;
}	//弱定义,弱定义函数不可使用while来阻塞


void usart6_soft_timer_driver(void);//函数声明

uint8_t USART6_RW = 0;		//读/写判断
uint8_t USART6_RWAB =1;  //读A/B板判断 
typedef struct 
{
	uint8_t recv_over_timer;	//超时标志，1 = 接收超时
	uint8_t recv_finish;		//接收完成标志
	uint8_t crc_error;				//crc_error校验结果标志,置1标志crc_error校验未通过
	uint8_t write_error;
	uint8_t recv_lenth;			//实际接收数据长度
	uint8_t expect_recv_lenth;	//期望接收数据长度
	uint8_t *Rx_buffer;			//接收数据缓冲区指针

}_Customer_Usart6;

_Customer_Usart6 customer_usart6; 

void Customer_Init(void)
{
	customer_usart6.recv_over_timer = 0;
	customer_usart6.expect_recv_lenth = 0;
	customer_usart6.crc_error = 0;
	customer_usart6.recv_lenth = 0;
	customer_usart6.recv_finish = 0;
	customer_usart6.Rx_buffer =  Exuart6_RX_buffer;
}

void Customer_usart6_Send(uint8_t *send_buffer , uint8_t nSendCount, uint8_t nExpected_lenth)
{
	customer_usart6.expect_recv_lenth = nExpected_lenth;
	customer_usart6.recv_finish = 0;
	customer_usart6.recv_lenth = 0;
	customer_usart6.crc_error = 0;
	customer_usart6.recv_over_timer = 0;
	
	USART6_DMA_Begin_Send(send_buffer , nSendCount);
}	




void Customer_usart6_task()	//将此函数放入TimeBase.c的100ms任务处理中
{
	static uint8_t time_base = 0;
	if(time_base == 8)				//每800ms进行一次下询
	{
		if(USART6_RW==0)
		{
			if (USART6_RWAB==1)
			{
			uint8_t Send_Cmd[] = {0xaa,0x00,0x01,0xff,0xff,0xcc};
			Customer_usart6_Send(Send_Cmd , 6, H6_USART6_RX_MAXLENTH);					//由发送完成中断启动超时定时器
		  USART6_RWAB=2;
		  }
			else if (USART6_RWAB==2)
			{
			uint8_t Send_Cmd[] = {0xaa,0x01,0x01,0xff,0xff,0xcc};
			Customer_usart6_Send(Send_Cmd , 6, H6_USART6_RX_MAXLENTH);					//由发送完成中断启动超时定时器
			USART6_RWAB=1;
      }
		}
		else
		{
			Customer_usart6_Send(Exuart6_TX_buffer , 46, 46);
			customer_usart6.write_error = 0;
			//USART6_RW = 0;
		}
		time_base = 0;
	}
	else if(time_base == 5 && customer_usart6.recv_over_timer==0 && customer_usart6.recv_finish==0 )			//每500ms做一次接收超时判断
	{
		usart6_soft_timer_driver();
	}
	else if(time_base == 6)
	{
		if(customer_usart6.recv_finish==1 && customer_usart6.crc_error ==0 && customer_usart6.recv_over_timer ==0)	//每600ms处理接收的数据
		{
			//从customer_usart6.Rx_buffer中获取数据
			//数据处理........
			customer_usart6_irqhandle();
		}
	}
	time_base++;
}


uint16_t _crc_error(unsigned char *pDataIn, unsigned char iLenIn)
{
     uint16_t wcrc_error = 0xffff;
     unsigned char i, j;
     for (i = 0; i < iLenIn; i++)
     {
         wcrc_error ^= pDataIn[i];
         for (j = 0; j < 8; j++)
         {
             if (wcrc_error & 0x0001)
             {
                 wcrc_error >>= 1;
                 wcrc_error ^= 0xa001;
             }
             else
             {
                 wcrc_error >>= 1;
             }
         }
     }
     return wcrc_error;
 }

uint8_t crc_error_check()
{
    uint8_t crc_errorbit_H = customer_usart6.recv_lenth -3;
    uint8_t crc_errorbit_L = customer_usart6.recv_lenth -2;

//    unsigned char *pDataIn = &(customer_usart6.Rx_buffer[0]);
//    unsigned char iLenIn = customer_usart6.recv_lenth-7;

	//CRC校验修改为Buffer[0] --- Buffer[Len-3]
    uint16_t temp_crc_error = _crc_error(customer_usart6.Rx_buffer,customer_usart6.recv_lenth-3);
	
	
    if(temp_crc_error == (customer_usart6.Rx_buffer[crc_errorbit_H] * 256 + customer_usart6.Rx_buffer[crc_errorbit_L]) )
    {
        return 0;
    }
    return 1;
}





//测试程序
void Stars_Exuart6_Test()
{
    uint8_t Send_Cmd[] = {0xaa,0xbb,0x01,0xff,0xff,0xcc};
	Customer_usart6_Send(Send_Cmd , 6, 76);
//	uint8_t Send_Data[46] = {0xaa,0xbb,0x02,0x00,0x01,0x02,0x03,0x04,0x05,0xcc,
//							 0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,
//							 0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,
//							 0x26,0x27,0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,
//							 0x36,0x37,0x38,0x39,0x40,0xcc};
//	uint16_t crc = _crc_error(&Send_Data[4],46-7);
//	Send_Data[43] = (uint8_t)(crc/256);
//	Send_Data[44] = (uint8_t)crc;	
//	Customer_usart6_Send(Send_Data , 46, 46);	


}

//以下为驱动
//*******************************宏定义**************************************
#define USART6_DMA_RX_BUFFER_MAX_LENGTH		128
#define USART6_DMA_TX_BUFFER_MAX_LENGTH		128
uint8_t USART6_DMA_RX_Buffer[USART6_DMA_RX_BUFFER_MAX_LENGTH];
uint8_t USART6_DMA_TX_Buffer[USART6_DMA_TX_BUFFER_MAX_LENGTH];


//软定时器
typedef struct UT
{

	uint16_t counter;					//计数器
	uint16_t  Interval;					//监测间隔
	uint8_t  Enabled;
	void* (*Handler_Callback)(void*);	//定时器回调函数

}_USART6_SOFTTIIMER;

_USART6_SOFTTIIMER usart_timer;
//*******************************类型声明************************************
void USART6_Configuration(void);
void USART6_DMA_Tx_Configuration(void);
void USART6_DMA_Rx_Configuration(void);
void Exuart6_NVIC_Configuration(void);



void ExUart6_Configuration(void)
{
	Customer_Init();
	USART6_Configuration();
	Exuart6_NVIC_Configuration();
	USART6_DMA_Tx_Configuration();
	USART6_DMA_Rx_Configuration();
	
	RS485_RDE6_RX;
}

void soft_timer_init(_USART6_SOFTTIIMER* timer,void* (*_Callback)(void*))
{
	timer->counter = 0;
	timer->Interval = 0;
	timer->Enabled = 0;
	timer->Handler_Callback = _Callback;
}

void soft_timer_start(_USART6_SOFTTIIMER* timer,void* (*_Callback)(void*),int16_t interval)
{
	soft_timer_init(timer,_Callback);
	timer->Interval = interval;
	timer->Enabled = 1;
}

void soft_timer_stop(_USART6_SOFTTIIMER* timer)
{
	timer->Enabled = 0;
}

void usart6_soft_timer_driver()	//将此函数放入定时器中断中，否则不会判断软件超时
{
	if(usart_timer.Enabled == 1)
	{
		usart_timer.counter++;
		if(usart_timer.counter == usart_timer.Interval)
		{
			usart_timer.counter = 0;
			if(usart_timer.Handler_Callback !=NULL)
			{
				usart_timer.Handler_Callback(NULL);	//执行回调
			}
			soft_timer_stop(&usart_timer);
		}
	}	
	
}

void* over_timer_check(void* arg)
{
	if(customer_usart6.recv_finish ==0)
	{
		customer_usart6.recv_over_timer = 1;
		if(USART6_RW == 1)
		{
			customer_usart6.write_error = 1;
			USART6_RW = 0;
		}
	}

	return NULL;
}

 
void USART6_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);		//for USART1 and USART6 
 
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);   	
 
	/* Configure USART6 Rx (PC.07) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Configure USART6 Tx (PC.06) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 配置485通信方向控制引脚：RDE6 (PG.6) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    RS485_RDE6_RX; // 初始化485通信方向为接收
 
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);
	USART_Cmd(USART6, ENABLE);
		
	USART_ClearFlag(USART6, USART_FLAG_TC); //清除发送完成标志	
	while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);	//等待空闲帧发送完成后再清零发送完成标志（警告：如果不使能USART_Mode_Tx，会导致单片机在这里死机）
	USART_ClearFlag(USART6, USART_FLAG_TC);	//清除发送完成标志
 
    USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);				//禁止USART1接收不为空中断
	USART_ITConfig(USART6, USART_IT_TXE, DISABLE);				//禁止USART1发送空中断
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);				//开启USART1空闲中断 
	USART_ITConfig(USART6, USART_IT_TC, ENABLE);				//开启USART1传输完成中断 
	
	USART_DMACmd(USART6 ,   USART_DMAReq_Tx,ENABLE);  			//使能串口的DMA发送
	USART_DMACmd(USART6 ,   USART_DMAReq_Rx,ENABLE);  			//使能串口的DMA接收	
	
	
}
 
void USART6_DMA_Tx_Configuration(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 , ENABLE);					//DMA2时钟使能
	DMA_DeInit(DMA2_Stream6);
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE);						//等待DMA可配置
	DMA_InitStructure.DMA_Channel = DMA_Channel_5; 							//DMA通道配置
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;		//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)USART6_DMA_TX_Buffer;	//发送缓存指针
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;					//DMA传输方向：内存--->外设
	DMA_InitStructure.DMA_BufferSize = USART6_DMA_TX_BUFFER_MAX_LENGTH;		//数据传输字节数量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					//中等优先级 DMA_Priority_High
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//外设突发单次传输
	DMA_Init(DMA2_Stream6 , &DMA_InitStructure);							//初始化DMA Stream
	DMA_Cmd(DMA2_Stream6 , DISABLE); 										//开启DMA传输
}

void USART6_DMA_Rx_Configuration(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 , ENABLE);					//DMA2时钟使能
	DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);						//等待DMA可配置  
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;  						//通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;		//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)USART6_DMA_RX_Buffer;	//接收缓存指针
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;				//DMA传输方向：外设到存储器模式：外设--->内存
	DMA_InitStructure.DMA_BufferSize = USART6_DMA_RX_BUFFER_MAX_LENGTH;		//缓冲大小 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					//中等优先级 DMA_Priority_VeryHigh
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		//外设突发单次传输
	DMA_Init(DMA2_Stream1 , &DMA_InitStructure);							//初始化DMA_Stream5	
	DMA_Cmd(DMA2_Stream1 , ENABLE);  										//开启DMA传输
}


void Exuart6_NVIC_Configuration(void)
{
	 
	NVIC_InitTypeDef NVIC_InitStructure;
	/* 允许USART6中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 响应6
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
//    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;// RX
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
}


void USART6_DMA_Begin_Send(uint8_t *send_buffer , uint16_t nSendCount)
{	
	if (nSendCount < USART6_DMA_TX_BUFFER_MAX_LENGTH)
	{
		memcpy(USART6_DMA_TX_Buffer , send_buffer , nSendCount);
		DMA_Cmd(DMA2_Stream6 , DISABLE);                    //关闭DMA传输
		while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE);	//确保DMA可以被设置
		
		RS485_RDE6_TX;  
		DMA_SetCurrDataCounter(DMA2_Stream6 , nSendCount);  //数据传输量
		DMA_Cmd(DMA2_Stream6 , ENABLE);               		//开启DMA传输
	}
}



 
void USART6_IRQHandler(void)
{
	int16_t ch;
 
	
	if (USART_GetITStatus(USART6 , USART_IT_IDLE) != RESET)
	{		
		USART_ClearITPendingBit(USART6 , USART_IT_IDLE);	//必须先清除总线空闲中断标识，然后读一下数据寄存器，DMA接收才会正确（先读SR，然后读DR才能清除空闲中断标识）注意：这句必须要，否则不能够清除中断标志位。
		ch =  USART_ReceiveData(USART6);					//必须先清除总线空闲中断标识，然后读一下数据寄存器，DMA接收才会正确（先读SR，然后读DR才能清除空闲中断标识）注意：这句必须要，否则不能够清除中断标志位。
		
		
		
		DMA_Cmd(DMA2_Stream1 , DISABLE);
		DMA_ClearFlag(DMA2_Stream1 , DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);
		ch = USART6_DMA_RX_BUFFER_MAX_LENGTH - DMA_GetCurrDataCounter(DMA2_Stream1);
		if (ch > 0)
		{
			memset(customer_usart6.Rx_buffer,0,H6_USART6_RX_MAXLENTH);
			memcpy(customer_usart6.Rx_buffer,USART6_DMA_RX_Buffer,ch);	
			customer_usart6.recv_finish = 1;
			customer_usart6.recv_lenth = ch;
			
			soft_timer_stop(&usart_timer);

			if(customer_usart6.recv_lenth == customer_usart6.expect_recv_lenth)
			{
				customer_usart6.crc_error = crc_error_check();
			}
			else
			{
				//customer_usart6.recv_over_timer = 1;	//接收长度不符合预期也认为接收超时？
				customer_usart6.crc_error = 1;
			}
			
			
		}
		if(USART6_RW == 1)
		{
			USART6_RW = 0;
			if(ch>0)
			{
				if(memcmp(customer_usart6.Rx_buffer,USART6_DMA_RX_Buffer,customer_usart6.expect_recv_lenth)!=0)
				{
					customer_usart6.write_error = 1;
				}
			}
		}
		
		DMA_SetCurrDataCounter(DMA2_Stream1 , USART6_DMA_RX_BUFFER_MAX_LENGTH);
		DMA_Cmd(DMA2_Stream1 , ENABLE);
	}
	
	else if (USART_GetITStatus(USART6 , USART_IT_TC)!= RESET) 
	{
		USART_ClearITPendingBit(USART6 , USART_IT_TC);
		
		DMA_ClearFlag(DMA2_Stream6 , DMA_FLAG_TCIF6 | DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6);
		DMA_SetCurrDataCounter(DMA2_Stream6 , 0);
		
		RS485_RDE6_RX;

		//发送完成，启动软件超时定时器，在接收空闲中断停止定时器前，触发回调任务，则认为接收超时
		soft_timer_start(&usart_timer,over_timer_check,(uint16_t)H6_USART6_TIMER);
	}	
} 


