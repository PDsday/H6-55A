#ifndef  __ADC_CONFIG_H__
#define  __ADC_CONFIG_H__
#include "stm32f4xx_conf.h"

#ifdef ADC_CONF_GLOBALS 
    #define  ADC_CONF_EXT 
#else 
    #define  ADC_CONF_EXT extern 
#endif

// ±í 54. ADC È«¾Ö¼Ä´æÆ÷Ó³Éä 
// Æ«ÒÆ ¼Ä´æÆ÷
// 0x000 - 0x04C ADC1
// 0x050 - 0x0FC Reserved
// 0x100 - 0x14C ADC2
// 0x118 - 0x1FC Reserved
// 0x200 - 0x24C ADC3
// 0x250 - 0x2FC Reserved
// 0x300 - 0x308 Í¨ÓÃ¼Ä´æÆ÷

// 0x40000000
// 0x00010000
// 0x00002200
// 0x0000004C
// =
// 0x4001224C

//#define ADC3_LEN 10
#define ADC3_LEN 11
#define ADC3_DR_Address	0x4001224C 		//(ADC3_BASE + 0x4C)
//ADC_CONF_EXT vu16 ADC_ConvValue[10];
ADC_CONF_EXT vu16 ADC_ConvValue[11];

void ADC_Configuration(void);

#endif