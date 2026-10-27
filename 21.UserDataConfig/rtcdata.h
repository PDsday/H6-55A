#ifndef _RTC_DATE_H
#define _RTC_DATE_H

//单片机头文件

//设置RTC
struct rtc_time
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_week;
};

//读写RTC
struct RW_Time
{
	unsigned short xp_year;		//年
	unsigned short xp_secon;		//秒
	unsigned short xp_monday;		//月、日
	unsigned short xp_hourmin;		//时、分
};

//=============================================================================
//=============================================================================
void Time_Adjust(void);             //用于将接收要校正的时间参数去修改结构体
void Time_Resive_ExctUsart(void);   //接收星辰上位机发送过来数据
void Time_Txto_Startxp(void);       //星辰上位机显示时间
void Time_PollRtc(void);            //RTC任务总函数
	

#endif 