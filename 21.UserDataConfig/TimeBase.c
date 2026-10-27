#include "stm32f4xx_conf.h"
#include "TimeBase.h"
#include "ExternPrototype.h"

extern u16 Display_Cnt;
extern u8 LED_Flash_Flag;
extern u8 ERR_Flash_Flag;
extern u8 ClrCMDStart;
extern u8 ClrFaultFlag;
extern u16 RefCheckFinish_Cnt;
extern u8 err_rst1_delay;
extern u8 EraseErrFlag0;
extern u16 CanLineOffCnt;
extern DefectConfigType DefectConfigParaID[41];
extern u16 SaveParaCntNext;
extern u16 err_rst1_cnt;
extern u16 FD_ObjDict_obj1017;
extern UNS32 FD_ObjDict_obj1016[];
extern u8 FanOnFlag;
extern u16 MotoHotCnt;
extern u16 SaveParaCnt;
extern u16 SaveMode;
extern u16 HeartBeat;
extern u16 HeartBeatOld;
extern u16 Temperature_Channel;
extern u16 SafeRunTime;
extern u16 User_PLC_Sta;
extern u8 err_rst1;
extern u8 MEN_Flash_Flag;
extern u16 FlashValifyErr;
extern u16 BKCurre_OverMax_Cnt;
extern s16 FastVol;
extern u32 ChargeRunEnable_Delay;
extern u32 DisCharge_Delay;
extern u32 CapTestFinishDelay;
extern u8 DataDiffFlag;
extern u16 DefectFlagInH_Delay[];
extern u16 GridVoltage_Over500VAC_KeepOnTime;
extern bool TimeBase_1Secon;
extern bool TimeBase_3Secon_ForDO8;
extern bool TimeBase_5Secon_ForDO8;
extern bool TimeBase_3Secon_ForFan0Err;
extern bool TimeBase_3Secon_ForFan1Err;
extern bool RefCheckFinish;
extern bool GridVoltage_Over500VAC_Flag;
union  SLOWTIMEFLAG   SlowTimeBaseFlag;

u16 SSI_EnableFlag = 0;
u32 tec=0;
u32 boff=0;
u16 boffcnt=0;
u8  boffflag=0;
u16 FD_ObjDict_obj1016_L = 0;
u32 capture1 = 0;
u32 Count1 = 0;
u32 capture2 = 0;
u32 Count2 = 0;
u32 capture3 = 0;
u32 Count3 = 0;
u32 capture4 = 0;
u32 Count4 = 0;
u8 Init_flag = 0;
u8 Init_flag1 = 0;
u8 Init_flag2 = 0;
u8 Init_flag3 = 0;	   		// 驱动器有过一次就绪状态，才允许进行桨叶角度存盘操作
u8 canopen_para_precoss_flag = 0;
u8 charger_para_precoss_flag = 0;
u32 HeartBeatStopCnt = 0;	//心跳停止计数器
u32 NoSyncFrameCnt = 0;

#define  CCR0_Val  50
vu16 CCR1_Val = 720  * 2;	 //100us
vu16 CCR2_Val = 2160 * 2;	 //300us
vu16 CCR3_Val = 3600 * 2;	 //500us
vu16 CCR4_Val = 7200 * 2;	 //1000us

void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		//定时器T3

    /* Enable the TIM3 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM3_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM3_SBUPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_PrescalerConfig(TIM3, 4, TIM_PSCReloadMode_Immediate);

    /* Output Compare Timing Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Active;//TIM_OCMode_Timing;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);

    /* Output Compare Timing Mode configuration: Channel2 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);

    /* Output Compare Timing Mode configuration: Channel3 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);

    /* Output Compare Timing Mode configuration: Channel4 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);

    /* TIM IT enable */
    TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);

	DBGMCU_APB1PeriphConfig(DBGMCU_TIM3_STOP, ENABLE);

    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);
}

void TIM4_Configuration(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);    //使能TIM4时钟  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 	//使能GPIOB时钟
      
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;              //PB6 清除之前设置    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//gpio模式设置为复用模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//设置为推挽
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//不上下拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);	//设置GPIO复用为中断定时器输入
      
    //初始化定时器4 TIM4     
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;              //设定计数器自动重装值   
    TIM_TimeBaseStructure.TIM_Prescaler =0;             	//预分频器     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	 	//TIM向上计数模式  
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);         			//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位  
	
//  TIM_ITRxExternalClockConfig(TIM4,TIM_TS_ETRF);          			//配置外部触发计数，否则不会计数  
	TIM_TIxExternalClockConfig(TIM4,TIM_TIxExternalCLK1Source_TI1,TIM_ICPolarity_Falling,03);//设置为外部计数
//	TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);  
    TIM_SetCounter(TIM4, 0);          
    TIM_Cmd(TIM4,ENABLE );                                  			//使能定时器2  
}

void TIM5_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);		//定时器T5

    /* Enable the TIM5 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM5_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM5_SBUPRIORITY;
    NVIC_Init(&NVIC_InitStructure);

    /* 配置TIM5 */
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = CCR4_Val;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    TIM_PrescalerConfig(TIM5, 4, TIM_PSCReloadMode_Immediate);

    /* 允许TIM5更新事件中断 */
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

	DBGMCU_APB1PeriphConfig(DBGMCU_TIM5_STOP, ENABLE);

    /* TIM5 enable counter */
    TIM_Cmd(TIM5, ENABLE);
}

void TIM7_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);		//定时器T7

    /* Enable the TIM7 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM7_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM7_SBUPRIORITY;
    NVIC_Init(&NVIC_InitStructure);

    /* 配置TIM7 */
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = CCR4_Val;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
    TIM_PrescalerConfig(TIM7, 4, TIM_PSCReloadMode_Immediate);

    /* 允许TIM7更新事件中断 */
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	DBGMCU_APB1PeriphConfig(DBGMCU_TIM7_STOP, ENABLE);

    /* TIM7 enable counter */
    TIM_Cmd(TIM7, ENABLE);
}

void SlowTimeBase(void)						// 1ms时基信号产生
{
    static u16 	TimeBaseCnt = 0;
    TimeBaseCnt++;
    if(TimeBaseCnt >= SLOWTIMEBASECNTTOP)
    {
        TimeBaseCnt=0;
        SlowTimeBaseFlag.all=0xffff;
    }
}

void ChangeNodeState(CO_Data* d)
{
	if (d->nodeState == Operational)
	{
		if (NoSyncFrameCnt < 100)
		{
			NoSyncFrameCnt ++;
		}
		else if ((NoSyncFrameCnt >= 100) && (NoSyncFrameCnt <= 101))
		{
			NoSyncFrameCnt = 102;
			setState(&FD_ObjDict_Data, Pre_operational);
		}
	}
	else
	{
		NoSyncFrameCnt = 0;
	}
}

void ForceChangeNodeState(CO_Data* d)
{
	if (d->nodeState == Operational)
	{
		setState(&FD_ObjDict_Data, Pre_operational);
	}
}

void Time1msDeg(void)
{
    if (err_rst1_cnt)
    {
        err_rst1_cnt --;
    }

    if (MotoHotCnt)
    {
        MotoHotCnt --;
    }

    if (SaveParaCnt)
    {
        SaveParaCnt --;
    }
	
    if (ChargeRunEnable_Delay)
    {
        ChargeRunEnable_Delay --;
    }	
	
	if (CapTestFinishDelay > 1)
    {
        CapTestFinishDelay --;
    }	
	
	if (DisCharge_Delay)
    {
        DisCharge_Delay --;
    }	

	#ifndef TEST_FOR_STARS
		if (CANAddr)
		{
			if ((DataDiffFlag == 1) && ((ReadPara(SYS_ST_CODE) & 0x0004) == 0))
			{
				if (FlashValifyErr == 0)
					SaveMode = 3;												// SaveMode=3代表需存盘HMI自学习相关参数，不需存盘充电器参数
				DataDiffFlag = 0;
			}
			else if ((DataDiffFlag == 2) && ((ReadPara(SYS_ST_CODE) & 0x0004) == 0))
			{
				if (FlashValifyErr == 0)	
					SaveMode = 4;												// SaveMode=4代表需存盘加密激活参数，不需存盘充电器参数
				DataDiffFlag = 0;
			}
		}
	#else
	#endif

    if (err_rst1_delay)
    {
        err_rst1_delay --;
    }

    if (RefCheckFinish_Cnt)
    {
        RefCheckFinish_Cnt --;
    }
    else if (RefCheckFinish_Cnt == 0)
    {
        RefCheckFinish = 0;
    }
	
	if (GridVoltage_Over500VAC_KeepOnTime)
    {
        GridVoltage_Over500VAC_KeepOnTime --;
    }
	else
	{
		GridVoltage_Over500VAC_Flag = 0;
	}
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
    static u16 	TimeCnt = 0;
    static u16 	TimeCnt1 = 0;
    static u16 	TimeCnt2 = 0;
    static u16 	TimeCnt3 = 0;
    static u16 	TimeCnt4 = 0;
    static u16 	TimeCnt5 = 0;
    static u16 	TimeCnt6 = 0;
    static u16 	TimeCnt7 = 0;
    static u16 	TimeCnt8 = 0;
    static u16 	TimeCnt9 = 0;
    static u16 	TimeCnt10 = 0;
    static u16 	TimeCnt11 = 0;
	static u16 	TimeCnt12 = 0;

    static u8 RunFlagCC1 = 0; // 时间中断1运行标志，防重复调用应用程序
    static u8 RunFlagCC2 = 0; // 时间中断2运行标志，防重复调用应用程序
    static u8 RunFlagCC3 = 0; // 时间中断3运行标志，防重复调用应用程序
    static u8 RunFlagCC4 = 0; // 时间中断4运行标志，防重复调用应用程序
    static u8 HeartBeatTrigBegin = 0;

    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) 
    {
        TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, DISABLE);

        /* T = 100 us */
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

        SlowTimeBase();			//1ms时基信号产生
        Up_CL2351_Disp();

        capture1 = TIM_GetCounter(TIM3);
        TIM_SetCompare1(TIM3, (u16)(capture1 + CCR1_Val));

        TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
        if (RunFlagCC2 == 0) // 未处于中断状态则开中断
        {
            TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
        }
        if (RunFlagCC3 == 0) // 未处于中断状态则开中断
        {
            TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
        }
        if (RunFlagCC4 == 0) // 未处于中断状态则开中断
        {
            TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
        }
    }
    else if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
    {
        TIM_ITConfig(TIM3, TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, DISABLE);

        RunFlagCC2 = 1;

        /* T = 300 us */
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
        Task1();			 								//高速任务
		if (CANopen_EN)
		{
			MainDriveState_Control();														// 下发状态机控制命令
			Servo_State_Machine();															// CiA402状态机跳转
			DoubleRAM_ReadData();
		}
        Count2 = 0;
        capture2 = 0;
        Count2 = TIM_GetCounter(TIM3); 						// 获取当前计数值
        capture2 = TIM_GetCapture2(TIM3);					// 获取上次比较值
        if (Count2 < capture2)
        {
            Count2 += 0x10000;
        }
        capture2 += CCR2_Val; 								// 计算新的比较值
        if (capture2 < (Count2 + CCR0_Val)) 				// +CCR0_Val 为保证开中断之前当前计数值仍小于比较值
        {
            capture2 = Count2 + CCR2_Val; 					// 计算新的比较值
        }

        TIM_SetCompare2(TIM3, (u16)capture2);

        RunFlagCC2 = 0;

        TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
        if (RunFlagCC3 == 0)							 	// 未处于中断状态则开中断
        {
            TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
        }
        if (RunFlagCC4 == 0) 								// 未处于中断状态则开中断
        {
            TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
        }
    }
    else if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
    {
        TIM_ITConfig(TIM3, TIM_IT_CC3 | TIM_IT_CC4, DISABLE);

        RunFlagCC3 = 1;

        /* T = 500 us */
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);

        PLC_CLK_TRIG_Precoss();								// 执行PLC CLK边沿检测后台处理

        Task2();			 								// 中速任务
        
        Count3 = 0;
        capture3 = 0;
        Count3 = TIM_GetCounter(TIM3); 						// 获取当前计数值
        capture3 = TIM_GetCapture3(TIM3);					// 获取上次比较值
        if (Count3 < capture3)
        {
            Count3 += 0x10000;
        }
        capture3 += CCR3_Val; 								// 计算新的比较值
        if (capture3 < (Count3 + CCR0_Val)) 				// +CCR0_Val 为保证开中断之前当前计数值仍小于比较值
        {
            capture3 = Count3 + CCR3_Val; 					// 计算新的比较值
        }

        TIM_SetCompare3(TIM3, (u16)capture3);

        RunFlagCC3 = 0;

        TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
        if (RunFlagCC4 == 0) 								// 未处于中断状态则开中断
        {
            TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
        }
    }
    else
    {
        TIM_ITConfig(TIM3, TIM_IT_CC4, DISABLE);

        RunFlagCC4 = 1;

        /* T = 1000 us */
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);

        TimeCnt++;
        if (TimeCnt == 1000)
        {
            TimeCnt = 0;
        }

        if (Init_flag1 == 0)
        {
            if (((ReadPara(SYS_ST_CODE) & 0x0002) == 0x0002) && (FastVol > ReadPara(LOW_VOL)))//就绪后才允许计数
            {
                TimeCnt8++;
                Init_flag3 = 1;	   							// 驱动器有过一次就绪状态，才允许进行桨叶角度存盘操作
            }
        }
        if ((TimeCnt8 > 1500) && (Init_flag1 == 0))
        {
            Init_flag1 = 1;	   								// 就绪后再过1.5秒才允许进行上电角度初始化操作
            TimeCnt8 = 0;
        }
        if((Init_flag1 == 1) && (Init_flag2 == 0))
        {
            TimeCnt8++;
            if (TimeCnt8 > 1000)
            {
                Init_flag2 = 1;	   							// 旋变角度初始化操作1S后才允许进入变桨逻辑控制UserMainTask和RunCtrl
                TimeCnt8 = 0;
            }
        }

        Time1msDeg();										// 1ms时基--处理

        if ((TimeCnt % 10) == 0) 							// 10ms时基
        {
            InUart2DataProcess(); 							// 与充电器通讯
			
            if (HeartBeat == HeartBeatOld)					// 心跳位不翻转
            {
                if (HeartBeatStopCnt < 300)
                {
                    HeartBeatStopCnt++;
                }
            }
            else
            {
                HeartBeatStopCnt = 0;
            }

            if (Init_flag1)
            {
				if (CanLineOffCnt < 30)	
				{
					CanLineOffCnt ++;
				}
				
                if ((CanLineOffCnt >= 20))					
                {
					CANopenErrFlag = 1;						// 200ms未收到主控同步帧，置位CANOpen断线标志
                }
                else
                {
                    CANopenErrFlag = 0;
                }
            }

            HeartBeatOld = HeartBeat;						// 取上一次心跳位数值
        }

        //if(Init_flag2)
        {
            ReadDefectCodeL();
            ReadDefectCodeH();
            ReadDefectCodeM();
            DefectCodeProcess();
        }

        Count4 = 0;
        capture4 = 0;
        Count4 = TIM_GetCounter(TIM3); 						// 获取当前计数值
        capture4 = TIM_GetCapture4(TIM3);					// 获取上次比较值
        if (Count4 < capture4)
        {
            Count4 += 0x10000;
        }
        capture4 += CCR4_Val; 								// 计算新的比较值
        if (capture4 < (Count4 + CCR0_Val)) 				// +CCR0_Val 为保证开中断之前当前计数值仍小于比较值
        {
            capture4 = Count4 + CCR4_Val; 					// 计算新的比较值
        }

        TIM_SetCompare4(TIM3, (u16)capture4);

        RunFlagCC4 = 0;

        TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
    }
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : TIM5_1ms周期中断
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
        UserTaskSchedule();		   							// 调度用户任务
        PLC_TON_TOF_Process();      						// PLC TON TOF后台处理
        UserTaskPrecess();	 								// 执行用户任务
    }
}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : TIM7_1ms周期中断
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void)
{
	static bool ledout = 0;
	
	static u16 	TimeCnt = 0;
	static u16 	TimeCnt1 = 0;
	static u16 	TimeCnt2 = 0;
	static u16 	TimeCnt3 = 0;
	static u16 	TimeCnt5 = 0;
	
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
		
		//ledout = !ledout;
		//GPIO_WriteBit(GPIO_LED, GPIO_LED_PIN, (BitAction)ledout);
		
		FD_ObjDict_obj1016_L = FD_ObjDict_obj1016[0] & 0xffff;
		ReadPara(CO_CONSUMER_HEARTBEAT_TIME) = FD_ObjDict_obj1016_L;
				
		TimeCnt1++;
        if (TimeCnt1 == 5000)
        {
            Init_flag = 1;	   								// 5秒后初始化完成
        }
		
		TimeCnt2++;
		if (TimeCnt2 == 800)								// 800ms时基
		{
			TimeCnt2 = 0;
			canopen_para_precoss_flag = 1;
		}
		
		TimeCnt3++;
	  //if (TimeCnt3 == 3000)								// 3S时基
		if (TimeCnt3 == 10000)								// 10S时基	2025-06-12优化风机故障检测机制
		{
			TimeCnt3 = 0;
			TimeBase_3Secon_ForDO8 = 1;
			TimeBase_3Secon_ForFan0Err = 1;
			TimeBase_3Secon_ForFan1Err = 1;
		}
		
		TimeCnt5++;
		if (TimeCnt5 == 5000)								// 5S时基
		{
			TimeCnt5 = 0;
			TimeBase_5Secon_ForDO8 = 1;
			MEN_Flash_Flag = ~MEN_Flash_Flag;
		}
		
		TimeCnt++;
        if (TimeCnt == 10)
        {			
			ReadPara(FASTVOL) = FastVol;
			
			ReadPara(ARM_RESERVE_PARA_830) = TIM4->CNT/10;	// F=CNT/T(timer)(S) / 1000 Khz
			TIM4->CNT = 0;
			
			Task10ms();										// 温度检测等低速任务处理
			Fan0ErrCheck();									// 底座风机故障检测
            Fan1ErrCheck();									// 内腔风机故障检测
			ChangeNodeState(&FD_ObjDict_Data);
			
            TimeCnt = 0;
        }
		
		Display_Cnt++;
		if ((Display_Cnt % 500) == 0) 						// 500ms时基
		{
			Display_Precess();
		}

		if (Display_Cnt == 1000)
		{
			Display_Cnt = 0;
			LED_Flash_Flag = ~LED_Flash_Flag;
			ERR_Flash_Flag = ~ERR_Flash_Flag;
		}
				
		if (ReadPara(SEC_CODER_OFFSET) == 5555)
		{
			if (SSI_EnableFlag == 0)
			{
				SSI_EnableFlag = 1;
				SSI_24V_ON;
				SSI_Configuration();					//SPI2：SSI编码器通讯 
			}
			
			Get_SSI_POS();
		}
		else
		{
			SSI_EnableFlag = 0;
			SSI_24V_OFF;
		}
    }
}
