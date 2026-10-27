#ifndef	_IO_
#define	_IO_

#include "stm32f4xx_conf.h"
#ifdef IO_GLOBALS 
    #define  IO_EXT 
#else 
    #define  IO_EXT extern 
#endif

/* WKO ARM输出握手信号 通知DSP进入等待数据打包*/
#define	GPIO_WKO 	    		GPIOE	
#define	GPIO_WKO_PIN 			GPIO_Pin_3
#define WKO_HIGH     			GPIO_WriteBit(GPIO_WKO, GPIO_WKO_PIN, (BitAction)1)   
#define WKO_LOW      			GPIO_WriteBit(GPIO_WKO, GPIO_WKO_PIN, (BitAction)0) 

/* WKI ARM接收DSP回复*/
#define	GPIO_WKI 	    		GPIOE	
#define	GPIO_WKI_PIN 			GPIO_Pin_4
#define READ_WKI       			GPIO_ReadInputDataBit(GPIO_WKI, GPIO_WKI_PIN) 

/* LED测试灯 */
#define	GPIO_LED 	    		GPIOE	
#define	GPIO_LED_PIN 			GPIO_Pin_2
#define LEDON       			GPIO_WriteBit(GPIO_LED, GPIO_LED_PIN, (BitAction)1)   
#define LEDOFF      			GPIO_WriteBit(GPIO_LED, GPIO_LED_PIN, (BitAction)0) 

/* DO8反馈输入信号 */
#define	GPIO_DO8FEB 	    	GPIOA	
#define	GPIO_DO8FEB_PIN 		GPIO_Pin_2
#define READ_DO8FEB      		GPIO_ReadInputDataBit(GPIO_DO8FEB, GPIO_DO8FEB_PIN) 

/* DI16高速输入信号 */
#define	GPIO_DI16 	    		GPIOB	
//#define	GPIO_DI16_PIN 		GPIO_Pin_8
#define	GPIO_DI16_PIN 			GPIO_Pin_6
#define READ_DI16      			GPIO_ReadInputDataBit(GPIO_DI16, GPIO_DI16_PIN) 

/* PWR3输入信号 */
#define	GPIO_PWR3 	    		GPIOC	
#define	GPIO_PWR3_PIN 			GPIO_Pin_4
#define READ_PWR3      			GPIO_ReadInputDataBit(GPIO_PWR3, GPIO_PWR3_PIN) 

/* DSP编程模式选择 */
#define	GPIO_DSP_PROGRAMER 	    GPIOA				//DSP编程模式选择引脚定义
#define	GPIO_DSP_PROGRAMER_PIN 	GPIO_Pin_15			//DSP编程模式选择引脚定义
#define READ_DSP_PROGRAMER      GPIO_ReadInputDataBit(GPIO_DSP_PROGRAMER, GPIO_DSP_PROGRAMER_PIN) 
#define TAGHIGH       			GPIO_WriteBit(GPIO_DSP_PROGRAMER, GPIO_DSP_PROGRAMER_PIN, (BitAction)1) 

/* 底座风机(IGBT散热风机)故障检测输入 */
#define	GPIO_FAN0IN 	    	GPIOD	
#define	GPIO_FAN0IN_PIN 		GPIO_Pin_8
#define READ_FAN0IN      		GPIO_ReadInputDataBit(GPIO_FAN0IN, GPIO_FAN0IN_PIN) 

/* 内腔风机(抱闸散热风机)故障检测输入 */
#define	GPIO_FAN1IN 	    	GPIOG	
#define	GPIO_FAN1IN_PIN 		GPIO_Pin_4
#define READ_FAN1IN      		GPIO_ReadInputDataBit(GPIO_FAN1IN, GPIO_FAN1IN_PIN) 

/* AI0电流/电压可编程切换输出 */
#define	GPIO_AI0CONTROL 	    GPIOC	
#define	GPIO_AI0CONTROL_PIN 	GPIO_Pin_5
#define AI0_CULIN        		GPIO_WriteBit(GPIO_AI0CONTROL, GPIO_AI0CONTROL_PIN, (BitAction)1)  
#define AI0_VOLIN      			GPIO_WriteBit(GPIO_AI0CONTROL, GPIO_AI0CONTROL_PIN, (BitAction)0)  

/* AI1电流/电压可编程切换输出 */
#define	GPIO_AI1CONTROL 	    GPIOB	
#define	GPIO_AI1CONTROL_PIN 	GPIO_Pin_0
#define AI1_CULIN        		GPIO_WriteBit(GPIO_AI1CONTROL, GPIO_AI1CONTROL_PIN, (BitAction)1)  
#define AI1_VOLIN      			GPIO_WriteBit(GPIO_AI1CONTROL, GPIO_AI1CONTROL_PIN, (BitAction)0)  

/* AI2电流/电压可编程切换输出 */
#define	GPIO_AI2CONTROL 	    GPIOB	
#define	GPIO_AI2CONTROL_PIN 	GPIO_Pin_1
#define AI2_CULIN        		GPIO_WriteBit(GPIO_AI2CONTROL, GPIO_AI2CONTROL_PIN, (BitAction)1)  
#define AI2_VOLIN      			GPIO_WriteBit(GPIO_AI2CONTROL, GPIO_AI2CONTROL_PIN, (BitAction)0)  

/* 内腔风机控制输出 */
#define	GPIO_FAN1OUT 	    	GPIOA	
#define	GPIO_FAN1OUT_PIN 		GPIO_Pin_8
#define FANON       			GPIO_WriteBit(GPIO_FAN1OUT, GPIO_FAN1OUT_PIN, (BitAction)0)   
#define FANOFF      			GPIO_WriteBit(GPIO_FAN1OUT, GPIO_FAN1OUT_PIN, (BitAction)1) 

/* 24V_BK输出 */
#define	GPIO_IO_BK 	    		GPIOE	// 引脚定义
#define	GPIO_IO_BK_PIN 			GPIO_Pin_8	// 引脚定义 
#define BKON        			GPIO_WriteBit(GPIO_IO_BK, GPIO_IO_BK_PIN, (BitAction)1)  
#define BKOFF      				GPIO_WriteBit(GPIO_IO_BK, GPIO_IO_BK_PIN, (BitAction)0)   

/* SSI_24V供电源输出选择 */
#define	GPIO_SSI_24V	    	GPIOG	
#define	GPIO_SSI_24V_PIN 		GPIO_Pin_7
#define SSI_24V_ON        		GPIO_WriteBit(GPIO_SSI_24V, GPIO_SSI_24V_PIN, (BitAction)0)  
#define SSI_24V_OFF      		GPIO_WriteBit(GPIO_SSI_24V, GPIO_SSI_24V_PIN, (BitAction)1)  

/* 输入口锁存信号输出 */
#define	GPIO_IO_IN_ST 	    	GPIOE	// 引脚定义
#define	GPIO_IO_IN_ST_PIN 		GPIO_Pin_12	// 引脚定义 
#define SET_IO_IN_ST        	GPIO_WriteBit(GPIO_IO_IN_ST, GPIO_IO_IN_ST_PIN, (BitAction)1) 
#define CLEAR_IO_IN_ST      	GPIO_WriteBit(GPIO_IO_IN_ST, GPIO_IO_IN_ST_PIN, (BitAction)0) 

/* IO时钟输出 */
#define	GPIO_IO_CLK 	  		GPIOE	// 引脚定义
#define	GPIO_IO_CLK_PIN   		GPIO_Pin_11	// 引脚定义 
#define SET_IO_CLK        		GPIO_WriteBit(GPIO_IO_CLK, GPIO_IO_CLK_PIN, (BitAction)1)  
#define CLEAR_IO_CLK      		GPIO_WriteBit(GPIO_IO_CLK, GPIO_IO_CLK_PIN, (BitAction)0)  

/* 输出口使能输出 */
#define	GPIO_IO_OUT_EN 	     	GPIOE	// 引脚定义
#define	GPIO_IO_OUT_EN_PIN   	GPIO_Pin_13	// 引脚定义 
#define SET_IO_OUT_EN        	GPIO_WriteBit(GPIO_IO_OUT_EN, GPIO_IO_OUT_EN_PIN, (BitAction)1)  
#define CLEAR_IO_OUT_EN      	GPIO_WriteBit(GPIO_IO_OUT_EN, GPIO_IO_OUT_EN_PIN, (BitAction)0)  

/* 输出口锁存信号输出 */
#define	GPIO_IO_OUT_ST 	     	GPIOE	// 引脚定义
#define	GPIO_IO_OUT_ST_PIN   	GPIO_Pin_14	// 引脚定义 
#define SET_IO_OUT_ST        	GPIO_WriteBit(GPIO_IO_OUT_ST, GPIO_IO_OUT_ST_PIN, (BitAction)1)  
#define CLEAR_IO_OUT_ST      	GPIO_WriteBit(GPIO_IO_OUT_ST, GPIO_IO_OUT_ST_PIN, (BitAction)0) 

/* 数据输出 */
#define	GPIO_IO_TX 	     		GPIOE	// 引脚定义
#define	GPIO_IO_TX_PIN   		GPIO_Pin_9	// 引脚定义 
#define SET_IO_TX        		GPIO_WriteBit(GPIO_IO_TX, GPIO_IO_TX_PIN, (BitAction)1) 
#define CLEAR_IO_TX      		GPIO_WriteBit(GPIO_IO_TX, GPIO_IO_TX_PIN, (BitAction)0)  

/* 数据输入 */
#define	GPIO_IO_RX 	     		GPIOE	// 引脚定义
#define	GPIO_IO_RX_PIN   		GPIO_Pin_10	// 引脚定义 
#define READ_IO_RX       		GPIO_ReadInputDataBit(GPIO_IO_RX, GPIO_IO_RX_PIN)  

struct	IO_IN_BIT
{
	u16 IN0:1;	//BIT0
	u16 IN1:1;	//BIT1
	u16 IN2:1;	//BIT2
	u16 IN3:1;	//BIT3
	u16 IN4:1;	//BIT4
	u16 IN5:1;	//BIT5
	u16 IN6:1;	//BIT6
	u16 IN7:1;	//BIT7
};
union	IO_IN{
unsigned int all;
struct	IO_IN_BIT	bit;
};

struct	DI_BIT
{
	u16 DI00	:1;
	u16 DI01	:1;
	u16 DI02	:1;
	u16 DI03	:1;
	u16 DI04	:1;
	u16 DI05	:1;
	u16 DI06	:1;
	u16 DI07	:1;
	u16 DI08	:1;
	u16 DI09	:1;
	u16 DI10	:1;
	u16 DI11	:1;
	u16 DI12	:1;
	u16 DI13	:1;
	u16 DI14	:1;
	u16 DI15	:1;
	u16 DI16	:1;
	u16 DI17	:1;
	u16 DI18	:1;
	u16 DI19	:1;
	u16 DI20	:1;
	u16 DI21	:1;
	u16 DI22	:1;
	u16 DI23	:1;
	u16 DI24	:1;
	u16 DI25	:1;
	u16 DI26	:1;
	u16 DI27	:1;
	u16 DI28	:1;
	u16 DI29	:1;
	u16 DI30	:1;
	u16 DI31	:1;			
};
union	DI_STATUS
{
    u32 all;
    struct	DI_BIT	bit;
};

struct TONTOF_CNT
{
	u16 DI00;
	u16 DI01;
	u16 DI02;
	u16 DI03;
	u16 DI04;
	u16 DI05;
	u16 DI06;
	u16 DI07;
	u16 DI08;
	u16 DI09;
	u16 DI10;
	u16 DI11;
	u16 DI12;
	u16 DI13;
	u16 DI14;
	u16 DI15;
	u16 DI16;
	u16 DI17;
	u16 DI18;
	u16 DI19;
	u16 DI20;
	u16 DI21;
	u16 DI22;
	u16 DI23;
	u16 DI24;
	u16 DI25;
	u16 DI26;
	u16 DI27;
	u16 DI28;
	u16 DI29;
	u16 DI30;
	u16 DI31;
};

#define TON_FILTER_MS 2														//DI状态滤波：2ms
#define TOF_FILTER_MS 2														//DI状态滤波：2ms
#define TON_FILTER_MS_ENPO 2												//ENPO高状态滤波：2ms
#define TOF_FILTER_MS_ENPO 10												//ENPO低状态滤波：10ms
#define TON_FILTER_TIMEBASE 300												//DI状态函数周期：300us
#define TOF_FILTER_TIMEBASE 300												//DI状态函数周期：300us
#define TON_FILTER ((TON_FILTER_MS*1000/TON_FILTER_TIMEBASE)+1)				//DI状态滤波系数：2ms对应2000/300+1=7次
#define TOF_FILTER ((TOF_FILTER_MS*1000/TOF_FILTER_TIMEBASE)+1)				//DI状态滤波系数：2ms对应2000/300+1=7次
#define TON_FILTER_ENPO ((TON_FILTER_MS_ENPO*1000/TON_FILTER_TIMEBASE)+1)	//ENPO高状态滤波系数：2ms对应2000/300+1=7次
#define TOF_FILTER_ENPO ((TOF_FILTER_MS_ENPO*1000/TOF_FILTER_TIMEBASE)+1)	//ENPO低状态滤波系数：10ms对应10000/300+1=34次
#define TON_IDADDR 333														//拨码开关地址滤波
#define TOF_IDADDR 333														//拨码开关地址滤波

IO_EXT u16 MoutValue[33];     // 定义全局变量

IO_EXT bool OD0;	  // 
IO_EXT bool OD1;	  // 
IO_EXT bool OD2;	  // 
IO_EXT bool OD3;	  // 
IO_EXT bool OD4;	  // 
IO_EXT bool OD5;	  // 
IO_EXT bool OD6;	  // 
IO_EXT bool OD7;	  //
IO_EXT bool OD8;	  // 
IO_EXT bool OD9;	  // 
IO_EXT bool OD10;	  // 
IO_EXT bool OD11;	  // 
IO_EXT bool OD12;	  // 
IO_EXT bool OD13;	  // 
IO_EXT bool OD14;	  //
IO_EXT bool OD15;	  //

IO_EXT bool SAFE;	 
IO_EXT bool BK;		  
IO_EXT bool BKOUT;		  
IO_EXT bool DefectCodeClear;
IO_EXT bool SafeRunErrFlag; // 安全收桨故障标志：1有安全收桨故障;
IO_EXT bool StopRunErrFlag; //减速停机标志：1有安全收桨故障;
IO_EXT bool NormRunErrFlag; // 安全收桨故障标志：1有安全收桨故障;
IO_EXT bool SnStopErrFlag; // 封锁脉冲关使能故障标志：1有安全收桨故障;
IO_EXT bool OpenRunErrFlag; // 封锁脉冲关使能故障标志：1有安全收桨故障;

#endif