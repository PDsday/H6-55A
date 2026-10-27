#define IO_GLOBALS
#include "ExternPrototype.h"
#include "IO.h"
#include "InterUart.H"
#include "state_machine.h"
#include "AppFunc.h"

#ifndef TEST_FOR_STARS				// 未定义星辰厂内测试版
	#include "AppInputOutPut.h"
#else
#endif

#define	FANONMIN				3							// 内腔风机间歇开启时间，3min
#define	FANOFMIN				10							// 内腔风机间歇关闭时间，10min
#define	FANONCNT				FANONMIN * 60				// 内腔风机间歇开启时间，180sec
#define	FANOFCNT				FANOFMIN * 60				// 内腔风机间歇关闭时间，600sec
#define	FANONOFCNT				FANONCNT + FANOFCNT 		// 内腔风机间歇启停时间，780sec
#define FANONTEMP 				40							// 内腔风机启动温度
//u16 FANONTEMP =				40;							// 内腔风机启动温度
#define FANOFBACKLASH 			5 							// 内腔风机关闭回差
#define FANOFTEMP 				(FANONTEMP - FANOFBACKLASH)	// 内腔风机关闭温度

u8 BK_Buf = 0;
u8 FanOnFlag = 0;
u16 FanOnOffCnt = 0;
u16 Fan0ErrCnt = 0;
u16 Fan1ErrCnt = 0;
u64 DecodeAll = 0;

bool Fan0ErrFlag = 0;
bool Fan1ErrFlag = 0;
bool PWR1 = 0;
bool TimeBase_1Secon;

struct TONTOF_CNT TonCnt;		
struct TONTOF_CNT TofCnt;	
union DI_STATUS	ReadDI;
union DI_STATUS	DIStatus;

extern union DEFECT_FLAGH DefectCodeH;					//故障输入
extern union DEFECT_FLAGL DefectFlagInL;				//故障输入
extern union DEFECT_FLAGH DefectFlagInH;				//故障输入
extern union DEFECT_FLAGL DefectFlagOutL;				//故障输出
extern union DEFECT_FLAGH DefectFlagOutH;				//故障输出
extern union DEFECT_FLAGH DefectFlagH;					//故障输出最终输出
extern union IO_IN DigitInput;
extern bool TimeBase_3Secon_ForFan0Err;
extern bool TimeBase_3Secon_ForFan1Err;
extern bool User_ID02;
extern bool RST_QY;     								//电网欠压
extern bool RST_GY;     								//电网过压
extern u64 SafeRunEn;									//安全收桨
extern u64 StopRunEn;									//停机抱闸
extern u64 NormRunEn;									//正常运行
extern u64 SNstopRunEn;									//抱闸停机
extern u64 SafeOutFlag;									//开环收桨
extern u8 InUart2ErrFlag; 								//与充电器通讯失败标志
extern u16 ChargeRunEnable;
extern u16 MotoOffLineFlag;
extern u16 Controlword_Out;
extern u8 Init_flag1;
extern u8 Init_flag2;
extern u8 DSP_Status;
extern s16 BK_Curre;
extern u8 Init_flag;
extern u16 Bk205_Vol;
extern u8 State_Machine;
extern u8 err_rst1;
extern u8 FanOnForce;									//1=HMI强制关闭内腔风机；2=HMI强制开启内腔风机
extern u16 CrcPassFlag;
extern u16 PcCtrl; 										//上位机控制标志：1上位机控制，0上位机不控制
extern u16 PcCtrlFlag; 									//上位机开关使能标志：1开使能，0关使能
extern u16 PcCtrlFlag_Out; 								//上位机开关使能标志：1开使能，0关使能
extern s32 MotoRpmSpeed;
extern u16 Func_0x03_Data[];
extern u16 Func_0x06_Data[];
extern u16 Func_0x10_Data[];
extern u16 CANAddr_Real; 	

/*******************************************************************************
* 函数名 : PWR3_Configuration
* 描  述 : 配置PWR3口为输入口
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PWR3_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* PWR3(IO口24V电源检测)输入*/
    GPIO_InitStructure.GPIO_Pin = GPIO_PWR3_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_PWR3, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名 : GPIO_Configuration
* 描  述 : 配置不同的GPIO端口
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    /* LED输出*/
    GPIO_InitStructure.GPIO_Pin = GPIO_LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_LED, &GPIO_InitStructure);
	LEDOFF;
	
	/* DO8反馈输入*/
    GPIO_InitStructure.GPIO_Pin = GPIO_DO8FEB_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_DO8FEB, &GPIO_InitStructure);
	
    /* DI16输入*/
    GPIO_InitStructure.GPIO_Pin = GPIO_DI16_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_DI16, &GPIO_InitStructure);

// 	/* PWR3(IO口24V电源检测)输入*/
// 	GPIO_InitStructure.GPIO_Pin = GPIO_PWR3_PIN;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
// 	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
// 	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
// 	GPIO_Init(GPIO_PWR3, &GPIO_InitStructure);

    /* WKO输出*/
    GPIO_InitStructure.GPIO_Pin = GPIO_WKO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_WKO, &GPIO_InitStructure);
	WKO_HIGH;

    /* WKI输入*/
    GPIO_InitStructure.GPIO_Pin = GPIO_WKI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_WKI, &GPIO_InitStructure);

    /* 先配置DSP编程设置脚TAG为输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_DSP_PROGRAMER_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_DSP_PROGRAMER, &GPIO_InitStructure);
    TAGHIGH;

    /* 再配置DSP编程设置脚TAG为输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_DSP_PROGRAMER_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_DSP_PROGRAMER, &GPIO_InitStructure);

    /* 底座风机故障检测输入*/
    GPIO_InitStructure.GPIO_Pin = GPIO_FAN0IN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_FAN0IN, &GPIO_InitStructure);

    /* 内腔风机故障检测输入*/
    GPIO_InitStructure.GPIO_Pin = GPIO_FAN1IN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_FAN1IN, &GPIO_InitStructure);

    /* AI0电流/电压可编程切换输出*/
    GPIO_InitStructure.GPIO_Pin = GPIO_AI0CONTROL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_AI0CONTROL, &GPIO_InitStructure);

    /* AI1电流/电压可编程切换输出*/
    GPIO_InitStructure.GPIO_Pin = GPIO_AI1CONTROL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_AI1CONTROL, &GPIO_InitStructure);

    /* AI2电流/电压可编程切换输出*/
    GPIO_InitStructure.GPIO_Pin = GPIO_AI2CONTROL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_AI2CONTROL, &GPIO_InitStructure);
	
    /* 内腔风机控制输出*/
    GPIO_InitStructure.GPIO_Pin = GPIO_FAN1OUT_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_FAN1OUT, &GPIO_InitStructure);
    FANOFF;

    /* 配置抱闸输出脚： BK(PE.8) */
    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_8);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    BKOFF;
	
    /* 配置抱闸输出脚： SSI_24V输出(PG.7) */
    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_7);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    SSI_24V_OFF;

    /* 配置输入口锁存信号输出脚：IO_IN_ST */
    GPIO_InitStructure.GPIO_Pin = (GPIO_IO_IN_ST_PIN);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_IO_IN_ST, &GPIO_InitStructure);

    /* 配置IO时钟输出脚：IO_CLK */
    GPIO_InitStructure.GPIO_Pin = (GPIO_IO_CLK_PIN);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_IO_CLK, &GPIO_InitStructure);

    /* 配置输出口使能信号输出脚：IO_OUT_EN */
    SET_IO_OUT_EN; // 使能脚置高，不允许输出
    GPIO_InitStructure.GPIO_Pin = (GPIO_IO_OUT_EN_PIN);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_IO_OUT_EN, &GPIO_InitStructure);
    SET_IO_OUT_EN; // 使能脚置高，不允许输出

    /* 配置输出口锁存信号输出脚：IO_OUT_ST */
    GPIO_InitStructure.GPIO_Pin = (GPIO_IO_OUT_ST_PIN);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_IO_OUT_ST, &GPIO_InitStructure);

    /* 配置数据输出脚：IO_TX */
    GPIO_InitStructure.GPIO_Pin = (GPIO_IO_TX_PIN);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_IO_TX, &GPIO_InitStructure);

    /* 配置数据输入脚：IO_RX */
    GPIO_InitStructure.GPIO_Pin = (GPIO_IO_RX_PIN);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_IO_RX, &GPIO_InitStructure);

	{
		u16 i;
		u16 CANAddr_Old = 0;
		u16 Read_CANAddr_Times = 0;
		u32 InData = 0;
		u8 ID20_Temp = 0;
		u8 ID21_Temp = 0;
		u8 ID22_Temp = 0;
		u8 ID23_Temp = 0;

		while (Read_CANAddr_Times < 100)
		{
			CLEAR_IO_IN_ST; 	// 输入锁存
			SET_IO_IN_ST; 		// 输入保持
			CLEAR_IO_CLK; 		// IO时钟输出置低
			
			for (i = 0; i < 24; i++)
			{
				InData <<= 1;
				InData |= (READ_IO_RX & 1);

				CLEAR_IO_CLK; 	// IO时钟输出置低
				SET_IO_CLK;   	// IO时钟输出置高
			}

			ID20_Temp = (InData & (1<<20)) && 1;
			ID21_Temp = (InData & (1<<21)) && 1;
			ID22_Temp = (InData & (1<<22)) && 1;
			ID23_Temp = (InData & (1<<23)) && 1;

			CANAddr = ID20_Temp * 8 + ID21_Temp * 4 + ID22_Temp * 2 + ID23_Temp * 1;
			
			if (CANAddr == CANAddr_Old)
			{
				Read_CANAddr_Times ++;
			}
			else if (Read_CANAddr_Times > 0)
			{
				//Read_CANAddr_Times --;
				Read_CANAddr_Times = 0;
			}
			CANAddr_Old = CANAddr;
		}
		
		DoubleRAM.CanAddr = CANAddr;
	}
}

/*******************************************************************************
* 函数名 : GPIO_Configuration1
* 描  述 : 将串口配置为普通IO口
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void GPIO_Configuration1(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure USART2 Tx (PD.05) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Configure USART2 Rx (PD.06) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名 : TON_TOF
* 描  述 : 双态标准滤波器
* 输  入 : DIx当前输入状态，DIx上一次锁定状态，高电平状态计数器，低电平状态计数器，高电平状态锁定滤波值，低电平状态锁定滤波值
* 输  出 : 
* 返  回 : BitStatus
*******************************************************************************/
bool TON_TOF(bool DIx, bool DIx_Old ,u16 *TonCnt, u16 *TofCnt, u16 TonFilter, u16 TofFilter)
{
	bool BitStatus;
	
	BitStatus = DIx_Old;
	
    if (DIx)
    {
        *TofCnt = 0;	
        if ((*TonCnt) < TonFilter)	
        {
            (*TonCnt) ++;
        }					
        else					
        {
            BitStatus = 1;
        }
    }
    else 
    {
        *TonCnt = 0;
        if ((*TofCnt) < TofFilter)
        {
            (*TofCnt) ++;
        }
        else
        {
            BitStatus = 0;
        }

    }
	
    return BitStatus;
}

/*******************************************************************************
* 函数名 : DI_DO_Pro
* 描  述 : DI_DO信号处理
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void DI_DO_Pro(void)
{
    static u8 state = 0;
    u32 i;
    u32 InData = 0;
    u16 OutData = 0;

    bool OD0_Temp = 0;
    bool OD1_Temp = 0;
    bool OD2_Temp = 0;
    bool OD3_Temp = 0;
    bool OD4_Temp = 0;
    bool OD5_Temp = 0;
    bool OD6_Temp = 0;
    bool OD7_Temp = 0;
    bool OD8_Temp = 0;
    bool OD9_Temp = 0;
    bool OD10_Temp = 0;
    bool OD11_Temp = 0;
    bool OD12_Temp = 0;
    bool OD13_Temp = 0;
    bool OD14_Temp = 0;
    bool OD15_Temp = 0;

    if (ReadPara(OUT0_SEL) < 16)
    {
        OD0_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT0_SEL))) && 1;
    }
    else
    {
        OD0_Temp = OD0;
    }

    if (ReadPara(OUT1_SEL) < 16)
    {
        OD1_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT1_SEL))) && 1;
    }
    else
    {
        OD1_Temp = OD1; 
    }

    if (ReadPara(OUT2_SEL) < 16)
    {
        OD2_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT2_SEL))) && 1;
    }
    else
    {
        OD2_Temp = OD2; 
    }

    if (ReadPara(OUT3_SEL) < 16)
    {
        OD3_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT3_SEL))) && 1;
    }
    else
    {
        OD3_Temp = OD3; 
    }

    if (ReadPara(OUT4_SEL) < 16)
    {
        OD4_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT4_SEL))) && 1;
    }
    else
    {
        OD4_Temp = OD4;
    }

    if (ReadPara(OUT5_SEL) < 16)
    {
        OD5_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT5_SEL))) && 1;
    }
    else
    {
        OD5_Temp = OD5;
    }

    if (ReadPara(OUT6_SEL) < 16)
    {
        OD6_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT6_SEL))) && 1;
    }
    else
    {
        OD6_Temp = OD6; 
    }

    if (ReadPara(OUT7_SEL) < 16)
    {
        OD7_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT7_SEL))) && 1;
    }
    else
    {
        OD7_Temp = OD7; 
    }

    if (ReadPara(OUT8_SEL) < 16)
    {
        OD8_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT8_SEL))) && 1;
    }
    else
    {
        OD8_Temp = OD8; 
    }

    if (ReadPara(OUT9_SEL) < 16)
    {
        OD9_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT9_SEL))) && 1;
    }
    else
    {
        OD9_Temp = OD9; 
    }

    if (ReadPara(OUT10_SEL) < 16)
    {
        OD10_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT10_SEL))) && 1;
    }
    else
    {
        OD10_Temp = OD10; 
    }

    if (ReadPara(OUT11_SEL) < 16)
    {
        OD11_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT11_SEL))) && 1;
    }
    else
    {
        OD11_Temp = OD11;
    }

    if (ReadPara(OUT12_SEL) < 16)
    {
        OD12_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT12_SEL))) && 1;
    }
    else
    {
        OD12_Temp = OD12; 
    }

	#ifndef TEST_FOR_STARS								// 未定义星辰厂内测试版
	if ((CANAddr == 0) && (CANAddr_Real == 0))
	{
		OD13_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT13_SEL))) && 1;	// id=0，REL继电器由OUT13参数控制
	}
	else
	{
		OD13_Temp = !OD13;
	}
	#else
		if (CANAddr_Real == 0)							// id=0，REL继电器由OUT13参数控制
		{
			OD13_Temp = (ReadPara(SYS_ST_CODE) & (1<<ReadPara(OUT13_SEL))) && 1;
		}
		else											// id非0，运达HMI上位机控制REL继电器
		{
			if ((Func_0x06_Data[3] >> 0) & 1)			// HMI选择了DO测试使能
			{
				OD13_Temp = !OD13; 						// HMIRLA安全继电器控制
			}
			else
			{
				OD13_Temp = 0; 							// REL继电器闭合
			}
		}
	#endif

    CLEAR_IO_IN_ST; 	// 输入锁存
    CLEAR_IO_OUT_ST; 	// 输出锁存置低

    OutData =
		  ((OD15_Temp & 1) << 0)  | ((OD14_Temp & 1) << 1)  | ((OD13_Temp & 1) << 2)  | ((OD12_Temp & 1) << 3)
        | ((OD11_Temp & 1) << 4)  |	((OD10_Temp & 1) << 5)  | ((OD9_Temp  & 1) << 6)  | ((OD8_Temp  & 1) << 7)
        | ((OD7_Temp  & 1) << 8)  | ((OD6_Temp  & 1) << 9)  | ((OD5_Temp  & 1) << 10) | ((OD4_Temp  & 1) << 11)
        | ((OD3_Temp  & 1) << 12) | ((OD2_Temp  & 1) << 13) | ((OD1_Temp  & 1) << 14) | ((OD0_Temp  & 1) << 15);

    ReadPara(DO_STA) =
          ((OD15_Temp & 1) << 15) | ((OD14_Temp & 1) << 14) | ((OD13_Temp & 1) << 13) | ((OD12_Temp & 1) << 12)
        | ((OD11_Temp & 1) << 11) | ((OD10_Temp & 1) << 10) | ((OD9_Temp  & 1) << 9)  | ((OD8_Temp  & 1) << 8)
        | ((OD7_Temp  & 1) << 7)  | ((OD6_Temp  & 1) << 6)  | ((OD5_Temp  & 1) << 5)  | ((OD4_Temp  & 1) << 4)
        | ((OD3_Temp  & 1) << 3)  | ((OD2_Temp  & 1) << 2)  | ((OD1_Temp  & 1) << 1)  | ((OD0_Temp  & 1) << 0);

    OutData = ~OutData; 				// 输出取反

    SET_IO_IN_ST; 						// 输入保持
    CLEAR_IO_CLK; 						// IO时钟输出置低

    for (i = 0; i < 24; i++)			// 有N片74HC165则：X=N*8=3*8=24
    {
        if(i <= 15) 					// 可省略
        {
            if (OutData & (1 << (i & 0x000f)))
            {
                SET_IO_TX; 				// 输出1
            }
            else
            {
                CLEAR_IO_TX; 			// 输出0
            }
        }

        InData <<= 1;
        InData |= (READ_IO_RX & 1);
		
        CLEAR_IO_CLK; 					// IO时钟输出置低
        SET_IO_CLK;   					// IO时钟输出置高

        if (i == 15)					// 有M片74HC595则：Y=M*8-1=2*8-1=15
        {
            SET_IO_OUT_ST; 				// 输出锁存置高，锁存
            CLEAR_IO_OUT_ST; 			// 输出锁存置低
        }
    }

    CLEAR_IO_OUT_EN; 					// 输出使能置低，允许输出
	
	PWR1 = (InData & (1<<16)) && 1;

	ReadDI.all = InData;
	ReadDI.bit.DI16 = !READ_DI16;

  //DIStatus.bit.DI00 = 	TON_TOF(ReadDI.bit.DI00,	DIStatus.bit.DI00,	 &TonCnt.DI00, 	&TofCnt.DI00,	TON_FILTER, TOF_FILTER);			//DI0：ENOP 低电平滤波2ms
	DIStatus.bit.DI00 = 	TON_TOF(ReadDI.bit.DI00,	DIStatus.bit.DI00,	 &TonCnt.DI00, 	&TofCnt.DI00,	TON_FILTER_ENPO, TOF_FILTER_ENPO);	//DI0：ENOP 低电平滤波10ms
	DIStatus.bit.DI01 = 	TON_TOF(ReadDI.bit.DI01,	DIStatus.bit.DI01,	 &TonCnt.DI01, 	&TofCnt.DI01,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI02 = 	TON_TOF(ReadDI.bit.DI02,	DIStatus.bit.DI02,	 &TonCnt.DI02, 	&TofCnt.DI02,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI03 = 	TON_TOF(ReadDI.bit.DI03,	DIStatus.bit.DI03,	 &TonCnt.DI03, 	&TofCnt.DI03,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI04 = 	TON_TOF(ReadDI.bit.DI04,	DIStatus.bit.DI04,	 &TonCnt.DI04, 	&TofCnt.DI04,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI05 = 	TON_TOF(ReadDI.bit.DI05,	DIStatus.bit.DI05,	 &TonCnt.DI05, 	&TofCnt.DI05,	TON_FILTER, TOF_FILTER);	//DI5：手/自动选择
	DIStatus.bit.DI06 = 	TON_TOF(ReadDI.bit.DI06,	DIStatus.bit.DI06,	 &TonCnt.DI06, 	&TofCnt.DI06,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI07 = 	TON_TOF(ReadDI.bit.DI07,	DIStatus.bit.DI07,	 &TonCnt.DI07, 	&TofCnt.DI07,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI08 = 	TON_TOF(ReadDI.bit.DI08,	DIStatus.bit.DI08,	 &TonCnt.DI08, 	&TofCnt.DI08,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI09 = 	TON_TOF(ReadDI.bit.DI09,	DIStatus.bit.DI09,	 &TonCnt.DI09, 	&TofCnt.DI09,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI10 = 	TON_TOF(ReadDI.bit.DI10,	DIStatus.bit.DI10,	 &TonCnt.DI10, 	&TofCnt.DI10,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI11 = 	TON_TOF(ReadDI.bit.DI11,	DIStatus.bit.DI11,	 &TonCnt.DI11, 	&TofCnt.DI11,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI12 = 	TON_TOF(ReadDI.bit.DI12,	DIStatus.bit.DI12,	 &TonCnt.DI12, 	&TofCnt.DI12,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI13 = 	TON_TOF(ReadDI.bit.DI13,	DIStatus.bit.DI13,	 &TonCnt.DI13, 	&TofCnt.DI13,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI14 = 	TON_TOF(ReadDI.bit.DI14,	DIStatus.bit.DI14,	 &TonCnt.DI14, 	&TofCnt.DI14,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI15 = 	TON_TOF(ReadDI.bit.DI15,	DIStatus.bit.DI15,	 &TonCnt.DI15, 	&TofCnt.DI15,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI16 = 	TON_TOF(ReadDI.bit.DI16,	DIStatus.bit.DI16,	 &TonCnt.DI16, 	&TofCnt.DI16,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI17 = 	TON_TOF(ReadDI.bit.DI17,	DIStatus.bit.DI17,	 &TonCnt.DI17, 	&TofCnt.DI17,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI18 = 	TON_TOF(ReadDI.bit.DI18,	DIStatus.bit.DI18,	 &TonCnt.DI18, 	&TofCnt.DI18,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI19 = 	TON_TOF(ReadDI.bit.DI19,	DIStatus.bit.DI19,	 &TonCnt.DI19, 	&TofCnt.DI19,	TON_FILTER, TOF_FILTER);	
	DIStatus.bit.DI20 = 	TON_TOF(ReadDI.bit.DI20,	DIStatus.bit.DI20,	 &TonCnt.DI20, 	&TofCnt.DI20,	TON_IDADDR, TOF_IDADDR);	//拨码开关地址 AD8
	DIStatus.bit.DI21 = 	TON_TOF(ReadDI.bit.DI21,	DIStatus.bit.DI21,	 &TonCnt.DI21, 	&TofCnt.DI21,	TON_IDADDR, TOF_IDADDR);	//拨码开关地址 AD4
	DIStatus.bit.DI22 = 	TON_TOF(ReadDI.bit.DI22,	DIStatus.bit.DI22,	 &TonCnt.DI22, 	&TofCnt.DI22,	TON_IDADDR, TOF_IDADDR);	//拨码开关地址 AD2
	DIStatus.bit.DI23 = 	TON_TOF(ReadDI.bit.DI23,	DIStatus.bit.DI23,	 &TonCnt.DI23, 	&TofCnt.DI23,	TON_IDADDR, TOF_IDADDR);	//拨码开关地址 AD1		
	
	ReadPara(DIH_STA) = DIStatus.all >> 16;
	ReadPara(DI_STA) = DIStatus.all & 0x0000FFFF;
}

void Fan0ErrCheck(void)									// 底座风机故障检测	调用周期=10ms
{
    //if((ReadPara(SYS_ST_CODE) & 0x0002) == 0x0002)	
    {
		if (((s16)ReadPara(IGBT_TEMP) > (s16)ReadPara(FAN_ON_TEMP)) || ((ReadPara(ARM_DSP_COMMPARA0) & 0x0001) == 0x0001))
		{
		  //if ((READ_FAN0IN == 0) && (Fan0ErrCnt < 300))	// 反馈信号不是高电平	
			if ((READ_FAN0IN == 0) && (Fan0ErrCnt < 1000))	// 2025-06-12优化风机故障检测机制，3S检测周期改为10
			{
				Fan0ErrCnt ++;
			}
		}
		else
		{
			Fan0ErrCnt = 0;
			Fan0ErrFlag = 0;
		}
    }
	
	if (TimeBase_3Secon_ForFan0Err)
	{
	  //if (Fan0ErrCnt > 150)								// 1500ms/3000ms
		if (Fan0ErrCnt > 930)								// 2025-06-12优化：10S检测窗（1000次）底座风机反馈信号低电平次数大于930次
		{
			Fan0ErrFlag = 1;
		}
		else
		{
			Fan0ErrFlag = 0;
		}
		
		ReadPara(ARM_RESERVE_PARA_845) = Fan0ErrCnt;
			
		Fan0ErrCnt = 0;
		TimeBase_3Secon_ForFan0Err = 0;
	}

    if (Fan0ErrFlag)
    {
        ReadPara(RESERVE_915) |= 0x0001;
    }
    else
    {
        ReadPara(RESERVE_915) &= 0xFFFE;
    }
}

void Fan1ErrCheck(void)					// 内腔风机故障检测	调用周期=10ms
{
    if (ReadPara(BKFAN) == 1) 			// 星辰上位机强行关内腔风扇
    {
		FanOnOffCnt = 0;
		FanOnFlag = 0;
        FANOFF;
    }
    else if(ReadPara(BKFAN) == 2)		// 星辰上位机强行开内腔风扇
    {
		FanOnOffCnt = 0;
		FanOnFlag = 1;
        FANON;
    }
    else
    {
		if (FanOnForce == 2)			// HMI强行开内腔风扇
        {
			FanOnOffCnt = 0;
			FanOnFlag = 1;
            FANON;
        }
        else
        {			
// 			if (ReadPara(SYS_ST_CODE) & 0x0004) 			//驱动器开使能条件下：
			{				
				if (((s16)ReadPara(CHARG_IGBT_TEMP) > FANONTEMP) || ((s16)ReadPara(CHARG_INTERSPACE_TEMP) > FANONTEMP) || ((s16)ReadPara(CHARG_DC24V_TEMP) > FANONTEMP)) 
				{
					FANON;									//充电器3个温度检测点任何一个大于40°C, 则开内腔风机
					FanOnFlag = 1;
				}
				else if (((s16)ReadPara(CHARG_IGBT_TEMP) < FANOFTEMP) && ((s16)ReadPara(CHARG_INTERSPACE_TEMP) < FANOFTEMP) && ((s16)ReadPara(CHARG_DC24V_TEMP) < FANOFTEMP))	 			
				{
					FANOFF;									//充电器3个温度检测点全部都小于40°-5°回差, 则关内腔风机
					FanOnFlag = 0;
				}
				
				FanOnOffCnt = 0;
			}
// 			else											//驱动器关使能条件下：
// 			{
// 				if (((s16)ReadPara(CHARG_IGBT_TEMP) > FANONTEMP) || ((s16)ReadPara(CHARG_INTERSPACE_TEMP) > FANONTEMP) || ((s16)ReadPara(CHARG_DC24V_TEMP) > FANONTEMP)) 
// 				{
// 					if (TimeBase_1Secon)
// 					{
// 						if (FanOnOffCnt < FANONOFCNT)		//780S
// 						{
// 							FanOnOffCnt ++;
// 						}
// 						else
// 						{
// 							FanOnOffCnt = 0;
// 						}

// 						if ((FanOnOffCnt > FANONCNT) && (FanOnOffCnt <= FANONOFCNT))	//>180S && <780S
// 						{
// 							FANOFF;							//充电器3个温度检测点任何一个大于40°C, 则间歇：开3min*60S=180S、关10min*60S=600S
// 							FanOnFlag = 0;
// 						}
// 						else if ((FanOnOffCnt >= 0) && (FanOnOffCnt <= FANONCNT))
// 						{
// 							FANON;							
// 							FanOnFlag = 1;
// 						}

// 						TimeBase_1Secon = 0;
// 					}
// 				}
// 				else if (((s16)ReadPara(CHARG_IGBT_TEMP) < FANOFTEMP) && ((s16)ReadPara(CHARG_INTERSPACE_TEMP) < FANOFTEMP) && ((s16)ReadPara(CHARG_DC24V_TEMP) < FANOFTEMP))
// 				{
// 					FANOFF;									//充电器3个温度检测点全部都小于40°-5°回差, 则关内腔风机
// 					FanOnFlag = 0;
// 					FanOnOffCnt = 0;
// 				}
// 				else
// 				{
// 					FanOnOffCnt = 0;
// 				}
// 			}
        } 
    }

	if (FanOnFlag == 1)										// 有内腔风机开启命令
	{
	  //if ((READ_FAN1IN == 0) && (Fan1ErrCnt < 300))		// 反馈信号不是高电平	
		if ((READ_FAN1IN == 0) && (Fan1ErrCnt < 1000))		// 2025-06-12优化：10S检测窗（1000次）内腔风机反馈信号低电平次数大于500次
		{
			Fan1ErrCnt ++;
		}
	}
	else
	{
		Fan1ErrFlag = 0;
	}

	if (TimeBase_3Secon_ForFan1Err)
	{
	  //if (Fan1ErrCnt > 50)								// 500ms/3000ms
		if (Fan1ErrCnt > 500)								// 2025-06-12优化：10S检测窗（1000次）内腔风机反馈信号低电平次数大于500次
		{
			Fan1ErrFlag = 1;
		}
		else
		{
			Fan1ErrFlag = 0;
		}
		
		ReadPara(ARM_RESERVE_PARA_846) = Fan1ErrCnt;
			
		Fan1ErrCnt = 0;
		TimeBase_3Secon_ForFan1Err = 0;
	}

    if (Fan1ErrFlag)
    {
        ReadPara(RESERVE_915) |= 0x0002;
    }
    else
    {
        ReadPara(RESERVE_915) &= 0xFFFD;
    }
}

void RunCtrl(void)
{
    static u16 BK_DelayCnt = 0;
    static u16 BK_DelayCnt1 = 0;

    s32 Temp = 0;

    if(PcCtrlSet)
    {
        if (ReadPara(WORK_MOD) == 2)
        {
            DspSet = (s32)PcSet;
        }
        else if (ReadPara(WORK_MOD) == 1) 				// 位置环32位给定
        {
            //if (ReadPara(CAN_SEL) == 0)
			if(CANopen_EN == 0)
            {
                Temp = (s32)PcSet;
                Temp *= 10000;
                Temp /= 0x6C00;
                DspSet = Temp;
            }
            else
            {
                DspSet = (s32)UserTargetPosition; 		// 给定位置
            }
        }
        else
        {
            DspSet = (s32)PcSet;                        // 电流环不走S曲线
        }
    }
    else
    {
        DspSet = 0;
    }
	
	PcCtrlFlag_Out = PcCtrlFlag;
    StmStatus.bit.STM_SN_CTRL = PcCtrl;
	BK_Buf = PcCtrlFlag_Out;
	StmStatus.bit.STM_SN_CTRLFLAG = PcCtrlFlag_Out;
    StmStatus.bit.STM_SET_CTRL = PcCtrlSet;
	
    if (ReadPara(BK_SWITCH) == 0)
    {
        BK = ~BK_Buf; 					// 制动器与使能联动
    }
    else if (ReadPara(BK_SWITCH) == 1)
    {
        BK = 1; 						// 制动器关
    }
    else if (ReadPara(BK_SWITCH) == 2)
    {
        BK = 0; 						// 制动器开
    }
}

void Mout(void)
{
    s32 Mout1Value, Mout2Value;

    if (ReadPara(MOUT1_SEL) > 32) ReadPara(MOUT1_SEL) = 32;
    if (ReadPara(MOUT2_SEL) > 32) ReadPara(MOUT2_SEL) = 32;

    Mout1Value = (s16)(MoutValue[ReadPara(MOUT1_SEL)]);
    Mout2Value = (s16)(MoutValue[ReadPara(MOUT2_SEL)]);

    Mout1Value += (s16)ReadPara(MOUT1_OFFSET);         					// 调节零偏
    Mout2Value += (s16)ReadPara(MOUT2_OFFSET);         					// 调节零偏

    Mout1Value = Mout1Value * (u16)ReadPara(MOUT1_GAIN) / 1024;         // 调节增益
    Mout2Value = Mout2Value * (u16)ReadPara(MOUT2_GAIN) / 1024;         // 调节增益

    if (Mout1Value > 32767) Mout1Value = 32767;
    else if (Mout1Value < -32768) Mout1Value = -32768;

    if (Mout2Value > 32767) Mout2Value = 32767;
    else if (Mout2Value < -32768) Mout2Value = -32768;

    Mout1Value += 32768;         // 输出偏置
    Mout2Value += 32768;         // 输出偏置

    //DAC_SetDualChannelData(DAC_Align_12b_L, (u16)Mout1Value, (u16)Mout2Value); // 设置双通道待转换数据，右对齐
}