#define  USER_GLOBALS
#include "UserMainTask.h"
#include "includes.h"
#include "data.h"
#include "ExternPrototype.h"
#include "state_machine.h"
#include "AppFunc.h"
	
#ifndef TEST_FOR_STARS							// 未定义星辰厂内测试版
	#include "AppInputOutPut.h"
	#include "GlobalVar_POUS.h"
#else
#endif

long long UserTargetPositiontemp = 0;	
bool CapTestFlag = 0;
bool DisChargeFailFlag = 0;
bool gPosClear =0;								// 旋变角度校准
bool Zero_Old;
bool Silicon_Controlled_Rectifier = 0;			// 1=关闭半控桥
bool SelcetMotorTypeFlag = 0;
bool PichMotorHeatEna = 0;						// 1=开启电机预热功能
u32 ChargeRunEnable_Delay = 0;
u32 DisCharge_Delay = 0;
u32 CapTestFinishDelay = 0;
u16 CapTestState = 0;
u8 FanOnForce = 0;								// 1=HMI强制关闭内腔风机；2=HMI强制开启内腔风机
u8 ToothInit = 0;
u16 MotorType_Old = 0;
u16 HeartBeat = 0;
u16 HeartBeatOld = 0;
u8 CANOffLineErr = 0;
u8 err_rst1 = 0;
u16 defect_code=0;
u16 err_rst1_cnt = 0;
u8 MotorRstMode = 1;							// 电机复位模式 0客户复位，1上电复位
u8 motor_pos_rst0 = 0;
u8 motor_pos_rst_sel = 0;
u8 MotorZ_SetFinish = 0;						// 上电复位0未开始上电复位，1上电复位结束
u8 SaveMotorOffSet = 0;							// 保存手动模式下校零的值。
u8 PLCZ_SetFinish = 0;
s8 Modes_of__operation = 0x1;
u16 Controlword_Out = 0;
s16 Target_position_Out = 0;
s16 Target_position_Test = 0;
s16 Velocity_Limit_Out = 200;					// 位置环下的最高转速默认2.00°/S
s16 Velocity_Limit_Test = 200;				
s16 Target_velocity_Out = 0;
s16 Target_velocity_Test = 0;
u16	SafeRunPositon = 9000;						// 安全收桨目标位置
u16 P_PV_Temp = 850;
s32 HMI_Pich_Control_Vset = 200;				// 手动模式速度°/S 
u16 HMI_Pich_Control_Reg = 0;					// 调桨控制：地址01
u16 HMI_Pich_Control_Mode = 0;					// 调桨控制：模式反馈值：0=无、1=点动模式、2=区间模式、3=位置模式
u16 HMI_Pich_Control_Dir = 0;					// 区间控制：开桨、顺桨
s16 HMI_Pich_Control_Pset1 = -200;				// 区间控制：地址14，开桨角度最小值 = -2.00°
s16 HMI_Pich_Control_Sped1 = -200;				// 区间控制：地址15，开桨速度范围：0.00°/S ~ 10°/S
s16 HMI_Pich_Control_Pset2 = 9400;				// 区间控制：地址13，顺桨角度最大值 = 94.00°
s16 HMI_Pich_Control_Sped2 = 200;				// 区间控制：地址16，顺桨速度范围：0.00°/S ~ 10°/S 
s16 HMI_Pich_Control_Pset3 = 0;					// 位置控制：地址17，目标角度最大值 = 94.00°
s16 HMI_Pich_Control_Sped3 = -200;				// 位置控制：地址18，目标速度范围：0.00°/S ~ 10°/S	

volatile u16 SavePitchAngleCmd = 0;		//客户接口：1=请求保存桨叶角度（客户写，ARM只读）
/* 21.UserDataConfig\ExternPrototype.h，紧接 574 行 ARM_DSP_COMMPARA0 之后 */
#define	ARM_DSP_COMMPARA0_BIT_SAVE_PITCH	0x0008	//bit3 呼叫DSP保存桨叶角度
#define	ARM_DSP_COMMPARA0_CLR_KEEP_BIT3		0xFFF8	//整体清零时保留bit3

extern volatile u16 SavePitchAngleCmd;	//客户接口：1=请求保存桨叶角度（客户写，ARM只读）

//2026-09-21 新增：客户请求保存桨叶角度的DSP结果判定用变量（仅本文件使用）
 u16 SavePitchCmdOld      = 0;	//上一周期的客户请求标志，用于识别"0→1跳变"=新一轮请求开始（仅此时清上一轮结果）
 u16 SavePitchDoneLatch   = 0;	//1=本次请求已收到DSP保存完成的结果（锁存，避免DSP_Status刷新间隙漏判）
 u16 SavePitchErrLatch    = 0;	//1=本次请求失败（在超时时间内未收到DSP_Status.bit1保存完成反馈）
static u16 SavePitchOverTimeCnt = 0;	//超时计数（UserMainTask调用周期2ms，3000次=6S）

/* 2026-10-09新增：DSP是否运行在APP
与DSP通讯正常(无通讯失败标志)且ARM/DSP参数打包校验通过 → DSP在APP；
DSP跳Bootloader期间不响应本串口协议，超时后 InUartErrFlag 置1，本标志随之清0。*/

u8 DspAppState = 0; // 2026-10-09新增：DSP是否运行在APP程序 1=在APP
u8 ChargerAppState = 0; // 2026-10-09新增：充电器是否运行在APP程序 1=在APP 0=不在APP(Boot/通讯异常/未确认)

extern u16 Func_0x06_Data[];
extern u16 Func_0x10_Data[];
extern u32 ChipUniqueID[];
extern u16 ServerFault1;
extern u16 ServerFault2;
extern u16 ServerFault3;
extern u16 ServerFault4;
extern u16 ServerFault5;
extern u8 Tpdo1_Data[8];
extern u8 Tpdo2_Data[8];
extern u8 Tpdo3_Data[8];
extern u8 Tpdo4_Data[8];
extern u8 Rpdo1_Data[8];
extern u8 Rpdo2_Data[8];
extern u8 Rpdo3_Data[8];
extern u8 Rpdo4_Data[8];
extern INTEGER16 Target_position;		       										
extern INTEGER16 Modes_of_operation;		   										
extern s32 ENC_HomingPos;
extern u8 Init_flag;
extern u8 Init_flag1;
extern u8 Init_flag2;	
extern CO_Data FD_ObjDict_Data;
extern u16 Handslowspeed; 	
extern u16 Handfastspeed; 
extern INTEGER32 RealAngle;
extern UNS16 DC_link_circuit_voltage;	
extern s16 Current_actual_value;
extern u32 CiA402_Statusword;
extern u8 err_rst1_delay;
extern UNS16 UserPara5;
extern u64 DecodeAll;
extern u16 SaveMode;
extern u16 CrcPassFlag;
extern u8 State_Machine;
extern u8 InUartErrFlag; 
extern s16 Position_actual_value_s16;
extern s16 DspSetSpeed;
extern s32 MotoRpmSpeed;
extern s32 Velocity_sensor_actual_value_temp;
extern u8 DSP_Status;
extern bool RST_QY;
extern bool RST_GY;
extern bool RefCheckFinish;	
extern bool Runrequest;
extern bool RunMode;
extern bool RunMode_Out;
extern bool RunMode_Test;			
extern bool Runrequest_Test;
extern bool Fan0ErrFlag;
extern bool Fan1ErrFlag;
extern bool AngleCrcErr;
extern bool AngleCrcErr_VloopRunFlag;
extern bool GridVoltage_Over500VAC_Flag;
extern long Velocity_sensor_actual_value;
extern union DI_STATUS DIStatus;

/*******************************************************************************
* 函数名 : UserPLCInit
* 描  述 : DI输入口滤波处理
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void UserPLCInit(void)
{
    AddUserTask(&UserMainTask, 2, 0); 							// 变桨逻辑处理，调用周期2ms，优先级0
    User_work_mod = 1;
    User_PLC_Sta = PLC_STA_STOP;
	v_up_time_buf = vl_ramp_function_time;
	v_down_time_buf = vl_slow_down_time;
	Modes_of__operation = 1;
	UserSafeFinishFlag = 0;
	err_rst = 0;
}

/*******************************************************************************
* 函数名 : UserReadPort
* 描  述 : DO口及使能信号输出
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void UserWritePort(void)
{	
	#ifndef TEST_FOR_STARS				// 未定义星辰厂内测试版
		OD0  = gDOState.bit.DO0;
		OD1  = gDOState.bit.DO1;
		OD2  = gDOState.bit.DO2;
		OD3  = gDOState.bit.DO3;
		OD4  = gDOState.bit.DO4;
		OD5  = gDOState.bit.DO5;
		OD6  = gDOState.bit.DO6;
		OD7  = gDOState.bit.DO7; 
		OD8  = gDOState.bit.DO8;
		OD9  = gDOState.bit.DO9;
		OD10 = gDOState.bit.DO10;
		OD11 = gDOState.bit.DO11;
		OD12 = gDOState.bit.DO12; 
		OD13 = gDOState.bit.Relay;
	#else
		if ((Func_0x06_Data[3] >> 0) & 1)			// DO测试使能 
		{
			OD0  = (Func_0x06_Data[3] >> 1) & 1;		
			OD1  = (Func_0x06_Data[3] >> 2) & 1;
			OD2  = (Func_0x06_Data[3] >> 3) & 1;
			OD3  = (Func_0x06_Data[3] >> 4) & 1;
			OD4  = (Func_0x06_Data[3] >> 5) & 1;
			OD5  = (Func_0x06_Data[3] >> 6) & 1;
			OD6  = (Func_0x06_Data[3] >> 7) & 1;
			OD7  = (Func_0x06_Data[3] >> 8) & 1; 
			OD8  = (Func_0x06_Data[3] >> 9) & 1;
			OD9  = 0;
			OD10 = 0;
			OD11 = 0;
			OD12 = 0;
			OD13 = (Func_0x06_Data[3] >> 10) & 1;	// Relay输出	
			OD14 = 0;
			OD15 = 0;
		}
	#endif
	
    work_mod = User_work_mod; 
	target_value = User_target_value;

	if (((RunEnable) || (User_PLC_Sta == PLC_STA_VLOOP) || (User_PLC_Sta == PLC_STA_OPENLOOP) || (User_PLC_Sta == PLC_STA_HAND)) 
	&& (User_sn_on == 0xAA55))
	{
		sn_on_buf = User_sn_on;
		sn_off_buf = 0;
	}
	else
	{
		sn_on_buf = 0;
		sn_off_buf = User_sn_off;
	}
				
	sn_on = sn_on_buf;			// sn_on = 0xAA55 --> UpSysPara(0xC0, 3, 0xAA55) --> ExCANopenRxFullFlag=1 -->
    sn_off = sn_off_buf;		// case 3: if(Data == 0xAA55) PcCtrlFlag = 1 --> StmStatus.bit.STM_SN_CTRLFLAG = PcCtrlFlag = 1 --> 下传到DSP
}

/*******************************************************************************
* 函数名 : DoubleRAM_InitData
* 描  述 : DoubleRAM初始化数据
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/	
void DoubleRAM_InitData(void)
{
	#ifndef TEST_FOR_STARS												
		if ((G_ControllerInit) && (ToothInit < 8))
		{			
			if ((ExCANopenRxFullFlag == 0) && (ToothInit == 0))
			{
				ReadPara(SCALE_A) = DoubleRAM.SlowTooth;
				UpSysPara(0x80, SCALE_A,ReadPara(SCALE_A));				
				ToothInit = 1;
			}
			else if ((ExCANopenRxFullFlag == 0) && (ToothInit == 1))
			{
				ReadPara(SCALE_B) = DoubleRAM.BigTooth; 
				UpSysPara(0x80, SCALE_B,ReadPara(SCALE_B));				
				ToothInit = 2;
			}
			else if ((ExCANopenRxFullFlag == 0) && (ToothInit == 2))
			{
				/*
				15.84°/S * 60S * 大齿25000 / 小齿11 / 360° = 6000RPM，电机最高转速ReadPara(MAX_V)=3000RPM，则0RPM上升到3000RPM用时：3000/6000=500ms		
				ReadPara(V_UP_TIME)   = (3000*100000) / (1584*25000/11/6) / 10 = 50，代表500ms（ReadPara(V_UP_TIME)=1S，下传100数值）
				ReadPara(V_DOWN_TIME) = (3000*100000) / (1584*25000/11/6) / 10 = 50，代表500ms（ReadPara(V_UP_TIME)=1S，下传100数值）
				*/
				
				ReadPara(V_UP_TIME)   = ((u32)ReadPara(MAX_V) * 100000 / ((u32)DoubleRAM.AccSpeedSetValue * DoubleRAM.BigTooth / DoubleRAM.SlowTooth / 6)) / 10;
				UpSysPara(0x80, V_UP_TIME,ReadPara(V_UP_TIME));			
				ToothInit = 3;
			}
			else if ((ExCANopenRxFullFlag == 0) && (ToothInit == 3))
			{
				ReadPara(V_DOWN_TIME) = ((u32)ReadPara(MAX_V) * 100000 / ((u32)DoubleRAM.DecSpeedSetValue * DoubleRAM.BigTooth / DoubleRAM.SlowTooth / 6)) / 10;
				UpSysPara(0x80, V_DOWN_TIME,ReadPara(V_DOWN_TIME));		
				ToothInit = 4;
			}
			else if ((ExCANopenRxFullFlag == 0) && (ToothInit == 4))
			{
				ReadPara(P_PACC) = DoubleRAM.AccSpeedSetValue;							// 加速度15.84°/S
				UpSysPara(0x80, P_PACC,ReadPara(P_PACC));				
				ToothInit = 5;
			}
			else if ((ExCANopenRxFullFlag == 0) && (ToothInit == 5))
			{
				ReadPara(P_PDEC) = DoubleRAM.DecSpeedSetValue;							// 加速度15.84°/S	
				UpSysPara(0x80, P_PDEC,ReadPara(P_PDEC));				
				ToothInit = 6;
			}
			else if ((ExCANopenRxFullFlag == 0) && (ToothInit == 6))
			{
				ReadPara(DSP_AUTORUN_OVERTIME) = DoubleRAM.EmergencyAutoRunTIME; 		// DSP自主收桨超时：320代表32.0S		
				UpSysPara(0x80, DSP_AUTORUN_OVERTIME,ReadPara(DSP_AUTORUN_OVERTIME));	
				ToothInit = 7;
			}
			else if ((ExCANopenRxFullFlag == 0) && (ToothInit == 7))
			{
				ReadPara(SAFE_CLOSE_SPEED) = DoubleRAM.EmergencyAutoRunSpe;				// DSP自主收桨转速：300代表3°/S	
				UpSysPara(0x80, SAFE_CLOSE_SPEED,ReadPara(SAFE_CLOSE_SPEED));
				ToothInit = 8;
			}
		}
	#else
	#endif
}

/*******************************************************************************
* 函数名 : DoubleRAM_ReadData
* 描  述 : DoubleRAM读数据处理
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/	
void DoubleRAM_ReadData(void)
{
	u16 i;
		
	//DSP传给ARM的状态值
	DoubleRAM.StateWord =												//F01.00    驱动器状态字
	
	  (((State_Machine == NRSO) 					? 1 : 0) << 0)		//bit0		初始化（驱动器状态机为NRSO）
	| (((State_Machine == FT) 						? 1 : 0) << 1)		//bit1		故障（驱动器状态机为Fault）
	
	| ((((State_Machine != FT) && 
	  ((ReadPara(SYS_ST_CODE) & 0x0004) == 0x0004))	? 1 : 0) << 2)		//bit2		运行（正常运行或SR运行）
	
	| ((((State_Machine != FT) && 
	  ((ReadPara(SYS_ST_CODE) & 0x0004) == 0x0000)) ? 1 : 0) << 3)		//bit3		停机（驱动器状态机不为Fault且无输出）
	
	| ((((Controlword_Out & 0x0200) && CapTestFlag)	? 1 : 0) << 4)		//bit4		后备电源容量测试	
	
	| ((((Controlword_Out & 0x1000) &&				
	  ((ReadPara(SYS_ST_CODE) & 0x2000) == 0x2000)) ? 1 : 0) << 5)		//bit5		后备电源泄放
	
	| (((State_Machine == OP) 						? 1 : 0) << 6)		//bit6		正常运行
	| (((DoubleRAM.StateWord & 0x0700)				? 1 : 0) << 7)		//bit7		SR运行 SR1=1、或SR2=1、或SR3=1
	| (((0) 										? 1 : 0) << 8)		//bit8		SR1状态 ？
	| (((0) 										? 1 : 0) << 9)		//bit9		SR2状态 DSP与ARM通讯失败自主收桨，通讯恢复后可上传到ARM
	| (((ReadPara(DEFECT_CODE) & 0x0010) 			? 1 : 0) << 10)		//bit10		SR3状态 速度开环运行（位置码异常）
	| (((0) 										? 1 : 0) << 11)		//bit11		外部触发紧急顺桨 ？
	| (((ReadPara(CHARG_SYS_ST_CODE) & 0x8000) 		? 1 : 0) << 12)		//bit12		预充电（半控桥开启过程中）
	| (((0) 										? 1 : 0) << 13)		//bit13		预留
	| (((0) 										? 1 : 0) << 14)		//bit14		预留
	| (((0) 										? 1 : 0) << 15);	//bit15		预留
	
	DoubleRAM.inverter_statusword = State_Machine;      				//F01.01    CIA402状态机
	DoubleRAM.ActualPosA = Position_actual_value_s16;               	//F01.02    A编桨叶实际位置			范围：-90.00°~270.00°
	DoubleRAM.ActualPosB = 0;              					 			//F01.03    B编桨叶实际位置
	DoubleRAM.Dsp_SetSpeed = DspSetSpeed;             					//F01.04    桨叶设定转速				0.01°/s
	DoubleRAM.ActualVelocity = ReadPara(PITCH_SPEED);           		//F01.05    桨叶反馈转速				0.01°/s
	DoubleRAM.ActualTorque = ReadPara(ACTUALTORQUE);					//F01.06    实际输出转矩				0.1Nm	 
	DoubleRAM.ActualOutputCurr = labs(Current_actual_value);		  	//F01.07    实际输出电流				0.1A
	DoubleRAM.ActualDCVoltage = DC_link_circuit_voltage * 10;         	//F01.08    直流母线电压				0.1V
	DoubleRAM.GridVoltage = ReadPara(CHARG_AC_VOL);              		//F01.09    电网电压					0.1V
	DoubleRAM.BatteryVoltage = ReadPara(CHARG_V_FEEDBACK) * 10;         //F01.10    超级电容电压				0.1V
	DoubleRAM.ActChargeVoltage = ReadPara(CHARG_V_FEEDBACK) * 10;       //F01.11    实际充电电压				0.1V
	DoubleRAM.ActChargeCurrent = ReadPara(CHARG_I_FEEDBACK) / 10;       //F01.12    实际充电电流				0.1A
	DoubleRAM.BatteryCap = ReadPara(CHARG_CAP)/10;		   				//F01.13    超级电容容量				0.001F
	DoubleRAM.InverterFaultWord1 = ServerFault1;       					//F01.14    驱动器故障字1
	DoubleRAM.InverterFaultWord2 = ServerFault2;       					//F01.15    驱动器故障字2
	DoubleRAM.InverterFaultWord3 = ServerFault3;       					//F01.16    驱动器故障字3
	DoubleRAM.InverterFaultWord4 = ServerFault4;       					//F01.17    驱动器故障字4
	DoubleRAM.ChargerFaultWord   = ServerFault5;        			 	//F01.18    充电器故障字
	DoubleRAM.BatteryFaultWord = ReadPara(CHARG_DEFECT_CODE) << 2;      //F01.19    电池故障字（充电器输出断路）
	DoubleRAM.MotorTemp = ReadPara(MOTO_TEMP) * 10;                		//F01.20    电机温度					0.1℃
	DoubleRAM.HeatsinkTemp = ReadPara(IGBT_TEMP) * 10;             		//F01.21    散热片温度（IGBT温度） 	0.1℃
	DoubleRAM.PT1003Temp = ReadPara(TEMP3);		              			//F01.22    T3温度					0.1℃
	DoubleRAM.Brake = (DSP_Status >> 7) & 0x01;                   		//F01.23    DSP实际输出的抱闸状态反馈
	DoubleRAM.MotorOverLoadPer = ReadPara(OVER_I_TOTAL) / 10;			//F01.24	驱动器超载量反馈			1%
	
	DoubleRAM.HeartbeatCount = ReadPara(DSP_HEARTBEAT_COUNT1);          //F01.25    双口RAM通讯心跳
	
	DoubleRAM.ChargerState = 							             	//F01.26    充电器状态字
	
	  (((((ReadPara(CHARG_SYS_ST_CODE) >> 0) & 1))	? 0 : 1) << 0)		//bit0		停止
	| (((((ReadPara(CHARG_SYS_ST_CODE) >> 0) & 1))	? 1 : 0) << 1)		//bit1		运行
	| (((((ReadPara(CHARG_SYS_ST_CODE) >> 5) & 1))	? 1 : 0) << 2)		//bit2		故障
	| (((1) 										? 1 : 0) << 3)		//bit3		配置完成
	| (((0) 										? 1 : 0) << 4)		//bit4		充电超时（无效）
	| (((0) 										? 1 : 0) << 5)		//bit5		电容容值低（无效）
	| (((0) 										? 1 : 0) << 6)		//bit6		开机条件异常
	| (((((ReadPara(CHARG_SYS_ST_CODE) >> 1) & 1))	? 1 : 0) << 7)		//bit7		充电完成
	| (((0) 										? 1 : 0) << 8)		//bit8		风扇异常
	| (((0) 										? 1 : 0) << 9)		//bit9		预留
	| (((0) 										? 1 : 0) << 10)		//bit10		预留
    | (((0) 										? 1 : 0) << 11)		//bit11		预留
	| (((0) 										? 1 : 0) << 12)		//bit12		预留
	| (((0) 										? 1 : 0) << 13)		//bit13		预留
	| (((0) 										? 1 : 0) << 14)		//bit14		预留
	| (((0) 										? 1 : 0) << 15);	//bit15		预留
	
	DoubleRAM.MotorRotationRPM = MotoRpmSpeed; 							//F01.27    电机RPM转速   
	//DoubleRAM.Relative_humidity = 0;        							//F01.28    驱动器内部湿度
	//DoubleRAM.Acceleration_x = 0;        	  							//F01.29    驱动器X轴加速度
	//DoubleRAM.Acceleration_y = 0;        	  							//F01.30    驱动器Y轴加速度
	//DoubleRAM.Acceleration_z = 0;        	  							//F01.31    驱动器Z轴加速度
	DoubleRAM.Driver_unit_Version = ReadPara(SOFT_VER);	  				//F01.32    驱动器DSP软件版本
	//DoubleRAM.EmergencyAutoRunTIME = 0;	  							//F01.33    驱动器底层急停顺桨时间设定
	
    DoubleRAM.DSP_StateWord =											//F01.34    DSP状态值反馈	
	  (((0) 										? 1 : 0) << 0)		//bit0		预留
	| (((RefCheckFinish & 1)						? 1 : 0) << 1)		//bit1		编码器校准成功
	| (((ReadPara(CHARG_SYS_ST_CODE) & 0x0008)		? 1 : 0) << 2)		//bit2		超级电容容量检测完成					
	| ((((ReadPara(CHARG_SYS_ST_CODE) & 0x0010) || 		
	  (DisChargeFailFlag))							? 1 : 0) << 3)		//bit3		超级电容容量检测失败 一直保持到下一次容量测试的启动时刻
	| (((Fan1ErrFlag) 								? 1 : 0) << 4)		//bit4		驱动器内腔风扇故障
	| (((Fan0ErrFlag) 								? 1 : 0) << 5)		//bit5		驱动器底座风扇故障
	| (((0) 										? 1 : 0) << 6)		//bit6		预留
	| (((0) 										? 1 : 0) << 7)		//bit7		预留
	| (((0) 										? 1 : 0) << 8)		//bit8		预留
	| (((0) 										? 1 : 0) << 9)		//bit9		预留
	| ((((DSP_Status & 0x08)) 						? 1 : 0) << 10)		//bit10		DSP泄放状态标志(bit3)
	| (((GridVoltage_Over500VAC_Flag) 				? 1 : 0) << 11)		//bit11		电网电压超过500V标志
	| (((ReadPara(POSITIONSK)) 						? 1 : 0) << 12)		//bit12		DSP位置跟随超差
	| (((0) 										? 1 : 0) << 13)		//bit13		预留
	| (((ReadPara(CHARG_SYS_ST_CODE) & 0x3800) 		? 1 : 0) << 14)		//bit14		电网缺相
	| (((ReadPara(CHARG_SYS_ST_CODE) & 0x4000) 		? 1 : 0) << 15);	//bit15		三相不平衡	
	
	DoubleRAM.InternalTemp = ReadPara(CHARG_INTERSPACE_TEMP) * 10;     	//F01.35    驱动器内部温度
	DoubleRAM.DSP_ActMotorType = ReadPara(MOTO_TYPE);         			//F01.36    驱动器底层实际电机类型

	//ARM传给DSP的状态值
	DoubleRAM.CapTemp = ReadPara(TEMP2);                				//F02.05    电容柜温度
	DoubleRAM.AxleTemp = ReadPara(TEMP1);        						//F02.
	DoubleRAM.HubTemp = ReadPara(TEMP0);        						//F02.06    轮毂温度
	DoubleRAM.ControlBoardTemp = ReadPara(CHARG_DC24V_TEMP) * 10;     	//F02.07    控制板温度
	
	DoubleRAM.BatteryVoltageMid = ReadPara(CHARG_BTMVOL) * 10;  		//Fxx.xx	BTM电压 ID824
}

/*******************************************************************************
* 函数名 : DoubleRAM_WriteData
* 描  述 : DoubleRAM写数据处理
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/	
void DoubleRAM_WriteData(void)
{
	//if(ReadPara(ARM_RESERVE_PARA_835) != 0xAA55)						//PDO通道数据配置：手动自由配置
	{
		//TPDO(主控 <-- 变桨驱动器)	
		Tpdo1_Data[0] = (u8)(DoubleRAM.TXPDO1_1 >> 0);					//Tpdox_Data[]为CANOpen底层发送函数用数组
		Tpdo1_Data[1] = (u8)(DoubleRAM.TXPDO1_1 >> 8);
		Tpdo1_Data[2] = (u8)(DoubleRAM.TXPDO1_2 >> 0);
		Tpdo1_Data[3] = (u8)(DoubleRAM.TXPDO1_2 >> 8);
		Tpdo1_Data[4] = (u8)(DoubleRAM.TXPDO1_3 >> 0);
		Tpdo1_Data[5] = (u8)(DoubleRAM.TXPDO1_3 >> 8);
		Tpdo1_Data[6] = (u8)(DoubleRAM.TXPDO1_4 >> 0);
		Tpdo1_Data[7] = (u8)(DoubleRAM.TXPDO1_4 >> 8);

		Tpdo2_Data[0] = (u8)(DoubleRAM.TXPDO2_1 >> 0);
		Tpdo2_Data[1] = (u8)(DoubleRAM.TXPDO2_1 >> 8);
		Tpdo2_Data[2] = (u8)(DoubleRAM.TXPDO2_2 >> 0);
		Tpdo2_Data[3] = (u8)(DoubleRAM.TXPDO2_2 >> 8);
		Tpdo2_Data[4] = (u8)(DoubleRAM.TXPDO2_3 >> 0);
		Tpdo2_Data[5] = (u8)(DoubleRAM.TXPDO2_3 >> 8);
		Tpdo2_Data[6] = (u8)(DoubleRAM.TXPDO2_4 >> 0);
		Tpdo2_Data[7] = (u8)(DoubleRAM.TXPDO2_4 >> 8);
	
		Tpdo3_Data[0] = (u8)(DoubleRAM.TXPDO3_1 >> 0);
		Tpdo3_Data[1] = (u8)(DoubleRAM.TXPDO3_1 >> 8);
		Tpdo3_Data[2] = (u8)(DoubleRAM.TXPDO3_2 >> 0);
		Tpdo3_Data[3] = (u8)(DoubleRAM.TXPDO3_2 >> 8);
		Tpdo3_Data[4] = (u8)(DoubleRAM.TXPDO3_3 >> 0);
		Tpdo3_Data[5] = (u8)(DoubleRAM.TXPDO3_3 >> 8);
		Tpdo3_Data[6] = (u8)(DoubleRAM.TXPDO3_4 >> 0);
		Tpdo3_Data[7] = (u8)(DoubleRAM.TXPDO3_4 >> 8);
		
		Tpdo4_Data[0] = (u8)(DoubleRAM.TXPDO4_1 >> 0);
		Tpdo4_Data[1] = (u8)(DoubleRAM.TXPDO4_1 >> 8);
		Tpdo4_Data[2] = (u8)(DoubleRAM.TXPDO4_2 >> 0);
		Tpdo4_Data[3] = (u8)(DoubleRAM.TXPDO4_2 >> 8);
		Tpdo4_Data[4] = (u8)(DoubleRAM.TXPDO4_3 >> 0);
		Tpdo4_Data[5] = (u8)(DoubleRAM.TXPDO4_3 >> 8);
		Tpdo4_Data[6] = (u8)(DoubleRAM.TXPDO4_4 >> 0);
		Tpdo4_Data[7] = (u8)(DoubleRAM.TXPDO4_4 >> 8);										

		//RPDO1(变桨驱动器 <-- 主控)	
		DoubleRAM.RXPDO1_1 = (u16)((u16)Rpdo1_Data[0] << 0) | ((u16)Rpdo1_Data[1] << 8);	//RPDO1_通道1    //Rpdox_Data[]为CANOpen底层接收函数用数组    
		DoubleRAM.RXPDO1_2 = (u16)((u16)Rpdo1_Data[2] << 0) | ((u16)Rpdo1_Data[3] << 8);	//RPDO1_通道2
		DoubleRAM.RXPDO1_3 = (u16)((u16)Rpdo1_Data[4] << 0) | ((u16)Rpdo1_Data[5] << 8);	//RPDO1_通道3
		DoubleRAM.RXPDO1_4 = (u16)((u16)Rpdo1_Data[6] << 0) | ((u16)Rpdo1_Data[7] << 8);	//RPDO1_通道4
		
		DoubleRAM.RXPDO2_1 = (u16)((u16)Rpdo2_Data[0] << 0) | ((u16)Rpdo2_Data[1] << 8);	//RPDO2_通道1    //Rpdox_Data[]为CANOpen底层接收函数用数组    
		DoubleRAM.RXPDO2_2 = (u16)((u16)Rpdo2_Data[2] << 0) | ((u16)Rpdo2_Data[3] << 8);	//RPDO2_通道2
		DoubleRAM.RXPDO2_3 = (u16)((u16)Rpdo2_Data[4] << 0) | ((u16)Rpdo2_Data[5] << 8);	//RPDO2_通道3
		DoubleRAM.RXPDO2_4 = (u16)((u16)Rpdo2_Data[6] << 0) | ((u16)Rpdo2_Data[7] << 8);	//RPDO2_通道4
		
					//================ 2026-09-21 新增：客户标志呼叫DSP保存桨叶角度 ================
			if (SavePitchAngleCmd != 0)											//客户请求保存桨叶角度
			{
				if ((SavePitchDoneLatch == 0) && (SavePitchErrLatch == 0))				//本轮呼叫尚无结果（未收到保存完成、也未超时）→ 保持bit3=1呼叫DSP
				{
					ReadPara(ARM_DSP_COMMPARA0) |= ARM_DSP_COMMPARA0_BIT_SAVE_PITCH;		//bit3=1，随轮询帧下传DSP（约900ms一轮）
				}
				else																	//本轮呼叫已完成（收到DSP保存完成 或 超时判失败）→ 清bit3，防止DSP反复保存桨叶角度
				{
					ReadPara(ARM_DSP_COMMPARA0) &= (u16)(~ARM_DSP_COMMPARA0_BIT_SAVE_PITCH);	//等价于 &= 0xFFF7
				}
			}
			else																		//客户已清除标志，ARM同步清bit3
			{
				ReadPara(ARM_DSP_COMMPARA0) &= (u16)(~ARM_DSP_COMMPARA0_BIT_SAVE_PITCH);	//等价于 &= 0xFFF7
			}
			//=============================================================================

		#ifndef TEST_FOR_STARS																//未定义星辰厂内测试版
			Controlword_Out 	       = DoubleRAM.ArmControlWord;							//控制字
			Target_position_Out        = DoubleRAM.ArmTargetPos;							//位置给定 精度0.01°
			Velocity_Limit_Out 	       = DoubleRAM.ArmProfileVel;							//位置环下的速度限制值
			Target_velocity_Out        = DoubleRAM.ArmTargetVel;							//手动模式下的速度给定	
			RunMode_Out 		       = DoubleRAM.ControlMode;								//运行模式：0=速度环；1=位置环
			ReadPara(CHARG_V_SET)  	   = DoubleRAM.SetChargeVoltage/10;  					//充电电压设定,精度0.1V
            ReadPara(CHARG_I_SET) 	   = DoubleRAM.SetChargeCurrent*10;  					//充电电流设定,精度0.1A
			ReadPara(SAFE_CLOSE_SPEED) = DoubleRAM.EmergencyAutoRunSpe;						//DSP自主收桨转速：300代表3°/S	
			
			err_rst1 = G_Inverter_reset;													//CANOpen或HMI复位指令
			gPosClear = (Controlword_Out >> 8) & 1; 										//DoubleRAM.ArmControlWord.Bit8：桨叶角度校准
			UserPara5 = DoubleRAM.RefCheckVal;												//校准角度
			
			if ((((Controlword_Out >> 0) & 1) == 0) && ((Controlword_Out >> 1) & 1))		//强制开启内部风扇(内腔风机)
			{
				FanOnForce = 2;
			}
			else if ((Controlword_Out >> 0) & 1)											//强制关闭内部风扇(内腔风机)
			{
				FanOnForce = 1;
			}	
			else
			{
				FanOnForce = 0;
			}
			
			if ((ReadPara(CHARG_SYS_ST_CODE) & 0x0008) && (CapTestFinishDelay == 0))
			{
				CapTestFinishDelay = 300;
			}
			else if ((ReadPara(CHARG_SYS_ST_CODE) & 0x0008) == 0)
			{
				CapTestFinishDelay = 0;
			}
			
			if ((Controlword_Out >> 9) & 1)													//DoubleRAM.ArmControlWord.Bit9：容量测试
			{
				CapTestFlag = 1;
				
				if ((ReadPara(CHARG_V_FEEDBACK) <= 290) || (CapTestState >= 1))
				{
					if (CapTestState == 0)
					{
						DisChargeFailFlag = 0;
						CapTestState = 1;
						ChargeRunEnable = 0;
						ChargeRunEnable_Delay = 10000;
					}				
					
					Silicon_Controlled_Rectifier = 0;										//取消关闭半控命令					
					ReadPara(ARM_DSP_COMMPARA0) &= ARM_DSP_COMMPARA0_CLR_KEEP_BIT3;							//取消泄放+开底座风机命令（2026-09-21修改：由 =0 改为 &=0xFFF8，只清bit0~bit2，保留bit3保存桨叶角度请求）
					
					if (((ReadPara(CHARG_SYS_ST_CODE) & 0x0100) == 0x0100) && ((ReadPara(SYS_ST_CODE) & 0x2000) == 0)	//半控桥已开启、且驱动器未处于泄放状态
					&& (ReadPara(C_VOL) > (ReadPara(CHARG_V_SET) + 5)) && (ChargeRunEnable_Delay == 0))		
					{
						ChargeRunEnable = 1;	
						DisCharge_Delay = 0;						
					}
					else
					{
						ChargeRunEnable = 0;
					}
				}
				else if ((ReadPara(CHARG_V_FEEDBACK) > 290) && (CapTestState == 0))
				{
					CapTestState = 0;
					ChargeRunEnable_Delay = 0;
					
					ChargeRunEnable = 0;
					Silicon_Controlled_Rectifier = 1;										//关闭半控桥
					
					if (((ReadPara(SYS_ST_CODE) & 0x0004) == 0) && ((ReadPara(CHARG_SYS_ST_CODE) & 0x0001) == 0) 
					&& ((ReadPara(CHARG_SYS_ST_CODE) & 0x0100) == 0) && (ReadPara(C_VOL) < (ReadPara(CHARG_V_SET) + 5)))				
					{
						ReadPara(ARM_DSP_COMMPARA0) |= 0x0003;								//泄放+开底座风机 需设置ID182泄放电阻=9Ω、ID178泄放功率=800W
						
						if (DisCharge_Delay == 0)
						{
							DisCharge_Delay = 600020;										//10Min+20ms
						}
					}
				}	
				
				if ((DisCharge_Delay > 0) && (DisCharge_Delay < 20) && (ReadPara(CHARG_V_FEEDBACK) > 300) && ((ReadPara(ARM_DSP_COMMPARA0) & 0x0003) == 0x0003))
				{
					DisChargeFailFlag = 1;
				}
				else
				{
					//DisChargeFailFlag = 0;
				}
			}
			else
			{
				if (CapTestFlag)
				{
					Silicon_Controlled_Rectifier = 0;
					ChargeRunEnable = 0;
					ChargeRunEnable_Delay = 0;
					DisCharge_Delay = 0;
					ReadPara(ARM_DSP_COMMPARA0) &= ARM_DSP_COMMPARA0_CLR_KEEP_BIT3;							//2026-09-21修改：由 =0 改为 &=0xFFF8，只清bit0~bit2，保留bit3保存桨叶角度请求
					DisChargeFailFlag = 0;
					CapTestState = 0;
					CapTestFlag = 0;
				}

				ChargeRunEnable = (Controlword_Out >> 11) & 1;								//充电器使能：1=启动；0=禁止
				
				if ((BK & 0x01) == 0x01)													//关抱闸
				{
					if ((Controlword_Out >> 4) & 1)											//DoubleRAM.ArmControlWord.Bit4：电机预热
					{
						PichMotorHeatEna = 1;
						ReadPara(ARM_DSP_COMMPARA0) |= 0x0004;
					}
					else
					{
						PichMotorHeatEna = 0;
						ReadPara(ARM_DSP_COMMPARA0) &= 0xFFFB;
					}		
				}
				else
				{
					PichMotorHeatEna = 0;
					ReadPara(ARM_DSP_COMMPARA0) &= 0xFFFB;
				}				
			
				if ((Controlword_Out >> 5) & 1)												//DoubleRAM.ArmControlWord.Bit5：关闭半控桥
				{
					Silicon_Controlled_Rectifier = 1;
				}
				else
				{
					Silicon_Controlled_Rectifier = 0;
				}	
				
				if ((Controlword_Out >> 2) & 1)												//DoubleRAM.ArmControlWord.Bit2：强制开启外部风扇(底座IGBT风机)
				{
					ReadPara(ARM_DSP_COMMPARA0) |= 0x0001;				
				}
				else
				{
					ReadPara(ARM_DSP_COMMPARA0) &= 0xFFFE;
				}	
				
				if ((Controlword_Out >> 12) & 1)											//DoubleRAM.ArmControlWord.Bit12：超级电容泄放
				{
					if (((ReadPara(SYS_ST_CODE) & 0x0004) == 0) && ((ReadPara(CHARG_SYS_ST_CODE) & 0x0001) == 0) 
					&& ((ReadPara(CHARG_SYS_ST_CODE) & 0x0100) == 0) && (ReadPara(C_VOL) < (ReadPara(CHARG_V_SET) + 5)))		
					{
						ReadPara(ARM_DSP_COMMPARA0) |= 0x0003;								//需设置ID182泄放电阻=9Ω、ID178泄放功率=800W
					}					
				}
				else
				{
					ReadPara(ARM_DSP_COMMPARA0) &= 0xFFFD;
				}	
				
				if ((DoubleRAM.SelcetMotorType != MotorType_Old) && ((ReadPara(SYS_ST_CODE) & 0x0004) == 0))
				{
					SelcetMotorTypeFlag = 1;
				}
				
				if ((SelcetMotorTypeFlag) && (ExCANopenRxFullFlag == 0))					//存盘电机类型
				{												
					SelcetMotorTypeFlag = 0;
				}
				MotorType_Old = DoubleRAM.SelcetMotorType;													
			}
		#else
			Target_position_Out = Target_position_Test;										//位置给定 精度0.01°
			Velocity_Limit_Out 	= Velocity_Limit_Test;										//位置环下的速度限制值
			Target_velocity_Out = Target_velocity_Test;										//手动模式下的速度给定				
			err_rst1 = (Func_0x06_Data[0] >> 1) & 1;										//故障复位
			gPosClear = (Func_0x06_Data[1] >> 0) & 1;										//桨叶角度校准位
			UserPara5 = Func_0x06_Data[10];													//桨叶校准角度，目前HMI默认下发0°	
			
			if ((((Func_0x06_Data[2] >> 10) & 1) == 0) && ((Func_0x06_Data[2] >> 9) & 1))	//强制开启内部风扇(内腔风机)
			{
				FanOnForce = 2;
			}
			else if ((Func_0x06_Data[2] >> 10) & 1) 										//强制关闭内部风扇(内腔风机)
			{
				FanOnForce = 1;
			}	
			else
			{
				FanOnForce = 0;
			}
			
			if ((Func_0x10_Data[2] >> 7) & 1)												//容量测试
			{
				CapTestFlag = 1;
				
				if ((ReadPara(CHARG_V_FEEDBACK) <= 290) || (CapTestState == 1))
				{
					CapTestState = 1;
					
					Silicon_Controlled_Rectifier = 0;										//取消关闭半控命令					
					ReadPara(ARM_DSP_COMMPARA0) &= ARM_DSP_COMMPARA0_CLR_KEEP_BIT3;							//取消泄放+开底座风机命令（2026-09-21修改：由 =0 改为 &=0xFFF8，只清bit0~bit2，保留bit3保存桨叶角度请求）
					
					if (((ReadPara(CHARG_SYS_ST_CODE) & 0x0100) == 0x0100) && ((ReadPara(SYS_ST_CODE) & 0x2000) == 0)//半控桥已开启、且驱动器未处于泄放状态
					&& (ReadPara(C_VOL) > (ReadPara(CHARG_V_SET) + 5)))						
					{
						ChargeRunEnable = 1;												
					}
				}
				else if ((ReadPara(CHARG_V_FEEDBACK) > 290) && (CapTestState == 0))
				{
					CapTestState = 0;
					
					Silicon_Controlled_Rectifier = 1;										//关闭半控桥
					
					if (((ReadPara(SYS_ST_CODE) & 0x0004) == 0) && ((ReadPara(CHARG_SYS_ST_CODE) & 0x0001) == 0) 
					&& ((ReadPara(CHARG_SYS_ST_CODE) & 0x0100) == 0) && (ReadPara(C_VOL) < (ReadPara(CHARG_V_SET) + 5)))					
					{
						ReadPara(ARM_DSP_COMMPARA0) |= 0x0003;								//泄放+开底座风机 需设置ID182泄放电阻=9Ω、ID178泄放功率=800W
					}
				}	
			}
			else																			//非容量测试模式
			{
				if (CapTestFlag)
				{
					Silicon_Controlled_Rectifier = 0;
					ChargeRunEnable = 0;
					ReadPara(ARM_DSP_COMMPARA0) &= ARM_DSP_COMMPARA0_CLR_KEEP_BIT3;							//2026-09-21修改：由 =0 改为 &=0xFFF8，只清bit0~bit2，保留bit3保存桨叶角度请求
					CapTestState = 0;
					CapTestFlag = 0;
				}
				
				if ((BK & 0x01) == 0x01)													//关抱闸
				{
					if ((Controlword_Out >> 4) & 1)											//DoubleRAM.ArmControlWord.Bit4：电机预热
					{
						PichMotorHeatEna = 1;
						ReadPara(ARM_DSP_COMMPARA0) |= 0x0004;
					}
					else
					{
						PichMotorHeatEna = 0;
						ReadPara(ARM_DSP_COMMPARA0) &= 0xFFFB;
					}		
				}
				else
				{
					PichMotorHeatEna = 0;
					ReadPara(ARM_DSP_COMMPARA0) &= 0xFFFB;
				}
				

				
				if ((Func_0x06_Data[2] >> 6) & 1)											//充电使能（注意：点击HMI“禁止充电”开启充电）
				{
					ChargeRunEnable = 1;				
				}
				else
				{
					ChargeRunEnable = 0;	
				}
				
				if ((Func_0x10_Data[2] >> 4) & 1)											//关闭半控桥(主电源隔离指令)
				{
					Silicon_Controlled_Rectifier = 1;
				}
				else
				{
					Silicon_Controlled_Rectifier = 0;
				}	
				
				if ((Func_0x06_Data[2] >> 8) & 1)											//强制开启外部风扇(底座IGBT风机)
				{
					ReadPara(ARM_DSP_COMMPARA0) |= 0x0001;				
				}
				else
				{
					ReadPara(ARM_DSP_COMMPARA0) &= 0xFFFE;
				}
				
				if ((Func_0x10_Data[2] >> 5) & 1)											//超级电容泄放
				{
					if (((ReadPara(SYS_ST_CODE) & 0x0004) == 0) && ((ReadPara(CHARG_SYS_ST_CODE) & 0x0001) == 0) 			//驱动器关使能&&充电器关使能
					&& ((ReadPara(CHARG_SYS_ST_CODE) & 0x0100) == 0) && (ReadPara(C_VOL) < (ReadPara(CHARG_V_SET) + 5)))	//半控桥关使能&&母线电压小于充电电压设定+5V			
					{
						ReadPara(ARM_DSP_COMMPARA0) |= 0x0003;								//需设置ID182泄放电阻=9Ω、ID178泄放功率=800W
					}					
				}
				else
				{
					ReadPara(ARM_DSP_COMMPARA0) &= 0xFFFD;
				}	
				
				if ((Func_0x06_Data[36]  != MotorType_Old) && ((ReadPara(SYS_ST_CODE) & 0x0004) == 0))
				{
					SelcetMotorTypeFlag = 1;
				}
				
				if ((SelcetMotorTypeFlag) && (ExCANopenRxFullFlag == 0))					//存盘电机类型		
				{												
					SelcetMotorTypeFlag = 0;
				}
				MotorType_Old = Func_0x06_Data[36];								
			}
		#endif
	}
	/*
	else																					//PDO通道数据配置：1=eds文件配置					
	{
		//RPDO(主控 --> 变桨驱动器)															//[通道]		[索引号]		[定义]
		Controlword_Out = Controlword;														//RPDO1_通道1	0x6040		控制字
		Target_position_Out = Target_position;												//RPDO1_通道2	0x607A		位置给定 精度0.01°
		Velocity_Limit_Out = Target_Speed;													//RPDO1_通道3	0x6102		位置环下的速度限制值
		Target_velocity_Out = ActPosition1;													//RPDO1_通道4	0x6103		速度给定
		
		RunEnable 		= (Controlword_Out >> 0) & 1;										//RPDO1_通道1	0x6040		Bit0  驱动器使能
		ChargeRunEnable = (Controlword_Out >> 1) & 1;										//RPDO1_通道1	0x6040		Bit1  充电器使能
		err_rst1 		= (Controlword_Out >> 2) & 1; 										//RPDO1_通道1	0x6040		Bit2  故障复位
		gPosClear 		= (Controlword_Out >> 3) & 1; 										//RPDO1_通道1	0x6040		Bit3  旋变角度校准
		HeartBeat	 	= (Controlword_Out >> 4) & 1;										//RPDO1_通道1	0x6040		Bit4  心跳
		RunMode 		= (Controlword_Out >> 5) & 1;										//RPDO1_通道1	0x6040		Bit5  运行模式：0=位置；1=速度
		
		//TPDO(主控 <-- 变桨驱动器)															//[通道]		[索引号]		[定义]
		Axis_Status_Word1 = ReadPara(SYS_ST_CODE);											//TPDO1_通道1	0x610A		轴状态字1
		Axis_Status_Word2 = 0;																//TPDO1_通道2	0x610B		轴状态字2
		Axis1_2_3_Drive_Fault1 = ReadPara(DEFECT_CODE);										//TPDO1_通道3	0x610C		轴1/2/3驱动器故障字1
		Axis1_2_3_Drive_Fault2 = ReadPara(DEFECT_CODEH);									//TPDO1_通道4	0x610D		轴1/2/3驱动器故障字2
		
		Axis1_2_3_Drive_DiStatusword1 = ReadPara(DI_STA);									//TPDO2_通道1	0x610E		轴1/2/3驱动器DI状态字1
		Axis1_2_3_Drive_DiStatusword2 = ReadPara(DIH_STA);									//TPDO2_通道2	0x610F		轴1/2/3驱动器DI状态字2
		Axis_Pitch_Actual_Position = Position_actual_value;									//TPDO2_通道3	0x6110		轴桨叶实际位置 精度0.01°
		Axis_Pitch_Speed = Velocity_sensor_actual_value/100;								//TPDO2_通道4	0x6111		轴变桨速度 精度0.1°/S
		
		Axis_Sever_Temperature = 0;															//TPDO3_通道1	0x6113		轴驱动器温度
		Axis_Motor_Temperature = 0;															//TPDO3_通道2	0x6114		轴电机温度
		Axis_Cabinet_Temperature = 0;														//TPDO3_通道3	0x6115		轴柜温度
		Cap_Cabinet_Temperature = 0;														//TPDO3_通道4	0x6116		电容柜温度

		Backup_Voltage = ReadPara(CHARG_V_FEEDBACK);										//TPDO4_通道1	0x6118		后备电源电压 精度1V
		Sever_Dclink_Voltage = ReadPara(C_VOL);												//TPDO4_通道2	0x6119		驱动器直流母线电压 精度1V
		Pitch_Fault_Word1 = 0;																//TPDO4_通道3	0x611A		变桨系统故障码1
		Pitch_Fault_Word2 = 0;																//TPDO4_通道4	0x611B		变桨系统故障码2
	}
	*/
}

/*******************************************************************************
* 函数名 : ResetCharger
* 描  述 : 充电器复位操作
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void ResetCharger(void)
{ 	
	static u8 i=0,j=0;
	
	//if(err_rst1)
	if (ReadPara(ERR_RST) || err_rst1)
	{		
		if(i<5)							// 5次复位
		{
			WriteChargPara((18 | 0x8000), err_rst1);
			i++;
		}
		
		j = 0;
	}
	else
	{
		if(i != 0)
		{
			err_rst1_cnt = 1000;
		}
		
		if(err_rst1_cnt == 0)			// TIM3的1ms中断中err_rst1_cnt --操作，1000就是1S中后给充电器的ID18发送0值，否则充电器会一直ERRRES=1
		{
			if(j<5)						// 5次清零
			{
				WriteChargPara((18 | 0x8000), 0);
				j++;
			}
		}
		
		i = 0;
	}
}

/*******************************************************************************
* 函数名 : RunModePro
* 描  述 : 运行模式处理
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void RunModePro(void)
{ 		
    DecodeAll = (u64)ReadPara(DEFECT_CODE) << 0 | (u64)ReadPara(DEFECT_CODEH) << 16 | (u64)ReadPara(CHARG_DEFECT_CODE) << 32;

	if ((DIStatus.bit.DI00 == 0) || ((ReadPara(SYS_ST_CODE) & (1<<1)) == 0) || (State_Machine == FT)) 	
	{		
		User_PLC_Sta = PLC_STA_STOP;								// 抱闸停机
	}	
	else if (RunMode == 0)											
	{
		User_PLC_Sta = PLC_STA_HAND;								// 手动模式
	}
	else if (RunMode && RunEnable)									
	{
		User_PLC_Sta = PLC_STA_PLOOP; 								// 位置环运行
	}
	else
	{
		User_PLC_Sta = PLC_STA_STOP;								// 抱闸停机
	}
}

/*******************************************************************************
* 函数名 : StopRun
* 描  述 : 抱闸停机
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void StopRun(void)
{
    User_sn_on = 0; 			
    User_sn_off = 0x55AA; 		//关使能
	UserSafeFinishFlag = 1; 		
    User_target_value = 0;
    User_PLC_Step = 0;
}

/*******************************************************************************
* 函数名 : PloopRun
* 描  述 : 位置环运行
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PloopRun(void)
{
    switch (User_PLC_Step)
    {
        case 0:
        {
			if (InUartErrFlag == 0)
			{
				if (ReadPara(WORK_MOD) != P_LOOP)
				{
					if (ExCANopenRxFullFlag == 0) 
					{
						UpSysPara(0x80, WORK_MOD,P_LOOP);		
					}	
				}
				else
				{
					User_PLC_Step++;
				}
			}

            break;
        }
		
		case 1:
		{
			if (ReadPara(WORK_MOD) != P_LOOP)
			{
				if (ExCANopenRxFullFlag == 0) 
				{
					UpSysPara(0x80, WORK_MOD,P_LOOP);		
				}	
			}
				
			if (Runrequest)														// 有使能命令
			{
				UserTargetPosition = Target_position_Out;						// 位置给定 精度0.01°
				
				User_sn_on = 0xAA55; 											// 开使能
				User_sn_off = 0;
				User_PLC_Step = 1;
			}
			else															
			{
				User_sn_on = 0;
				User_sn_off = 0x55AA; 											// 关使能
				User_PLC_Step = 0;
			}
			
			break;
		}

        default:
        {
			User_sn_on = 0;
			User_sn_off = 0x55AA; 								
			User_PLC_Step = 0;
            break;
        }
    }
}

/*******************************************************************************
* 函数名 : VloopRun
* 描  述 : 速度环运行
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void VloopRun(void)
{
	s32 Temp = 0;
	
	//Temp = ReadPara(SAFE_CLOSE_SPEED) * 0x6C00;
	//Temp = Temp/Max_motor_speed;
	Temp = (long long)ReadPara(SAFE_CLOSE_SPEED) * ReadPara(SCALE_B) / 600 * 0x6C00 / ReadPara(SCALE_A) / ReadPara(MAX_V);
	
	if (Temp > 0x6C00)
	{
		Temp = 0x6C00;
	}
	else if (Temp < -0x6C00)
	{
		Temp = -0x6C00;
	}
	
	User_target_value = Temp;
	
    switch (User_PLC_Step)
    {
        case 0:
        {		
			if (InUartErrFlag == 0)
			{
				if (ReadPara(WORK_MOD) != V_LOOP)
				{
					if (ExCANopenRxFullFlag == 0) 
					{
						UpSysPara(0x80, WORK_MOD,V_LOOP);		
					}	
				}
				else
				{
					User_PLC_Step = 1;
				}
				
				if (DIStatus.bit.DI00 == 0) 						//DI0=0
				{
					User_target_value = 0;
					User_PLC_Step = 3;	
				}
			}

            break;
        }

        case 1:
        {
			if (ReadPara(WORK_MOD) != V_LOOP)
			{
				if (ExCANopenRxFullFlag == 0) 
				{
					UpSysPara(0x80, WORK_MOD,V_LOOP);		
				}	
			}
				
            User_sn_on = 0xAA55; 				//开使能
            User_sn_off = 0;
            User_PLC_Step++;
            break;
        }

        case 2:
        {
			User_sn_on = 0xAA55; 				//开使能
            User_sn_off = 0;

			if (DIStatus.bit.DI00 == 0)  					//DI0=0
            {
                User_target_value = 0;
                User_PLC_Step = 3;
            }
            break;
        }

        case 3:
        {
			//if (labs(MotoRpmSpeed) < 10)								// 速度小于10RPM才给出抱闸停机指令	
            {
                User_sn_on = 0;
                User_sn_off = 0x55AA; 			//关使能
				AngleCrcErr_VloopRunFlag = 0;
            }
			
            User_target_value = 0;
			
			if (DIStatus.bit.DI00) 	
			{
				User_PLC_Step = 0;
			}
            break;
        }

        default:
        {
            User_PLC_Step = 0;
            break;
        }
    }
}

/*******************************************************************************
* 函数名 : SafeRun
* 描  述 : 安全收桨运行
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void SafeRun(void)
{

}

/*******************************************************************************
* 函数名 : Hand_Mode_Windey
* 描  述 : HMI手动速度环调桨控制（1=点动模式、2=区间模式、3=位置模式）
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void Hand_Mode_Windey(void)
{												
// 	HMI_Pich_Control_Vset = Target_velocity_Out;									
// 	HMI_Pich_Control_Vset = HMI_Pich_Control_Vset * 6 * ReadPara(SCALE_B);
// 	HMI_Pich_Control_Vset /= 3600;
// 	HMI_Pich_Control_Vset *= 0x6C00;
// 	HMI_Pich_Control_Vset = HMI_Pich_Control_Vset / ReadPara(SCALE_A) / ReadPara(MAX_V);
									
	HMI_Pich_Control_Vset = (long long)Target_velocity_Out * ReadPara(SCALE_B) / 600 * 0x6C00 / ReadPara(SCALE_A) / ReadPara(MAX_V);
	
	//1.00°/S换算为电机端RPM举例：1.00°/S * 60S * 大齿2000 / 小齿1 / 360°= 333RPM
	//换算为DSP速度环给定值：333RPM / 最高转速3000RPM * 0x6C00 = 0x0BFC
	
	if (HMI_Pich_Control_Vset > 0x6C00)
	{
		HMI_Pich_Control_Vset = 0x6C00;
	}
	else if (HMI_Pich_Control_Vset < -0x6C00)
	{
		HMI_Pich_Control_Vset = -0x6C00;
	}
	
	switch (User_PLC_Step)
	{
		case 0:
		{
			if (InUartErrFlag == 0)
			{
				if (ReadPara(WORK_MOD) != V_LOOP)
				{
					if (ExCANopenRxFullFlag == 0) 
					{
						UpSysPara(0x80, WORK_MOD,V_LOOP);		
					}	
				}
				else
				{
					User_PLC_Step ++;
				}
			}

			break;
		}

		case 1:
		{
			if (ReadPara(WORK_MOD) != V_LOOP)
			{
				if (ExCANopenRxFullFlag == 0) 
				{
					UpSysPara(0x80, WORK_MOD,V_LOOP);		
				}	
			}
				
			if (Runrequest)														// 有使能命令
			{
				User_target_value = HMI_Pich_Control_Vset;						// 速度给定
				User_sn_on = 0xAA55; 											// 开使能
				User_sn_off = 0;
				User_PLC_Step = 1;
			}
			else																
			{
				User_target_value = 0;
				User_sn_on = 0;
				User_sn_off = 0x55AA; 											// 关使能
				User_PLC_Step = 0;
			}

			break;
		}

		default:
		{
			User_target_value = 0;
			User_sn_on = 0;
			User_sn_off = 0x55AA; 								
			User_PLC_Step = 0;
			break;
		}
	}
}

/*******************************************************************************
* 函数名 : Hand_Mode_Stars
* 描  述 : HMI手动调桨控制（模式反馈值：0=无、1=点动模式、2=区间模式、3=位置模式）
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void Hand_Mode_Stars(void)
{
	HMI_Pich_Control_Reg   = Func_0x06_Data[PICH_CONTROL_ADRESS];					// 调桨控制：地址01，模式：点动模式、区间模式、位置模式
	HMI_Pich_Control_Vset  = Func_0x06_Data[VLOOP_SPEED_ADRESS];					// 点动控制：地址11，目标速度范围：0.00°/S ~ 10°/S
	HMI_Pich_Control_Pset1 = Func_0x06_Data[OPEN_ANGLE_ADRESS];						// 区间控制：地址14，开桨角度最小值 = -2.00°
	HMI_Pich_Control_Sped1 = Func_0x06_Data[OPEN_SPEED_ADRESS];						// 区间控制：地址15，开桨速度范围：0.00°/S ~ 10°/S
	HMI_Pich_Control_Pset2 = Func_0x06_Data[CLOSE_ANGLE_ADRESS];					// 区间控制：地址13，顺桨角度最大值 = 94.00°
	HMI_Pich_Control_Sped2 = Func_0x06_Data[CLOSE_SPEED_ADRESS];					// 区间控制：地址16，顺桨速度范围：0.00°/S ~ 10°/S 
	HMI_Pich_Control_Pset3 = Func_0x06_Data[PLOOP_ANGLE_ADRESS];					// 位置控制：地址17，目标角度最大值 = 94.00°
	HMI_Pich_Control_Sped3 = Func_0x06_Data[PLOOP_SPEED_ADRESS];					// 位置控制：地址18，目标速度范围：0.00°/S ~ 10°/S
	
	RunMode_Test = 0;																// 速度模式
	
	if ((HMI_Pich_Control_Reg & 0x0002) == 0x0002)									// 点动控制
	{
		HMI_Pich_Control_Mode = 1;
		
		HMI_Pich_Control_Vset = (long long)HMI_Pich_Control_Vset * ReadPara(SCALE_B) / 600 * 0x6C00 / ReadPara(SCALE_A) / ReadPara(MAX_V);
		
		if (HMI_Pich_Control_Vset > 0x6C00)
		{
			HMI_Pich_Control_Vset = 0x6C00;
		}
		else if (HMI_Pich_Control_Vset < -0x6C00)
		{
			HMI_Pich_Control_Vset = -0x6C00;
		}
		
		switch (User_PLC_Step)
		{
			case 0:
			{	
				if (InUartErrFlag == 0)
				{
					if (ReadPara(WORK_MOD) != V_LOOP)
					{
						if (ExCANopenRxFullFlag == 0) 
						{
							UpSysPara(0x80, WORK_MOD,V_LOOP);		
						}	
					}
					else
					{
						User_PLC_Step ++;
					}					
				}		

				break;
			}

			case 1:
			{
				if (ReadPara(WORK_MOD) != V_LOOP)
				{
					if (ExCANopenRxFullFlag == 0) 
					{
						UpSysPara(0x80, WORK_MOD,V_LOOP);		
					}	
				}
					
				if ((HMI_Pich_Control_Reg & 0x0004) == 0x0004)						// 点动控制开桨
				{
					User_target_value = -HMI_Pich_Control_Vset;						// 负转速
					
					/*
					if (Position_actual_value >= -27000)
					{
						User_target_value = -HMI_Pich_Control_Vset;					// 负转速
					}
					else
					{
						User_target_value = 0;										// 角度范围限制如：-270.00° ~ 95.00°，超出此范围转速强制为0°/S
					}
					*/
					
					Runrequest_Test = 1;									
					User_sn_on = 0xAA55; 											// 开使能
					User_sn_off = 0;
					User_PLC_Step = 1;
				}
				else if ((HMI_Pich_Control_Reg & 0x0008) == 0x0008)					// 点动控制顺桨
				{
					User_target_value = HMI_Pich_Control_Vset;						// 正转速
					
					/*
					if (Position_actual_value <= 9500)
					{
						User_target_value = HMI_Pich_Control_Vset;					// 正转速
					}
					else
					{
						User_target_value = 0;										// 角度范围限制如：-270.00° ~ 95.00°，超出此范围转速强制为0°/S
					}
					*/
					
					Runrequest_Test = 1;	
					User_sn_on = 0xAA55; 											// 开使能
					User_sn_off = 0;
					User_PLC_Step = 1;
				}
				else																// 点动控制停止
				{
					Runrequest_Test = 0;	
					User_target_value = 0;
					User_sn_on = 0;
					User_sn_off = 0x55AA; 											// 关使能
					User_PLC_Step = 0;
				}

				break;
			}

			default:
			{
				Runrequest_Test = 0;	
				User_target_value = 0;
				User_sn_on = 0;
				User_sn_off = 0x55AA; 								
				User_PLC_Step = 0;
				break;
			}
		}
	}
	else if ((HMI_Pich_Control_Reg & 0x0040) == 0x0040)								// 区间控制
	{
		HMI_Pich_Control_Mode = 2;
		
		if (HMI_Pich_Control_Dir == 0)
		{
			HMI_Pich_Control_Vset = (long long)HMI_Pich_Control_Sped1 * ReadPara(SCALE_B) / 600 * 0x6C00 / ReadPara(SCALE_A) / ReadPara(MAX_V);
			
			if (HMI_Pich_Control_Vset > 0x6C00)
			{
				HMI_Pich_Control_Vset = 0x6C00;
			}
			else if (HMI_Pich_Control_Vset < -0x6C00)
			{
				HMI_Pich_Control_Vset = -0x6C00;
			}
		
			if (Position_actual_value > HMI_Pich_Control_Pset1)						// 开桨角度地址=14，目标位置最小值=-2.00°
			{
				User_target_value = -HMI_Pich_Control_Vset;							// 负转速
			}
			else
			{
				HMI_Pich_Control_Dir = 1;
			}
		}	
		else
		{
			HMI_Pich_Control_Vset = (long long)HMI_Pich_Control_Sped2 * ReadPara(SCALE_B) / 600 * 0x6C00 / ReadPara(SCALE_A) / ReadPara(MAX_V);
			
			//1.00°/S换算为电机端RPM举例：1.00°/S * 60S * 大齿2000 / 小齿1 / 360°= 333RPM
			//换算为DSP速度环给定值：333RPM / 最高转速3000RPM * 0x6C00 = 0x0BFC
			
			if (HMI_Pich_Control_Vset > 0x6C00)
			{
				HMI_Pich_Control_Vset = 0x6C00;
			}
			else if (HMI_Pich_Control_Vset < -0x6C00)
			{
				HMI_Pich_Control_Vset = -0x6C00;
			}
			
			if (Position_actual_value < HMI_Pich_Control_Pset2)						// 顺桨角度地址=13，目标位置最大值=94.00°
			{
				User_target_value = HMI_Pich_Control_Vset;							// 正转速
			}
			else
			{
				HMI_Pich_Control_Dir = 0;
			}
		}	
		
		switch (User_PLC_Step)
		{
			case 0:
			{
				if (InUartErrFlag == 0)
				{
					if (ReadPara(WORK_MOD) != V_LOOP)
					{
						if (ExCANopenRxFullFlag == 0) 
						{
							UpSysPara(0x80, WORK_MOD,V_LOOP);		
						}	
					}
					else
					{
						User_PLC_Step ++;
					}					
				}	

				break;
			}

			case 1:
			{
				if (ReadPara(WORK_MOD) != V_LOOP)
				{
					if (ExCANopenRxFullFlag == 0) 
					{
						UpSysPara(0x80, WORK_MOD,V_LOOP);		
					}	
				}
				
				if ((HMI_Pich_Control_Reg & 0x0080) == 0x0080)						// 区间控制启动
				{
					Runrequest_Test = 1;
					User_sn_on = 0xAA55; 											// 开使能
					User_sn_off = 0;
					User_PLC_Step = 1;
				}
				else																// 区间控制停止
				{
					Runrequest_Test = 0;
					User_sn_on = 0;
					User_sn_off = 0x55AA; 											// 关使能
					User_PLC_Step = 0;
				}

				break;
			}

			default:
			{
				Runrequest_Test = 0;	
				User_target_value = 0;
				User_sn_on = 0;
				User_sn_off = 0x55AA; 								
				User_PLC_Step = 0;
				break;
			}
		}
	}	
	else if ((HMI_Pich_Control_Reg & 0x0200) == 0x0200)								// 位置控制
	{
		HMI_Pich_Control_Mode = 3;

		HMI_Pich_Control_Vset = (long long)HMI_Pich_Control_Sped3 * ReadPara(SCALE_B) / 600 * 0x6C00 / ReadPara(SCALE_A) / ReadPara(MAX_V);
		
		if (HMI_Pich_Control_Vset > 0x6C00)
		{
			HMI_Pich_Control_Vset = 0x6C00;
		}
		else if (HMI_Pich_Control_Vset < -0x6C00)
		{
			HMI_Pich_Control_Vset = -0x6C00;
		}
		
		switch (User_PLC_Step)
		{
			case 0:
			{	
				if (InUartErrFlag == 0)
				{
					if (ReadPara(WORK_MOD) != V_LOOP)
					{
						if (ExCANopenRxFullFlag == 0) 
						{
							UpSysPara(0x80, WORK_MOD,V_LOOP);		
						}	
					}
					else
					{
						User_PLC_Step ++;
					}				
				}
		
				break;
			}

			case 1:
			{
				if (ReadPara(WORK_MOD) != V_LOOP)
				{
					if (ExCANopenRxFullFlag == 0) 
					{
						UpSysPara(0x80, WORK_MOD,V_LOOP);		
					}	
				}
					
				if ((HMI_Pich_Control_Reg & 0x0400) == 0x0400)						// 位置控制启动
				{
					User_target_value = HMI_Pich_Control_Vset;						// 正转速
					Runrequest_Test = 1;
					User_sn_on = 0xAA55; 											// 开使能
					User_sn_off = 0;
					User_PLC_Step = 1;
				}
				else																// 区间控制停止
				{
					User_target_value = 0;
					Runrequest_Test = 0;
					User_sn_on = 0;
					User_sn_off = 0x55AA; 											// 关使能
					User_PLC_Step = 0;
				}

				break;
			}

			default:
			{
				Runrequest_Test = 0;	
				User_target_value = 0;
				User_sn_on = 0;
				User_sn_off = 0x55AA; 								
				User_PLC_Step = 0;
				break;
			}
		}
	}	
	else																			
	{
		HMI_Pich_Control_Dir = 0;
		Runrequest_Test = 0;
		User_target_value = 0;
		User_sn_on = 0;
		User_sn_off = 0x55AA; 														// 关使能
		User_PLC_Step = 0;
		HMI_Pich_Control_Mode = 0;
	}
}

/*******************************************************************************
* 函数名 : HandMode
* 描  述 : 
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void HandMode(void)
{
	#ifndef TEST_FOR_STARS													
		Hand_Mode_Windey();									
	#else
		Hand_Mode_Stars();										// 星辰测试用
	#endif	
}

/*******************************************************************************
* 函数名 : OpenRun
* 描  述 : 开环收桨运行
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void OpenRun(void)
{
	double Temp = 0;
	
	Temp = ReadPara(OPEN_CLOSE_SPEED) * 0x6C00;
	Temp = Temp/Max_motor_speed;
	
	if (Temp > 0x6C00)
	{
		Temp = 0x6C00;
	}
	else if (Temp < 0)
	{
		Temp = 0;
	}
	
	User_target_value = Temp;
	
    switch (User_PLC_Step)
    {
        case 0:
        {
			if (InUartErrFlag == 0)
			{
				if (ReadPara(WORK_MOD) != V_LOOP)
				{
					if (ExCANopenRxFullFlag == 0) 
					{
						UpSysPara(0x80, WORK_MOD,V_LOOP);		
					}	
				}
				else
				{
					User_PLC_Step = 1;
				}	
				
				if (DIStatus.bit.DI00 == 0) 						
				{
					User_target_value = 0;
					User_PLC_Step = 3;	
				}
			}

            break;
        }

        case 1:
        {
			if (ReadPara(WORK_MOD) != V_LOOP)
			{
				if (ExCANopenRxFullFlag == 0) 
				{
					UpSysPara(0x80, WORK_MOD,V_LOOP);		
				}	
			}
				
            User_sn_on = 0xAA55; 				//开使能
            User_sn_off = 0;
            User_PLC_Step++;
            break;
        }

        case 2:
        {
			User_sn_on = 0xAA55; 				//开使能
            User_sn_off = 0;

			if (DIStatus.bit.DI00 == 0)  		//DI0=0
            {
                User_target_value = 0;
                User_PLC_Step = 3;
            }
            break;
        }

        case 3:
        {
			//if (labs(MotoRpmSpeed) < 10)							// 速度小于10RPM才给出抱闸停机指令
            {
                User_sn_on = 0;
                User_sn_off = 0x55AA; 			//关使能
				UserSafeFinishFlag = 1; 		//收桨完成
            }
			
            User_target_value = 0;
			
			if (DIStatus.bit.DI00) 	
			{
				User_PLC_Step = 0;
			}
            break;
        }

        default:
        {
            User_PLC_Step = 0;
            break;
        }
    }
}

/*******************************************************************************
* 函数名 : UserMainTask
* 描  述 : 用户主任务
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void UserMainTask(void)
{
	DoubleRAM_InitData();
	
	if (((Init_flag2 == 0) && (CrcPassFlag == 0)) || (((DSP_Status >> 5) & 0x01) == 0))
	{	
		#if TEST_FOR_STARS
		RPDO_Recv_link_analysis();  ///是否CANopen升级判断)	
		#endif	
		return;
	}
	
	//================ 2026-09-21 新增：客户标志"0→1跳变"=一轮新的保存请求 ================
	//说明：客户标志为1期间，上一轮的判断结果保持不变（客户清掉自己的标志后仍能读到上一轮结果）；
	//      只有客户重新发起（标志由0变为非0）时，才清掉上一轮的判断结果与超时计时。
	if ((SavePitchCmdOld == 0) && (SavePitchAngleCmd != 0))
	{
			SavePitchDoneLatch   = 0;
			SavePitchErrLatch    = 0;
			SavePitchOverTimeCnt = 0;
			ReadPara(DOUBLERAM_DATA_04) = 0;					//清上一轮上报结果（ID949）
	}
	SavePitchCmdOld = SavePitchAngleCmd;					//记录本周期客户标志，供下周期做0→1跳变判断
	//====================================================================================

	if((SavePitchAngleCmd != 0) && (ReadPara(ARM_DSP_COMMPARA0) & ARM_DSP_COMMPARA0_BIT_SAVE_PITCH) )
	{
			//================================================================================
			//判断DSP_Status 获得桨叶角度保存的结果（2026-09-21新增）
			//DSP_Status：ARM与DSP串口回帧中的扩展状态字节（InterUart.c 中 u8 DSP_Status；DSP_Status = InUartRxBuf[REV_EX_NO=13]），
			//            每收到一帧DSP应答刷新一次（约30ms一帧）；通讯中断期间该值保持最后一次的旧值，
			//            所以判结果前必须先确认与DSP通讯正常。
			//DSP_Status 现有位：bit7=抱闸状态、bit6=底座风机状态、bit5=桨叶角度初始化完成、bit4=DSP心跳、bit3=DSP泄放状态；
			//本功能新增使用（不新增宏，按与DSP侧约定直接给掩码值）：
			//    bit1(0x02)=1 ：DSP反馈本次桨叶角度保存已完成（成功）；DSP侧只用这一个位表示保存结果
			//    bit0/bit2   ：本功能不使用（DSP不反馈失败，是否失败由ARM侧在同一请求期间超时判定）
			//结果去向：ID949(DOUBLERAM_DATA_04)：bit0=保存完成、bit1=保存失败或超时、bit8=DSP_Status.bit1原始值(诊断用)，
			//          星辰上位机可直接监控该参数；结果保留到客户下一次"0→1跳变"发起新请求时才清零，
			//          若客户需要在别的.c文件中读取结果，请把这4个static变量改为全局并在ExternPrototype.h中加extern声明。
			//================================================================================
			if ((InUartErrFlag != 1) && (CrcPassFlag == 666))		//与DSP通讯正常（无通讯失败标志，且ARM/DSP参数打包校验通过）→ DSP_Status有效
			{
				if (DSP_Status & 0x02)								//bit1=1：DSP已完成本次桨叶角度保存（唯一的保存结果位）
				{
					SavePitchDoneLatch = 1;						//锁存保存完成（DSP_Status约30ms刷新一次，避免只判瞬时值而漏掉结果）
					SavePitchErrLatch  = 0;						//收到保存完成反馈，清除ARM侧的超时判失败
				}
			}

			if ((SavePitchDoneLatch == 0) && (SavePitchErrLatch == 0))	//尚未得到结果时开始/继续计时
			{
				if (SavePitchOverTimeCnt < 3000)					//2ms*3000=6S：覆盖约900ms轮询+DSP保存耗时，留足余量
				{
					SavePitchOverTimeCnt++;
				}
				else
				{
					SavePitchErrLatch = 1;						//在ARM给出bit3期间累计超时仍收不到DSP_Status.bit1 → 判失败
				}
			}

			//本轮呼叫已有结果（收到DSP保存完成 或 超时判失败）→ 立即清bit3，不再呼叫DSP保存；
			//（清位后本判定块的外层条件不再成立，结果锁存保持不变，直到客户重新发起：标志0→1跳变）
			if ((SavePitchDoneLatch) || (SavePitchErrLatch))
			{
				ReadPara(ARM_DSP_COMMPARA0) &= (u16)(~ARM_DSP_COMMPARA0_BIT_SAVE_PITCH);	//清bit3（等价 &= 0xFFF7）
			}

			//结果上报星辰上位机监控参数ID949
			ReadPara(DOUBLERAM_DATA_04) = (u16)(((SavePitchDoneLatch)   ? 0x0001 : 0)		//bit0 保存完成
			                                  | ((SavePitchErrLatch)    ? 0x0002 : 0)		//bit1 保存失败/超时
			                                  | (((DSP_Status & 0x02)  ? 0x0100 : 0)));	//bit8 DSP_Status.bit1原始值（诊断）
	}
	
    UserWritePort();
	DefectCodeMap();
	//MainDriveState_Control();														// 下发状态机控制命令
	//Servo_State_Machine();														// CiA402状态机跳转
	//DoubleRAM_ReadData();
	
	#if TEST_FOR_STARS
    if(!RPDO_Recv_link_analysis())  ///是否CANopen升级判断)
	#endif		
    {
        DoubleRAM_WriteData();  //非CANopen升级，执行变桨逻辑
    }
	ResetCharger();
    RunModePro();

	if (User_PLC_Sta != User_PLC_StaOld) 											// 当前状态与上次状态不同，即切换状态
    {
        User_PLC_Step = 0;
    }	
	
    switch (User_PLC_Sta)
    {
        case PLC_STA_STOP: 		// 1=抱闸停机
        {
            StopRun();
            break;
        }

        case PLC_STA_PLOOP: 	// 2=正常变桨(位置环)
        {
            PloopRun();
            break;
        }
		
        case PLC_STA_VLOOP: 	// 5=速度收桨(速度环)
        {
            VloopRun();
            break;
        }
		
		case PLC_STA_HAND:		// 11=手动变桨(速度环)
        {
            HandMode();
            break;
        }
		
        case PLC_STA_OPENLOOP: 	// 20=开环收桨(速度环)
        {
            OpenRun();
            break;
        }

        default:
        {
            break;
        }
    }
	
	User_PLC_StaOld = User_PLC_Sta;
	ReadPara(PITCH_PLC_STATE) = User_PLC_Sta;
}
