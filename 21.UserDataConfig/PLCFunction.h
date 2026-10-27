/******************** (C) COPYRIGHT 2010 ********************
* File Name          : PLCFunction.h
* Author             : 
* Version            : 
* Date               : 
* Description        : 
********************************************************************************
* 
* 
* 
* 
* 
* 
*******************************************************************************/

#ifndef  __PLCFUNCTION_H__
#define  __PLCFUNCTION_H__

/* Includes ------------------------------------------------------------------*/
// #include "stm32f10x_lib.h"
// #include "stm32f10x_map.h"
#include "stm32f4xx_conf.h"

#define  PLC_TON_ADD_TOF_NUM	30	   // 定义PLC TON和TOF总数量
#define  PLC_CLK_TRIG_Q_WIDTH   10000   // 定义边沿测试输出高电平宽度(ms)
#define  PLC_CLK_TRIG_NUM	    20	   // 定义边沿测试数量

typedef struct
{
    u8*  PLC_TON_TOF_Out;
    u8*  PLC_TON_TOF_In;
    u32  PLC_TON_TOF_Time;
	u32  PLC_TON_TOF_TimeCnt;
	u8   PLC_Type;
} PLC_TON_TOF_TypeDef;		  // 定义PLC TON和TOF共用类型

typedef struct
{
    u8*  PLC_CLK_TRIG_Out;
    u8*  PLC_CLK_TRIG_In;
	u8   PLC_CLK_TRIG_InOld;
    u16  PLC_CLK_TRIG_TimeCnt;
	u8   PLC_CLK_TRIG_Type;
} PLC_CLK_TRIG_TypeDef;		  // 定义CLK_TRIG类型

/*******************************************************************************
* 函数名 : PLC_TON
* 描  述 : PLC TON函数
* 输  入 : u8* PLC_TONOut    PLC TON函数输出布尔变量指针
		   u8* PLC_TONIn	    PLC TON函数输入布尔变量指针
		   u32 PLC_TON_Time 	PLC TON函数输出延时时间
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_TON(u8* PLC_TONOut, u8* PLC_TONIn, u32 PLC_TON_Time);

/*******************************************************************************
* 函数名 : PLC_TOF
* 描  述 : PLC TOF函数
* 输  入 : u8* PLC_TOFOut       PLC TOF函数输出布尔变量指针
		   u8* PLC_TOFIn	    PLC TOF函数输入布尔变量指针
		   u32 PLC_TOF_Time 	PLC TOF函数输出延时时间
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_TOF(u8* PLC_TOFOut, u8* PLC_TOFIn, u32 PLC_TOF_Time);

/*******************************************************************************
* 函数名 : PLC_TON_TOF_Process
* 描  述 : PLC TON和TOF后台处理函数
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_TON_TOF_Process(void);

/*******************************************************************************
* 函数名 : PLC_CLK_R_TRIG
* 描  述 : PLC时钟上升沿检测函数
* 输  入 : u8* PLC_CLK_R_TRIG_Q     PLC 上升沿检测输出布尔变量指针
		   u8* PLC_CLK_TRIG_In	    PLC 时钟输入布尔变量指针
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_CLK_R_TRIG(u8* PLC_CLK_R_TRIG_Q, u8* PLC_CLK_TRIG_In);

/*******************************************************************************
* 函数名 : PLC_CLK_F_TRIG
* 描  述 : PLC时钟下升沿检测函数
* 输  入 : u8* PLC_CLK_F_TRIG_Q     PLC 下升沿检测输出布尔变量指针
		   u8* PLC_CLK_TRIG_In	    PLC 时钟输入布尔变量指针
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_CLK_F_TRIG(u8* PLC_CLK_F_TRIG_Q, u8* PLC_CLK_TRIG_In);

/*******************************************************************************
* 函数名 : PLC_CLK_TRIG_Precoss
* 描  述 : PLC时钟检测后台处理函数  500us调用一次
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_CLK_TRIG_Precoss(void);

#endif

/******************* (C) COPYRIGHT 2010  *****END OF FILE****/
