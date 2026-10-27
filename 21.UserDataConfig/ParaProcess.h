#ifndef	_PARAPROCESS_
#define	_PARAPROCESS_
// #include "stm32f4xx.h"
#include "stm32f4xx.h"
extern u16 DefaultParaTableNum;

struct	ATTR_BITS	{
u16 WREN:1;			//1 允许用户更改  		0  不允许用户更改	BIT0
u16 RUNWREN:1;			//1 允许运行时更改  	0  不允许运行时更改	BIT1
//u16 ADJWREN:1;			//1 允许调试时更改  	0  不允许调试时更改	BIT2
u16 SIGN:1;			//1 有符号  			0  无符号			BIT3
u16 rsvd1:12;			//保留										BIT4~15
};

union	ATTR	{
struct	ATTR_BITS	bit;		
u16 all;
};

struct	SysPara	{
u16		DATA;		//参数数据（均按无符号数存放，使用时按属性确定其具体含义）
union	ATTR		ATTR;		//参数属性
u16		UPLIMIT;	//参数上限
u16		LOWLIMIT;	//参数下限
};

typedef struct	DefaultSysPara	{
    u16     ParaID;     //参数ID
    u16		DATA;		//参数数据（均按无符号数存放，使用时按属性确定其具体含义）
    union	ATTR		ATTR;		//参数属性
    u16		UPLIMIT;	//参数上限
    u16		LOWLIMIT;	//参数下限
}DefaultSysParaType;
typedef struct	DefectConfig{
	u16 DefectBit;//在故障码中的位置位的代号0
	u16 EnID;     //1允许0禁止1
	u16 DefModeID;//2 0为关断脉冲抱闸停机；1立即减速停机；2为驱动器进入收桨运行模式；3为忽略故障；4开环收桨
	u16 EnDlyID;  //3 故障置位延时
	u16 RstDlyID; //4 复位类型为自动复位时有效 复位延时
	u16 RstTypID; //5 0为手动复位；1：为自动复位
	u16 EnTrigID; //6 故障触发条件
	u16  SafeID;  //7 0不断开1断开
	u16 CountEN;  //8 定时器的置1定时计数
	u16 CountRST;  //9定时器的置0定时计数
	u16 DefectStateTemp;//10第一次判断后故障状态
	u16 DefectState;    //11返回状态
}DefectConfigType;

void DefectProcess(u64 in,DefectConfigType *DefectTable  );
void DefectParaProcess(DefectConfigType *DefectTable  );
extern void	CalculPara1(void);
#endif

