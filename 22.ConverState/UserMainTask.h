#ifndef  __USERMAINTASK_H__
#define  __USERMAINTASK_H__

#include "stm32f4xx.h"
#include "UserRegDefine.h"
#include "io.h"
#include "AppFunc.h"

#ifdef USER_GLOBALS 
    #define  USER_EXT 
#else 
    #define  USER_EXT extern 
#endif

#define PLC_STA_STOP       	1 	// 停机抱闸
#define PLC_STA_PLOOP     	2 	// 位置环运行
#define PLC_STA_SAFE       	3 	// 安全收桨
#define PLC_STA_VLOOP      	5 	// 速度环运行	
#define PLC_STA_HAND		11	// 手动模式
#define PLC_STA_OPENLOOP   	20 	// 开环收桨

#define P_LOOP 				1 	// 位置环
#define V_LOOP 				2 	// 速度环
#define I_LOOP 				3 	// 电流环

USER_EXT bool User_OD0;	     
USER_EXT bool User_OD1;	   
USER_EXT bool User_OD2;	   
USER_EXT bool User_OD3;	   
USER_EXT bool User_OD4;	   
USER_EXT bool User_OD5;	   
USER_EXT bool User_OD6;	   
USER_EXT bool User_OD7;	  
USER_EXT bool User_OD8;	  
USER_EXT bool User_OD9;	  
USER_EXT bool User_OD10;	  
USER_EXT bool User_OD11;	  
USER_EXT bool User_OD12;	  
USER_EXT bool User_OD13;	  
USER_EXT bool User_OD14;	  
USER_EXT bool User_OD15;	  

USER_EXT s16 User_target_value; 		// 给定指令 target_value = (RPM * 0x6C00) / max_v;其中RPM为目标转速，max_v为系统最高转速
USER_EXT u16 User_sn_on;        		// 当sn_on = 0xAA55时开使能
USER_EXT u16 User_sn_off;       		// 当sn_off = 0x55AA时关使能
USER_EXT u16 User_work_mod;       		// 工作模式选择：1为位置环，2为速度环，3为力矩环
USER_EXT u16 RunEnable;				 	// 驱动器使能允许
USER_EXT u16 ChargeRunEnable; 			// 充电器使能允许
USER_EXT u16 sn_on_buf;        			// 当sn_on = 0xAA55时开使能
USER_EXT u16 sn_off_buf;       			// 当sn_off = 0x55AA时关使能
USER_EXT u16 User_PLC_Sta; 				// 用户模拟PLC工作状态
USER_EXT u16 User_PLC_StaOld; 			// 用户模拟PLC上一次工作状态
USER_EXT u16 User_PLC_Step; 			// 用户模拟PLC工作步
USER_EXT u16 v_up_time_buf;      		// 速度环上升时间 单位10ms	
USER_EXT u16 v_down_time_buf;     		// 速度环下降时间 单位10ms	
USER_EXT bool UserSafeFinishFlag; 		// 收桨完成标志;
USER_EXT bool CANopenErrFlag; 			// CANopen通讯故障标志：1故障;

#endif