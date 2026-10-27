#include "state_machine.h"
#include "ExternPrototype.h"
#include "TaskScheduler.h"
#include "TimeBase.h"
#include "InterUart.H"
#include "AppFunc.h"

#ifndef TEST_FOR_STARS
	#include "AppInputOutPut.h"
	#include "GlobalVar_POUS.h"
#else
#endif

u8 err_rst1_delay = 0;
u8 State_Machine = 1;
u32 CiA402_Statusword = 0;

bool RunMode = 1; 						// 运行模式 0=速度环；1=位置环
bool RunMode_Out = 1; 					
bool RunMode_Test = 0;
bool Runrequest = 0;					// 运行命令 0=关使能；1=开使能
bool Runrequest_Out = 0;				
bool Runrequest_Test = 0;				
bool G_Inverter_reset_Old = 0;

extern s16 Velocity_Limit_Out;			// 位置环下的最高转速默认2.00°/S
extern s16 Velocity_Limit_Test;
extern u16 ServerFault1;
extern u16 ServerFault2;
extern u16 ServerFault3;
extern u16 ServerFault4;
extern u16 ServerFault5;
extern u64 DecodeAll;
extern u16 RunEnable;				
extern u8 err_rst1;
extern union DI_STATUS DIStatus;

/*	Device Control For CiA402
【Controlword】
bit0：Switch_On
bit1：Enable_Voltage 
bit2：Quick_Stop(Active when 0)
bit3：Enable_Operation
bit7：Fault_Reset
	
#define  SHUTDOWN        			0x0006
#define  SHUTDOWN_MARK    			0x0087
#define  SWITCH_ON        			0x0007
#define  SWITCH_ON_MARK   			0x008F
#define  DISABLE_VOLTAGE  			0x0000
#define  DISABLE_VOLTAGE_MARK 		0x0082
#define  DISABLE_OPERATION 			0x0007
#define  DISABLE_OPERATION_MARK 	0x008F
#define  ENABLE_OPERATION   		0x000F
#define  ENABLE_OPERATION_MARK 		0x008F
#define  QUICK_STOP       			0x0002
#define  QUICK_STOP_MARK  			0x0086
#define  FAULT_RESET           		0x0080
#define  FAULT_RESET_MARK      		0x0080

(Controlword & Command_Mask) = Command				bit7			bit3				bit2			bit1				bit0			Transitons
													Fault_Reset		Enable_Operation	Quick_Stop		Enable_Voltage		Switch_On		
(Controlword & 0x0087) = 0x0006 = SHUTDOWN			0				x					1				1					0				2、6、8
(Controlword & 0x008F) = 0x0007 = SWITCH_ON			0				0					1				1					1				3
(Controlword & 0x0082) = 0x0000 = DISABLE_VOLTAGE	0				x					x				0					x				7、9、10、12
(Controlword & 0x008F) = 0x000F = ENABLE_OPERATION	0				1					1				1					1				4、16
(Controlword & 0x008F) = 0x0007 = DISABLE_OPERATION	0				0					1				1					1				5
(Controlword & 0x0086) = 0x0002 = QUICK_STOP		0				x					0				1					x				11
(Controlword & 0x0080) = 0x0080 = FAULT_RESET		1				x					x				x					x				15

【Statusword】
bit0：Ready to switch on
bit1：Switched on 
bit2：Operation enable
bit3：Fault
bit4：Votage enable
bit5：Quick stop(Active when 0)
bit6：Switch on disable

					 STARS		   MICC		State					  xxxx xxxx 7654 3210	Key Bit Value
#define  NRSO_VALUE  0x0000    	// 0x0000	Not Ready to Switch On	= xxxx xxxx x0xx 0000	bit6：Switch on disable=0
#define  SOD_VALUE   0x0050    	// 0x0040	Switch on disable		= xxxx xxxx x1xx 0000	bit6：Switch on disable=1
#define  RSO_VALUE   0x0031    	// 0x0021	Ready to Switch On		= xxxx xxxx x01x 0001	bit0：Ready to switch on=1
#define  SO_VALUE    0x0033    	// 0x0023	Switched On 			= xxxx xxxx x01x 0011	bit1：Switch on=1
#define  OP_VALUE    0x0037    	// 0x0027	Operation Enable		= xxxx xxxx x01x 0111	bit2：Operation enable=1
#define  QSA_VALUE   0x0007    	// 0x0007	Quick Stop Active		= xxxx xxxx x00x 0111	bit5：Quick stop=0
#define  FRA_VALUE   0x000F    	// 0x000F	Fault Reaction Active	= xxxx xxxx x0xx 1111	bit3：Fault=1
#define  FT_VALUE    0x0008    	// 0x0008	Fault					= xxxx xxxx x0xx 1000	bit3：Fault=1

【State Machine】
#define NRSO  1    				// Not Ready to Switch On
#define SOD   2    				// Switch on disable
#define RSO   3    				// Ready to Switch On
#define SO    4    				// Switched On 
#define OP    5    				// Operation Enable
#define QSA   6    				// Quick Stop Active
#define FRA   7    				// Fault Reaction Active
#define FT    8    				// Fault
*/

/************************************
* 函数名 :
* 描  述 : 伺服状态机处理函数
* 输  入 : 
* 输  出 : 
* 返  回 : 
************************************/
void Servo_State_Machine(void)
{
	// 产生了顺桨的故障不进入FRA状态，继续保持Operation状态，顺桨过程中、或收到89°位置，上位机都不会下发复位指令，需手动复位；
	// 产生了位置码异常(开环)、或ARM与DSP通讯故障，进入FRA状态，直到碰DI0=0(ENPO)、或DI3=10(95开关)、或DI5=1(手动)、或收桨超时，才过渡到FT状态；
	// 产生了其它所有抱闸停机故障进入FRA后，都从FRA无延时自然过渡到FT,如：IGBT过温2等；10S后上位机每间隔200ms就下发600ms高电平复位脉冲指令；FT故障码清除后，方可跳转到SOD状态。
	// 若在FRA状态下DI5=1，进入FT，超时计数器停止；DI5=0，则又进入FRA，并且继续超时计数
	// 另：系统状态机定义：T1=Power On、T2=StandBy、T3=Normol、T4=Emergency、T5=HandMode
	
	if (((ServerFault1 & 0x4000) || (ServerFault3 & 0x0600))															//有FRA运行故障
	|| ((ServerFault1 & 0x897E) || (ServerFault2 & 0x8382) || (ServerFault3 & 0x4000) || (ServerFault4 & 0xA000)))		//有抱闸停机故障
	{
		State_Machine = FRA;								// Transitons 13：状态机跳转到7故障响应
    }

    switch (State_Machine)
    {
		case NRSO:                                          //【1】Not Ready to Switch On（初始化）
			RunEnable = 0;
			CiA402_Statusword = NRSO_VALUE;					// NRSO_VALUE=xxxx xxxx x0xx 0000	bit6：Switch on disable=0

			State_Machine = SOD;							// Transitons 01：状态机自然过渡到2		
			break;

		case SOD:       									//【2】Switch on disable（开关未使能）
			RunEnable = 0;
			CiA402_Statusword = SOD_VALUE;					// SOD_VALUE=xxxx xxxx x1xx 0000	bit6：Switch on disable=1
					
			if ((DoubleRAM.inverter_controlword & SHUTDOWN_MARK) == SHUTDOWN)	
			{												// &0x87==0x0006：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=1、bit1接通主回路电=1、bit0开启=0
				State_Machine = RSO;						// Transitons 02：状态机跳转到3
			}			
			break;
			
		case RSO:     										//【3】Ready to Switch On（准备闭合开关）
			RunEnable = 0;
			CiA402_Statusword = RSO_VALUE;					// RSO_VALUE=xxxx xxxx x01x 0001	bit0：Ready to Switch On=1
		
			if ((DoubleRAM.inverter_controlword & SWITCH_ON_MARK) == SWITCH_ON) 
			{												// &0x8F==0x0007：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=1、bit1接通主回路电=1、bit0开启=1
				State_Machine = SO;							// Transitons 03：状态机跳转到4
			}
			else if ((DoubleRAM.inverter_controlword & DISABLE_VOLTAGE_MARK) == DISABLE_VOLTAGE)
			{												// &0x82==0x0000：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=0、bit1接通主回路电=0、bit0开启=0
				State_Machine = SOD;						// Transitons 07：状态机跳转到2
			}		
			break;

		case SO:											//【4】Switch On（闭合开关）
			RunEnable = 0;
			CiA402_Statusword = SO_VALUE;					// SO_VALUE=xxxx xxxx x01x 0011		bit1：Switch On=1

			if ((DoubleRAM.inverter_controlword & ENABLE_OPERATION_MARK) == ENABLE_OPERATION)
			{												// &0x8F==0x000F：bit7复位=0、bit3伺服运行=1、bit2快速停机无效=1、bit1接通主回路电=1、bit0开启=1
				State_Machine = OP;							// Transitons 04：状态机跳转到5
			}
			else if ((DoubleRAM.inverter_controlword & DISABLE_VOLTAGE_MARK) == DISABLE_VOLTAGE)
			{												// &0x82==0x0000：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=0、bit1接通主回路电=0、bit0开启=0
				State_Machine = SOD;						// Transitons 10：状态机跳转到2	注意：DISABLE_VOLTAGE“或”QUICK_STOP命令都可以跳转
			}
			else if ((DoubleRAM.inverter_controlword & QUICK_STOP_MARK) == QUICK_STOP)
			{												// &0x86==0x0002：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=0、bit1接通主回路电=1、bit0开启=0
				State_Machine = SOD;						// Transitons 10：状态机跳转到2	注意：DISABLE_VOLTAGE“或”QUICK_STOP命令都可以跳转	
			}	
			else if ((DoubleRAM.inverter_controlword & SHUTDOWN_MARK) == SHUTDOWN)
			{												// &0x87==0x0006：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=1、bit1接通主回路电=1、bit0开启=0
				State_Machine = RSO;						// Transitons 06：状态机跳转到3
			}
			break;

		case OP: 											//【5】Operation Enable（使能电机通电运行）
			RunEnable = 1;
			CiA402_Statusword = OP_VALUE;					// OP_VALUE=xxxx xxxx x01x 0111		bit2：Operation Enable=1
			
			if ((DoubleRAM.inverter_controlword & DISABLE_OPERATION_MARK) == DISABLE_OPERATION /*& RFG == 1*/)	
			{												// &0x8F==0x0007：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=1、bit1接通主回路电=1、bit0开启=1
				State_Machine = SO;							// Transitons 05：状态机跳转到4
			}
			else if ((DoubleRAM.inverter_controlword & SHUTDOWN_MARK) == SHUTDOWN)
			{												// &0x87==0x0006：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=1、bit1接通主回路电=1、bit0开启=0
				State_Machine = RSO;						// Transitons 08：状态机跳转到3
			}
			else if ((DoubleRAM.inverter_controlword & DISABLE_VOLTAGE_MARK) == DISABLE_VOLTAGE)
			{												// &0x82==0x0000：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=0、bit1接通主回路电=0、bit0开启=0
				State_Machine = SOD;						// Transitons 09：状态机跳转到2
			}
			else if ((DoubleRAM.inverter_controlword & QUICK_STOP_MARK) == QUICK_STOP)
			{												// &0x86==0x0002：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=0、bit1接通主回路电=1、bit0开启=0
				State_Machine = QSA;						// Transitons 11：状态机跳转到6
			}
			break;

		case QSA:											//【6】Quick Stop Active（急停激活）
			RunEnable = 0;
			CiA402_Statusword = QSA_VALUE;					// QSA_VALUE=xxxx xxxx x00x 0111	bit5：Quick Stop=0
		
			if ((DoubleRAM.inverter_controlword & ENABLE_OPERATION_MARK) == ENABLE_OPERATION)
			{												// &0x8F==0x000F：bit7复位=0、bit3伺服运行=1、bit2快速停机无效=1、bit1接通主回路电=1、bit0开启=1
				State_Machine = OP;							// Transitons 16：状态机跳转到5
			}
			else if ((DoubleRAM.inverter_controlword & DISABLE_VOLTAGE_MARK) == DISABLE_VOLTAGE)
			{												// &0x82==0x0000：bit7复位=0、bit3伺服运行=0、bit2快速停机无效=0、bit1接通主回路电=0、bit0开启=0
				State_Machine = SOD;						// Transitons 12：状态机跳转到2
			}
			break;

		case FRA:        									//【7】Fault Reaction Active（故障响应）
			CiA402_Statusword = FRA_VALUE;					// FRA_VALUE=xxxx xxxx x0xx 1111	bit3：Fault=1

			if ((DIStatus.bit.DI05 == 1) || (DIStatus.bit.DI03 == 0) || ((ReadPara(SYS_ST_CODE) & 0x00004) == 0)) //DI5=1、或DI3=0、或DSP关使能	
			{
				RunEnable = 0;			
				CiA402_Statusword = FT_VALUE;				// FT_VALUE=xxxx xxxx x0xx 1000	bit3：Fault=1
				State_Machine = FT;							// Transitons 14：状态机跳转到8
			}
			
			if ((ServerFault1 & 0x897E) || (ServerFault2 & 0x8382) || (ServerFault3 & 0x4000) || (ServerFault4 & 0xA000))		//有抱闸停机故障
			{
				RunEnable = 0;			
				CiA402_Statusword = FT_VALUE;				// FT_VALUE=xxxx xxxx x0xx 1000	bit3：Fault=1
				State_Machine = FT;							// Transitons 14：状态机跳转到8
			}
			break;

		case FT:       										//【8】Fault（故障）		
			RunEnable = 0;			
			CiA402_Statusword = FT_VALUE;					// FT_VALUE=xxxx xxxx x0xx 1000	bit3：Fault=1

			//若尝试复位仍有FRA或FT故障，不能跳转到SOD
			if (((DoubleRAM.inverter_controlword & FAULT_RESET_MARK) == FAULT_RESET)
			&& ((ServerFault1 & 0x897E) == 0) && ((ServerFault2 & 0x8382) == 0) && ((ServerFault3 & 0x4000) == 0) && ((ServerFault4 & 0xA000) == 0)	//无抱闸停机故障
			&& ((ServerFault1 & 0x4000) == 0) && ((ServerFault3 & 0x0600) == 0))									//无FRA运行故障
			{												// &0x0080==0x0080：bit7复位=1、bit3伺服运行=0、bit2快速停机无效=0、bit1接通主回路电=0、bit0开启=0
				State_Machine = SOD;						// Transitons 15：状态机跳转到2
			}
			// 压限位时打手动，没有FRA故障和抱闸停机故障情况下，切换状态机为SOD
			else if ((DIStatus.bit.DI05 == 1) && (DIStatus.bit.DI03 == 0)
			&& ((ServerFault1 & 0x897E) == 0) && ((ServerFault2 & 0x8382) == 0) && ((ServerFault3 & 0x4000) == 0) && ((ServerFault4 & 0xA000) == 0)	//无抱闸停机故障
			&& ((ServerFault1 & 0x4000) == 0) && ((ServerFault3 & 0x0600) == 0))									//无FRA运行故障
			{												// &0x0080==0x0080：bit7复位=1、bit3伺服运行=0、bit2快速停机无效=0、bit1接通主回路电=0、bit0开启=0
				State_Machine = SOD;						// Transitons 15：状态机跳转到2
			}
			
			break;

		default:
			break;
    }
	
	#ifndef TEST_FOR_STARS							
		DriveP6FSM = State_Machine;
	#else
	
	#endif
}

/************************************
* 函数名 :
* 描  述 : 状态切换
* 输  入 : 
* 输  出 : 
* 返  回 : 
************************************/
void MainDriveState_Control (void)
{
	#ifndef TEST_FOR_STARS													
		Runrequest = G_Driver.Runrequest;				
		//RunMode = RunMode_Out;
		RunMode = DoubleRAM.ControlMode;
		ReadPara(P_PV) = Velocity_Limit_Out;
	#else
		Runrequest = Runrequest_Test;										// 星辰测试用
		RunMode = RunMode_Test;
		ReadPara(P_PV) = Velocity_Limit_Test;
	#endif	
	
	switch (State_Machine)
	{
		case STATUS_NOT_READY_TO_SWITCH_ON:									// 当前状态机：初始化
		{
			break;
		}
		
		case STATUS_SWITCH_ON_DISABLED:										// 当前状态机：开关未使能
		{	
			if (Runrequest)													// 软件使能控制
			{	
				DoubleRAM.inverter_controlword = SHUTDOWN;  
			}
			break;
		}
		
		case STATUS_READY_TO_SWITCH_ON:										// 当前状态机：准备闭合开关
		{	
			if (Runrequest)													// 软件使能控制
			{	
				DoubleRAM.inverter_controlword = SWITCH_ON;			
			}
			else
			{	
				DoubleRAM.inverter_controlword = DISABLE_VOLTAGE;  
			}
			
			break;		
		}
		
		case STATUS_SWITCHED_ON:											// 当前状态机：闭合开关
		{
			if (Runrequest)
			{	
				DoubleRAM.inverter_controlword = ENABLE_OPERATION; 
			}
			else
			{ 	
				DoubleRAM.inverter_controlword = SHUTDOWN;			
			}
			break;
		}
			
		case STATUS_OPERATION_ENABLED:										// 当前状态机：使能电机通电运行
		{
			if (!Runrequest) 
			{	
				DoubleRAM.inverter_controlword = DISABLE_OPERATION;	
			}
			
			/*
			if(?)
			{
				DoubleRAM.inverter_controlword = QUICK_STOP;
			}
			*/
			break;
		}
			
		case STATUS_QUICK_STOP_ACTIVE:										// 当前状态机：急停激活
		{
			DoubleRAM.inverter_controlword = DISABLE_VOLTAGE;
			break;
		}
			
		case STATUS_FAULT_REACTION_ACTIVE:									// 当前状态机：故障响应
		{
			//DoubleRAM.inverter_controlword = DISABLE_VOLTAGE;				// 自动过渡到故障状态(过渡延时自定义)，无需控制指令	
			break;
		}
		
		case STATUS_FAULT:													// 当前状态机：故障
		{
			#ifndef TEST_FOR_STARS													
				if ((G_Inverter_reset_Old == 0) && G_Inverter_reset)		// 有复位命令上升沿
				{
					DoubleRAM.inverter_controlword = FAULT_RESET;
				}
				else
				{
					DoubleRAM.inverter_controlword &= (~FAULT_RESET);
				}
				
				G_Inverter_reset_Old = G_Inverter_reset;
			#else
				if ((err_rst1) ||											// 有故障复位指令	
				((ServerFault1 == 0) && (ServerFault2 == 0) && (ServerFault3 == 0) && (ServerFault4 == 0)))
				{
					DoubleRAM.inverter_controlword = FAULT_RESET;
				}
			#endif	
			break;
		}
		
		default:
		{	
			DoubleRAM.inverter_controlword = DISABLE_VOLTAGE;	
			break;
		}	
	}
}
