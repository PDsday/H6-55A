
#include "stm32f4xx_conf.h"
#include "dac_config.h"

/*******************************************************************************
* 函数名 : DAC_RCC_Configuration
* 描  述 : 配置DAC系统时钟
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void DAC_RCC_Configuration(void)
{
    /* 允许GPIOA时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* 允许DAC时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    /* 允许TIM8时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
}

/*******************************************************************************
* 函数名 : DAC_NVIC_Configuration
* 描  述 : 配置DAC中断向量表基地址
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void DAC_NVIC_Configuration(void)
{

}

/*******************************************************************************
* 函数名 : DAC_GPIO_Configuration
* 描  述 : 配置DAC_GPIO端口
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void DAC_GPIO_Configuration(void)		//注意：PA5与以太网SPI1有冲突
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 配置GPIO: PA.4(DAC1), PA.5(DAC2)为模拟输入 */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名 : DAC_Configuration
* 描  述 : 配置DAC
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void DAC_Configuration(void)
{
    DAC_InitTypeDef            DAC_InitStructure;
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

    /* 配置DAC_RCC */
    DAC_RCC_Configuration();

    /* 配置DAC_NVTC */
    DAC_NVIC_Configuration();

    /* 配置DAC_GPIOs */
    DAC_GPIO_Configuration();

    /* 配置TIM8 */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 0x19;                    // 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;                  // 设置时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 设置时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 选择计数器模式: 递增
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    /* 选择TIM8更新事件触发DAC转换 */
    TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);

    /* TIM8 enable counter */
    TIM_Cmd(TIM8, ENABLE);

    /* DAC channel1 Configuration */
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T8_TRGO;            // 软件触发
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None; // 无自动波形输出模式
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;   // 开启输出缓存，减少输出阻抗，提高驱动能力
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    /* DAC channel2 Configuration */
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
     automatically connected to the DAC converter. */
    DAC_Cmd(DAC_Channel_1, ENABLE);
    /* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is
     automatically connected to the DAC converter. */
    DAC_Cmd(DAC_Channel_2, ENABLE);
}
