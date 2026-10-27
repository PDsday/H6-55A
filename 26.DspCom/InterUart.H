#ifndef	_INTERUART_
#define	_INTERUART_
#include "stm32f4xx.h"

#define	INUART_TX_DATA_FRAMELEN	12+2			//发送数据帧长度
#define	INUART_RX_DATA_FRAMELEN	12+4				//接收数据帧长度

#define	SEND_CMDH_NO		0			//CMDH在发送数据缓冲区中的位置
#define	SEND_CMDL_NO		1			//CMDL在发送数据缓冲区中的位置
#define	SEND_DATAH_NO		2			//DATAH在发送数据缓冲区中的位置
#define	SEND_DATAL_NO		3			//DATAL在发送数据缓冲区中的位置
#define	SEND_SETH_NO		4			//SETH在发送数据缓冲区中的位置
#define	SEND_SETL_NO		5			//SETL在发送数据缓冲区中的位置
#define	SEND_EXSETH_NO		6			//扩展SETH在发送数据缓冲区中的位置
#define	SEND_EXSETL_NO		7			//扩展SETL在发送数据缓冲区中的位置
#define	SEND_STMSTATUSH_NO	8			//STM状态H在发送数据缓冲区中的位置
#define	SEND_STMSTATUSL_NO	9			//STM状态L在发送数据缓冲区中的位置
#define	SEND_DECODEH_NO	   10			//高十六位故障码H在发送数据缓冲区中的位置
#define	SEND_DECODEL_NO	   11			//高十六位故障码L在发送数据缓冲区中的位置
#define	SEND_AUTOH_NO	   12			//高十六位故障码H在发送数据缓冲区中的位置
#define	SEND_AUTOL_NO	   13			//高十六位故障码L在发送数据缓冲区中的位置

#define	REV_DATAH_NO		0			//DATAH在接收数据缓冲区中的位置
#define	REV_DATAL_NO		1			//DATAL在接收数据缓冲区中的位置
#define	REV_VFBH_NO			2			//速度反馈H在接收数据缓冲区中的位置
#define	REV_VFBL_NO			3			//速度反馈L在接收数据缓冲区中的位置
#define	REV_TSETH_NO		4			//力矩设定H在接收数据缓冲区中的位置
#define	REV_TSETL_NO		5			//力矩设定L在接收数据缓冲区中的位置
#define	REV_IFBH_NO			6			//电流反馈H在接收数据缓冲区中的位置
#define	REV_IFBL_NO			7			//电流反馈L在接收数据缓冲区中的位置
#define	REV_DSPSTATUSH_NO	8			//DSP状态H在接收数据缓冲区中的位置
#define	REV_DSPSTATUSL_NO	9			//DSP状态L在接收数据缓冲区中的位置

#define	REV_POSITIONLL_NO	10			//位置码H在接收数据缓冲区中的位置
#define	REV_POSITIONLH_NO	11			//位置码L在接收数据缓冲区中的位置
#define	REV_POSITIONHH_NO	12			//位置圈数H在接收数据缓冲区中的位置
#define	REV_EX_NO			13			//位置圈数L在接收数据缓冲区中的位置

struct	INUART_COMMAND_BIT{
u16 PARAID:14;			//参数ID					BIT0~13
u16 CONTENT:1;			//命令/ID	0 ID   1 命令	BIT14
u16 RWBIT:1;			//读写标志 	0 读   1 写		BIT15
};
union	INUART_COMMAND{
u16 all;
struct	INUART_COMMAND_BIT	bit;
};

struct	STMSTATUS_BIT{
u16 IO_IN0:1;				//STM硬件数字输入0
u16 IO_IN1:1;				//Bit01：ARM呼叫DSP升级（DSP侧命名IO_CANopenUpdate）；绝对不允许置位，否则DSP进入Bootloader
u16 IO_IN2:1;				//STM硬件数字输入2
u16 IO_IN3:1;				//STM硬件数字输入3
u16 IO_IN4:1;				//STM硬件数字输入4
u16 IO_IN5:1;				//STM硬件数字输入5
u16 IO_IN6:1;				//STM硬件数字输入6
u16 IO_IN7:1;				//STM硬件数字输入7
u16 STM_SN_CTRL:1;			//STM使能控制权
u16 STM_SN_CTRLFLAG:1;		//STM使能状态
u16 STM_SET_CTRL:1;			//STM设定控制权
u16 MOTOGR:1;			    //STM温度过热（模拟信号处理结果）
u16 DefectCodeClear:1;		//BIT12	1清除故障告警
u16 SSIBK:1;				//BIT13	电机温度断线改
u16 rsvd1:2;				//BIT14~15	保留功能 	

};
union	STMSTATUS{
u16 all;
struct	STMSTATUS_BIT	bit;
};

void InUart_Configuration(void);
extern u8 DSPReturnPCFlag; // DSP返回数据标志：0未返回数据，1已返回数据
extern u8 InUartErrFlag; // 与DSP通讯失败标志：1失败

#endif
