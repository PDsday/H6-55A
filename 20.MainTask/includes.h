/******************** (C) COPYRIGHT 2010 Stars             ********************
* File Name          : includes.c
* Author             : Dach
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

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#include   "stdio.h"
#include   "UserMainTask.h"
// #include   "UserRegDefine.h"
// #include   "StarsType.h"

/* 外部函数声明 */
void SysINit(void);      // 系统初始化函数

/*******************************************************************************
* 函数名 : AddUserTask
* 描  述 : 添加用户任务
* 输  入 : u32* pUserTask         32位用户任务函数指针
*          u16 UesrTaskLoopTime	  16位用户任务循环时间，单位为ms
           u8  UesrTaskPriority   任务优先级
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void AddUserTask(void (*pUserTask)(), u16 UesrTaskLoopTime, u8 UesrTaskPriority);

/* PLC函数声明 ****************************************************************/
/*******************************************************************************
* 函数名 : PLC_TON
* 描  述 : PLC TON函数
* 输  入 : u8* PLC_TONOut    PLC TON函数输出布尔变量指针
		   u8* PLC_TONIn	    PLC TON函数输入布尔变量指针
		   u32 PLC_TON_Time 	PLC TON函数输出延时时间(ms)
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_TON(u8* PLC_TONOut, u8* PLC_TONIn, u32 PLC_TON_Time);

/*******************************************************************************
* 函数名 : PLC_TOF
* 描  述 : PLC TOF函数
* 输  入 : u8* PLC_TOFOut       PLC TOF函数输出布尔变量指针
		   u8* PLC_TOFIn	    PLC TOF函数输入布尔变量指针
		   u32 PLC_TOF_Time 	PLC TOF函数输出延时时间(ms)
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_TOF(u8* PLC_TOFOut, u8* PLC_TOFIn, u32 PLC_TOF_Time);

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

#endif

/******************* (C) COPYRIGHT 2010 Stars                     *****END OF FILE****/