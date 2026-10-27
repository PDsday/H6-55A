/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */


// STM32 implementation of the  CANopen timer driver, uses Timer 2 (16 bit)

// Includes for the CANopen driver

// #include "stm32f10x_lib.h"
#include "can_STM32.h"	   //LED HW

#include "stm32f4xx_conf.h"
#include "CANopen.h"
#include "timer.h"


TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

/************************** Modul variables **********************************/
// Store the last timer value to calculate the elapsed time
static TIMEVAL last_time_set = TIMEVAL_MAX; 
//volatile static TIMEVAL last_time_set = TIMEVAL_MAX;    

//static u16 CANopen_LED_cnt = 0;			   //50mS +1


void initTimer(void)
/******************************************************************************
Initializes the timer, turn on the interrupt and put the interrupt time to zero
INPUT	void
OUTPUT	void
******************************************************************************/
{

 
 NVIC_InitTypeDef NVIC_InitStructure;

  
  //-----------------------CAN\CANopen CLK----------------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

  //--------------------------------------------------------------------
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //---------------------------------------------------------------------


  //TIM_SetCompare1(TIM1, 0);		//CCR1
  //TIM_SetCompare1(TIM2, 0xFFFE);
	// Set timer for CANopen operation tick 8us max, time is 524ms , TIM_Prescaler = 287
	//Set timer for CANopen operation tick 1us max, time is 65ms  ,  TIM_Prescaler = 35

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  TIM_TimeBaseStructure.TIM_Prescaler = 71;		//分频,36MHZ /(35+1)=1MHz  ,1uS tick  ;; //36MHZ /(287+1)=125KHz  ,8uS tick	 ;;//72MHZ /(575+1)=125KHz  ,8uS tick
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  //向上计数

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  //TIM_PrescalerConfig(TIM2, 71, TIM_PSCReloadMode_Immediate);	  //立即更新

  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Active; //TIM_OCMode_Toggle;  //TIM_OCMode_Timing;			  //纯计时模式，不输出引脚电平
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //TIM_OutputState_Enable;	 //TIM_OutputState_Disable;
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);	   //CANopen Main Timer	OC1

//  TIM_OC2Init(TIM2, &TIM_OCInitStructure);
//  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);	   //CANopen LED OC2

  //--------
  TIM_ARRPreloadConfig(TIM2,ENABLE);		 //---自动重载ARR



    /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_CC1 , ENABLE);			//------IT
//  TIM_ITConfig(TIM2, TIM_IT_CC2 , ENABLE);			//------IT

      /* Clear TIM2 Capture Compare1 interrupt pending bit*/
  TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
//  TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);


  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);							//--启动定时器

   TIM_SetCompare1(TIM1, 0xFFFF);		//CCR1 INIT
//   TIM_SetCompare2(TIM1, 0xC350);		//CCR2 INIT	  ,50000uS=50mS IRQ
}

void setTimer(TIMEVAL value)
/******************************************************************************
Set the timer for the next alarm.
INPUT	value TIMEVAL (unsigned long)
OUTPUT	void
******************************************************************************/
{
//  TimerAlarm += (int)value;	// Add the desired time to timer interrupt time
u16 capture = 0;
//capture = TIM_GetCapture1(TIM2);
//TIM_SetCompare1(TIM2, (capture + (u16)value)%0xFFFF);
capture = TIM_GetCounter(TIM2);
//TIM_SetCompare1(TIM2, capture + (u16)value);
TIM_SetCompare1(TIM2, (u16)(capture + (u16)value));
//TIM_SetCompare1(TIM2, (capture + (u16)value)%0xFFFF);
}


TIMEVAL getElapsedTime(void)
/******************************************************************************
Return the elapsed time to tell the Stack how much time is spent since last call.
INPUT	void
OUTPUT	value TIMEVAL (unsigned long) the elapsed time
******************************************************************************/
{
  unsigned int timer = TIM_GetCounter(TIM2);            // Copy the value of the running timer
  if (timer > last_time_set)                    // In case the timer value is higher than the last time.
    return (timer - last_time_set);             // Calculate the time difference
  else if (timer < last_time_set)
    //return (last_time_set - timer);             // Calculate the time difference
	return ((TIMEVAL_MAX - last_time_set) + timer);		//--S
  else
    return TIMEVAL_MAX;

//return ((TIMEVAL_MAX - last_time_set) + timer);

}


//extern UNS16 Max_motor_speed;
void TIM2_IRQHandler(void)
/******************************************************************************
Interruptserviceroutine Timer 2 Compare 1 for the CANopen Timer
******************************************************************************/
{

if (TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET)		  //CANopen Main Timer
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

  last_time_set = TIM_GetCounter(TIM2);
  //last_time_set = TIM_GetCapture1(TIM2);
  TimeDispatch();                               // Call the time handler of the stack to adapt the elapsed time
  //TIM_SetCompare1(TIM2, (u16)(TIM_GetCapture1(TIM2)+1000)%0xFFFF);
  //TIM_SetCounter(TIM2,0);

  }

  //while (Max_motor_speed == 2200);


//-------------------------

//  if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//  {
//    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//  }


}
