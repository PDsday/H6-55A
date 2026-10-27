#ifndef	_EXTERNPROTOTYPE_
#define	_EXTERNPROTOTYPE_

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stdio.h"
#include "ParaProcess.h"
#include "ExterUart.H"
#include "CANopenParaPreocess.h"
#include "IO.h"
#include "TaskScheduler.h"
#include "adc_config.h"
#include "dac_config.h"
#include "PLCFunction.h"
#include "CANopen.h"
#include "can_STM32.h"
#include "store_STM32.h"
#include "FD_ObjDict.h"

#define VERSION_STARS 261027		//InterUart中增加SAFE_CLOSE_SPEED(ID313:DSP自主收桨速度)的下传

//#define TEST_FOR_STARS
#define PARA_SECTOR_SELECT_16K			// 定义ARM自带FLASH用户参数和出厂参数扇区为16K,否则为原定义的128K
	
#ifdef CAN_GLOBALS
    #define  CAN_EXT
#else
    #define  CAN_EXT   extern
#endif

CAN_EXT u16 CANopen_EN;

#define CPU_CRITICAL_ENTER()        {cpu_sr = CPU_SR_Save();}
#define CPU_CRITICAL_EXIT()         {CPU_SR_Restore(cpu_sr);}

#define ReadPara(ParaID)			SysParaTable[ParaID].DATA			//读驱动器参数
#define WritePara(ParaID,NewPara)	SysParaTable[ParaID].DATA=NewPara	//写驱动器参数
#define ReadPara2(ParaID)			SysParaTable2[ParaID].DATA			//读充电器参数

#define	SYS_PARA_LEN_STARS				999					// 参数数量
#define	SYS_PARA_LEN_MAX				1420				// SysParaTable数组最大参数数量：0~429为ARM与DSP共用参数号、430~999为ARM用普通参数号、1000~1299为ARM用历史故障信息参数号、1300~1419为自学习用参数号
#define	SYS_PARA_AUTOSTUDY				1300				// 1300为运达存盘数据起始参数号：1300~1359为自学习参数、1360~1419为加密激活参数
#define	DSP_PARA_LEN	    			429					// ARM与DSP共用参数数量(429之后的ID号不下传DSP)
#define	PWR_PARA_LEN					280					// 参数数量

#define INTER_USART_BAUDRATE 			1565217				// ARM与DSP的UART通讯波特率
#define	SLOW_TIME_TICK					2					// 低速层时间节拍：2ms

#define BroadcastAddr 					0xFF				// 广播帧地址
#define BroadcastErrAddr 				0x00				// 广播帧下发未收到带地址的回复，HMI会下发0x00+0x03+......
#define BroadcastAddrLen 				71					// 广播帧及0x03正常数据轮询需回复的寄存器数量
#define ErrDataLen 						35					// 0x03地址102开始的历史故障轮询需回复的寄存器数量
#define ReadDataLen 					30					// 0x03地址200~229数据轮询需回复的寄存器数量
#define CAN_ADDR_OFFSET 				10					// CAN节点地址偏移量
#define ModbusAddr						CANAddr + CAN_ADDR_OFFSET

#define	MODBUS_FUNC_0X03				0x03				// 功能码0x03
#define	MODBUS_FUNC_0X06				0x06				// 功能码0x06
#define	MODBUS_FUNC_0X10				0x10				// 功能码0x10

#define USART1_DR_Base  				(u32)(&USART1->DR)	// 串口1数据寄存器地址0x40011004
#define	USART1_DMA_DATA_LEN				100					// 串口1_DMA方式接收数据帧长度(注意：此数值需大于最大接收数量：0x10功能码连续写300~329地址时最大，字节数为30*2+9=69Byte)
#define	USART1_STARS_DATA_LEN			7					// 串口1_STARS协议接收数据帧长度
#define	USART1_MODBUS_0X03_DATA_LEN		8					// 串口1_Modbus协议0x03功能码接收数据帧长度
#define	USART1_MODBUS_0X06_DATA_LEN		8					// 串口1_Modbus协议0x06功能码接收数据帧长度
#define	USART1_MODBUS_0X10_DATA_LEN1	13					// 串口1_Modbus协议0x10功能码接收数据帧长度1（写地址：0~1、1~2、38~39）
#define	USART1_MODBUS_0X10_DATA_LEN2	21					// 串口1_Modbus协议0x10功能码接收数据帧长度2（写北京时间地址：30~35）
#define	USART1_MODBUS_DATA_MAX_LEN		22					// 串口1_Modbus协议接收数据帧最大的长度（比协议最大的长度大1即可：USART1_MODBUS_0X10_DATA_LEN2+1）
	
#define READ_START_ADRESS 				0     				// 读起始地址：0
#define READ_END_ADRESS 				229					// 读结束地址：0~70为常规读地址；71~199未定义；200~229为测试读地址(功能码0x03)
#define WRITE_START_ADRESS 				0     				// 写起始地址：0
#define WRITE_END_ADRESS 				329					// 写结束地址：0~39为常规写地址；40~299未定义；300~329为测试写地址(功能码0x10)
#define MODBUS_SEND_LEN					200					// Modbus协议下的串口发送数组,最多为回复0x03功能码：71Word*2+5=147Byte，预留到200

#define PICH_CONTROL_ADRESS 			1					// 调桨控制指令地址
#define VLOOP_SPEED_ADRESS 				11					// 点动控制模式：目标速度地址
#define CLOSE_ANGLE_ADRESS 				13					// 区间控制模式：顺桨角度值地址
#define OPEN_ANGLE_ADRESS 				14					// 区间控制模式：开桨角度值地址
#define OPEN_SPEED_ADRESS 				15					// 区间控制模式：开桨速度值地址
#define CLOSE_SPEED_ADRESS 				16					// 区间控制模式：顺桨速度值地址
#define PLOOP_ANGLE_ADRESS 				17					// 位置控制模式：目标角度地址
#define PLOOP_SPEED_ADRESS 				18					// 位置控制模式：目标速度地址
#define TIME_ADJUST_ADRESS 				30					// 北京时间校准指令起始地址
#define MOTORTYPE_SELECT_ADRESS 		36					// 电机类型选择指令地址
#define FAULT_CLEAR_ADRESS 				37					// 历史故障清除指令地址 
#define DRIVE_ACTIVE_ADDRESS 			38					// 驱动器激活码地址
#define FAULT_CLEAR_CMD 				0x55AA 				// 历史故障清除指令
#define FAULT_RECORD_MAX_NUM 			20					// 历史故障信息最大数量20组（第0~19组）
#define FAULT_CODE_START_ADRESS 		102					// 历史故障存盘数据起始地址=102、寄存器数量=56
#define FAULT_CODE_NUM 					56					// 历史故障存盘数据起始地址=102、寄存器数量=56
#define FAULT_CODE_END_ADRESS 			158					// 历史故障存盘数据结束地址=158
#define REGISTER_READ_START_ADRESS 		200					// 寄存器变量显示起始地址=200、寄存器数量=30
#define REGISTER_READ_NUM 				30					// 寄存器变量显示起始地址=200、寄存器数量=30
#define REGISTER_READ_END_ADRESS 		229					// 寄存器变量显示结束地址=229
#define REGISTER_WRITE_START_ADRESS 	300					// 寄存器变量修改起始地址=300、寄存器数量=30
#define REGISTER_WRITE_NUM 				30					// 寄存器变量修改起始地址=300、寄存器数量=30
#define REGISTER_WRITE_END_ADRESS 		329					// 寄存器变量修改结束地址=329

//通讯类型
#define USART_1    						0					// 串口1 RS232或RS485上位机通讯
#define CAN232         					1					// 星辰上位机CAN通讯
#define CANopen        					2					// CANOpen通讯
#define TCPMB          					3					// 以太网通讯
#define USART_6    						6					// 串口6 RS232或RS485上位机通讯

//中断优先级排序（以下按由高到底排序）
//PRE为抢占优先级排序(1bit，最大=1)
//SBU为子优先级排序(3bit，最大=7)

#define TIM3_PREPRIORITY				0					// 定时器3 100us、300us、500us、1000us基本时基中断 
#define TIM3_SBUPRIORITY				0

#define CAN1_RX_PREPRIORITY				0					// CANOpen通讯RX中断
#define CAN1_RX_SBUPRIORITY				0

#define TIM5_PREPRIORITY				0					// 定时器5 1ms中断
#define TIM5_SBUPRIORITY				0

#define USART2_TX_PREPRIORITY			0					// 与DSP串口通讯TX中断
#define USART2_TX_SBUPRIORITY			1

#define USART1_PREPRIORITY				0					// 与上位机RS485通讯TX中断
#define USART1_SBUPRIORITY				1					// 6

#define DMA2_Stream7_PREPRIORITY		0					// USART1_DMA2发送传输中断
#define DMA2_Stream7_SBUPRIORITY		1

#define USART2_DMA1_4_5_PREPRIORITY		0 					// 与DSP串口通讯DMA1中断
#define USART2_DMA1_4_5_SBUPRIORITY		2

#define TIM2_PREPRIORITY				0 					// CANOpen协议栈定时器中断
#define TIM2_SBUPRIORITY				3

#define CAN1_TX_PREPRIORITY				0					// CANOpen通讯TX中断
#define CAN1_TX_SBUPRIORITY				5

#define USART6_PREPRIORITY				0					// 与上位机RS485通讯RX、TX中断
#define USART6_SBUPRIORITY				6

#define SPI2_DMA1_0_3_PREPRIORITY		0					// SSI绝对值编码器SPI2_DMA1_RX中断
#define SPI2_DMA1_0_3_SBUPRIORITY		7

#define USART3_PREPRIORITY				1					// 与充电器RS485通讯RX、TX中断
#define USART3_SUBPRIORITY				1

#define USART3_DMA1_7_4_PREPRIORITY		1					// 与充电器RS485通讯DMA1中断
#define USART3_DMA1_7_4_SUBPRIORITY		2

#define SDIO_PREPRIORITY				1					// SDIO中断
#define SDIO_SUBPRIORITY				3

#define TIM7_PREPRIORITY				1					// 定时器7 1ms中断
#define TIM7_SBUPRIORITY				3

#define BRAKE_DELAY_MECHINE				300					// 开抱闸机械延时，需根据实测数据填入

#define	Vset_Step_Delay	(labs((s16)ReadPara(BRAKE_ON_DELAY)) + BRAKE_DELAY_MECHINE)
#define	Pset_Step_Delay	(labs((s16)ReadPara(BRAKE_ON_DELAY)) + BRAKE_DELAY_MECHINE)

#define DEFECT_CODET_OPENLOOP_MASK		0x0006
#define DEFECT_CODET_FRA_MASK			0x0007

#ifdef PARA_SECTOR_SELECT_16K
	//STM32F407ZGT6，FLASH大小1024K，扇区定义如下：
	#define BOOTLOADER_ADDR		  		((u32)0x08000000)												//博德楼德，首地址0x0800 0000
	#define BOOTLOADER_FLAG_ADDR		((u32)0x08004000)												//博德楼德，首地址0x0800 4000
	#define USER_PARA_ADDR		  		((u32)0x08008000)												//用户参数，首地址0x0800 8000
	#define DEFAULT_PARA_ADDR	  		((u32)0x0800C000)												//出厂参数，首地址0x0800 C000
	#define APP1_ADDR		  			((u32)0x08010000)												//固件升级，首地址0x0801 0000
	#define APP2_ADDR					((u32)0x08020000)												//固件升级，首地址0x0802 0000
	#define MOTOR_ADDR              	((u32)0x08040000)												//桨叶角度，首地址0x0804 0000
	#define MOTOR_ADDR_ERA          	((u32)0x0804FFFF)												//桨叶角度，中地址0x0804 FFFF
	#define MOTOR_ADDR_END          	((u32)0x0805FF00)												//桨叶角度，末地址0x0805 FF00
	#define RESERVE07_ADDR          	((u32)0x08060000)												//预留扇区，首地址0x0806 0000
	#define ATTR_PARA_ADDR	  			((u32)0x08080000)												//参数属性，首地址0x0808 0000
	#define ErrRecord_StartAddr     	((u32)0x080A0000)	 											//历史故障，首地址0x080A 0000
	#define ErrRecord_EndAddr       	((u32)0x080BFFFE)												//历史故障，末地址0x080B FFFE
	#define RESERVE10_ADDR          	((u32)0x080C0000)												//预留扇区，首地址0x080C 0000
	#define RESERVE11_ADDR          	((u32)0x080E0000)												//预留扇区，首地址0x080E 0000
	#define CHIP_FLASH_ENDADDR    		((u32)0x080FFFFF)       										//结束地址，末地址0x080F FFFF

	#define FLASH_PAGE_SIZE_16K		  	((u32)16384)													//扇区大小16Kbytes：16K * 1024 = 16384
	#define FLASH_PAGE_SIZE				((u32)131072)													//扇区大小128Kbytes：128K * 1024 = 131072
	#define PARA_PAGE_NUM				1																//参数占用扇区数
	#define ATTR_PAGE_NUM				1																//属性占用扇区数
	#define SAVE_PARA_FLAG_OFFSET		(FLASH_PAGE_SIZE_16K * PARA_PAGE_NUM - 2)						//数据放在一个128K里面
	#define SAVE_ATTR_FLAG_OFFSET		(FLASH_PAGE_SIZE * ATTR_PAGE_NUM - 2)							//属性以及上下限都放在一个128K里面
	#define SAVE_SUCCESS_FLAG			0x55aa  														//首尾字节校验数值
	#define ErrRecord_PAGE_NUM	    	1				        										//历史故障,占用扇区数
	#define GZM_StartID     			1000 															//历史故障信息起始ID
	#define GZM_EndID       			1299 															//历史故障信息结束ID
	#define GZM_Num         			20 		  														//历史故障20组
	#define GZM_InfoNum     			30   															//历史故障信息

	/*
	[定义]		[定义]				[扇区]		      [地址范围]						  [大小]	
	Boot		Boot				Sector 00	0x0800 0000 - 0x0800 3FFF			16 	Kbytes	BootLoader.bin文件大小约10K
	BootFlag	BootFlag			Sector 01	0x0800 4000 - 0x0800 7FFF			16 	Kbytes  IAP升级标志存放地址：#define SJFlagADDR 0x0800 4000
	用户参数		USER_PARA_ADDR		Sector 02	0x0800 8000 - 0x0800 BFFF			16 	Kbytes
	出厂参数		DEFAULT_PARA_ADDR	Sector 03	0x0800 C000 - 0x0800 FFFF			16 	Kbytes	
	代码		Code				Sector 04	0x0801 0000 - 0x0801 FFFF			64 	Kbytes
	代码		Code				Sector 05	0x0802 0000 - 0x0803 FFFF			128 Kbytes
	桨叶角度		MOTOR_ADDR			Sector 06	0x0804 0000 - 0x0805 FF00			128 Kbytes
	预留		预留				Sector 07	0x0806 0000 - 0x0807 FFFF			128 Kbytes
	参数属性		ATTR_PARA_ADDR		Sector 08	0x0808 0000 - 0x0809 FFFF			128 Kbytes
	历史故障		ErrRecord_StartAddr	Sector 09	0x080A 0000 - 0x080B FFFF			128 Kbytes
	预留		预留				Sector 10	0x080C 0000 - 0x080D FFFF			128 Kbytes
	预留		预留				Sector 11	0x080E 0000 - 0x080F FFFF			128 Kbytes
	*/
#else
	//STM32F407ZGT6，FLASH大小1024K，扇区定义如下：
	#define CHIP_FLASH_ENDADDR    		((u32)0x080FFFFF)       										//芯片FLASH结束地址	第11扇区的结束地址
	#define FLASH_PAGE_SIZE		  		((u32)131072)													//扇区大小128Kbytes:0x20000 = 131072 = 128 * 1024
	#define PARA_PAGE_NUM				1																//参数占用扇区数
	#define ATTR_PAGE_NUM				1																//属性占用扇区数
	#define SAVE_PARA_FLAG_OFFSET		(FLASH_PAGE_SIZE * PARA_PAGE_NUM - 2)							//数据放在一个128K里面
	#define SAVE_ATTR_FLAG_OFFSET		(FLASH_PAGE_SIZE * ATTR_PAGE_NUM - 2)							//属性以及上下限都放在一个128K里面
	#define DEFAULT_PARA_ADDR	  		((CHIP_FLASH_ENDADDR - FLASH_PAGE_SIZE * PARA_PAGE_NUM) + 1 )	//出厂参数，首地址0x080E 0000
	#define USER_PARA_ADDR		  		(DEFAULT_PARA_ADDR - FLASH_PAGE_SIZE * PARA_PAGE_NUM) 			//用户参数，首地址0x080C 0000
	#define MOTOR_ADDR              	((u32)0x08040000)												//旋变角度，首地址0x0804 0000
	#define MOTOR_ADDR_ERA          	((u32)0x0804FFFF)												//旋变角度，中地址0x0804 FFFF
	#define MOTOR_ADDR_END          	((u32)0x0805FF00)												//旋变角度，末地址0x0805 FF00
	#define ATTR_PARA_ADDR	  			(ErrRecord_StartAddr - FLASH_PAGE_SIZE * ATTR_PAGE_NUM)			//参数属性，首地址0x0808 0000
	#define SAVE_SUCCESS_FLAG			0x55aa  														//首尾字节校验数值
	#define ErrRecord_PAGE_NUM	    	1				        										//历史故障,占用扇区数
	#define ErrRecord_StartAddr     	(USER_PARA_ADDR - FLASH_PAGE_SIZE * ErrRecord_PAGE_NUM) 		//历史故障,起始地址0x080A 0000
	#define ErrRecord_EndAddr       	(USER_PARA_ADDR - 2) 											//历史故障,结束地址0x080B FFFE
	#define GZM_StartID     			1000 															//历史故障信息起始ID
	#define GZM_EndID       			1299 															//历史故障信息结束ID
	#define GZM_Num         			19 		  														//历史故障条数，原10组改为19组
	#define GZM_InfoNum     			30   															//历史故障信息

	/*
	[定义]		[定义]				[扇区]		      [地址范围]						  [大小]	
	Boot		Boot				Sector 00	0x0800 0000 - 0x0800 3FFF			16 	Kbytes	BootLoader.bin文件大小约10K
	Boot		Boot				Sector 01	0x0800 4000 - 0x0800 7FFF			16 	Kbytes
	Boot		Boot				Sector 02	0x0800 8000 - 0x0800 BFFF			16 	Kbytes
	Boot		Boot				Sector 03	0x0800 C000 - 0x0800 FFFF			16 	Kbytes	IAP升级标志存放地址：#define SJFlagADDR 0x800C000
	代码		Code				Sector 04	0x0801 0000 - 0x0801 FFFF			64 	Kbytes
	代码		Code				Sector 05	0x0802 0000 - 0x0803 FFFF			128 Kbytes
	旋变角度		MOTOR_ADDR			Sector 06	0x0804 0000 - 0x0805 FF00(未用完)	128 Kbytes
	未定义		未定义				Sector 07	0x0806 0000 - 0x0807 FFFF			128 Kbytes
	参数属性		ATTR_PARA_ADDR		Sector 08	0x0808 0000 - 0x0809 FFFF			128 Kbytes
	历史故障		ErrRecord_StartAddr	Sector 09	0x080A 0000 - 0x080B FFFF			128 Kbytes
	用户参数		USER_PARA_ADDR		Sector 10	0x080C 0000 - 0x080D FFFF			128 Kbytes
	出厂参数		DEFAULT_PARA_ADDR	Sector 11	0x080E 0000 - 0x080F FFFF			128 Kbytes
	*/
#endif

#ifdef EXUSART_GLOBALS
	#define EXUSART_EXT
#else
	#define EXUSART_EXT extern
#endif

EXUSART_EXT u8 CommuType;  			 			//通讯方式选择
EXUSART_EXT u8 TxToPCCompleteFlag;   			//一帧发送完成标志: 1 完成，0 数据发送中

extern u32 cpu_sr;
extern s32 DspSet;	
extern u16 PcCtrlSet;							//PC设定控制权
extern u16 PcCtrl;								//PC使能控制权
extern u16 PcCtrlFlag;							//PC使能状态标志  		
extern s16 PcSet;								//PC设定值
extern union DEFECT_FLAG DefectFlag;			//故障状态
extern union DEFECT_FLAG DefectFlag1;			//故障状态
extern union DEFECT_FLAG DefectFlag2;			//故障状态
extern union SYS_STATE SysState;				//系统状态
extern union EXCEP_ENA ExcepENA;				//各类影响使能的异常状态
extern union FUNCTION_IN FunctionPortIn;		//外部逻辑功能输入(光耦)	
extern union IO_OUT	PortOut;					//IO口输出状态
extern union STMSTATUS	StmStatus;
extern union SLOWTIMEFLAG SlowTimeBaseFlag;
extern struct SysPara SysParaTable[];			//参数表
extern struct SysPara SysParaTable2[];			//充电器参数
extern DefaultSysParaType DefaultParaTable[];

extern u8 EraseFlash(u32 StartAddr, u8 FlashNum);
extern void WriteHalfWordToFlash(u32 StartAddr, u16* pWrData, u32 Len);
extern void InitErrRecord(void);
extern void ReadErrRecord(void);
extern void WriteErrRecord(void);

u16 CRC16(u8 *ptr, u8 len);
u16	ParaProcess(u16 ProcessCode,u16 ParaID,u16 NewPara);
s16 SavePara(u16 Mode);	   
s16	GetMotoTemp(u16 AdcVal);
u16 IsMotoHot(void);      						//置电机过热标志
u16 IsMotoOffLine(void);  						//置电机温度断线标志
u16	IsWaitStatus(void);
u16 ReadFlashPara(u16 Mode);
u16 GetSaveMode(void);							//返回保存指令 0 无需保存 1 需要保存用户参数 2 需要保存用户参数和出厂默认参数

void ParaInit(void);
void ChangePara(void);
void CabinetTempMeasureInit(u8 CabNum);			//柜体温度传感器曲线查表表格初始化
void Task1(void);								//高速任务
void Task2(void);								//调用在低速层中执行的函数
void GPIO_Configuration(void);
void IoHandle(void);
void SysINit(void);
void UserMainTask(void);
void InUartOverHandel(void);
void InUartDataProcess(void);	
void InUart_Configuration(void);
void SendParaPackToDsp(void);
void RealTimeDataFilter(void);
void EraseErrRecord(void);
void UpdateInUartTxFrame(u16 Cmd,u16 Data);		//更新发往DSP的数据帧
void ExUartOverHandel(void);
void ExUart6OverHandel(void);
void ChangeCommAddr(unsigned int NewBDR);		//更改通讯地址
void ChangeCommBDR(unsigned int NewBDR);		//更改波特率
void ExUartDataProcess(void);					//数据帧处理
void ExUart1_Configuration(void);
void ExUart4_Configuration(void);
void ResetSaveFlag(void);
void SaveAttrToFlash(u32 StartAddr);		
void SaveParaToFlash(u32 StartAddr);
void TIM_Configuration(void);

//驱动器用参数				
#define	NULL                         	0	//	占位
#define	SOFT_VER                     	1	//	软件版本号
#define	HARD_VER                     	2	//	硬件版本号
#define	DEFECT_CODE                  	3	//	告警码
#define	SYS_ST_CODE                  	4	//	系统状态
#define	V_FEEDBACK                   	5	//	速度反馈
#define	T_SET                        	6	//	转矩设定
#define	V_SET                        	7	//	速度设定
#define	I_FEEDBACK                   	8	//	电流反馈
#define	P_SET                        	9	//	压力设定
#define	P_FEEDBACK                   	10	//	压力反馈
#define	IGBT_TEMP                    	11	//	IGBT温度
#define	MOTO_TEMP                    	12	//	电机温度
#define	C_VOL                        	13	//	母线电压
#define	TEMP0                        	14	//	T0温度
#define	TEMP1                        	15	//	T1温度
#define	TEMP2                        	16	//	T2温度
#define	MOTO_TYPE                    	17	//	电机型号
#define	COMM_ADDR                    	18	//	分机地址
#define	COMM_BDR                     	19	//	通讯波特率
#define	PW_ON_DISP                   	20	//	上电开机默认显示运行参数序号
#define	MOUT1_SEL                    	21	//	MOUT1输出量类别选择
#define	MOUT2_SEL                    	22	//	MOUT2输出量类别选择
#define	MAX_V                        	23	//	最高转速
#define	MAX_I                        	24	//	最大电流
#define	OVER_I_C                     	25	//	额定电流，超载门槛电流
#define	OVER_I_T                     	26	//	超载延时
#define	MOTO_SAFE_TEMP               	27	//	电机过热温度保护点
#define	ALARM_LOCK                   	28	//	驱动器低16位告警停机锁定(1=锁定)
#define	ALARM_MASK                   	29	//	驱动器低16位告警屏蔽(1=屏蔽)
#define	OUT0_SEL                     	30	//	OUT00输出功能选择
#define	OUT1_SEL                     	31	//	OUT01输出功能选择
#define	OUT2_SEL                     	32	//	OUT02输出功能选择
#define	WORK_MOD                     	33	//	工作模式选择
#define	BRAKE_ON_DELAY               	34	//	制动器开延时
#define	BRAKE_OFF_DELAY              	35	//	制动器关延时
#define	T_SET_SCALE                  	36	//	力矩设定增益调节
#define	T_SET_OFFSET                 	37	//	力矩设定零偏
#define	T_TIME                       	38	//	力矩梯度限制
#define	V_PP                         	39	//	速度环比例系数
#define	V_PI                         	40	//	速度环积分常数
#define	V_FB_OFFSET                  	41	//	速度环反馈零偏
#define	V_SET_SCALE                  	42	//	速度环设定增益调节
#define	V_SET_OFFSET                 	43	//	速度环设定零偏
#define	V_DEAD_VAL                   	44	//	速度设定死区
#define	V_UP_TIME                    	45	//	速度环上升梯度延时
#define	V_DOWN_TIME                  	46	//	速度环下降梯度延时
#define	V_DIR_MOD                    	47	//	速度环电机方向选择
#define	P_PP                         	48	//	位置环比例系数
#define	P_PI                         	49	//	位置环前馈系数
#define	P_FB_SCAL                    	50	//	速度超限值
#define	RESERVE_PARA_51              	51	//	预留用参数_51
#define	RESERVE_PARA_52              	52	//	预留用参数_52
#define	BKFAN                        	53	//	抱闸风机控制逻辑：0=风机与使能联动；1=风机强行关；2=风机强行开
#define	DIH_STA                      	54	//	DI口状态高16位
#define	RESERVE_PARA_55              	55	//	预留用参数_55
#define	DISCHARGE_TIM                	56	//	每3秒泄放时间
#define	DISCHARGE_ACCTIM             	57	//	泄放累计时间
#define	DISCHARGE_ACCOUT             	58	//	泄放累计时间退出斜率分母
#define	DISCHARGE_ACCTH              	59	//	泄放累计时间报警门槛
#define	S_UP_TIME                    	60	//	S曲线上升时间
#define	S_DOWN_TIME                  	61	//	S曲线下降时间
#define	ALARM_AUTO_RST_EN            	62	//	驱动器低16位故障手自动复位:0=手动；1=自动
#define	DEFECTINSEL                  	63	//	位置超差
#define	ANALOG_IN0                   	64	//	模拟输入0
#define	ANALOG_IN0_PFIL              	65	//	模拟输入0滤波系数
#define	FLASHSAVEFLAG                	66	//	DSP回复设置为保存时1：不复位模式2：正常模式
#define	MAX_T                        	67	//	最大扭矩
#define	OPEN_T_LIMIT                 	68	//	转矩限制值
#define	MOTO_HUMID_EN                	69	//	电机温度湿度选择
#define	LIMIT_T                      	70	//	转矩限制值，带转矩限制的速度控制
#define	LIMIT_V                      	71	//	速度限制值，带速度限制的位置控制
#define	SCALE_A                      	72	//	电机位置传递比分子，对应电机负载终端圈数
#define	SCALE_B                      	73	//	电机位置传递比分母，对应电机圈数
#define	OVER_I_TOTAL                 	74	//	超载量
#define	Q_RESISTANCE                 	75	//	q轴电阻
#define	PM_FLUX                      	76	//	永磁磁链
#define	MOTOR_LD                     	77	//	D轴电感
#define	MOTOR_LQ                     	78	//	Q轴电感
#define	SPEED_OVERPROOF_T            	79	//	速度超差时间
#define	OUT3_SEL                     	80	//	OUT03输出功能选择
#define	OUT4_SEL                     	81	//	OUT04输出功能选择
#define	CABINET_TEMP0_ADJ            	82	//	PT100温度调节
#define	NORMAL_PWM_FREQ              	83	//	定频频率（为 0 时自动变频）
#define	STAR_PWM_TFREQ               	84	//	自动变频初始频率
#define	MAX_PWM_FREQ                 	85	//	自动变频最大频率
#define	MIN_PWM_FREQ                 	86	//	自动变频最小频率
#define	PWM_time_set                 	87	//	自动变频最小时间间隔
#define	PWM_FREQ_set                 	88	//	自动变频变频频率步进
#define	PWM_temp_set                 	89	//	自动变频温度步距
#define	PWM_print                    	90	//	实时运行频率
#define	HAND_SLOW_SPEED              	91	//	手动慢速
#define	FAN_ON_TEMP                  	92	//	风机关闭温度
#define	HAND_FAST_SPEED              	93	//	手动快速
#define	FAN_OFF_TEMP                 	94	//	风机关闭温度
#define	DI_STA                       	95	//	DI口状态
#define	DO_STA                       	96	//	DO口状态
#define	ERRSAVE_ENABLE               	97	//	故障信息存盘0=启用；非0=禁止
#define	MOTO_HUMID_OFFSET            	98	//	T3湿度零偏
#define	ERRSAVE_ADDR_DISPL           	99	//	故障信息存盘地址低16位显示
#define	ERRSAVE_ADDR_DISPH           	100	//	故障信息存盘地址高16位显示
#define	RESERVE_PARA_101             	101	//	预留用参数_101
#define	RESERVE_PARA_102             	102	//	预留用参数_102
#define	RESERVE_PARA_103             	103	//	预留用参数_103
#define	RESERVE_PARA_104             	104	//	预留用参数_104
#define	RESERVE_PARA_105             	105	//	预留用参数_105
#define	RESERVE_PARA_106             	106	//	预留用参数_106
#define	RESERVE_PARA_107             	107	//	预留用参数_107
#define	RESERVE_PARA_108             	108	//	预留用参数_108
#define	MOTOR_TEMP0_ADJ              	109	//	原电机温度校准参数，已不用
#define	MOTOR_TEMP0_SEL              	110	//	原电机温度传感器选择：0 KTY84；1 KTY83，已不用
#define	OUT5_SEL                     	111	//	OUT05输出功能选择
#define	OUT6_SEL                     	112	//	OUT06输出功能选择
#define	ARM_BUILD_YEAR               	113	//	ARM编译年份
#define	ARM_BUILD_DATE               	114	//	ARM编译月日
#define	ARM_BUILD_TIME               	115	//	ARM编译时分
#define	DSP_BUILD_YEAR               	116	//	DSP编译年份
#define	DSP_BUILD_DATE               	117	//	DSP编译月日
#define	DSP_BUILD_TIME               	118	//	DSP编译时分
#define	OUT7_SEL                     	119	//	OUT07输出功能选择
#define	AI0_OFFSET                   	120	//	AI0输入零偏
#define	AI0_SCALE                    	121	//	AI0输入增益调节
#define	MOTO_HUMID_K                 	122	//	T3湿度增益
#define	P_PV                         	123	//	位置环速度限制值
#define	EX_DEFECT_CODE               	124	//	扩展告警代码
#define	ERR_STOP_NUM                 	125	//	强制停机故障自动复位次数
#define	ERR_STOP_RST_TIM             	126	//	强制停机故障自动复位间隔时间
#define	CTRL_CMD                     	127	//	强制停机故障停机解除时间
#define	MOTO_PP_NUM                  	128	//	电机极对数
#define	CODER_TYPE                   	129	//	编码器型号
#define	HSD_T                        	130	//	缓上电延时
#define	IGBT_SAFE_TEMP               	131	//	IGBT过热温度保护点
#define	EXECP_MASK                   	132	//	异常停机屏蔽
#define	POWERONENABLE                	133	//	带使能上电允许
#define	IO_LOGIC_SET                 	134	//	输入输出逻辑电平设置
#define	IN1_SEL                      	135	//	输入功能选择1
#define	IN2_SEL                      	136	//	输入功能选择2
#define	VFB_SEL                      	137	//	速度反馈选择：0=测速线圈速度反馈；1=旋变DSP解码速度反馈；2=带开环保护的旋转变压器PMSM控制
#define	V_COMPENSATE                 	138	//	旋转变压器反馈换向：0=不换；1=换向
#define	P_PACC                       	139	//	位置环加速度
#define	PWM_FREQ_SEL                 	140	//	PWM频率选择
#define	FW_GAIN                      	141	//	弱磁增益
#define	FW_T                         	142	//	弱磁时间常数
#define	ID_MAX                       	143	//	D轴最大电流
#define	RESOLVERERR_C                	144	//	旋变异常门槛
#define	T_PP3                        	145	//	6K力矩环比例系数
#define	T_PI3                        	146	//	6K力矩环积分常数
#define	P_PDEC                       	147	//	位置环减速度
#define	T_PI4                        	148	//	8K力矩环积分常数
#define	MOD_CTRL                     	149	//	工作模式控制字
#define	V_FILT_L1                    	150	//	速度滤波上限系数
#define	PDECODE_I                    	151	//	位置解码I
#define	PDECODE_P                    	152	//	位置解码P
#define	VDECODE_F                    	153	//	速度滤波时间常数
#define	IGBT_SEN_SEL                 	154	//	IGBT温度传感器选择：0=IGBT内置传感器；1=外置独立传感器
#define	ADC_APP                      	155	//	AD采样增益误差比例修正系数
#define	ADC_OFFSET                   	156	//	AD采样零点偏移值
#define	COS_OFFSET                   	157	//	COS信号零点偏移值
#define	SIN_OFFSET                   	158	//	SIN信号零点偏移值
#define	SIN_APP                      	159	//	SIN、COS信号幅值修正系数
#define	ZREF_MID                     	160	//	ZREF中点值
#define	IA_MID                       	161	//	A相电流中点值
#define	IB_MID                       	162	//	B相电流中点值
#define	I_SCALE                      	163	//	电流校准比例
#define	OFFSET_ANGLE                 	164	//	电机初始偏角
#define	ADJ_INPUT1                   	165	//	调试输入1
#define	ADJ_INPUT2                   	166	//	调试输入2
#define	ADJ_DISP1                    	167	//	调试显示1
#define	ADJ_DISP2                    	168	//	调试显示2
#define	HFINJE_FREQ                  	169	//	信号注入频率
#define	HFINJE_AMPL                  	170	//	信号注入幅值
#define	HFINJE_PFIL                  	171	//	信号注入法位置滤波时间常数
#define	HFINJE_PDEP                  	172	//	信号注入法位置解码P
#define	HFINJE_PDEI                  	173	//	信号注入法位置解码I
#define	NSREGN_FILT                  	174	//	磁钢极性识别滤波
#define	HFIWAY_SPEED                 	175	//	信号注入法切换速度
#define	SMOWAY_SPEED                 	176	//	滑模观测法切换速度
#define	WAVE_ADJ9                    	177	//	IGBT风机关断电压：当母线电压低于250V时关断IGBT底部风机
#define	WAVE_ADJ10                   	178	//	泻放电阻功率
#define	WAVE_ADJ11                   	179	//	原开环设定速度，未使用
#define	WAVE_ADJ12                   	180	//	泻放过载时间
#define	WAVE_ADJ13                   	181	//	泻放过载恢复时间
#define	WAVE_ADJ14                   	182	//	泻放电阻阻值
#define	WAVE_ADJ15                   	183	//	泻放过载倍率
#define	WAVE_ADJ16                   	184	//	预留用参数_184
#define	P_ERR_TH                     	185	//	速度设定滤波次数
#define	SMOPDIR_COMP                 	186	//	滑模观测法正向位置补偿
#define	CAN_CURRE_FIR                	187	//	CAN上传的电机电流滤波常数
#define	SMONDIR_COMP                 	188	//	滑模观测法负向位置补偿
#define	P_FILT_A                     	189	//	位置环设定滤波系数
#define	P_Q_TH                       	190	//	压力流量跳变门槛
#define	P_SET_TH                     	191	//	压力设定下跳变门槛
#define	ELE_POS                      	192	//	位置码
#define	FEEDBACK_CAL                 	193	//	模拟速度反馈校准允许
#define	ELE_POS_NUM                  	194	//	圈数
#define	DISCHARGE_R                  	195	//	泄放电阻阻值
#define	DISCHARGE_P                  	196	//	泄放电阻功率
#define	ERR_OPEN_MASK                	197	//	故障继电器断开屏蔽
#define	CAN_MXVOL_FIR                	198	//	CAN上传的母线电压滤波常数
#define	CAN_SPEED_FIR                	199	//	CAN上传的电机速度滤波常数
#define	FUN_MASK                     	200	//	飞车保护功能屏蔽
#define	FCBHMK                       	201	//	飞车保护门槛
#define	LD_REALSE_ENABLE             	202	//	漏电故障解除允许
#define	OPENLOOP_PP                  	203	//	开环控制比例系数
#define	OPENLOOP_PI                  	204	//	开环控制时间常数
#define	OPENLOOP_LSEN                	205	//	低速无缝开环允许
#define	SIN_OFFSET2                  	206	//	SIN零偏
#define	COS_OFFSET2                  	207	//	COS零偏
#define	SIN_APP2                     	208	//	正余弦信号幅值修正系数: 波谷
#define	SIN_PHASE                    	209	//	SINCOS采样相位调节
#define	MANUF_DEV_NAME_SEL           	210	//	从站设备型号选择
#define	RESERVE_PARA_211             	211	//	预留用参数_211
#define	CAN232_BAUD                  	212	//	CAN232波特率
#define	CAN_SEL                      	213	//	CAN协议选择 0：CAN232  1：CANopen
#define	CO_NODE_ID                   	214	//	CANopen节点号
#define	CO_GUARD_TIME                	215	//	"CANopen Guard Time 守护时间	  Index:0x100C"
#define	CO_LIFE_TIME_FACTOR          	216	//	CANopen Life Time Factor 寿命保护因子   Index:0x100D
#define	CO_PRODUCER_HEARTBEAT_TIME   	217	//	CANopen Producer Heartbeat Time 心跳产生时间  Index:0x1017
#define	CO_RPDO1_COB_ID_H            	218	//	CANopen RPDO1 Cob-ID 高16位
#define	CO_RPDO1_COB_ID_L            	219	//	CANopen RPDO1 Cob-ID 低16位
#define	CO_RPDO1_TRANSMISSION_TYPE   	220	//	CANopen RPDO1 Transmission Type 传输类型
#define	CO_RPDO2_COB_ID_H            	221	//	CANopen RPDO2 Cob-ID 高16位
#define	CO_RPDO2_COB_ID_L            	222	//	CANopen RPDO2 Cob-ID 低16位
#define	CO_RPDO2_TRANSMISSION_TYPE   	223	//	CANopen RPDO2 Transmission Type 传输类型
#define	CO_RPDO3_COB_ID_H            	224	//	CANopen RPDO3 Cob-ID 高16位
#define	CO_RPDO3_COB_ID_L            	225	//	CANopen RPDO3 Cob-ID 低16位
#define	CO_RPDO3_TRANSMISSION_TYPE   	226	//	CANopen RPDO3 Transmission Type 传输类型
#define	CO_RPDO4_COB_ID_H            	227	//	CANopen RPDO4 Cob-ID 高16位
#define	CO_RPDO4_COB_ID_L            	228	//	CANopen RPDO4 Cob-ID 低16位
#define	CO_RPDO4_TRANSMISSION_TYPE   	229	//	CANopen RPDO4 Transmission Type 传输类型
#define	CO_TPDO1_COB_ID_H            	230	//	CANopen TPDO1 Cob-ID 高16位
#define	CO_TPDO1_COB_ID_L            	231	//	CANopen TPDO1 Cob-ID 低16位
#define	CO_TPDO1_TRANSMISSION_TYPE   	232	//	CANopen TPDO1 Transmission Type 传输类型
#define	CO_TPDO1_INHIBIT_TIME        	233	//	CANopen TPDO1 Inhibit Time 抑制时间
#define	CO_TPDO1_EVENT_TIMER         	234	//	CANopen TPDO1 Event Timer 事件时间
#define	CO_TPDO2_COB_ID_H            	235	//	CANopen TPDO2 Cob-ID 高16位
#define	CO_TPDO2_COB_ID_L            	236	//	CANopen TPDO2 Cob-ID 低16位
#define	CO_TPDO2_TRANSMISSION_TYPE   	237	//	CANopen TPDO2 Transmission Type 传输类型
#define	CO_TPDO2_INHIBIT_TIME        	238	//	CANopen TPDO2 Inhibit Time 抑制时间
#define	CO_TPDO2_EVENT_TIMER         	239	//	CANopen TPDO2 Event Timer 事件时间
#define	CO_TPDO3_COB_ID_H            	240	//	CANopen TPDO3 Cob-ID 高16位
#define	CO_TPDO3_COB_ID_L            	241	//	CANopen TPDO3 Cob-ID 低16位
#define	CO_TPDO3_TRANSMISSION_TYPE   	242	//	CANopen TPDO3 Transmission Type 传输类型
#define	CO_TPDO3_INHIBIT_TIME        	243	//	CANopen TPDO3 Inhibit Time 抑制时间
#define	CO_TPDO3_EVENT_TIMER         	244	//	CANopen TPDO3 Event Timer 事件时间
#define	CO_TPDO4_COB_ID_H            	245	//	CANopen TPDO4 Cob-ID 高16位
#define	CO_TPDO4_COB_ID_L            	246	//	CANopen TPDO4 Cob-ID 低16位
#define	CO_TPDO4_TRANSMISSION_TYPE   	247	//	CANopen TPDO4 Transmission Type 传输类型
#define	CO_TPDO4_INHIBIT_TIME        	248	//	CANopen TPDO4 Inhibit Time 抑制时间
#define	CO_TPDO4_EVENT_TIMER         	249	//	CANopen TPDO4 Event Timer 事件时间
#define	ARM_DSP_COMMPARA0            	250	//	ARM与DSP的通讯控制参数0：bit0=1：强制开启底座风机；bit1=1：开启泄放；bit2=1：开启电机预热（注意：bit15为本ID号下传DSP的写命令，不能使用）
#define	ARM_DSP_COMMPARA1            	251	//	ARM与DSP的通讯控制参数1
#define	ARM_DSP_COMMPARA2            	252	//	ARM与DSP的通讯控制参数2
#define	ARM_DSP_COMMPARA3            	253	//	ARM与DSP的通讯控制参数3
#define	ARM_DSP_COMMPARA4            	254	//	ARM与DSP的通讯控制参数4
#define	ARM_DSP_COMMPARA5            	255	//	ARM与DSP的通讯控制参数5
#define	DRIVER_NUM                   	256	//	驱动器及弱磁代号
#define	DISCHARGE_VOL                	257	//	泄放保护点电压
#define	RESERVE_PARA_258             	258	//	预留用参数_258
#define	OVER2_VOL                    	259	//	过压2保护点电压
#define	ALARM_AUTO_RST_ENH           	260	//	驱动器高16位故障手自动复位
#define	MOUT2_OFFSET                 	261	//	MOUT2 零偏调节
#define	MOUT2_GAIN                   	262	//	MOUT2 增益调节
#define	ALARM_MASKH                  	263	//	驱动器高16位故障屏蔽
#define	DEFECT_CODEH                 	264	//	驱动器高16位故障码
#define	MOUT1_GAIN                   	265	//	MOUT1 增益调节
#define	MOUT1_OFFSET                 	266	//	MOUT1 零偏调节
#define	ERR_DIP_SELECT               	267	//	故障代码显示方式选择
#define	RESERVE_PARA_268             	268	//	预留用参数_268
#define	RESERVE_PARA_269             	269	//	预留用参数_269
#define	IC_MID                       	270	//	C相电流中点值
#define	MOTOR_MAX_I                  	271	//	标配电机最大限流
#define	RESERVE_PARA_272             	272	//	预留用参数_272
#define	RATED_LIMIT_I                	273	//	额定限流
#define	BK_CURR_DISP                 	274	//	抱闸电流显示
#define	LOW_VOL                      	275	//	母线欠压保护点
#define	BK_CURR_MAX                  	276	//	抱闸电流上限
#define	BK_CURR_OFFSET               	277	//	抱闸电流零偏
#define	BK_CURR_MIN                  	278	//	抱闸电流下限
#define	BK_CURR_SCALE                	279	//	抱闸电流增益
#define	ALARM_LOCKH                  	280	//	驱动器高16位故障停机锁定
#define	DISCHARGE_RELEASE_VOL        	281	//	泄放回差电压
#define	RESERVE_PARA_282             	282	//	预留用参数_282
#define	OVER1_VOL                    	283	//	过压1保护点电压
#define	RESERVE_PARA_284             	284	//	预留用参数_284
#define	C_VOL_CAL                    	285	//	母线电压校准
#define	RESERVE_PARA_286             	286	//	预留用参数_286
#define	RESERVE_PARA_287             	287	//	预留用参数_287
#define	RESERVE_PARA_288             	288	//	预留用参数_288
#define	CHARGE_SLOPE                 	289	//	充电压差斜率
#define	RESERVE_PARA_290             	290	//	预留用参数_290
#define	MOTOR_LIMIT_TEMP             	291	//	电机过热限温
#define	RESERVE_PARA_292            	292	//	预留用参数_292
#define	POWER_IN_LOSE_EN             	293	//	缺相保护允许
#define	RESERVE_PARA_294            	294	//	预留用参数_294
#define	DISCHARGE_PROTECT            	295	//	泄放能量保护
#define	OPEN_LOOP_EN                 	296	//	开环控制允许: 0不允许，1允许
#define	RESERVE_PARA_297            	297	//	预留用参数_297
#define	RESERVE_PARA_298            	298	//	预留用参数_298
#define	RESERVE_PARA_299            	299	//	预留用参数_299
#define	USER_ARM_BUILD_YEAR          	300	//	用户编程ARM编译年份
#define	USER_ARM_BUILD_DATE          	301	//	用户编程ARM编译月日
#define	USER_ARM_BUILD_TIME          	302	//	用户编程ARM编译时分
#define	OPEN_CLOSE_SPEED             	303	//	开环收桨速度：27648对应最高转速
#define	POSITIONSKH                  	304	//	位置超差限制值高十六位
#define	POSITIONSKL                  	305	//	位置超差限制值低十六位
#define	POSITIONSK                   	306	//	位置超差故障
#define	ERR_RST                      	307	//	手动复位故障
#define	SEC_SCALE_A                  	308	//	第二编码器传递比分子，对应轮毂圈数
#define	SEC_SCALE_B                  	309	//	第二编码器传递比分子，对应编码器圈数
#define	GRAY_EN                      	310	//	格雷码解码允许
#define	SEC_POS_NUM                  	311	//	第二编码器圈数
#define	SEC_POS                      	312	//	第二编码器位置
#define	SAFE_CLOSE_SPEED             	313	//	DSP自主收桨速度（单位0.01°/S，包含位置码异常开环收桨、与ARM通讯故障收桨、桨叶角度错误收桨）
#define	SAFE_CLOSE_UP_TIME           	314	//	安全收桨上升梯度延时 1 = 10ms
#define	SAFE_CLOSE_DOWN_TIME         	315	//	安全收桨下降梯度延时 1 = 10ms
#define	MOTOR_POS_RST                	316	//	电机位置复位
#define	MOTOR_ANGLE                  	317	//	桨叶角度
#define	SEC_CODER_ANGLE              	318	//	"第二编码器角度低16位	（	ID366为高16位）"
#define	SEC_CODER_OFFSET             	319	//	第二编码器圈数偏移
#define	SEC_CODER_DIR                	320	//	第二编码器计数方向选择：0逆时针加计数，1顺时针加计数
#define	BK_SWITCH                    	321	//	制动器开关：0=制动器与使能联动；1=制动器关，2=制动器开
#define	SAFE_DI_DLY                  	322	//	安全链断开延时
#define	MOTOR_ANGLE_OFFSETH          	323	//	电机角度零偏高十六位
#define	MOTOR_ANGLE_OFFSETL          	324	//	电机角度零偏低十六位
#define	RESERVE_PARA_325            	325	//	预留用参数_325
#define	RESERVE_PARA_326            	326	//	预留用参数_326
#define	PITCH_PLC_STATE              	327	//	驱动器状态机状态
#define	SERVICE_TIME_DELAY           	328	//	手动正反速度延时
#define	RESERVE_PARA_329            	329	//	预留用参数_329
#define	RESERVE_PARA_330            	330	//	预留用参数_330
#define	RESERVE_PARA_331            	331	//	预留用参数_331
#define	RESERVE_PARA_332            	332	//	预留用参数_332
#define	RESERVE_PARA_333            	333	//	预留用参数_333
#define	RESERVE_PARA_334            	334	//	预留用参数_334
#define	RESERVE_PARA_335            	335	//	预留用参数_335
#define	USER_PARA1_H                 	336	//	用户保存参数
#define	USER_PARA1_L                 	337	//	用户保存参数
#define	USER_PARA2_H                 	338	//	用户保存参数
#define	USER_PARA2_L                 	339	//	用户保存参数
#define	USER_PARA3_H                 	340	//	用户保存参数3
#define	USER_PARA3_L                 	341	//	用户保存参数3
#define	RESERVE_PARA_342             	342	//	预留用参数_342
#define	RESERVE_PARA_343             	343	//	预留用参数_343
#define	RESERVE_PARA_344             	344	//	预留用参数_344
#define	RESERVE_PARA_345             	345	//	预留用参数_345
#define	RESERVE_PARA_346             	346	//	预留用参数_346
#define	RESERVE_PARA_347             	347	//	预留用参数_347
#define	RESERVE_PARA_348             	348	//	预留用参数_348
#define	RESERVE_PARA_349             	349	//	预留用参数_349
#define	RESERVE_PARA_350             	350	//	预留用参数_350
#define	RESERVE_PARA_351             	351	//	预留用参数_351
#define	RESERVE_PARA_352             	352	//	预留用参数_352
#define	RESERVE_PARA_353             	353	//	预留用参数_353
#define	RESERVE_PARA_354             	354	//	预留用参数_354
#define	RESERVE_PARA_355             	355	//	预留用参数_355
#define	RESERVE_PARA_356             	356	//	预留用参数_356
#define	RESERVE_PARA_357             	357	//	预留用参数_357
#define	RESERVE_PARA_358             	358	//	预留用参数_358
#define	RESERVE_PARA_359             	359	//	预留用参数_359
#define	CONTROLWORD_DISP             	360	//	客户给定控制字送上位机显示
#define	TARGET_POSITION_L            	361	//	目标位置给定送上位机显示
#define	TARGET_POSITION_H            	362	//	目标位置给定送上位机显示
#define	DATAINIT_SELECT              	363	//	"上电初始化数值选择：0=eds默认值	1=FLASH存盘值 已不用"
#define	PITCH_SPEED                  	364	//	桨叶速度，单位°/S
#define	REAL_ANGLE                   	365	//	SSI桨叶绝对角度（0.01°） = SSI当前角度值 - SSI零点值（UserPara1）
#define	SEC_CODER_ANGLEH             	366	//	"第二编码器角度高16位	（	ID318为低16位）"
#define	SAVE_PARA                    	367	//	只用于查询客户下发的save_para值，ReadPara(SAVE_PARA) = save_para;
#define	CHARGE_PARA_SAVE_EN          	368	//	"充电器参数存盘使能	0=不允许存盘充电器参数，非0=运行存盘充电器参数"
#define	FAN_ON_DELAY                 	369	//	"使能开启后，风机的启动延时		单位ms"
#define	REAL_ANGLEH                  	370	//	SSI桨叶角度高16位
#define	CO_CONSUMER_HEARTBEAT_TIME   	371	//	从站判断主站心跳时间
#define	ZLG7289_RES                  	372	//	消隐指令清除时间：0不发送清除指令，1手动发送一次指令
#define	BOFFCNT                      	373	//	CAN离线次数
#define	TECCNT                       	374	//	CAN发送错误计数器
#define	USER_PARA8                   	375	//	用户参数8
#define	USER_PARA9                   	376	//	用户参数9
#define	USER_PARA10                  	377	//	用户参数10
#define	C_VOL_ONOFF                  	378	//	IO24V输出关断母线电压值
#define	C_VOL_ONOFF_SEL              	379	//	IO24V输出关断电逻辑
#define	RESERVE_PARA_380             	380	//	预留用参数_390
#define	RESERVE_PARA_381             	381	//	预留用参数_390
#define	RESERVE_PARA_382             	382	//	预留用参数_390
#define	RESERVE_PARA_383             	383	//	预留用参数_390
#define	RESERVE_PARA_384             	384	//	预留用参数_390
#define	RESERVE_PARA_385             	385	//	预留用参数_390
#define	RESERVE_PARA_386             	386	//	DSP Boot Version // 最新为100
#define	RESERVE_PARA_387             	387	//	预留用参数_390
#define	RESERVE_PARA_388             	388	//	预留用参数_390
#define	RESERVE_PARA_389             	389	//	预留用参数_390
#define	RESERVE_PARA_390             	390	//	预留用参数_390
#define	RESERVE_PARA_391             	391	//	预留用参数_391
#define	RESERVE_PARA_392             	392	//	预留用参数_392
#define	RESERVE_PARA_393             	393	//	预留用参数_393
#define	RESERVE_PARA_394             	394	//	预留用参数_394
#define	RESERVE_PARA_395             	395	//	预留用参数_395
#define	RESERVE_PARA_396             	396	//	预留用参数_396
#define	RESERVE_PARA_397             	397	//	预留用参数_397
#define	RESERVE_PARA_398             	398	//	预留用参数_398
#define	RESERVE_PARA_399             	399	//	预留用参数_399
#define	RESERVE_PARA_400             	400	//	预留用参数_400
#define	POSITIONSKL2                 	401	//	位置超差计算值
#define	CAP_VOLT                     	402	//	AI0P反馈
#define	CAP_VOLT_OFFSET              	403	//	AI0P零偏
#define	CAP_VOLT_SCALE               	404	//	AI0P增益
#define	RESERVE_PARA_405             	405	//	位置误差阀值高16位
#define	RESERVE_PARA_406             	406	//	位置误差阀值低16位
#define	RESERVE_PARA_407             	407	//	双驱主从机设置
#define	RESERVE_PARA_408             	408	//	预留用参数_408
#define	RESERVE_PARA_409             	409	//	预留用参数_409
#define	MOTOOVERLOADDISP             	410	//	电机超载量反馈 单位1%
#define	MOTOOVERLOADMAXI             	411	//	电机超载最大电流
#define	MOTOOVERLOADTIMES            	412	//	电机超载延时时间
#define	MOTOOVERLOADI                	413	//	电机超载额定电流
#define	IGBTOVERTEMP1                	414	//	IGBT过热1触发条件
#define	DSP_AUTOSAFERUN_EN_DELAY     	415	//	DSP自主收桨开使能延时
#define	DSP_HEARTBEAT_COUNT          	416	//	DSP心跳计数器
#define	DSP_AUTORUN_OVERTIME          	417	//	DSP自主收桨超时时间（默认32S，包含位置码异常开环收桨、与ARM通讯故障收桨、桨叶角度错误收桨）
#define	MOTO_SAFE_TEMP1              	418	//	电机过热温度保护点1(130°)
#define	DEFECT_CODET                 	419	//	驱动器T16位故障(DSP扩展故障码)
#define	ALARM_MASKT                  	420	//	驱动器T16位故障屏蔽
#define	ALARM_AUTO_RST_ENT           	421	//	驱动器T16位故障手自动复位
#define	DEFECT_CODET_SET             	422	//	驱动器T16位故障码预留参数
#define	WORK_MOD_FEB                 	423	//	DSP当前实际控制模式反馈
#define	RESOLVER_ERR_RESET           	424	//	编码器故障复位（星辰协议下ID307无法复位编码器故障）
#define	ACTUALTORQUE                 	425	//	实际输出转矩 
#define	RESERVE_PARA_426             	426	//	预留用参数_426
#define	RESERVE_PARA_427             	427	//	预留用参数_427
#define	RESERVE_PARA_428             	428	//	预留用参数_428
#define	RESERVE_PARA_429             	429	//	预留用参数_429；DSP_PARA_LEN=429，此后的ID不下传到DSP
            	                      		//	430~737为Bit00~Bit43故障属性参数，不能占用！！！
            	                      		//	738~765为Bit44~Bit47故障属性参数（充电器Bit12~Bit15故障，目前为预留位,不能占用！！！）
#define	CHARG_DC24V_TEMP             	766	//	充电器DC24V电源温度
#define	CHARG_AC_OFFEST              	767	//	相电压偏移量
#define	CHARG_SCR_CTR                	768	//	半控桥控制
#define	CHARG_AC_VOL                 	769	//	相电压检测
#define	CHARG_DEFECT_CODE            	770	//	充电器告警码
#define	CHARG_SYS_ST_CODE            	771	//	充电器系统状态
#define	CHARG_IGBT_TEMP              	772	//	充电器IGBT温度
#define	CHARG_VOL                    	773	//	充电器母线电压
#define	CHARG_V_FEEDBACK             	774	//	充电器电容电压
#define	CHARG_I_FEEDBACK             	775	//	充电器充电电流
#define	CHARG_ALARM_EN               	776	//	充电器故障允许
#define	CHARG_ALARM_AUTO_RST_EN      	777	//	充电器故障自动复位允许
#define	CHARG_ADDR                   	778	//	充电器通讯地址
#define	CHARG_BDR                    	779	//	充电器通讯波特率
#define	CHARG_TIM                    	780	//	充电器通讯周期
#define	CHARG_V_SET                  	781	//	充电器电压设定
#define	CHARG_I_SET                  	782	//	充电器电流设定
#define	CHARG_CTRL_REG               	783	//	充电器控制寄存器
#define	CHARG_CTRL_MOD               	784	//	充电器控制模式
#define	CHARG_INTERSPACE_TEMP        	785	//	充电器内腔温度
#define	CHARG_ARM_BUILD_YEAR         	786	//	充电器ARM编译年份
#define	CHARG_ARM_BUILD_DATE         	787	//	充电器ARM编译月日
#define	CHARG_ARM_BUILD_TIME         	788	//	充电器ARM编译时分
#define	CHARG_V_SET_BRAKER           	789	//	抱闸电压设定：205代表205V
#define	CHARG_V_FEEDBACK_BRAKER      	790	//	抱闸电压反馈：205代表205V
#define	CHARG_SN_BRAKER              	791	//	抱闸使能控制：0=关抱闸；其他=开抱闸
#define	RTC_YEAR_SET                 	792	//	RTC年设置
#define	RTC_MONTH_AND_DATA_SET       	793	//	RTC月日设置
#define	RTC_HOUR_AND_MINIT_SET       	794	//	RTC时分设置
#define	RTC_SECON_SET                	795	//	RTC秒设置
#define	RTC_YEAR_FEB                 	796	//	RTC年显示
#define	RTC_MONTH_AND_DATA_FEB       	797	//	RTC月日显示
#define	RTC_HOUR_AND_MINIT_FEB       	798	//	RTC时分显示
#define	RTC_SECON_FEB                	799	//	RTC秒显示
#define	TCPSEL                       	800	//	TCP的选择
#define	TCPNETMASK1                  	801	//	子网掩码1
#define	TCPNETMASK2                  	802	//	子网掩码2
#define	TCPNETMASK3                  	803	//	子网掩码3
#define	TCPNETMASK4                  	804	//	子网掩码4
#define	TCPGATE1                     	805	//	网关1
#define	TCPGATE2                     	806	//	网关2
#define	TCPGATE3                     	807	//	网关3
#define	TCPGATE4                     	808	//	网关4
#define	TCPIP1                       	809	//	IP地址1
#define	TCPIP2                       	810	//	IP地址2
#define	TCPIP3                       	811	//	IP地址3
#define	TCPIP4                       	812	//	IP地址4
#define	MACADDRESS1                  	813	//	物理地址macaddress1
#define	MACADDRESS2                  	814	//	物理地址macaddress2
#define	MACADDRESS3                  	815	//	物理地址macaddress3
#define	MACADDRESS4                  	816	//	物理地址macaddress4
#define	MACADDRESS5                  	817	//	物理地址macaddress5
#define	MACADDRESS6                  	818	//	物理地址macaddress6
#define	TCPCONTROL                   	819	//	1：TCP控制:0：TCP监控
#define	TCPCANOPENPRO                	820	//	1：TCP控制具有优先权 ， 0：CANOPEN控制具有优先权
#define	PT100TYPE                    	821	//	PT100电路参数类型0=老电路；1=新电路；2=最新电路
#define	PARASAVEENABLE               	822	//	抱闸电源异常、误触发、超限的故障属性默认值允许修改：=0xAA55允许修改；其他不允许
#define	CHARG_CAP                    	823	//	超级电容容量 20000对应2F
#define	CHARG_BTMVOL                	824	//	BTM电压：精度1V
#define	CHARG_ADC_RESET_TIMES         	825	//	充电器AD模块复位次数
#define	CURRENT_ACTUAL_VALUE         	826	//	未经滤波的电流值
#define	VELOCITY_SENSOR_ACTUAL_VALUE 	827	//	未经滤波的速度值
#define	ARM_RESERVE_PARA_828         	828	//	ARM Boot Version // 来自0x08004000的"H6_Boot:xxx"
#define	ARM_RESERVE_PARA_829         	829	//	ARM预留用参数_829
#define	ARM_RESERVE_PARA_830         	830	//	ARM预留用参数_830
#define	ARM_RESERVE_PARA_831         	831	//	ARM预留用参数_831
#define	ARM_RESERVE_PARA_832         	832	//	ARM预留用参数_832
#define	ARM_RESERVE_PARA_833         	833	//	ARM预留用参数_833
#define	ARM_RESERVE_PARA_834         	834	//	ARM预留用参数_834
#define	ARM_RESERVE_PARA_835         	835	//	ARM预留用参数_835
#define	ARM_RESERVE_PARA_836         	836	//	ARM预留用参数_836
#define	ARM_RESERVE_PARA_837         	837	//	ARM预留用参数_837
#define	ARM_RESERVE_PARA_838         	838	//	ARM预留用参数_838
#define	ARM_RESERVE_PARA_839         	839	//	ARM预留用参数_839
#define	ARM_RESERVE_PARA_840         	840	//	ARM预留用参数_840
#define	OVER_POSITION                	841	//	桨叶位置超限阀值
#define	POWER75_OVER_TEMP             	842	//	开关电源过温阀值
#define	SDCARD_ERRCODE               	843	//	SD卡错误码
#define	ARM_RESERVE_PARA_844         	844	//	ARM预留用参数_844
#define	ARM_RESERVE_PARA_845         	845	//	ARM预留用参数_845
#define	ARM_RESERVE_PARA_846         	846	//	ARM预留用参数_846
#define	DEFECT_CODEM                 	847	//	驱动器M16位故障码
#define	ALARM_MASKM                  	848	//	驱动器M16位故障屏蔽
#define	ALARM_AUTO_RST_ENM           	849	//	驱动器M16位故障手自动复位
#define	DEFECT_CODEM_SET             	850	//	驱动器M16位故障码预留参数
#define	CLEAR_STARS_PARA_CMD         	851	//	擦除SD卡星辰参数命令：1=擦除用户参数；2=擦除出厂参数；3=擦除用户参数和出厂参数
#define	ARM_RESERVE_PARA_852         	852	//	ARM预留用参数852
#define	ARM_RESERVE_PARA_853         	853	//	ARM预留用参数853
#define	ARM_RESERVE_PARA_854         	854	//	ARM预留用参数854
#define	ARM_RESERVE_PARA_855         	855	//	ARM预留用参数855
#define	ARM_RESERVE_PARA_856         	856	//	ARM预留用参数856
#define	ARM_RESERVE_PARA_857         	857	//	ARM预留用参数857
#define	ARM_RESERVE_PARA_858         	858	//	ARM预留用参数858
#define	ARM_RESERVE_PARA_859         	859	//	ARM预留用参数859
#define	ARM_RESERVE_PARA_860         	860	//	ARM预留用参数860
#define	ARM_RESERVE_PARA_861         	861	//	ARM预留用参数861
#define	ARM_RESERVE_PARA_862         	862	//	ARM预留用参数862
#define	ARM_RESERVE_PARA_863         	863	//	ARM预留用参数863
#define	ARM_RESERVE_PARA_864         	864	//	ARM预留用参数864
#define	ARM_RESERVE_PARA_865         	865	//	ARM预留用参数865
#define	ARM_RESERVE_PARA_866         	866	//	ARM预留用参数866
#define	ARM_RESERVE_PARA_867         	867	//	ARM预留用参数867
#define	ARM_RESERVE_PARA_868         	868	//	ARM预留用参数868
#define	ARM_RESERVE_PARA_869         	869	//	ARM预留用参数869
#define	DSP_HEARTBEAT_COUNT1         	870	//	DSP心跳翻转计数器（DSP每2ms翻转一次心跳位,ARM每1ms轮询一帧数据）
#define	ARM_RESERVE_PARA_871         	871	//	ARM预留用参数871
#define	ARM_RESERVE_PARA_872         	872	//	ARM预留用参数872
#define	ARM_RESERVE_PARA_873         	873	//	ARM预留用参数873
#define	AI2P_SEL                    	874	//	AI2P功能选择
#define	AI2P_DISP                    	875	//	AI2P反馈
#define	AI2P_OFFSET                  	876	//	AI2P零偏
#define	AI2P_SCALE                   	877	//	AI2P增益
#define	ARM_RESERVE_PARA_878         	878	//	ARM预留用参数878
#define	ARM_RESERVE_PARA_879         	879	//	ARM预留用参数879
#define	ARM_RESERVE_PARA_880         	880	//	ARM预留用参数880
#define	ARM_RESERVE_PARA_881         	881	//	ARM预留用参数881
#define	MOTO_ADDR_DISPL              	882	//	旋变存盘的最新地址低16位送上位机显示
#define	MOTO_ADDR_DISPH              	883	//	旋变存盘的最新地址高16位送上位机显示
#define	MOTO_ANGL_DISPL              	884	//	旋变存盘的最新数值L16位送上位机显示
#define	MOTO_ANGL_DISPH              	885	//	旋变存盘的最新数值H16位送上位机显示
#define	MOTO_ANGL_TEST               	886	//	旋变存盘角度测试8=写入；9=擦除
#define	MOTO_ANGL_DISPC              	887	//	旋变存盘的最新数值C16位送上位机显示
#define	MOTO_ANGL_SAVE_MIN_VOL       	888	//	旋变掉电保存母线下限值 已固化90V
#define	MOTO_ANGL_SAVE_MAX_VOL       	889	//	旋变掉电保存母线上限值 已固化160V
#define	FASTVOL                      	890	//	快速上传的母线电压值
#define	CRC_ERRCNT                   	891	//	CRC错误计数器
#define	CRC_RIGCNT                   	892	//	CRC正确计数器
#define	P_MAX_V                      	893	//	位置环下的最高转速
#define	SSI_ANGLE                    	894	//	SSI角度
#define	MOTO_ANGLE_SET               	895	//	使用此参数设置旋变角度：上电默认=65535，除此数值以外任何角度都可设置
#define	SAFE_SPEED_ADD               	896	//	安全收桨速度切换递增值
#define	USERPARA5                   	897	//	用户参数5
#define	USERPARA6                   	898	//	用户参数6
#define	USERPARA7                   	899	//	用户参数7
#define	USERPARA8                   	900	//	用户参数8
#define	USERPARA9                   	901	//	用户参数9
#define	USERPARA10                   	902	//	用户参数10
#define	ERRSAVE_ADDR_ERASE           	903	//	历史故障扇区擦除：上电若不等于0xAA55就执行一次历史故障信息扇区擦除操作，随后保存此参数为0xAA55
#define	USERPARAVALIFYDATA0           	904	//	用户参数扇区头地址校验字节，正常时应为：0x080C 0000=0xAA、0x080C 0001=0x55
#define	USERPARAVALIFYDATA1           	905	//	用户参数扇区尾地址校验字节，正常时应为：0x080D FFFE=0xAA、0x080D FFFF=0x55
#define	DEFTPARAVALIFYDATA0           	906	//	出厂参数扇区头地址校验字节，正常时应为：0x080E 0000=0xAA、0x080E 0001=0x55
#define	DEFTPARAVALIFYDATA1           	907	//	出厂参数扇区尾地址校验字节，正常时应为：0x080F FFFE=0xAA、0x080F FFFF=0x55
#define	PARAVALIFYERR                	908	//	0=校验全部正确；1=用户参数头地址校验错误；2=用户参数头尾地址校验错误；3=用户和出厂参数头地址校验错误；4=用户和出厂参数头尾地址校验错误
#define	ARM_RESERVE_PARA_909         	909	//	ARM预留用参数_909
#define	CABINET_TEMP_ADJ0            	910	//	PT100通道0校准参数
#define	CABINET_TEMP_ADJ1            	911	//	PT100通道1校准参数
#define	CABINET_TEMP_ADJ2            	912	//	PT100通道2校准参数
#define	CABINET_TEMP_ADJ3            	913	//	PT100通道3校准参数
#define	SAFERUN_T1T2                 	914	//	分段式收桨第1、第2段时间 单位S
#define	RESERVE_915                 	915	//	预留参数_915
#define	SAFERUN_V1V2                	916	//	分段式收桨第1、第2段速度百分比 单位X%
#define	RESERVE_917                 	917	//	预留参数_917
#define	CONFIGWORDS                  	918	//	Bit3~Bit0为分段式收桨功能、Bit7~Bit4为轮毂温度和C+C-湿度分时复用：A=启用；其他=禁止
#define	SAFERUN_MULTIPLE_CNT          	919	//	分段式收桨功能计时器 单位S
#define	OUT8_SEL                     	920	//	OUT08输出功能选择
#define	OUT9_SEL                     	921	//	OUT09输出功能选择
#define	OUT10_SEL                    	922	//	OUT10输出功能选择
#define	OUT11_SEL                    	923	//	OUT11输出功能选择
#define	OUT12_SEL                    	924	//	OUT12输出功能选择
#define	OUT13_SEL                    	925	//	OUT13输出功能选择
#define	OUT14_SEL                    	926	//	OUT14输出功能选择
#define	OUT15_SEL                    	927	//	OUT15输出功能选择
#define	CABINET_TEMP_ADJ4            	928	//	PT100通道4校准参数
#define	CABINET_TEMP_ADJ5            	929	//	PT100通道5校准参数
#define	TEMP3                        	930	//	T3温度
#define	TEMP4                        	931	//	T4温度
#define	CABINET_TEMPx                	932	//	Tx温度(预留)
#define	ABPULSENUM                   	933	//	AB编码器线数
#define	ABSPEED                      	934	//	AB编码器实际转速，精度0.1RPM
#define	ABSPEEDFILTER                	935	//	AB编码器转速滤波系数
#define	ABPARA1                      	936	//	AB编码器预留用参数1
#define	ABPARA2                      	937	//	AB编码器预留用参数2
#define	ABPARA3                      	938	//	AB编码器预留用参数3
#define	ABPARA4                      	939	//	AB编码器预留用参数4
#define	ABPARA5                      	940	//	AB编码器预留用参数5
#define	AI1P_DISP                    	941	//	AI1P反馈
#define	AI1P_OFFSET                  	942	//	AI1P零偏
#define	AI1P_SCALE                   	943	//	AI1P增益
#define	PITCH_ANGLE                  	944	//	桨叶角度(精度0.01°)
#define	PITCH_ANGLE1                 	945	//	桨叶角度(精度1°，仅用于监控位置超限3200°)
#define	DOUBLERAM_DATA_01            	946	//	星辰上位机可同步监控DoubleRAM.X
#define	DOUBLERAM_DATA_02            	947	//	星辰上位机可同步监控DoubleRAM.X
#define	DOUBLERAM_DATA_03            	948	//	星辰上位机可同步监控DoubleRAM.X
#define	DOUBLERAM_DATA_04            	949	//	星辰上位机可同步监控DoubleRAM.X
#define	DOUBLERAM_DATA_05            	950	//	星辰上位机可同步监控DoubleRAM.X
#define	DOUBLERAM_DATA_06            	951	//	星辰上位机可同步监控DoubleRAM.X
#define	DOUBLERAM_DATA_07            	952	//	星辰上位机可同步监控DoubleRAM.X
#define	DOUBLERAM_DATA_08            	953	//	星辰上位机可同步监控DoubleRAM.X
#define	DOUBLERAM_DATA_09            	954	//	星辰上位机可同步监控DoubleRAM.X
#define	DOUBLERAM_DATA_10            	955	//	星辰上位机可同步监控DoubleRAM.X
            	                      		//	ARM预留用参数_956~959（最大ID号SYS_PARA_LEN = 999）											
#define	GYRO_ACC_X                   	960	//	陀螺仪X轴加速度
#define	GYRO_ACC_Y                   	961	//	陀螺仪Y轴加速度
#define	GYRO_ACC_Z                   	962	//	陀螺仪X轴加速度
#define	GYRO_ACC_X_GAIN              	963	//	陀螺仪X轴加速度增益
#define	GYRO_ACC_Y_GAIN              	964	//	陀螺仪Y轴加速度增益
#define	GYRO_ACC_Z_GAIN              	965	//	陀螺仪Z轴加速度增益
#define	GYRO_ACC_X_OFFSET            	966	//	陀螺仪X轴加速度零偏
#define	GYRO_ACC_Y_OFFSET            	967	//	陀螺仪Y轴加速度零偏
#define	GYRO_ACC_Z_OFFSET            	968	//	陀螺仪Z轴加速度零偏
#define	GYRO_ANG_X                   	969	//	陀螺仪X轴角速度
#define	GYRO_ANG_Y                   	970	//	陀螺仪Y轴角速度
#define	GYRO_ANG_Z                   	971	//	陀螺仪Z轴角速度
#define	GYRO_ANG_X_OFFSET            	972	//	陀螺仪X轴角速度零偏
#define	GYRO_ANG_Y_OFFSET            	973	//	陀螺仪Y轴角速度零偏
#define	GYRO_ANG_Z_OFFSET            	974	//	陀螺仪Z轴角速度零偏
#define	GYRO_TEMP                    	975	//	陀螺仪芯片
#define	GYRO_CHIP                    	976	//	陀螺仪芯片
#define	GYRO_CODE                    	977	//	陀螺仪芯片
#define	GYRO_SPARE1                  	978	//	陀螺仪预留参数1
#define	GYRO_SPARE2                  	979	//	陀螺仪预留参数2
#define	GYRO_SPARE3                  	980	//	陀螺仪预留参数3
            	                      		//	ARM预留用参数_981~999（最大ID号SYS_PARA_LEN = 999）	
            	                      		//	1000~1299为ARM用历史故障信息参数号、1300~1419为自学习用参数号
											
//充电器用参数				
#define	CP_ALARM_TRIG0               	0	//	预留
#define	CP_ALARM_TRIG5               	0	//	DY
#define	CP_ALARM_TRIG8               	0	//	硬过压
#define	CP_ALARM_TRIG9               	0	//	硬过流1
#define	CP_ALARM_TRIG11              	0	//	硬过流2
#define	CP_SOFT_VER                  	1	//	软件版本号
#define	CP_HARD_VER                  	2	//	硬件版本号
#define	CP_DEFECT_CODE               	3	//	告警码
#define	CP_SYS_ST_CODE               	4	//	系统状态
#define	CP_VOL                       	5	//	母线电压
#define	CP_KEB_FAN                   	6	//	抱闸风机开启命令
#define	CP_V_FEEDBACK                	7	//	电容电压
#define	CP_I_FEEDBACK                	8	//	充电电流
#define	CP_IGBT_TEMP                 	9	//	IGBT温度
#define	CP_ALARM_TRIG7               	12	//	IGBT温度
#define	CP_ERR_RST                   	18	//	故障手动复位
#define	CP_V_SET                     	20	//	电压设定
#define	CP_I_SET                     	21	//	电流设定
#define	CP_ALARM_EN                  	22	//	充电器故障允许
#define	CP_DC24V_TEMP                	25	//	充电器DC24V电源温度
#define	CP_INTERSPACE_TEMP           	27	//	充电器内腔温度
#define	CP_V_KP                      	30	//	电压环比例系数
#define	CP_V_KI                      	31	//	电压环积分系数
#define	CP_I_KP                      	32	//	电流环比例系数
#define	CP_I_KI                      	33	//	电流环积分系数
#define	CP_ADREFADJUST               	34	//	AD采样基准值
#define	CP_FREADJUST                 	35	//	频率校准系数
#define	CP_VRESADJUST                	36	//	电流采样电阻分压系数
#define	CP_CURADJUST                 	37	//	电流校准系数
#define	CP_ALARM_TRIG1               	39	//	充电器输出过压
#define	CP_ALARM_TRIG10              	42	//	母线过压
#define	CP_ALARM_TRIG6               	43	//	母线欠压
#define	CP_CAP                       	52	//	超级电容容量 20000对应2F
#define	CP_ALARM_TRIG2               	53	//	充电器过流
#define	CP_ALARM_TRIG3               	54	//	输出短路过流
#define	CP_ALARM_TRIG4               	58	//	输出断路压差
#define	CP_ALARM_EN_TIME1            	109	//	置位延时
#define	CP_ALARM_RST_TIME1           	110	//	复位延时
#define	CP_ALARM_EN_TIME2            	111	//	置位延时
#define	CP_ALARM_RST_TIME2           	112	//	复位延时
#define	CP_ALARM_EN_TIME3            	113	//	置位延时
#define	CP_ALARM_RST_TIME3           	114	//	复位延时
#define	CP_ALARM_EN_TIME4            	115	//	置位延时
#define	CP_ALARM_RST_TIME4           	116	//	复位延时
#define	CP_ALARM_EN_TIME5            	117	//	置位延时
#define	CP_ALARM_RST_TIME5           	118	//	复位延时
#define	CP_ALARM_EN_TIME6            	119	//	置位延时
#define	CP_ALARM_RST_TIME6           	120	//	复位延时
#define	CP_ALARM_EN_TIME7            	121	//	置位延时
#define	CP_ALARM_RST_TIME7           	122	//	复位延时
#define	CP_ALARM_EN_TIME8            	123	//	置位延时
#define	CP_ALARM_RST_TIME8           	124	//	复位延时
#define	CP_ALARM_EN_TIME9            	125	//	置位延时
#define	CP_ALARM_RST_TIME9           	126	//	复位延时
#define	CP_ALARM_EN_TIME10           	127	//	置位延时
#define	CP_ALARM_RST_TIME10          	128	//	复位延时
#define	CP_ALARM_AUTO_RST_EN         	129	//	自动复位设置
#define	CP_ALARM_EN_TIME11           	131	//	硬过流
#define	CP_ALARM_RST_TIME11          	132	//	硬过流
#define	CP_ADC_RESET_TIMES          	160	//	充电器AD模块复位次数
#define	CP_BTMVOL_OFFSET             	174	//	BTM电压校准零偏
#define	CP_BTMVOL_GAIN               	175	//	BTM电压校准增益
#define	CP_BTMVOL_FEEDBACK           	176	//	BTM电压反馈：精度1V
#define	CP_V_SET_BRAKER              	190	//	抱闸电压设定：205代表205V
#define	CP_V_FEEDBACK_BRAKER         	192	//	抱闸电压反馈：205代表205V
#define	CP_SN_BRAKER                 	194	//	抱闸使能控制：0=关抱闸；其他=开抱闸
#define	CP_ARM_BUILD_YEAR            	197	//	充电器的ARM编译年份
#define	CP_ARM_BUILD_DATE            	198	//	充电器的ARM编译月日
#define	CP_ARM_BUILD_TIME            	199	//	充电器的ARM编译时分
#define	CP_AC_VOL                    	244	//	R相电压的读取
#define	CP_SCR_CTL                   	246	//	半控桥控制
#define	CP_AC_LOWVOL                 	255	//	电网欠压值
#define	CP_AC_LOWVOL_BACK            	256	//	电网欠压回差值
#define	CP_AC_OVERVOL                	257	//	电网过压值
#define	CP_AC_OVERVOL_BACK           	258	//	电网过压回差值
#define	CP_CAPTEST_CMD               	272	//	容量测试命令：0x000A代表启动容量测试，0x0000停止容量测试
				
//注意：#define PWR_PARA_LEN 宏定义数值 需大于等于充电器用参数最大ID号。上限280！				

#endif

