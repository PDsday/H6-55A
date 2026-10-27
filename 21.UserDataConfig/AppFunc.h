//源文件名为:Microsoft Excel - PDC650_ARM功能码定义V1.04  [兼容模式]
//下列表格建立时间: Monday, Jul 22 2019 17:41:49
#ifndef _APP_FUNC_H_
#define _APP_FUNC_H_
#include "stm32f4xx.h"

#ifndef  _APP_FUNC_DEF_
    #define  _APP_FUNC_VAR_     extern 
#else
    #define  _APP_FUNC_VAR_    
#endif

typedef struct
{
//ARM传给DSP的设置值
    
    uint16_t    ControlMode;              //   速度位置控制模式   
    uint16_t    inverter_controlword;     //    CIA402控制字
	
    uint16_t    ArmControlWord;           //    ARM控制字
    int16_t     ArmTargetPos;             //   ARM给定位置
    uint16_t    ArmProfileVel;            //   ARM给定位置模式下速度
    int16_t     ArmTargetVel;             //    ARM给定速度
    
    uint16_t    AccSpeedSetValue;         //    加速度设定值
	uint16_t	DecSpeedSetValue;		  //    减速度设定值
	uint16_t    BigTooth;                 //    大齿轮数，DSP需存入EEPROM
	uint16_t    SlowTooth;                //    小齿轮数，DSP需存入EEPROM
    
    uint16_t    SetChargeVoltage;         //    设定充电电压
    uint16_t    SetChargeCurrent;         //    设定充电电流  
    uint16_t    RefCheckVal;              //    桨叶角度校准值  
	uint16_t    SelcetMotorType;          //    变桨电机类型选择，写入DSP的ROM
    uint16_t    TrmRsv01;
    uint16_t    TrmRsv02;

//DSP传给ARM的状态值
    
    uint16_t    StateWord;                //    驱动器状态字
    uint16_t    inverter_statusword;      //    CIA402状态字
    
    int16_t     ActualPosA;               //    A编桨叶实际位置
    int16_t     ActualPosB;               //    B编桨叶实际位置
    
    int16_t     Dsp_SetSpeed;             //    桨叶实际设定转速
    uint16_t    ActualVelocity;           //    桨叶实际转速
     
    int16_t     ActualTorque;             //    实际输出转矩
    uint16_t    ActualOutputCurr;         //    实际输出电流
    
    uint16_t    ActualDCVoltage;          //    直流母线电压
    uint16_t    GridVoltage;              //    电网电压
    
    uint16_t    BatteryVoltage;           //    超级电容电压
    uint16_t    BatteryVoltageMid;        //    超级电容中间点电压
    uint16_t    ActChargeVoltage;         //    实际充电电压
    uint16_t    ActChargeCurrent;         //    实际充电电流
    uint16_t    BatteryCap;               //    超级电容容量
    
    uint16_t    InverterFaultWord1;       //    驱动器故障字1
    uint16_t    InverterFaultWord2;       //    驱动器故障字2
    uint16_t    InverterFaultWord3;       //    驱动器故障字3
    uint16_t    InverterFaultWord4;       //    驱动器故障字4
    
    uint16_t    ChargerFaultWord;         //    充电器故障字
    uint16_t    BatteryFaultWord;         //    电池故障字
    
    int16_t     MotorTemp;                //    电机温度
    int16_t     HeatsinkTemp;             //    IGBT温度
    int16_t     PT1003Temp;               //    PT1003温度
    
    uint16_t    Brake;                    //    抱闸关闭状态
    uint16_t    MotorOverLoadPer;         //    电机预过载百分比 
    uint16_t    HeartbeatCount;           //    双口RAM通讯心跳
    uint16_t    ChargerState;             //    充电器状态字
    int16_t     MotorRotationRPM;         //    电机RPM转速   
	int16_t     Relative_humidity;        //    驱动器内部湿度 单位1
	int16_t     Acceleration_x;        	  //    驱动器X轴加速度
	int16_t     Acceleration_y;        	  //    驱动器Y轴加速度
	int16_t     Acceleration_z;        	  //    驱动器Z轴加速度
	uint16_t	Driver_unit_Version;	  //    驱动器DSP软件版本
	uint16_t	EmergencyAutoRunTIME;	  //    驱动器底层急停顺桨时间设定
 	uint16_t    EmergencyAutoRunSpe;	  //	驱动器底层急停顺桨速度设定
	uint16_t	DSP_StateWord;			  //    DSP状态值反馈
    uint16_t    InternalTemp;             //    驱动器内部温度
	uint16_t    DSP_ActMotorType;         //    驱动器底层实际电机类型
	uint16_t    RcvRsv01;
	uint16_t    DSPBootVersion;				//  DSP boot版本号
	uint16_t    BL_Version;						//  ARM boot版本号
	uint16_t    RcvRsv02;
	uint16_t    RcvRsv03;
	
//ARM传给DSP的状态值

    uint16_t    CanAddr;                  //    CAN通讯地址
    uint16_t    InputWord1;               //    DI输入状态1
    uint16_t    InputWord2;               //    DI输入状态2
    uint16_t    OutputWord;               //    DO输出状态
    int16_t     AxleTemp;                 //    轴箱温度
    int16_t     CapTemp;                  //    电容温度
    int16_t     HubTemp;                  //    轮毂温度
    int16_t     ControlBoardTemp;         //    控制板温度
    
    int16_t     AI1Current;                
    int16_t     AI2Current;                
    int16_t     AI1Voltage;                
    int16_t     AI2Voltage;                
//CAN通讯参数

    uint16_t    TXPDO1_1;
    uint16_t    TXPDO1_2;
    uint16_t    TXPDO1_3;
	uint16_t    TXPDO1_4;
    uint16_t    TXPDO2_1;
    uint16_t    TXPDO2_2;
    uint16_t    TXPDO2_3;
    uint16_t    TXPDO2_4;
    uint16_t    TXPDO3_1;
    uint16_t    TXPDO3_2;
    uint16_t    TXPDO3_3;
    uint16_t    TXPDO3_4;
    uint16_t    TXPDO4_1;
    uint16_t    TXPDO4_2;
    uint16_t    TXPDO4_3;
    uint16_t    TXPDO4_4;   
    uint16_t    RXPDO1_1;
    uint16_t    RXPDO1_2;
    uint16_t    RXPDO1_3;
    uint16_t    RXPDO1_4;
    uint16_t    RXPDO2_1;
    uint16_t    RXPDO2_2;
    uint16_t    RXPDO2_3;
    uint16_t    RXPDO2_4;
    uint16_t    RXPDO3_1;
    uint16_t    RXPDO3_2;
    uint16_t    RXPDO3_3;
    uint16_t    RXPDO3_4;
    uint16_t    RXPDO4_1;
    uint16_t    RXPDO4_2;
    uint16_t    RXPDO4_3;
    uint16_t    RXPDO4_4;
}sfunccode;

_APP_FUNC_VAR_    sfunccode        DoubleRAM;

#define      DoubleRAM_LEN      (sizeof(sfunccode)/sizeof(uint16_t))
/*******************************************************************************************/
#endif

    
