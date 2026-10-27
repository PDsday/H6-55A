
#include "rtc.h"
#include "rtcdata.h"
#include "ExternPrototype.h"
#include "stdbool.h"
//********************************************************************************
//修改说明
//
//新增:RTC_Get_Week函数,用于根据年月日信息,得到星期信息.
//////////////////////////////////////////////////////////////////////////////////

// bool rtc_clock_able = 0;
NVIC_InitTypeDef   NVIC_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;
u16 oldt_year=0,oldt_monday=0,oldt_hourmin=0,oldt_secon=0;
u16 RTC_fLAG[2] = {0x5050,0x5050};


//=================================================================================
//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败
//==================================================================================
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
    RTC_TimeTypeDef RTC_TimeTypeInitStructure;

    RTC_TimeTypeInitStructure.RTC_Hours=hour;
    RTC_TimeTypeInitStructure.RTC_Minutes=min;
    RTC_TimeTypeInitStructure.RTC_Seconds=sec;
    RTC_TimeTypeInitStructure.RTC_H12=ampm;

    return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);

}



//===================================================================================
//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败
//====================================================================================
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{

    RTC_DateTypeDef RTC_DateTypeInitStructure;
    RTC_DateTypeInitStructure.RTC_Date=date;
    RTC_DateTypeInitStructure.RTC_Month=month;
    RTC_DateTypeInitStructure.RTC_WeekDay=week;
    RTC_DateTypeInitStructure.RTC_Year=year;
    return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}


//==========================================================================
//RTC初始化
//返回值:0,初始化成功;
//       1,LSE开启失败;
//       2,进入初始化模式失败;
//===========================================================================
u8 My_RTC_Init(void)
{
    RTC_InitTypeDef RTC_InitStructure;
    u16 retry=0X90;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//使能PWR时钟
    PWR_BackupAccessCmd(ENABLE);							//使能后备寄存器访问(关掉后备寄存器，更省电-----纽扣电池或者超级电容)

    RCC_LSEConfig(RCC_LSE_ON);							//LSE 开启

    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
    {
        retry--;
        delay_lan(1000);               					 //10ms
        if(retry==0)
        {
            ReadPara(RTC_YEAR_FEB) = 5050;					//====================年===========================
            ReadPara(RTC_MONTH_AND_DATA_FEB) = 5050;		//=================月、日==========================
            ReadPara(RTC_HOUR_AND_MINIT_FEB) = 5050;		//=================时、分==========================
            ReadPara(RTC_SECON_FEB) = 5050;					//===================秒=============================
            return 1;										//LSE 开启失败.
        }
    }

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);					//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
    RCC_RTCCLKCmd(ENABLE);									//使能RTC时钟

    RTC_OutputConfig(RTC_Output_Disable,RTC_Output_AlarmA);	//PC13可以悬空
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//RTC异步分频系数(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;				//RTC同步分频系数(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;	//RTC设置为,24小时格式
    RTC_Init(&RTC_InitStructure);

    oldt_year = ReadPara(RTC_YEAR_SET);  //年
    oldt_monday = ReadPara(RTC_MONTH_AND_DATA_SET);  //月、日
    oldt_hourmin = ReadPara(RTC_HOUR_AND_MINIT_SET);  //时、分
    oldt_secon = ReadPara(RTC_SECON_SET);  //秒

    return 0;
}



/*
************************************************************
*	函数名称：delay_lan
*
*	函数功能：延时   延时时间t（us）= a*200*((1/144)*4)     //此处会存在指令误差
*
************************************************************
*/
void delay_lan(unsigned short a)  //用于相应的外设进行引脚的复位以及相应的延时控制
{
    unsigned short pollone = a;
    unsigned char polltwo = 0;
    for(; pollone > 0; pollone--)
    {
        for(polltwo=0; polltwo <200; polltwo++)
        {
        }
    }
}