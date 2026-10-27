#include "stm32f4xx_conf.h"
#include "ExternPrototype.h"
#include "AppFunc.h"

#ifndef TEST_FOR_STARS							// 未定义星辰厂内测试版
	#include "HMI_Display.h"
	#include "AppInputOutPut.h"
	#include "GlobalVar_POUS.h"
	#include "GlobalVar_HMI.h"
	#include "GlobalVar_IOS.h"
#else
#endif

/* 故障代码记录结构体定义 */
typedef struct GZM_Type
{
    u16	Data0;
    u16	Data1;
    u16	Data2;
    u16	Data3;
    u16	Data4;
    u16	Data5;
    u16	Data6;
    u16	Data7;
    u16	Data8;
    u16	Data9;
    u16	Data10;
    u16	Data11;
    u16	Data12;
    u16	Data13;
    u16	Data14;
    u16	Data15;
    u16	Data16;
    u16	Data17;
    u16	Data18;
    u16	Data19;
    u16	Data20;
    u16	Data21;
    u16	Data22;
    u16	Data23;
    u16	Data24;
    u16	Data25;
    u16	Data26;
    u16	Data27;
    u16	Data28;
    u16	Data29;
} GZM_TypeDef;
u16 DataArray[GZM_InfoNum * GZM_Num];						// 每组30条故障信息(16位) * 20组故障记录 = 600Half Word
u16 ErrRecordDataArray[GZM_InfoNum * GZM_Num]; 				// 故障代码记录数组
u16 ErrRecordDataArray_Old[GZM_InfoNum * GZM_Num]; 			// 故障代码记录数组Old
GZM_TypeDef* GZM_Info = (GZM_TypeDef*)ErrRecordDataArray; 	// 指向故障代码记录数组的指针

struct	YEAR_BIT
{
    unsigned int RESERVE03:4;		//BIT0	~	BIT3		预留
    unsigned int RESERVE47:4;		//BIT4	~	BIT7		预留
    unsigned int YEAR_L:4;			//BIT8	~	BIT11		年_L
    unsigned int YEAR_H:4;			//BIT12	~	BIT15		年_H
};
union	YEAR
{
    unsigned int all;
    struct	YEAR_BIT bit;
};
union YEAR YEAR_DATA;

struct	DAY_AND_MONTH_BIT
{
    unsigned int MONTH_L:4;			//BIT0	~	BIT3		月_L
    unsigned int MONTH_H:4;			//BIT4	~	BIT7		月_H
    unsigned int DAY_L:4;			//BIT8	~	BIT11		日_L
    unsigned int DAY_H:4;			//BIT12	~	BIT15		日_H
};
union	DAY_AND_MONTH
{
    unsigned int all;
    struct	DAY_AND_MONTH_BIT bit;
};
union DAY_AND_MONTH DAY_AND_MONTH_DATA;

struct	HOUR_AND_WEEK_BIT
{
    unsigned int WEEK_L:4;			//BIT0	~	BIT3		周_L
    unsigned int WEEK_H:4;			//BIT4	~	BIT7		周_H
    unsigned int HOUR_L:4;			//BIT8	~	BIT11		时_L
    unsigned int HOUR_H:4;			//BIT12	~	BIT15		时_H
};
union	HOUR_AND_WEEK
{
    unsigned int all;
    struct	HOUR_AND_WEEK_BIT bit;
};
union HOUR_AND_WEEK HOUR_AND_WEEK_DATA;

struct	SECON_AND_MINIT_BIT
{
    unsigned int MINIT_L:4;			//BIT0	~	BIT3		分_L
    unsigned int MINIT_H:4;			//BIT4	~	BIT7		分_H
    unsigned int SECON_L:4;			//BIT8	~	BIT11		秒_L
    unsigned int SECON_H:4;			//BIT12	~	BIT15		秒_H
};
union	SECON_AND_MINIT
{
    unsigned int all;
    struct	SECON_AND_MINIT_BIT bit;
};
union SECON_AND_MINIT SECON_AND_MINIT_DATA;

u16 ReadErrRecordOK = 0;
u8 ErrSaveEnable = 0;
u8 RunState = 0;
u16 FirstErrCode = 0;
u16 FirstErrCodeOld = 0;
u8 FirstErrCodeNum = 0;
u16 FirstErrCodeTrig = 0;
u16 DataBuff[30] = {0};
u16 DataBuff_Old[30] = {0};
u16 DataSave[30] = {0};
u8 History_Err_Save_Flag;
u8 Err_Record_Test = 0;

extern u8 ClrFaultFlag;
extern u8 Init_flag;
extern u64 DecodeAll;
extern s16 FastVol;
extern u16 Controlword_Out;
extern s16 Target_position_Out;
extern u64 ServerFault;
extern u16 ServerFault1;
extern u16 ServerFault2;
extern u16 ServerFault3;
extern u16 ServerFault4;
extern u16 ServerFault5;
extern s16 Current_actual_value;
extern union STMSTATUS StmStatus;
extern bool	FaultData_Storage_Flag;

/*******************************************************************************
* 函数名 : GetErrRecord
* 描  述 :
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void GetErrRecord(void)
{
    u16 i;
    static u64 DecodeAll_Old = 0;

    if(Init_flag == 0)
    {
        return;
    }

	#ifndef TEST_FOR_STARS
		if ((G_EmergRequest) && (G_PitchRunState == EMERGENCY)  && (CAN_T.ErrorWord) && (FirstErrCodeTrig == 0))
		{
			FirstErrCodeTrig = 1;
		}
		
		if (FirstErrCodeTrig == 2)
		{
			if ((G_EmergRequest == 0) || (G_PitchRunState != EMERGENCY) || (CAN_T.ErrorWord == 0))
			{
				ErrSaveEnable = 0;
				History_Err_Save_Flag = 0;
				FirstErrCodeTrig = 0;
			}
		}
		
	#else
		if (ServerFault == 0)
		{
			ErrSaveEnable = 0;
			History_Err_Save_Flag = 0;
			FirstErrCode = 0;
			FirstErrCodeTrig = 0;
		}
	#endif

    if ((FirstErrCodeTrig == 1)															// 首发故障标志
    || Err_Record_Test																	// For Test
    || ReadPara(ERRSAVE_ENABLE))														// For Test
    {
        Err_Record_Test = 0;
        ReadPara(ERRSAVE_ENABLE) = 0;
																						// For Exmple：22年01月21日05周10时08分02秒
        YEAR_DATA.bit.YEAR_H = ReadPara(RTC_YEAR_FEB) / 10;								// 年H：22/10=2
        YEAR_DATA.bit.YEAR_L = ReadPara(RTC_YEAR_FEB) % 10;								// 年L：22%10=2
        YEAR_DATA.bit.RESERVE47 = 0xA;													// 校验首字节FlashReadFaultError，正常读出应为0xAA
        YEAR_DATA.bit.RESERVE03 = 0xA;													// 校验首字节FlashReadFaultError，正常读出应为0xAA

        DAY_AND_MONTH_DATA.bit.DAY_H = ReadPara(RTC_MONTH_AND_DATA_FEB) % 100 / 10;		// 日H：21/10=2
        DAY_AND_MONTH_DATA.bit.DAY_L = ReadPara(RTC_MONTH_AND_DATA_FEB) % 100 % 10;		// 日L：21%10=1
        DAY_AND_MONTH_DATA.bit.MONTH_H = ReadPara(RTC_MONTH_AND_DATA_FEB) / 100 / 10;	// 月H：01/10=0
        DAY_AND_MONTH_DATA.bit.MONTH_L = ReadPara(RTC_MONTH_AND_DATA_FEB) / 100 % 10;	// 月L：01%10=1

        HOUR_AND_WEEK_DATA.bit.HOUR_H = ReadPara(RTC_HOUR_AND_MINIT_FEB) / 100 / 10;	// 时H：10/10=1
        HOUR_AND_WEEK_DATA.bit.HOUR_L = ReadPara(RTC_HOUR_AND_MINIT_FEB) / 100 % 10;	// 时L：10%10=0
        HOUR_AND_WEEK_DATA.bit.WEEK_H = 0;												// 周H：05/10=0
        HOUR_AND_WEEK_DATA.bit.WEEK_L = 5;												// 周L：05%10=5

        SECON_AND_MINIT_DATA.bit.SECON_H = ReadPara(RTC_SECON_FEB) / 10;				// 秒H：02/10=0
        SECON_AND_MINIT_DATA.bit.SECON_L = ReadPara(RTC_SECON_FEB) % 10;				// 秒L：02%10=2
        SECON_AND_MINIT_DATA.bit.MINIT_H = ReadPara(RTC_HOUR_AND_MINIT_FEB) % 100 / 10;	// 分H：08/10=0
        SECON_AND_MINIT_DATA.bit.MINIT_L = ReadPara(RTC_HOUR_AND_MINIT_FEB) % 100 % 10;	// 分L：08%10=8

        //故障时刻数据记录
		#ifndef TEST_FOR_STARS
			DataSave[0]  = YEAR_DATA.all; 										// 年X	0x22xx代表：22年、xx为预留
			DataSave[1]  = DAY_AND_MONTH_DATA.all;								// 日月	0x2101代表：21日、01月
			DataSave[2]  = HOUR_AND_WEEK_DATA.all; 								// 时周	0x1005代表：10时、周05(上位机忽略)
			DataSave[3]  = SECON_AND_MINIT_DATA.all;							// 秒分	0x0208代表：02秒、08分
			DataSave[4]  = CAN_T.ErrorWord ; 									// 首发故障码
			DataSave[5]  = G_Driver.PitchFaulStatus1.all; 						// 系统故障字1
			DataSave[6]  = G_Driver.PitchFaulStatus2.all; 						// 系统故障字2
			DataSave[7]  = G_Driver.PitchFaulStatus3.all; 						// 系统故障字3
			DataSave[8]  = G_Driver.PitchFaulStatus4.all; 						// 系统故障字4
			DataSave[9]  = DoubleRAM.InverterFaultWord1; 						// 驱动字故障字1
			DataSave[10] = DoubleRAM.InverterFaultWord1; 						// 驱动字故障字2
			DataSave[11] = DoubleRAM.InverterFaultWord1; 						// 驱动字故障字3
			DataSave[12] = DoubleRAM.InverterFaultWord1;						// 驱动字故障字4
			DataSave[13] = DoubleRAM.inverter_statusword; 						// CAN发送状态字
			DataSave[14] = DoubleRAM.inverter_controlword; 						// CAN接收控制字
			DataSave[15] = DoubleRAM.DSP_StateWord;								// DSP状态字
			DataSave[16] = G_Driver.Act_CapVoltage;								// 电容电压
			DataSave[17] = G_Driver.Act_DCVoltage;								// 母线电压
			DataSave[18] = G_Driver.Act_MotorCurrent;							// 电机电流
			DataSave[19] = G_Driver.Act_PitchPosition;							// 桨叶角度
			DataSave[20] = G_Driver.act_torque;									// 扭矩
			DataSave[21] = G_Driver.Act_InternalTem;							// 驱动器内部温度
			DataSave[22] = G_Driver.Act_HeatsinkTem;							// IGBT温度
			DataSave[23] = PT_MotorTem;											// 电机温度
			DataSave[24] = 0;													// 相对湿度
			DataSave[25] = G_TargetPosition;									// 目标位置
			DataSave[26] = Torque_limit_percent;								// 扭矩限制百分比
			DataSave[27] = PT_HubTem;											// 轮毂温度
			DataSave[28] = PT_DrvCabTem;										// 驱动柜温度
			DataSave[29] = 0x5555;												// 校验尾字节FlashReadFaultError，正常读出应为0x5555
		#else
			DataSave[0]  = YEAR_DATA.all; 										// 年X	0x22xx代表：22年、xx为预留
			DataSave[1]  = DAY_AND_MONTH_DATA.all;								// 日月	0x2101代表：21日、01月
			DataSave[2]  = HOUR_AND_WEEK_DATA.all; 								// 时周	0x1005代表：10时、周05(上位机忽略)
			DataSave[3]  = SECON_AND_MINIT_DATA.all;							// 秒分	0x0208代表：02秒、08分
			DataSave[4]  = FirstErrCode; 										// 首发故障码
			DataSave[5]  = 5; 													// 系统故障字1
			DataSave[6]  = 6; 													// 系统故障字2
			DataSave[7]  = 7; 													// 系统故障字3
			DataSave[8]  = 8; 													// 系统故障字4
			DataSave[9]  = 9; 													// 驱动字故障字1
			DataSave[10] = 10; 													// 驱动字故障字2
			DataSave[11] = 11; 													// 驱动字故障字3
			DataSave[12] = 12;													// 驱动字故障字4
			DataSave[13] = 13; 													// CAN发送状态字
			DataSave[14] = 14; 													// CAN接收控制字
			DataSave[15] = 15;													// DSP状态字
			DataSave[16] = 16 * 10;												// 电容电压
			DataSave[17] = 17 * 10;												// 母线电压
			DataSave[18] = 18 * 10;												// 电机电流
			DataSave[19] = 19 * 100;											// 桨叶角度
			DataSave[20] = 20 * 10;												// 扭矩
			DataSave[21] = 21;													// 驱动器内部温度
			DataSave[22] = 22;													// IGBT温度
			DataSave[23] = 23;													// 电机温度
			DataSave[24] = 24;													// 相对湿度
			DataSave[25] = 25 * 100;											// 目标位置
			DataSave[26] = 26;													// 扭矩限制百分比
			DataSave[27] = 27;													// 轮毂温度
			DataSave[28] = 28;													// 驱动柜温度
			DataSave[29] = 0x5555;												// 校验尾字节FlashReadFaultError，正常读出应为0x5555
		#endif
		
        History_Err_Save_Flag =1;
    }

    DecodeAll_Old = DecodeAll;
}

/*******************************************************************************
* 函数名 : EraseErrRecord
* 描  述 : 清除SD卡历史故障
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void EraseErrRecord(void)
{
	if (ClrFaultFlag)
	{
		EraseErrRecordfromSdcard();
		ClrFaultFlag = 0;
	}
}

/*******************************************************************************
* 函数名 : ReadErrRecord
* 描  述 : 读出SD卡历史故障
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void ReadErrRecord(void)
{
    u16 i;						
	
	ReadErrRecordfromSdcard();
	
	for (i = 0; i < (GZM_InfoNum * GZM_Num); i++)
	{
		ErrRecordDataArray_Old[i] = ErrRecordDataArray[i];
	}

    for (i = 0; i < 300; i++)												// 星辰历史故障配置表读取第0~9组(30条故障信息*10组=300字=600字节)历史信息
    {
        SysParaTable[i+1000].DATA = ErrRecordDataArray[i];
    }

    ReadErrRecordOK = 1; 													// 置读历史故障完成标志
}

/*******************************************************************************
* 函数名 : WriteErrRecord
* 描  述 : SD卡记录历史故障
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void WriteErrRecord(void)
{
    static u16 OldDefectCode = 0;
	u16 i;
	
    if (History_Err_Save_Flag)
    {
        FirstErrCodeTrig = 2;
        ErrSaveEnable = 1;
        History_Err_Save_Flag = 0;

        GZM_Info[0].Data0	=	DataSave[0];
        GZM_Info[0].Data1	=	DataSave[1];
        GZM_Info[0].Data2	=	DataSave[2];
        GZM_Info[0].Data3	=	DataSave[3];
        GZM_Info[0].Data4	=	DataSave[4];
        GZM_Info[0].Data5	=	DataSave[5];
        GZM_Info[0].Data6	=	DataSave[6];
        GZM_Info[0].Data7	=	DataSave[7];
        GZM_Info[0].Data8	=	DataSave[8];
        GZM_Info[0].Data9	=	DataSave[9];
        GZM_Info[0].Data10	=	DataSave[10];
        GZM_Info[0].Data11	=	DataSave[11];
        GZM_Info[0].Data12	=	DataSave[12];
        GZM_Info[0].Data13	=	DataSave[13];
        GZM_Info[0].Data14	=	DataSave[14];
        GZM_Info[0].Data15	=	DataSave[15];
        GZM_Info[0].Data16	=	DataSave[16];
        GZM_Info[0].Data17	=	DataSave[17];
        GZM_Info[0].Data18	=	DataSave[18];
        GZM_Info[0].Data19	=	DataSave[19];
        GZM_Info[0].Data20	=	DataSave[20];
        GZM_Info[0].Data21	=	DataSave[21];
        GZM_Info[0].Data22	=	DataSave[22];
        GZM_Info[0].Data23	=	DataSave[23];
        GZM_Info[0].Data24	=	DataSave[24];
        GZM_Info[0].Data25	=	DataSave[25];
        GZM_Info[0].Data26	=	DataSave[26];
        GZM_Info[0].Data27	=	DataSave[27];
        GZM_Info[0].Data28	=	DataSave[28];
        GZM_Info[0].Data29	=	DataSave[29];
		
		for (i = 0; i < (GZM_InfoNum * GZM_Num - 30); i++)
		{
			ErrRecordDataArray[i+30] = ErrRecordDataArray_Old[i];
		}
    }	
	
    if (ErrSaveEnable)
    {
		WriteErrRecorToSdCard(ErrRecordDataArray, GZM_InfoNum * GZM_Num); 

		for (i = 0; i < GZM_InfoNum * GZM_Num; i++)
		{
			ErrRecordDataArray_Old[i] = ErrRecordDataArray[i];
		}
		
		ReadErrRecordOK = 0; 																	// 清读历史故障完成标志
        ErrSaveEnable = 0;
    }
}