/******************** (C) COPYRIGHT 2010 ********************
* File Name          : sys_config.c
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

#ifndef  __FLASH_H__
#define  __FLASH_H__

/* 定义系统参数结构体类型 */
typedef struct
{
    vu32 SysWarning;
    vu32 SysStatus;
} PARAMETERS_TypeDef;
/* */

#define ParametersStartAddr  ((u32)0x08030000)          // 参数保存FLASH起始地址
#define ParametersEndAddr    ((u32)0x0803FFFF)          // 参数保存FLASH结束地址

#define FLASH_PAGE_SIZE    ((u16)0x800)                 // FLASH页大小

#define SYSTEM_PARA          ((PARAMETERS_TypeDef *)ParametersStartAddr)

/* Includes ------------------------------------------------------------------*/

#include "stm32f10x_map.h"

void SavePara(PARAMETERS_TypeDef* SysPara, u16 SaveSize);
u32 ReadPara(u32 ReadAddr);

#endif

/******************* (C) COPYRIGHT 2010  *****END OF FILE****/
