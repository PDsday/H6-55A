#define ADC_CONF_GLOBALS
#include "adc_config.h"
#include "stm32f4xx_conf.h"

/*******************************************************************************
* 函数名 : ADC_RCC_Configuration
* 描  述 : 配置ADC系统时钟
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void ADC_RCC_Configuration(void)
{
    /* 允许GPIOC、GPIOF时钟 */
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOA, ENABLE);

    /* 允许ADC3时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

    /* 允许DMA2时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
}

/*******************************************************************************
* 函数名 : ADC_NVIC_Configuration
* 描  述 : 配置ADC中断向量表基地址
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void ADC_NVIC_Configuration(void)
{

}

/*******************************************************************************
* 函数名 : ADC_GPIO_Configuration
* 描  述 : 配置ADC_GPIO端口
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void ADC_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //与STM32F10X不同，STM32F40X使用端口复用时需要使用GPIO_PinAFConfig函数进行指定引脚的挂载,
    //除ADC和DAC外的所有非GPIO功能，都要使用GPIO_PinAFConfig函数进行端口复用，端口复用没有时钟，直接使用即可

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名 : ADC_Configuration
* 描  述 : 配置ADC
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void ADC_Configuration(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;

    /* 配置ADC_RCC */
    ADC_RCC_Configuration();

    /* 配置ADC_NVTC */
    ADC_NVIC_Configuration();

    /*ADC_GPIOs */
    ADC_GPIO_Configuration();

    /* 配置DMA2 通道2 数据流0 作为ADC数据传输*/
    DMA_DeInit(DMA2_Stream0);
    DMA_InitStructure.DMA_Channel = DMA_Channel_2;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC3->DR;                 	//外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&ADC_ConvValue;                //内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     //dma传输方向单向
    DMA_InitStructure.DMA_BufferSize = ADC3_LEN;                                //设置DMA在传输时缓冲区的长度：
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //设置DMA的内存递增模式，
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         //内存数据字长
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             //设置DMA的传输模式：连续不断的循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);

    /* ADC Common Init **********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC3 Init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
    ADC_InitStructure.ADC_NbrOfConversion = ADC3_LEN;
    ADC_Init(ADC3, &ADC_InitStructure);

    /*
    PF.03：T0
    PF.04：T3
    PF.05：T2
    PF.06：T1
    PF.07：T4
    PF.08：T5(DJWD)
    PF.09：AI1P
    PF.10：AI0P
    PC.00：REF0
    PC.03：IBK
    */
    /* ADC3规则模式通道配置 */													// 对应原理图定义
    ADC_RegularChannelConfig(ADC3, ADC_Channel_9,  1, ADC_SampleTime_28Cycles); // T0
    ADC_RegularChannelConfig(ADC3, ADC_Channel_4,  2, ADC_SampleTime_28Cycles); // T1
    ADC_RegularChannelConfig(ADC3, ADC_Channel_15, 3, ADC_SampleTime_28Cycles); // T2
    ADC_RegularChannelConfig(ADC3, ADC_Channel_14, 4, ADC_SampleTime_28Cycles); // T3
    ADC_RegularChannelConfig(ADC3, ADC_Channel_5,  5, ADC_SampleTime_28Cycles); // T4
    ADC_RegularChannelConfig(ADC3, ADC_Channel_6,  6, ADC_SampleTime_28Cycles); // T5(DJWD)
    ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 7, ADC_SampleTime_28Cycles); // REF0
    ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 8, ADC_SampleTime_28Cycles); // IBK
    ADC_RegularChannelConfig(ADC3, ADC_Channel_7,  9, ADC_SampleTime_28Cycles); // AI1P
    ADC_RegularChannelConfig(ADC3, ADC_Channel_8,  10, ADC_SampleTime_28Cycles);// AI0P
	ADC_RegularChannelConfig(ADC3, ADC_Channel_3,  11, ADC_SampleTime_28Cycles);// AI2P

    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

    /* 允许ADC3进行DMA传送*/
    ADC_DMACmd(ADC3, ENABLE);

    /* 允许ADC3 */
    ADC_Cmd(ADC3, ENABLE);
    ADC_SoftwareStartConv(ADC3);
}