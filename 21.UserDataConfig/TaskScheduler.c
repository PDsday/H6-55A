#include "ExternPrototype.h"
#include "TaskScheduler.h"
#include "TimeBase.h"
#include "math.h"
#include "InterUart.H"
#include "sdio_sdcard.h"
#include "sdcard.h"
#include "AppFunc.h"
#include "stm32f4xx_i2c.h"

union DEFECT_FLAGL DefectFlagInL;				//L故障输入(DSP)
union DEFECT_FLAGL DefectFlagOutL;				//L故障输出
union DEFECT_FLAGH DefectFlagInH;				//H故障输入(ARM)
union DEFECT_FLAGH DefectFlagOutH;				//H故障输出
union DEFECT_FLAGH DefectFlagH;					//H故障输出
union DEFECT_FLAGH DefectCodeH;					//H16位故障码最终输出
union DEFECT_FLAGM DefectFlagInM;				//M故障输入(ARM)
union DEFECT_FLAGM DefectFlagOutM;				//M故障输出
union DEFECT_FLAGM DefectFlagM;					//M故障输出
union DEFECT_FLAGM DefectCodeM;					//M16位故障码最终输出
union DEFECT_FLAGH ChargeDefectFlagIn;			//C故障输入(CHG)
union DEFECT_FLAGL ChargeDefectFlagOut;			//C故障输出

bool ARM_Flash_Fault = 0;
bool DO8_OverLoad_Flag = 0;
bool OverPosition = 0;							//桨叶位置累加超限
bool RST_GY=0;     								//电网过压
bool RST_QY=0;     								//电网欠压
bool RefCheckFinish = 0;						//校准完成置1
bool TimeBase_3Secon_ForDO8 = 0;
bool TimeBase_5Secon_ForDO8 = 0;
bool TimeBase_3Secon_ForFan0Err = 0;
bool TimeBase_3Secon_ForFan1Err = 0;
bool I2C_Err_Flag = 0;

u16 RefCheckFinish_Cnt = 0;						//校准完成置1时间
u16 Display_Cnt = 0;
u8 LED_Flash_Flag = 0;
u8 ERR_Flash_Flag = 0;
u8 MEN_Flash_Flag = 0;
u8 ChargeSaveFlag = 0;							//充电器保存标志
u16 SaveParaCnt = 3000;							//3s后才响应用户的保存命令
u8 Moto_Pos_Rst_Finish = 0;
s32 ENC_HomingPos = 0;
s16 Position_actual_value_s16 = 0;
s32 Position_actual_value_s32 = 0;
s32 MON_ActValues_I16_Actual_position = 0x0;
s32 Velocity_sensor_actual_value_temp = 0;
u16 DO8_OverLoad_Cnt = 0;
u16 DefectFlagInH_Delay[16] = {0};
u16 DefectFlagInM_Delay[16] = {0};
u16 DefectFlagInM_Delay_Out[16] = {0};			//故障M退出延时
s32 RealAngle = 0x0;
s32 Angle_Old = 0x0;
s32 Angle = 0x0;
s16 DspSetSpeed = 0;
s32 MotoRpmSpeed = 0;
u64 ServerFault = 0;
u16 ServerFault1 = 0;
u16 ServerFault2 = 0;
u16 ServerFault3 = 0;
u16 ServerFault4 = 0;
u16 ServerFault5 = 0;
s16 Pt100Temp[5] = 0;
u16 Pt100Channel = 0;
u16 GyroTime = 0;

extern s32 PosReg;
extern s16 PositionNum;
extern u16 Position;
extern u8 Init_flag;
extern u8 InUartErrFlag;
extern s16 Current_actual_value;
extern u8 Init_flag;
extern u8 Init_flag1;
extern s16 FastVol;
extern s32 MotorPowerOffset;
extern u8 MotorRstMode;
extern u8 motor_pos_rst0;
extern u8 motor_pos_rst_sel;
extern UNS16 DC_link_circuit_voltage;
extern UNS16 UserPara5;
extern UNS16 charg_sys_st_code;
extern u16 DSPErrCode;
extern u16 ChargeRunEnable;
extern u16 SaveMode;
extern u8 err_rst1;
extern u8 Init_flag1;
extern u8 Init_flag2;
extern u8 DSP_Status;
extern u16 CrcPassFlag;
extern DefectConfigType DefectConfigParaData[];
extern DefectConfigType DefectConfigParaID[41];
extern u8 Init_flag3;
extern u16 MotoOffLineFlag;
extern u8 InUart2ErrFlag;
extern u16 Func_0x03_Data[];
extern u16 Func_0x06_Data[];
extern u16 Func_0x10_Data[];
extern u32 SysParaBuff[];
extern u32 EncryptParaBuff[];
extern u32 SysParaBuffBack[];
extern u32 EncryptParaBuffBack[];
extern u8 DataDiffFlag;
extern u16 FlashValifyErr;
extern u16 VsetTemp;
extern u16 VfebTemp;
extern bool Fan0ErrFlag;
extern bool Fan1ErrFlag;
extern bool Zero_Old;
extern bool gPosClear;
extern bool PWR1;
extern union DI_STATUS DIStatus;

/*******************************************************************************
* 函数名 : 旋变角度掉电存盘、上电读出、校准处理
* 描  述 :
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void MotorAngleDo(void)
{
    static s32 ENC_HomingPosOld = 55555;

    if (Init_flag1 == 0)
    {
        return;
    }

    if(ReadPara(MOTO_ANGLE_SET) != 65535)			//2、星辰上位机校准桨叶角度
    {
        if(((ReadPara(SYS_ST_CODE) & 0x0002) == 0x0002) && ((ReadPara(SYS_ST_CODE) & 0x0004) == 0x0000))	//就绪、关使能状态下才允许桨叶角度校准
        {
            motor_pos_rst0 = 1;
            MotorRstMode = 2;
            motor_pos_rst_sel = 2;
        }
    }

    if((gPosClear == 1) && (Zero_Old == 0))			// 控制字bit8有上升沿才允许进行旋变角度校准操作
    {
        if((UserPara5 >= 0) && (UserPara5 <= 65535))// 允许进行校准的角度值范围：0到655.35°
        {
            if (UserPara5 != ENC_HomingPosOld)		//3、HMI校准桨叶角度
            {
                if((ReadPara(SYS_ST_CODE) & 0x0004) == 0x0000)
                {
                    ENC_HomingPos = UserPara5;
                    motor_pos_rst0 = 1;
                    MotorRstMode = 0;
                }
            }
        }
    }

    Zero_Old = gPosClear;
    ENC_HomingPosOld = 55555;

    if(motor_pos_rst0 == 1)
    {
        if (MotorRstMode == 0)						// HMI校准桨叶角度
        {
            ReadPara(MOTOR_ANGLE_OFFSETH) = (u16)((u32)ENC_HomingPos >> 16);
            ReadPara(MOTOR_ANGLE_OFFSETL) = (u16)((u32)ENC_HomingPos & 0xffff);
            ReadPara(MOTOR_POS_RST)  = 1;
        }
        else if (MotorRstMode == 2)					// 星辰上位机ID859参数校桨叶角度
        {
            ReadPara(MOTOR_ANGLE_OFFSETH) = 0;
            ReadPara(MOTOR_ANGLE_OFFSETL) = ReadPara(MOTO_ANGLE_SET);
            ReadPara(MOTOR_POS_RST)  = 1;
        }
    }

    if (motor_pos_rst0 == 1)
    {
        if (motor_pos_rst_sel == 2) 				// 星辰上位机ID859参数校桨叶角度
        {
            MotorRstMode = 2;
            if (((u32)MON_ActValues_I16_Actual_position) == (u32)ReadPara(MOTO_ANGLE_SET))
            {
                motor_pos_rst_sel = 0; 				// 星辰上位机校准完成
                motor_pos_rst0 = 0;
                ReadPara(MOTOR_POS_RST)  = 0;
                ReadPara(MOTO_ANGLE_SET) = 65535;	// 星辰上位机校准完成，必须把MOTO_ANGLE_SET置为65535
            }
        }
        else 										// 用户校准
        {
            MotorRstMode = 0;
            if (((u32)MON_ActValues_I16_Actual_position == (u32)ENC_HomingPos))
            {
                motor_pos_rst_sel = 0;
                motor_pos_rst0 = 0;
                ReadPara(MOTOR_POS_RST)  = 0;
                ENC_HomingPos = 0;
                RefCheckFinish = 1;					// 桨叶角度校准完成置1
                RefCheckFinish_Cnt = 100;			// 桨叶角度校准完成置1时间
            }
        }
    }
}

/*******************************************************************************
* 函数名 :
* 描  述 : 调用周期500us
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void Task2(void)							
{
    double DataBuf = 0;
    static u16 BKTimeDly = 0;
    static u16 BKTimeDly1 = 0;
    long long PositonErr = 0;
    static s32 MotoHumidSum = 0;
    static u16 HumidCnt = 0;
    static u16 Zerodelay = 0;
    static bool Save_Flag = 0;
    s32 MotoHumidTemp =0;

    if (SlowTimeBaseFlag.bit.SLOW)						// 1ms时基
    {
// 		Display_Cnt++;
// 		if ((Display_Cnt % 500) == 0) 					// 500ms时基
// 		{
// 			Display_Precess();
// 		}

// 		if (Display_Cnt == 1000)
// 		{
// 			Display_Cnt = 0;
// 			LED_Flash_Flag = ~LED_Flash_Flag;
// 			ERR_Flash_Flag = ~ERR_Flash_Flag;
// 		}

        {   // 母线电压滤波
            static long Cvol_Total = 0;
            static u16 Cvol_Cnt=0;

            if(ReadPara(CAN_MXVOL_FIR))
            {
                Cvol_Total += FastVol;	 							//未经DSP滤波的母线电压值
                Cvol_Cnt++;
                if (Cvol_Cnt == ReadPara(CAN_MXVOL_FIR))
                {
                    DC_link_circuit_voltage = Cvol_Total / Cvol_Cnt;
                    Cvol_Total = 0;
                    Cvol_Cnt = 0;
                }
                else if(Cvol_Cnt > ReadPara(CAN_MXVOL_FIR))
                {
                    Cvol_Cnt = 0;
                    Cvol_Total = 0;
                }
            }
            else													//ReadPara(I_RC_FILTER)=0,不做平均值滤波处理
            {
                DC_link_circuit_voltage = FastVol;
            }
        }

		/* if (GyroTime ++< 9)
        {       	
			IIM42652_Handle();		//if (GyroChip == 1) 
			//MPU6050_Handle();		//if (GyroChip == 0)
        }
        else
        {
            GyroTime = 0;
        } */
		
        SlowTimeBaseFlag.bit.SLOW=0;
    }

    ExUartOverHandel();												//USART1或USART6：ARM与上位机通讯超时处理
    InUartOverHandel();												//USART2：ARM与DSP通讯超时处理
    CANopen_Para_Precoss();
    ChangePara();
}

/*******************************************************************************
* 函数名 : 10ms任务 
* 描  述 :
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void Task10ms(void)
{
    double DataBuf = 0;
    static s32 VOL_Sum0 = 0;
    static u16 AIX_Cnt0 = 0;
    static s32 VOL_Sum1 = 0;
    static u16 AIX_Cnt1 = 0;
	static s32 VOL_Sum2 = 0;
    static u16 AIX_Cnt2 = 0;

    ADC_SoftwareStartConv(ADC3);

    VOL_Sum0 += ADC_ConvValue[9];
    AIX_Cnt0++;
    if (AIX_Cnt0 == 50)
    {
        if (ReadPara(IN1_SEL) == 0)									//AI0接-10V~+10V模拟电压输入：零偏=250，增益=3300
        {
            AI0_VOLIN;

            VOL_Sum0 = VOL_Sum0 / AIX_Cnt0;
            VOL_Sum0 = 28193 - VOL_Sum0 + (s16)ReadPara(CAP_VOLT_OFFSET); 		//零偏 默认=250，AIO输入为0V时输出为1.42V，对应AD值28193
            VOL_Sum0 = VOL_Sum0 * (u16)ReadPara(CAP_VOLT_SCALE) * 10 / 655215; 	//增益 默认=3300

            /*
            AI0：00V输入，对应1.42V输出，对应AD值28193（3.3V对应左对齐AD值65520）
            AI0：10V输入，对应2.42V输出，对应AD值48048（3.3V对应左对齐AD值65520）
            2.42V-1.42V=1V,对应48048-28193=19855，即|+-10V|对应19855

            			   19855*增益		   19855*增益		   19855*增益
            AI0输出显示 =  ----------- * 10V = ----------- * 10V = ----------- * 10V (结果保留2位小数点)
            			   19855*3300		   65521500			     655215
            */
        }
        else														//AI0接-20mA~+20mA模拟电流输入：零偏=230，增益=3370
        {
            AI0_CULIN;

            VOL_Sum0 = VOL_Sum0 / AIX_Cnt0;
            VOL_Sum0 = 28193 - VOL_Sum0 + (s16)ReadPara(CAP_VOLT_OFFSET); 		//零偏 默认=250，AIO输入为0mA时输出为1.42V，对应AD值28193
            VOL_Sum0 = VOL_Sum0 * (u16)ReadPara(CAP_VOLT_SCALE) * 20 / 655215; 	//增益 默认=3300

            /*
            AI0：00mA输入，对应1.42V输出，对应AD值28193（3.3V对应左对齐AD值65520）
            AI0：20mA输入，对应2.42V输出，对应AD值48048（3.3V对应左对齐AD值65520）
            2.42V-1.42V=1V,对应48048-28193=19855，即|+-20mA|对应19855

            			   19855*增益		    19855*增益		     19855*增益
            AI0输出显示 =  ----------- * 20mA = ----------- * 20mA = ----------- * 20mA (结果保留2位小数点)
            			   19855*3300		    65521500			   655215
            */
        }

        ReadPara(CAP_VOLT) = VOL_Sum0;
        ReadPara(ANALOG_IN0) = VOL_Sum0;
        VOL_Sum0 = 0;
        AIX_Cnt0 = 0;
    }

    VOL_Sum1 += ADC_ConvValue[8];
    AIX_Cnt1++;
    if (AIX_Cnt1 == 51)												//与AI1错开一个周期
    {
        if (ReadPara(IN2_SEL) == 0)									//AI1接-10V~+10V模拟电压输入：零偏=300，增益=3300
        {
            AI1_VOLIN;

            VOL_Sum1 = VOL_Sum1 / AIX_Cnt1;
            VOL_Sum1 = 28193 - VOL_Sum1 + (s16)ReadPara(AI1P_OFFSET); 			//零偏 默认=250，AI1输入为0V时输出为1.42V，对应AD值28193
            VOL_Sum1 = VOL_Sum1 * (u16)ReadPara(AI1P_SCALE) * 10 / 655215; 		//增益 默认=3300

            /*
            AI1：00V输入，对应1.42V输出，对应AD值28193（3.3V对应左对齐AD值65520）
            AI1：10V输入，对应2.42V输出，对应AD值48048（3.3V对应左对齐AD值65520）
            2.42V-1.42V=1V,对应48048-28193=19855，即|+-10V|对应19855

            			   19855*增益		   19855*增益		   19855*增益
            AI1输出显示 =  ----------- * 10V = ----------- * 10V = ----------- * 10V (结果保留2位小数点)
            			   19855*3300		   65521500			     655215
            */
        }
        else														//AI1接-20mA~+20mA模拟电流输入：零偏=300，增益=3360
        {
            AI1_CULIN;

            VOL_Sum1 = VOL_Sum1 / AIX_Cnt1;
            VOL_Sum1 = 28193 - VOL_Sum1 + (s16)ReadPara(AI1P_OFFSET); 			//零偏 默认=250，AI1输入为0mA时输出为1.42V，对应AD值28193
            VOL_Sum1 = VOL_Sum1 * (u16)ReadPara(AI1P_SCALE) * 20 / 655215; 		//增益 默认=3300

            /*
            AI1：00mA输入，对应1.42V输出，对应AD值28193（3.3V对应左对齐AD值65520）
            AI1：20mA输入，对应2.42V输出，对应AD值48048（3.3V对应左对齐AD值65520）
            2.42V-1.42V=1V,对应48048-28193=19855，即|+-20mA|对应19855

            			   19855*增益		    19855*增益		     19855*增益
            AI1输出显示 =  ----------- * 20mA = ----------- * 20mA = ----------- * 20mA (结果保留2位小数点)
            			   19855*3300		    65521500			   655215
            */
        }

        ReadPara(AI1P_DISP) = VOL_Sum1;
        VOL_Sum1 = 0;
        AIX_Cnt1 = 0;
    }
	
    VOL_Sum2 += ADC_ConvValue[10];
    AIX_Cnt2++;
    if (AIX_Cnt2 == 52)												//与AI0、AI1错开一个周期
    {
        if (ReadPara(AI2P_SEL) == 0)								//AI2接-10V~+10V模拟电压输入：零偏=300，增益=3300
        {
            AI2_VOLIN;

            VOL_Sum2 = VOL_Sum2 / AIX_Cnt2;
            VOL_Sum2 = 28193 - VOL_Sum2 + (s16)ReadPara(AI2P_OFFSET); 			//零偏 默认=250，AI2输入为0V时输出为1.42V，对应AD值28193
            VOL_Sum2 = VOL_Sum2 * (u16)ReadPara(AI2P_SCALE) * 10 / 655215; 		//增益 默认=3300

            /*
            AI2：00V输入，对应1.42V输出，对应AD值28193（3.3V对应左对齐AD值65520）
            AI2：10V输入，对应2.42V输出，对应AD值48048（3.3V对应左对齐AD值65520）
            2.42V-1.42V=1V,对应48048-28193=19855，即|+-10V|对应19855

            			   19855*增益		   19855*增益		   19855*增益
            AI2输出显示 =  ----------- * 10V = ----------- * 10V = ----------- * 10V (结果保留2位小数点)
            			   19855*3300		   65521500			     655215
            */
        }
        else														//AI2接-20mA~+20mA模拟电流输入：零偏=300，增益=3360
        {
            AI2_CULIN;

            VOL_Sum2 = VOL_Sum2 / AIX_Cnt2;
            VOL_Sum2 = 28193 - VOL_Sum2 + (s16)ReadPara(AI2P_OFFSET); 			//零偏 默认=250，AI2输入为0mA时输出为1.42V，对应AD值28193
            VOL_Sum2 = VOL_Sum2 * (u16)ReadPara(AI2P_SCALE) * 20 / 655215; 		//增益 默认=3300
			if (VOL_Sum2 < 400)
			{
				VOL_Sum2 = 400;		//2024-05-11		4~20mA
			}
            /*
            AI2：00mA输入，对应1.42V输出，对应AD值28193（3.3V对应左对齐AD值65520）
            AI2：20mA输入，对应2.42V输出，对应AD值48048（3.3V对应左对齐AD值65520）
            2.42V-1.42V=1V,对应48048-28193=19855，即|+-20mA|对应19855

            			   19855*增益		    19855*增益		     19855*增益
            AI2输出显示 =  ----------- * 20mA = ----------- * 20mA = ----------- * 20mA (结果保留2位小数点)
            			   19855*3300		    65521500			   655215
            */
        }

        ReadPara(AI2P_DISP) = VOL_Sum2;
        VOL_Sum2 = 0;
        AIX_Cnt2 = 0;
    }
	
	if (Pt100Channel < 4)
	{
		Pt100Channel ++;
	}
	else
	{
		Pt100Channel = 0;
	}
	
	Pt100Temp[Pt100Channel] = GetCabinetTemp(Pt100Channel);
	
// 	WritePara(TEMP4, GetCabinetTemp(0));  							// PT100 		T4（备用）
// 	WritePara(TEMP3, GetCabinetTemp(3));  							// 16-15 		T3
// 	WritePara(TEMP2, GetCabinetTemp(2));  							// 14-13 		T2	
// 	WritePara(TEMP1, GetCabinetTemp(1));  							// 12-11 		T1
// 	WritePara(TEMP0, GetCabinetTemp(4));  							// 10-09 		T0

	WritePara(TEMP4, Pt100Temp[0]);  								// PT100 		T4（备用）
	WritePara(TEMP3, Pt100Temp[3]);  								// 16-15 		T3
	WritePara(TEMP2, Pt100Temp[2]);  								// 14-13 		T2	
	WritePara(TEMP1, Pt100Temp[1]);  								// 12-11 		T1
    WritePara(TEMP0, Pt100Temp[4]);  								// 10-09 		T0
	
	{
		static s32 FilData = 0;
		static s32 FilTemp = 0;
		static s32 FilBuf = 0;
		s32 FilCnt = 5;

		FilCnt  = ReadPara(CAN_SPEED_FIR); 
		FilTemp = (s32)((s16)VfebTemp);
		FilTemp = FilTemp + FilBuf - FilData;
		FilBuf  = FilTemp % FilCnt;
		FilTemp = FilTemp / FilCnt;
		FilData += FilTemp;

		Velocity_sensor_actual_value = (s32)FilData;			// 桨叶速度反馈，精度0.01°/S
		ReadPara(PITCH_SPEED) = Velocity_sensor_actual_value;	
	}

	{
		static s32 FilData = 0;
		static s32 FilTemp = 0;
		static s32 FilBuf = 0;
		s32 FilCnt = 5;

		FilCnt  = ReadPara(CAN_SPEED_FIR); 	
		FilTemp = (s32)((s16)VsetTemp);
		FilTemp = FilTemp + FilBuf - FilData;
		FilBuf  = FilTemp % FilCnt;
		FilTemp = FilTemp / FilCnt;
		FilData += FilTemp;
		
		DspSetSpeed = (s32)FilData;								// 桨叶速度设定，精度0.01°/S
	}


    //电流反馈
	//if (ReadPara(CAN_CURRE_FIR))
	{
		DataBuf = (s32)((s16)MoutValue[3]);
		DataBuf *= ReadPara(MAX_I);
		DataBuf *= 10;
		DataBuf /= 0x6c00;
		if (DataBuf > 32767) // 限幅
		{
			DataBuf = 32767;
		}
		else if (DataBuf < -32767)
		{
			DataBuf = -32767;
		}
		{
			// 滤波
			static s32 FilData = 0;
			static s32 FilTemp = 0;
			static s32 FilBuf = 0;
			s32 FilCnt = 0;

			FilCnt  = ReadPara(CAN_CURRE_FIR); // 取滤波常数
			FilTemp = DataBuf;

			FilTemp = FilTemp + FilBuf - FilData;
			FilBuf  = FilTemp % FilCnt;
			FilTemp = FilTemp / FilCnt;
			FilData += FilTemp;

			Current_actual_value = (s16)FilData;
		}
	}
	
	MotoRpmSpeed = (s32)((s16)ReadPara(V_FEEDBACK)) * ReadPara(MAX_V) / 0x6C00;
	
    {
		Position_actual_value = PosReg;			//精度0.01°  		
        MON_ActValues_I16_Actual_position = Position_actual_value;
        Position_actual_value_s32 = Position_actual_value;
		ReadPara(PITCH_ANGLE) = Position_actual_value;
		ReadPara(PITCH_ANGLE1) = Position_actual_value/100;

        //上传主控的桨叶角度范围：-180.00° ~ +180.00°
        if ((Position_actual_value_s32 > -36000) && (Position_actual_value_s32 < -18000))
        {
            Position_actual_value_s16 = Position_actual_value_s32 % 36000 + 36000;
        }
        else if (Position_actual_value_s32 <= -36000)
        {
            Position_actual_value_s32 = Position_actual_value_s32 % 36000;

            if ((Position_actual_value_s32 <= -18000) && (Position_actual_value_s32 > -36000))
            {
                Position_actual_value_s16 = Position_actual_value_s32 % 36000 + 36000;
            }
            else
            {
                Position_actual_value_s16 = Position_actual_value_s32;
            }
        }
        else if ((Position_actual_value_s32 >= 18000) && (Position_actual_value_s32 < 36000))
        {
            Position_actual_value_s16 = Position_actual_value_s32 % 36000 - 36000;
        }
        else if (Position_actual_value_s32 >= 36000)
        {
            Position_actual_value_s32 = Position_actual_value_s32 % 36000;

            if ((Position_actual_value_s32 >= 18000) && (Position_actual_value_s32 < 36000))
            {
                Position_actual_value_s16 = Position_actual_value_s32 % 36000 - 36000;
            }
            else
            {
                Position_actual_value_s16 = Position_actual_value_s32;
            }
        }
        else
        {
            Position_actual_value_s16 = Position_actual_value_s32;
        }
    }

    Angle = Position_actual_value;										

    {
        static u16 cnt = 0;
        cnt++;

        if (cnt > 20)													//10ms * 20 = 200ms
        {
            if ((labs(Angle) / 100) >= ReadPara(OVER_POSITION))			//当前桨叶角度绝对值大于3200°
            {
                if ((ReadPara(SYS_ST_CODE) & 0x0004) == 0x0004)			//运行时才进行位置超限判断
                {
                    if (Angle > 0)										//如果是正方向超过了3200°，通过复位先清除故障，然后可往负方向运行，不能往正方向继续运行
                    {
                        if ((Angle - Angle_Old) >= 5)
                        {
                            OverPosition = 1;
                        }
                    }
                    else												//如果是负方向超过了3200°，通过复位先清除故障，然后可往正方向运行，不能往负方向继续运行
                    {
                        if ((Angle - Angle_Old) <= -5)
                        {
                            OverPosition = 1;
                        }
                    }
                }
            }
            else
            {
                OverPosition = 0;
            }


            Angle_Old = Angle;
            cnt = 0;
        }

        if (ReadPara(ERR_RST) || err_rst1)
        {
            OverPosition = 0;											//复位指令可强行清除桨叶位置累加超限故障
        }
    }

    MotorAngleDo();														//桨叶角度校准

    //if (FlashValifyErr)
	if (ServerFault4 & 0x8000)	
    {
        if ((ReadPara(ERR_RST) || err_rst1) && ((ReadPara(SYS_ST_CODE) & 0x0004) == 0))
        {
            if (((ReadPara(USERPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG) || (ReadPara(USERPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)) &&
                ((ReadPara(DEFTPARAVALIFYDATA1) == SAVE_SUCCESS_FLAG) && (ReadPara(DEFTPARAVALIFYDATA0) == SAVE_SUCCESS_FLAG)))
            {
                u16 i = 0;

				ReadStarsSdcardPara(1);				// 读ARM外挂SD卡的出厂参数
				
				for(i=SYS_PARA_AUTOSTUDY; i<SYS_PARA_LEN_MAX; i++)
				{
					SysParaTable[i].DATA = 0;  		// 自学习参数全部初始化为0
				}
				
                SaveMode = 1;						// 将出厂参数数据存入用户参数扇区
                //FlashValifyErr = 0;
                if (err_rst1 != 1)					// ReadFlashPara(1)会将ReadPara(ERR_RST)清0，后续无法清除手动清除DefectCodeH.FLASHVALIFYERR
                {
                    ReadPara(ERR_RST) = 1;			// 此处需还原
                }
            }
            else if (((ReadPara(DEFTPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG) || (ReadPara(DEFTPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)) &&
                     ((ReadPara(USERPARAVALIFYDATA1) == SAVE_SUCCESS_FLAG) && (ReadPara(USERPARAVALIFYDATA0) == SAVE_SUCCESS_FLAG)))
            {
                u16 i = 0;

				ReadStarsSdcardPara(0);				// 读ARM外挂SD卡的出厂参数
				
				for(i=SYS_PARA_AUTOSTUDY; i<SYS_PARA_LEN_MAX; i++)
				{
					SysParaTable[i].DATA = 0;  		// 自学习参数全部初始化为0
				}
				
                SaveMode = 5;						// 出厂参数扇区参数已发生错误，将用户参数数据存入出厂参数扇区
                //FlashValifyErr = 0;
                if (err_rst1 != 1)					// ReadFlashPara(1)会将ReadPara(ERR_RST)清0，后续无法清除手动清除DefectCodeH.FLASHVALIFYERR
                {
                    ReadPara(ERR_RST) = 1;			// 此处需还原
                }
            }
            else if (((ReadPara(DEFTPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG) || (ReadPara(DEFTPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)) &&
                     ((ReadPara(USERPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG) || (ReadPara(USERPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)))
            {
                u16 i = 0;
				
				for(i=SYS_PARA_AUTOSTUDY; i<SYS_PARA_LEN_MAX; i++)
				{
					SysParaTable[i].DATA = 0;  		// 自学习参数全部初始化为0
				}

                for(i=0; i<SYS_PARA_LEN_STARS; i++)
                {
                    SysParaTable[i].DATA = 0;   	// 系统全部初始化为0
                }

                for(i=0; i < DefaultParaTableNum; i++)
                {
                    SysParaTable[DefaultParaTable[i].ParaID].DATA = DefaultParaTable[i].DATA; // 读默认参数
                }

                for(i=0; i<44; i++)
                {
                    SysParaTable[DefectConfigParaID[i].EnID].DATA =DefectConfigParaData[i].EnID;
                    SysParaTable[DefectConfigParaID[i].DefModeID].DATA =DefectConfigParaData[i].DefModeID;
                    SysParaTable[DefectConfigParaID[i].EnDlyID].DATA =DefectConfigParaData[i].EnDlyID;
                    SysParaTable[DefectConfigParaID[i].RstDlyID].DATA =DefectConfigParaData[i].RstDlyID;
                    SysParaTable[DefectConfigParaID[i].EnTrigID].DATA =DefectConfigParaData[i].EnTrigID;
                    SysParaTable[DefectConfigParaID[i].SafeID].DATA =DefectConfigParaData[i].SafeID;
                    SysParaTable[DefectConfigParaID[i].RstTypID].DATA =DefectConfigParaData[i].RstTypID;
                }

                SaveMode = 2;						// 将程序默认值写入用户参数和出厂参数扇区
                //FlashValifyErr = 0;
                if (err_rst1 != 1)					// ReadFlashPara(1)会将ReadPara(ERR_RST)清0，后续无法清除手动清除DefectCodeH.FLASHVALIFYERR
                {
                    ReadPara(ERR_RST) = 1;			// 此处需还原
                }
            }
        }
    }
	
	EraseErrRecord();								// WindeyHmi擦除SD卡历史故障
}

void Read_DO8_State(void)							// 调用周期=300us
{
	// 1、DO8正常时：反馈信号为恒低电平；
	// 2、DO8短路时：脉冲周期约108ms，5000ms内有约42次脉冲，每个脉冲高电平约100us，5000ms时间窗内检测到DO8反馈高电平超过20次就报故障；
	// 3、DO8超载时：脉冲周期约60ms，5000ms内有约83次脉冲，每个脉冲高电平约1000us，5000ms时间窗内检测到DO8反馈高电平超过20次就报故障；
	
	if ((READ_DO8FEB) && (DO8_OverLoad_Cnt < 100))			
	{													
		DO8_OverLoad_Cnt ++;
	}
	
	if (DO8_OverLoad_Cnt >= 20)								
	{														
		DO8_OverLoad_Flag = 1;
	}
	else
	{
		DO8_OverLoad_Flag = 0;
	}
	
	if (TimeBase_5Secon_ForDO8)
	{
		TimeBase_5Secon_ForDO8 = 0;
		DO8_OverLoad_Cnt = 0;
	}
}

void Task1(void)											// 调用周期300us
{
	DI_DO_Pro();
	
	if ((Init_flag2 && (CrcPassFlag == 666)) && (((DSP_Status >> 5) & 0x01) == 0x01))   
	{
        RunCtrl();
    }
	
    ExUartDataProcess();
    InUartDataProcess();
	Read_DO8_State();
					
    if ((CommuType == CANopen) && err_rst1)
    {
        TxToPCCompleteFlag = 1;
    }
}

/*******************************************************************************
* 函数名 : AddUserTask
* 描  述 : 添加用户任务
* 输  入 : u32* pUserTask         32位用户任务函数指针
*          u16 UesrTaskLoopTime	  16位用户任务循环时间，单位为ms
           u8  UesrTaskPriority   任务优先级
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
USERTASK_TypeDef  UserTaskArray[USER_TASK_NUM];
void AddUserTask(void (*pUserTask)(), u16 UesrTaskLoopTime, u8 UesrTaskPriority)
{
    if (UesrTaskPriority >= USER_TASK_NUM) return;    					// 用户任务优先级超出则跳出
    if (UesrTaskLoopTime == 0) return;                					// 用户任务循环时间为0则跳出
    if (UserTaskArray[UesrTaskPriority].TaskLoopTime != 0) return;    	// 用户任务已存在则跳出

    UserTaskArray[UesrTaskPriority].TaskLoopTime  = UesrTaskLoopTime;
    UserTaskArray[UesrTaskPriority].TaskHand      = pUserTask;
    UserTaskArray[UesrTaskPriority].TaskStartFlag = 0;
    UserTaskArray[UesrTaskPriority].TaskTimeCnt   = 0;
}

/*******************************************************************************
* 函数名 : UserTaskSchedule
* 描  述 : 用户任务调度  1ms调用一次
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void UserTaskSchedule(void)
{
    u8 i = 0;

    for (i = 0; i < USER_TASK_NUM; i++)
    {
        if (UserTaskArray[i].TaskLoopTime == 0) continue;  						// 用户任务不存在则跳过本次循环
        UserTaskArray[i].TaskTimeCnt++;  										// 用户任务循环时间计数
        if (UserTaskArray[i].TaskTimeCnt == UserTaskArray[i].TaskLoopTime)		// 用户任务循环时间到
        {
            UserTaskArray[i].TaskTimeCnt = 0;	  								// 用户任务循环时间清零重新计数
            UserTaskArray[i].TaskStartFlag = 1;	  								// 启动一次用户任务
        }
    }
}

/*******************************************************************************
* 函数名 : UserTaskPrecess
* 描  述 : 执行用户任务
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void UserTaskPrecess(void)
{
    u8 i = 0;

    for (i = 0; i < USER_TASK_NUM; i++)
    {
        if (UserTaskArray[i].TaskStartFlag == 1)	// 用户任务循环时间到
        {
            UserTaskArray[i].TaskStartFlag = 0;	    // 清除用户任务启动标志, 防止重复进入
            (*(UserTaskArray[i].TaskHand))();
        }
    }
}

/*****************************************************************************
* 函数名 : ReadDefectCodeL
* 描  述 :
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*****************************************************************************/
void ReadDefectCodeL(void)
{
	ReadPara(DEFECT_CODE) = DSPErrCode;				// DSP抱闸停机故障L
}

/*****************************************************************************
* 函数名 : ReadDefectCodeH
* 描  述 : ARM故障H，调用周期：1ms
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*****************************************************************************/
void ReadDefectCodeH(void)
{
	if (Init_flag == 0)
    {
		return;
    }
	
	DefectFlagInH.bit.RESERVE00 = 0;							// Bit00：预留
	
	DefectFlagInH.bit.CANOPENOFFLINE = CANopenErrFlag;			// Bit01：CANOpen断线
	
    if (PWR1)	
	{		
		DefectFlagInH.bit.BKPOWERLOWVOL = 0;					// Bit02：PWR1 24V辅源低压（5.0A抱闸电源）故障码255
	}
	else					
	{
		DefectFlagInH.bit.BKPOWERLOWVOL = 1;
	}
	
    if (READ_PWR3)				
	{
		DefectFlagInH.bit.POWER24VLOWVOL = 0;					// Bit03：PWR3 开关电源欠压（7.5A开关电源）故障码256
	}
	else					
	{
		DefectFlagInH.bit.POWER24VLOWVOL = 1;
	}
	
	DefectFlagInH.bit.RESERVE04 = 0;							// Bit04：预留
	
	if ((s16)ReadPara(IGBT_TEMP) >= ReadPara(IGBTOVERTEMP1))
	{
		DefectFlagInH.bit.IGBTOVERTEMP11 = 1;					// Bit05：IGBT过热1,大于100°，回差10°
	}
	else if ((s16)ReadPara(IGBT_TEMP) < (ReadPara(IGBTOVERTEMP1) -10))
	{
		DefectFlagInH.bit.IGBTOVERTEMP11 = 0;
	}

	if ((s16)ReadPara(MOTO_TEMP) >= ReadPara(MOTO_SAFE_TEMP1))
	{
		DefectFlagInH.bit.MOTOOVERTEMP1 = 1;					// Bit06：电机过热1,大于130°
	}
	else if ((s16)ReadPara(MOTO_TEMP) < (ReadPara(MOTO_SAFE_TEMP1) -10))
	{
		DefectFlagInH.bit.MOTOOVERTEMP1 = 0;
	}
	
	DefectFlagInH.bit.RESERVE07 = 0;							// Bit07：预留
	
    DefectFlagInH.bit.CHARGECOMMERR = InUart2ErrFlag; 			// Bit08：与充电器通信故障

    if ((ReadPara(CHARG_DEFECT_CODE) & 0xFCEF) && Init_flag)	
    {
        DefectFlagInH.bit.CHARGEERR = 1;						// Bit09：充电器有故障（除bit4输出断路、bit8电网过压、bit9半控缓启失败以外）
    }
    else
    {
        DefectFlagInH.bit.CHARGEERR = 0;
    }
	
	DefectFlagInH.bit.RESERVE10 = 0;							// Bit10：预留

    if((s16)ReadPara(CHARG_DC24V_TEMP) >= ReadPara(POWER75_OVER_TEMP))
    {
        DefectFlagInH.bit.DC24VOVERTEMP = 1;					// Bit11：DC24V/7.5A开关电源过温，超过95°
    }
    else if((s16)ReadPara(CHARG_DC24V_TEMP) < (ReadPara(POWER75_OVER_TEMP) - 10))
    {
        DefectFlagInH.bit.DC24VOVERTEMP = 0;
    }

	DefectFlagInH.bit.RESERVE12 = 0;							// Bit12：预留
	
    if ((ReadPara(CHARG_DEFECT_CODE) & 0x0080) == 0x0080)
    {
        DefectFlagInH.bit.CHARGEOVERTEMP = 1;					// Bit13：24V抱闸电源过温：取充电器IGBT过温故障
    }
    else
    {
        DefectFlagInH.bit.CHARGEOVERTEMP = 0;
    }
	
	if (DO8_OverLoad_Flag) 	
	{
		DefectFlagInH.bit.DO8OVERLOAD = 1;						// Bit14：DO8超载故障		
	}		
	else 					
	{
		DefectFlagInH.bit.DO8OVERLOAD = 0;
	}
	
	DefectFlagInH.bit.RESERVE15 = 0;							// Bit15：预留
	
    DefectFlagH.all = DefectFlagOutH.all;

    DefectCodeH.all |= (DefectFlagH.all & (~ReadPara(ALARM_MASKH))) & DEFECTMASKH;										// 高十六位的故障更新

    if ((ReadPara(ERR_RST) != 0) || (err_rst1 == 1))
    {
        StmStatus.bit.DefectCodeClear = 1;
        DefectCodeH.all &= (DefectFlagH.all & ~ReadPara(ALARM_MASKH)) | (ReadPara(ALARM_AUTO_RST_ENH)) & DEFECTMASKH;	// 手动复位
    }

    DefectCodeH.all &= ((DefectFlagH.all & ~ReadPara(ALARM_MASKH)) | (~ReadPara(ALARM_AUTO_RST_ENH)) & DEFECTMASKH); 	// 自动复位

    ReadPara(DEFECT_CODEH) = DefectCodeH.all;
}

/*****************************************************************************
* 函数名 : ReadDefectCodeM
* 描  述 : 调用周期：1ms
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*****************************************************************************/
void ReadDefectCodeM(void)
{
    if (Init_flag == 0)
    {
        return;
    }
	
// 	if ((DefectFlagInM_Delay[0] < 5) && (DIStatus.bit.DI00 == 0))
// 	{		
// 		DefectFlagInM_Delay[0] ++;
// 	}
// 	else if ((DefectFlagInM_Delay[0] > 0) && (DIStatus.bit.DI00 == 1))	
// 	{
// 		DefectFlagInM_Delay[0] --;
// 	}
// 	if (DefectFlagInM_Delay[0] == 0)	
// 	{		
// 		DefectFlagInM.bit.ENOPERR = 0;
// 	}
// 	else if (DefectFlagInM_Delay[0] >= 5)		
// 	{		
// 		DefectFlagInM.bit.ENOPERR = 1;
// 	}
	
// 	if (DIStatus.bit.DI00)	
// 	{		
// 		DefectFlagInM.bit.ENOPERR = 0;
// 	}
// 	else 		
// 	{		
// 		DefectFlagInM.bit.ENOPERR = 1;							// Bit00：ENPO低电平故障
// 	}

    if (DIStatus.bit.DI00)	
	{		
		if (DefectFlagInM_Delay_Out[0] > 0)
		{
			DefectFlagInM_Delay_Out[0] --;
		}
		else
		{
			DefectFlagInM.bit.ENOPERR = 0;						// ENPO故障退出延时：500ms
		}
	}
    else 		
	{		
		DefectFlagInM.bit.ENOPERR = 1;							// Bit00：ENPO低电平故障；故障退出延时：500ms
		DefectFlagInM_Delay_Out[0] = 500;
	}
	
	if (OverPosition)
    {
        DefectFlagInM.bit.OVERPOSITION = 1;						// Bit01：桨叶位置累加超限
    }
    else
    {
        DefectFlagInM.bit.OVERPOSITION = 0;
    }
	
	if (((ReadPara(USERPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG) || (ReadPara(USERPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)) ||
		((ReadPara(DEFTPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG) || (ReadPara(DEFTPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)) ||
		((ReadPara(CHARG_DEFECT_CODE) & 0x0001) == 0x0001))
    //if ((FlashValifyErr) || ((ReadPara(CHARG_DEFECT_CODE) & 0x0001) == 0x0001))
    {
        DefectFlagInM.bit.FLASHVALIFYERR = 1;					// Bit15：ARM_Flash_Fault(主ARM SD卡、或辅ARM Flash校验参数读出错误)
		ARM_Flash_Fault = 1;									
    }
    else
    {
        DefectFlagInM.bit.FLASHVALIFYERR = 0;
		ARM_Flash_Fault = 0;
    }	

    DefectFlagOutM.all = DefectFlagInM.all;
    DefectFlagM.all = DefectFlagOutM.all;

    DefectCodeM.all |= (DefectFlagM.all & (~ReadPara(ALARM_MASKM))) & DEFECTMASKM;										// M十六位的故障更新

    if ((ReadPara(ERR_RST) != 0) || (err_rst1 == 1))
    {
        StmStatus.bit.DefectCodeClear = 1;
        DefectCodeM.all &= (DefectFlagM.all & ~ReadPara(ALARM_MASKM)) | (ReadPara(ALARM_AUTO_RST_ENM)) & DEFECTMASKM;	// 手动复位
    }

    DefectCodeM.all &= ((DefectFlagM.all & ~ReadPara(ALARM_MASKM)) | (~ReadPara(ALARM_AUTO_RST_ENM)) & DEFECTMASKM); 	// 自动复位

    ReadPara(DEFECT_CODEM) = DefectCodeM.all;
}

/*****************************************************************************
* 函数名 :
* 描  述 :
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*****************************************************************************/
void DefectCodeProcess(void)
{
    u64 DefectTempIn = 0;

    DefectTempIn = ((u64)DefectFlagInH.all<<16) | ((u64)DefectFlagInL.all<<0) | ((u64)ChargeDefectFlagIn.all<<32);

    DefectProcess ((u64)(DefectTempIn),DefectConfigParaID);

    DefectFlagOutH.all =
         ((DefectConfigParaID[31].DefectState&1)<<15)|((DefectConfigParaID[30].DefectState&1)<<14)
        |((DefectConfigParaID[29].DefectState&1)<<13)|((DefectConfigParaID[28].DefectState&1)<<12)
        |((DefectConfigParaID[27].DefectState&1)<<11)|((DefectConfigParaID[26].DefectState&1)<<10)
        |((DefectConfigParaID[25].DefectState&1)<<9) |((DefectConfigParaID[24].DefectState&1)<<8)
        |((DefectConfigParaID[23].DefectState&1)<<7) |((DefectConfigParaID[22].DefectState&1)<<6)
        |((DefectConfigParaID[21].DefectState&1)<<5) |((DefectConfigParaID[20].DefectState&1)<<4)
        |((DefectConfigParaID[19].DefectState&1)<<3) |((DefectConfigParaID[18].DefectState&1)<<2)
        |((DefectConfigParaID[17].DefectState&1)<<1) |((DefectConfigParaID[16].DefectState&1)<<0);
}

void DefectCodeMap(void)																		
{
    if(((ReadPara(DEFECT_CODEH)) >> 5) & 0x0001) 				ServerFault1 |= 0x0001;			//Bit0		201		IGBT过温1		100°
    else		 												ServerFault1 &= 0xFFFE;
    if(((ReadPara(DEFECT_CODE)) >> 6) & 0x0001)					ServerFault1 |= 0x0002;			//Bit1		202		IGBT过温2		105°
    else														ServerFault1 &= 0xFFFD;
    if(((ReadPara(DEFECT_CODE)) >> 9) & 0x0001) 				ServerFault1 |= 0x0004;			//Bit2		203		输出过流		Fault低电平
    else		 												ServerFault1 &= 0xFFFB;
    if(((ReadPara(DEFECT_CODE)) >> 0) & 0x0001) 				ServerFault1 |= 0x0008;			//Bit3		204		驱动器输出过载	I^2*T
    else		 												ServerFault1 &= 0xFFF7;
    if(((ReadPara(DEFECT_CODE)) >> 4) & 0x0001) 				ServerFault1 |= 0x0010;			//Bit4		205		输出过流(软件)	170A
    else														ServerFault1 &= 0xFFEF;
    if(((ReadPara(DEFECT_CODE)) >> 10) & 0x0001)				ServerFault1 |= 0x0020;			//Bit5		206		输出过流		Comp低电平
    else														ServerFault1 &= 0xFFDF;
    if(((ReadPara(DEFECT_CODE)) >> 1) & 0x0001) 				ServerFault1 |= 0x0040;			//Bit6		207		母线过压(软件)	790VDC
    else		 												ServerFault1 &= 0xFFBF;
    //															ServerFault1 |= 0x0080;			//Bit7		208		母线过压(硬件)	暂无
    //															ServerFault1 &= 0xFF7F;
    if(((ReadPara(DEFECT_CODE)) >> 2) & 0x0001) 				ServerFault1 |= 0x0100;			//Bit8		209		母线欠压(软件)	160VDC 回差50VDC
    else		 												ServerFault1 &= 0xFEFF;
    if(((ReadPara(DEFECT_CODE)) >> 8) & 0x0001)					ServerFault1 |= 0x0200;			//Bit9		210		电机过载		I^2*T
    else														ServerFault1 &= 0xFDFF;
	if(((ReadPara(DEFECT_CODEH)) >> 6) & 0x0001) 				ServerFault1 |= 0x0400;			//Bit10		211		电机过温1		130°
	else		 												ServerFault1 &= 0xFBFF;
	if(((ReadPara(DEFECT_CODE)) >> 7) & 0x0001)					ServerFault1 |= 0x0800;			//Bit11		212		电机过温2		140°
	else														ServerFault1 &= 0xF7FF;
	if(((ReadPara(DEFECT_CODEH)) >> 11) & 0x0001)				ServerFault1 |= 0x1000;			//Bit12		213		开关电源过温	100°
    else														ServerFault1 &= 0xEFFF;
    //															ServerFault1 |= 0x2000;			//Bit13		214		输出接地故障
    //															ServerFault1 &= 0xDFFF;
    if(((ReadPara(DEFECT_CODET)) >> 0) & 0x0001)				ServerFault1 |= 0x4000;			//Bit14		215		DSP与ARM通讯故障300ms
    else														ServerFault1 &= 0xBFFF;
    if(((ReadPara(DEFECT_CODE)) >> 14) & 0x0001) 			  	ServerFault1 |= 0x8000;			//Bit15		216		电机转速超限	3000RPM
    else														ServerFault1 &= 0x7FFF;

    //							 								ServerFault2 |= 0x0001;			//Bit0		217		预留
    //		 													ServerFault2 &= 0xFFFE;
    if(((ReadPara(DEFECT_CODEM)) >> 1) & 0x0001)				ServerFault2 |= 0x0002;			//Bit1		218		桨叶位置累加超限3200.00°
    else														ServerFault2 &= 0xFFFD;
    // 															ServerFault2 |= 0x0004;			//Bit2		219		两编码器位置差超限
    // 															ServerFault2 &= 0xFFFB;
    //															ServerFault2 |= 0x0008;			//Bit3		220		电网过压			
    //		 													ServerFault2 &= 0xFFF7;
    //															ServerFault2 |= 0x0010;			//Bit4		221		电网欠压
    //		 													ServerFault2 &= 0xFFEF;
    //															ServerFault2 |= 0x0020;			//Bit5		222		电网缺相
    //															ServerFault2 &= 0xFFDF;
    if(((ReadPara(DEFECT_CODET)) >> 4) & 0x0001)				ServerFault2 |= 0x0040;			//Bit6		223		DSP：EEPROM写入故障（忽略）
    else														ServerFault2 &= 0xFFBF;
	if(((ReadPara(DEFECT_CODE)) >> 13) & 0x0001)				ServerFault2 |= 0x0080;			//Bit7		224		DSP：EEPROM读出故障（抱闸）
	else														ServerFault2 &= 0xFF7F;
    if(((ReadPara(DEFECT_CODE)) >> 5) & 0x0001)					ServerFault2 |= 0x0100;			//Bit8		225		电流采样电路故障	
    else														ServerFault2 &= 0xFEFF;
    if(((ReadPara(DEFECT_CODE)) >> 12) & 0x0001)				ServerFault2 |= 0x0200;			//Bit9		226		制动器故障
    else														ServerFault2 &= 0xFDFF;
    //															ServerFault2 |= 0x0400;			//Bit10		227		预留
    //															ServerFault2 &= 0xFBFF;
    //															ServerFault2 |= 0x0800;			//Bit11		228		预留
    //															ServerFault2 &= 0xF7FF;
	if(((ReadPara(DEFECT_CODET)) >> 3) & 0x0001)				ServerFault2 |= 0x1000;			//Bit12		229		抱闸短路或开路	>7.5A、<0.8
    else														ServerFault2 &= 0xEFFF;
    //															ServerFault2 |= 0x2000;			//Bit13		230		预留
    //															ServerFault2 &= 0xDFFF;
    //															ServerFault2 |= 0x4000;			//Bit14		231		预留
    //															ServerFault2 &= 0xBFFF;
    if(((ReadPara(DEFECT_CODE)) >>11) & 0x0001) 				ServerFault2 |= 0x8000;			//Bit15		232		收桨超时		32S
	else														ServerFault2 &= 0x7FFF;

    // 															ServerFault3 |= 0x0001;			//Bit0		233		Can Bus off
    //		 													ServerFault3 &= 0xFFFE;
    //															ServerFault3 |= 0x0002;			//Bit1		234		CAN 节点错误
    //															ServerFault3 &= 0xFFFD;
    // 															ServerFault3 |= 0x0004;			//Bit2		235		CAN 心跳错误
    // 															ServerFault3 &= 0xFFFB;
    if(((ReadPara(DEFECT_CODEH)) >> 1) & 0x0001) 				ServerFault3 |= 0x0008;			//Bit3		236		CAN 通讯故障
    else		 												ServerFault3 &= 0xFFF7;
    //															ServerFault3 |= 0x0010;			//Bit4		237		CAN 过载
    //															ServerFault3 &= 0xFFEF;
    //															ServerFault3 |= 0x0020;			//Bit5		238		CAN 被动故障
    //															ServerFault3 &= 0xFFDF;
    if(((ReadPara(DEFECT_CODEH)) >> 9) & 0x0001) 				ServerFault3 |= 0x0040;			//Bit6		239		充电器故障
    else														ServerFault3 &= 0xFFBF;
    if(((ReadPara(DEFECT_CODEH)) >> 8) & 0x0001) 				ServerFault3 |= 0x0080;			//Bit7		240		充电器通讯故障
    else		 												ServerFault3 &= 0xFF7F;
    //															ServerFault3 |= 0x0100;			//Bit8		241		后备电源故障（输出断路）
    //															ServerFault3 &= 0xFEFF;
    if(((ReadPara(DEFECT_CODET)) >> 2) & 0x0001) 				ServerFault3 |= 0x0200;			//Bit9		242		电机编码器故障
    else		 												ServerFault3 &= 0xFDFF;
    if(((ReadPara(DEFECT_CODET)) >> 1) & 0x0001)				ServerFault3 |= 0x0400;			//Bit10		243		DSP EEPROM桨叶角度存储错误
    else														ServerFault3 &= 0xFBFF;
    //															ServerFault3 |= 0x0800;			//Bit11		244		预留
    //															ServerFault3 &= 0xF7FF;
    //															ServerFault3 |= 0x1000;			//Bit12		245		预留
    //															ServerFault3 &= 0xEFFF;
    //															ServerFault3 |= 0x2000;			//Bit13		246		预留
    //															ServerFault3 &= 0xDFFF;
    if(((ReadPara(DEFECT_CODEM)) >> 0) & 0x0001) 				ServerFault3 |= 0x4000;			//Bit14		247		ENPO低电平故障
    else														ServerFault3 &= 0xBFFF;
    // 															ServerFault3 |= 0x8000;			//Bit15		248		预留
    // 															ServerFault3 &= 0x7FFF;

    if(((ReadPara(DEFECT_CODEH)) >> 2) & 0x0001)				ServerFault4 |= 0x0040;			//Bit6		255		24V辅源PWR1欠压（5A抱闸）
    else														ServerFault4 &= 0xFFBF;
// 	if(((ReadPara(DEFECT_CODEH)) >> 3) & 0x0001)				ServerFault4 |= 0x0080;			//Bit7		256		开关电源PWR3欠压（7.5A）
// 	else														ServerFault4 &= 0xFF7F;
    if((((ReadPara(DEFECT_CODEH)) >> 3) & 0x0001) ||
	   (((ReadPara(DEFECT_CODET)) >> 5) & 0x0001))				ServerFault4 |= 0x0080;			//Bit7		256		开关电源PWR3欠压（7.5A）或DY低电平(Add On：2023-05-08)
    else														ServerFault4 &= 0xFF7F;
    if(((ReadPara(CHARG_DEFECT_CODE)) >> 7) & 0x0001)			ServerFault4 |= 0x0100;			//Bit8		257		24V抱闸电源过温 取充电器IGBT过温故障
    else														ServerFault4 &= 0xFEFF;
    if((((ReadPara(CHARG_DEFECT_CODE)) >> 8) & 0x0001) ||
	   (((ReadPara(CHARG_DEFECT_CODE)) >> 9) & 0x0001))			ServerFault4 |= 0x0200;			//Bit9		258		半控整流故障(电网过压或半控缓启失败)
    else														ServerFault4 &= 0xFDFF;
    if(((ReadPara(DEFECT_CODE)) >> 3) & 0x0001) 				ServerFault4 |= 0x2000;			//Bit13		262		看门狗故障
    else														ServerFault4 &= 0xDFFF;
    if(((ReadPara(DEFECT_CODEM)) >> 2) & 0x0001) 				ServerFault4 |= 0x8000;			//Bit15		264		ARM_Flash_Fault(主ARMSD、或辅ARM Flash校验参数读出错误)
    else 														ServerFault4 &= 0x7FFF;							

	ServerFault5 = ReadPara(CHARG_DEFECT_CODE) & 0xFCEF;										//Bit0~15	265~279	充电器故障（bit4输出断路、bit8电网过压、bit9半控缓启失败，除外）
}













