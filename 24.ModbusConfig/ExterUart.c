#include "ExterUart.H"
#include "stm32f4xx_conf.h"
#define   EXUSART_GLOBALS
#include "ExternPrototype.h"
#include "TimeBase.h"
#include "InterUart.h"
#include "AppFunc.h"
#include "UserMainTask.h"

#ifndef TEST_FOR_STARS				
	#include "HMI_Display.h"
	#include "GlobalVar_HMI.h"  
	#include "GlobalVar_POUS.h"
#else
#endif

#define USART1BAURATE     	115200
	
#define	RS485_RDE1 	    	GPIOG	
#define	RS485_RDE1_PIN 		GPIO_Pin_5	
#define RS485_RDE1_RX       GPIO_WriteBit(RS485_RDE1, RS485_RDE1_PIN, (BitAction)1)  
#define RS485_RDE1_TX      	GPIO_WriteBit(RS485_RDE1, RS485_RDE1_PIN, (BitAction)0)   
#define	RS485_RDE6 	    	GPIOG	
#define	RS485_RDE6_PIN 		GPIO_Pin_6	
#define RS485_RDE6_RX       GPIO_WriteBit(RS485_RDE6, RS485_RDE6_PIN, (BitAction)1)  
#define RS485_RDE6_TX      	GPIO_WriteBit(RS485_RDE6, RS485_RDE6_PIN, (BitAction)0)   
#define READ_RDE1      		GPIO_ReadInputDataBit(RS485_RDE1, RS485_RDE1_PIN) 
#define READ_RDE6      		GPIO_ReadInputDataBit(RS485_RDE6, RS485_RDE6_PIN) 

u16 PcCtrl = 0; 						// 上位机控制标志：1上位机控制，0上位机不控制
u16 PcCtrlFlag = 0; 					// 上位机开关使能标志：1开使能，0关使能
u16 PcCtrlFlag_Out = 0; 				// 上位机开关使能标志：1开使能，0关使能
u16	PcCtrlSet = 0;
s16 PcSet = 0;
u16	CommAddr;
u16	CANAddr = 0; 						// CAN通讯地址，0为星辰上位机协议通讯用
u8 Time_Adjust_Flag = 0;
u8 Charge_Ctrl_Flag = 0;

//USART1
u8 ExUart_RxBuf_Temp[USART1_DMA_DATA_LEN] = {0};			// 星辰协议、Modbus协议共用接收缓存数组。0x10功能码连续写300~329地址时最大，字节数为30*2+9=69Byte
u8 ExUart_RxBuf[USART1_DMA_DATA_LEN] = {0};					// 星辰协议、Modbus协议共用接收缓存数组。0x10功能码连续写300~329地址时最大，字节数为30*2+9=69Byte
u8 ExUart_TxBuf[USART1_DMA_DATA_LEN] = {0};					// 星辰协议下的串口发送数组(7Byte即可)
u8 Rs485ModbusSnd[MODBUS_SEND_LEN] = {0};					// Modbus协议下的串口发送数组,最多为回复0x03功能码：71Word*2+5=147Byte，预留到200

u16 Func_0x03_Data[READ_END_ADRESS + 1] = {0};
u16 Func_0x06_Data[WRITE_END_ADRESS + 1] = {0};
u16 Func_0x10_Data[WRITE_END_ADRESS + 1] = {0}; 

u16 Monitor_Data[31] = {0}; 								// HMI数据监控-->寄存器变量显示
u8 DMAInitTest = 0;

u16 ModbusFunc_0x03_Crc_Err = 0;
u16 ModbusFunc_0x06_Crc_Err = 0;
u16 ModbusFunc_0x10_Crc_Err = 0;
u16	ExUartRxLoc = 0;
u16 ExUartTxLoc = 0;
u16	ExUartRxFullFlag = 0;
u16	ExUartRxOverTimer = 0;
u16	ExUartRxOverTime = 12;
u16	ExUartTxOverTime = 12;
u16 Tx1Cnt = 400;
u16 Tx6Cnt = 400;
u16 DMA2_Rev_Data_Len = 0;
u16 DMA2_Rev_Data_Len1 = 0;
u16 DMA2_Rev_Data_Len_Remain = 0;
u16 DMA2_Rev_Data_Len_Old = 0;
u16 DMA2_Rev_Data_Len_Over_Cnt = 0;
u16 DMA2_Rev_Data_Len_Same_Cnt = 0;
u8 IAP_Cnt = 0;
u8 IAP_Flag = 0;
u8 Protocol = 0;						// 0=星辰协议(拨码开关地址为0)；1=Modbus RTU协议(拨码开关地址为非0)
u16	CANAddr_Real = 0; 					// 拨码开关实时地址
u16	CANAddr_Real_Old = 1; 				// 拨码开关上一次的地址
u16	ExModbusRxFullFlag = 0;   			// 接收RS485一帧完成标志
u16 Rs485ModbusSndLen = 147;			// 握手广播帧，ARM需发送字节数 = Rs485ModbusSndLen = BroadcastAddrLen * 2 + 5 = 147
u8 Rs485ModbusSndEn =0;
u8 Rs485ModbusSndEn_Temp =0;
u16 TxNumber = 0;	
u16 ReadEndAdress = 0;					// 读结束地址
u16 ReadStartAdress = 0;				// 读起始地址
u16 ReadNumber = 0;						// 读寄存器的数量	
u16 WriteEndAdress = 0;					// 写结束地址
u16 WriteStartAdress = 0;				// 写起始地址
u16 WriteNumber = 0;					// 写寄存器的数量
u16 WriteStarsId = 0;					// 写星辰协议参数ID号
u16 WriteStarsData = 0;					// 写星辰协议参数数值
u16 Crc = 0;
u8 FuncCode = 0;
u16 ErrRecordDataNum = 0;
u16 ErrRecordDataNum_Temp = 0;
u32 ErrRecordAddr_Temp = 0x080BFFC4;

//USART6
u16	ExUart6RxFullFlag = 0;
vu8 USARTErrCnt = 0;
vu8 SetNewBDR = 0;
vu8 USARTErrFlag = 0;
u16	ExRxFullFlag = 0;       			// 接收上位机一帧数据完成标志
u16	ExUartReturnFlag = 0;				// 0 不需要返回帧到上位机 1 需要返回帧到上位机
u16	ReturnSelect = 0;					// 0 返回ARM数据 1 返回DSP数据
u16	PendParaID = 0;						// 需要DSP返的帧ID
u16	SaveMode = 0;						// 0 无需保存 1 需要保存用户参数 2 需要保存用户参数和出厂默认参数
u16 delay_flag=0;
u8  AdjModFlag = 0; 					// 0：正常模式 1：调试模式
u8  LoadDefaultParaFlag = 0;			// 调出厂参数标志: 1调出厂参数
u8  * pExRxBuf;

u8 ClrCMDStart = 0; 
u8 ClrFaultFlag = 0;
bool FlashErrFlag = 0;

extern u8 DownLoadToDsp;										
extern u8 SdCardErrDataCnt;
extern u8 EraseErrFlag0;
extern u16 ReadErrRecordOK;
extern u16 DSP_ComSta;
extern u16 DSP_ComCMD;
extern u16 DSP_ComData;
extern s16 Target_position_Out;
extern s16 Current_actual_value;
extern u32 CiA402_Statusword;
extern u16 FirstErrCode;
extern u64 ServerFault;
extern u16 ServerFault1;
extern u16 ServerFault2;
extern u16 ServerFault3;
extern u16 ServerFault4;
extern u16 ServerFault5;
extern u16 ErrRecordDataArray[]; 
extern u16* ErrRecordAddr; 
extern u8 err_rst1;
extern u16 User_PLC_Sta;
extern u8 State_Machine;	
extern s16 Target_position_Test;
extern s16 Velocity_Limit_Test;
extern s32 MotoRpmSpeed;
extern s16 Position_actual_value_s16;
extern s32 Velocity_sensor_actual_value_temp;
extern bool RunMode_Test;
extern bool Runrequest_Test;
extern bool User_ID05;
extern bool gPosClear;
extern union DI_STATUS DIStatus;
extern void TestWriteDataHandle(void);
extern s16 TestReadArrary[];

#ifdef PARA_SECTOR_SELECT_16K
    #define SJFlagADDR 0x08004000
#else
    #define SJFlagADDR 0x0800C000	
#endif

typedef void (*pFunction)(void);
pFunction Jump_To_Bootloader;
u32 JumpAddress;
u8 UpdataFlag=0; 
u16 IAP_fLAG[2] = {0Xaa55, 0Xaa55}; 

/*******************************************************************************
* 函数名 : 
* 描  述 : 
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
// void Jump_bootloader()
// {
//     CPU_CRITICAL_ENTER();       												//关总中断
// 	EraseFlash_IAP((u32)SJFlagADDR, ErrRecord_PAGE_NUM);						//先擦除刷机标志位所在16K扇区
//     WriteHalfWordToFlash(SJFlagADDR,IAP_fLAG,2); 								//再写入刷机标志位
//     NVIC_SystemReset(); 														//复位
// }

/*******************************************************************************
* 函数名 : 串口1_DMA2_数据流7发送初始化函数
* 描  述 : 默认运达协议
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void USART1_DMA2_Tx_Configuration_WINDEY(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	DMA_DeInit(DMA2_Stream7);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;								//配置串口1接收：DMA2_通道4_数据流2
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_Base;                  //外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)Rs485ModbusSnd;             	//内存地址：Windey Modbus Protocol Buff
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                     //DMA传输方向：从外设读取，发送到内存
    DMA_InitStructure.DMA_BufferSize = Rs485ModbusSndLen;                 		//设置DMA在传输时缓冲区的长度：可变长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //设置DMA的内存递增模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据单位为字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存数据单位为字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;        						//设置DMA的传输模式：正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     //设置DMA的优先级别
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;						//开启DMA2数据流7发送中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_Stream7_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA2_Stream7_SBUPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA2_Stream7, ENABLE);
}

/*******************************************************************************
* 函数名 : 串口1_DMA2_数据流7发送初始化函数
* 描  述 : 星辰协议
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void USART1_DMA2_Tx_Configuration_STARTS(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	DMA_DeInit(DMA2_Stream7);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;								//配置串口1接收：DMA2_通道4_数据流2
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_Base;                  //外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ExUart_TxBuf;             		//内存地址：STARS Protocol Buff
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                     //DMA传输方向：从外设读取，发送到内存
    DMA_InitStructure.DMA_BufferSize = EXUART_TX_DATA_FRAMELEN;                 //设置DMA在传输时缓冲区的长度：星辰协议帧长固定7字节
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //设置DMA的内存递增模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据单位为字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存数据单位为字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;        						//设置DMA的传输模式：正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     //设置DMA的优先级别
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;						//开启DMA2数据流7发送中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_Stream7_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA2_Stream7_SBUPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA2_Stream7, ENABLE);
}

void USART1_DMA2_Tx_Configuration_STARTS_IAP(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	DMA_DeInit(DMA2_Stream7);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;								//配置串口1接收：DMA2_通道4_数据流2
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_Base;                  //外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ExUart_TxBuf;             		//内存地址：STARS Protocol Buff
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                     //DMA传输方向：从外设读取，发送到内存
    DMA_InitStructure.DMA_BufferSize = EXUART_TX_DATA_FRAMELEN;                 //设置DMA在传输时缓冲区的长度：星辰协议帧长固定7字节
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //设置DMA的内存递增模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据单位为字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存数据单位为字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;        						//设置DMA的传输模式：正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     //设置DMA的优先级别
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;						//开启DMA2数据流7发送中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_Stream7_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = DMA2_Stream7_SBUPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	//USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	//DMA_Cmd(DMA2_Stream7, ENABLE);
}

/*******************************************************************************
* 函数名 : 串口1_DMA2_数据流2接收初始化函数
* 描  述 : 最大长度100字节（预留），目前实际最多接收21字节
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void USART1_DMA2_Rx_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA2_Stream2);
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;								//配置串口1接收：DMA2_通道4_数据流2
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_Base;                  //外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ExUart_RxBuf_Temp;             //内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     //DMA传输方向：从外设读取，发送到内存
    DMA_InitStructure.DMA_BufferSize = USART1_DMA_DATA_LEN;                 	//设置DMA在传输时缓冲区的长度：最大100
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //设置DMA的内存递增模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据单位为字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存数据单位为字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;        						//设置DMA的传输模式：循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     //设置DMA的优先级别
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream2, &DMA_InitStructure);

    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);								//允许串口1_DMA2请求，但不启用DMA接收中断，由串口1空闲中断执行接收处理
	DMA_Cmd(DMA2_Stream2, ENABLE);												//允许DMA2数据流2
}

/*******************************************************************************
* 函数名 : USART1_Configuration
* 描  述 : 配置与上位机RS485通讯的USART1
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void ExUart1_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //与STM32F10X不同，STM32F40X使用端口复用时需要使用GPIO_PinAFConfig函数进行指定引脚的挂载,
    //除ADC和DAC外的所有非GPIO功能，都要使用GPIO_PinAFConfig函数进行端口复用，端口复用没有时钟，直接使用即可

    /*AF功能的映*/
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_USART1);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置485通信方向控制引脚：RDE1 (PG.5) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
	RS485_RDE1_RX;												// 初始化485通信方向为接收
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			// 允许USART1中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART1_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART1_SBUPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		// 开启USART1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);		// 开启DMA2时钟
	
	USART_InitStructure.USART_BaudRate = USART1BAURATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
    USART_Init(USART1, &USART_InitStructure);		
    USART_ITConfig(USART1, USART_IT_TC, ENABLE); 				// 允许发送寄存器为空、发送完成中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 				// 允许串口空闲中断（从有数据状态到无数据状态，持续时间大于一个字节，即判定为总线空闲）
		
	USART1_DMA2_Rx_Configuration();								// 配置串口1_DMA2_通道4_数据流2接收（接收长度最大=100Byte）
	USART1_DMA2_Tx_Configuration_WINDEY();						// 配置串口1_DMA2_通道4_数据流7发送（上电默认运达协议）
    USART_Cmd(USART1, ENABLE);									// 注意：需保证USART的使能在DMA初始化之后
	
    TxToPCCompleteFlag = 1;          							// 初始化发送完成标志为发送完成
}

/*******************************************************************************
* Function Name  : 串口1_DMA2_通道4_数据流2接收处理函数：判断接收到何种协议数据帧：星辰协议（7个字节）、或运达Modbus协议（包含0x03、0x06、0x10）
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_DMA2_Rx_Process(void)
{			
	DMA2_Rev_Data_Len_Remain = DMA_GetCurrDataCounter(DMA2_Stream2);// DMA方式接收到的字节数=定义的总数-剩余数	
	DMA2_Rev_Data_Len = USART1_DMA_DATA_LEN - DMA2_Rev_Data_Len_Remain;	
	
	if (Protocol == 1)												// Modbus RTU协议
	{		
		if ((DMA2_Rev_Data_Len == USART1_MODBUS_0X03_DATA_LEN) && ((ExUart_RxBuf[1] == MODBUS_FUNC_0X03) || (ExUart_RxBuf[1] == MODBUS_FUNC_0X06)))							
		{																							
			ExUartRxOverTimer = 0;
			ExModbusRxFullFlag = 1;									// 0x03功能码读71个地址、或0x06功能码写1个地址
		}
		else if ((DMA2_Rev_Data_Len == USART1_MODBUS_0X10_DATA_LEN1) && (ExUart_RxBuf[1] == MODBUS_FUNC_0X10) && (ExUart_RxBuf[5] == 2))		
		{																							
			ExUartRxOverTimer = 0;
			ExModbusRxFullFlag = 1;									// 0x10功能码写2个地址（0~1或1~2、或38~39）
		}
		else if ((DMA2_Rev_Data_Len == USART1_MODBUS_0X10_DATA_LEN2) && (ExUart_RxBuf[1] == MODBUS_FUNC_0X10) && (ExUart_RxBuf[5] == 6))		
		{																							
			ExUartRxOverTimer = 0;
			ExModbusRxFullFlag = 1;									// 0x10功能码写6个地址（北京时间）
		}
		else if ((DMA2_Rev_Data_Len == USART1_STARS_DATA_LEN) && (ExUart_RxBuf[0] == 1) && (ExUart_RxBuf[1] == 0xC0) && (ExUart_RxBuf[2] == 0x07))							
		{
			IAP_Flag = 1;																			
			IAP_Cnt ++;
			ExUartRxOverTimer = 0;
			ExUartRxFullFlag = 1;									// 拨码开关在非0地址下进行IAP升级	
		}
		
		if ((ExUart_RxBuf[0] != ModbusAddr) && (ExUart_RxBuf[0] != BroadcastAddr) && (ExUart_RxBuf[0] != BroadcastErrAddr))
		{
			ExModbusRxFullFlag = 0;
		}
	}
	else															// 星辰协议
	{	
		if (DMA2_Rev_Data_Len == USART1_STARS_DATA_LEN)
		{
			ExUartRxOverTimer = 0;
			ExUartRxFullFlag = 1;
			
			if ((ExUart_RxBuf[0] != CommAddr) && (ExUart_RxBuf[0] != 201)  && (ExUart_RxBuf[0] != 0xFF))		
			{
				ExUartRxFullFlag = 0;								// 通讯地址不符
			}
		}
	}
}

/*******************************************************************************
* Function Name  : DMA2数据流7传输完成中断
* Description    : 数据从内存ExUart_TxBuf[]、或Rs485ModbusSnd[]到串口1传输完成
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Stream7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) != RESET)
    {
        DMA_ClearITPendingBit (DMA2_Stream7,DMA_IT_TCIF7);
        USART_ClearITPendingBit(USART1, USART_IT_TC);
        USART_ITConfig(USART1, USART_IT_TC, ENABLE); 				// 允许串口1发送完成中断（数据先从内存地址搬运至串口外设，再由串口发出）
    }
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{	
    u16 RxErrDat;
    u16 i = 0;

    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)   		// USART1产生了空闲中断
    {
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);				// 清除USART1空闲中断中断标志（注意：先读SR，再读DR，才能清除空闲中断标志IDLE）
        RxErrDat = USART_ReceiveData(USART1); 
		
		DMA2_Rev_Data_Len1 = USART1_DMA_DATA_LEN - DMA_GetCurrDataCounter(DMA2_Stream2);
		
        for (i = 0; i < DMA2_Rev_Data_Len1; i++)					// 将DMA接收的数据转移到ExUart_RxBuf数组中
        {
            ExUart_RxBuf[i] = ExUart_RxBuf_Temp[i];
        }
		
		USART1_DMA2_Rx_Process();									// 判断接收到何种协议数据帧：星辰协议（7个字节）、或运达Modbus协议（包含0x03、0x06、0x10功能码）
		
        DMA_Cmd(DMA2_Stream2, DISABLE);
        DMA_ClearFlag(DMA2_Stream2,DMA_IT_TCIF2);					// 串口1空闲中断已完成数据接收协议判断，清DMA传输完成标志（接收完成）
        DMA2_Stream2->NDTR = USART1_DMA_DATA_LEN;					// 重置DMA接收长度：100个字节
        DMA_Cmd(DMA2_Stream2, ENABLE);
    }
    else if(USART_GetITStatus(USART1, USART_IT_FE)  != RESET)   	// 帧错误
    {
        USART_GetFlagStatus(USART1, USART_FLAG_FE);       			// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        RxErrDat = USART_ReceiveData(USART1);
    }
    else if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET) 		// 溢出错误
    {
        USART_GetFlagStatus(USART1, USART_FLAG_ORE);       			// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        RxErrDat = USART_ReceiveData(USART1);
    }
    else if (USART_GetITStatus(USART1, USART_IT_NE)  != RESET) 		// 噪声错误
    {
        USART_GetFlagStatus(USART1, USART_FLAG_NE);       			// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        RxErrDat = USART_ReceiveData(USART1);
    }
    else if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)   		// 发送完成中断
    {
        USART_ClearITPendingBit(USART1, USART_IT_TC);				// 清除USART1发送完成中断标志
        USART_ITConfig(USART1, USART_IT_TC, DISABLE); 				// 禁止中断
        RS485_RDE1_RX;												// 控制485通信方向为接收
		TxToPCCompleteFlag = 1;                       				// 置一帧发送完成标志
    }
}

/*******************************************************************************
* 函数名 : USART6_Configuration
* 描  述 : 配置与上位机RS485通讯的USART6
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void ExUart6_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //与STM32F10X不同，STM32F40X使用端口复用时需要使用GPIO_PinAFConfig函数进行指定引脚的挂载,
    //除ADC和DAC外的所有非GPIO功能，都要使用GPIO_PinAFConfig函数进行端口复用，端口复用没有时钟，直接使用即可

    /*AF功能的映*/
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource6, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource7, GPIO_AF_USART6);

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
	RS485_RDE6_RX;													// 初始化485通信方向为接收

    /* 允许USART6中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART6_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART6_SBUPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART6 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    USART_InitStructure.USART_BaudRate = 19200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    /* 配置USART6 */
    USART_Init(USART6, &USART_InitStructure);

    /* 允许USART6接收、发送中断 */
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);              	// 允许接收寄存器不为空中断
    USART_ITConfig(USART6, USART_IT_TXE, ENABLE); 					// 允许发送寄存器为空、发送完成中断
    USART_ITConfig(USART6, USART_IT_TC, ENABLE); 					// 允许发送寄存器为空、发送完成中断
    USART_ITConfig(USART6, USART_IT_ERR, ENABLE);      				// 允许串口接收错误中断

    /* 允许 USART6 */
    USART_Cmd(USART6, ENABLE);

    TxToPCCompleteFlag = 1;          								// 初始化发送完成标志为发送完成
}

/*******************************************************************************
* Function Name  : USART6_IRQHandler
* Description    : This function handles USART6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART6_IRQHandler(void)
{
    vu16 RxErrDat;

    /* 串口错误中断处理 */

    if(USART_GetITStatus(USART6, USART_IT_ORE) != RESET) 			// 溢出错误
    {
        USART_GetFlagStatus(USART6, USART_FLAG_ORE);       			// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        RxErrDat = USART_ReceiveData(USART6);    //
        USARTErrFlag = 1;
    }
    else if(USART_GetITStatus(USART6, USART_IT_NE)  != RESET) 		// 噪声错误
    {
        USART_GetFlagStatus(USART6, USART_FLAG_NE);       			// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        RxErrDat = USART_ReceiveData(USART6);
        USARTErrFlag = 1;
    }
    else if(USART_GetITStatus(USART6, USART_IT_FE)  != RESET)   	// 帧错误
    {
        USART_GetFlagStatus(USART6, USART_FLAG_FE);       			// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        RxErrDat = USART_ReceiveData(USART6);
        USARTErrFlag = 1;
    }
    else if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
    {
        ExUart_RxBuf[ExUartRxLoc]  = USART_ReceiveData(USART6);		// 接收USART6的数据
        ExUartRxOverTimer=0;
        ExUartRxLoc++;
        if(ExUartRxLoc > (EXUART_RX_DATA_FRAMELEN - 1))
        {
            ExUart6RxFullFlag = 1;
            ExUartRxLoc = 0;
            USARTErrCnt = 0;               							// 通讯正常清除错误计数
        }
        if((ExUart_RxBuf[0] != CommAddr) && (ExUart_RxBuf[0] != 201)  && (ExUart_RxBuf[0] != 0xFF))		
        {
            ExUart6RxFullFlag = 0;
            ExUartRxLoc = 0;										// 通讯地址不符则清零
        }
    }
    else if(USART_GetITStatus(USART6, USART_IT_TXE) != RESET)
    {
        if(ExUartTxLoc > (EXUART_TX_DATA_FRAMELEN - 1))
        {
            ExUartTxLoc = 0;
            USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
        }
        else
        {
            USART_SendData(USART6, ExUart_TxBuf[ExUartTxLoc]);
            ExUartTxLoc++;
        }
    }
    else if(USART_GetITStatus(USART6, USART_IT_TC) != RESET)   		// 发送完成中断
    {
        USART_ClearFlag(USART6, USART_FLAG_TC);						// 清除USART6发送完成中断标志 */
        if (ExUartTxLoc == 0)                             			// 一帧发送完成
        {
			RS485_RDE6_RX;											// 控制485通信方向为接收
            TxToPCCompleteFlag = 1;                       			// 置一帧发送完成标志
        }
    }
}

void ExUartTxFrameAsm(u16 Data)
{
    u16	i;
    ExUart_TxBuf[0] = pExRxBuf[0]; 									// CommAddr;
    ExUart_TxBuf[1] = pExRxBuf[1]; 									// ExUart_RxBuf[1];
    ExUart_TxBuf[2] = pExRxBuf[2]; 									// ExUart_RxBuf[2];通讯ID
    ExUart_TxBuf[3] = Data/256;
    ExUart_TxBuf[4] = (u8)Data;										// 返回数据
    i=CRC16(ExUart_TxBuf, EXUART_TX_DATA_FRAMELEN-2);
    ExUart_TxBuf[EXUART_TX_DATA_FRAMELEN-2]	= i/256;
    ExUart_TxBuf[EXUART_TX_DATA_FRAMELEN-1]	= (u8)i;
}

void PwrTxFrameAsm(u8 Addr, u16 CMD, u16 Data)
{
    u16	i;
    ExUart_TxBuf[0] = Addr;
    ExUart_TxBuf[1] = CMD/256;
    ExUart_TxBuf[2] = (u8)CMD;
    ExUart_TxBuf[3] = Data/256;
    ExUart_TxBuf[4] = (u8)Data;
    i=CRC16(ExUart_TxBuf, EXUART_TX_DATA_FRAMELEN-2);
    ExUart_TxBuf[EXUART_TX_DATA_FRAMELEN-2]	= i/256;
    ExUart_TxBuf[EXUART_TX_DATA_FRAMELEN-1]	= (u8)i;
}

/************************************************************************
 CRC计算函数  MODBUS标准CRC计算
 功能：MODBUS的CRC-16快速查表计算函数
************************************************************************/
u16 MODBUS_CRC16 (u8 *nData, u8 wLength)
{
    static const u16 wCRCTable[] = 
	{
        0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
        0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
        0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
        0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
        0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,

        0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
        0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
        0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
        0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
        0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
        0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
        0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
        0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,

        0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
        0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
        0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
        0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
        0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
        0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
        0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
        0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,

        0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
        0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
        0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
        0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
        0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
        0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
        0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
        0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,

        0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
        0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
        0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
    };

    u8 nTemp;
    u16 wCRCWord = 0xFFFF;

    while (wLength--)
    {
        nTemp = *nData++ ^ wCRCWord;
        wCRCWord >>= 8;
        wCRCWord  ^= wCRCTable[nTemp];
    }
    return wCRCWord;
}

u16 Func_0x03_DataForTest = 0;
void Read_Func_0x03_Data(void)
{
	Func_0x03_Data[0]	=	Target_position_Out;								// 主控给定角度				0.01°
	Func_0x03_Data[1]	=	Position_actual_value_s16;							// 编码器角度				0.01°	范围：-90.00°~270.00°
	Func_0x03_Data[2]	=	0;													// 预留
	Func_0x03_Data[3]	=	ReadPara(PITCH_SPEED);								// 调桨速度					0.1°/S
	Func_0x03_Data[4]	=	ReadPara(C_VOL)*10;									// 直流母线电压				0.1V
	Func_0x03_Data[5]	=	MotoRpmSpeed;										// 电机转速					1RPM
	Func_0x03_Data[6]	=	Current_actual_value;								// 电机电流					0.1A
	Func_0x03_Data[7]	=	Current_actual_value*2;								// 电机转矩					电流*扭矩系数
	Func_0x03_Data[8]	=	ReadPara(TEMP0)*10;									// 轮毂温度					0.01°C
	Func_0x03_Data[9]	=	ReadPara(IGBT_TEMP)*100;							// 驱动器温度				0.01°C
	Func_0x03_Data[10]	=	ReadPara(TEMP1)*10;									// 散热片温度				0.01°C
	Func_0x03_Data[11]	=	ReadPara(TEMP2)*10;									// 驱动柜温度				0.01°C
	Func_0x03_Data[12]	=	ReadPara(MOTO_TEMP)*100;							// 电机温度					0.01°C
	Func_0x03_Data[13]	=	ReadPara(TEMP3)*10;									// 电容柜温度				0.01°C
	Func_0x03_Data[14]	=	ReadPara(CHARG_V_FEEDBACK)*10;						// 电容电压					0.1°V
	Func_0x03_Data[15]	=	ReadPara(CHARG_CAP)/10;								// 电容容值					0.001F
	Func_0x03_Data[16]	=	ReadPara(CHARG_I_FEEDBACK)*10;						// 充电电流					0.1A
	Func_0x03_Data[17]	=	0;													// 86°开桨上升沿角度		0.01°
	Func_0x03_Data[18]	=	0;													// 86°开桨下降沿角度		0.01°	
	Func_0x03_Data[19]	=	ReadPara(CHARG_INTERSPACE_TEMP)*100;				// 电源板温度				0.01°C
	Func_0x03_Data[20]	=	0;													// 相对湿度					1°
	Func_0x03_Data[21]	=	User_PLC_Sta*100;									// 86°接近开关偏差值		0.01°			
	Func_0x03_Data[22]	=	ReadPara(SCALE_B);									// 总传动比
	Func_0x03_Data[23]	=	Func_0x03_DataForTest++;							// 5°开桨上升沿角度			0.01°		[For Test]			
	Func_0x03_Data[24]	=	ReadPara(DSP_HEARTBEAT_COUNT);						// 5°开桨下降沿角度			0.01°		[For Test]
	Func_0x03_Data[25]	=	ReadPara(CHARG_AC_VOL);								// 电网电压					1V
	Func_0x03_Data[26]	=	0;													// 预留
	Func_0x03_Data[27]	=	0;													// 5°接近开关偏差值			0.01°
	Func_0x03_Data[28]	=	ModbusAddr;											// CAN节点号+10
	Func_0x03_Data[29]	=	State_Machine;										// 驱动器状态机
	Func_0x03_Data[30]	=	0;													// 状态机状态字
	Func_0x03_Data[31]	=	0;													// 手动模式状态字
	Func_0x03_Data[32]	=	FirstErrCode;										// 首发故障码
	Func_0x03_Data[33]	=	0;													// 自学习状态字
	Func_0x03_Data[34]	=	DoubleRAM.StateWord;								// 驱动器系统状态字
	Func_0x03_Data[35]	=	ReadPara(DI_STA);									// DI状态字					//强制DI5=1：ReadPara(DI_STA) | 0x0020;
	Func_0x03_Data[36]	=	ReadPara(DO_STA);									// DO状态字
	
	Func_0x03_Data[37]	=   													// 主控控制字1反馈
	  ((((DIStatus.bit.DI05)													
	  &&((Func_0x10_Data[0] >> 0)&1))				? 1 : 0) << 0)				// bit0		调试模式反馈
	| (((err_rst1)									? 1 : 0) << 1);				// bit1		复位状态反馈

	Func_0x03_Data[38]	=														// 主控控制字2反馈
	  (((gPosClear) 								? 1 : 0) << 0)				// bit0		角度校验
	| (((((Func_0x06_Data[1] >> 1) & 1))			? 1 : 0) << 1)				// bit1		点动控制使能反馈
	| (((((Func_0x06_Data[1] >> 2) & 1))			? 1 : 0) << 2)				// bit2		点动开桨反馈
	| (((((Func_0x06_Data[1] >> 3) & 1))			? 1 : 0) << 3)				// bit3		点动顺桨反馈
	| (((0) 										? 1 : 0) << 4)				// bit4		预留
	| (((0) 										? 1 : 0) << 5)				// bit5		预留
	| (((((Func_0x06_Data[1] >> 6) & 1))			? 1 : 0) << 6)				// bit6		区间控制使能反馈
	| (((((Func_0x06_Data[1] >> 7) & 1))			? 1 : 0) << 7)				// bit7		区间控制启动反馈
	| (((0) 										? 1 : 0) << 8)				// bit8		预留
	| (((((Func_0x06_Data[1] >> 9) & 1))			? 1 : 0) << 9)				// bit9		位置控制使能反馈
	| (((((Func_0x06_Data[1] >> 10) & 1))			? 1 : 0) << 10)				// bit10	位置控制启动反馈
    | (((0) 										? 1 : 0) << 11)				// bit11	预留
	| (((0) 										? 1 : 0) << 12)				// bit12	自学习开始
	| (((0) 										? 1 : 0) << 13)				// bit13	动态测试使能
	| (((0) 										? 1 : 0) << 14)				// bit14	动态测试运行
	| (((0) 										? 1 : 0) << 15);			// bit15	预留
	
	Func_0x03_Data[39]	=														// 主控控制字3反馈
	  (((0) 										? 1 : 0) << 0)				// bit0		预留
	| (((0) 										? 1 : 0) << 1)				// bit1		预留
	| (((0) 										? 1 : 0) << 2)				// bit2		预留
	| (((((Func_0x06_Data[2] >> 3) & 1))			? 1 : 0) << 3)				// bit3		电机预热反馈
	| (((((Func_0x10_Data[2] >> 4) & 1))			? 1 : 0) << 4)				// bit4		主电源隔离指令
	| (((((Func_0x10_Data[2] >> 5) & 1))			? 1 : 0) << 5)				// bit5		电容放电指令
	| (((((Func_0x06_Data[2] >> 6) & 1))			? 1 : 0) << 6)				// bit6		禁止充电指令
	| (((((Func_0x10_Data[2] >> 7) & 1))			? 1 : 0) << 7)				// bit7		超级电容容量测试
	| (((((Func_0x06_Data[2] >> 8) & 1))			? 1 : 0) << 8)				// bit8		外部风扇测试
	| (((((Func_0x06_Data[2] >> 9) & 1))			? 1 : 0) << 9)				// bit9		内部风扇测试
	| (((((Func_0x06_Data[2] >> 10) & 1))			? 1 : 0) << 10)				// bit10	关闭内部风扇
	| (((((Func_0x10_Data[2] >> 11) & 1))			? 1 : 0) << 11)				// bit11	润滑泵供电测试
	| (((0) 										? 1 : 0) << 12)				// bit12	预留
	| (((0) 										? 1 : 0) << 13)				// bit13	预留
	| (((0) 										? 1 : 0) << 14)				// bit14	预留
	| (((0) 										? 1 : 0) << 15);			// bit15	预留
	
	Func_0x03_Data[40]	=														// 主控控制字4反馈
	  (((((Func_0x06_Data[3] >> 0) & 1))			? 1 : 0) << 0)				// bit0		DO测试使能
	| (((((Func_0x06_Data[3] >> 1) & 1))			? 1 : 0) << 1)				// bit1		DO0输出
	| (((((Func_0x06_Data[3] >> 2) & 1))			? 1 : 0) << 2)				// bit2		DO1输出
	| (((((Func_0x06_Data[3] >> 3) & 1))			? 1 : 0) << 3)				// bit3		DO2输出
	| (((((Func_0x06_Data[3] >> 4) & 1))			? 1 : 0) << 4)				// bit4		DO3输出
	| (((((Func_0x06_Data[3] >> 5) & 1))			? 1 : 0) << 5)				// bit5		DO4输出
	| (((((Func_0x06_Data[3] >> 6) & 1))			? 1 : 0) << 6)				// bit6		DO5输出
	| (((((Func_0x06_Data[3] >> 7) & 1))			? 1 : 0) << 7)				// bit7		DO6输出
	| (((((Func_0x06_Data[3] >> 8) & 1))			? 1 : 0) << 8)				// bit8		DO7输出
	| (((((Func_0x06_Data[3] >> 9) & 1))			? 1 : 0) << 9)				// bit9		DO8输出
	| (((((Func_0x06_Data[3] >> 10) & 1))			? 1 : 0) << 10)				// bit10	Relay输出
	| (((0) 										? 1 : 0) << 11)				// bit11	预留
	| (((0) 										? 1 : 0) << 12)				// bit12	预留
	| (((0) 										? 1 : 0) << 13)				// bit13	预留
	| (((0) 										? 1 : 0) << 14)				// bit14	预留
	| (((0) 										? 1 : 0) << 15);			// bit15	预留
	
	Func_0x03_Data[41]	=	0;													// 版本号_1
	Func_0x03_Data[42]	=	0;													// 版本号_2
	Func_0x03_Data[43]	=	0;													// 版本号_3
	Func_0x03_Data[44]	= 	ReadPara(RTC_YEAR_FEB) + 2000;						// 系统时间：年
	Func_0x03_Data[45]	=	ReadPara(RTC_MONTH_AND_DATA_FEB) / 100;				// 系统时间：月
	Func_0x03_Data[46]	=	ReadPara(RTC_MONTH_AND_DATA_FEB) % 100;				// 系统时间：日
	Func_0x03_Data[47]	= 	ReadPara(RTC_HOUR_AND_MINIT_FEB) / 100;				// 系统时间：时
	Func_0x03_Data[48]	=	ReadPara(RTC_HOUR_AND_MINIT_FEB) % 100;				// 系统时间：分
	Func_0x03_Data[49]	=	ReadPara(RTC_SECON_FEB);							// 系统时间：秒 
// 	Func_0x03_Data[50]	=	ServerFault1;										// 驱动器故障1
// 	Func_0x03_Data[51]	=	ServerFault2;										// 驱动器故障2
// 	Func_0x03_Data[52]	=	ServerFault3;										// 驱动器故障3
// 	Func_0x03_Data[53]	=	ServerFault5;										// 驱动器故障5
	Func_0x03_Data[50]	=	ServerFault1 << 1; 									// 驱动器故障1
	Func_0x03_Data[51]	=	ServerFault2 << 1 | (ServerFault1 & 0x0001);       	// 驱动器故障2
	Func_0x03_Data[52]	=	ServerFault3 << 1 | (ServerFault2 & 0x0001);    	// 驱动器故障3
	Func_0x03_Data[53]	=	ServerFault4 << 1 | (ServerFault3 & 0x0001);    	// 驱动器故障4
	Func_0x03_Data[54]	=	0;													// 系统故障1
	Func_0x03_Data[55]	=	0;													// 系统故障2
	Func_0x03_Data[56]	=	0;													// 系统故障3
	Func_0x03_Data[57]	=	0;													// 系统故障4
	Func_0x03_Data[58]	=	ReadPara(ARM_BUILD_TIME);							// ARM版本号					0x1120 = V1.1.2.0版本
	Func_0x03_Data[59]	=	ReadPara(ARM_BUILD_DATE);							// DSP版本号					120 = V1.2.0版本
	Func_0x03_Data[60]	=	0;													// 预留
	Func_0x03_Data[61]	=	0;													// 预留
	Func_0x03_Data[62]	=	0;													// 预留
	Func_0x03_Data[63]	=	0;													// 预留
	Func_0x03_Data[64]	=	0;													// 预留
	Func_0x03_Data[65]	=	0;													// 预留
	Func_0x03_Data[66]	=	ReadPara(MOTO_TYPE);								// 电机类型
	Func_0x03_Data[67]	=	0;													// 预留
	Func_0x03_Data[68]	=	0;													// 预留
	Func_0x03_Data[69]	=	0;													// 预留
	Func_0x03_Data[70]	=	0;													// 预留
}

void Read_Func_0x03_Monitor_Data(void)											
{
	Monitor_Data[0]		=	0;													// NULL(从1开始到30,与HMI序号对应)
	Monitor_Data[1]		=	ReadPara(WriteStarsId);								// 查询状态寄存器修ID数值
	Monitor_Data[2]		=	ReadPara(ALARM_MASK);								// 驱动器低16位故障屏蔽
 	Monitor_Data[3]		=	ReadPara(ALARM_MASKH);								// 驱动器高16位故障屏蔽
 	Monitor_Data[4]		=	ReadPara(CHARG_ALARM_EN);							// 充电器低16位故障屏蔽
	Monitor_Data[5]		=	ReadPara(PITCH_PLC_STATE);							// 驱动器模式
	Monitor_Data[6]		=	ReadPara(SEC_POS_NUM);								// SSI圈数
	Monitor_Data[7]		=	ReadPara(SEC_POS);									// SSI位置
	Monitor_Data[8]		=	ReadPara(MOTO_ADDR_DISPL);							// 旋变存盘的最新地址低16位
	Monitor_Data[9]		=	ReadPara(MOTO_ADDR_DISPH);							// 旋变存盘的最新地址高16位
	Monitor_Data[10]	=	ReadPara(CAP_VOLT);									// AI0反馈
	Monitor_Data[11]	=	ReadPara(AI1P_DISP);								// AI1反馈
	Monitor_Data[12]	=	ReadPara(CHARG_SCR_CTR);							// 半控桥开关
	Monitor_Data[13]	=	ReadPara(P_PV);										// 位置环下的速度限制值 0.01°/S
	Monitor_Data[14]	=	ReadPara(CHARG_V_SET);								// 充电器电压设定
	Monitor_Data[15]	=	ReadPara(CHARG_I_SET);								// 充电器电流设定
	Monitor_Data[16]	=	ReadPara(TARGET_POSITION_L);						// 位置给定L		0.01°
	Monitor_Data[17]	=	ReadPara(PITCH_ANGLE);								// 位置反馈		0.01°
	Monitor_Data[18]	=	ReadPara(WORK_MOD);									// 驱动器工作模式
	Monitor_Data[19]	=	ReadPara(SYS_ST_CODE);								// 驱动器系统状态
	Monitor_Data[20]	=	ReadPara(DEFECT_CODEM);								// 驱动器M故障
	Monitor_Data[21]	=	ReadPara(DEFECT_CODET);								// 驱动器T故障
	Monitor_Data[22]	=	ReadPara(DEFECT_CODE);								// 驱动器L故障
	Monitor_Data[23]	=	ReadPara(DEFECT_CODEH);								// 驱动器H故障
 	Monitor_Data[24]	=	ReadPara(CHARG_DEFECT_CODE);						// 充电器C故障
	Monitor_Data[25]	=	ReadPara(ARM_BUILD_YEAR);							// ARM编译年份
	Monitor_Data[26]	=	ReadPara(ARM_BUILD_DATE);							// ARM编译月日
	Monitor_Data[27]	=	ReadPara(ARM_BUILD_TIME);							// ARM编译时分
	Monitor_Data[28]	=	ReadPara(DSP_BUILD_YEAR);							// DSP编译年份
	Monitor_Data[29]	=	ReadPara(DSP_BUILD_DATE);							// DSP编译月日
	Monitor_Data[30]	=	ReadPara(DSP_BUILD_TIME);							// DSP编译时分
}

void Write_Func_0x06_Data(void)													// HMI数据监控菜单-->地址300~329寄存器状态值修改 
{
	//寄存器状态值修改目前的数量类型为int16，-32768~32767，后续考虑优化为0~65535，否则无法设置大于32767的数值
	
	if (WriteStartAdress == REGISTER_WRITE_START_ADRESS + 1 -1)					// 先发送寄存器1：参数号
	{
		WriteStarsId = Func_0x06_Data[WriteStartAdress];
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 2 -1))			// 再发送寄存器2：数据值
	{
		WriteStarsData = Func_0x06_Data[WriteStartAdress];	
		
		if ((WriteStarsId > 0) && (WriteStarsId < 999))
		{
			if ((WriteStarsId > 653) && (WriteStarsId < 738))
			{
				switch (WriteStarsId)
				{
					case 673:	//Charger Over Current = 6.5A
					{
						ReadPara(WriteStarsId) = WriteStarsData;
						WriteChargPara((CP_ALARM_TRIG2 | 0x8000), ReadPara(WriteStarsId));
						break;
					}
					case 701:	//Charger DClink Low Vol = 390VDC
					{
						ReadPara(WriteStarsId) = WriteStarsData;
						WriteChargPara((CP_ALARM_TRIG6 | 0x8000), ReadPara(WriteStarsId));
						break;
					}
					case 708:	//Charger Igbt Over Temp = 95°
					{
						ReadPara(WriteStarsId) = WriteStarsData;
						WriteChargPara((CP_ALARM_TRIG7 | 0x8000), ReadPara(WriteStarsId));
						break;
					}
					case 736:	//Charger DClink Over Vol = 790VDC
					{
						ReadPara(WriteStarsId) = WriteStarsData;
						WriteChargPara((CP_ALARM_TRIG10 | 0x8000), ReadPara(WriteStarsId));
						break;
					}				
					default:
					{
						break;
					}
				}	

				WriteStarsId = 0;
			}
			else if (ExCANopenRxFullFlag == 0)		
			{		
				ReadPara(WriteStarsId) = WriteStarsData;
				
				//if ((ExCANopenRxFullFlag == 0) && (WriteStarsId < 429))
				{
					UpSysPara(0x80, WriteStarsId, WriteStarsData);
				}
				
				WriteStarsId = 0;
			}
		}
		else if ((WriteStarsId == 1999) && (WriteStarsData == 5555))			// 开放参数上下限制
		{
			AdjModFlag = 1;
			WriteStarsId = 0;
			WriteStarsData = 0;
		}
		else
		{
			WriteStarsId = 0;
			WriteStarsData = 0;
		}
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 3 -1))			// ID776 充电器低16位告警屏蔽 
	{
		//ReadPara(CHARG_ALARM_EN) = Func_0x06_Data[WriteStartAdress];
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 4 -1))			// ID62  驱动器低16位告警手/自动复位
	{
		ReadPara(ALARM_AUTO_RST_EN) = Func_0x06_Data[WriteStartAdress];			
		if (ExCANopenRxFullFlag == 0)		
		{
			UpSysPara(0x80, ALARM_AUTO_RST_EN,ReadPara(ALARM_AUTO_RST_EN));
		}
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 5 -1))			// ID260 驱动器高16位告警手/自动复位
	{
		ReadPara(ALARM_AUTO_RST_ENH) = Func_0x06_Data[WriteStartAdress];
		if (ExCANopenRxFullFlag == 0)		
		{		
			UpSysPara(0x80, ALARM_AUTO_RST_ENH,ReadPara(ALARM_AUTO_RST_ENH));
		}
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 6 -1))			// ID777 充电器低16位告警手/自动复位
	{
		ReadPara(CHARG_ALARM_AUTO_RST_EN) = Func_0x06_Data[WriteStartAdress];			
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 7 -1))			// ID23  电机最高转速RPM
	{
		ReadPara(MAX_V) = Func_0x06_Data[WriteStartAdress];
		if (ExCANopenRxFullFlag == 0)		
		{		
			UpSysPara(0x80, MAX_V,ReadPara(MAX_V));	
		}			
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 8 -1))			// ID321 制动器开关：0=制动器与使能联动；1=制动器关，2=制动器开
	{
		ReadPara(BK_SWITCH) = Func_0x06_Data[WriteStartAdress];		
		if (ExCANopenRxFullFlag == 0)		
		{		
			UpSysPara(0x80, BK_SWITCH,ReadPara(BK_SWITCH));		
		}			
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 9 -1))			// 
	{
		
	}	
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 10 -1))			// ID477 驱动器IGBT过热2（>95°）
	{
		ReadPara(477) = Func_0x06_Data[WriteStartAdress];
		ReadPara(IGBT_SAFE_TEMP) = Func_0x06_Data[WriteStartAdress];
		if (ExCANopenRxFullFlag == 0)		
		{		
			UpSysPara(0x80, IGBT_SAFE_TEMP,ReadPara(IGBT_SAFE_TEMP));			
		}
	}		
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 11 -1))			// ID449 驱动器母线欠压保护点
	{
		ReadPara(449) = Func_0x06_Data[WriteStartAdress];	
		ReadPara(LOW_VOL) = Func_0x06_Data[WriteStartAdress];		
		if (ExCANopenRxFullFlag == 0)		
		{
			UpSysPara(0x80, LOW_VOL,ReadPara(LOW_VOL));			
		}
	}	
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 12 -1))			// ID442 驱动器过压保护点
	{
		ReadPara(442) = Func_0x06_Data[WriteStartAdress];	
		ReadPara(OVER1_VOL) = Func_0x06_Data[WriteStartAdress];		
		if (ExCANopenRxFullFlag == 0)		
		{
			UpSysPara(0x80, OVER1_VOL,ReadPara(OVER1_VOL));			
		}
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 13 -1))			// 充电电压设定,精度1V
	{
		ReadPara(CHARG_V_SET) = Func_0x06_Data[WriteStartAdress];  								
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 14 -1))			// 充电电流设定,精度1A	
	{
        ReadPara(CHARG_I_SET) = Func_0x06_Data[WriteStartAdress] * 100;  							
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 15 -1))			// ErrSave	
	{
        ReadPara(ERRSAVE_ENABLE) = Func_0x06_Data[WriteStartAdress];  							
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 26 -1))			
	{
		//Pset_Step_DelaySet = Func_0x06_Data[WriteStartAdress];				// 位置给定延时
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 27 -1))			
	{
		if (Func_0x06_Data[WriteStartAdress] == 1)								// 1：模式设置为位置环、使能命令开启
		{
			RunMode_Test = 1;
			Runrequest_Test = 1;													
		}	
		else																	// 0：模式设置为速度环、使能命令关闭
		{
			RunMode_Test = 0;
			Runrequest_Test = 0;
		}
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 28 -1))			
	{
		Target_position_Test = Func_0x06_Data[WriteStartAdress];				// 位置给定，200代表2.00°
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 29 -1))			
	{
		Velocity_Limit_Test = Func_0x06_Data[WriteStartAdress];					// 位置环下的速度限制，200代表2.00°/S
	}
	else if (WriteStartAdress == (REGISTER_WRITE_START_ADRESS + 30 -1))			
	{
		if (Func_0x06_Data[WriteStartAdress] == 21930)							// 保存参数命令0x55AA=21930
		{
			//if(SaveAgainDelay == 0)											
			{
				SaveMode = 1;
			}
		}
	}
}

void ModbusFrameProcess(void)
{	
	u16 i = 0;
	u16 j = 0;
	u16 k = 0;
	
	if(ExModbusRxFullFlag == 0)         											//接收到Modbus发送过来的数据
	{
		return;
	}
		
	if ((ExUart_RxBuf[RS485_MB_ADDR] == ModbusAddr) || (ExUart_RxBuf[RS485_MB_ADDR] == BroadcastAddr) || (ExUart_RxBuf[0] == BroadcastErrAddr))	//ExUart_RxBuf[0] = 地址
	{
		FuncCode = ExUart_RxBuf[RS485_MB_FUNC];																//ExUart_RxBuf[1] = 功能码
		
		if ((FuncCode == MODBUS_FUNC_0X03) || (FuncCode == MODBUS_FUNC_0X06) || (FuncCode == MODBUS_FUNC_0X10))
		{
			#ifndef TEST_FOR_STARS													//未定义星辰厂内测试版
				HMI_TimeOutCount = 0;
				HMI_ConnectFlag = 1;				
			#else
			#endif	
		}
		
		switch (FuncCode)
		{
			case MODBUS_FUNC_0X03:	//连续地址读		
			if (MODBUS_CRC16(ExUart_RxBuf,6) == (ExUart_RxBuf[6] + ExUart_RxBuf[7] * 256))
			{
				ReadStartAdress = ExUart_RxBuf[RS485_MB_FUNC + 1] * 256 + ExUart_RxBuf[RS485_MB_FUNC + 2];	//起始地址
				ReadNumber = ExUart_RxBuf[RS485_MB_FUNC + 3] * 256 + ExUart_RxBuf[RS485_MB_FUNC + 4];		//读数据的数量			
				ReadEndAdress = ReadStartAdress + ReadNumber - 1;											//结束地址
				TxNumber = RS485_MB_FUNC + 2;																//[1+2]=[3]
			 
				if ((ReadNumber >= 0x01) && (ReadNumber <= 0x7D))											//数据长度最大为0x7D=125（数据字节数最大=125*2=250）
				{
					if ((ExUart_RxBuf[RS485_MB_ADDR] == ModbusAddr) || (ExUart_RxBuf[RS485_MB_ADDR] == BroadcastAddr) || (ExUart_RxBuf[RS485_MB_ADDR] == BroadcastErrAddr))
					{							
						if ((ExUart_RxBuf[RS485_MB_ADDR + 3] == FAULT_CODE_START_ADRESS) && (ExUart_RxBuf[RS485_MB_ADDR + 5] == FAULT_CODE_NUM)) // 读历史故障：起始地址=102&&寄存器数量=56
						{													
							/* 
							ErrRecordDataArray[0] = 0x22xx		//代表：22年、预留
							ErrRecordDataArray[1] = 0x2101		//代表：21日、1月
							ErrRecordDataArray[2] = 0x1005		//代表：10点、周5
							ErrRecordDataArray[3] = 0x0208		//代表：2秒、8分
							ErrRecordDataArray[4] = 0x9300		//代表：故障码
							...
							ErrRecordDataArray[28] = 0x7F00		//代表：驱动柜温度							
							ErrRecordDataArray[29] = 0x5555		//校验
							
							驱动器回复上位机的历史故障信息数据格式如下(以下为16进制数，北京时间特殊处理)：							
							[0B] [03]   [46]   [0022]  [0121]  [0510]  [0802]  [9300]......[7F00]    [0000 0000 0000 0000 0000 0000]  [2626]
							地址 功能码 字节数 序号22年 1月21日 周510点  8分2秒  故障码	  驱动柜温度	   预留 预留 预留 预留 预留 预留	    CRC						
							*/						
// 							if (ErrRecordDataNum == 0)
// 							{
// 								Rs485ModbusSndEn_Temp = 1;									// 需等到SD卡历史故障数据读出后再回复
// 								return;
// 							}
							
							if (SdCardErrDataCnt == 0)
							{
								Rs485ModbusSnd[RS485_MB_FUNC + 1] = 0x0E;					// NoFaults=0x0E	
								FlashErrFlag = 1;
							}
							
							if (ErrRecordDataNum == 0)
							{
								ReadErrRecord();											// 读取历史故障数据 
							}					
							
							if (FlashErrFlag)													// Flash有故障
							{							
								ErrRecordDataNum = 0;																			
								Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;						// 从机地址
								Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode + 0x80;				// 功能码+0x80								
								Crc = MODBUS_CRC16(Rs485ModbusSnd,3);
								Rs485ModbusSnd[3] = (u8)Crc;									// CRCL
								Rs485ModbusSnd[4] = Crc >> 8;									// CRCH					
								Rs485ModbusSndLen = 5;
								Rs485ModbusSndEn = 1;											// 发送标志为1		
							}						
							else																// Flash正常									
							{
								Rs485ModbusSnd[TxNumber] = ErrRecordDataNum;										// 历史故障序号
								Rs485ModbusSnd[TxNumber + 1] = ErrRecordDataArray[0 + ErrRecordDataNum * 30] / 256;	// 年
								TxNumber = TxNumber + 2;
																
								for (i = 1; i <= (ErrDataLen - 6); i++) 											// 35-6=29
								{
									Rs485ModbusSnd[TxNumber] = (u8)ErrRecordDataArray[i + ErrRecordDataNum * 30]; 	// 注意：历史故障数据低8位在前，高8位在后
									Rs485ModbusSnd[TxNumber + 1] = ErrRecordDataArray[i + ErrRecordDataNum * 30] / 256;
									TxNumber = TxNumber + 2;
								}
								//最后预留的6个字不做处理
								
								ErrRecordDataNum_Temp = SdCardErrDataCnt;							
								
								if (ErrRecordDataNum_Temp > GZM_Num)							// GZM_Num=20组（历史故障序号0~19）
								{
									ErrRecordDataNum_Temp = GZM_Num;
								}	
																
								if (ErrRecordDataNum < ErrRecordDataNum_Temp)							
								{
									ErrRecordDataNum ++;										// 按次序最多回复0~18共19组历史故障信息

									Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;					// 从机地址
									Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode;					// 功能码
									Rs485ModbusSnd[RS485_MB_FUNC + 1] = ErrDataLen * 2;			// 字节数					
									Crc = MODBUS_CRC16(Rs485ModbusSnd,(ErrDataLen * 2 + 3));
									Rs485ModbusSnd[ErrDataLen * 2 + 3] = (u8)Crc;				// CRCL
									Rs485ModbusSnd[ErrDataLen * 2 + 4] = Crc >> 8;				// CRCH
									Rs485ModbusSndLen = ErrDataLen * 2 + 5;
									Rs485ModbusSndEn = 1;										// 发送标志为1
								}
								else
								{
									ErrRecordDataNum = 0;										// 19组历史故障信息已发送完毕，发送停止命令：0B 83 0D A0 F7
									
									Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;					// 从机地址
									Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode + 0x80;			// 功能码+0x80
									Rs485ModbusSnd[RS485_MB_FUNC + 1] = 0x0D;					// FlashReadFaultFinish=0x0D
									Crc = MODBUS_CRC16(Rs485ModbusSnd,3);
									Rs485ModbusSnd[3] = (u8)Crc;								// CRCL
									Rs485ModbusSnd[4] = Crc >> 8;								// CRCH					
									Rs485ModbusSndLen = 5;
									Rs485ModbusSndEn = 1;										// 发送标志为1																
								}
							}
						}	
						else if ((ExUart_RxBuf[RS485_MB_ADDR + 3] == REGISTER_READ_START_ADRESS) && (ExUart_RxBuf[RS485_MB_ADDR + 5] == REGISTER_READ_NUM)) // 寄存器变量显示地址=200&&寄存器数量=30	
						{
							FlashErrFlag = 0;
							//ErrRecordDataNum = 0;
							
							#ifndef TEST_FOR_STARS												// 未定义星辰厂内测试版	
								TestReadDataHandle();					
								for (i = ReadStartAdress; i <= REGISTER_READ_END_ADRESS; i++) 	// 寄存器变量显示起始地址=200、寄存器数量=30、结束地址229
								{
								  //Rs485ModbusSnd[TxNumber] = TestReadArrary[i-200] / 256; 
									Rs485ModbusSnd[TxNumber] = (u8)(((u16)TestReadArrary[i-200]) / 256);
									Rs485ModbusSnd[TxNumber + 1] = (u8)TestReadArrary[i-200];
									TxNumber = TxNumber + 2;
								}
								
							#else
								Read_Func_0x03_Monitor_Data();	
								for (i = 1; i <= 30; i++) 										// 读地址200~229数据	
								{
									Rs485ModbusSnd[TxNumber] = Monitor_Data[i] / 256; 			
									Rs485ModbusSnd[TxNumber + 1] = (u8)Monitor_Data[i];
									TxNumber = TxNumber + 2;
								}								
							#endif																
								
							Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;						// 从机地址
							Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode;						// 功能码
							Rs485ModbusSnd[RS485_MB_FUNC + 1] = ReadDataLen * 2;			// 字节数=30*2(地址200~229)				
							Crc = MODBUS_CRC16(Rs485ModbusSnd,(ReadDataLen * 2 + 3));
							Rs485ModbusSnd[ReadDataLen * 2 + 3] = (u8)Crc;					// CRCL
							Rs485ModbusSnd[ReadDataLen * 2 + 4] = Crc >> 8;					// CRCH
							Rs485ModbusSndLen = ReadDataLen * 2 + 5;
							Rs485ModbusSndEn = 1;											// 发送标志为1
						}	
						else 	 // 正常数据轮询
						{
							FlashErrFlag = 0;
							ErrRecordDataNum = 0;
							
							#ifndef TEST_FOR_STARS											// 未定义星辰厂内测试版						
								HMI_Year 	= ReadPara(RTC_YEAR_FEB) + 2000;				// 系统时间：年
								HMI_Month 	= ReadPara(RTC_MONTH_AND_DATA_FEB) / 100;		// 系统时间：月
								HMI_Day		= ReadPara(RTC_MONTH_AND_DATA_FEB) % 100;		// 系统时间：日
								HMI_Hour 	= ReadPara(RTC_HOUR_AND_MINIT_FEB) / 100;		// 系统时间：时
								HMI_Min   	= ReadPara(RTC_HOUR_AND_MINIT_FEB) % 100;		// 系统时间：分
								HMI_Second  = ReadPara(RTC_SECON_FEB);						// 系统时间：秒
							
								Mdb_Tx_data(); 
													
								for (i = ReadStartAdress; i <= BroadcastAddrLen; i++) 	
								{
								  //Rs485ModbusSnd[TxNumber] = Tx_image[i].all / 256; 
									Rs485ModbusSnd[TxNumber] = (u8)(((u16)Tx_image[i].all) / 256);
									Rs485ModbusSnd[TxNumber + 1] = (u8)Tx_image[i].all;
									TxNumber = TxNumber + 2;
								}	
							#else
								Read_Func_0x03_Data();	
								for (i = ReadStartAdress; i <= BroadcastAddrLen; i++) 	
								{
									Rs485ModbusSnd[TxNumber] = Func_0x03_Data[i] / 256; 	// 注意：正常轮询数据高8位在前，低8位在后
									Rs485ModbusSnd[TxNumber + 1] = (u8)Func_0x03_Data[i];
									TxNumber = TxNumber + 2;
								}								
							#endif																
								
							Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;						// 从机地址
							Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode;						// 功能码
							Rs485ModbusSnd[RS485_MB_FUNC + 1] = BroadcastAddrLen * 2;		// 字节数					
							Crc = MODBUS_CRC16(Rs485ModbusSnd,(BroadcastAddrLen * 2 + 3));
							Rs485ModbusSnd[BroadcastAddrLen * 2 + 3] = (u8)Crc;				// CRCL
							Rs485ModbusSnd[BroadcastAddrLen * 2 + 4] = Crc >> 8;			// CRCH
							Rs485ModbusSndLen = BroadcastAddrLen * 2 + 5;
							Rs485ModbusSndEn = 1;											// 发送标志为1
						}						
					}
					else
					{
						Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;					//从机地址
						Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode + 0x80;			//差错码=功能码+0x80
						Rs485ModbusSnd[RS485_MB_FUNC + 1] = 0x02;					//地址不符合异常码			
						Crc = MODBUS_CRC16(Rs485ModbusSnd,3);
						Rs485ModbusSnd[3] = (u8)Crc;								//CRCL
						Rs485ModbusSnd[4] = Crc >> 8;								//CRCH					
						Rs485ModbusSndLen = 5;
						Rs485ModbusSndEn = 1;										//发送标志为1
					}
				}
				else																//数据长度异常
				{
					Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;						//从机地址
					Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode + 0x80;				//差错码=功能码+0x80
					Rs485ModbusSnd[RS485_MB_FUNC + 1] = 0x03;						//长度不符合异常码
					Crc = MODBUS_CRC16(Rs485ModbusSnd,3);
					Rs485ModbusSnd[3] = (u8)Crc;									//CRCL
					Rs485ModbusSnd[4] = Crc >> 8;									//CRCH	
					Rs485ModbusSndLen = 5;
					Rs485ModbusSndEn = 1;											//发送标志为1
				}	
				
				ExModbusRxFullFlag = 0;
			}	
			else
			{
				ExModbusRxFullFlag = 0;
				ModbusFunc_0x03_Crc_Err++;
			}
			break;
			
			case MODBUS_FUNC_0X06:	//单个地址写		
			FlashErrFlag = 0;
			//ErrRecordDataNum = 0;
			if (MODBUS_CRC16(ExUart_RxBuf,6) == (ExUart_RxBuf[6] + ExUart_RxBuf[7] * 256))
			{
				WriteStartAdress = ExUart_RxBuf[RS485_MB_FUNC + 1] * 256 + ExUart_RxBuf[RS485_MB_FUNC + 2];
			 
				if (((ExUart_RxBuf[5] + ExUart_RxBuf[4] * 256) >= 0x00) && ((ExUart_RxBuf[5] + ExUart_RxBuf[4] * 256) <= 0xFFFF))
				{
					if ((WriteStartAdress >= WRITE_START_ADRESS) && (WriteStartAdress <= WRITE_END_ADRESS))
					{	
						#ifndef TEST_FOR_STARS										//未定义星辰厂内测试版
							Rx_image[WriteStartAdress].all = ExUart_RxBuf[4] * 256 + ExUart_RxBuf[5];
						
							if (WriteStartAdress == MOTORTYPE_SELECT_ADRESS)
							{
								G_MotorTypeSelcectFlag = TRUE;
							}
							else if (WriteStartAdress == FAULT_CLEAR_ADRESS)
							{
								if (Rx_image[FAULT_CLEAR_ADRESS].all == FAULT_CLEAR_CMD)//HMI清除历史故障指令0x55AA
								{
									ClrCMDStart = TRUE;
									ClrFaultFlag = 1;																												
								}
							}
							else if ((WriteStartAdress >= REGISTER_WRITE_START_ADRESS) && (WriteStartAdress <= REGISTER_WRITE_END_ADRESS))//地址300~329为寄存器变量修改操作
							{							
								TestWriteDataHandle();								
							}
						#else
							Func_0x06_Data[WriteStartAdress] = ExUart_RxBuf[4] * 256 + ExUart_RxBuf[5];	//字节4、5为写单个地址的数据值
							if (WriteStartAdress == FAULT_CLEAR_ADRESS)			
							{
								if (Func_0x06_Data[FAULT_CLEAR_ADRESS] == FAULT_CLEAR_CMD)	//HMI清除历史故障指令0x55AA
								{
									ClrFaultFlag = 1;										
								}
							}		
							else if ((WriteStartAdress >= REGISTER_WRITE_START_ADRESS) && (WriteStartAdress <= REGISTER_WRITE_END_ADRESS))//地址300~329为寄存器变量修改操作
							{
								Write_Func_0x06_Data();								// HMI数据监控菜单-->地址300~329寄存器状态值修改 
							}							
						#endif	

						Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;					//从机地址
						Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode;					//功能码
						Rs485ModbusSnd[RS485_MB_FUNC + 1] = ExUart_RxBuf[2];
						Rs485ModbusSnd[RS485_MB_FUNC + 2] = ExUart_RxBuf[3];
						Rs485ModbusSnd[RS485_MB_FUNC + 3] = ExUart_RxBuf[4];
						Rs485ModbusSnd[RS485_MB_FUNC + 4] = ExUart_RxBuf[5];			
						Crc = MODBUS_CRC16(Rs485ModbusSnd,6);
						Rs485ModbusSnd[RS485_MB_FUNC + 5] = (u8)Crc;				//CRCL
						Rs485ModbusSnd[RS485_MB_FUNC + 6] = Crc >> 8;				//CRCH						
						Rs485ModbusSndLen = 8;
						Rs485ModbusSndEn = 1;										//发送标志为1
					}
					else
					{
						Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;					//从机地址
						Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode + 0x90;			//差错码=功能码+0x90						
						Rs485ModbusSnd[RS485_MB_FUNC + 1] = 0x02;					//地址不符合异常码			
						Crc = MODBUS_CRC16(Rs485ModbusSnd,3);
						Rs485ModbusSnd[3] = (u8)Crc;								//CRCL
						Rs485ModbusSnd[4] = Crc >> 8;								//CRCH	
						Rs485ModbusSndLen = 5;
						Rs485ModbusSndEn = 1;										//发送标志为1
					}
				}
				else																//寄存器值异常
				{
					Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;						//从机地址
					Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode + 0x90;				//差错码=功能码+0x90
					Rs485ModbusSnd[RS485_MB_FUNC + 1] = 0x03;						//长度不符合异常码
					Crc = MODBUS_CRC16(Rs485ModbusSnd,3);
					Rs485ModbusSnd[3] = (u8)Crc;									//CRCL
					Rs485ModbusSnd[4] = Crc >> 8;									//CRCH	
					Rs485ModbusSndLen = 5;
					Rs485ModbusSndEn = 1;											//发送标志为1
				}	
				
				ExModbusRxFullFlag = 0;
			}	
			else
			{
				ExModbusRxFullFlag = 0;
				ModbusFunc_0x06_Crc_Err++;
			}			
			break;
			
			case MODBUS_FUNC_0X10:	//连续地址写	
			FlashErrFlag = 0;				
			//ErrRecordDataNum = 0;
			if (MODBUS_CRC16(ExUart_RxBuf,(7+ExUart_RxBuf[6])) == (ExUart_RxBuf[7+ExUart_RxBuf[6]] + ExUart_RxBuf[7+ExUart_RxBuf[6]+1] * 256))
			{
				WriteStartAdress = ExUart_RxBuf[RS485_MB_FUNC + 1] * 256 + ExUart_RxBuf[RS485_MB_FUNC + 2];	//起始地址
				WriteNumber = ExUart_RxBuf[RS485_MB_FUNC + 3] * 256 + ExUart_RxBuf[RS485_MB_FUNC + 4];		//数据的数量			
				WriteEndAdress = WriteStartAdress + WriteNumber - 1;										//结束地址
				TxNumber = 7;																				//[1+2]=[3]
			 
				if ((WriteNumber >= 0x01) && (WriteNumber <= 0x7B))											//数据长度最大为0x7B=123（数据字节数最大=123*2=246）
				{
					if ((WriteStartAdress >= WRITE_START_ADRESS) && (WriteEndAdress <= WRITE_END_ADRESS)) 	//索引地址
					{
						for (i = WriteStartAdress; i <= WriteEndAdress; i++)  
						{
							#ifndef TEST_FOR_STARS															//未定义星辰厂内测试版
								Rx_image[i].all = ExUart_RxBuf[TxNumber] * 256 + ExUart_RxBuf[TxNumber+1];
							#else
								Func_0x10_Data[i] = ExUart_RxBuf[TxNumber] * 256 + ExUart_RxBuf[TxNumber+1];	
							#endif

							TxNumber = TxNumber + 2;
						}		
						
						if (WriteStartAdress == TIME_ADJUST_ADRESS)					//地址30~35为北京时间校准参数
						{
							Time_Adjust_Flag = 1;					
						}
						
						#ifndef TEST_FOR_STARS		
							if (WriteStartAdress == DRIVE_ACTIVE_ADDRESS)        	//地址38为驱动器激活码
							{
								G_HardWareEncodeFlag = TRUE;
							}						
						#else
						#endif
						
						Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;					//从机地址
						Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode;					//功能码
						Rs485ModbusSnd[RS485_MB_FUNC + 1] = ExUart_RxBuf[2];
						Rs485ModbusSnd[RS485_MB_FUNC + 2] = ExUart_RxBuf[3];
						Rs485ModbusSnd[RS485_MB_FUNC + 3] = ExUart_RxBuf[4];
						Rs485ModbusSnd[RS485_MB_FUNC + 4] = ExUart_RxBuf[5];			
						Crc = MODBUS_CRC16(Rs485ModbusSnd,6);
						Rs485ModbusSnd[RS485_MB_FUNC + 5] = (u8)Crc;				//CRCL
						Rs485ModbusSnd[RS485_MB_FUNC + 6] = Crc >> 8;				//CRCH						
						Rs485ModbusSndLen = 8;
						Rs485ModbusSndEn = 1;										//发送标志为1
					}
					else
					{
						Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;					//从机地址
						Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode + 0x90;			//差错码=功能码+0x90						
						Rs485ModbusSnd[RS485_MB_FUNC + 1] = 0x02;					//地址不符合异常码			
						Crc = MODBUS_CRC16(Rs485ModbusSnd,3);
						Rs485ModbusSnd[3] = (u8)Crc;								//CRCL
						Rs485ModbusSnd[4] = Crc >> 8;								//CRCH	
						Rs485ModbusSndLen = 5;
						Rs485ModbusSndEn = 1;										//发送标志为1
					}
				}
				else																//数据长度异常
				{
					Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;						//从机地址
					Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode + 0x90;				//差错码=功能码+0x90
					Rs485ModbusSnd[RS485_MB_FUNC + 1] = 0x03;						//长度不符合异常码
					Crc = MODBUS_CRC16(Rs485ModbusSnd,3);
					Rs485ModbusSnd[3] = (u8)Crc;									//CRCL
					Rs485ModbusSnd[4] = Crc >> 8;									//CRCH	
					Rs485ModbusSndLen = 5;
					Rs485ModbusSndEn = 1;											//发送标志为1
				}	
				
				ExModbusRxFullFlag = 0;
			}	
			else
			{
				ExModbusRxFullFlag = 0;
				ModbusFunc_0x10_Crc_Err++;
			}			
			break;
			
			default: 
			FlashErrFlag = 0;
			//ErrRecordDataNum = 0;
			break;
		}
	}
}

void ModbusTrans(void)							// 调用周期：300us
{
	static u16 SndDly = 0; 						// 发送延时计数器；加一定延时可避免HMI未做好接收准备，驱动器已发出数据的情况

	if (Rs485ModbusSndEn == 1)
	{
		RS485_RDE1_TX;	
		SndDly ++;
		if (SndDly == 7) 						// 发送延时：300us*7~=2ms
		{
			Rs485ModbusSndEn = 0;				
			DMA_Cmd(DMA2_Stream7, DISABLE);
			DMA2_Stream7->NDTR = Rs485ModbusSndLen;	
			DMA_Cmd(DMA2_Stream7, ENABLE); 		// 发送前重新初始化DMA确保DMA传输目的起始地址为	
			SndDly = 0;
		}
	}
// 	else if (Rs485ModbusSndEn_Temp == 2)											// 需等到读出SD中历史故障完成，才能回复HMI第0组故障
// 	{
// 		u16 i = 0;
// 		u16 TxNumber = RS485_MB_FUNC + 2;											// [1+2]=[3]
// 		
// 		ErrRecordDataNum = 0;
// 		Rs485ModbusSnd[TxNumber] = ErrRecordDataNum;								// 历史故障序号
// 		Rs485ModbusSnd[TxNumber + 1] = ErrRecordDataArray[ErrRecordDataNum] / 256;	// 年
// 		TxNumber = TxNumber + 2;
// 										
// 		for (i = 1; i <= (ErrDataLen - 6); i++) 									// 35-6=29
// 		{
// 			Rs485ModbusSnd[TxNumber] = (u8)ErrRecordDataArray[i + ErrRecordDataNum * 30];
// 			Rs485ModbusSnd[TxNumber + 1] = ErrRecordDataArray[i + ErrRecordDataNum * 30] / 256;
// 			TxNumber = TxNumber + 2;
// 		}
// 		//最后预留的6个字不做处理
// 		
// 		ErrRecordDataNum ++;														// 按次序最多回复0~18共19组历史故障信息

// 		Rs485ModbusSnd[RS485_MB_ADDR] = ModbusAddr;									// 从机地址
// 		Rs485ModbusSnd[RS485_MB_FUNC] = FuncCode;									// 功能码
// 		Rs485ModbusSnd[RS485_MB_FUNC + 1] = ErrDataLen * 2;							// 字节数					
// 		Crc = MODBUS_CRC16(Rs485ModbusSnd,(ErrDataLen * 2 + 3));
// 		Rs485ModbusSnd[ErrDataLen * 2 + 3] = (u8)Crc;								// CRCL
// 		Rs485ModbusSnd[ErrDataLen * 2 + 4] = Crc >> 8;								// CRCH
// 		Rs485ModbusSndLen = ErrDataLen * 2 + 5;
// 		Rs485ModbusSndEn = 1;														// 发送标志为1
// 		Rs485ModbusSndEn_Temp = 0;
// 	}
	else
	{
		SndDly = 0;
	}
}

void ExUartFrameProcess(void)
{
    u16 	Data;
    u16     CrcData = 0;
    union	EXUART_COMMAND Command;

    if (ExUartRxFullFlag)            			// 接收到USART1发送过来的数据
    {
        pExRxBuf     = ExUart_RxBuf;       		// 通过USART1接收上位机数据
        ExRxFullFlag = ExUartRxFullFlag;   		// 置ARM一帧数据接收完成标志
        CommuType    = USART_1;         		// 置通讯方式
        TxToPCCompleteFlag = 0;            		// 清除发送完成标志
    }
    else if (ExCANRxFullFlag)              		// 接收到CAN发送过来的数据
    {
        pExRxBuf     = ExCAN_RxBuf;       		// 通过CAN接收上位机数据
        ExRxFullFlag = ExCANRxFullFlag;   		// 置ARM一帧数据接收完成标志
        CommuType    = CAN232;            		// 置通讯方式
    }
    else if (ExCANopenRxFullFlag)          		// 接收到CANopen发送过来的数据
    {
        if (TxToPCCompleteFlag == 1)        	// ARM与PC通信一帧完成
        {
            pExRxBuf     = ExCANopen_RxBuf;     // 通过CANopen接收上位机数据
            ExRxFullFlag = ExCANopenRxFullFlag; // 置ARM一帧数据接收完成标志
            CommuType    = CANopen;             // 置通讯方式
        }
    }
    else if (ExUart6RxFullFlag)            		// 接收到USART6发送过来的数据
    {
        pExRxBuf     = ExUart_RxBuf;       		// 通过USART6接收上位机数据
        ExRxFullFlag = ExUart6RxFullFlag;   	// 置ARM一帧数据接收完成标志
        CommuType    = USART_6;         		// 置通讯方式
        TxToPCCompleteFlag = 0;            		// 清除发送完成标志
    }

    if ((ExRxFullFlag == 1) && (DSPReturnPCFlag == 1))
    {
        ExRxFullFlag        = 0;

        Command.all = pExRxBuf[1] * 256 + pExRxBuf[2]; 	// ExUart_RxBuf[1] * 256 + ExUart_RxBuf[2];
        Data = pExRxBuf[3] * 256 + pExRxBuf[4]; 		// ExUart_RxBuf[3] * 256 + ExUart_RxBuf[4];
        CrcData = pExRxBuf[5] * 256 + pExRxBuf[6]; 		// 读取CRC校验数据

        if(CRC16(pExRxBuf, EXUART_TX_DATA_FRAMELEN-2) == CrcData)
        {
            if (pExRxBuf[0] == 201) 					// 通讯地址
            {
                DSP_ComSta = 0x11;
                DSP_ComCMD = Command.all;
                DSP_ComData = Data;
                Charge_Ctrl_Flag = 1;
                ExUartReturnFlag = 0;
            }
            else
            {
                ExUartReturnFlag = 1;					 	//1 需要返回帧到上位机

                if( Command.bit.CONTENT == 0 )			 	//读写参数处理
                {
                    Data = 	ParaProcess(Command.bit.RWBIT,Command.bit.PARAID,Data);
					DownLoadToDsp_Judge (Command.bit.PARAID);	
					
                    if((Command.bit.PARAID == COMM_BDR) && (Command.bit.RWBIT))	//更改波特率无需返回
                    {
                        ExUartReturnFlag = 0;				//0 不需要返回帧到上位机
                        DSPReturnPCFlag = 1; 			 	//置DSP已返回PC数据标志
                    }
					else if (((Command.bit.PARAID < DSP_PARA_LEN) && (Command.bit.RWBIT)) ||  	// 需发送给DSP的参数	DSP_PARA_LEN = 429					
							 ((Command.bit.PARAID < DSP_PARA_LEN) && (DownLoadToDsp)))
                    {
                        UpdateInUartTxFrame(Command.all,Data);	// 更新发往DSP的数据帧
                        PendParaID = Command.bit.PARAID;
                        ReturnSelect = 1;	                    // 需要从DSP返回数据
                        DSPReturnPCFlag = 0; 					// 清DSP已返回数据标志
                    }
                    else                                        // 不需发送给DSP的参数
                    {
                        ReturnSelect = 0;	                    // 不需要从DSP返回数据
                        ExUartTxFrameAsm(Data);                 // 将上位机发送的数据返回给上位机
                        DSPReturnPCFlag = 1; 					// 置DSP已返回PC数据标志
                    }
                }
                else if(Command.bit.RWBIT == 1)					//写控制命令
                {

                    ExUartTxFrameAsm(0);    					// 组装返回帧

                    switch(Command.bit.PARAID)
                    {
                    case 1:									//设定命令
                    {
                        PcCtrlSet = 1;
                        PcSet = (s16)Data;
                        ExUartTxFrameAsm(Data);   			// 组装返回帧
                        if(CommuType != CANopen)            // 接收到RS485发送过来的写控制命令
                        {
                            ReadPara(CAN_SEL) = 0; 			// 禁用CANopen
                        }

                        break;
                    }

                    case 2:									//速传命令
                    {
                        PcCtrlSet = 1;
                        PcSet = (s16)Data;
                        //ExUartReturnFlag = 0;				// 不需要返回帧
                        if(CommuType != CANopen)            // 接收到RS485发送过来的写控制命令
                        {
                            ReadPara(CAN_SEL) = 0; 			// 禁用CANopen
                        }

                        break;
                    }

                    case 3:									//开使能命令
                    {
                        if(Data == 0xAA55)
                        {
                            PcCtrl = 1;
                            PcCtrlFlag = 1;
                            PcCtrlSet = 1;
                            //ExUartTxFrameAsm(0);   				// 组装返回帧
                            if(CommuType != CANopen)        		// 接收到RS485发送过来的写控制命令
                            {
                                ReadPara(CAN_SEL) = 0; 				// 禁用CANopen
                            }
 
                            break;
                        }
                    }

                    case 4:											//关使能命令
                    {
                        if(Data == 0x55AA)
                        {
                            PcCtrl = 1;
                            PcCtrlFlag = 0;
                            PcCtrlSet = 1;
                            //ExUartTxFrameAsm(0);    				// 组装返回帧
                            if(CommuType != CANopen)            	// 接收到RS485发送过来的写控制命令
                            {
                                ReadPara(CAN_SEL) = 0; 				// 禁用CANopen
                            }

                            break;
                        }
                    }

                    case 5:										// 保存命令
                    {
                        switch (Data)
                        {
                        case 0xa151:  							// 保存用户参数
                        {
                            SaveMode = 1;						// 0 无需保存 1 需要保存用户参数 2 需要保存用户参数和出厂默认参数
                            WriteChargPara(0xC005, 0xA151);
                            break;
                        }
                        case 0xa152:  							// 保存用户参数和出厂参数
                        {
                            SaveMode = 2;						// 0 无需保存 1 需要保存用户参数 2 需要保存用户参数和出厂默认参数
                            break;
                        }
                        case 0xa153:  							// 调出厂参数到内存
                        {
                            LoadDefaultParaFlag = 1; 			// 置调出厂参数标志
                            break;
                        }
                        case 0xa154:  							// 清除所有历史故障
                        {
                            ExUartTxFrameAsm(1);    			// 成功返回1
                            break;
                        }
                        case 0xa156:  							// 调出历史故障到内存
                        {
                            ExUartTxFrameAsm(1);   		 		// 成功返回1
                            break;
                        }
                        case 0xa157:  							// 读调出历史故障到内存完成标志
                        {
                            ExUartTxFrameAsm(ReadErrRecordOK);  // 组装返回完成标志
                            break;
                        }
                        default:
                        {
                            break;
                        }
                        }
                        break;
                    }

                    case 6:										//切换调试模式命令
                    {

                        switch (Data)
                        {
                        case 0xaa55:							//进入调试模式
                        {
                            AdjModFlag=1;						//调试模式标志
                            break;
                        }
                        case 0x55aa:							//退出调试模式
                        {
                            AdjModFlag=0;						//调试模式标志
                            break;
                        }
                        }
                        break;
                    }

                    case 7:
                    {
						switch(Data)
						{
							case 0xAA55:
							{							
								UpdataFlag = 1; 					
								StmStatus.bit.IO_IN0 = 1;		//置位StmStatus.bit.IO_IN0和SET_WKO并不能锁住看门狗，
								ExUartTxFrameAsm(Data);
								ReturnSelect = 0;	            //不需要从DSP返回数据
								USART1_DMA2_Tx_Configuration_STARTS_IAP();
								break;
							}
							
							case 0x55AA:
							{
								if (UpdataFlag == 1)
								{             
									UpdataFlag =0;
									IAP_Flag = 0;
									//Jump_bootloader(); 
									CANopen_Jump_bootloader();
								}
								else
								{
									UpdataFlag = 0;
									IAP_Flag = 0;
									ExUartTxFrameAsm(0); 
								}
								break;
							}               
						}

						break;
                    }

                    case 8:										//切换参数/属性配置 命令
                    {
                        break;
                    }

                    case 9:										//取消通信控制
                    {
                        if(Data == 0xAA12)
                        {
                            PcCtrl = 0;
                            PcCtrlFlag = 0;
                            PcCtrlSet = 0;

                            if (CANAddr == 0)
                            {
                                WritePara(CAN_SEL, 0);
                            }
                            else
                            {
                                WritePara(CAN_SEL, 1);
                            }

                            break;
                        }
                    }

                    case 10:
                    {
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
                else
                {
                    ExUartReturnFlag = 0;     		// 读控制命令,不需要返回帧到上位机
                }
            }
        }
        else
        {
            ExUartRxLoc = 0;  						// 复位指针
        }

        if(ExUartReturnFlag == 0)					// 0不需要返回帧到上位机
        {
            ExUartRxLoc = 0;
        }

        /* 待数据处理完再清除接收完成标志 */
        if(CommuType == USART_1)            		// 接收到RS485发送过来的数据
        {
            ExUartRxFullFlag = 0;		   			// 清除RS485接收完成标志
        }
        else if (CommuType == CAN232)           	// 接收到CAN发送过来的数据
        {
            ExCANRxFullFlag = 0;		 	 		// 清除CAN232接收完成标志
        }
        else if(CommuType == CANopen)        		// 接收到CANopen发送过来的数据
        {
            ExCANopenRxFullFlag = 0;			 	// 清除CANopen接收完成标志
        }
        else if(CommuType == USART_6)            	// 接收到RS485发送过来的数据
        {
            ExUart6RxFullFlag = 0;		   			// 清除RS485接收完成标志
        }
    }
}

void	ExUartFrameProcess1(void)
{	
	u16 	Data;
    u16     CrcData = 0;
	union	EXUART_COMMAND Command;  
	static u8 CANopenFullFlag = 0;
    
	if(ExCANopenRxFullFlag)           		// 接收到CANopen发送过来的数据
    {
        if (TxToPCCompleteFlag == 1)        	// ARM与PC通信一帧完成
        {
            pExRxBuf     = ExCANopen_RxBuf;     // 通过CANopen接收上位机数据
            CANopenFullFlag = ExCANopenRxFullFlag; // 置ARM一帧数据接收完成标志
        }
    }

	if((CANopenFullFlag == 1) && (DSPReturnPCFlag == 1)) 	// 接收到上位机发送的数据且DSP已返回了上一帧的数据才处理
	{
        CANopenFullFlag        = 0;
       
		Command.all = pExRxBuf[1] * 256 + pExRxBuf[2]; 	// ExUart_RxBuf[1] * 256 + ExUart_RxBuf[2];
		Data = pExRxBuf[3] * 256 + pExRxBuf[4]; 		// ExUart_RxBuf[3] * 256 + ExUart_RxBuf[4];
        CrcData = pExRxBuf[5] * 256 + pExRxBuf[6]; 		// 读取CRC校验数据
		
		if(CRC16(pExRxBuf, EXUART_TX_DATA_FRAMELEN-2) == CrcData)
		{              
			//ExUartReturnFlag = 0;	
			
			if (Command.bit.CONTENT == 0)			//读写参数处理
			{
				Data = 	ParaProcess(Command.bit.RWBIT,Command.bit.PARAID,Data);
				DownLoadToDsp_Judge (Command.bit.PARAID);
				
				if (((Command.bit.PARAID < DSP_PARA_LEN) && (Command.bit.RWBIT)) ||  	// 需发送给DSP的参数	DSP_PARA_LEN = 429					
					((Command.bit.PARAID < DSP_PARA_LEN) && (DownLoadToDsp)))
				{
					UpdateInUartTxFrame(Command.all,Data);	// 更新发往DSP的数据帧
					PendParaID = Command.bit.PARAID;
					ReturnSelect = 1;	                    // 需要从DSP返回数据
					DSPReturnPCFlag = 0; 					// 清DSP已返回数据标志
				}
				else                                        // 不需发送给DSP的参数
				{
					ReturnSelect = 0;	                    // 不需要从DSP返回数据
					//ExUartTxFrameAsm(Data);               // 将上位机发送的数据返回给上位机
					DSPReturnPCFlag = 1; 					// 置DSP已返回PC数据标志
				}
			}
			else if(Command.bit.RWBIT == 1)
			{	//写控制命令

				//ExUartTxFrameAsm(0);    // 组装返回帧

				switch(Command.bit.PARAID)
				{
					case 1://设定命令
					{   
						PcCtrlSet = 1;
						PcSet = (s16)Data;
						//ExUartTxFrameAsm(Data);   		// 组装返回帧
														
						break;	
					}
					case 2://速传命令
					{   
						PcCtrlSet = 1;
						PcSet = (s16)Data;
						//ExUartReturnFlag = 0;				// 不需要返回帧
													
						break;	
					}
					case 3://开使能命令
					{
						if(Data == 0xAA55)    
						{
							PcCtrl = 1;
							PcCtrlFlag = 1;
							PcCtrlSet = 1;
							//ExUartTxFrameAsm(0);   		// 组装返回帧

							break;
						}
					}
					case 4://关使能命令
					{
						if(Data == 0x55AA)  
						{
							PcCtrl = 1;
							PcCtrlFlag = 0;
							PcCtrlSet = 1;
							//ExUartTxFrameAsm(0);    		// 组装返回帧

							break;
						}
					}
					case 5://保存命令(改为数据发送完毕信号)
					{
						switch (Data)
						{
							case 0xa151:  							// 保存用户参数
							{
								SaveMode = 1;						// 0 无需保存 1 需要保存用户参数 2 需要保存用户参数和出厂默认参数							
								WriteChargPara(0xC005, 0xA151);								
								break;
							}
							case 0xa152:  							// 保存用户参数和出厂参数
							{
								SaveMode = 2;						// 0 无需保存 1 需要保存用户参数 2 需要保存用户参数和出厂默认参数
								break;
							}
							default:
							{
								break;
							}
						}
						break;
					}
					default:
					{
						break;
					}
				}
			}
			else
			{
				ExUartReturnFlag = 0;     							// 读控制命令，无返回
			}           
		}

		ExCANopenRxFullFlag = 0;			 						// 清除CANopen接收完成标志
	}
}

void	ExUartStartSend(void)						// 启动发送, 300us的周期
{
    static u16 DlyCnt1 = 0; 						// USART1延时计数
    static u16 DlyCnt6 = 0; 						// USART6延时计数

    if (CommuType == USART_1)
    {
		RS485_RDE1_TX;								// 控制485通信方向为发送
        TxToPCCompleteFlag = 0;                     // 置一帧发送中标志    
        DlyCnt6 = 0;
        DlyCnt1 ++;
		
		if (DlyCnt1 == 5) 							// 发送延时300us*5~=1.5ms(STARS铝壳RS485通讯模块需有一定延时)
        {
			DMA_Cmd(DMA2_Stream7, DISABLE);
			DMA2_Stream7->NDTR = EXUART_TX_DATA_FRAMELEN;	
			DMA_Cmd(DMA2_Stream7, ENABLE); 			// 发送前重新初始化DMA确保DMA传输目的起始地址为Uart1_TxBuf
			
            ExUartReturnFlag = 0;
            ReturnSelect = 0;
            DlyCnt1 = 0;
			
			if (DSP_ComSta == 0x12)
			{
				DSP_ComSta = 0;
			}
        }
    }
    else if (CommuType == CAN232)
    {
        CAN_TXFrame((u8*)(&ExUart_TxBuf));
        ExUartReturnFlag = 0;                      	// 标志复位
        ReturnSelect = 0;                          	// 标志复位
    }
    else if (CommuType == CANopen)
    {
        ExUartReturnFlag = 0;                      	// 标志复位
        ReturnSelect = 0;                          	// 标志复位
    }
    else if (CommuType == USART_6)
    {
		RS485_RDE6_TX;								// 控制485通信方向为发送
        TxToPCCompleteFlag = 0;                     // 置一帧发送中标志
        DlyCnt1 = 0;
        DlyCnt6 ++;
		
		if (DlyCnt6 == 5) 							// 发送延时300us*5~=1.5ms(STARS铝壳RS485通讯模块需有一定延时)
        {
            ExUartTxLoc = 1;
            USART_ITConfig(USART6, USART_IT_TXE, ENABLE);
            USART_SendData(USART6, ExUart_TxBuf[0]);
            ExUartReturnFlag = 0;
            ReturnSelect = 0;
            DlyCnt6 = 0;
			
			if (DSP_ComSta == 0x12)
			{
				DSP_ComSta = 0;
			}
        }
    }
	else
	{
        DlyCnt1 = 0;
        DlyCnt6 = 0;
	}
}

void	ExUartTrans(void)
{
    if (CommAddr == 0xFF)													// 广播帧不返回，清标志后退出
    {
        ExUartReturnFlag = 0;                      							// 标志复位
        ReturnSelect = 0;                          							// 标志复位		
        return;
    }

    if (DSP_ComSta == 0x12)
    {
        PwrTxFrameAsm(201, DSP_ComCMD, DSP_ComData);						// 组装数据
        ExUartStartSend();

		if (CommuType == CAN232)
		{
			DSP_ComSta = 0;
			ExUartReturnFlag = 0;
		}
    }

    //启动发送
    if(ExUartReturnFlag == 1)												// 1 需要返回帧到上位机
    {
		if(ReturnSelect == 1)												// 需要从DSP返回数据
		{
			if(IsWaitStatus() != 1)											// DSP已有返回帧
			{
				ExUartTxFrameAsm(ReadPara(PendParaID));
				ExUartStartSend();
			}
		}
		else
		{
			ExUartStartSend();
		}
    }
}

u16 GetSaveMode(void)														// 0 无需保存 1 需要保存用户参数 2 需要保存用户参数和出厂默认参数
{
    return(SaveMode);
}

void ResetSaveFlag(void)
{
    SaveMode = 0;
}

void ChangeCommAddr(unsigned int NewBDR)									// 更改通讯地址
{
    //CommAddr=NewBDR;
	CommAddr=1;
    if (CommAddr > 200) CommAddr = 200;
    WritePara(COMM_ADDR, CommAddr);
}

void ChangeCommBDR(unsigned int NewBDR)										// 更改波特率
{
    unsigned long i;
    USART_InitTypeDef USART_InitStructure;
	
	if (CANAddr == 0)
		NewBDR = 6;
	else
		NewBDR = 9;
	
	switch(NewBDR)
	//switch(9)
	{
		case 1:{i=1200;ExUartRxOverTime=100/SLOW_TIME_TICK+1;break;}		// 超时50ms+20ms
		case 2:{i=2400;ExUartRxOverTime=44/SLOW_TIME_TICK+1;break;}			// 超时24ms+20ms
		case 3:{i=4800;ExUartRxOverTime=32/SLOW_TIME_TICK+1;break;}			// 超时12ms+20ms
		case 4:{i=9600;ExUartRxOverTime=26/SLOW_TIME_TICK+1;break;}			// 超时6ms+20ms
		case 5:{i=14400;ExUartRxOverTime=24/SLOW_TIME_TICK+1;break;}		// 超时4ms+20ms
		case 6:{i=19200;ExUartRxOverTime=22/SLOW_TIME_TICK+1;break;}		// 超时2ms+20ms
		case 7:{i=38400;ExUartRxOverTime=20/SLOW_TIME_TICK+1;break;}		// 超时0ms+20ms
		case 8:{i=57600;ExUartRxOverTime=20/SLOW_TIME_TICK+1;break;}		// 超时0ms+20ms
		case 9:{i=115200;ExUartRxOverTime=20/SLOW_TIME_TICK+1;break;}		// 超时0ms+20ms		

		default:{i=19200;ExUartRxOverTime=22/SLOW_TIME_TICK+1;break;}		// 超时2ms+20ms
	}

    USART_InitStructure.USART_BaudRate = i;									// 新波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    /* 配置USART1 */ 
    USART_Init(USART1, &USART_InitStructure);
}

void ChangeCommBDR1(void)												
{
    unsigned long i;
    USART_InitTypeDef USART_InitStructure;

	if(CANAddr_Real == 0)					
	{
		Protocol = 0;														
		i=19200;
		ExUartRxOverTime=22/SLOW_TIME_TICK+1;
		//i=USART1BAURATE;
		//ExUartRxOverTime=20/SLOW_TIME_TICK+1;
	}
	else									
	{
		Protocol = 1;														
		i=USART1BAURATE;
		ExUartRxOverTime=20/SLOW_TIME_TICK+1;
	}

    USART_InitStructure.USART_BaudRate = i;									
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    /* 配置USART1 */ 
    USART_Init(USART1, &USART_InitStructure);
}

void ExUartOverHandel(void)
{
	if (DMA_GetCurrDataCounter(DMA2_Stream2) != USART1_DMA_DATA_LEN)		// DMA已接收到数据
	{
		if (SlowTimeBaseFlag.bit.EXUART == 1)								// 1ms时基
		{
			ExUartRxOverTimer++;
			SlowTimeBaseFlag.bit.EXUART = 0;
		}
		
		if (ExUartRxOverTimer > ExUartRxOverTime)							// 超时：ExUartRxOverTime=22/2+1=12ms
		{
			ExUartRxOverTimer = 0;
			RS485_RDE1_RX;
			RS485_RDE6_RX;	
		}
	}
	else
	{
		ExUartRxOverTimer = 0;
	}
	
	if (READ_RDE1 == 0)			
	{
		if (Tx1Cnt < 400)							//RDE1为低电平状态(TX状态)持续时间大于400*500us = 200ms，则强制切换到RX状态
		{
			Tx1Cnt ++;								//19200bps发送66*2Byte字节耗时约73ms，9600bps发送66*2Byte字节耗时约150ms？
		}
		else
		{
			RS485_RDE1_RX;
			Tx1Cnt = 0;
		}
	}
	else
	{
		Tx1Cnt=0;
	}
	
	if (READ_RDE6 == 0)			
	{
		if (Tx6Cnt < 400)							//RDE6为低电平状态(TX)持续时间大于400*500us = 200ms，则强制切换到RX状态
		{
			Tx6Cnt ++;
		}
		else
		{
			RS485_RDE6_RX;
			Tx6Cnt = 0;
		}
	}
	else
	{
		Tx6Cnt = 0;
	}
}

void ExUartDataProcess(void)						//数据帧处理
{
	CANAddr_Real = DIStatus.bit.DI20 * 8 + DIStatus.bit.DI21 * 4 + DIStatus.bit.DI22 * 2 + DIStatus.bit.DI23 * 1;
	
	if (CANAddr_Real != CANAddr_Real_Old)			//根据拨码地址选择不同波特率：0地址为星辰协议，19200bps；其它地址为Modebus协议，115200bps
	{
		ChangeCommBDR1();							//更改波特率	
		
		if (CANAddr_Real == 0)						//重新配置DMA（主要是重新配置内存地址、发送的数据长度）
		{
			USART1_DMA2_Tx_Configuration_STARTS();	 
		}
		else									
		{
			USART1_DMA2_Tx_Configuration_WINDEY();
		}
	}
	
	CANAddr_Real_Old = CANAddr_Real;
	
	if ((CANAddr_Real == 0) || (IAP_Flag == 1))					
	{
		Protocol = 0;			//星辰协议
		ExUartFrameProcess();	//星辰协议、CANOpen协议数据处理(上电前拨到非0地址，上电就绪后，拨码再拨回0，即可实现星辰上位机RS232与CANOpen同时通讯)							
		ExUartTrans();			//注意：当ExCANopenRxFullFlag=1，即接收到CANopen发送过来的数据时，即使无RS232通讯，也会进行串口发送ExUartTxFrameAsm()处理
	}
	else									
	{
		Protocol = 1;			//Modbus协议
		ModbusFrameProcess();	//Modbus通讯处理
		ModbusTrans();			//Modbus启动发送
		ExUartFrameProcess1();	//CANOpen通讯处理：此函数内已经屏蔽星辰协议发送函数ExUartTxFrameAsm()处理，否则会与Modbus协议发送有冲突
	}
}