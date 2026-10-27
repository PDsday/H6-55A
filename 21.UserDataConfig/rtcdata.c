#include "rtc.h"
#include "rtcdata.h"
#include "ExternPrototype.h"

//=========C库=============
#include "stdbool.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#ifndef TEST_FOR_STARS				// 未定义星辰厂内测试版
#include "HMI_Display.h"
#include "DUT_CAN_Recive_Data.h"
#include "GlobalVar_POUS.h"
#else
#endif

//===================变量定义区======================================
struct rtc_time set_time;
struct RW_Time  rewr_time;
extern u16 oldt_year,oldt_monday,oldt_hourmin,oldt_secon;

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;


extern u16 Func_0x10_Data[];
extern u8 Time_Adjust_Flag;

/*===================================================================
 * 函数名：Time_Adjust
 * 描述  ：时间调节
 * 输入  ：用于读取RTC时间的结构体指针（北京时间）
 * 输出  ：无
 * 调用  ：外部调用
 =====================================================================*/
void Time_Adjust()
{
    if(set_time.tm_hour <= 12)
    {
        RTC_Set_Time( set_time.tm_hour, set_time.tm_min, set_time.tm_sec,RTC_H12_AM);	 //设置时间
    }
    else
    {
        RTC_Set_Time( set_time.tm_hour, set_time.tm_min, set_time.tm_sec,RTC_H12_PM);	 //设置时间
    }

    RTC_Set_Date( set_time.tm_year, set_time.tm_mon, set_time.tm_mday,set_time.tm_week); //设置日期
}


/************************************************************
*	函数名称：	void Time_Resive_ExctUsart(void)
*
*	函数功能：	处理输入的数据决定修改的参数
*
*
*	对应上位机ID号说明：
*       SET:             |    READ:
*         年； 408       |       年：412
*         月、日：409    |       月、日：413
*         时、分：410    |       时、分：414
*         秒    ：411    |           秒：415
*
************************************************************/
void Time_Resive_ExctUsart(void)
{
	static bool CAN_R_CtrlWord_Time_Calibration_Old = 0;
	
    rewr_time.xp_year = ReadPara(RTC_YEAR_SET);  //年
    rewr_time.xp_monday = ReadPara(RTC_MONTH_AND_DATA_SET);  //月、日
    rewr_time.xp_hourmin = ReadPara(RTC_HOUR_AND_MINIT_SET);  //时、分
    rewr_time.xp_secon = ReadPara(RTC_SECON_SET);  //秒

	if (Time_Adjust_Flag)
    {
		#ifndef TEST_FOR_STARS														// 未定义星辰厂内测试版
				set_time.tm_year = Rx_image[30].all % 100;							// 系统时间年
				set_time.tm_mon  = Rx_image[31].all;
				set_time.tm_mday = Rx_image[32].all;
				set_time.tm_hour = Rx_image[33].all;
				set_time.tm_min  = Rx_image[34].all;
				set_time.tm_sec  = Rx_image[35].all;
		#else
				set_time.tm_year = Func_0x10_Data[30] % 100;						// 系统时间年
				set_time.tm_mon  = Func_0x10_Data[31];
				set_time.tm_mday = Func_0x10_Data[32];
				set_time.tm_hour = Func_0x10_Data[33];
				set_time.tm_min  = Func_0x10_Data[34];
				set_time.tm_sec  = Func_0x10_Data[35];
		#endif

        Time_Adjust();
        Time_Adjust_Flag = 0;
    }
	
	#ifndef TEST_FOR_STARS
		if (CAN_R.CtrlWord_Time_Calibration && (CAN_R_CtrlWord_Time_Calibration_Old == 0))
		{	
			set_time.tm_year = RTC_DateStruct.RTC_Year;						
			set_time.tm_mon  = RTC_DateStruct.RTC_Month;
			set_time.tm_mday = RTC_DateStruct.RTC_Date;
			set_time.tm_hour = 12;													// 主控会在每天中午12:00进行一次校准
			set_time.tm_min  = 0;
			set_time.tm_sec  = 0;

			Time_Adjust();
		}
		CAN_R_CtrlWord_Time_Calibration_Old = CAN_R.CtrlWord_Time_Calibration;
	#else
	#endif
		
    if(rewr_time.xp_year != oldt_year ||  rewr_time.xp_monday != oldt_monday ||
            rewr_time.xp_hourmin != oldt_hourmin || rewr_time.xp_secon !=oldt_secon)
    {
        RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);  //获取年、月、日
        set_time.tm_year = RTC_DateStruct.RTC_Year;
        set_time.tm_mon = RTC_DateStruct.RTC_Month;
        set_time.tm_mday = RTC_DateStruct.RTC_Date;

        RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);   //获取时、分、秒
        set_time.tm_hour = RTC_TimeStruct.RTC_Hours;
        set_time.tm_min  = RTC_TimeStruct.RTC_Minutes;
        set_time.tm_sec = RTC_TimeStruct.RTC_Seconds;

//====================年==========================
        if(rewr_time.xp_year != oldt_year)
        {
            set_time.tm_year = rewr_time.xp_year;
            oldt_year = ReadPara(RTC_YEAR_SET);             //年
        }
//=================月、日==========================
        if(rewr_time.xp_monday != oldt_monday)
        {
            set_time.tm_mon = rewr_time.xp_monday/100;  	//月
            set_time.tm_mday = rewr_time.xp_monday%100;  	//日
            oldt_monday = ReadPara(RTC_MONTH_AND_DATA_SET); //月、日
        }
//=================时、分==========================
        if(rewr_time.xp_hourmin != oldt_hourmin)
        {
            set_time.tm_hour = rewr_time.xp_hourmin/100;  	 //时
            set_time.tm_min =  rewr_time.xp_hourmin%100;  	 //分
            oldt_hourmin = ReadPara(RTC_HOUR_AND_MINIT_SET); //时、分
        }
//===================秒==========================
        if(rewr_time.xp_secon !=oldt_secon)
        {
            set_time.tm_sec = rewr_time.xp_secon ;
            oldt_secon = ReadPara(RTC_SECON_SET);  //秒
        }
        Time_Adjust();
    }


}


/************************************************************
*	函数名称：	void Time_Txto_Startxp(void)
*
*	函数功能：	将实时时间发送到星辰上位机
*
*
*	对应上位机ID号说明：
*       SET:             |    READ:
*         年； 408       |       年：412
*         月、日：409    |       月、日：413
*         时、分：410    |       时、分：414
*         秒    ：411    |           秒：415
*
************************************************************/
void Time_Txto_Startxp(void)
{
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);   //获取时、分、秒
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);  //获取年、月、日

//====================年===========================
    ReadPara(RTC_YEAR_FEB) = RTC_DateStruct.RTC_Year;
//=================月、日==========================
    ReadPara(RTC_MONTH_AND_DATA_FEB) = ( RTC_DateStruct.RTC_Month*100) +  RTC_DateStruct.RTC_Date;
//=================时、分==========================
    ReadPara(RTC_HOUR_AND_MINIT_FEB) = (RTC_TimeStruct.RTC_Hours*100) + RTC_TimeStruct.RTC_Minutes;
//===================秒=============================
    ReadPara(RTC_SECON_FEB) = RTC_TimeStruct.RTC_Seconds;

}


/************************************************************
*	函数名称：	void Time_PollRtc(void)
*
*	对应上位机ID号说明：
*       SET:             |    READ:
*         年； 408       |       年：412
*         月、日：409    |       月、日：413
*         时、分：410    |       时、分：414
*         秒    ：411    |           秒：415
*
************************************************************/
void Time_PollRtc(void)
{
    Time_Resive_ExctUsart();
    Time_Txto_Startxp();
}