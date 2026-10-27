#ifndef	_TASKSCHEDULER_
#define	_TASKSCHEDULER_
#include "stm32f4xx_conf.h"

struct TICK1FLAG_BIT{
u16 RS232:1;				//232超时节拍标志
u16 RS485:1;				//485超时节拍标志
u16 SLOW:1;					//低速层节拍标志
unsigned int resvd1:8;		//保留
};

union TICK1FLAG{
struct TICK1FLAG_BIT bit;
unsigned int all;
};

#define  USER_TASK_NUM	  10	   	// 定义用户任务数量
typedef struct
{
    u8   TaskStartFlag;
    u16  TaskLoopTime;
    u16  TaskTimeCnt;
	void (*TaskHand)();
} USERTASK_TypeDef;		  			// 定义用户任务类型

//Lxxxx故障（DSP处理的故障）
struct	DEFECT_BITL
{
	unsigned int SERVOOVERLOAD:1;	//BIT0	驱动器超载
	unsigned int OVERVOL:1;			//BIT1	母线过压
	unsigned int LOWVOL:1;			//BIT2	母线欠压或DY异常
	unsigned int DOGRESETDSP:1;		//BIT3	看门狗故障	硬件看门狗复位DSP
	unsigned int POSLOS:1;			//BIT4	位置码异常	
	unsigned int RESERVE5:1;		//BIT5	抱闸电流超限
	unsigned int IGBTGR2:1;			//BIT6	IGBT过热2	IGBT过热2（95°，抱闸停机）
	unsigned int IGBTGR1:1;			//BIT7	IGBT过热1	IGBT过热1（90°，忽略）
	unsigned int MOTOOVERLOAD:1;	//BIT8	电机超载
	unsigned int FAULT:1;			//BIT9	驱动器过流	Fault低电平
	unsigned int ERRSTOP:1;			//BIT10	驱动器过流	Comp
	unsigned int DLSK:1;			//BIT11	电流超差
	unsigned int EEPROMWRITEERR:1;	//BIT12	桨叶角度写入错误
	unsigned int EEPROMREADERR:1;	//BIT13	读出错误	
	unsigned int SDSK:1;			//BIT14:速度超差
	unsigned int ARMCOMMERR:1;		//BIT15:与ARM通讯故障
};

union	DEFECT_FLAGL{
unsigned int all;
struct	DEFECT_BITL	bit;
};

//Hxxxx故障（ARM处理的故障）
struct	DEFECT_BITH
{
	unsigned int RESERVE00:1;		//BIT0	预留
	unsigned int CANOPENOFFLINE:1;	//BIT1	CANOPEN断线
	unsigned int BKPOWERLOWVOL:1;	//BIT2  24V辅源低压（5.0A抱闸电源）
	unsigned int POWER24VLOWVOL:1;	//BIT3	开关电源欠压（7.5A开关电源）	
	unsigned int RESERVE04:1;		//BIT4	预留
	unsigned int IGBTOVERTEMP11:1;	//BIT5	IGBT过热1：
	unsigned int MOTOOVERTEMP1:1;	//BIT6	电机过热1：超过130°，回差10°
	unsigned int RESERVE07:1;		//BIT7	预留
	unsigned int CHARGECOMMERR:1;	//BIT8	与充电器通讯故障
	unsigned int CHARGEERR:1;      	//BIT9	充电器有任何故障
	unsigned int RESERVE10:1;		//BIT10	预留
	unsigned int DC24VOVERTEMP:1;   //BIT11 电源板7.5A开关电源过温
	unsigned int RESERVE12:1;    	//BIT12	预留
	unsigned int CHARGEOVERTEMP:1;  //BIT13 24V抱闸电源过温：取充电器IGBT过温故障
	unsigned int DO8OVERLOAD:1;  	//BIT14 DO8超载
	unsigned int RESERVE15:1;		//BIT15 预留
};

union	DEFECT_FLAGH{
unsigned int all;
struct	DEFECT_BITH	bit;
};

//Mxxxx故障（ARM处理的故障）
struct	DEFECT_BITM
{
	unsigned int ENOPERR:1;			//BIT0	ENPO
	unsigned int OVERPOSITION:1;	//BIT1	桨叶位置超限
	unsigned int FLASHVALIFYERR:1;	//BIT2  主ARM、辅ARM存盘参数头部或尾部校验字节不等于0xAA55 
	unsigned int RESERVE03:1;		//BIT3	
	unsigned int RESERVE04:1;		//BIT4
	unsigned int RESERVE05:1;		//BIT5
	unsigned int RESERVE06:1;		//BIT6
	unsigned int RESERVE07:1;		//BIT7
	unsigned int RESERVE08:1;		//BIT8
	unsigned int RESERVE09:1;		//BIT9
	unsigned int RESERVE10:1;		//BIT10
	unsigned int RESERVE11:1;		//BIT11
	unsigned int RESERVE12:1;		//BIT12
	unsigned int RESERVE13:1;		//BIT13
	unsigned int RESERVE14:1;		//BIT14
	unsigned int RESERVE15:1;		//BIT15
};

union	DEFECT_FLAGM{
unsigned int all;
struct	DEFECT_BITM	bit;
};

#define DEFECTMASK  	0xffff
#define DEFECTMASKH 	0xffff 
#define DEFECTMASKC  	0xffff
#define DEFECTMASKM  	0xffff

struct ERR_STATE_FILTER
{
	u16		HigCnt1; 
	u16 	HigCnt2; 
	u16 	HigCnt3;
	u16 	HigCnt4;
	u16 	HigCnt5;
	u16 	HigCnt6;
	u16 	HigCnt7;
	u16 	HigCnt8;
	u16 	HigCnt9;
	u16 	HigCnt10;
	u16 	HigCnt11;
	u16 	HigCnt12;
	u16 	HigCnt13;
	u16 	HigCnt14;

	u16 	LowCnt1;
	u16 	LowCnt2;
	u16 	LowCnt3;
	u16 	LowCnt4;
	u16 	LowCnt5;
	u16 	LowCnt6;
	u16 	LowCnt7;
	u16 	LowCnt8;
	u16 	LowCnt9;
	u16 	LowCnt10;
	u16 	LowCnt11;  
	u16 	LowCnt12;
	u16 	LowCnt13;
	u16 	LowCnt14;
};

void AddUserTask(void (*pUserTask)(), u16 UesrTaskLoopTime, u8 UesrTaskPriority);
void UserTaskSchedule(void);
void UserTaskPrecess(void);

#endif
