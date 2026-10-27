#define CANopen_GLOBALS
#include "ExternPrototype.h"

#define CANopenParaReadTableSize   (sizeof(CANopenParaReadTable)/sizeof(CANopenParaType))
#define CANopenParaWriteTableSize  (sizeof(CANopenParaWriteTable)/sizeof(CANopenParaType))

u16 co_life_time_factor_u16;
u16 co_rpdo1_transmission_type_u16;
u16 co_rpdo2_transmission_type_u16;
u16 co_rpdo3_transmission_type_u16;
u16 co_rpdo4_transmission_type_u16;
u16 co_tpdo1_transmission_type_u16;
u16 co_tpdo2_transmission_type_u16;
u16 co_tpdo3_transmission_type_u16;
u16 co_tpdo4_transmission_type_u16;

extern u8 ExUart_RxBuf[];
extern u8 InUartErrFlag;
extern u16 ExUartRxFullFlag;   		// 接收RS485一帧完成标志
extern u16 PcCtrl; 					// 上位机控制标志：1上位机控制，0上位机不控制
extern u16 PcCtrlFlag; 				// 上位机开关使能标志：1开使能，0关使能
extern u16 s_up_time;       		// S曲线上升时间
extern u16 s_down_time;     		// S曲线下降时间
extern u16 igbt_safe_temp;  		// IGBT过热温度保护点
extern u16 max_t;           		// 最大扭矩
extern u16 c_vol;          	 		// 母线电压
extern u16 limit_t;         		// 转矩限制值
extern u16 limit_v;         		// 位置环的速度限制值
extern u16 scale_a;         		// 位置设定传递比分子
extern u16 scale_b;         		// 位置设定传递比分母
extern u16 SSI_scale_a;         	// 绝对位置反馈传递比分子
extern u16 SSI_scale_b;         	// 绝对位置反馈传递比分母
extern u16 ModeCtrl; 				// 工作模式控制字
extern u16 RatedCurrent; 			// 额定电流
extern s16 BKOnDelay; 				// 制动器开延时
extern s16 BKOffDelay; 				// 制动器关延时
extern u16 gray_en; 				// 格雷码解码允许
extern s16 sec_pos_num; 			// 第二编码器圈数
extern u16 sec_pos; 				// 第二编码器位置
extern u16 Homing_speeds; 			// 安全收桨速度
extern u16 safe_close_up_time; 		// 安全收桨上升梯度延时 1 = 10ms
extern u16 safe_close_down_time; 	// 安全收桨下降梯度延时 1 = 10ms
extern u16 motor_pos_rst; 			// 电机位置复位
extern s16 SecCoderOffset; 			// 第二编码器圈数偏移
extern u16 SecCoderDir; 			// 第二编码器计数方向选择：0逆时针加计数，1顺时针加计数
extern u16 BK_Switch; 				// 制动器开关：0制动器与使能联动；1制动器关，2制动器开
extern u16 User_PLC_Step;
extern u16 SpeedOverproofT; 		// 速度超差时间：1对应0.1秒
extern s32 PosReg;           		// 圈数*65536 + 位置
extern s32 SSI_PosReg;       		// 圈数*65536 + 位置
extern u8 canopen_para_precoss_flag;
extern u16 defect_code;
extern u8 SendParaPackToDsp_Flag;
extern INTEGER16 Target_position_Out;
extern u8 RunState;
extern u64 DecodeAll;
extern int FastVol;
extern u8 Init_flag2;
extern DefectConfigType DefectConfigParaID[41];
extern u16 Controlword_Out;
extern s16 Target_position_Out;
extern u16 User_PLC_Sta;
extern u16 SafeRunPositon;
extern s16 Velocity_Limit_Out;

u8 Arm_Dsp_CommErr_Flag = 0;
u8 Arm_Dsp_CommErr_Step = 0;
u8 CANOpen_Comm_Enable = 0;
u8 CANopen_Step = 0;
u16 DspHeartBeatCount = 0;
u16 DspHeartBeatCountOld = 0;
u16 DspHeartBeatSameCount = 0;
u16 CANopen_i;
u16 CANopen_Num = 0;
s16 err_rst_s16;
s16 target_value = 0;
u16 open_close_speed;
u16 user_para1_h = 0; // 用户保存参数1
u16 user_para1_l = 0; // 用户保存参数1
u16 user_para2_h = 0; // 用户保存参数2
u16 user_para2_l = 0; // 用户保存参数2
u16 user_para3_h = 0; // 用户保存参数3
u16 user_para3_l = 0; // 用户保存参数3

//MPC项目的eds文件已无以下变量如t_set的定义，所以CANOpen通讯参数无法与星辰参数联动，以下变量目前为预留用
INTEGER16 t_set = 0x0;		/* Mapped at index 0x2006, subindex 0x00 */
INTEGER16 User_C_Vol = 0x0;		/* Mapped at index 0x200B, subindex 0x00 */
UNS16 Service_Time_Delay = 0x1F4;		/* Mapped at index 0x200D, subindex 0x00 */
INTEGER32 MotorAngelNew = 0x0;		/* Mapped at index 0x200E, subindex 0x00 */
INTEGER16 igbt_temp = 0x0;		/* Mapped at index 0x2011, subindex 0x00 */
INTEGER16 moto_temp = 0x0;		/* Mapped at index 0x2012, subindex 0x00 */
INTEGER16 AxleCab_Temp = 0x0;		/* Mapped at index 0x2013, subindex 0x00 */
INTEGER16 CapCab_Temp = 0x0;		/* Mapped at index 0x2014, subindex 0x00 */
INTEGER16 Hub_Humi_Temp = 0x0;		/* Mapped at index 0x2015, subindex 0x00 */
UNS16 Over_I_Total = 0x0;		/* Mapped at index 0x2016, subindex 0x00 */
UNS16 SpeedOverproofT = 0x0;		/* Mapped at index 0x2017, subindex 0x00 */
UNS16 v_pi = 0x32;		/* Mapped at index 0x2019, subindex 0x00 */
UNS16 p_pv = 0x7530;		/* Mapped at index 0x201A, subindex 0x00 */
UNS16 max_i = 0x69;		/* Mapped at index 0x2024, subindex 0x00 */
UNS16 RatedCurrent = 0x190;		/* Mapped at index 0x2025, subindex 0x00 */
UNS16 max_t = 0x69;		/* Mapped at index 0x2026, subindex 0x00 */
UNS16 moto_safe_temp = 0x8C;		/* Mapped at index 0x2027, subindex 0x00 */
UNS16 igbt_safe_temp = 0x5A;		/* Mapped at index 0x2028, subindex 0x00 */
UNS16 work_mod = 0x2;		/* Mapped at index 0x2033, subindex 0x00 */
UNS16 ModeCtrl = 0x0;		/* Mapped at index 0x2034, subindex 0x00 */
INTEGER8 err_rst = 0x0;		/* Mapped at index 0x2035, subindex 0x00 */
UNS16 motor_pos_rst = 0x0;		/* Mapped at index 0x2036, subindex 0x00 */
UNS16 BK_Switch = 0x0;		/* Mapped at index 0x2037, subindex 0x00 */
UNS16 s_up_time = 0x0;		/* Mapped at index 0x2043, subindex 0x00 */
UNS16 s_down_time = 0x0;		/* Mapped at index 0x2044, subindex 0x00 */
UNS16 v_dir_mod = 0x1;		/* Mapped at index 0x2047, subindex 0x00 */
UNS16 limit_t = 0x6C00;		/* Mapped at index 0x2050, subindex 0x00 */
UNS16 limit_v = 0x6C00;		/* Mapped at index 0x2051, subindex 0x00 */
INTEGER16 BKOnDelay = 0xC8;		/* Mapped at index 0x2056, subindex 0x00 */
INTEGER16 BKOffDelay = 0x78;		/* Mapped at index 0x2057, subindex 0x00 */
UNS16 safe_close_up_time = 0x1E;		/* Mapped at index 0x2059, subindex 0x00 */
UNS16 safe_close_down_time = 0x1E;		/* Mapped at index 0x205A, subindex 0x00 */
UNS16 SecCoderDir = 0x1;		/* Mapped at index 0x205C, subindex 0x00 */
INTEGER16 motor_pos_num = 0x0;		/* Mapped at index 0x2060, subindex 0x00 */
UNS16 motor_pos = 0x0;		/* Mapped at index 0x2061, subindex 0x00 */
INTEGER16 sec_pos_num = 0x0;		/* Mapped at index 0x2063, subindex 0x00 */
UNS16 sec_pos = 0x0;		/* Mapped at index 0x2064, subindex 0x00 */
INTEGER32 SecCoderAngle = 0x0;		/* Mapped at index 0x2065, subindex 0x00 */
UNS16 SafeDI_Dly = 0x1F4;		/* Mapped at index 0x2073, subindex 0x00 */
UNS32 UserPara1 = 0x0;		/* Mapped at index 0x2080, subindex 0x00 */
UNS32 UserPara2 = 0x0;		/* Mapped at index 0x2081, subindex 0x00 */
UNS16 DischargeTime = 0x96;		/* Mapped at index 0x2082, subindex 0x00 */
UNS16 DischargeAccTime = 0x12C;		/* Mapped at index 0x2083, subindex 0x00 */
UNS16 DischargeTimeOutSlope = 0x12C;		/* Mapped at index 0x2084, subindex 0x00 */
UNS16 DischargeTimeThreshold = 0x96;		/* Mapped at index 0x2085, subindex 0x00 */
UNS32 UserPara3 = 0x0;		/* Mapped at index 0x2086, subindex 0x00 */
UNS16 charg_defect_code = 0x0;		/* Mapped at index 0x2090, subindex 0x00 */
UNS16 charg_sys_st_code = 0x0;		/* Mapped at index 0x2091, subindex 0x00 */
INTEGER16 charg_igbt_temp = 0x0;		/* Mapped at index 0x2092, subindex 0x00 */
UNS16 charg_vol = 0x0;		/* Mapped at index 0x2093, subindex 0x00 */
UNS16 VoltageOfUser = 0x0;		/* Mapped at index 0x2094, subindex 0x00 */
INTEGER16 charg_i_feedback = 0x0;		/* Mapped at index 0x2095, subindex 0x00 */
UNS16 charg_v_set = 0x1C2;		/* Mapped at index 0x2096, subindex 0x00 */
INTEGER16 charg_i_set = 0xFA;		/* Mapped at index 0x2097, subindex 0x00 */
UNS16 charg_ctrl_reg = 0x0;		/* Mapped at index 0x2098, subindex 0x00 */
UNS16 charg_ctrl_mod = 0x0;		/* Mapped at index 0x2099, subindex 0x00 */
UNS32 Error_code2 = 0x0;		/* Mapped at index 0x2100, subindex 0x00 */
UNS16 IO_Control = 0x0;		/* Mapped at index 0x2101, subindex 0x00 */
UNS16 DI_Error_OutSide_Choose = 0x0;		/* Mapped at index 0x2102, subindex 0x00 */
UNS16 FltPraOfCurrent = 0x100;		/* Mapped at index 0x2103, subindex 0x00 */
UNS16 UserPara4 = 0x0;		/* Mapped at index 0x2104, subindex 0x00 */
UNS16 UserPara5 = 0x0;		/* Mapped at index 0x2105, subindex 0x00 */
UNS16 UserPara6 = 0x0;		/* Mapped at index 0x2106, subindex 0x00 */
UNS16 UserPara7 = 0x0;		/* Mapped at index 0x2107, subindex 0x00 */
UNS16 UserPara8 = 0x0;		/* Mapped at index 0x2108, subindex 0x00 */
UNS16 UserPara9 = 0x0;		/* Mapped at index 0x2109, subindex 0x00 */
UNS16 UserPara10 = 0x0;		/* Mapped at index 0x210A, subindex 0x00 */
INTEGER16 SafeCloseACC_PD = 0xF;		/* Mapped at index 0x210B, subindex 0x00 */
INTEGER16 SafeCloseDec_PD = 0xF;		/* Mapped at index 0x210C, subindex 0x00 */
UNS16 FltPraOfDCVol = 0x0;		/* Mapped at index 0x210D, subindex 0x00 */
UNS16 FltPraOfVelocity = 0x0;		/* Mapped at index 0x210E, subindex 0x00 */
INTEGER32 EncAUserActPos = 0x0;		/* Mapped at index 0x3013, subindex 0x00 */
INTEGER32 EncBUserActPos = 0x0;		/* Mapped at index 0x3014, subindex 0x00 */
UNS16 sn_on = 0x0;		/* Mapped at index 0x4003, subindex 0x00 */
UNS16 sn_off = 0x0;		/* Mapped at index 0x4004, subindex 0x00 */
UNS16 save_para = 0x0;		/* Mapped at index 0x4005, subindex 0x00 */
UNS32 RWParaComm = 0x0;		/* Mapped at index 0x4006, subindex 0x00 */
UNS32 RWParaData = 0x0;		/* Mapped at index 0x4007, subindex 0x00 */
UNS16 esc_remote = 0x0;		/* Mapped at index 0x4009, subindex 0x00 */

UNS16 Servo_Err_OverLoad_On_Off = 0x1;		/* Mapped at index 0x5000, subindex 0x01 */
UNS16 Servo_Err_OverLoad_Operating_Mode = 0x0;		/* Mapped at index 0x5000, subindex 0x02 */
UNS16 Servo_Err_OverLoad_Ena_Delay = 0x0;		/* Mapped at index 0x5000, subindex 0x03 */
UNS16 Servo_Err_OverLoad_Res_Delay = 0x0;		/* Mapped at index 0x5000, subindex 0x04 */
UNS16 Servo_Err_OverLoad_Res_Type = 0x0;		/* Mapped at index 0x5000, subindex 0x05 */
UNS16 Servo_Err_OverLoad_Trigger_Conditions = 0x5;		/* Mapped at index 0x5000, subindex 0x06 */
UNS16 Servo_Err_OverLoad_Safer_Off = 0x1;		/* Mapped at index 0x5000, subindex 0x07 */
UNS16 Servo_Err_OverVol_On_Off = 0x1;		/* Mapped at index 0x5001, subindex 0x01 */
UNS16 Servo_Err_OverVol_Operating_Mode = 0x0;		/* Mapped at index 0x5001, subindex 0x02 */
UNS16 Servo_Err_OverVol_Ena_Delay = 0x0;		/* Mapped at index 0x5001, subindex 0x03 */
UNS16 Servo_Err_OverVol_Res_Delay = 0x0;		/* Mapped at index 0x5001, subindex 0x04 */
UNS16 Servo_Err_OverVol_Res_Type = 0x1;		/* Mapped at index 0x5001, subindex 0x05 */
UNS16 Servo_Err_OverVol_Trigger_Conditions = 0x30C;		/* Mapped at index 0x5001, subindex 0x06 */
UNS16 Servo_Err_OverVol_Safer_Off = 0x1;		/* Mapped at index 0x5001, subindex 0x07 */
UNS16 Servo_Err_LowVol_On_Off = 0x1;		/* Mapped at index 0x5002, subindex 0x01 */
UNS16 Servo_Err_LowVol_Operating_Mode = 0x0;		/* Mapped at index 0x5002, subindex 0x02 */
UNS16 Servo_Err_LowVol_Ena_Delay = 0x0;		/* Mapped at index 0x5002, subindex 0x03 */
UNS16 Servo_Err_LowVol_Res_Delay = 0x0;		/* Mapped at index 0x5002, subindex 0x04 */
UNS16 Servo_Err_LowVol_Res_Type = 0x1;		/* Mapped at index 0x5002, subindex 0x05 */
UNS16 Servo_Err_LowVol_Trigger_Conditions = 0x96;		/* Mapped at index 0x5002, subindex 0x06 */
UNS16 Servo_Err_LowVol_Safer_Off = 0x1;		/* Mapped at index 0x5002, subindex 0x07 */
UNS16 Servo_Err_Outer24VLost_On_Off = 0x1;		/* Mapped at index 0x5003, subindex 0x01 */
UNS16 Servo_Err_Outer24VLost_Operating_Mode = 0x2;		/* Mapped at index 0x5003, subindex 0x02 */
UNS16 Servo_Err_Outer24VLost_Ena_Delay = 0x14;		/* Mapped at index 0x5003, subindex 0x03 */
UNS16 Servo_Err_Outer24VLost_Res_Delay = 0x0;		/* Mapped at index 0x5003, subindex 0x04 */
UNS16 Servo_Err_Outer24VLost_Res_Type = 0x1;		/* Mapped at index 0x5003, subindex 0x05 */
UNS16 Servo_Err_Outer24VLost_Trigger_Conditions = 0x3FFE;		/* Mapped at index 0x5003, subindex 0x06 */
UNS16 Servo_Err_Outer24VLost_Safer_Off = 0x1;		/* Mapped at index 0x5003, subindex 0x07 */
UNS16 Servo_Err_PosLost_On_Off = 0x1;		/* Mapped at index 0x5004, subindex 0x01 */
UNS16 Servo_Err_PosLost_Operating_Mode = 0x4;		/* Mapped at index 0x5004, subindex 0x02 */
UNS16 Servo_Err_PosLost_Ena_Delay = 0x0;		/* Mapped at index 0x5004, subindex 0x03 */
UNS16 Servo_Err_PosLost_Res_Delay = 0x0;		/* Mapped at index 0x5004, subindex 0x04 */
UNS16 Servo_Err_PosLost_Res_Type = 0x0;		/* Mapped at index 0x5004, subindex 0x05 */
UNS16 Servo_Err_PosLost_Trigger_Conditions = 0x0;		/* Mapped at index 0x5004, subindex 0x06 */
UNS16 Servo_Err_PosLost_Safer_Off = 0x1;		/* Mapped at index 0x5004, subindex 0x07 */
UNS16 Servo_Err_CANOpenLineOff_On_Off = 0x1;		/* Mapped at index 0x5005, subindex 0x01 */
UNS16 Servo_Err_CANOpenLineOff_Operating_Mode = 0x2;		/* Mapped at index 0x5005, subindex 0x02 */
UNS16 Servo_Err_CANOpenLineOff_Ena_Delay = 0x1F4;		/* Mapped at index 0x5005, subindex 0x03 */
UNS16 Servo_Err_CANOpenLineOff_Res_Delay = 0x0;		/* Mapped at index 0x5005, subindex 0x04 */
UNS16 Servo_Err_CANOpenLineOff_Res_Type = 0x0;		/* Mapped at index 0x5005, subindex 0x05 */
UNS16 Servo_Err_CANOpenLineOff_Trigger_Conditions = 0x0;		/* Mapped at index 0x5005, subindex 0x06 */
UNS16 Servo_Err_CANOpenLineOff_Safer_Off = 0x1;		/* Mapped at index 0x5005, subindex 0x07 */
UNS16 Servo_Err_IGBTOverTemper_On_Off = 0x1;		/* Mapped at index 0x5006, subindex 0x01 */
UNS16 Servo_Err_IGBTOverTemper_Operating_Mode = 0x0;		/* Mapped at index 0x5006, subindex 0x02 */
UNS16 Servo_Err_IGBTOverTemper_Ena_Delay = 0x1;		/* Mapped at index 0x5006, subindex 0x03 */
UNS16 Servo_Err_IGBTOverTemper_Res_Delay = 0x0;		/* Mapped at index 0x5006, subindex 0x04 */
UNS16 Servo_Err_IGBTOverTemper_Res_Type = 0x0;		/* Mapped at index 0x5006, subindex 0x05 */
UNS16 Servo_Err_IGBTOverTemper_Trigger_Conditions = 0x5A;		/* Mapped at index 0x5006, subindex 0x06 */
UNS16 Servo_Err_IGBTOverTemper_Safer_Off = 0x1;		/* Mapped at index 0x5006, subindex 0x07 */
UNS16 Servo_Err_SSILineOff_On_Off = 0x1;		/* Mapped at index 0x5007, subindex 0x01 */
UNS16 Servo_Err_SSILineOff_Operating_Mode = 0x2;		/* Mapped at index 0x5007, subindex 0x02 */
UNS16 Servo_Err_SSILineOff_Ena_Delay = 0x64;		/* Mapped at index 0x5007, subindex 0x03 */
UNS16 Servo_Err_SSILineOff_Res_Delay = 0x0;		/* Mapped at index 0x5007, subindex 0x04 */
UNS16 Servo_Err_SSILineOff_Res_Type = 0x0;		/* Mapped at index 0x5007, subindex 0x05 */
UNS16 Servo_Err_SSILineOff_Trigger_Conditions = 0x0;		/* Mapped at index 0x5007, subindex 0x06 */
UNS16 Servo_Err_SSILineOff_Safer_Off = 0x1;		/* Mapped at index 0x5007, subindex 0x07 */
UNS16 Servo_Err_MotoOverTemper_On_Off = 0x1;		/* Mapped at index 0x5008, subindex 0x01 */
UNS16 Servo_Err_MotoOverTemper_Operating_Mode = 0x3;		/* Mapped at index 0x5008, subindex 0x02 */
UNS16 Servo_Err_MotoOverTemper_Ena_Delay = 0x2;		/* Mapped at index 0x5008, subindex 0x03 */
UNS16 Servo_Err_MotoOverTemper_Res_Delay = 0x0;		/* Mapped at index 0x5008, subindex 0x04 */
UNS16 Servo_Err_MotoOverTemper_Res_Type = 0x0;		/* Mapped at index 0x5008, subindex 0x05 */
UNS16 Servo_Err_MotoOverTemper_Trigger_Conditions = 0x8C;		/* Mapped at index 0x5008, subindex 0x06 */
UNS16 Servo_Err_MotoOverTemper_Safer_Off = 0x1;		/* Mapped at index 0x5008, subindex 0x07 */
UNS16 Servo_Err_OverCurrent_On_Off = 0x1;		/* Mapped at index 0x5009, subindex 0x01 */
UNS16 Servo_Err_OverCurrent_Operating_Mode = 0x0;		/* Mapped at index 0x5009, subindex 0x02 */
UNS16 Servo_Err_OverCurrent_Ena_Delay = 0x0;		/* Mapped at index 0x5009, subindex 0x03 */
UNS16 Servo_Err_OverCurrent_Res_Delay = 0x0;		/* Mapped at index 0x5009, subindex 0x04 */
UNS16 Servo_Err_OverCurrent_Res_Type = 0x0;		/* Mapped at index 0x5009, subindex 0x05 */
UNS16 Servo_Err_OverCurrent_Trigger_Conditions = 0x0;		/* Mapped at index 0x5009, subindex 0x06 */
UNS16 Servo_Err_OverCurrent_Safer_Off = 0x1;		/* Mapped at index 0x5009, subindex 0x07 */
UNS16 Servo_Err_ShortCircuit_On_Off = 0x1;		/* Mapped at index 0x500A, subindex 0x01 */
UNS16 Servo_Err_ShortCircuit_Operating_Mode = 0x2;		/* Mapped at index 0x500A, subindex 0x02 */
UNS16 Servo_Err_ShortCircuit_Ena_Delay = 0x0;		/* Mapped at index 0x500A, subindex 0x03 */
UNS16 Servo_Err_ShortCircuit_Res_Delay = 0x0;		/* Mapped at index 0x500A, subindex 0x04 */
UNS16 Servo_Err_ShortCircuit_Res_Type = 0x0;		/* Mapped at index 0x500A, subindex 0x05 */
UNS16 Servo_Err_ShortCircuit_Trigger_Conditions = 0x0;		/* Mapped at index 0x500A, subindex 0x06 */
UNS16 Servo_Err_ShortCircuit_Safer_Off = 0x1;		/* Mapped at index 0x500A, subindex 0x07 */
UNS16 Servo_Err_CurrentOver_On_Off = 0x1;		/* Mapped at index 0x500B, subindex 0x01 */
UNS16 Servo_Err_CurrentOver_Operating_Mode = 0x3;		/* Mapped at index 0x500B, subindex 0x02 */
UNS16 Servo_Err_CurrentOver_Ena_Delay = 0x0;		/* Mapped at index 0x500B, subindex 0x03 */
UNS16 Servo_Err_CurrentOver_Res_Delay = 0x0;		/* Mapped at index 0x500B, subindex 0x04 */
UNS16 Servo_Err_CurrentOver_Res_Type = 0x1;		/* Mapped at index 0x500B, subindex 0x05 */
UNS16 Servo_Err_CurrentOver_Trigger_Conditions = 0x0;		/* Mapped at index 0x500B, subindex 0x06 */
UNS16 Servo_Err_CurrentOver_Safer_Off = 0x1;		/* Mapped at index 0x500B, subindex 0x07 */
UNS16 Servo_Err_Limit96_On_Off = 0x1;		/* Mapped at index 0x500C, subindex 0x01 */
UNS16 Servo_Err_Limit96_Operating_Mode = 0x0;		/* Mapped at index 0x500C, subindex 0x02 */
UNS16 Servo_Err_Limit96_Ena_Delay = 0x1E;		/* Mapped at index 0x500C, subindex 0x03 */
UNS16 Servo_Err_Limit96_Res_Delay = 0x0;		/* Mapped at index 0x500C, subindex 0x04 */
UNS16 Servo_Err_Limit96_Res_Type = 0x0;		/* Mapped at index 0x500C, subindex 0x05 */
UNS16 Servo_Err_Limit96_Trigger_Conditions = 0x4000;		/* Mapped at index 0x500C, subindex 0x06 */
UNS16 Servo_Err_Limit96_Safer_Off = 0x1;		/* Mapped at index 0x500C, subindex 0x07 */
UNS16 Servo_Err_380VErr_On_Off = 0x1;		/* Mapped at index 0x500D, subindex 0x01 */
UNS16 Servo_Err_380VErr_Operating_Mode = 0x2;		/* Mapped at index 0x500D, subindex 0x02 */
UNS16 Servo_Err_380VErr_Ena_Delay = 0xDAC;		/* Mapped at index 0x500D, subindex 0x03 */
UNS16 Servo_Err_380VErr_Res_Delay = 0x0;		/* Mapped at index 0x500D, subindex 0x04 */
UNS16 Servo_Err_380VErr_Res_Type = 0x0;		/* Mapped at index 0x500D, subindex 0x05 */
UNS16 Servo_Err_380VErr_Trigger_Conditions = 0x0;		/* Mapped at index 0x500D, subindex 0x06 */
UNS16 Servo_Err_380VErr_Safer_Off = 0x1;		/* Mapped at index 0x500D, subindex 0x07 */
UNS16 Servo_Err_VelocityOver_On_Off = 0x1;		/* Mapped at index 0x500E, subindex 0x01 */
UNS16 Servo_Err_VelocityOver_Operating_Mode = 0x3;		/* Mapped at index 0x500E, subindex 0x02 */
UNS16 Servo_Err_VelocityOver_Ena_Delay = 0x0;		/* Mapped at index 0x500E, subindex 0x03 */
UNS16 Servo_Err_VelocityOver_Res_Delay = 0x0;		/* Mapped at index 0x500E, subindex 0x04 */
UNS16 Servo_Err_VelocityOver_Res_Type = 0x1;		/* Mapped at index 0x500E, subindex 0x05 */
UNS16 Servo_Err_VelocityOver_Trigger_Conditions = 0x0;		/* Mapped at index 0x500E, subindex 0x06 */
UNS16 Servo_Err_VelocityOver_Safer_Off = 0x1;		/* Mapped at index 0x500E, subindex 0x07 */
UNS16 Servo_Err_DischargeFail_On_Off = 0x1;		/* Mapped at index 0x500F, subindex 0x01 */
UNS16 Servo_Err_DischargeFail_Operating_Mode = 0x0;		/* Mapped at index 0x500F, subindex 0x02 */
UNS16 Servo_Err_DischargeFail_Ena_Delay = 0x0;		/* Mapped at index 0x500F, subindex 0x03 */
UNS16 Servo_Err_DischargeFail_Res_Delay = 0x0;		/* Mapped at index 0x500F, subindex 0x04 */
UNS16 Servo_Err_DischargeFail_Res_Type = 0x1;		/* Mapped at index 0x500F, subindex 0x05 */
UNS16 Servo_Err_DischargeFail_Trigger_Conditions = 0x0;		/* Mapped at index 0x500F, subindex 0x06 */
UNS16 Servo_Err_DischargeFail_Safer_Off = 0x1;		/* Mapped at index 0x500F, subindex 0x07 */
UNS16 Servo_Err_BK24VLost_On_Off = 0x1;		/* Mapped at index 0x5010, subindex 0x01 */
UNS16 Servo_Err_BK24VLost_Operating_Mode = 0x2;		/* Mapped at index 0x5010, subindex 0x02 */
UNS16 Servo_Err_BK24VLost_Ena_Delay = 0x14;		/* Mapped at index 0x5010, subindex 0x03 */
UNS16 Servo_Err_BK24VLost_Res_Delay = 0x0;		/* Mapped at index 0x5010, subindex 0x04 */
UNS16 Servo_Err_BK24VLost_Res_Type = 0x0;		/* Mapped at index 0x5010, subindex 0x05 */
UNS16 Servo_Err_BK24VLost_Trigger_Conditions = 0x3FFE;		/* Mapped at index 0x5010, subindex 0x06 */
UNS16 Servo_Err_BK24VLost_Safer_Off = 0x1;		/* Mapped at index 0x5010, subindex 0x07 */
UNS16 Servo_Err_Iner24VLost_On_Off = 0x1;		/* Mapped at index 0x5011, subindex 0x01 */
UNS16 Servo_Err_Iner24VLost_Operating_Mode = 0x2;		/* Mapped at index 0x5011, subindex 0x02 */
UNS16 Servo_Err_Iner24VLost_Ena_Delay = 0x14;		/* Mapped at index 0x5011, subindex 0x03 */
UNS16 Servo_Err_Iner24VLost_Res_Delay = 0x0;		/* Mapped at index 0x5011, subindex 0x04 */
UNS16 Servo_Err_Iner24VLost_Res_Type = 0x1;		/* Mapped at index 0x5011, subindex 0x05 */
UNS16 Servo_Err_Iner24VLost_Trigger_Conditions = 0x3FFE;		/* Mapped at index 0x5011, subindex 0x06 */
UNS16 Servo_Err_Iner24VLost_Safer_Off = 0x1;		/* Mapped at index 0x5011, subindex 0x07 */
UNS16 Servo_Err_DITrigErr_On_Off = 0x0;		/* Mapped at index 0x5012, subindex 0x01 */
UNS16 Servo_Err_DITrigErr_Operating_Mode = 0x2;		/* Mapped at index 0x5012, subindex 0x02 */
UNS16 Servo_Err_DITrigErr_Ena_Delay = 0x3E8;		/* Mapped at index 0x5012, subindex 0x03 */
UNS16 Servo_Err_DITrigErr_Res_Delay = 0x0;		/* Mapped at index 0x5012, subindex 0x04 */
UNS16 Servo_Err_DITrigErr_Res_Type = 0x0;		/* Mapped at index 0x5012, subindex 0x05 */
UNS16 Servo_Err_DITrigErr_Trigger_Conditions = 0x3FFE;		/* Mapped at index 0x5012, subindex 0x06 */
UNS16 Servo_Err_DITrigErr_Safer_Off = 0x0;		/* Mapped at index 0x5012, subindex 0x07 */
UNS16 Servo_Err_SaferErr_On_Off = 0x1;		/* Mapped at index 0x5013, subindex 0x01 */
UNS16 Servo_Err_SaferErr_Operating_Mode = 0x2;		/* Mapped at index 0x5013, subindex 0x02 */
UNS16 Servo_Err_SaferErr_Ena_Delay = 0x1F4;		/* Mapped at index 0x5013, subindex 0x03 */
UNS16 Servo_Err_SaferErr_Res_Delay = 0x0;		/* Mapped at index 0x5013, subindex 0x04 */
UNS16 Servo_Err_SaferErr_Res_Type = 0x0;		/* Mapped at index 0x5013, subindex 0x05 */
UNS16 Servo_Err_SaferErr_Trigger_Conditions = 0x4000;		/* Mapped at index 0x5013, subindex 0x06 */
UNS16 Servo_Err_SaferErr_Safer_Off = 0x0;		/* Mapped at index 0x5013, subindex 0x07 */
UNS16 Servo_Err_PositionOver_On_Off = 0x1;		/* Mapped at index 0x5014, subindex 0x01 */
UNS16 Servo_Err_PositionOver_Operating_Mode = 0x2;		/* Mapped at index 0x5014, subindex 0x02 */
UNS16 Servo_Err_PositionOver_Ena_Delay = 0x0;		/* Mapped at index 0x5014, subindex 0x03 */
UNS16 Servo_Err_PositionOver_Res_Delay = 0x0;		/* Mapped at index 0x5014, subindex 0x04 */
UNS16 Servo_Err_PositionOver_Res_Type = 0x0;		/* Mapped at index 0x5014, subindex 0x05 */
UNS16 Servo_Err_PositionOver_Trigger_Conditions = 0x64;		/* Mapped at index 0x5014, subindex 0x06 */
UNS16 Servo_Err_PositionOver_Safer_Off = 0x1;		/* Mapped at index 0x5014, subindex 0x07 */
UNS16 Servo_Err_PaddleOver_On_Off = 0x1;		/* Mapped at index 0x5015, subindex 0x01 */
UNS16 Servo_Err_PaddleOver_Operating_Mode = 0x0;		/* Mapped at index 0x5015, subindex 0x02 */
UNS16 Servo_Err_PaddleOver_Ena_Delay = 0x1F4;		/* Mapped at index 0x5015, subindex 0x03 */
UNS16 Servo_Err_PaddleOver_Res_Delay = 0x0;		/* Mapped at index 0x5015, subindex 0x04 */
UNS16 Servo_Err_PaddleOver_Res_Type = 0x0;		/* Mapped at index 0x5015, subindex 0x05 */
UNS16 Servo_Err_PaddleOver_Trigger_Conditions = 0xCA9;		/* Mapped at index 0x5015, subindex 0x06 */
UNS16 Servo_Err_PaddleOver_Safer_Off = 0x0;		/* Mapped at index 0x5015, subindex 0x07 */
UNS16 Servo_Err_IGBTLineOff_On_Off = 0x1;		/* Mapped at index 0x5016, subindex 0x01 */
UNS16 Servo_Err_IGBTLineOff_Operating_Mode = 0x2;		/* Mapped at index 0x5016, subindex 0x02 */
UNS16 Servo_Err_IGBTLineOff_Ena_Delay = 0xC8;		/* Mapped at index 0x5016, subindex 0x03 */
UNS16 Servo_Err_IGBTLineOff_Res_Delay = 0x0;		/* Mapped at index 0x5016, subindex 0x04 */
UNS16 Servo_Err_IGBTLineOff_Res_Type = 0x0;		/* Mapped at index 0x5016, subindex 0x05 */
UNS16 Servo_Err_IGBTLineOff_Trigger_Conditions = 0x15E;		/* Mapped at index 0x5016, subindex 0x06 */
UNS16 Servo_Err_IGBTLineOff_Safer_Off = 0x1;		/* Mapped at index 0x5016, subindex 0x07 */
UNS16 Servo_Err_MotoLineOff_On_Off = 0x1;		/* Mapped at index 0x5017, subindex 0x01 */
UNS16 Servo_Err_MotoLineOff_Operating_Mode = 0x2;		/* Mapped at index 0x5017, subindex 0x02 */
UNS16 Servo_Err_MotoLineOff_Ena_Delay = 0xC8;		/* Mapped at index 0x5017, subindex 0x03 */
UNS16 Servo_Err_MotoLineOff_Res_Delay = 0x0;		/* Mapped at index 0x5017, subindex 0x04 */
UNS16 Servo_Err_MotoLineOff_Res_Type = 0x0;		/* Mapped at index 0x5017, subindex 0x05 */
UNS16 Servo_Err_MotoLineOff_Trigger_Conditions = 0x15E;		/* Mapped at index 0x5017, subindex 0x06 */
UNS16 Servo_Err_MotoLineOff_Safer_Off = 0x1;		/* Mapped at index 0x5017, subindex 0x07 */
UNS16 Servo_Err_ChargerComErr_On_Off = 0x1;		/* Mapped at index 0x5018, subindex 0x01 */
UNS16 Servo_Err_ChargerComErr_Operating_Mode = 0x2;		/* Mapped at index 0x5018, subindex 0x02 */
UNS16 Servo_Err_ChargerComErr_Ena_Delay = 0xC8;		/* Mapped at index 0x5018, subindex 0x03 */
UNS16 Servo_Err_ChargerComErr_Res_Delay = 0x0;		/* Mapped at index 0x5018, subindex 0x04 */
UNS16 Servo_Err_ChargerComErr_Res_Type = 0x0;		/* Mapped at index 0x5018, subindex 0x05 */
UNS16 Servo_Err_ChargerComErr_Trigger_Conditions = 0x0;		/* Mapped at index 0x5018, subindex 0x06 */
UNS16 Servo_Err_ChargerComErr_Safer_Off = 0x0;		/* Mapped at index 0x5018, subindex 0x07 */
UNS16 Servo_Err_NULL1_On_Off = 0x1;		/* Mapped at index 0x5019, subindex 0x01 */
UNS16 Servo_Err_NULL1_Operating_Mode = 0x0;		/* Mapped at index 0x5019, subindex 0x02 */
UNS16 Servo_Err_NULL1_Ena_Delay = 0x0;		/* Mapped at index 0x5019, subindex 0x03 */
UNS16 Servo_Err_NULL1_Res_Delay = 0x0;		/* Mapped at index 0x5019, subindex 0x04 */
UNS16 Servo_Err_NULL1_Res_Type = 0x1;		/* Mapped at index 0x5019, subindex 0x05 */
UNS16 Servo_Err_NULL1_Trigger_Conditions = 0x0;		/* Mapped at index 0x5019, subindex 0x06 */
UNS16 Servo_Err_NULL1_Safer_Off = 0x0;		/* Mapped at index 0x5019, subindex 0x07 */
UNS16 Servo_Err_NULL2_On_Off = 0x1;		/* Mapped at index 0x501A, subindex 0x01 */
UNS16 Servo_Err_NULL2_Operating_Mode = 0x0;		/* Mapped at index 0x501A, subindex 0x02 */
UNS16 Servo_Err_NULL2_Ena_Delay = 0x0;		/* Mapped at index 0x501A, subindex 0x03 */
UNS16 Servo_Err_NULL2_Res_Delay = 0x0;		/* Mapped at index 0x501A, subindex 0x04 */
UNS16 Servo_Err_NULL2_Res_Type = 0x1;		/* Mapped at index 0x501A, subindex 0x05 */
UNS16 Servo_Err_NULL2_Trigger_Conditions = 0x0;		/* Mapped at index 0x501A, subindex 0x06 */
UNS16 Servo_Err_NULL2_Safer_Off = 0x0;		/* Mapped at index 0x501A, subindex 0x07 */
UNS16 Servo_Err_NULL3_On_Off = 0x1;		/* Mapped at index 0x501B, subindex 0x01 */
UNS16 Servo_Err_NULL3_Operating_Mode = 0x0;		/* Mapped at index 0x501B, subindex 0x02 */
UNS16 Servo_Err_NULL3_Ena_Delay = 0x0;		/* Mapped at index 0x501B, subindex 0x03 */
UNS16 Servo_Err_NULL3_Res_Delay = 0x0;		/* Mapped at index 0x501B, subindex 0x04 */
UNS16 Servo_Err_NULL3_Res_Type = 0x1;		/* Mapped at index 0x501B, subindex 0x05 */
UNS16 Servo_Err_NULL3_Trigger_Conditions = 0x0;		/* Mapped at index 0x501B, subindex 0x06 */
UNS16 Servo_Err_NULL3_Safer_Off = 0x0;		/* Mapped at index 0x501B, subindex 0x07 */
UNS16 Servo_Err_NULL4_On_Off = 0x1;		/* Mapped at index 0x501C, subindex 0x01 */
UNS16 Servo_Err_NULL4_Operating_Mode = 0x0;		/* Mapped at index 0x501C, subindex 0x02 */
UNS16 Servo_Err_NULL4_Ena_Delay = 0x0;		/* Mapped at index 0x501C, subindex 0x03 */
UNS16 Servo_Err_NULL4_Res_Delay = 0x0;		/* Mapped at index 0x501C, subindex 0x04 */
UNS16 Servo_Err_NULL4_Res_Type = 0x1;		/* Mapped at index 0x501C, subindex 0x05 */
UNS16 Servo_Err_NULL4_Trigger_Conditions = 0x0;		/* Mapped at index 0x501C, subindex 0x06 */
UNS16 Servo_Err_NULL4_Safer_Off = 0x0;		/* Mapped at index 0x501C, subindex 0x07 */
UNS16 Servo_Err_NULL5_On_Off = 0x1;		/* Mapped at index 0x501D, subindex 0x01 */
UNS16 Servo_Err_NULL5_Operating_Mode = 0x0;		/* Mapped at index 0x501D, subindex 0x02 */
UNS16 Servo_Err_NULL5_Ena_Delay = 0x0;		/* Mapped at index 0x501D, subindex 0x03 */
UNS16 Servo_Err_NULL5_Res_Delay = 0x0;		/* Mapped at index 0x501D, subindex 0x04 */
UNS16 Servo_Err_NULL5_Res_Type = 0x1;		/* Mapped at index 0x501D, subindex 0x05 */
UNS16 Servo_Err_NULL5_Trigger_Conditions = 0x0;		/* Mapped at index 0x501D, subindex 0x06 */
UNS16 Servo_Err_NULL5_Safer_Off = 0x0;		/* Mapped at index 0x501D, subindex 0x07 */
UNS16 Servo_Err_NULL6_On_Off = 0x1;		/* Mapped at index 0x501E, subindex 0x01 */
UNS16 Servo_Err_NULL6_Operating_Mode = 0x0;		/* Mapped at index 0x501E, subindex 0x02 */
UNS16 Servo_Err_NULL6_Ena_Delay = 0x0;		/* Mapped at index 0x501E, subindex 0x03 */
UNS16 Servo_Err_NULL6_Res_Delay = 0x0;		/* Mapped at index 0x501E, subindex 0x04 */
UNS16 Servo_Err_NULL6_Res_Type = 0x1;		/* Mapped at index 0x501E, subindex 0x05 */
UNS16 Servo_Err_NULL6_Trigger_Conditions = 0x0;		/* Mapped at index 0x501E, subindex 0x06 */
UNS16 Servo_Err_NULL6_Safer_Off = 0x0;		/* Mapped at index 0x501E, subindex 0x07 */
UNS16 Servo_Err_NULL7_On_Off = 0x1;		/* Mapped at index 0x501F, subindex 0x01 */
UNS16 Servo_Err_NULL7_Operating_Mode = 0x0;		/* Mapped at index 0x501F, subindex 0x02 */
UNS16 Servo_Err_NULL7_Ena_Delay = 0x0;		/* Mapped at index 0x501F, subindex 0x03 */
UNS16 Servo_Err_NULL7_Res_Delay = 0x0;		/* Mapped at index 0x501F, subindex 0x04 */
UNS16 Servo_Err_NULL7_Res_Type = 0x1;		/* Mapped at index 0x501F, subindex 0x05 */
UNS16 Servo_Err_NULL7_Trigger_Conditions = 0x0;		/* Mapped at index 0x501F, subindex 0x06 */
UNS16 Servo_Err_NULL7_Safer_Off = 0x0;		/* Mapped at index 0x501F, subindex 0x07 */
UNS16 Charger_Err_Total_On_Off = 0x1;		/* Mapped at index 0x5020, subindex 0x01 */
UNS16 Charger_Err_Total_Operating_Mode = 0x3;		/* Mapped at index 0x5020, subindex 0x02 */
UNS16 Charger_Err_Total_Ena_Delay = 0x3E8;		/* Mapped at index 0x5020, subindex 0x03 */
UNS16 Charger_Err_Total_Res_Delay = 0x3E8;		/* Mapped at index 0x5020, subindex 0x04 */
UNS16 Charger_Err_Total_Res_Type = 0x1;		/* Mapped at index 0x5020, subindex 0x05 */
UNS16 Charger_Err_Total_Trigger_Conditions = 0x0;		/* Mapped at index 0x5020, subindex 0x06 */
UNS16 Charger_Err_Total_Safer_Off = 0x0;		/* Mapped at index 0x5020, subindex 0x07 */
UNS16 Charger_Err_SoftOverVol_On_Off = 0x1;		/* Mapped at index 0x5021, subindex 0x01 */
UNS16 Charger_Err_SoftOverVol_Operating_Mode = 0x3;		/* Mapped at index 0x5021, subindex 0x02 */
UNS16 Charger_Err_SoftOverVol_Ena_Delay = 0x0;		/* Mapped at index 0x5021, subindex 0x03 */
UNS16 Charger_Err_SoftOverVol_Res_Delay = 0x0;		/* Mapped at index 0x5021, subindex 0x04 */
UNS16 Charger_Err_SoftOverVol_Res_Type = 0x1;		/* Mapped at index 0x5021, subindex 0x05 */
UNS16 Charger_Err_SoftOverVol_Trigger_Conditions = 0x1CC;		/* Mapped at index 0x5021, subindex 0x06 */
UNS16 Charger_Err_SoftOverVol_Safer_Off = 0x0;		/* Mapped at index 0x5021, subindex 0x07 */
UNS16 Charger_Err_SoftOverCurrent_On_Off = 0x1;		/* Mapped at index 0x5022, subindex 0x01 */
UNS16 Charger_Err_SoftOverCurrent_Operating_Mode = 0x3;		/* Mapped at index 0x5022, subindex 0x02 */
UNS16 Charger_Err_SoftOverCurrent_Ena_Delay = 0x64;		/* Mapped at index 0x5022, subindex 0x03 */
UNS16 Charger_Err_SoftOverCurrent_Res_Delay = 0x0;		/* Mapped at index 0x5022, subindex 0x04 */
UNS16 Charger_Err_SoftOverCurrent_Res_Type = 0x1;		/* Mapped at index 0x5022, subindex 0x05 */
UNS16 Charger_Err_SoftOverCurrent_Trigger_Conditions = 0x15E;		/* Mapped at index 0x5022, subindex 0x06 */
UNS16 Charger_Err_SoftOverCurrent_Safer_Off = 0x0;		/* Mapped at index 0x5022, subindex 0x07 */
UNS16 Charger_Err_ShortCircuit_On_Off = 0x1;		/* Mapped at index 0x5023, subindex 0x01 */
UNS16 Charger_Err_ShortCircuit_Operating_Mode = 0x3;		/* Mapped at index 0x5023, subindex 0x02 */
UNS16 Charger_Err_ShortCircuit_Ena_Delay = 0x12C;		/* Mapped at index 0x5023, subindex 0x03 */
UNS16 Charger_Err_ShortCircuit_Res_Delay = 0x0;		/* Mapped at index 0x5023, subindex 0x04 */
UNS16 Charger_Err_ShortCircuit_Res_Type = 0x1;		/* Mapped at index 0x5023, subindex 0x05 */
UNS16 Charger_Err_ShortCircuit_Trigger_Conditions = 0x32;		/* Mapped at index 0x5023, subindex 0x06 */
UNS16 Charger_Err_ShortCircuit_Safer_Off = 0x0;		/* Mapped at index 0x5023, subindex 0x07 */
UNS16 Charger_Err_BrokenCircuit_On_Off = 0x1;		/* Mapped at index 0x5024, subindex 0x01 */
UNS16 Charger_Err_BrokenCircuit_Operating_Mode = 0x3;		/* Mapped at index 0x5024, subindex 0x02 */
UNS16 Charger_Err_BrokenCircuit_Ena_Delay = 0x12C;		/* Mapped at index 0x5024, subindex 0x03 */
UNS16 Charger_Err_BrokenCircuit_Res_Delay = 0x0;		/* Mapped at index 0x5024, subindex 0x04 */
UNS16 Charger_Err_BrokenCircuit_Res_Type = 0x1;		/* Mapped at index 0x5024, subindex 0x05 */
UNS16 Charger_Err_BrokenCircuit_Trigger_Conditions = 0x14;		/* Mapped at index 0x5024, subindex 0x06 */
UNS16 Charger_Err_BrokenCircuit_Safer_Off = 0x0;		/* Mapped at index 0x5024, subindex 0x07 */
UNS16 Charger_Err_DYErr_On_Off = 0x1;		/* Mapped at index 0x5025, subindex 0x01 */
UNS16 Charger_Err_DYErr_Operating_Mode = 0x3;		/* Mapped at index 0x5025, subindex 0x02 */
UNS16 Charger_Err_DYErr_Ena_Delay = 0x0;		/* Mapped at index 0x5025, subindex 0x03 */
UNS16 Charger_Err_DYErr_Res_Delay = 0x0;		/* Mapped at index 0x5025, subindex 0x04 */
UNS16 Charger_Err_DYErr_Res_Type = 0x1;		/* Mapped at index 0x5025, subindex 0x05 */
UNS16 Charger_Err_DYErr_Trigger_Conditions = 0x3FFE;		/* Mapped at index 0x5025, subindex 0x06 */
UNS16 Charger_Err_DYErr_Safer_Off = 0x0;		/* Mapped at index 0x5025, subindex 0x07 */
UNS16 Charger_Err_LowVol_On_Off = 0x1;		/* Mapped at index 0x5026, subindex 0x01 */
UNS16 Charger_Err_LowVol_Operating_Mode = 0x3;		/* Mapped at index 0x5026, subindex 0x02 */
UNS16 Charger_Err_LowVol_Ena_Delay = 0xC8;		/* Mapped at index 0x5026, subindex 0x03 */
UNS16 Charger_Err_LowVol_Res_Delay = 0x0;		/* Mapped at index 0x5026, subindex 0x04 */
UNS16 Charger_Err_LowVol_Res_Type = 0x1;		/* Mapped at index 0x5026, subindex 0x05 */
UNS16 Charger_Err_LowVol_Trigger_Conditions = 0x1CC;		/* Mapped at index 0x5026, subindex 0x06 */
UNS16 Charger_Err_LowVol_Safer_Off = 0x0;		/* Mapped at index 0x5026, subindex 0x07 */
UNS16 Charger_Err_IGBTOverTemper_On_Off = 0x1;		/* Mapped at index 0x5027, subindex 0x01 */
UNS16 Charger_Err_IGBTOverTemper_Operating_Mode = 0x3;		/* Mapped at index 0x5027, subindex 0x02 */
UNS16 Charger_Err_IGBTOverTemper_Ena_Delay = 0x1;		/* Mapped at index 0x5027, subindex 0x03 */
UNS16 Charger_Err_IGBTOverTemper_Res_Delay = 0x0;		/* Mapped at index 0x5027, subindex 0x04 */
UNS16 Charger_Err_IGBTOverTemper_Res_Type = 0x0;		/* Mapped at index 0x5027, subindex 0x05 */
UNS16 Charger_Err_IGBTOverTemper_Trigger_Conditions = 0x5A;		/* Mapped at index 0x5027, subindex 0x06 */
UNS16 Charger_Err_IGBTOverTemper_Safer_Off = 0x0;		/* Mapped at index 0x5027, subindex 0x07 */
UNS16 Charger_Err_HardOverVol_On_Off = 0x1;		/* Mapped at index 0x5028, subindex 0x01 */
UNS16 Charger_Err_HardOverVol_Operating_Mode = 0x3;		/* Mapped at index 0x5028, subindex 0x02 */
UNS16 Charger_Err_HardOverVol_Ena_Delay = 0x0;		/* Mapped at index 0x5028, subindex 0x03 */
UNS16 Charger_Err_HardOverVol_Res_Delay = 0x0;		/* Mapped at index 0x5028, subindex 0x04 */
UNS16 Charger_Err_HardOverVol_Res_Type = 0x1;		/* Mapped at index 0x5028, subindex 0x05 */
UNS16 Charger_Err_HardOverVol_Trigger_Conditions = 0x1D6;		/* Mapped at index 0x5028, subindex 0x06 */
UNS16 Charger_Err_HardOverVol_Safer_Off = 0x0;		/* Mapped at index 0x5028, subindex 0x07 */
UNS16 Charger_Err_HardOverCurrent_On_Off = 0x1;		/* Mapped at index 0x5029, subindex 0x01 */
UNS16 Charger_Err_HardOverCurrent_Operating_Mode = 0x3;		/* Mapped at index 0x5029, subindex 0x02 */
UNS16 Charger_Err_HardOverCurrent_Ena_Delay = 0x64;		/* Mapped at index 0x5029, subindex 0x03 */
UNS16 Charger_Err_HardOverCurrent_Res_Delay = 0x0;		/* Mapped at index 0x5029, subindex 0x04 */
UNS16 Charger_Err_HardOverCurrent_Res_Type = 0x1;		/* Mapped at index 0x5029, subindex 0x05 */
UNS16 Charger_Err_HardOverCurrent_Trigger_Conditions = 0x5;		/* Mapped at index 0x5029, subindex 0x06 */
UNS16 Charger_Err_HardOverCurrent_Safer_Off = 0x0;		/* Mapped at index 0x5029, subindex 0x07 */
UNS16 Charger_Err_HardOverCurrent1_On_Off = 0x1;		/* Mapped at index 0x5030, subindex 0x01 */
UNS16 Charger_Err_HardOverCurrent1_Operating_Mode = 0x3;		/* Mapped at index 0x5030, subindex 0x02 */
UNS16 Charger_Err_HardOverCurrent1_Ena_Delay = 0x0;		/* Mapped at index 0x5030, subindex 0x03 */
UNS16 Charger_Err_HardOverCurrent1_Res_Delay = 0x0;		/* Mapped at index 0x5030, subindex 0x04 */
UNS16 Charger_Err_HardOverCurrent1_Res_Type = 0x0;		/* Mapped at index 0x5030, subindex 0x05 */
UNS16 Charger_Err_HardOverCurrent1_Trigger_Conditions = 0xA;		/* Mapped at index 0x5030, subindex 0x06 */
UNS16 Charger_Err_HardOverCurrent1_Safer_Off = 0x1;		/* Mapped at index 0x5030, subindex 0x07 */
UNS16 Charger_Err_OverVol_On_Off = 0x1;		/* Mapped at index 0x5031, subindex 0x01 */
UNS16 Charger_Err_OverVol_Operating_Mode = 0x3;		/* Mapped at index 0x5031, subindex 0x02 */
UNS16 Charger_Err_OverVol_Ena_Delay = 0x64;		/* Mapped at index 0x5031, subindex 0x03 */
UNS16 Charger_Err_OverVol_Res_Delay = 0x0;		/* Mapped at index 0x5031, subindex 0x04 */
UNS16 Charger_Err_OverVol_Res_Type = 0x0;		/* Mapped at index 0x5031, subindex 0x05 */
UNS16 Charger_Err_OverVol_Trigger_Conditions = 0x30C;		/* Mapped at index 0x5031, subindex 0x06 */
UNS16 Charger_Err_OverVol_Safer_Off = 0x0;		/* Mapped at index 0x5031, subindex 0x07 */

UNS32 Error_code1 = 0x0;		/* Mapped at index 0x603F, subindex 0x00 */
UNS16 vl_ramp_function_time = 0x0;		/* Mapped at index 0x604F, subindex 0x00 */
UNS16 vl_slow_down_time = 0x0;		/* Mapped at index 0x6050, subindex 0x00 */
INTEGER16 Current_actual_value = 0x0;		/* Mapped at index 0x6078, subindex 0x00 */
UNS16 DC_link_circuit_voltage = 0x0;		/* Mapped at index 0x6079, subindex 0x00 */
UNS16 Max_motor_speed = 0xBB8;		/* Mapped at index 0x6080, subindex 0x00 */
UNS16 Profile_velocity = 0x352;		/* Mapped at index 0x6081, subindex 0x00 */
UNS16 Profile_acceleration = 0xF;		/* Mapped at index 0x6083, subindex 0x00 */
UNS16 Profile_deceleration = 0xF;		/* Mapped at index 0x6084, subindex 0x00 */
INTEGER16 Torque_profile_type = 0x0;		/* Mapped at index 0x6088, subindex 0x00 */
INTEGER8 Acceleration_notation_index = 0x0;		/* Mapped at index 0x608D, subindex 0x00 */
UNS8 Acceleration_dimension_index = 0x0;		/* Mapped at index 0x608E, subindex 0x00 */
UNS16 Position_encoder_resolution_Encoder_increments_a = 0xB;		/* Mapped at index 0x608F, subindex 0x01 */
UNS16 Position_encoder_resolution_Encoder_increments_b = 0xA3;		/* Mapped at index 0x608F, subindex 0x02 */
UNS16 Gear_ratio_Motor_revolutions_a = 0x1;		/* Mapped at index 0x6091, subindex 0x01 */
UNS16 Gear_ratio_Motor_revolutions_b = 0x6F7;		/* Mapped at index 0x6091, subindex 0x02 */
UNS16 Homing_speeds = 0x5DC;		/* Mapped at index 0x6099, subindex 0x00 */
UNS16 Velocity_control_parameter_set_Gain = 0x9C4;		/* Mapped at index 0x60F9, subindex 0x01 */
UNS8 Velocity_control_parameter_null2 = 0x0;		/* Mapped at index 0x60F9, subindex 0x02 */
UNS16 Velocity_control_parameter_null3 = 0x0;		/* Mapped at index 0x60F9, subindex 0x03 */
INTEGER16 Velocity_control_parameter_null4 = 0x0;		/* Mapped at index 0x60F9, subindex 0x04 */
UNS16 Velocity_control_parameter_v_fiterNum = 0x1;		/* Mapped at index 0x60F9, subindex 0x05 */
UNS16 Position_control_parameter_set_manufacturer_specific1 = 0x1388;		/* Mapped at index 0x60FB, subindex 0x01 */
UNS16 Position_control_parameter_set_manufacturer_specific2 = 0x2BC;		/* Mapped at index 0x60FB, subindex 0x02 */
UNS16 Position_control_parameter_set_manufacturer_specific3 = 0x0;		/* Mapped at index 0x60FB, subindex 0x03 */
UNS16 Position_control_parameter_set_manufacturer_specific4 = 0x0;		/* Mapped at index 0x60FB, subindex 0x04 */
UNS16 Position_control_parameter_set_manufacturer_specific5 = 0x28;		/* Mapped at index 0x60FB, subindex 0x05 */
UNS16 Motor_type = 0x3;		/* Mapped at index 0x6402, subindex 0x00 */

//MPC项目的eds文件已无以下变量如t_set的定义，所以CANOpen通讯参数无法与星辰参数联动，以下数组目前为预留用
CANopen_EXT CANopenParaType CANopenParaReadTable[] =
{
    {T_SET,          				(u16*)(&t_set)},          											/* 转矩设定 */
    {IGBT_TEMP,      				(u16*)(&igbt_temp)},      											/* IGBT温度 */
    {ANALOG_IN0, 					(u16*)(&User_C_Vol)}, 												/* 超级电容电压 */
    {OPEN_CLOSE_SPEED, 				(u16*)(&open_close_speed)}, 										/* 开环收桨速度 */
    {OVER_I_C, 						(u16*)(&RatedCurrent)}, 											/* 额定电流 */
    {TECCNT,						(u16*)(&Over_I_Total)}, 											/* 超载量 */
    {BOFFCNT, 						(u16*)(&SpeedOverproofT)}, 											/* 速度超差时间 */
    {CHARG_DEFECT_CODE, 			(u16*)(&charg_defect_code)},
    {CHARG_SYS_ST_CODE, 			(u16*)(&charg_sys_st_code)},
    {CHARG_IGBT_TEMP,   			(u16*)(&charg_igbt_temp)},
    {CHARG_VOL,         			(u16*)(&charg_vol)},
    {CHARG_V_FEEDBACK,  			(u16*)(&VoltageOfUser)},
    {CHARG_I_FEEDBACK,  			(u16*)(&charg_i_feedback)},
};

CANopen_EXT CANopenParaType  CANopenParaWriteTable[] =
{
    {MAX_V,         			 	(u16*)(&Max_motor_speed)},          								/* 最高转速 */
    {MAX_I,          				(u16*)(&max_i)},          											/* 最大电流 */
    {MAX_T,          				(u16*)(&max_t)},          											/* 最大扭矩*/
    {MOTO_SAFE_TEMP, 				(u16*)(&moto_safe_temp)}, 											/* 电机过热温度保护点 */
//  {WORK_MOD,       				(u16*)(&work_mod)},       											/* 工作模式选择 */
    {V_UP_TIME,      				(u16*)(&vl_ramp_function_time)},    								/* 速度环上升梯度延时 */
    {V_DOWN_TIME,   		 		(u16*)(&vl_slow_down_time)},    									/* 速度环下降梯度延时 */
    {V_DIR_MOD,      				(u16*)(&v_dir_mod)},      											/* 速度环电机方向选择 */
    {DEFECTINSEL,      				(u16*)(&DI_Error_OutSide_Choose)},
    {MOTO_HUMID_EN,     			(u16*)(&IO_Control)},       										/* 温度/湿度选择 */
    {SERVICE_TIME_DELAY, 			(u16*)(&Service_Time_Delay)},
    {S_UP_TIME,    			 		(u16*)(&s_up_time)},       											/* S曲线上升时间 */
    {S_DOWN_TIME,   				(u16*)(&s_down_time)},     											/* S曲线下降时间*/
    {P_PP,							(u16*)(&Position_control_parameter_set_manufacturer_specific1)},	/* 位置环比例系数 */
    {P_PI,							(u16*)(&Position_control_parameter_set_manufacturer_specific2)},	/* 位置环前馈系数 */
    {P_ERR_TH,						(u16*)(&Velocity_control_parameter_v_fiterNum)},					/* 速度设定滤波系数 */
    {P_FILT_A,						(u16*)(&Position_control_parameter_set_manufacturer_specific5)},	/* 位置环设定滤波 */
  //{P_PV,							(u16*)(&Velocity_Limit_Out)},										/* 位置环速度限制值 */
    {P_PACC,						(u16*)(&Profile_acceleration)},										/* 位置环加速度 */
    {P_PDEC,						(u16*)(&Profile_deceleration)},										/* 位置环减速度 */
    {V_PP, 							(u16*)(&Velocity_control_parameter_set_Gain)},						/* 速度环比例系数*/
    {V_PI,							(u16*)(&v_pi)},														/* 速度环积分系数*/
    {SAFE_CLOSE_UP_TIME,     		(u16*)(&safe_close_up_time)},       								/* 安全收桨上升时间 */
    {SAFE_CLOSE_DOWN_TIME,   		(u16*)(&safe_close_down_time)},     								/* 安全收桨下降时间*/
    {SAFE_CLOSE_SPEED,      		(u16*)(&Homing_speeds)},     										/* 安全收桨速度*/
    {LIMIT_T,  						(u16*)(&limit_t)},     												/* 转矩限制值 */
    {LIMIT_V,   					(u16*)(&limit_v)},     												/* 位置环的速度限制值 */
    {MOD_CTRL,   					(u16*)(&ModeCtrl)},    											 	/* 工作模式控制字 */
    {ERR_RST,  					 	(u16*)(&err_rst_s16)},     											/* 1故障更新 */
    {SCALE_A, 						(u16*)(&Gear_ratio_Motor_revolutions_a)}, 							/* 位置设定传递比分子 */
    {SCALE_B, 						(u16*)(&Gear_ratio_Motor_revolutions_b)}, 							/* 位置设定传递比分母 */
    {SEC_SCALE_A, 					(u16*)(&Position_encoder_resolution_Encoder_increments_a)}, 		/* 绝对位置反馈传递比分子 */
    {SEC_SCALE_B, 					(u16*)(&Position_encoder_resolution_Encoder_increments_b)}, 		/* 绝对位置反馈传递比分母 */
    {SEC_POS_NUM, 					(u16*)(&sec_pos_num)}, 												/* 第二编码器圈数 */
    {SEC_POS,     					(u16*)(&sec_pos)},     												/* 第二编码器位置 */
    {MOTOR_POS_RST, 				(u16*)(&motor_pos_rst)}, 											/* 电机位置复位 */
    {SEC_CODER_DIR, 				(u16*)(&SecCoderDir)}, 												/* 第二编码器计数方向 */
    {BRAKE_ON_DELAY, 				(u16*)(&BKOnDelay)}, 												/* 制动器开延时 */
    {BRAKE_OFF_DELAY, 				(u16*)(&BKOffDelay)}, 												/* 制动器关延时 */
    {BK_SWITCH, 					(u16*)(&BK_Switch)},												/* 制动器开关 */
    {SAFE_DI_DLY, 					(u16*)(&SafeDI_Dly)}, 												/* 安全链输入断开延时，低电平为断开情况 */
    //{MOTO_TYPE, 					(u16*)(&Motor_type)},												/* 电机型号 */
    {USER_PARA1_H, 					(u16*)(&user_para1_h)}, 											/* 用户保存参数1 */
    {USER_PARA1_L, 					(u16*)(&user_para1_l)}, 											/* 用户保存参数1 */
    {USER_PARA2_H, 					(u16*)(&user_para2_h)},											 	/* 用户保存参数2 */
    {USER_PARA2_L, 					(u16*)(&user_para2_l)}, 											/* 用户保存参数2 */
    {USER_PARA3_H, 					(u16*)(&user_para3_h)}, 											/* 用户保存参数3 */
    {USER_PARA3_L, 					(u16*)(&user_para3_l)}, 											/* 用户保存参数3 */
    {CHARG_V_SET,    				(u16*)(&charg_v_set)},
    {CHARG_I_SET,    				(u16*)(&charg_i_set)},
    {CAN_CURRE_FIR, 				(u16*)(&FltPraOfCurrent)},
    {CAN_SPEED_FIR, 				(u16*)(&FltPraOfVelocity)},
    {CAN_MXVOL_FIR, 				(u16*)(&FltPraOfDCVol)},
    {USERPARA5,      				(u16*)(&UserPara5)},
    {USERPARA6,      				(u16*)(&UserPara6)},
    {USERPARA7,      				(u16*)(&UserPara7)},
    {USERPARA8,      				(u16*)(&UserPara8)},
    {USERPARA9,     				(u16*)(&UserPara9)},
    {USERPARA10,     				(u16*)(&UserPara10)},

// 	{ CO_GUARD_TIME,                (u16*)(&co_guard_time)},  					// u16 CANopen Guard Time 守护时间	  Index:0x100C
// 	{ CO_LIFE_TIME_FACTOR,          (u16*)(&co_life_time_factor_u16)},  		// u16 CANopen Life Time Factor 寿命保护因子   Index:0x100D
// 	{ CO_PRODUCER_HEARTBEAT_TIME,   (u16*)(&co_producer_heartbeat_time)},  		// u16 CANopen Producer Heartbeat Time 心跳产生时间  Index:0x1017

// 	//Receive PDO Communication Parameter
// 	{ CO_RPDO1_COB_ID_H,            ((u16*)(&co_rpdo1_cob_id)+1)},      		// u16 CANopen RPDO1 Cob-ID 高16位
// 	{ CO_RPDO1_COB_ID_L,            (u16*)(&co_rpdo1_cob_id)},  				// u16 CANopen RPDO1 Cob-ID 低16位
// 	{ CO_RPDO1_TRANSMISSION_TYPE,   (u16*)(&co_rpdo1_transmission_type_u16)},   // u16  CANopen RPDO1 Transmission Type 传输类型
// 	{ CO_RPDO2_COB_ID_H,            ((u16*)(&co_rpdo2_cob_id)+1)},      		// u16 CANopen RPDO2 Cob-ID 高16位
// 	{ CO_RPDO2_COB_ID_L,            (u16*)(&co_rpdo2_cob_id)},  				// u16 CANopen RPDO2 Cob-ID 低16位
// 	{ CO_RPDO2_TRANSMISSION_TYPE,   (u16*)(&co_rpdo2_transmission_type_u16)},   // u16  CANopen RPDO2 Transmission Type 传输类型
// 	{ CO_RPDO3_COB_ID_H,            ((u16*)(&co_rpdo3_cob_id)+1)},  			// u16 CANopen RPDO3 Cob-ID 高16位
// 	{ CO_RPDO3_COB_ID_L,            (u16*)(&co_rpdo3_cob_id)},  				// u16 CANopen RPDO3 Cob-ID 低16位
// 	{ CO_RPDO3_TRANSMISSION_TYPE,   (u16*)(&co_rpdo3_transmission_type_u16)},   // u16  CANopen RPDO3 Transmission Type 传输类型
// 	{ CO_RPDO4_COB_ID_H,            ((u16*)(&co_rpdo4_cob_id)+1)},      		// u16 CANopen RPDO4 Cob-ID 高16位
// 	{ CO_RPDO4_COB_ID_L,            (u16*)(&co_rpdo4_cob_id)},  				// u16 CANopen RPDO4 Cob-ID 低16位
// 	{ CO_RPDO4_TRANSMISSION_TYPE,   (u16*)(&co_rpdo4_transmission_type_u16)},   // u16  CANopen RPDO4 Transmission Type 传输类型

// 	//Transmit PDO Communication Parameter
// 	{ CO_TPDO1_COB_ID_H,            ((u16*)(&co_tpdo1_cob_id)+1)},     			// u16 CANopen TPDO1 Cob-ID 高16位
// 	{ CO_TPDO1_COB_ID_L,            (u16*)(&co_tpdo1_cob_id)},  				// u16 CANopen TPDO1 Cob-ID 低16位
// 	{ CO_TPDO1_TRANSMISSION_TYPE,   (u16*)(&co_tpdo1_transmission_type_u16)},   // u16  CANopen TPDO1 Transmission Type 传输类型
// 	{ CO_TPDO1_INHIBIT_TIME,        (u16*)(&co_tpdo1_inhibit_time)},       		// u16 CANopen TPDO1 Inhibit Time 抑制时间
// 	{ CO_TPDO1_EVENT_TIMER,         (u16*)(&co_tpdo1_event_timer)},         	// u16 CANopen TPDO1 Event Timer 事件时间
// 	{ CO_TPDO2_COB_ID_H,            ((u16*)(&co_tpdo2_cob_id)+1)},      		// u16 CANopen TPDO2 Cob-ID 高16位
// 	{ CO_TPDO2_COB_ID_L,            (u16*)(&co_tpdo2_cob_id)},  				// u16 CANopen TPDO2 Cob-ID 低16位
// 	{ CO_TPDO2_TRANSMISSION_TYPE,   (u16*)(&co_tpdo2_transmission_type_u16)},   // u16  CANopen TPDO2 Transmission Type 传输类型
// 	{ CO_TPDO2_INHIBIT_TIME,        (u16*)(&co_tpdo2_inhibit_time)},        	// u16 CANopen TPDO2 Inhibit Time 抑制时间
// 	{ CO_TPDO2_EVENT_TIMER,         (u16*)(&co_tpdo2_event_timer)},         	// u16 CANopen TPDO2 Event Timer 事件时间
// 	{ CO_TPDO3_COB_ID_H,            ((u16*)(&co_tpdo3_cob_id)+1)},      		// u16 CANopen TPDO3 Cob-ID 高16位
// 	{ CO_TPDO3_COB_ID_L,            (u16*)(&co_tpdo3_cob_id)},  				// u16 CANopen TPDO3 Cob-ID 低16位
// 	{ CO_TPDO3_TRANSMISSION_TYPE,   (u16*)(&co_tpdo3_transmission_type_u16)},   // u16  CANopen TPDO3 Transmission Type 传输类型
// 	{ CO_TPDO3_INHIBIT_TIME,        (u16*)(&co_tpdo3_inhibit_time)},        	// u16 CANopen TPDO3 Inhibit Time 抑制时间
// 	{ CO_TPDO3_EVENT_TIMER,         (u16*)(&co_tpdo3_event_timer)},         	// u16 CANopen TPDO3 Event Timer 事件时间
// 	{ CO_TPDO4_COB_ID_H,            ((u16*)(&co_tpdo4_cob_id)+1)},      		// u16 CANopen TPDO4 Cob-ID 高16位
// 	{ CO_TPDO4_COB_ID_L,            (u16*)(&co_tpdo4_cob_id)},  				// u16 CANopen TPDO4 Cob-ID 低16位
// 	{ CO_TPDO4_TRANSMISSION_TYPE,   (u16*)(&co_tpdo4_transmission_type_u16)},   // u16  CANopen TPDO4 Transmission Type 传输类型
// 	{ CO_TPDO4_INHIBIT_TIME,        (u16*)(&co_tpdo4_inhibit_time)},        	// u16 CANopen TPDO4 Inhibit Time 抑制时间
// 	{ CO_TPDO4_EVENT_TIMER,         (u16*)(&co_tpdo4_event_timer)},         	// u16 CANopen TPDO4 Event Timer 事件时间
};

CANopen_EXT CANopenParaType  CANopenParaWriteTable1[] =
{
    {430,	(u16*)(&	Servo_Err_OverLoad_On_Off	)},	/* Mapped at index 0x5000, subindex 0x01 */
    {431,	(u16*)(&	Servo_Err_OverLoad_Operating_Mode	)},	/* Mapped at index 0x5000, subindex 0x02 */
    {432,	(u16*)(&	Servo_Err_OverLoad_Ena_Delay	)},	/* Mapped at index 0x5000, subindex 0x03 */
    {433,	(u16*)(&	Servo_Err_OverLoad_Res_Delay	)},	/* Mapped at index 0x5000, subindex 0x04 */
    {434,	(u16*)(&	Servo_Err_OverLoad_Res_Type	)},	/* Mapped at index 0x5000, subindex 0x05 */
    {435,	(u16*)(&	Servo_Err_OverLoad_Trigger_Conditions	)},	/* Mapped at index 0x5000, subindex 0x06 */
    {436,	(u16*)(&	Servo_Err_OverLoad_Safer_Off	)},	/* Mapped at index 0x5000, subindex 0x07 */
    {437,	(u16*)(&	Servo_Err_OverVol_On_Off	)},	/* Mapped at index 0x5001, subindex 0x01 */
    {438,	(u16*)(&	Servo_Err_OverVol_Operating_Mode	)},	/* Mapped at index 0x5001, subindex 0x02 */
    {439,	(u16*)(&	Servo_Err_OverVol_Ena_Delay	)},	/* Mapped at index 0x5001, subindex 0x03 */
    {440,	(u16*)(&	Servo_Err_OverVol_Res_Delay	)},	/* Mapped at index 0x5001, subindex 0x04 */
    {441,	(u16*)(&	Servo_Err_OverVol_Res_Type	)},	/* Mapped at index 0x5001, subindex 0x05 */
    {442,	(u16*)(&	Servo_Err_OverVol_Trigger_Conditions	)},	/* Mapped at index 0x5001, subindex 0x06 */
    {443,	(u16*)(&	Servo_Err_OverVol_Safer_Off	)},	/* Mapped at index 0x5001, subindex 0x07 */
    {444,	(u16*)(&	Servo_Err_LowVol_On_Off	)},	/* Mapped at index 0x5002, subindex 0x01 */
    {445,	(u16*)(&	Servo_Err_LowVol_Operating_Mode	)},	/* Mapped at index 0x5002, subindex 0x02 */
    {446,	(u16*)(&	Servo_Err_LowVol_Ena_Delay	)},	/* Mapped at index 0x5002, subindex 0x03 */
    {447,	(u16*)(&	Servo_Err_LowVol_Res_Delay	)},	/* Mapped at index 0x5002, subindex 0x04 */
    {448,	(u16*)(&	Servo_Err_LowVol_Res_Type	)},	/* Mapped at index 0x5002, subindex 0x05 */
    {449,	(u16*)(&	Servo_Err_LowVol_Trigger_Conditions	)},	/* Mapped at index 0x5002, subindex 0x06 */
    {450,	(u16*)(&	Servo_Err_LowVol_Safer_Off	)},	/* Mapped at index 0x5002, subindex 0x07 */
    {451,	(u16*)(&	Servo_Err_Outer24VLost_On_Off	)},	/* Mapped at index 0x5003, subindex 0x01 */
    {452,	(u16*)(&	Servo_Err_Outer24VLost_Operating_Mode	)},	/* Mapped at index 0x5003, subindex 0x02 */
    {453,	(u16*)(&	Servo_Err_Outer24VLost_Ena_Delay	)},	/* Mapped at index 0x5003, subindex 0x03 */
    {454,	(u16*)(&	Servo_Err_Outer24VLost_Res_Delay	)},	/* Mapped at index 0x5003, subindex 0x04 */
    {455,	(u16*)(&	Servo_Err_Outer24VLost_Res_Type	)},	/* Mapped at index 0x5003, subindex 0x05 */
    {456,	(u16*)(&	Servo_Err_Outer24VLost_Trigger_Conditions	)},	/* Mapped at index 0x5003, subindex 0x06 */
    {457,	(u16*)(&	Servo_Err_Outer24VLost_Safer_Off	)},	/* Mapped at index 0x5003, subindex 0x07 */
    {458,	(u16*)(&	Servo_Err_PosLost_On_Off	)},	/* Mapped at index 0x5004, subindex 0x01 */
    {459,	(u16*)(&	Servo_Err_PosLost_Operating_Mode	)},	/* Mapped at index 0x5004, subindex 0x02 */
    {460,	(u16*)(&	Servo_Err_PosLost_Ena_Delay	)},	/* Mapped at index 0x5004, subindex 0x03 */
    {461,	(u16*)(&	Servo_Err_PosLost_Res_Delay	)},	/* Mapped at index 0x5004, subindex 0x04 */
    {462,	(u16*)(&	Servo_Err_PosLost_Res_Type	)},	/* Mapped at index 0x5004, subindex 0x05 */
    {463,	(u16*)(&	Servo_Err_PosLost_Trigger_Conditions	)},	/* Mapped at index 0x5004, subindex 0x06 */
    {464,	(u16*)(&	Servo_Err_PosLost_Safer_Off	)},	/* Mapped at index 0x5004, subindex 0x07 */
    {465,	(u16*)(&	Servo_Err_CANOpenLineOff_On_Off	)},	/* Mapped at index 0x5005, subindex 0x01 */
    {466,	(u16*)(&	Servo_Err_CANOpenLineOff_Operating_Mode	)},	/* Mapped at index 0x5005, subindex 0x02 */
    {467,	(u16*)(&	Servo_Err_CANOpenLineOff_Ena_Delay	)},	/* Mapped at index 0x5005, subindex 0x03 */
    {468,	(u16*)(&	Servo_Err_CANOpenLineOff_Res_Delay	)},	/* Mapped at index 0x5005, subindex 0x04 */
    {469,	(u16*)(&	Servo_Err_CANOpenLineOff_Res_Type	)},	/* Mapped at index 0x5005, subindex 0x05 */
    {470,	(u16*)(&	Servo_Err_CANOpenLineOff_Trigger_Conditions	)},	/* Mapped at index 0x5005, subindex 0x06 */
    {471,	(u16*)(&	Servo_Err_CANOpenLineOff_Safer_Off	)},	/* Mapped at index 0x5005, subindex 0x07 */
    {472,	(u16*)(&	Servo_Err_IGBTOverTemper_On_Off	)},	/* Mapped at index 0x5006, subindex 0x01 */
    {473,	(u16*)(&	Servo_Err_IGBTOverTemper_Operating_Mode	)},	/* Mapped at index 0x5006, subindex 0x02 */
    {474,	(u16*)(&	Servo_Err_IGBTOverTemper_Ena_Delay	)},	/* Mapped at index 0x5006, subindex 0x03 */
    {475,	(u16*)(&	Servo_Err_IGBTOverTemper_Res_Delay	)},	/* Mapped at index 0x5006, subindex 0x04 */
    {476,	(u16*)(&	Servo_Err_IGBTOverTemper_Res_Type	)},	/* Mapped at index 0x5006, subindex 0x05 */
    {477,	(u16*)(&	Servo_Err_IGBTOverTemper_Trigger_Conditions	)},	/* Mapped at index 0x5006, subindex 0x06 */
    {478,	(u16*)(&	Servo_Err_IGBTOverTemper_Safer_Off	)},	/* Mapped at index 0x5006, subindex 0x07 */
    {479,	(u16*)(&	Servo_Err_SSILineOff_On_Off	)},	/* Mapped at index 0x5007, subindex 0x01 */
    {480,	(u16*)(&	Servo_Err_SSILineOff_Operating_Mode	)},	/* Mapped at index 0x5007, subindex 0x02 */
    {481,	(u16*)(&	Servo_Err_SSILineOff_Ena_Delay	)},	/* Mapped at index 0x5007, subindex 0x03 */
    {482,	(u16*)(&	Servo_Err_SSILineOff_Res_Delay	)},	/* Mapped at index 0x5007, subindex 0x04 */
    {483,	(u16*)(&	Servo_Err_SSILineOff_Res_Type	)},	/* Mapped at index 0x5007, subindex 0x05 */
    {484,	(u16*)(&	Servo_Err_SSILineOff_Trigger_Conditions	)},	/* Mapped at index 0x5007, subindex 0x06 */
    {485,	(u16*)(&	Servo_Err_SSILineOff_Safer_Off	)},	/* Mapped at index 0x5007, subindex 0x07 */
    {486,	(u16*)(&	Servo_Err_MotoOverTemper_On_Off	)},	/* Mapped at index 0x5008, subindex 0x01 */
    {487,	(u16*)(&	Servo_Err_MotoOverTemper_Operating_Mode	)},	/* Mapped at index 0x5008, subindex 0x02 */
    {488,	(u16*)(&	Servo_Err_MotoOverTemper_Ena_Delay	)},	/* Mapped at index 0x5008, subindex 0x03 */
    {489,	(u16*)(&	Servo_Err_MotoOverTemper_Res_Delay	)},	/* Mapped at index 0x5008, subindex 0x04 */
    {490,	(u16*)(&	Servo_Err_MotoOverTemper_Res_Type	)},	/* Mapped at index 0x5008, subindex 0x05 */
    {491,	(u16*)(&	Servo_Err_MotoOverTemper_Trigger_Conditions	)},	/* Mapped at index 0x5008, subindex 0x06 */
    {492,	(u16*)(&	Servo_Err_MotoOverTemper_Safer_Off	)},	/* Mapped at index 0x5008, subindex 0x07 */
    {493,	(u16*)(&	Servo_Err_OverCurrent_On_Off	)},	/* Mapped at index 0x5009, subindex 0x01 */
    {494,	(u16*)(&	Servo_Err_OverCurrent_Operating_Mode	)},	/* Mapped at index 0x5009, subindex 0x02 */
    {495,	(u16*)(&	Servo_Err_OverCurrent_Ena_Delay	)},	/* Mapped at index 0x5009, subindex 0x03 */
    {496,	(u16*)(&	Servo_Err_OverCurrent_Res_Delay	)},	/* Mapped at index 0x5009, subindex 0x04 */
    {497,	(u16*)(&	Servo_Err_OverCurrent_Res_Type	)},	/* Mapped at index 0x5009, subindex 0x05 */
    {498,	(u16*)(&	Servo_Err_OverCurrent_Trigger_Conditions	)},	/* Mapped at index 0x5009, subindex 0x06 */
    {499,	(u16*)(&	Servo_Err_OverCurrent_Safer_Off	)},	/* Mapped at index 0x5009, subindex 0x07 */
    {500,	(u16*)(&	Servo_Err_ShortCircuit_On_Off	)},	/* Mapped at index 0x500A, subindex 0x01 */
    {501,	(u16*)(&	Servo_Err_ShortCircuit_Operating_Mode	)},	/* Mapped at index 0x500A, subindex 0x02 */
    {502,	(u16*)(&	Servo_Err_ShortCircuit_Ena_Delay	)},	/* Mapped at index 0x500A, subindex 0x03 */
    {503,	(u16*)(&	Servo_Err_ShortCircuit_Res_Delay	)},	/* Mapped at index 0x500A, subindex 0x04 */
    {504,	(u16*)(&	Servo_Err_ShortCircuit_Res_Type	)},	/* Mapped at index 0x500A, subindex 0x05 */
    {505,	(u16*)(&	Servo_Err_ShortCircuit_Trigger_Conditions	)},	/* Mapped at index 0x500A, subindex 0x06 */
    {506,	(u16*)(&	Servo_Err_ShortCircuit_Safer_Off	)},	/* Mapped at index 0x500A, subindex 0x07 */
    {507,	(u16*)(&	Servo_Err_CurrentOver_On_Off	)},	/* Mapped at index 0x500B, subindex 0x01 */
    {508,	(u16*)(&	Servo_Err_CurrentOver_Operating_Mode	)},	/* Mapped at index 0x500B, subindex 0x02 */
    {509,	(u16*)(&	Servo_Err_CurrentOver_Ena_Delay	)},	/* Mapped at index 0x500B, subindex 0x03 */
    {510,	(u16*)(&	Servo_Err_CurrentOver_Res_Delay	)},	/* Mapped at index 0x500B, subindex 0x04 */
    {511,	(u16*)(&	Servo_Err_CurrentOver_Res_Type	)},	/* Mapped at index 0x500B, subindex 0x05 */
    {512,	(u16*)(&	Servo_Err_CurrentOver_Trigger_Conditions	)},	/* Mapped at index 0x500B, subindex 0x06 */
    {513,	(u16*)(&	Servo_Err_CurrentOver_Safer_Off	)},	/* Mapped at index 0x500B, subindex 0x07 */
    {514,	(u16*)(&	Servo_Err_Limit96_On_Off	)},	/* Mapped at index 0x500C, subindex 0x01 */
    {515,	(u16*)(&	Servo_Err_Limit96_Operating_Mode	)},	/* Mapped at index 0x500C, subindex 0x02 */
    {516,	(u16*)(&	Servo_Err_Limit96_Ena_Delay	)},	/* Mapped at index 0x500C, subindex 0x03 */
    {517,	(u16*)(&	Servo_Err_Limit96_Res_Delay	)},	/* Mapped at index 0x500C, subindex 0x04 */
    {518,	(u16*)(&	Servo_Err_Limit96_Res_Type	)},	/* Mapped at index 0x500C, subindex 0x05 */
    {519,	(u16*)(&	Servo_Err_Limit96_Trigger_Conditions	)},	/* Mapped at index 0x500C, subindex 0x06 */
    {520,	(u16*)(&	Servo_Err_Limit96_Safer_Off	)},	/* Mapped at index 0x500C, subindex 0x07 */
    {521,	(u16*)(&	Servo_Err_380VErr_On_Off	)},	/* Mapped at index 0x500D, subindex 0x01 */
    {522,	(u16*)(&	Servo_Err_380VErr_Operating_Mode	)},	/* Mapped at index 0x500D, subindex 0x02 */
    {523,	(u16*)(&	Servo_Err_380VErr_Ena_Delay	)},	/* Mapped at index 0x500D, subindex 0x03 */
    {524,	(u16*)(&	Servo_Err_380VErr_Res_Delay	)},	/* Mapped at index 0x500D, subindex 0x04 */
    {525,	(u16*)(&	Servo_Err_380VErr_Res_Type	)},	/* Mapped at index 0x500D, subindex 0x05 */
    {526,	(u16*)(&	Servo_Err_380VErr_Trigger_Conditions	)},	/* Mapped at index 0x500D, subindex 0x06 */
    {527,	(u16*)(&	Servo_Err_380VErr_Safer_Off	)},	/* Mapped at index 0x500D, subindex 0x07 */
    {528,	(u16*)(&	Servo_Err_VelocityOver_On_Off	)},	/* Mapped at index 0x500E, subindex 0x01 */
    {529,	(u16*)(&	Servo_Err_VelocityOver_Operating_Mode	)},	/* Mapped at index 0x500E, subindex 0x02 */
    {530,	(u16*)(&	Servo_Err_VelocityOver_Ena_Delay	)},	/* Mapped at index 0x500E, subindex 0x03 */
    {531,	(u16*)(&	Servo_Err_VelocityOver_Res_Delay	)},	/* Mapped at index 0x500E, subindex 0x04 */
    {532,	(u16*)(&	Servo_Err_VelocityOver_Res_Type	)},	/* Mapped at index 0x500E, subindex 0x05 */
    {533,	(u16*)(&	Servo_Err_VelocityOver_Trigger_Conditions	)},	/* Mapped at index 0x500E, subindex 0x06 */
    {534,	(u16*)(&	Servo_Err_VelocityOver_Safer_Off	)},	/* Mapped at index 0x500E, subindex 0x07 */
    {535,	(u16*)(&	Servo_Err_DischargeFail_On_Off	)},	/* Mapped at index 0x500F, subindex 0x01 */
    {536,	(u16*)(&	Servo_Err_DischargeFail_Operating_Mode	)},	/* Mapped at index 0x500F, subindex 0x02 */
    {537,	(u16*)(&	Servo_Err_DischargeFail_Ena_Delay	)},	/* Mapped at index 0x500F, subindex 0x03 */
    {538,	(u16*)(&	Servo_Err_DischargeFail_Res_Delay	)},	/* Mapped at index 0x500F, subindex 0x04 */
    {539,	(u16*)(&	Servo_Err_DischargeFail_Res_Type	)},	/* Mapped at index 0x500F, subindex 0x05 */
    {540,	(u16*)(&	Servo_Err_DischargeFail_Trigger_Conditions	)},	/* Mapped at index 0x500F, subindex 0x06 */
    {541,	(u16*)(&	Servo_Err_DischargeFail_Safer_Off	)},	/* Mapped at index 0x500F, subindex 0x07 */
    {542,	(u16*)(&	Servo_Err_BK24VLost_On_Off	)},	/* Mapped at index 0x5010, subindex 0x01 */
    {543,	(u16*)(&	Servo_Err_BK24VLost_Operating_Mode	)},	/* Mapped at index 0x5010, subindex 0x02 */
    {544,	(u16*)(&	Servo_Err_BK24VLost_Ena_Delay	)},	/* Mapped at index 0x5010, subindex 0x03 */
    {545,	(u16*)(&	Servo_Err_BK24VLost_Res_Delay	)},	/* Mapped at index 0x5010, subindex 0x04 */
    {546,	(u16*)(&	Servo_Err_BK24VLost_Res_Type	)},	/* Mapped at index 0x5010, subindex 0x05 */
    {547,	(u16*)(&	Servo_Err_BK24VLost_Trigger_Conditions	)},	/* Mapped at index 0x5010, subindex 0x06 */
    {548,	(u16*)(&	Servo_Err_BK24VLost_Safer_Off	)},	/* Mapped at index 0x5010, subindex 0x07 */
    {549,	(u16*)(&	Servo_Err_Iner24VLost_On_Off	)},	/* Mapped at index 0x5011, subindex 0x01 */
    {550,	(u16*)(&	Servo_Err_Iner24VLost_Operating_Mode	)},	/* Mapped at index 0x5011, subindex 0x02 */
    {551,	(u16*)(&	Servo_Err_Iner24VLost_Ena_Delay	)},	/* Mapped at index 0x5011, subindex 0x03 */
    {552,	(u16*)(&	Servo_Err_Iner24VLost_Res_Delay	)},	/* Mapped at index 0x5011, subindex 0x04 */
    {553,	(u16*)(&	Servo_Err_Iner24VLost_Res_Type	)},	/* Mapped at index 0x5011, subindex 0x05 */
    {554,	(u16*)(&	Servo_Err_Iner24VLost_Trigger_Conditions	)},	/* Mapped at index 0x5011, subindex 0x06 */
    {555,	(u16*)(&	Servo_Err_Iner24VLost_Safer_Off	)},	/* Mapped at index 0x5011, subindex 0x07 */
    {556,	(u16*)(&	Servo_Err_DITrigErr_On_Off	)},	/* Mapped at index 0x5012, subindex 0x01 */
    {557,	(u16*)(&	Servo_Err_DITrigErr_Operating_Mode	)},	/* Mapped at index 0x5012, subindex 0x02 */
    {558,	(u16*)(&	Servo_Err_DITrigErr_Ena_Delay	)},	/* Mapped at index 0x5012, subindex 0x03 */
    {559,	(u16*)(&	Servo_Err_DITrigErr_Res_Delay	)},	/* Mapped at index 0x5012, subindex 0x04 */
    {560,	(u16*)(&	Servo_Err_DITrigErr_Res_Type	)},	/* Mapped at index 0x5012, subindex 0x05 */
    {561,	(u16*)(&	Servo_Err_DITrigErr_Trigger_Conditions	)},	/* Mapped at index 0x5012, subindex 0x06 */
    {562,	(u16*)(&	Servo_Err_DITrigErr_Safer_Off	)},	/* Mapped at index 0x5012, subindex 0x07 */
    {563,	(u16*)(&	Servo_Err_SaferErr_On_Off	)},	/* Mapped at index 0x5013, subindex 0x01 */
    {564,	(u16*)(&	Servo_Err_SaferErr_Operating_Mode	)},	/* Mapped at index 0x5013, subindex 0x02 */
    {565,	(u16*)(&	Servo_Err_SaferErr_Ena_Delay	)},	/* Mapped at index 0x5013, subindex 0x03 */
    {566,	(u16*)(&	Servo_Err_SaferErr_Res_Delay	)},	/* Mapped at index 0x5013, subindex 0x04 */
    {567,	(u16*)(&	Servo_Err_SaferErr_Res_Type	)},	/* Mapped at index 0x5013, subindex 0x05 */
    {568,	(u16*)(&	Servo_Err_SaferErr_Trigger_Conditions	)},	/* Mapped at index 0x5013, subindex 0x06 */
    {569,	(u16*)(&	Servo_Err_SaferErr_Safer_Off	)},	/* Mapped at index 0x5013, subindex 0x07 */
    {570,	(u16*)(&	Servo_Err_PositionOver_On_Off	)},	/* Mapped at index 0x5014, subindex 0x01 */
    {571,	(u16*)(&	Servo_Err_PositionOver_Operating_Mode	)},	/* Mapped at index 0x5014, subindex 0x02 */
    {572,	(u16*)(&	Servo_Err_PositionOver_Ena_Delay	)},	/* Mapped at index 0x5014, subindex 0x03 */
    {573,	(u16*)(&	Servo_Err_PositionOver_Res_Delay	)},	/* Mapped at index 0x5014, subindex 0x04 */
    {574,	(u16*)(&	Servo_Err_PositionOver_Res_Type	)},	/* Mapped at index 0x5014, subindex 0x05 */
    {575,	(u16*)(&	Servo_Err_PositionOver_Trigger_Conditions	)},	/* Mapped at index 0x5014, subindex 0x06 */
    {576,	(u16*)(&	Servo_Err_PositionOver_Safer_Off	)},	/* Mapped at index 0x5014, subindex 0x07 */
    {577,	(u16*)(&	Servo_Err_PaddleOver_On_Off	)},	/* Mapped at index 0x5015, subindex 0x01 */
    {578,	(u16*)(&	Servo_Err_PaddleOver_Operating_Mode	)},	/* Mapped at index 0x5015, subindex 0x02 */
    {579,	(u16*)(&	Servo_Err_PaddleOver_Ena_Delay	)},	/* Mapped at index 0x5015, subindex 0x03 */
    {580,	(u16*)(&	Servo_Err_PaddleOver_Res_Delay	)},	/* Mapped at index 0x5015, subindex 0x04 */
    {581,	(u16*)(&	Servo_Err_PaddleOver_Res_Type	)},	/* Mapped at index 0x5015, subindex 0x05 */
    {582,	(u16*)(&	Servo_Err_PaddleOver_Trigger_Conditions	)},	/* Mapped at index 0x5015, subindex 0x06 */
    {583,	(u16*)(&	Servo_Err_PaddleOver_Safer_Off	)},	/* Mapped at index 0x5015, subindex 0x07 */
    {584,	(u16*)(&	Servo_Err_IGBTLineOff_On_Off	)},	/* Mapped at index 0x5016, subindex 0x01 */
    {585,	(u16*)(&	Servo_Err_IGBTLineOff_Operating_Mode	)},	/* Mapped at index 0x5016, subindex 0x02 */
    {586,	(u16*)(&	Servo_Err_IGBTLineOff_Ena_Delay	)},	/* Mapped at index 0x5016, subindex 0x03 */
    {587,	(u16*)(&	Servo_Err_IGBTLineOff_Res_Delay	)},	/* Mapped at index 0x5016, subindex 0x04 */
    {588,	(u16*)(&	Servo_Err_IGBTLineOff_Res_Type	)},	/* Mapped at index 0x5016, subindex 0x05 */
    {589,	(u16*)(&	Servo_Err_IGBTLineOff_Trigger_Conditions	)},	/* Mapped at index 0x5016, subindex 0x06 */
    {590,	(u16*)(&	Servo_Err_IGBTLineOff_Safer_Off	)},	/* Mapped at index 0x5016, subindex 0x07 */
    {591,	(u16*)(&	Servo_Err_MotoLineOff_On_Off	)},	/* Mapped at index 0x5017, subindex 0x01 */
    {592,	(u16*)(&	Servo_Err_MotoLineOff_Operating_Mode	)},	/* Mapped at index 0x5017, subindex 0x02 */
    {593,	(u16*)(&	Servo_Err_MotoLineOff_Ena_Delay	)},	/* Mapped at index 0x5017, subindex 0x03 */
    {594,	(u16*)(&	Servo_Err_MotoLineOff_Res_Delay	)},	/* Mapped at index 0x5017, subindex 0x04 */
    {595,	(u16*)(&	Servo_Err_MotoLineOff_Res_Type	)},	/* Mapped at index 0x5017, subindex 0x05 */
    {596,	(u16*)(&	Servo_Err_MotoLineOff_Trigger_Conditions	)},	/* Mapped at index 0x5017, subindex 0x06 */
    {597,	(u16*)(&	Servo_Err_MotoLineOff_Safer_Off	)},	/* Mapped at index 0x5017, subindex 0x07 */
    {598,	(u16*)(&	Servo_Err_ChargerComErr_On_Off	)},	/* Mapped at index 0x5018, subindex 0x01 */
    {599,	(u16*)(&	Servo_Err_ChargerComErr_Operating_Mode	)},	/* Mapped at index 0x5018, subindex 0x02 */
    {600,	(u16*)(&	Servo_Err_ChargerComErr_Ena_Delay	)},	/* Mapped at index 0x5018, subindex 0x03 */
    {601,	(u16*)(&	Servo_Err_ChargerComErr_Res_Delay	)},	/* Mapped at index 0x5018, subindex 0x04 */
    {602,	(u16*)(&	Servo_Err_ChargerComErr_Res_Type	)},	/* Mapped at index 0x5018, subindex 0x05 */
    {603,	(u16*)(&	Servo_Err_ChargerComErr_Trigger_Conditions	)},	/* Mapped at index 0x5018, subindex 0x06 */
    {604,	(u16*)(&	Servo_Err_ChargerComErr_Safer_Off	)},	/* Mapped at index 0x5018, subindex 0x07 */
    {605,	(u16*)(&	Servo_Err_NULL1_On_Off	)},	/* Mapped at index 0x5019, subindex 0x01 */
    {606,	(u16*)(&	Servo_Err_NULL1_Operating_Mode	)},	/* Mapped at index 0x5019, subindex 0x02 */
    {607,	(u16*)(&	Servo_Err_NULL1_Ena_Delay	)},	/* Mapped at index 0x5019, subindex 0x03 */
    {608,	(u16*)(&	Servo_Err_NULL1_Res_Delay	)},	/* Mapped at index 0x5019, subindex 0x04 */
    {609,	(u16*)(&	Servo_Err_NULL1_Res_Type	)},	/* Mapped at index 0x5019, subindex 0x05 */
    {610,	(u16*)(&	Servo_Err_NULL1_Trigger_Conditions	)},	/* Mapped at index 0x5019, subindex 0x06 */
    {611,	(u16*)(&	Servo_Err_NULL1_Safer_Off	)},	/* Mapped at index 0x5019, subindex 0x07 */
    {612,	(u16*)(&	Servo_Err_NULL2_On_Off	)},	/* Mapped at index 0x501A, subindex 0x01 */
    {613,	(u16*)(&	Servo_Err_NULL2_Operating_Mode	)},	/* Mapped at index 0x501A, subindex 0x02 */
    {614,	(u16*)(&	Servo_Err_NULL2_Ena_Delay	)},	/* Mapped at index 0x501A, subindex 0x03 */
    {615,	(u16*)(&	Servo_Err_NULL2_Res_Delay	)},	/* Mapped at index 0x501A, subindex 0x04 */
    {616,	(u16*)(&	Servo_Err_NULL2_Res_Type	)},	/* Mapped at index 0x501A, subindex 0x05 */
    {617,	(u16*)(&	Servo_Err_NULL2_Trigger_Conditions	)},	/* Mapped at index 0x501A, subindex 0x06 */
    {618,	(u16*)(&	Servo_Err_NULL2_Safer_Off	)},	/* Mapped at index 0x501A, subindex 0x07 */
    {619,	(u16*)(&	Servo_Err_NULL3_On_Off	)},	/* Mapped at index 0x501B, subindex 0x01 */
    {620,	(u16*)(&	Servo_Err_NULL3_Operating_Mode	)},	/* Mapped at index 0x501B, subindex 0x02 */
    {621,	(u16*)(&	Servo_Err_NULL3_Ena_Delay	)},	/* Mapped at index 0x501B, subindex 0x03 */
    {622,	(u16*)(&	Servo_Err_NULL3_Res_Delay	)},	/* Mapped at index 0x501B, subindex 0x04 */
    {623,	(u16*)(&	Servo_Err_NULL3_Res_Type	)},	/* Mapped at index 0x501B, subindex 0x05 */
    {624,	(u16*)(&	Servo_Err_NULL3_Trigger_Conditions	)},	/* Mapped at index 0x501B, subindex 0x06 */
    {625,	(u16*)(&	Servo_Err_NULL3_Safer_Off	)},	/* Mapped at index 0x501B, subindex 0x07 */
    {626,	(u16*)(&	Servo_Err_NULL4_On_Off	)},	/* Mapped at index 0x501C, subindex 0x01 */
    {627,	(u16*)(&	Servo_Err_NULL4_Operating_Mode	)},	/* Mapped at index 0x501C, subindex 0x02 */
    {628,	(u16*)(&	Servo_Err_NULL4_Ena_Delay	)},	/* Mapped at index 0x501C, subindex 0x03 */
    {629,	(u16*)(&	Servo_Err_NULL4_Res_Delay	)},	/* Mapped at index 0x501C, subindex 0x04 */
    {630,	(u16*)(&	Servo_Err_NULL4_Res_Type	)},	/* Mapped at index 0x501C, subindex 0x05 */
    {631,	(u16*)(&	Servo_Err_NULL4_Trigger_Conditions	)},	/* Mapped at index 0x501C, subindex 0x06 */
    {632,	(u16*)(&	Servo_Err_NULL4_Safer_Off	)},	/* Mapped at index 0x501C, subindex 0x07 */
    {633,	(u16*)(&	Servo_Err_NULL5_On_Off	)},	/* Mapped at index 0x501D, subindex 0x01 */
    {634,	(u16*)(&	Servo_Err_NULL5_Operating_Mode	)},	/* Mapped at index 0x501D, subindex 0x02 */
    {635,	(u16*)(&	Servo_Err_NULL5_Ena_Delay	)},	/* Mapped at index 0x501D, subindex 0x03 */
    {636,	(u16*)(&	Servo_Err_NULL5_Res_Delay	)},	/* Mapped at index 0x501D, subindex 0x04 */
    {637,	(u16*)(&	Servo_Err_NULL5_Res_Type	)},	/* Mapped at index 0x501D, subindex 0x05 */
    {638,	(u16*)(&	Servo_Err_NULL5_Trigger_Conditions	)},	/* Mapped at index 0x501D, subindex 0x06 */
    {639,	(u16*)(&	Servo_Err_NULL5_Safer_Off	)},	/* Mapped at index 0x501D, subindex 0x07 */
    {640,	(u16*)(&	Servo_Err_NULL6_On_Off	)},	/* Mapped at index 0x501E, subindex 0x01 */
    {641,	(u16*)(&	Servo_Err_NULL6_Operating_Mode	)},	/* Mapped at index 0x501E, subindex 0x02 */
    {642,	(u16*)(&	Servo_Err_NULL6_Ena_Delay	)},	/* Mapped at index 0x501E, subindex 0x03 */
    {643,	(u16*)(&	Servo_Err_NULL6_Res_Delay	)},	/* Mapped at index 0x501E, subindex 0x04 */
    {644,	(u16*)(&	Servo_Err_NULL6_Res_Type	)},	/* Mapped at index 0x501E, subindex 0x05 */
    {645,	(u16*)(&	Servo_Err_NULL6_Trigger_Conditions	)},	/* Mapped at index 0x501E, subindex 0x06 */
    {646,	(u16*)(&	Servo_Err_NULL6_Safer_Off	)},	/* Mapped at index 0x501E, subindex 0x07 */
    {647,	(u16*)(&	Servo_Err_NULL7_On_Off	)},	/* Mapped at index 0x501F, subindex 0x01 */
    {648,	(u16*)(&	Servo_Err_NULL7_Operating_Mode	)},	/* Mapped at index 0x501F, subindex 0x02 */
    {649,	(u16*)(&	Servo_Err_NULL7_Ena_Delay	)},	/* Mapped at index 0x501F, subindex 0x03 */
    {650,	(u16*)(&	Servo_Err_NULL7_Res_Delay	)},	/* Mapped at index 0x501F, subindex 0x04 */
    {651,	(u16*)(&	Servo_Err_NULL7_Res_Type	)},	/* Mapped at index 0x501F, subindex 0x05 */
    {652,	(u16*)(&	Servo_Err_NULL7_Trigger_Conditions	)},	/* Mapped at index 0x501F, subindex 0x06 */
    {653,	(u16*)(&	Servo_Err_NULL7_Safer_Off	)},	/* Mapped at index 0x501F, subindex 0x07 */
    {654,	(u16*)(&	Charger_Err_Total_On_Off	)},	/* Mapped at index 0x5020, subindex 0x01 */
    {655,	(u16*)(&	Charger_Err_Total_Operating_Mode	)},	/* Mapped at index 0x5020, subindex 0x02 */
    {656,	(u16*)(&	Charger_Err_Total_Ena_Delay	)},	/* Mapped at index 0x5020, subindex 0x03 */
    {657,	(u16*)(&	Charger_Err_Total_Res_Delay	)},	/* Mapped at index 0x5020, subindex 0x04 */
    {658,	(u16*)(&	Charger_Err_Total_Res_Type	)},	/* Mapped at index 0x5020, subindex 0x05 */
    {659,	(u16*)(&	Charger_Err_Total_Trigger_Conditions	)},	/* Mapped at index 0x5020, subindex 0x06 */
    {660,	(u16*)(&	Charger_Err_Total_Safer_Off	)},	/* Mapped at index 0x5020, subindex 0x07 */
    {661,	(u16*)(&	Charger_Err_SoftOverVol_On_Off	)},	/* Mapped at index 0x5021, subindex 0x01 */
    {662,	(u16*)(&	Charger_Err_SoftOverVol_Operating_Mode	)},	/* Mapped at index 0x5021, subindex 0x02 */
    {663,	(u16*)(&	Charger_Err_SoftOverVol_Ena_Delay	)},	/* Mapped at index 0x5021, subindex 0x03 */
    {664,	(u16*)(&	Charger_Err_SoftOverVol_Res_Delay	)},	/* Mapped at index 0x5021, subindex 0x04 */
    {665,	(u16*)(&	Charger_Err_SoftOverVol_Res_Type	)},	/* Mapped at index 0x5021, subindex 0x05 */
    {666,	(u16*)(&	Charger_Err_SoftOverVol_Trigger_Conditions	)},	/* Mapped at index 0x5021, subindex 0x06 */
    {667,	(u16*)(&	Charger_Err_SoftOverVol_Safer_Off	)},	/* Mapped at index 0x5021, subindex 0x07 */
    {668,	(u16*)(&	Charger_Err_SoftOverCurrent_On_Off	)},	/* Mapped at index 0x5022, subindex 0x01 */
    {669,	(u16*)(&	Charger_Err_SoftOverCurrent_Operating_Mode	)},	/* Mapped at index 0x5022, subindex 0x02 */
    {670,	(u16*)(&	Charger_Err_SoftOverCurrent_Ena_Delay	)},	/* Mapped at index 0x5022, subindex 0x03 */
    {671,	(u16*)(&	Charger_Err_SoftOverCurrent_Res_Delay	)},	/* Mapped at index 0x5022, subindex 0x04 */
    {672,	(u16*)(&	Charger_Err_SoftOverCurrent_Res_Type	)},	/* Mapped at index 0x5022, subindex 0x05 */
    {673,	(u16*)(&	Charger_Err_SoftOverCurrent_Trigger_Conditions	)},	/* Mapped at index 0x5022, subindex 0x06 */
    {674,	(u16*)(&	Charger_Err_SoftOverCurrent_Safer_Off	)},	/* Mapped at index 0x5022, subindex 0x07 */
    {675,	(u16*)(&	Charger_Err_ShortCircuit_On_Off	)},	/* Mapped at index 0x5023, subindex 0x01 */
    {676,	(u16*)(&	Charger_Err_ShortCircuit_Operating_Mode	)},	/* Mapped at index 0x5023, subindex 0x02 */
    {677,	(u16*)(&	Charger_Err_ShortCircuit_Ena_Delay	)},	/* Mapped at index 0x5023, subindex 0x03 */
    {678,	(u16*)(&	Charger_Err_ShortCircuit_Res_Delay	)},	/* Mapped at index 0x5023, subindex 0x04 */
    {679,	(u16*)(&	Charger_Err_ShortCircuit_Res_Type	)},	/* Mapped at index 0x5023, subindex 0x05 */
    {680,	(u16*)(&	Charger_Err_ShortCircuit_Trigger_Conditions	)},	/* Mapped at index 0x5023, subindex 0x06 */
    {681,	(u16*)(&	Charger_Err_ShortCircuit_Safer_Off	)},	/* Mapped at index 0x5023, subindex 0x07 */
    {682,	(u16*)(&	Charger_Err_BrokenCircuit_On_Off	)},	/* Mapped at index 0x5024, subindex 0x01 */
    {683,	(u16*)(&	Charger_Err_BrokenCircuit_Operating_Mode	)},	/* Mapped at index 0x5024, subindex 0x02 */
    {684,	(u16*)(&	Charger_Err_BrokenCircuit_Ena_Delay	)},	/* Mapped at index 0x5024, subindex 0x03 */
    {685,	(u16*)(&	Charger_Err_BrokenCircuit_Res_Delay	)},	/* Mapped at index 0x5024, subindex 0x04 */
    {686,	(u16*)(&	Charger_Err_BrokenCircuit_Res_Type	)},	/* Mapped at index 0x5024, subindex 0x05 */
    {687,	(u16*)(&	Charger_Err_BrokenCircuit_Trigger_Conditions	)},	/* Mapped at index 0x5024, subindex 0x06 */
    {688,	(u16*)(&	Charger_Err_BrokenCircuit_Safer_Off	)},	/* Mapped at index 0x5024, subindex 0x07 */
    {689,	(u16*)(&	Charger_Err_DYErr_On_Off	)},	/* Mapped at index 0x5025, subindex 0x01 */
    {690,	(u16*)(&	Charger_Err_DYErr_Operating_Mode	)},	/* Mapped at index 0x5025, subindex 0x02 */
    {691,	(u16*)(&	Charger_Err_DYErr_Ena_Delay	)},	/* Mapped at index 0x5025, subindex 0x03 */
    {692,	(u16*)(&	Charger_Err_DYErr_Res_Delay	)},	/* Mapped at index 0x5025, subindex 0x04 */
    {693,	(u16*)(&	Charger_Err_DYErr_Res_Type	)},	/* Mapped at index 0x5025, subindex 0x05 */
    {694,	(u16*)(&	Charger_Err_DYErr_Trigger_Conditions	)},	/* Mapped at index 0x5025, subindex 0x06 */
    {695,	(u16*)(&	Charger_Err_DYErr_Safer_Off	)},	/* Mapped at index 0x5025, subindex 0x07 */
    {696,	(u16*)(&	Charger_Err_LowVol_On_Off	)},	/* Mapped at index 0x5026, subindex 0x01 */
    {697,	(u16*)(&	Charger_Err_LowVol_Operating_Mode	)},	/* Mapped at index 0x5026, subindex 0x02 */
    {698,	(u16*)(&	Charger_Err_LowVol_Ena_Delay	)},	/* Mapped at index 0x5026, subindex 0x03 */
    {699,	(u16*)(&	Charger_Err_LowVol_Res_Delay	)},	/* Mapped at index 0x5026, subindex 0x04 */
    {700,	(u16*)(&	Charger_Err_LowVol_Res_Type	)},	/* Mapped at index 0x5026, subindex 0x05 */
    {701,	(u16*)(&	Charger_Err_LowVol_Trigger_Conditions	)},	/* Mapped at index 0x5026, subindex 0x06 */
    {702,	(u16*)(&	Charger_Err_LowVol_Safer_Off	)},	/* Mapped at index 0x5026, subindex 0x07 */
    {703,	(u16*)(&	Charger_Err_IGBTOverTemper_On_Off	)},	/* Mapped at index 0x5027, subindex 0x01 */
    {704,	(u16*)(&	Charger_Err_IGBTOverTemper_Operating_Mode	)},	/* Mapped at index 0x5027, subindex 0x02 */
    {705,	(u16*)(&	Charger_Err_IGBTOverTemper_Ena_Delay	)},	/* Mapped at index 0x5027, subindex 0x03 */
    {706,	(u16*)(&	Charger_Err_IGBTOverTemper_Res_Delay	)},	/* Mapped at index 0x5027, subindex 0x04 */
    {707,	(u16*)(&	Charger_Err_IGBTOverTemper_Res_Type	)},	/* Mapped at index 0x5027, subindex 0x05 */
    {708,	(u16*)(&	Charger_Err_IGBTOverTemper_Trigger_Conditions	)},	/* Mapped at index 0x5027, subindex 0x06 */
    {709,	(u16*)(&	Charger_Err_IGBTOverTemper_Safer_Off	)},	/* Mapped at index 0x5027, subindex 0x07 */
    {710,	(u16*)(&	Charger_Err_HardOverVol_On_Off	)},	/* Mapped at index 0x5028, subindex 0x01 */
    {711,	(u16*)(&	Charger_Err_HardOverVol_Operating_Mode	)},	/* Mapped at index 0x5028, subindex 0x02 */
    {712,	(u16*)(&	Charger_Err_HardOverVol_Ena_Delay	)},	/* Mapped at index 0x5028, subindex 0x03 */
    {713,	(u16*)(&	Charger_Err_HardOverVol_Res_Delay	)},	/* Mapped at index 0x5028, subindex 0x04 */
    {714,	(u16*)(&	Charger_Err_HardOverVol_Res_Type	)},	/* Mapped at index 0x5028, subindex 0x05 */
    {715,	(u16*)(&	Charger_Err_HardOverVol_Trigger_Conditions	)},	/* Mapped at index 0x5028, subindex 0x06 */
    {716,	(u16*)(&	Charger_Err_HardOverVol_Safer_Off	)},	/* Mapped at index 0x5028, subindex 0x07 */
    {717,	(u16*)(&	Charger_Err_HardOverCurrent_On_Off	)},
    {718,	(u16*)(&	Charger_Err_HardOverCurrent_Operating_Mode	)},
    {719,	(u16*)(&	Charger_Err_HardOverCurrent_Ena_Delay	)},
    {720,	(u16*)(&	Charger_Err_HardOverCurrent_Res_Delay	)},
    {721,	(u16*)(&	Charger_Err_HardOverCurrent_Res_Type	)},
    {722,	(u16*)(&	Charger_Err_HardOverCurrent_Trigger_Conditions	)},
    {723,	(u16*)(&	Charger_Err_HardOverCurrent_Safer_Off	)},
    {724,	(u16*)(&	Charger_Err_HardOverCurrent1_On_Off	)},	/* Mapped at index 0x5030, subindex 0x01 */
    {725,	(u16*)(&	Charger_Err_HardOverCurrent1_Operating_Mode 	)},	/* Mapped at index 0x5030, subindex 0x02 */
    {726,	(u16*)(&	Charger_Err_HardOverCurrent1_Ena_Delay 	)},	/* Mapped at index 0x5030, subindex 0x03 */
    {727,	(u16*)(&	Charger_Err_HardOverCurrent1_Res_Delay	)},	/* Mapped at index 0x5030, subindex 0x04 */
    {728,	(u16*)(&	Charger_Err_HardOverCurrent1_Res_Type 	)},	/* Mapped at index 0x5030, subindex 0x05 */
    {729,	(u16*)(&	Charger_Err_HardOverCurrent1_Trigger_Conditions	)},	/* Mapped at index 0x5030, subindex 0x06 */
    {730,	(u16*)(&	Charger_Err_HardOverCurrent1_Safer_Off 	)},	/* Mapped at index 0x5030, subindex 0x07 */
    {731,	(u16*)(&	Charger_Err_OverVol_On_Off 	)},	/* Mapped at index 0x5031, subindex 0x01 */
    {732,	(u16*)(&	Charger_Err_OverVol_Operating_Mode	)},	/* Mapped at index 0x5031, subindex 0x02 */
    {733,	(u16*)(&	Charger_Err_OverVol_Ena_Delay	)},	/* Mapped at index 0x5031, subindex 0x03 */
    {734,	(u16*)(&	Charger_Err_OverVol_Res_Delay	)},	/* Mapped at index 0x5031, subindex 0x04 */
    {735,	(u16*)(&	Charger_Err_OverVol_Res_Type	)},	/* Mapped at index 0x5031, subindex 0x05 */
    {736,	(u16*)(&	Charger_Err_OverVol_Trigger_Conditions	)},	/* Mapped at index 0x5031, subindex 0x06 */
    {737,	(u16*)(&	Charger_Err_OverVol_Safer_Off	)},
};

u16 CANopenParaWriteTableOld[CANopenParaWriteTableSize];
s16 target_value_old = 0;

#define CANopenParaWriteTableSize1 (sizeof(CANopenParaWriteTable1)/sizeof(CANopenParaType))
u16 CANopenParaWriteTableOld1[CANopenParaWriteTableSize1];
u16 CANopen_Num1  = 0;

/*******************************************************************************
* 函数名 : UpSysPara
* 描  述 : 组装CANopen参数帧
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void UpSysPara(u8 cmd, u16 ParaID, u16 NewData)
{
    u16	i;
    ExCANopen_RxBuf[0] = CommAddr;
    ExCANopen_RxBuf[1] = (cmd | ((u8)(ParaID>>8) & 0x3F));
    ExCANopen_RxBuf[2] = (u8)(ParaID & 0x00FF);
    ExCANopen_RxBuf[3] = NewData/256;
    ExCANopen_RxBuf[4] = (u8)NewData;
    i=CRC16(ExCANopen_RxBuf, EXUART_RX_DATA_FRAMELEN-2);
    ExCANopen_RxBuf[EXUART_RX_DATA_FRAMELEN-2]	= i/256;
    ExCANopen_RxBuf[EXUART_RX_DATA_FRAMELEN-1]	= (u8)i;

    ExCANopenRxFullFlag = 1;    // 模拟置ARM接收CANopen一帧完成标志
}

void UpSysParaU(u8 cmd, u16 ParaID, u16 NewData)
{
    u16	i;
    ExUart_RxBuf[0] = CommAddr;
    ExUart_RxBuf[1] = (cmd | ((u8)(ParaID>>8) & 0x3F));
    ExUart_RxBuf[2] = (u8)(ParaID & 0x00FF);
    ExUart_RxBuf[3] = NewData/256;
    ExUart_RxBuf[4] = (u8)NewData;
    i=CRC16(ExUart_RxBuf, EXUART_RX_DATA_FRAMELEN-2);
    ExUart_RxBuf[EXUART_RX_DATA_FRAMELEN-2]	= i/256;
    ExUart_RxBuf[EXUART_RX_DATA_FRAMELEN-1]	= (u8)i;

    ExUartRxFullFlag = 1;    // 模拟置ARM接收CANopen一帧完成标志
}

/*******************************************************************************
* 函数名 : Arm_Dsp_CommErr_Handle
* 描  述 : 300us
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void Arm_Dsp_CommErr_Handle(void)
{
	static u32 cnt = 0;
	
	switch (Arm_Dsp_CommErr_Step)
	{
		case 0:
		{
			if (READ_WKI)
			{
				cnt = 0;
				WKO_LOW;											//t0 ~= 10ms（此时间为DSP判断WKO为低电平，保持10ms后，再将WKI拉低）
			}
			else if ((READ_WKI == 0) && (cnt < 93))					//t1 ~= 300us*93 ~= 28ms
			{
				cnt++;
				WKO_LOW;
			}
			else if (cnt >= 93)	
			{
				cnt = 0;
				Arm_Dsp_CommErr_Step = 1;
			}
			break;
		}

		case 1:
		{
			WKO_HIGH;
			
			if (READ_WKI == 0)										//t2 ~= 10ms（此时间为DSP判断WKO为高电平，保持10ms后，再将WKI置高）
			{
				cnt = 0;
			}
			else if ((READ_WKI) && (cnt < 26))						//t3 ~= 300us*26 ~= 7.8ms(读取DSP输出的WKI信号低电平保持)，才进入SendParaPackToDsp() 
			{
				cnt++;
			}
			else
			{
				cnt = 0;
				Arm_Dsp_CommErr_Step = 2;
			}
			break;
		}
		
		case 2:
		{
			SendParaPackToDsp();
			break;
		}
		
		default:
		{
			cnt = 0;
			Arm_Dsp_CommErr_Step = 0;
			break;
		}
	}
	
	//DspHeartBeatCountOld = ReadPara(DSP_HEARTBEAT_COUNT);
}

/*******************************************************************************
* 函数名 : CANopenParaInit
* 描  述 : 初始化CANopen用到的所有参数
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void CANopenParaInit(void)
{
    u8 *pStr = 0;
    u16 i = 0;

    u16 temp =0;
    static u16 tempold = 0;

    ExCANopenRxFullFlag = 0;

    for (CANopen_i = 0; CANopen_i < CANopenParaWriteTableSize; CANopen_i++)              //
    {
        *CANopenParaWriteTable[CANopen_i].pCANopenData = ReadPara(CANopenParaWriteTable[CANopen_i].Index);
        CANopenParaWriteTableOld[CANopen_i]            = ReadPara(CANopenParaWriteTable[CANopen_i].Index);
    }

    err_rst = (s8)err_rst_s16;

    /* 32位参数特殊处理 */
    UserPara1 = ((u32)user_para1_h<<16) | (u32)user_para1_l;
    UserPara2 = ((u32)user_para2_h<<16) | (u32)user_para2_l;
    UserPara3 = ((u32)user_para3_h<<16) | (u32)user_para3_l;

    /* 从站型号 ***************************************/
    for (i = 0; i < 10; i++)
    {
        FD_ObjDict_obj1008[i] = 0;
    }
    if (ReadPara(MANUF_DEV_NAME_SEL))
    {
        pStr = "ST-100";
        for (i = 0; i < 6; i++)
        {
            FD_ObjDict_obj1008[i] = pStr[i];
        }
    }
    else
    {
        pStr = "ST-75";
        for (i = 0; i < 5; i++)
        {
            FD_ObjDict_obj1008[i] = pStr[i];
        }
    }
}

/*******************************************************************************
* 函数名 : CANopen_Para_Precoss
* 描  述 :
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void CANopen_Para_Precoss(void)
{
    ReadPara(CONTROLWORD_DISP) = Controlword_Out;
	
	if(User_PLC_Sta == 3)
	{
		ReadPara(TARGET_POSITION_L) = SafeRunPositon;
		ReadPara(TARGET_POSITION_H) = 0;
	}
	else if(User_PLC_Sta == 11)
	{
		//ReadPara(TARGET_POSITION_L) = HMI_Pich_Control_Pset;
		//ReadPara(TARGET_POSITION_H) = HMI_Pich_Control_Pset >> 16;
	}
	else
	{
		ReadPara(TARGET_POSITION_L) = Target_position_Out;
		ReadPara(TARGET_POSITION_H) = Target_position_Out >> 16;
	}
	
	//if (ReadPara(CAN_SEL) == 0)		
	if (CANopen_EN == 0)
    {
        return; 
    }

    if (CANOpen_Comm_Enable == 0)
    {
        if(Init_flag2)
        {
            if (CANopen_EN)
            {
                CANOpen_Comm_Enable = 1;
            }
        }
        return; 
    }

    /* 实时更新读参数 */
    for (CANopen_i = 0; CANopen_i < CANopenParaReadTableSize; CANopen_i++)
    {
        *CANopenParaReadTable[CANopen_i].pCANopenData = ReadPara(CANopenParaReadTable[CANopen_i].Index);
    }

    if (ExCANopenRxFullFlag == 0)         			// 当发送缓存为空时才可更新CANopen参数
    {
        switch (CANopen_Step)
        {
        //更新CANopen写参数到系统参数或从系统更新参数到CANopen
        case 0:
        {
            /* 8位参数特殊处理 读出 */
            co_life_time_factor_u16 = co_life_time_factor;
            co_rpdo1_transmission_type_u16 = co_rpdo1_transmission_type;
            co_rpdo2_transmission_type_u16 = co_rpdo2_transmission_type;
            co_rpdo3_transmission_type_u16 = co_rpdo3_transmission_type;
            co_rpdo4_transmission_type_u16 = co_rpdo4_transmission_type;
            co_tpdo1_transmission_type_u16 = co_tpdo1_transmission_type;
            co_tpdo2_transmission_type_u16 = co_tpdo2_transmission_type;
            co_tpdo3_transmission_type_u16 = co_tpdo3_transmission_type;
            co_tpdo4_transmission_type_u16 = co_tpdo4_transmission_type;
            err_rst_s16 = (s16)err_rst;

            /* 32位参数特殊处理 读出 */
            user_para1_h = (u16)(UserPara1>>16);
            user_para1_l = (u16)(UserPara1 & 0x0000FFFF);
            user_para2_h = (u16)(UserPara2>>16);
            user_para2_l = (u16)(UserPara2 & 0x0000FFFF);
            user_para3_h = (u16)(UserPara3>>16);
            user_para3_l = (u16)(UserPara3 & 0x0000FFFF);

            for (; CANopen_Num < CANopenParaWriteTableSize; CANopen_Num++)              //
            {
                if((u16)(*CANopenParaWriteTable[CANopen_Num].pCANopenData) != (u16)ReadPara(CANopenParaWriteTable[CANopen_Num].Index))       // CANopen的数据与系统数据不同
                {
                    if ((u16)(*CANopenParaWriteTable[CANopen_Num].pCANopenData) == (u16)CANopenParaWriteTableOld[CANopen_Num])        // CANopen的数据与CANopen的旧数据相同
                    {   // 则用系统数据更新CANopen的数据及旧数据
                        *CANopenParaWriteTable[CANopen_Num].pCANopenData = ReadPara(CANopenParaWriteTable[CANopen_Num].Index);
                        CANopenParaWriteTableOld[CANopen_Num]            = ReadPara(CANopenParaWriteTable[CANopen_Num].Index);
                    }
                    else                                                                                                     // 否则用CANopen数据更新系统数据及CANopen旧数据
                    {
                        CANopenParaWriteTableOld[CANopen_Num] = *CANopenParaWriteTable[CANopen_Num].pCANopenData;
                        UpSysPara(0x80, (CANopenParaWriteTable[CANopen_Num].Index), (*CANopenParaWriteTable[CANopen_Num].pCANopenData));     // 发送下传帧
                        CANopen_Num++;
                        break;
                    }
                }
            }

            if (CANopen_Num >= CANopenParaWriteTableSize)
            {
                CANopen_Num = 0;
                CANopen_Step++;
            }

            /* 8位参数特殊处理 写入 */
            co_life_time_factor = (u8)co_life_time_factor_u16;
            co_rpdo1_transmission_type = (u8)co_rpdo1_transmission_type_u16;
            co_rpdo2_transmission_type = (u8)co_rpdo2_transmission_type_u16;
            co_rpdo3_transmission_type = (u8)co_rpdo3_transmission_type_u16;
            co_rpdo4_transmission_type = (u8)co_rpdo4_transmission_type_u16;
            co_tpdo1_transmission_type = (u8)co_tpdo1_transmission_type_u16;
            co_tpdo2_transmission_type = (u8)co_tpdo2_transmission_type_u16;
            co_tpdo3_transmission_type = (u8)co_tpdo3_transmission_type_u16;
            co_tpdo4_transmission_type = (u8)co_tpdo4_transmission_type_u16;
            err_rst = (s8)err_rst_s16;

            /* 32位参数特殊处理 写入 */
            UserPara1 = ((u32)user_para1_h<<16) | (u32)user_para1_l;
            UserPara2 = ((u32)user_para2_h<<16) | (u32)user_para2_l;
            UserPara3 = ((u32)user_para3_h<<16) | (u32)user_para3_l;

            break;
        }

        //实时更新给定指令
        case 1:
        {
            if ((s16)PcSet != (s16)target_value)
            {
                if ((s16)target_value != (s16)target_value_old)    // 发送给定指令
                {
                    target_value_old = target_value;
                    PcSet            = target_value;    // 更新RS485的设定值
                    UpSysPara(0xC0, 1, target_value);   // 发送下传帧
                    CANopen_Step++;
                    break;
                }
                else
                {
                    target_value_old = PcSet;
                    target_value     = PcSet;    // 用RS485的设定值更新CANopen的设定值
                }
            }
            //else if (work_mod == 2)  // 当前工作在速度环
			else if (ReadPara(WORK_MOD) == 2)  // 当前工作在速度环
            {
                if (ReadPara(V_SET)!= (s16)target_value)   // 当前设定值与DSP反馈的设定值不同
                {
                    UpSysPara(0xC0, 1, target_value);   // 发送下传帧
                    CANopen_Step++;
                    break;
                }
            }
            //else if (work_mod == 3)   // 当前工作在转矩环
			else if (ReadPara(WORK_MOD) == 3) 
            {
                if ((s16)t_set != (s16)target_value)   // 当前设定值与DSP反馈的设定值不同
                {
                    UpSysPara(0xC0, 1, target_value);   // 发送下传帧
                    CANopen_Step++;
                    break;
                }
            }
            CANopen_Step++;
            //break;
        }

        case 2:
        {
            if (save_para == 0xA151)                // 发送保存指令
            {
                ReadPara(SAVE_PARA) = save_para;
                save_para = 0;
                UpSysPara(0xC0, 5, 0xA151);         // 发送下传帧
                CANopen_Step++;
                break;
            }
            CANopen_Step++;
            //break;
        }

        case 3:
        {
            if ((PcCtrl == 1) && (PcCtrlFlag == 1))	// ARM已收到上位机开使能指令不再发送开使能指令
            {
                sn_on = 0;
            }
            if (sn_on == 0xAA55)                    // 发送开使能指令
            {
                UpSysPara(0xC0, 3, 0xAA55);         // 发送下传帧
                CANopen_Step++;
                break;
            }
            CANopen_Step++;
            //break;
        }

        case 4:
        {
            if ((PcCtrl == 1) && (PcCtrlFlag == 0))	// ARM已收到上位机关使能指令不再发送关使能指令
            {
                sn_on = 0;
                sn_off = 0;
            }
            if (sn_off == 0x55AA)                   // 发送关使能指令
            {
                UpSysPara(0xC0, 4, 0x55AA);         // 发送下传帧
                CANopen_Step++;
                break;
            }
            CANopen_Step++;
            //break;
        }

        case 5: // 高级命令读写参数
        {
            static u32 RWParaCommOld = 0;
            u32 RWParaDataTemp = 0;
            if (RWParaComm != RWParaCommOld)                   // 发送高级命令
            {
                UpSysPara((u8)((RWParaComm>>24)&0xC0), (u16)((RWParaComm>>16)&0x3FFF), (u16)((RWParaComm)&0xFFFF)); // 发送下传帧
                RWParaCommOld = RWParaComm;
                CANopen_Step++;
                break;
            }
            if (RWParaComm & (1<<30)) // 读返回数据
            {
                RWParaDataTemp = RWParaComm; // 命令
            }
            else
            {
                RWParaDataTemp  = (u32)((u16)ReadPara((u16)((RWParaComm>>16)&0x3FFF)));
                RWParaDataTemp |= (RWParaComm & 0xFFFF0000); // 数据
            }
            RWParaData = RWParaDataTemp;

            RWParaCommOld = RWParaComm;
            CANopen_Step++;
            //break;
        }

        case 6:
        {
            if (esc_remote == 0xAA12)               // 发送脱离远程控制指令
            {
                esc_remote = 0;
                UpSysPara(0xC0, 9, 0xAA12);         // 发送下传帧
                CANopen_Step = 0;
                break;
            }
            CANopen_Step = 0;
            break;
        }

        default:
        {
            CANopen_Step = 0;
            break;
        }
        }
    }
}

/*******************************************************************************
* 函数名 : CANopen_DefectPara_Precoss
* 描  述 :
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void CANopen_DefectPara_Precoss(void)		//CANOPEN故障参数处理，速度不要求快，在主函数大while中执行
{
    static u16 i = 50;

    if(i >= CANopenParaWriteTableSize1)
    {
        i = CANopenParaWriteTableSize1;
    }

    if (canopen_para_precoss_flag)//800ms执行一次
    {
        for (; CANopen_Num1 < CANopenParaWriteTableSize1; CANopen_Num1++)
        {
            if((u16)(*CANopenParaWriteTable1[CANopen_Num1].pCANopenData) != (u16)ReadPara(CANopenParaWriteTable1[CANopen_Num1].Index))       // CANopen的数据与系统数据不同
            {
                if ((u16)(*CANopenParaWriteTable1[CANopen_Num1].pCANopenData) == (u16)CANopenParaWriteTableOld1[CANopen_Num1])        // CANopen的数据与CANopen的旧数据相同
                {   // 则用系统数据更新CANopen的数据及旧数据
                    *CANopenParaWriteTable1[CANopen_Num1].pCANopenData = ReadPara(CANopenParaWriteTable1[CANopen_Num1].Index);
                    CANopenParaWriteTableOld1[CANopen_Num1]            = ReadPara(CANopenParaWriteTable1[CANopen_Num1].Index);
                }
                else                                                                                                     // 否则用CANopen数据更新系统数据及CANopen旧数据
                {
                    CANopenParaWriteTableOld1[CANopen_Num1] = *CANopenParaWriteTable1[CANopen_Num1].pCANopenData;
                    UpSysPara(0x80, (CANopenParaWriteTable1[CANopen_Num1].Index), (*CANopenParaWriteTable1[CANopen_Num1].pCANopenData));     // 发送下传帧
                    CANopen_Num1++;
                    break;
                }
            }
        }

        if (CANopen_Num1 >= CANopenParaWriteTableSize1)
        {
            CANopen_Num1 = 0;
        }

        i = i + 50;		//每800ms刷新50个CANOPEN参数，避免一次刷新太多耗时太久
        canopen_para_precoss_flag = 0;
    }
}