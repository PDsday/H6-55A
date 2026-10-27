#include "stm32f4xx_conf.h"
#define  CAN_GLOBALS
#include "ExternPrototype.h"
#include "adc_config.h"
#include "dac_config.h"
#include "sdio_sdcard.h"
#include "rtc.h"

//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

ErrorStatus HSEStartUpStatus;

u8  InitDly1 = 0;
u8  InitDly2 = 0;
u8  SendParaPackToDsp_Flag = 0;

extern u16 SaveMode;
extern void DSP_Programer(void);
extern void SSI_Configuration(void);
extern u8 enc28j60_mac[6];
extern s8 err_rst;

extern void IIM42652_I2c_Bus_Init(void);
extern void MPU6050_I2c_Bus_Init(void);

/*******************************************************************************
* 函数名 : RCC_Configuration
* 描  述 : 配置不同的系统时钟
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void RCC_Configuration(void)
{
    /* 允许GPIOx时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

//	以下时钟已在各自外设的初始化函数中开启，此处不再重复：
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);							//CANOpen协议栈定时器TIM2
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);							//定时器T3
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);							//定时器T5
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);							//TIM8更新事件触发DAC转换
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);							//以太网控制芯片ENC28J60 SPI1通讯
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);							//SSI编码器 SPI2通讯
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);  							//ZLG7289 SPI3通讯
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);							//ADC3模拟输入
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);								//DAC模拟输出
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);							//CAN1通讯
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);							//与上位机的RS485通讯
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);							//与DSP的内部串口通讯
// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);							//与充电器的RS485通讯
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);							//与上位机的RS485通讯
// 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);							//DMA1(SPI2、USART2、USART3)
// 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);							//DMA2(ADC3)
}

/*******************************************************************************
* 函数名 : NVIC_Configuration
* 描  述 : 配置中断向量表基地址
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		//1 bits for抢占优先级;3 bits for子优先级
}

void ReadDspStatus(void)
{
    static u16 cnt0 = 0;
    static u16 cnt1 = 0;

    while((cnt0 < 50) && (cnt1 < 50))
    {
        if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5) == 1)	//读PA.2口
        {
            cnt1 ++;
            cnt0 = 0;
        }
        else
        {
            cnt1 = 0;
            cnt0 ++;
        }
    }

    if(cnt0 >= 50)
    {
        SendParaPackToDsp_Flag = 0;		//ARM冷复位
    }
    else if(cnt1 >= 50)
    {
        SendParaPackToDsp_Flag = 1;		//ARM热复位
    }

    cnt0 = 0;
    cnt1 = 0;
}

void HwInit(void)
{
    RCC_Configuration();				//配置系统时钟
    NVIC_Configuration();				//配置优先级
	
	PWR_PVDLevelConfig(PWR_PVDLevel_7);	//设置电压检测器检测阈值：PWR_PVDLevel_7 = 2.9V，即主控板VDD大于2.9V(持续100ms)后才允许后续程序的执行
	PWR_PVDCmd(ENABLE);
	
	{
		u32 Delay = 0;
		u32 PVDLevel_Cnt = 0;			//电压检测器计数器
		
		while (PVDLevel_Cnt < 2777)
		{	
			if ((PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET))
			{
				PVDLevel_Cnt = 0;
			}
			else
			{
				PVDLevel_Cnt ++;
			}
			
			Delay = (long long)Delay * 20 * 22 / 8 / 13 / 19;
		}
	}
	
	GPIO_Configuration();				//GPIO口配置
	
	CL2351_GPIO_Configuration();
	
	{
		u32 Delay = 0;
		u32 j = 0;
		
		while ((READ_DSP_PROGRAMER == 0) && (j++ < 100002));		//TAG
		{		
			Delay = (long long)Delay * 20 * 22 / 6 / 5 / 15;
			
			if (j > 100000)		
			{
				DSP_Programer(); 		//进入DSP编程模式
				while (1);
			}
		}
	}
	
	PWR3_Configuration();				//PWR3口配置
	
	{
		u32 Delay = 0;
		u32 PWR3_Cnt = 0;
		
		while (PWR3_Cnt < 277)
		{	
			if (READ_PWR3 == 0)
			{
				PWR3_Cnt = 0;
			}
			else
			{
				PWR3_Cnt ++;
			}
			
			Delay = (long long)Delay * 20 * 22 / 8 / 17 / 8;
		}
	}

    GPIO_Configuration1();
    ReadDspStatus();
    TIM3_Configuration();				//T3：为程序提供100us、300us、500us、1000us时基
	TIM4_Configuration(); 				//T4：DI16高频输入口脉冲捕获
    TIM5_Configuration();				//T5：为用户逻辑提供1ms时基
	TIM7_Configuration();				//T7：为低速任务提供1ms时基
    ExUart1_Configuration();			//USART1：与上位机的RS485通讯
    InUart_Configuration();				//USART2：与DSP的内部串口通讯
    InUart3_Configuration();			//USART3：与充电器的RS485通讯
    ExUart6_Configuration();			//USART6：与上位机的RS485通讯
    //SSI_Configuration();				//SPI2：SSI编码器通讯 
    ADC_Configuration();				//ADC3：模拟输入
    SD_Card_Init();						//SD卡初始化
    My_RTC_Init();		 		        //RTC初始化
}

void SwInit(void)
{
    ParaInit();							//内部参数初始化
	Sd_CardReErrData_Int();
	ReadErrRecord(); 					//上电调出一次历史故障
    CANopenParaInit();
	//IIM42652_I2c_Bus_Init();    		//Gyro_IIM42652初始化
}

u32 cpu_sr = 0;
u32 Isr_Test = 0;
void SysINit(void)
{
    CPU_CRITICAL_ENTER();      	// 关总中断
    HwInit();
    SwInit();

    //if (ReadPara(CAN_SEL))
	if (CANAddr)
    {
        CommuType = CANopen;   	// 设置通讯方式为CANopen
        CANopen_EN = 1;        	// CANopen打开
    }
    else
    {
        CommuType =  CAN232;   	// 设置通讯方式为CAN232
        CANopen_EN = 0;        	// CANopen关闭
    }
	
    CPU_CRITICAL_EXIT();        // 开总中断

    if (CANopen_EN)
    {
        canInit(500);          									// Initialize the CAN/CANopen bus	ReadPara(CAN232_BAUD)
        initTimer();                    						// Start timer for the CANopen stack
        setNodeId (&FD_ObjDict_Data, ModbusAddr);				// Init the Node ID 实际节点号+10
        setState(&FD_ObjDict_Data, Initialisation);				// Init the state ji
		
		UserPLCInit();
    }
    else
    {
        canInit(500); 											// 星辰上位机协议通讯速率固定为500K
    }
}

u8 CANBus_Init(u8 tmpNodeID, u16 bitrate)
{
    canInit(500);
    return (1);
}
