/******************** (C) COPYRIGHT 2010 ********************
* File Name          : dac_config.c
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

#ifndef  __DAC_CONFIG_H__
#define  __DAC_CONFIG_H__

/* Includes ------------------------------------------------------------------*/
// #include "stm32f10x_map.h"
#include "stm32f4xx_conf.h"
#define DAC_DHR12RD_Address     ((u32)0x40007420)       // DAC  DMA传输地址：双通道右对齐
#define DAC_DHR12LD_Address     ((u32)0x40007424)       // DAC  DMA传输地址：双通道左对齐

void DAC_Configuration(void);

#endif

/******************* (C) COPYRIGHT 2010  *****END OF FILE****/
