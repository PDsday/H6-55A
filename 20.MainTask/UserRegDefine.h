#ifndef  __USERREGDEFINE_H__
#define  __USERREGDEFINE_H__

extern u8 CANopen_LifeGuard_TimeOut; //CANopen寿命保护超时标志
extern s16 t_set;		/* Mapped at index 0x2006, subindex 0x00*/
extern s16 User_C_Vol;		/* Mapped at index 0x200B, subindex 0x00*/
extern u16 Service_Time_Delay;		/* Mapped at index 0x200D, subindex 0x00*/
extern s32 MotorAngelNew;		/* Mapped at index 0x200E, subindex 0x00*/
extern s16 igbt_temp;		/* Mapped at index 0x2011, subindex 0x00*/
extern s16 moto_temp;		/* Mapped at index 0x2012, subindex 0x00*/
extern u16 Over_I_Total;		/* Mapped at index 0x2016, subindex 0x00*/
extern u16 SpeedOverproofT;		/* Mapped at index 0x2017, subindex 0x00*/
extern u16 v_pi;		/* Mapped at index 0x2019, subindex 0x00*/
extern u16 p_pv;		/* Mapped at index 0x201A, subindex 0x00*/
extern u16 max_i;		/* Mapped at index 0x2024, subindex 0x00*/
extern u16 RatedCurrent;		/* Mapped at index 0x2025, subindex 0x00*/
extern u16 max_t;		/* Mapped at index 0x2026, subindex 0x00*/
extern u16 moto_safe_temp;		/* Mapped at index 0x2027, subindex 0x00*/
extern u16 igbt_safe_temp;		/* Mapped at index 0x2028, subindex 0x00*/
extern u16 work_mod;		/* Mapped at index 0x2033, subindex 0x00*/
extern u16 ModeCtrl;		/* Mapped at index 0x2034, subindex 0x00*/
extern s8  err_rst;		/* Mapped at index 0x2035, subindex 0x00*/
extern u16 motor_pos_rst;		/* Mapped at index 0x2036, subindex 0x00*/
extern u16 BK_Switch;		/* Mapped at index 0x2037, subindex 0x00*/
extern u16 s_up_time;		/* Mapped at index 0x2043, subindex 0x00*/
extern u16 s_down_time;		/* Mapped at index 0x2044, subindex 0x00*/
extern u16 v_dir_mod;		/* Mapped at index 0x2047, subindex 0x00*/
extern u16 limit_t;		/* Mapped at index 0x2050, subindex 0x00*/
extern u16 limit_v;		/* Mapped at index 0x2051, subindex 0x00*/
extern s16 BKOnDelay;		/* Mapped at index 0x2056, subindex 0x00*/
extern s16 BKOffDelay;		/* Mapped at index 0x2057, subindex 0x00*/
extern u16 safe_close_up_time;		/* Mapped at index 0x2059, subindex 0x00*/
extern u16 safe_close_down_time;		/* Mapped at index 0x205A, subindex 0x00*/
extern u16 SecCoderDir;		/* Mapped at index 0x205C, subindex 0x00*/
extern s16 motor_pos_num;		/* Mapped at index 0x2060, subindex 0x00*/
extern u16 motor_pos;		/* Mapped at index 0x2061, subindex 0x00*/
extern s16 sec_pos_num;		/* Mapped at index 0x2063, subindex 0x00*/
extern u16 sec_pos;		/* Mapped at index 0x2064, subindex 0x00*/
extern s32 SecCoderAngle;		/* Mapped at index 0x2065, subindex 0x00*/
extern u16 SafeDI_Dly;		/* Mapped at index 0x2073, subindex 0x00*/
extern u32 UserPara1;		/* Mapped at index 0x2080, subindex 0x00*/
extern u32 UserPara2;		/* Mapped at index 0x2081, subindex 0x00*/
extern u16 DischargeTime;		/* Mapped at index 0x2082, subindex 0x00*/
extern u16 DischargeAccTime;		/* Mapped at index 0x2083, subindex 0x00*/
extern u16 DischargeTimeOutSlope;		/* Mapped at index 0x2084, subindex 0x00*/
extern u16 DischargeTimeThreshold;		/* Mapped at index 0x2085, subindex 0x00*/
extern u32 UserPara3;		/* Mapped at index 0x2086, subindex 0x00*/
extern u16 charg_defect_code;		/* Mapped at index 0x2090, subindex 0x00*/
extern u16 charg_sys_st_code;		/* Mapped at index 0x2091, subindex 0x00*/
extern s16 charg_igbt_temp;		/* Mapped at index 0x2092, subindex 0x00*/
extern u16 charg_vol;		/* Mapped at index 0x2093, subindex 0x00*/
extern u16 charg_v_feedback;		/* Mapped at index 0x2094, subindex 0x00*/
extern s16 charg_i_feedback;		/* Mapped at index 0x2095, subindex 0x00*/
extern u16 charg_v_set;		/* Mapped at index 0x2096, subindex 0x00*/
extern s16 charg_i_set;		/* Mapped at index 0x2097, subindex 0x00*/
extern u16 charg_ctrl_reg;		/* Mapped at index 0x2098, subindex 0x00*/
extern u16 charg_ctrl_mod;		/* Mapped at index 0x2099, subindex 0x00*/
extern u32 Error_code2;		/* Mapped at index 0x2100, subindex 0x00*/
extern u16 IO_Control;		/* Mapped at index 0x2101, subindex 0x00*/
extern u16 DI_Error_OutSide_Choose;		/* Mapped at index 0x2102, subindex 0x00*/
extern s32 EncAUserActPos;		/* Mapped at index 0x3013, subindex 0x00*/
extern s32 EncBUserActPos;		/* Mapped at index 0x3014, subindex 0x00*/
extern u16 sn_on;		/* Mapped at index 0x4003, subindex 0x00*/
extern u16 sn_off;		/* Mapped at index 0x4004, subindex 0x00*/
extern u16 save_para;		/* Mapped at index 0x4005, subindex 0x00*/
extern u32 RWParaComm;		/* Mapped at index 0x4006, subindex 0x00*/
extern u32 RWParaData;		/* Mapped at index 0x4007, subindex 0x00*/
extern u16 esc_remote;		/* Mapped at index 0x4009, subindex 0x00*/

extern u16 Servo_Err_OverLoad_On_Off;		/* Mapped at index 0x5000, subindex 0x01 */
extern u16 Servo_Err_OverLoad_Operating_Mode;		/* Mapped at index 0x5000, subindex 0x02 */
extern u16 Servo_Err_OverLoad_Ena_Delay;		/* Mapped at index 0x5000, subindex 0x03 */
extern u16 Servo_Err_OverLoad_Res_Delay;		/* Mapped at index 0x5000, subindex 0x04 */
extern u16 Servo_Err_OverLoad_Res_Type;		/* Mapped at index 0x5000, subindex 0x05 */
extern u16 Servo_Err_OverLoad_Trigger_Conditions;		/* Mapped at index 0x5000, subindex 0x06 */
extern u16 Servo_Err_OverLoad_Safer_Off;		/* Mapped at index 0x5000, subindex 0x07 */
extern u16 Servo_Err_OverVol_On_Off;		/* Mapped at index 0x5001, subindex 0x01 */
extern u16 Servo_Err_OverVol_Operating_Mode;		/* Mapped at index 0x5001, subindex 0x02 */
extern u16 Servo_Err_OverVol_Ena_Delay;		/* Mapped at index 0x5001, subindex 0x03 */
extern u16 Servo_Err_OverVol_Res_Delay;		/* Mapped at index 0x5001, subindex 0x04 */
extern u16 Servo_Err_OverVol_Res_Type;		/* Mapped at index 0x5001, subindex 0x05 */
extern u16 Servo_Err_OverVol_Trigger_Conditions;		/* Mapped at index 0x5001, subindex 0x06 */
extern u16 Servo_Err_OverVol_Safer_Off;		/* Mapped at index 0x5001, subindex 0x07 */
extern u16 Servo_Err_LowVol_On_Off;		/* Mapped at index 0x5002, subindex 0x01 */
extern u16 Servo_Err_LowVol_Operating_Mode;		/* Mapped at index 0x5002, subindex 0x02 */
extern u16 Servo_Err_LowVol_Ena_Delay;		/* Mapped at index 0x5002, subindex 0x03 */
extern u16 Servo_Err_LowVol_Res_Delay;		/* Mapped at index 0x5002, subindex 0x04 */
extern u16 Servo_Err_LowVol_Res_Type;		/* Mapped at index 0x5002, subindex 0x05 */
extern u16 Servo_Err_LowVol_Trigger_Conditions;		/* Mapped at index 0x5002, subindex 0x06 */
extern u16 Servo_Err_LowVol_Safer_Off;		/* Mapped at index 0x5002, subindex 0x07 */
extern u16 Servo_Err_Outer24VLost_On_Off;		/* Mapped at index 0x5003, subindex 0x01 */
extern u16 Servo_Err_Outer24VLost_Operating_Mode;		/* Mapped at index 0x5003, subindex 0x02 */
extern u16 Servo_Err_Outer24VLost_Ena_Delay;		/* Mapped at index 0x5003, subindex 0x03 */
extern u16 Servo_Err_Outer24VLost_Res_Delay;		/* Mapped at index 0x5003, subindex 0x04 */
extern u16 Servo_Err_Outer24VLost_Res_Type;		/* Mapped at index 0x5003, subindex 0x05 */
extern u16 Servo_Err_Outer24VLost_Trigger_Conditions;		/* Mapped at index 0x5003, subindex 0x06 */
extern u16 Servo_Err_Outer24VLost_Safer_Off;		/* Mapped at index 0x5003, subindex 0x07 */
extern u16 Servo_Err_PosLost_On_Off;		/* Mapped at index 0x5004, subindex 0x01 */
extern u16 Servo_Err_PosLost_Operating_Mode;		/* Mapped at index 0x5004, subindex 0x02 */
extern u16 Servo_Err_PosLost_Ena_Delay;		/* Mapped at index 0x5004, subindex 0x03 */
extern u16 Servo_Err_PosLost_Res_Delay;		/* Mapped at index 0x5004, subindex 0x04 */
extern u16 Servo_Err_PosLost_Res_Type;		/* Mapped at index 0x5004, subindex 0x05 */
extern u16 Servo_Err_PosLost_Trigger_Conditions;		/* Mapped at index 0x5004, subindex 0x06 */
extern u16 Servo_Err_PosLost_Safer_Off;		/* Mapped at index 0x5004, subindex 0x07 */
extern u16 Servo_Err_CANOpenLineOff_On_Off;		/* Mapped at index 0x5005, subindex 0x01 */
extern u16 Servo_Err_CANOpenLineOff_Operating_Mode;		/* Mapped at index 0x5005, subindex 0x02 */
extern u16 Servo_Err_CANOpenLineOff_Ena_Delay;		/* Mapped at index 0x5005, subindex 0x03 */
extern u16 Servo_Err_CANOpenLineOff_Res_Delay;		/* Mapped at index 0x5005, subindex 0x04 */
extern u16 Servo_Err_CANOpenLineOff_Res_Type;		/* Mapped at index 0x5005, subindex 0x05 */
extern u16 Servo_Err_CANOpenLineOff_Trigger_Conditions;		/* Mapped at index 0x5005, subindex 0x06 */
extern u16 Servo_Err_CANOpenLineOff_Safer_Off;		/* Mapped at index 0x5005, subindex 0x07 */
extern u16 Servo_Err_IGBTOverTemper_On_Off;		/* Mapped at index 0x5006, subindex 0x01 */
extern u16 Servo_Err_IGBTOverTemper_Operating_Mode;		/* Mapped at index 0x5006, subindex 0x02 */
extern u16 Servo_Err_IGBTOverTemper_Ena_Delay;		/* Mapped at index 0x5006, subindex 0x03 */
extern u16 Servo_Err_IGBTOverTemper_Res_Delay;		/* Mapped at index 0x5006, subindex 0x04 */
extern u16 Servo_Err_IGBTOverTemper_Res_Type;		/* Mapped at index 0x5006, subindex 0x05 */
extern u16 Servo_Err_IGBTOverTemper_Trigger_Conditions;		/* Mapped at index 0x5006, subindex 0x06 */
extern u16 Servo_Err_IGBTOverTemper_Safer_Off;		/* Mapped at index 0x5006, subindex 0x07 */
extern u16 Servo_Err_SSILineOff_On_Off;		/* Mapped at index 0x5007, subindex 0x01 */
extern u16 Servo_Err_SSILineOff_Operating_Mode;		/* Mapped at index 0x5007, subindex 0x02 */
extern u16 Servo_Err_SSILineOff_Ena_Delay;		/* Mapped at index 0x5007, subindex 0x03 */
extern u16 Servo_Err_SSILineOff_Res_Delay;		/* Mapped at index 0x5007, subindex 0x04 */
extern u16 Servo_Err_SSILineOff_Res_Type;		/* Mapped at index 0x5007, subindex 0x05 */
extern u16 Servo_Err_SSILineOff_Trigger_Conditions;		/* Mapped at index 0x5007, subindex 0x06 */
extern u16 Servo_Err_SSILineOff_Safer_Off;		/* Mapped at index 0x5007, subindex 0x07 */
extern u16 Servo_Err_MotoOverTemper_On_Off;		/* Mapped at index 0x5008, subindex 0x01 */
extern u16 Servo_Err_MotoOverTemper_Operating_Mode;		/* Mapped at index 0x5008, subindex 0x02 */
extern u16 Servo_Err_MotoOverTemper_Ena_Delay;		/* Mapped at index 0x5008, subindex 0x03 */
extern u16 Servo_Err_MotoOverTemper_Res_Delay;		/* Mapped at index 0x5008, subindex 0x04 */
extern u16 Servo_Err_MotoOverTemper_Res_Type;		/* Mapped at index 0x5008, subindex 0x05 */
extern u16 Servo_Err_MotoOverTemper_Trigger_Conditions;		/* Mapped at index 0x5008, subindex 0x06 */
extern u16 Servo_Err_MotoOverTemper_Safer_Off;		/* Mapped at index 0x5008, subindex 0x07 */
extern u16 Servo_Err_OverCurrent_On_Off;		/* Mapped at index 0x5009, subindex 0x01 */
extern u16 Servo_Err_OverCurrent_Operating_Mode;		/* Mapped at index 0x5009, subindex 0x02 */
extern u16 Servo_Err_OverCurrent_Ena_Delay;		/* Mapped at index 0x5009, subindex 0x03 */
extern u16 Servo_Err_OverCurrent_Res_Delay;		/* Mapped at index 0x5009, subindex 0x04 */
extern u16 Servo_Err_OverCurrent_Res_Type;		/* Mapped at index 0x5009, subindex 0x05 */
extern u16 Servo_Err_OverCurrent_Trigger_Conditions;		/* Mapped at index 0x5009, subindex 0x06 */
extern u16 Servo_Err_OverCurrent_Safer_Off;		/* Mapped at index 0x5009, subindex 0x07 */
extern u16 Servo_Err_ShortCircuit_On_Off;		/* Mapped at index 0x500A, subindex 0x01 */
extern u16 Servo_Err_ShortCircuit_Operating_Mode;		/* Mapped at index 0x500A, subindex 0x02 */
extern u16 Servo_Err_ShortCircuit_Ena_Delay;		/* Mapped at index 0x500A, subindex 0x03 */
extern u16 Servo_Err_ShortCircuit_Res_Delay;		/* Mapped at index 0x500A, subindex 0x04 */
extern u16 Servo_Err_ShortCircuit_Res_Type;		/* Mapped at index 0x500A, subindex 0x05 */
extern u16 Servo_Err_ShortCircuit_Trigger_Conditions;		/* Mapped at index 0x500A, subindex 0x06 */
extern u16 Servo_Err_ShortCircuit_Safer_Off;		/* Mapped at index 0x500A, subindex 0x07 */
extern u16 Servo_Err_CurrentOver_On_Off;		/* Mapped at index 0x500B, subindex 0x01 */
extern u16 Servo_Err_CurrentOver_Operating_Mode;		/* Mapped at index 0x500B, subindex 0x02 */
extern u16 Servo_Err_CurrentOver_Ena_Delay;		/* Mapped at index 0x500B, subindex 0x03 */
extern u16 Servo_Err_CurrentOver_Res_Delay;		/* Mapped at index 0x500B, subindex 0x04 */
extern u16 Servo_Err_CurrentOver_Res_Type;		/* Mapped at index 0x500B, subindex 0x05 */
extern u16 Servo_Err_CurrentOver_Trigger_Conditions;		/* Mapped at index 0x500B, subindex 0x06 */
extern u16 Servo_Err_CurrentOver_Safer_Off;		/* Mapped at index 0x500B, subindex 0x07 */
extern u16 Servo_Err_Limit96_On_Off;		/* Mapped at index 0x500C, subindex 0x01 */
extern u16 Servo_Err_Limit96_Operating_Mode;		/* Mapped at index 0x500C, subindex 0x02 */
extern u16 Servo_Err_Limit96_Ena_Delay;		/* Mapped at index 0x500C, subindex 0x03 */
extern u16 Servo_Err_Limit96_Res_Delay;		/* Mapped at index 0x500C, subindex 0x04 */
extern u16 Servo_Err_Limit96_Res_Type;		/* Mapped at index 0x500C, subindex 0x05 */
extern u16 Servo_Err_Limit96_Trigger_Conditions;		/* Mapped at index 0x500C, subindex 0x06 */
extern u16 Servo_Err_Limit96_Safer_Off;		/* Mapped at index 0x500C, subindex 0x07 */
extern u16 Servo_Err_380VErr_On_Off;		/* Mapped at index 0x500D, subindex 0x01 */
extern u16 Servo_Err_380VErr_Operating_Mode;		/* Mapped at index 0x500D, subindex 0x02 */
extern u16 Servo_Err_380VErr_Ena_Delay;		/* Mapped at index 0x500D, subindex 0x03 */
extern u16 Servo_Err_380VErr_Res_Delay;		/* Mapped at index 0x500D, subindex 0x04 */
extern u16 Servo_Err_380VErr_Res_Type;		/* Mapped at index 0x500D, subindex 0x05 */
extern u16 Servo_Err_380VErr_Trigger_Conditions;		/* Mapped at index 0x500D, subindex 0x06 */
extern u16 Servo_Err_380VErr_Copy_of_Safer_Off;		/* Mapped at index 0x500D, subindex 0x07 */
extern u16 Servo_Err_VelocityOver_On_Off;		/* Mapped at index 0x500E, subindex 0x01 */
extern u16 Servo_Err_VelocityOver_Operating_Mode;		/* Mapped at index 0x500E, subindex 0x02 */
extern u16 Servo_Err_VelocityOver_Ena_Delay;		/* Mapped at index 0x500E, subindex 0x03 */
extern u16 Servo_Err_VelocityOver_Res_Delay;		/* Mapped at index 0x500E, subindex 0x04 */
extern u16 Servo_Err_VelocityOver_Res_Type;		/* Mapped at index 0x500E, subindex 0x05 */
extern u16 Servo_Err_VelocityOver_Trigger_Conditions;		/* Mapped at index 0x500E, subindex 0x06 */
extern u16 Servo_Err_VelocityOver_Safer_Off;		/* Mapped at index 0x500E, subindex 0x07 */
extern u16 Servo_Err_DischargeFail_On_Off;		/* Mapped at index 0x500F, subindex 0x01 */
extern u16 Servo_Err_DischargeFail_Operating_Mode;		/* Mapped at index 0x500F, subindex 0x02 */
extern u16 Servo_Err_DischargeFail_Ena_Delay;		/* Mapped at index 0x500F, subindex 0x03 */
extern u16 Servo_Err_DischargeFail_Res_Delay;		/* Mapped at index 0x500F, subindex 0x04 */
extern u16 Servo_Err_DischargeFail_Res_Type;		/* Mapped at index 0x500F, subindex 0x05 */
extern u16 Servo_Err_DischargeFail_Trigger_Conditions;		/* Mapped at index 0x500F, subindex 0x06 */
extern u16 Servo_Err_DischargeFail_Safer_Off;		/* Mapped at index 0x500F, subindex 0x07 */
extern u16 Servo_Err_BK24VLost_On_Off;		/* Mapped at index 0x5010, subindex 0x01 */
extern u16 Servo_Err_BK24VLost_Operating_Mode;		/* Mapped at index 0x5010, subindex 0x02 */
extern u16 Servo_Err_BK24VLost_Ena_Delay;		/* Mapped at index 0x5010, subindex 0x03 */
extern u16 Servo_Err_BK24VLost_Res_Delay;		/* Mapped at index 0x5010, subindex 0x04 */
extern u16 Servo_Err_BK24VLost_Res_Type;		/* Mapped at index 0x5010, subindex 0x05 */
extern u16 Servo_Err_BK24VLost_Trigger_Conditions;		/* Mapped at index 0x5010, subindex 0x06 */
extern u16 Servo_Err_BK24VLost_Safer_Off;		/* Mapped at index 0x5010, subindex 0x07 */
extern u16 Servo_Err_Iner24VLost_On_Off;		/* Mapped at index 0x5011, subindex 0x01 */
extern u16 Servo_Err_Iner24VLost_Operating_Mode;		/* Mapped at index 0x5011, subindex 0x02 */
extern u16 Servo_Err_Iner24VLost_Ena_Delay;		/* Mapped at index 0x5011, subindex 0x03 */
extern u16 Servo_Err_Iner24VLost_Res_Delay;		/* Mapped at index 0x5011, subindex 0x04 */
extern u16 Servo_Err_Iner24VLost_Res_Type;		/* Mapped at index 0x5011, subindex 0x05 */
extern u16 Servo_Err_Iner24VLost_Trigger_Conditions;		/* Mapped at index 0x5011, subindex 0x06 */
extern u16 Servo_Err_Iner24VLost_Safer_Off;		/* Mapped at index 0x5011, subindex 0x07 */
extern u16 Servo_Err_DITrigErr_On_Off;		/* Mapped at index 0x5012, subindex 0x01 */
extern u16 Servo_Err_DITrigErr_Operating_Mode;		/* Mapped at index 0x5012, subindex 0x02 */
extern u16 Servo_Err_DITrigErr_Ena_Delay;		/* Mapped at index 0x5012, subindex 0x03 */
extern u16 Servo_Err_DITrigErr_Res_Delay;		/* Mapped at index 0x5012, subindex 0x04 */
extern u16 Servo_Err_DITrigErr_Res_Type;		/* Mapped at index 0x5012, subindex 0x05 */
extern u16 Servo_Err_DITrigErr_Trigger_Conditions;		/* Mapped at index 0x5012, subindex 0x06 */
extern u16 Servo_Err_DITrigErr_Safer_Off;		/* Mapped at index 0x5012, subindex 0x07 */
extern u16 Servo_Err_SaferErr_On_Off;		/* Mapped at index 0x5013, subindex 0x01 */
extern u16 Servo_Err_SaferErr_Operating_Mode;		/* Mapped at index 0x5013, subindex 0x02 */
extern u16 Servo_Err_SaferErr_Ena_Delay;		/* Mapped at index 0x5013, subindex 0x03 */
extern u16 Servo_Err_SaferErr_Res_Delay;		/* Mapped at index 0x5013, subindex 0x04 */
extern u16 Servo_Err_SaferErr_Res_Type;		/* Mapped at index 0x5013, subindex 0x05 */
extern u16 Servo_Err_SaferErr_Trigger_Conditions;		/* Mapped at index 0x5013, subindex 0x06 */
extern u16 Servo_Err_SaferErr_Safer_Off;		/* Mapped at index 0x5013, subindex 0x07 */
extern u16 Servo_Err_PositionOver_On_Off;		/* Mapped at index 0x5014, subindex 0x01 */
extern u16 Servo_Err_PositionOver_Operating_Mode;		/* Mapped at index 0x5014, subindex 0x02 */
extern u16 Servo_Err_PositionOver_Ena_Delay;		/* Mapped at index 0x5014, subindex 0x03 */
extern u16 Servo_Err_PositionOver_Res_Delay;		/* Mapped at index 0x5014, subindex 0x04 */
extern u16 Servo_Err_PositionOver_Res_Type;		/* Mapped at index 0x5014, subindex 0x05 */
extern u16 Servo_Err_PositionOver_Trigger_Conditions;		/* Mapped at index 0x5014, subindex 0x06 */
extern u16 Servo_Err_PositionOver_Safer_Off;		/* Mapped at index 0x5014, subindex 0x07 */
extern u16 Servo_Err_PaddleOver_On_Off;		/* Mapped at index 0x5015, subindex 0x01 */
extern u16 Servo_Err_PaddleOver_Operating_Mode;		/* Mapped at index 0x5015, subindex 0x02 */
extern u16 Servo_Err_PaddleOver_Ena_Delay;		/* Mapped at index 0x5015, subindex 0x03 */
extern u16 Servo_Err_PaddleOver_Res_Delay;		/* Mapped at index 0x5015, subindex 0x04 */
extern u16 Servo_Err_PaddleOver_Res_Type;		/* Mapped at index 0x5015, subindex 0x05 */
extern u16 Servo_Err_PaddleOver_Trigger_Conditions;		/* Mapped at index 0x5015, subindex 0x06 */
extern u16 Servo_Err_PaddleOver_Safer_Off;		/* Mapped at index 0x5015, subindex 0x07 */
extern u16 Servo_Err_IGBTLineOff_On_Off;		/* Mapped at index 0x5016, subindex 0x01 */
extern u16 Servo_Err_IGBTLineOff_Operating_Mode;		/* Mapped at index 0x5016, subindex 0x02 */
extern u16 Servo_Err_IGBTLineOff_Ena_Delay;		/* Mapped at index 0x5016, subindex 0x03 */
extern u16 Servo_Err_IGBTLineOff_Res_Delay;		/* Mapped at index 0x5016, subindex 0x04 */
extern u16 Servo_Err_IGBTLineOff_Res_Type;		/* Mapped at index 0x5016, subindex 0x05 */
extern u16 Servo_Err_IGBTLineOff_Trigger_Conditions;		/* Mapped at index 0x5016, subindex 0x06 */
extern u16 Servo_Err_IGBTLineOff_Safer_Off;		/* Mapped at index 0x5016, subindex 0x07 */
extern u16 Servo_Err_MotoLineOff_On_Off;		/* Mapped at index 0x5017, subindex 0x01 */
extern u16 Servo_Err_MotoLineOff_Operating_Mode;		/* Mapped at index 0x5017, subindex 0x02 */
extern u16 Servo_Err_MotoLineOff_Ena_Delay;		/* Mapped at index 0x5017, subindex 0x03 */
extern u16 Servo_Err_MotoLineOff_Res_Delay;		/* Mapped at index 0x5017, subindex 0x04 */
extern u16 Servo_Err_MotoLineOff_Res_Type;		/* Mapped at index 0x5017, subindex 0x05 */
extern u16 Servo_Err_MotoLineOff_Trigger_Conditions;		/* Mapped at index 0x5017, subindex 0x06 */
extern u16 Servo_Err_MotoLineOff_Safer_Off;		/* Mapped at index 0x5017, subindex 0x07 */
extern u16 Servo_Err_ChargerComErr_On_Off;		/* Mapped at index 0x5018, subindex 0x01 */
extern u16 Servo_Err_ChargerComErr_Operating_Mode;		/* Mapped at index 0x5018, subindex 0x02 */
extern u16 Servo_Err_ChargerComErr_Ena_Delay;		/* Mapped at index 0x5018, subindex 0x03 */
extern u16 Servo_Err_ChargerComErr_Res_Delay;		/* Mapped at index 0x5018, subindex 0x04 */
extern u16 Servo_Err_ChargerComErr_Res_Type;		/* Mapped at index 0x5018, subindex 0x05 */
extern u16 Servo_Err_ChargerComErr_Trigger_Conditions;		/* Mapped at index 0x5018, subindex 0x06 */
extern u16 Servo_Err_ChargerComErr_Safer_Off;		/* Mapped at index 0x5018, subindex 0x07 */
extern u16 Servo_Err_NULL1_On_Off;		/* Mapped at index 0x5019, subindex 0x01 */
extern u16 Servo_Err_NULL1_Operating_Mode;		/* Mapped at index 0x5019, subindex 0x02 */
extern u16 Servo_Err_NULL1_Ena_Delay;		/* Mapped at index 0x5019, subindex 0x03 */
extern u16 Servo_Err_NULL1_Res_Delay;		/* Mapped at index 0x5019, subindex 0x04 */
extern u16 Servo_Err_NULL1_Res_Type;		/* Mapped at index 0x5019, subindex 0x05 */
extern u16 Servo_Err_NULL1_Trigger_Conditions;		/* Mapped at index 0x5019, subindex 0x06 */
extern u16 Servo_Err_NULL1_Safer_Off;		/* Mapped at index 0x5019, subindex 0x07 */
extern u16 Servo_Err_NULL2_On_Off;		/* Mapped at index 0x501A, subindex 0x01 */
extern u16 Servo_Err_NULL2_Operating_Mode;		/* Mapped at index 0x501A, subindex 0x02 */
extern u16 Servo_Err_NULL2_Ena_Delay;		/* Mapped at index 0x501A, subindex 0x03 */
extern u16 Servo_Err_NULL2_Res_Delay;		/* Mapped at index 0x501A, subindex 0x04 */
extern u16 Servo_Err_NULL2_Res_Type;		/* Mapped at index 0x501A, subindex 0x05 */
extern u16 Servo_Err_NULL2_Trigger_Conditions;		/* Mapped at index 0x501A, subindex 0x06 */
extern u16 Servo_Err_NULL2_Safer_Off;		/* Mapped at index 0x501A, subindex 0x07 */
extern u16 Servo_Err_NULL3_On_Off;		/* Mapped at index 0x501B, subindex 0x01 */
extern u16 Servo_Err_NULL3_Operating_Mode;		/* Mapped at index 0x501B, subindex 0x02 */
extern u16 Servo_Err_NULL3_Ena_Delay;		/* Mapped at index 0x501B, subindex 0x03 */
extern u16 Servo_Err_NULL3_Res_Delay;		/* Mapped at index 0x501B, subindex 0x04 */
extern u16 Servo_Err_NULL3_Res_Type;		/* Mapped at index 0x501B, subindex 0x05 */
extern u16 Servo_Err_NULL3_Trigger_Conditions;		/* Mapped at index 0x501B, subindex 0x06 */
extern u16 Servo_Err_NULL3_Safer_Off;		/* Mapped at index 0x501B, subindex 0x07 */
extern u16 Servo_Err_NULL4_On_Off;		/* Mapped at index 0x501C, subindex 0x01 */
extern u16 Servo_Err_NULL4_Operating_Mode;		/* Mapped at index 0x501C, subindex 0x02 */
extern u16 Servo_Err_NULL4_Ena_Delay;		/* Mapped at index 0x501C, subindex 0x03 */
extern u16 Servo_Err_NULL4_Res_Delay;		/* Mapped at index 0x501C, subindex 0x04 */
extern u16 Servo_Err_NULL4_Res_Type;		/* Mapped at index 0x501C, subindex 0x05 */
extern u16 Servo_Err_NULL4_Trigger_Conditions;		/* Mapped at index 0x501C, subindex 0x06 */
extern u16 Servo_Err_NULL4_Safer_Off;		/* Mapped at index 0x501C, subindex 0x07 */
extern u16 Servo_Err_NULL5_On_Off;		/* Mapped at index 0x501D, subindex 0x01 */
extern u16 Servo_Err_NULL5_Operating_Mode;		/* Mapped at index 0x501D, subindex 0x02 */
extern u16 Servo_Err_NULL5_Ena_Delay;		/* Mapped at index 0x501D, subindex 0x03 */
extern u16 Servo_Err_NULL5_Res_Delay;		/* Mapped at index 0x501D, subindex 0x04 */
extern u16 Servo_Err_NULL5_Res_Type;		/* Mapped at index 0x501D, subindex 0x05 */
extern u16 Servo_Err_NULL5_Trigger_Conditions;		/* Mapped at index 0x501D, subindex 0x06 */
extern u16 Servo_Err_NULL5_Safer_Off;		/* Mapped at index 0x501D, subindex 0x07 */
extern u16 Servo_Err_NULL6_On_Off;		/* Mapped at index 0x501E, subindex 0x01 */
extern u16 Servo_Err_NULL6_Operating_Mode;		/* Mapped at index 0x501E, subindex 0x02 */
extern u16 Servo_Err_NULL6_Ena_Delay;		/* Mapped at index 0x501E, subindex 0x03 */
extern u16 Servo_Err_NULL6_Res_Delay;		/* Mapped at index 0x501E, subindex 0x04 */
extern u16 Servo_Err_NULL6_Res_Type;		/* Mapped at index 0x501E, subindex 0x05 */
extern u16 Servo_Err_NULL6_Trigger_Conditions;		/* Mapped at index 0x501E, subindex 0x06 */
extern u16 Servo_Err_NULL6_Safer_Off;		/* Mapped at index 0x501E, subindex 0x07 */
extern u16 Servo_Err_NULL7_On_Off;		/* Mapped at index 0x501F, subindex 0x01 */
extern u16 Servo_Err_NULL7_Operating_Mode;		/* Mapped at index 0x501F, subindex 0x02 */
extern u16 Servo_Err_NULL7_Ena_Delay;		/* Mapped at index 0x501F, subindex 0x03 */
extern u16 Servo_Err_NULL7_Res_Delay;		/* Mapped at index 0x501F, subindex 0x04 */
extern u16 Servo_Err_NULL7_Res_Type;		/* Mapped at index 0x501F, subindex 0x05 */
extern u16 Servo_Err_NULL7_Trigger_Conditions;		/* Mapped at index 0x501F, subindex 0x06 */
extern u16 Servo_Err_NULL7_Safer_Off;		/* Mapped at index 0x501F, subindex 0x07 */
extern u16 Charger_Err_Total_On_Off;		/* Mapped at index 0x5020, subindex 0x01 */
extern u16 Charger_Err_Total_Operating_Mode;		/* Mapped at index 0x5020, subindex 0x02 */
extern u16 Charger_Err_Total_Ena_Delay;		/* Mapped at index 0x5020, subindex 0x03 */
extern u16 Charger_Err_Total_Res_Delay;		/* Mapped at index 0x5020, subindex 0x04 */
extern u16 Charger_Err_Total_Res_Type;		/* Mapped at index 0x5020, subindex 0x05 */
extern u16 Charger_Err_Total_Trigger_Conditions;		/* Mapped at index 0x5020, subindex 0x06 */
extern u16 Charger_Err_Total_Safer_Off;		/* Mapped at index 0x5020, subindex 0x07 */
extern u16 Charger_Err_SoftOverVol_On_Off;		/* Mapped at index 0x5021, subindex 0x01 */
extern u16 Charger_Err_SoftOverVol_Operating_Mode;		/* Mapped at index 0x5021, subindex 0x02 */
extern u16 Charger_Err_SoftOverVol_Ena_Delay;		/* Mapped at index 0x5021, subindex 0x03 */
extern u16 Charger_Err_SoftOverVol_Res_Delay;		/* Mapped at index 0x5021, subindex 0x04 */
extern u16 Charger_Err_SoftOverVol_Res_Type;		/* Mapped at index 0x5021, subindex 0x05 */
extern u16 Charger_Err_SoftOverVol_Trigger_Conditions;		/* Mapped at index 0x5021, subindex 0x06 */
extern u16 Charger_Err_SoftOverVol_Safer_Off;		/* Mapped at index 0x5021, subindex 0x07 */
extern u16 Charger_Err_SoftOverCurrent_On_Off;		/* Mapped at index 0x5022, subindex 0x01 */
extern u16 Charger_Err_SoftOverCurrent_Operating_Mode;		/* Mapped at index 0x5022, subindex 0x02 */
extern u16 Charger_Err_SoftOverCurrent_Ena_Delay;		/* Mapped at index 0x5022, subindex 0x03 */
extern u16 Charger_Err_SoftOverCurrent_Res_Delay;		/* Mapped at index 0x5022, subindex 0x04 */
extern u16 Charger_Err_SoftOverCurrent_Res_Type;		/* Mapped at index 0x5022, subindex 0x05 */
extern u16 Charger_Err_SoftOverCurrent_Trigger_Conditions;		/* Mapped at index 0x5022, subindex 0x06 */
extern u16 Charger_Err_SoftOverCurrent_Safer_Off;		/* Mapped at index 0x5022, subindex 0x07 */
extern u16 Charger_Err_ShortCircuit_On_Off;		/* Mapped at index 0x5023, subindex 0x01 */
extern u16 Charger_Err_ShortCircuit_Operating_Mode;		/* Mapped at index 0x5023, subindex 0x02 */
extern u16 Charger_Err_ShortCircuit_Ena_Delay;		/* Mapped at index 0x5023, subindex 0x03 */
extern u16 Charger_Err_ShortCircuit_Res_Delay;		/* Mapped at index 0x5023, subindex 0x04 */
extern u16 Charger_Err_ShortCircuit_Res_Type;		/* Mapped at index 0x5023, subindex 0x05 */
extern u16 Charger_Err_ShortCircuit_Trigger_Conditions;		/* Mapped at index 0x5023, subindex 0x06 */
extern u16 Charger_Err_ShortCircuit_Safer_Off;		/* Mapped at index 0x5023, subindex 0x07 */
extern u16 Charger_Err_BrokenCircuit_On_Off;		/* Mapped at index 0x5024, subindex 0x01 */
extern u16 Charger_Err_BrokenCircuit_Operating_Mode;		/* Mapped at index 0x5024, subindex 0x02 */
extern u16 Charger_Err_BrokenCircuit_Ena_Delay;		/* Mapped at index 0x5024, subindex 0x03 */
extern u16 Charger_Err_BrokenCircuit_Res_Delay;		/* Mapped at index 0x5024, subindex 0x04 */
extern u16 Charger_Err_BrokenCircuit_Res_Type;		/* Mapped at index 0x5024, subindex 0x05 */
extern u16 Charger_Err_BrokenCircuit_Trigger_Conditions;		/* Mapped at index 0x5024, subindex 0x06 */
extern u16 Charger_Err_BrokenCircuit_Safer_Off;		/* Mapped at index 0x5024, subindex 0x07 */
extern u16 Charger_Err_DYErr_On_Off;		/* Mapped at index 0x5025, subindex 0x01 */
extern u16 Charger_Err_DYErr_Operating_Mode;		/* Mapped at index 0x5025, subindex 0x02 */
extern u16 Charger_Err_DYErr_Ena_Delay;		/* Mapped at index 0x5025, subindex 0x03 */
extern u16 Charger_Err_DYErr_Res_Delay;		/* Mapped at index 0x5025, subindex 0x04 */
extern u16 Charger_Err_DYErr_Res_Type;		/* Mapped at index 0x5025, subindex 0x05 */
extern u16 Charger_Err_DYErr_Trigger_Conditions;		/* Mapped at index 0x5025, subindex 0x06 */
extern u16 Charger_Err_DYErr_Safer_Off;		/* Mapped at index 0x5025, subindex 0x07 */
extern u16 Charger_Err_LowVol_On_Off;		/* Mapped at index 0x5026, subindex 0x01 */
extern u16 Charger_Err_LowVol_Operating_Mode;		/* Mapped at index 0x5026, subindex 0x02 */
extern u16 Charger_Err_LowVol_Ena_Delay;		/* Mapped at index 0x5026, subindex 0x03 */
extern u16 Charger_Err_LowVol_Res_Delay;		/* Mapped at index 0x5026, subindex 0x04 */
extern u16 Charger_Err_LowVol_Res_Type;		/* Mapped at index 0x5026, subindex 0x05 */
extern u16 Charger_Err_LowVol_Trigger_Conditions;		/* Mapped at index 0x5026, subindex 0x06 */
extern u16 Charger_Err_LowVol_Safer_Off;		/* Mapped at index 0x5026, subindex 0x07 */
extern u16 Charger_Err_IGBTOverTemper_On_Off;		/* Mapped at index 0x5027, subindex 0x01 */
extern u16 Charger_Err_IGBTOverTemper_Operating_Mode;		/* Mapped at index 0x5027, subindex 0x02 */
extern u16 Charger_Err_IGBTOverTemper_Ena_Delay;		/* Mapped at index 0x5027, subindex 0x03 */
extern u16 Charger_Err_IGBTOverTemper_Res_Delay;		/* Mapped at index 0x5027, subindex 0x04 */
extern u16 Charger_Err_IGBTOverTemper_Res_Type;		/* Mapped at index 0x5027, subindex 0x05 */
extern u16 Charger_Err_IGBTOverTemper_Trigger_Conditions;		/* Mapped at index 0x5027, subindex 0x06 */
extern u16 Charger_Err_IGBTOverTemper_Safer_Off;		/* Mapped at index 0x5027, subindex 0x07 */
extern u16 Charger_Err_HardOverVol_On_Off;		/* Mapped at index 0x5028, subindex 0x01 */
extern u16 Charger_Err_HardOverVol_Operating_Mode;		/* Mapped at index 0x5028, subindex 0x02 */
extern u16 Charger_Err_HardOverVol_Ena_Delay;		/* Mapped at index 0x5028, subindex 0x03 */
extern u16 Charger_Err_HardOverVol_Res_Delay;		/* Mapped at index 0x5028, subindex 0x04 */
extern u16 Charger_Err_HardOverVol_Res_Type;		/* Mapped at index 0x5028, subindex 0x05 */
extern u16 Charger_Err_HardOverVol_Trigger_Conditions;		/* Mapped at index 0x5028, subindex 0x06 */
extern u16 Charger_Err_HardOverVol_Safer_Off;		/* Mapped at index 0x5028, subindex 0x07 */
extern u16 Charger_Err_HardOverCurrent_On_Off;		/* Mapped at index 0x5029, subindex 0x01 */
extern u16 Charger_Err_HardOverCurrent_Operating_Mode;		/* Mapped at index 0x5029, subindex 0x02 */
extern u16 Charger_Err_HardOverCurrent_Ena_Delay;		/* Mapped at index 0x5029, subindex 0x03 */
extern u16 Charger_Err_HardOverCurrent_Res_Delay;		/* Mapped at index 0x5029, subindex 0x04 */
extern u16 Charger_Err_HardOverCurrent_Res_Type;		/* Mapped at index 0x5029, subindex 0x05 */
extern u16 Charger_Err_HardOverCurrent_Trigger_Conditions;		/* Mapped at index 0x5029, subindex 0x06 */
extern u16 Charger_Err_HardOverCurrent_Safer_Off;		/* Mapped at index 0x5029, subindex 0x07 */
extern u16 Charger_Err_HardOverCurrent1_On_Off;		/* Mapped at index 0x5030, subindex 0x01 */
extern u16 Charger_Err_HardOverCurrent1_Operating_Mode;		/* Mapped at index 0x5030, subindex 0x02 */
extern u16 Charger_Err_HardOverCurrent1_Ena_Delay;		/* Mapped at index 0x5030, subindex 0x03 */
extern u16 Charger_Err_HardOverCurrent1_Res_Delay;		/* Mapped at index 0x5030, subindex 0x04 */
extern u16 Charger_Err_HardOverCurrent1_Res_Type;		/* Mapped at index 0x5030, subindex 0x05 */
extern u16 Charger_Err_HardOverCurrent1_Trigger_Conditions;		/* Mapped at index 0x5030, subindex 0x06 */
extern u16 Charger_Err_HardOverCurrent1_Safer_Off;		/* Mapped at index 0x5030, subindex 0x07 */
extern u16 Charger_Err_OverVol_On_Off;		/* Mapped at index 0x5031, subindex 0x01 */
extern u16 Charger_Err_OverVol_Operating_Mode;		/* Mapped at index 0x5031, subindex 0x02 */
extern u16 Charger_Err_OverVol_Ena_Delay;		/* Mapped at index 0x5031, subindex 0x03 */
extern u16 Charger_Err_OverVol_Res_Delay;		/* Mapped at index 0x5031, subindex 0x04 */
extern u16 Charger_Err_OverVol_Res_Type;		/* Mapped at index 0x5031, subindex 0x05 */
extern u16 Charger_Err_OverVol_Trigger_Conditions;		/* Mapped at index 0x5031, subindex 0x06 */
extern u16 Charger_Err_OverVol_Safer_Off;		/* Mapped at index 0x5031, subindex 0x07 */

extern u32 Error_code1;		/* Mapped at index 0x603F, subindex 0x00*/
extern u16 Controlword;		/* Mapped at index 0x6040, subindex 0x00*/
extern u16 Statusword;		/* Mapped at index 0x6041, subindex 0x00*/
extern u16 vl_ramp_function_time;		/* Mapped at index 0x604F, subindex 0x00*/
extern u16 vl_slow_down_time;		/* Mapped at index 0x6050, subindex 0x00*/
extern s8 Modes_of__operation;		/* Mapped at index 0x6060, subindex 0x00*/
extern s16 Modes_of_operation_display;		/* Mapped at index 0x6061, subindex 0x00*/
extern long Position_actual_value;		/* Mapped at index 0x6064, subindex 0x00*/
extern long Velocity_sensor_actual_value;		/* Mapped at index 0x6069, subindex 0x00*/
extern long Velocity_demand_value;		/* Mapped at index 0x606B, subindex 0x00*/
extern long Velocity_actual_value;		/* Mapped at index 0x606C, subindex 0x00*/
extern s16 Current_actual_value;		/* Mapped at index 0x6078, subindex 0x00*/
extern u16 DC_link_circuit_voltage;		/* Mapped at index 0x6079, subindex 0x00*/
extern s32 Target_Position;		/* Mapped at index 0x607A, subindex 0x00*/
extern u16 Max_motor_speed;		/* Mapped at index 0x6080, subindex 0x00*/
extern s16 Profile_velocity;		/* Mapped at index 0x6081, subindex 0x00*/
extern u16 Profile_acceleration;		/* Mapped at index 0x6083, subindex 0x00*/
extern u16 Profile_deceleration;		/* Mapped at index 0x6084, subindex 0x00*/
extern s16 Torque_profile_type;		/* Mapped at index 0x6088, subindex 0x00*/
extern s8  Acceleration_notation_index;		/* Mapped at index 0x608D, subindex 0x00*/
extern u8 Acceleration_dimension_index;		/* Mapped at index 0x608E, subindex 0x00*/
extern u16 Position_encoder_resolution_Encoder_increments_a;		/* Mapped at index 0x608F, subindex 0x01 */
extern u16 Position_encoder_resolution_Encoder_increments_b;		/* Mapped at index 0x608F, subindex 0x02 */
extern u16 Gear_ratio_Motor_revolutions_a;		/* Mapped at index 0x6091, subindex 0x01 */
extern u16 Gear_ratio_Motor_revolutions_b;		/* Mapped at index 0x6091, subindex 0x02 */
extern u16 Homing_speeds;		/* Mapped at index 0x6099, subindex 0x00*/
extern u16 Velocity_control_parameter_set_Gain;		/* Mapped at index 0x60F9, subindex 0x01 */
extern u8 Velocity_control_parameter_null2;		/* Mapped at index 0x60F9, subindex 0x02 */
extern u16 Velocity_control_parameter_null3;		/* Mapped at index 0x60F9, subindex 0x03 */
extern s16 Velocity_control_parameter_null4;		/* Mapped at index 0x60F9, subindex 0x04 */
extern u16 Velocity_control_parameter_v_fiterNum;		/* Mapped at index 0x60F9, subindex 0x05 */
extern u16 Position_control_parameter_set_manufacturer_specific1;		/* Mapped at index 0x60FB, subindex 0x01 */
extern u16 Position_control_parameter_set_manufacturer_specific2;		/* Mapped at index 0x60FB, subindex 0x02 */
extern u16 Position_control_parameter_set_manufacturer_specific3;		/* Mapped at index 0x60FB, subindex 0x03 */
extern u16 Position_control_parameter_set_manufacturer_specific4;		/* Mapped at index 0x60FB, subindex 0x04 */
extern u16 Position_control_parameter_set_manufacturer_specific5;		/* Mapped at index 0x60FB, subindex 0x05 */
extern long Target_velocity;		/* Mapped at index 0x60FF, subindex 0x00*/
extern u16 Motor_type;		/* Mapped at index 0x6402, subindex 0x00*/
extern u16 defect_code;    /* 告警码 Mapped at index 0x2003, subindex 0x00*/
extern u16 sys_st_code;    /* 系统状态 Mapped at index 0x2004, subindex 0x00*/
extern s16 t_set;          /* 转矩设定 Mapped at index 0x2006, subindex 0x00*/  // t_set    = 预设定扭矩 * 0x6c00 / max_t;
extern s16 v_set;          /* 速度设定 Mapped at index 0x2007, subindex 0x00*/  // v_set    = 预设定RPM  * 0x6c00 / max_v;
extern s16 i_feedback;     /* 电流反馈 Mapped at index 0x2008, subindex 0x00*/  // 反馈电流 = i_feedback * max_i  / 0x6c00;
extern s16 moto_temp;      /* 电机温度 Mapped at index 0x2012, subindex 0x00*/
extern u16 igbt_safe_temp;  // IGBT过热温度保护点
extern u16 s_up_time;       // S曲线上升时间 单位10ms	
extern u16 s_down_time;     // S曲线下降时间 单位10ms
extern u16 v_up_time;      /* 速度环上升梯度延时 Mapped at index 0x2045, subindex 0x00*/ // 单位10ms
extern u16 v_down_time;    /* 速度环下降梯度延时 Mapped at index 0x2046, subindex 0x00*/ // 单位10ms
extern u16 limit_t;         // 转矩限制值 0x6C00对应最大电流输出 limit_t = 预设定限制扭矩 * 0x6c00 / max_t;
extern u16 max_v;          /* 最高转速 Mapped at index 0x2023, subindex 0x00*/ // 单位：rpm
extern u16 max_i;          /* 最大电流 Mapped at index 0x2024, subindex 0x00*/ // 单位：A
extern u16 max_t;          // 最大扭矩 单位：牛米
extern u16 moto_safe_temp; /* 电机过热温度保护点 Mapped at index 0x2027, subindex 0x00*/
extern u16 work_mod;       /* 工作模式选择 Mapped at index 0x2033, subindex 0x00*/
extern u16 v_dir_mod;      /* 速度环电机方向选择 Mapped at index 0x2047, subindex 0x00*/
extern s16 target_value;   /* 给定指令 Mapped at index 0x4001, subindex 0x00*/
extern s32 UserTargetPosition; // 用户32位位置给定
extern u16 sn_on;          /* 开使能指令 Mapped at index 0x4003, subindex 0x00*/   // 
extern u16 sn_off;         /* 关使能指令 Mapped at index 0x4004, subindex 0x00*/   //
extern u16 save_para;      /* 保存参数指令 Mapped at index 0x4005, subindex 0x00*/ //   
extern u8  CANOpenOffLineFlag;//离线标志

extern u32 IO_InData; // [Bit31:Bit0]
extern u16 IO_OutData;// [Bit15:Bit0]
extern bool ID00;	  // 
extern bool ID01;	  // 
extern bool ID02;	  // 
extern bool ID03;	  // 
extern bool ID04;	  // 
extern bool ID05;	  // 
extern bool ID06;	  // 
extern bool ID07;	  //   
extern bool ID08;	  // 
extern bool ID09;	  // 
extern bool ID10;	  // 
extern bool ID11;	  // 
extern bool ID12;	  // 
extern bool ID13;	  // 
extern bool ID14;	  // 
extern bool ID15;	  // 
extern bool ID16;	  // 
extern bool ID17;	  // 
extern bool ID18;	  // 
extern bool ID19;	  // 
extern bool ID20;	  // 
extern bool ID21;	  // 
extern bool ID22;	  // 
extern bool ID23;	  // 
extern bool ID24;	  // 
extern bool ID25;	  // 
extern bool ID26;	  // 
extern bool ID27;	  // 
extern bool ID28;	  // 
extern bool ID29;	  // 
extern bool ID30;	  // 
extern bool ID31;	  // 
extern bool OD0;	  // 
extern bool OD1;	  // 
extern bool OD2;	  // 
extern bool OD3;	  // 
extern bool OD4;	  // 
extern bool OD5;	  // 
extern bool OD6;	  // 
extern bool OD7;	  //
extern bool OD8;	  //
extern bool OD9;	  //
extern bool OD10;	  //
extern bool OD11;	  //
extern bool OD12;	  //
extern bool OD13;	  //
extern bool OD14;	  //
extern bool OD15;	  //
extern bool BK;	  	  // 
extern bool SAFE; 	  // 
extern bool SafeRunErrFlag; // 安全收桨故障标志：1有安全收桨故障;
extern bool StopRunErrFlag; //减速停机标志：1有安全收桨故障;
extern bool NormRunErrFlag; // 安全收桨故障标志：1有安全收桨故障;
extern bool SnStopErrFlag; // 封锁脉冲关使能故障标志：1有安全收桨故障;
extern bool OpenRunErrFlag; // 封锁脉冲关使能故障标志：1有安全收桨故障;			
extern s32 PosReg;	   // 位置反馈：- 2147483648 ~ 2147483647	高16位为圈数
extern s32 SSI_PosReg;           // 圈数*65536 + 位置
extern s16 CabinetTemp0;        // 柜体温度T0
extern s16 CabinetTemp1;        // 柜体温度T1   
extern s16 CabinetTemp2;        // 柜体温度T2   
extern s16 CabinetTemp3;        // 柜体温度T3
extern s16 CabinetTemp4;        // 柜体温度T4
extern bool DefectCodeClear;    // 1: 清除故障告警命令  0：解除清除命令
extern u16 limit_v;         // 位置环的速度限制值
extern u16 scale_a;         // 位置设定传递比分子
extern u16 scale_b;         // 位置设定传递比分母
extern u16 SSI_scale_a;         // 绝对位置反馈传递比分子
extern u16 SSI_scale_b;         // 绝对位置反馈传递比分母
extern u16 ModeCtrl; // 工作模式控制字
extern u16 RatedCurrent; // 额定电流
extern s16 BKOnDelay; //制动器开延时
extern s16 BKOffDelay; //制动器关延时
extern u16 gray_en; //	格雷码解码允许
extern s16 sec_pos_num; //   第二编码器圈数
extern u16 sec_pos; //	第二编码器位置
extern u16 safe_close_speed; //	安全收桨速度
extern u16 safe_close_up_time; //	安全收桨上升梯度延时 1 = 10ms
extern u16 safe_close_down_time; //	安全收桨下降梯度延时 1 = 10ms
extern u16 motor_pos_rst; //	电机位置复位
extern s16 SecCoderOffset; //	第二编码器圈数偏移
extern u16 SecCoderDir; //	第二编码器计数方向选择：0逆时针加计数，1顺时针加计数
extern u16 BK_Switch; // 制动器开关：0制动器与使能联动；1制动器关，2制动器开
extern u16 CANopen_EN; // 只读 1:CANopen允许
extern u16 Over_I_Total; // 超载量
extern u16 SpeedOverproofT; // 速度超差时间：1对应0.1秒
extern u16 PitchPLCState; /* 驱动器状态机状态 */
extern u16 open_close_speed; //	开环收桨速度
extern u16 Service_Time_Delay;		/* Mapped at index 0x200D, subindex 0x00 */

#endif