#include "InterUart2.H"

extern u8  ChargerAppState;

#define	GPIO_IO_PWR_RDE 	  	GPIOE				// 引脚定义
#define	GPIO_IO_PWR_RDE_PIN   	GPIO_Pin_15			// 引脚定义 
#define	PWR_RDE_RX				(BitAction)1	    // 485通信方向控制: RX
#define	PWR_RDE_TX				(BitAction)0       	// 485通信方向控制: TX
#define QUEUE2_LEN	  			(sizeof(ReadDspFrameQueue2)/sizeof(u16))
#define COMMED        			0xC006
#define USART3_DR_Base  		(u32)(&USART3->DR)

u16	ReadDspFrameQueue2[] =
{	
	(1),								//低速轮询充电器所有ID
	(CP_AC_VOL),						//电网电压
	(CP_SYS_ST_CODE),					//系统状态
	(CP_V_FEEDBACK),					//充电电压反馈
	(CP_SCR_CTL | 0x8000), 				//半空桥控制
	(CP_BTMVOL_FEEDBACK),				//BTM中点电压
	(CP_AC_VOL),						//电网电压
	(CP_VOL | 0x8000), 				 	//驱动器母线	
	(CP_CAPTEST_CMD | 0x8000), 			//容量测试命令
	(CP_I_FEEDBACK),					//充电电流反馈
	(CP_AC_VOL),						//电网电压
	(CP_SCR_CTL | 0x8000), 				//半空桥控制
	(PWR_PARA_LEN),						//中速轮询特殊参数	
};

void InUart2RevDataProcess(void);

u8	InUart2_RxBuf[INUART_RX_DATA_FRAMELEN+1],InUart2_TxBuf[INUART_TX_DATA_FRAMELEN+1];
u8  InUart2_RxBuf0[INUART_RX_DATA_FRAMELEN+1];
u16 Queue2Loc = 0;
u16 AllParaID2 = 1;
u8  InUart2RxFullFlag = 0;
u8  InUart2RxErrFlag = 0; 
u16 InUart2IdeCnt = 0; 			// 与充电器通讯空闲计数清零
u8  InUart2ErrFlag = 0; 		// 与充电器通讯失败标志
u16	PcFrame2Cmd = 0, PcFrame2Data = 0;
u16 DSP_ComSta = 0x0000;
u16 DSP_ComCMD = 0;
u16 DSP_ComData = 0;
u16 InTxDataFlag = 0;
u16 ChargCommTime = 10;
u16 ChargParaTab[64][2] = {0}; //写充电器FIFO，目前下传给充电器的读写参数目前已超过32，需加大，改为ChargParaTab[64][2]，否则缓存数据会被覆盖
u16 WriteParaCnt = 0;
u16 ReadParaCnt = 0;
u16 Bk205_Vol = 0;
u16 paraid = CP_DEFECT_CODE;

u16	ChargerLowVol = 100;  						//电网欠压值
u16	ChargerLowVolBack = 40;  					//电网欠压回差值
u16	ChargerOverVol = 540;  						//电网过压值
u16	ChargerOverVolBack = 40;  					//电网过压回差值
u16 GridVoltage_Over500VAC_KeepOnTime = 0;

extern u8 ChargeSaveFlag;
extern u16 UpDateFlag;
extern DefectConfigType DefectConfigParaID[41];
extern u16 ChargeRunEnable;
extern u16 CapTestState;
extern u8 charger_para_precoss_flag;
extern u8 Charge_Ctrl_Flag;
extern bool CapTestFlag;
extern bool RST_QY;
extern bool RST_GY;
extern bool Silicon_Controlled_Rectifier;
extern u8 err_rst1;
extern u32 Control_unit_Version;
bool Control_unit_Version_ReadFinish = 0;
bool GridVoltage_Over500VAC_Flag = 0;

/*******************************************************************************
* Function Name  : DMA_Configuration
* Description    : Configures the DMA.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_Configuration2(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;// TX
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART3_DMA1_7_4_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART3_DMA1_7_4_SUBPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_DeInit(DMA1_Stream1);
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_DR_Base;                  //外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)InUart2_RxBuf0;                //内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     //dma传输方向方向
    DMA_InitStructure.DMA_BufferSize = 8;               						//设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //设置DMA的内存递增模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存数据字长
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								//设置DMA的传输模式：连续不断的循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     //设置DMA的优先级别
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;						//DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;					//DMA_MemoryBurst_INC8;//DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;			//DMA_PeripheralBurst_INC8;//DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream1, &DMA_InitStructure);

    /* DMA2 通道5 USART4 发送 */
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)InUart2_TxBuf;                  //内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                      //dma传输方向方向, 处设作为传输目的地
    DMA_InitStructure.DMA_BufferSize = 7;                          				 //设置DMA在传输时缓冲区的长度
    DMA_Init(DMA1_Stream4, &DMA_InitStructure);

    /* Enable USART3 DMA Rx request */
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
    /* Enable USART3 DMA Tx request */
    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);

    DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
    //DMA_ClearITPendingBit(DMA1_IT_TC4);

    DMA_Cmd(DMA1_Stream4, ENABLE);

    DMA_Cmd(DMA1_Stream1, ENABLE);
}

/*******************************************************************************
* 函数名 : USART_Configuration
* 描  述 : 配置与DSP相连的USART
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void InUart3_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    //与STM32F10X不同，STM32F40X使用端口复用时需要使用GPIO_PinAFConfig函数进行指定引脚的挂载,
    //除ADC和DAC外的所有非GPIO功能，都要使用GPIO_PinAFConfig函数进行端口复用，端口复用没有时钟，直接使用即可

    /*AF功能的映*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    /* 配置方向控制输出脚 */
    GPIO_WriteBit(GPIO_IO_PWR_RDE, GPIO_IO_PWR_RDE_PIN, PWR_RDE_RX);  // 接收
    GPIO_InitStructure.GPIO_Pin = (GPIO_IO_PWR_RDE_PIN);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_IO_PWR_RDE, &GPIO_InitStructure);
    GPIO_WriteBit(GPIO_IO_PWR_RDE, GPIO_IO_PWR_RDE_PIN, PWR_RDE_RX);  // 接收

    /* Configure USART3 Rx (PB.11) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure USART3 Tx (PB.10) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 允许USART3中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART3_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART3_SUBPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART3 clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /* DMA1 clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    USART_InitStructure.USART_BaudRate = 19200;
	//USART_InitStructure.USART_BaudRate = 57600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    /* 配置USART3 */
    USART_Init(USART3, &USART_InitStructure);

    /* 允许USART3接收、发送中断 */
    USART_ITConfig(USART3, USART_IT_ERR, ENABLE);      			// 允许串口接收错误中断
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);      		// 允许串口空闲中断

    DMA_Configuration2();

    /* 允许 USART3 */
    USART_Cmd(USART3, ENABLE);
}

/*******************************************************************************
* Function Name  : DMA2_Channel4_5_IRQHandler
* Description    : This function handles DMA2 Channel 4 and DMA2 Channel 5
*                  interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Stream4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) != RESET)
    {
        DMA_ClearITPendingBit (DMA1_Stream4,DMA_IT_TCIF4);
        USART_ClearITPendingBit(USART3, USART_IT_TC);
        USART_ITConfig(USART3, USART_IT_TC, ENABLE); 			// 允许发送寄存器发送完成中断
    }
}

void USART3_IRQHandler(void)
{
    vu16 RxErrDat;
    u16 i = 0;

    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)   	// 空闲中断
    {
        USART_ClearITPendingBit(USART3, USART_IT_IDLE);			// 清除USART1发送完成中断标志
        RxErrDat = USART_ReceiveData(USART3);    //

        for (i = 0; i < INUART_RX_DATA_FRAMELEN; i++)
        {
            InUart2_RxBuf[i] = InUart2_RxBuf0[i];
        }

        InUart2RxFullFlag = 1;
        DMA_Cmd(DMA1_Stream1, DISABLE);
        DMA_ClearFlag(DMA1_Stream1,DMA_IT_TCIF1);
        DMA1_Stream1->NDTR  = 8;
        DMA_Cmd(DMA1_Stream1, ENABLE);
        InUart2RevDataProcess();
    }
    else if(USART_GetITStatus(USART3, USART_IT_FE)  != RESET)   // 帧错误
    {
        USART_GetFlagStatus(USART3, USART_FLAG_FE);       		// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        RxErrDat = USART_ReceiveData(USART3);
    }
    else if(USART_GetITStatus(USART3, USART_IT_ORE) != RESET) 	// 溢出错误
    {
        USART_GetFlagStatus(USART3, USART_FLAG_ORE);       		// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        RxErrDat = USART_ReceiveData(USART3);
    }
    else if (USART_GetITStatus(USART3, USART_IT_NE)  != RESET) 	// 噪声错误
    {
        USART_GetFlagStatus(USART3, USART_FLAG_NE);       		// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        RxErrDat = USART_ReceiveData(USART3);
    }
    else if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_TXE);			// 清除USART1中断标志
    }
    else if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)   	// 发送完成中断
    {
        USART_ClearITPendingBit(USART3, USART_IT_TC);			// 清除USART1发送完成中断标志
        USART_ITConfig(USART3, USART_IT_TC, DISABLE); 			// 禁止中断
        GPIO_WriteBit(GPIO_IO_PWR_RDE, GPIO_IO_PWR_RDE_PIN, PWR_RDE_RX);
    }
}

void InUart2TxFrameAsm(u16 Cmd,u16 Data) 						// 星辰上位机协议
{
    u16	i;
    InUart2_TxBuf[0] = 1;
    InUart2_TxBuf[1] = (u8)(Cmd/256);
    InUart2_TxBuf[2] = (u8)Cmd;
    InUart2_TxBuf[3] = Data/256;
    InUart2_TxBuf[4] = (u8)Data;

    i=CRC16(InUart2_TxBuf, 5);
    InUart2_TxBuf[5] = (u8)(i/256);
    InUart2_TxBuf[6] = (u8)i;
}

void Slowly_Para_Pro(void)													// 中速轮询参数
{
	switch (paraid)
	{
		case CP_DEFECT_CODE:												
		{
			paraid = CP_IGBT_TEMP;											// 充电器IGBT温度
			break;
		}		
		case CP_IGBT_TEMP:														
		{
			//paraid = CP_CAP;												// 超级电容容量
			
			if (ReadPara2(CP_ARM_BUILD_YEAR) == 0)
			{
				paraid = CP_ARM_BUILD_YEAR;									// 充电器ARM编译年
			}
			else if ((ReadPara2(CP_ARM_BUILD_YEAR) != 0) && (ReadPara2(CP_ARM_BUILD_DATE) == 0))    
			{
				paraid = CP_ARM_BUILD_DATE;									// 充电器ARM编译月日
			}		
			else
			{
				paraid = CP_CAP;											// 超级电容容量
				
				#ifndef TEST_FOR_STARS
				
				if ((ReadPara2(CP_ARM_BUILD_YEAR) != 0) && (ReadPara2(CP_ARM_BUILD_DATE) != 0) && (Control_unit_Version_ReadFinish == 0))
				{
// 					Control_unit_Version = 											// 充电器编译时间，如221025代表22年10月25日
// 					(ReadPara2(CP_ARM_BUILD_YEAR) % 100) * 10000 + (ReadPara2(CP_ARM_BUILD_DATE) / 100 * 100) + ReadPara2(CP_ARM_BUILD_DATE) % 100;	

					Control_unit_Version = 											// 充电器版本，如：10511(HMI显示1.5.11)，代表充电器主版本号为1，编译日期为5月11日，不显示年份
					((ReadPara2(CP_ARM_BUILD_DATE) / 100 * 100) + (ReadPara2(CP_ARM_BUILD_DATE) % 100)) + 10000;	
					
					Control_unit_Version_ReadFinish = 1;
				}
				#else
				#endif
			}
			
			break;
		}	
		case CP_ARM_BUILD_YEAR:
		case CP_ARM_BUILD_DATE:	
		case CP_CAP:															
		{
			paraid = (CP_V_SET | 0x8000); 							 		// 下传充电器电压设定
			break;
		}	
		case (CP_V_SET | 0x8000):											
		{
			paraid = (CP_I_SET | 0x8000);									// 下传充电器电流设定
			break;
		}	
		case (CP_I_SET | 0x8000):											
		{
			paraid = (CP_ERR_RST | 0x8000); 				 				// 下传手动复位 						
			break;
		}		
		case (CP_ERR_RST | 0x8000):											
		{
			paraid = CP_ADC_RESET_TIMES; 								// 充电器复位AD模块次数										
			break;
		}
		case (CP_ADC_RESET_TIMES):											
		{
			paraid = CP_DEFECT_CODE; 									// 充电器故障										
			break;
		}			
		default:
		{
			paraid = CP_DEFECT_CODE;
			break;
		}
	}
}
			
void	InUart2SelectFrameAsm(void)
{
    //if(InUartAckFlag == 1)
    {
        if (DSP_ComSta == 0x11)												// 发送PC通讯帧
        {
            InUart2TxFrameAsm(DSP_ComCMD, DSP_ComData);
            if ((DSP_ComCMD == 0xC003) && (DSP_ComData == 0xAA55)) 			// 开使能指令
            {
                if(Charge_Ctrl_Flag == 1)
                {
                    ReadPara(CHARG_CTRL_REG) = 1; 							// 开使能
                }
            }
            else if ((DSP_ComCMD == 0xC004) && (DSP_ComData == 0x55AA)) 	// 关使能指令
            {
                if(Charge_Ctrl_Flag == 1)
                {
                    ReadPara(CHARG_CTRL_REG) = 0; 							// 关使能
                }
            }
        }
        else 																// 发送本地查询帧
        {
            if (++Queue2Loc >= QUEUE2_LEN)
            {
                Queue2Loc = 0;
                AllParaID2++;												// 慢速轮询充电器所有参数
                if (AllParaID2 > PWR_PARA_LEN)
                {
                    AllParaID2 = 1;
                }
                ReadDspFrameQueue2[0] = AllParaID2;
            }
			
			if (Queue2Loc == (QUEUE2_LEN - 1))								// 中速轮询参数
			{

				Slowly_Para_Pro();
				InUart2TxFrameAsm(paraid,ReadPara2(paraid&0x3FFF)); 		// 0x3FFF提取命令中的ID

			}
			else
			{
				InUart2TxFrameAsm(ReadDspFrameQueue2[Queue2Loc],ReadPara2(ReadDspFrameQueue2[Queue2Loc]&0x3FFF)); // 0x3FFF提取命令中的ID
			} 
		}
    }
}

void InUart2StartSend()
{
    GPIO_WriteBit(GPIO_IO_PWR_RDE, GPIO_IO_PWR_RDE_PIN, PWR_RDE_TX);  //	发送
    DMA_Cmd(DMA1_Stream4, DISABLE);
    DMA1_Stream4->NDTR = 7;
    DMA_Cmd(DMA1_Stream4, ENABLE); 		// 发送前重新初始化DMA确保DMA传输目的起始地址为InUart_RxBuf[0]
}

void InUart2RevDataProcess(void)
{
    u16 Data;
    union EXUART_COMMAND ParaCMD;
    static u32 InUartIdeCnt = 0; 		// 与充电器通讯空闲计数

    u32 Temp_V = 0;

    if(InUart2RxFullFlag == 1)
    {
        InUart2RxFullFlag = 0;

        if(CRC16(InUart2_RxBuf, 5) == (InUart2_RxBuf[5] * 256 + InUart2_RxBuf[6]))
        {
            InUart2IdeCnt = 0; 			// 与充电器通讯空闲计数清零
            InUart2ErrFlag = 0; 		// 清与充电器通讯失败标志
            InUart2RxErrFlag = 0;

            Data        = InUart2_RxBuf[3] * 256 + InUart2_RxBuf[4];
            ParaCMD.all = InUart2_RxBuf[1] * 256  + InUart2_RxBuf[2];

            if ((DSP_ComSta == 0x11) && (ParaCMD.all == DSP_ComCMD))
            {
                DSP_ComData = Data; // 更新数据

                if (InTxDataFlag == 0)
                {
                    DSP_ComSta++;
                }
                else
                {
                    DSP_ComSta = 0; // 内部通讯不发往上位机
                    InTxDataFlag = 0; // 清除标志
                }
            }
            //else //if(ActiveDdtaChannel == 0)
            {
                if ((ParaCMD.bit.CONTENT) == 0) // 数据
                {
                    if (ParaCMD.bit.PARAID < PWR_PARA_LEN)
                    {
                        ReadPara2(ParaCMD.bit.PARAID) = Data; // 0x01FF提取命令中的ID ID最大512个参数
                    }
                }
            }
        }
        else
        {
            InUart2RxErrFlag = InUart2RxErrFlag;      // 测试用
        }
    }
}

void ComFaultDec (void)
{
    if (InUart2IdeCnt < 100)
    {
        InUart2IdeCnt++;
    }
    else
    {
        InUart2ErrFlag = 1;
    }
		
			/* 2026-09-22新增：充电器是否运行在APP
	   与充电器通讯正常，且已成功读到充电器控制单元版本 → 充电器在APP；
	   充电器跳boot或掉线时本函数会把 InUart2ErrFlag 置1，本标志随之清0。
	   注：加"已读到版本"是为了避免上电瞬间 InUart2ErrFlag 初值0造成误判为在APP。*/
	ChargerAppState = ((InUart2ErrFlag == 0) && (Control_unit_Version_ReadFinish != 0)) ? 1 : 0;
}

void WriteChargPara(u16 ParaCMD, u16 ParaData)
{
    // 写入FIFO
    ChargParaTab[WriteParaCnt][0] = ParaCMD;
    ChargParaTab[WriteParaCnt][1] = ParaData;
    WriteParaCnt++;
    WriteParaCnt %= 64;
    if (WriteParaCnt == ReadParaCnt)
    {
        ReadParaCnt++;
        ReadParaCnt %= 64;
    }
}

void Charge_Scr(void)			//调用周期=10ms
{
    //if (Silicon_Controlled_Rectifier)
	if ((Silicon_Controlled_Rectifier) || ((ReadPara(DEFECT_CODE) & 0x0002) == 0x0002))	// 2022-10-24新增：DSP母线过压时给充电器ARM下发关半控指令
	{
		ReadPara(CHARG_SCR_CTR) = 0;      		//关闭半控桥
	}
	else
	{
		ReadPara(CHARG_SCR_CTR) = 1;			//开启半控桥	
	}		

	if (ReadPara(CHARG_AC_VOL) > 5000)			//电网电压超过500V，就重新计时1S
	{
		GridVoltage_Over500VAC_Flag = 1;
		GridVoltage_Over500VAC_KeepOnTime = 1000;		
	}
}
	
void InUart2DataProcess(void)
{
    static u16 CtrlOnCnt = 0;
    static u16 CtrlOffCnt = 0;

    static u16 CtrlSaveCnt = 0;
    u16 temp[2] = {0,0};
    static u16 tempold[2] = {0,0};

    if (ChargeSaveFlag) //保存参数
    {
        if (CtrlSaveCnt < 3)//强制发送3次充电器存盘参数命令，确保存盘成功
        {
            WriteChargPara(0xC005, 0xA151); // 保存参数
            CtrlSaveCnt++;
        }
        else
        {
            CtrlSaveCnt = 0;
            ChargeSaveFlag = 0;
        }
    }

    if ((ReadPara(CHARG_CTRL_REG) & 1) || ChargeRunEnable)
    {
        if (CtrlOnCnt < 3)//发送3次充电器开使能指令，确保开使能成功
        {
            WriteChargPara(0xC003, 0xAA55); // 发送充电器开使能指令
            CtrlOnCnt++;
            Charge_Ctrl_Flag = 0;
        }
        else if (CtrlOnCnt < 100)//if ((TimeCnt % ChargCommTime) == 0) ==> 主控板10ms与充电器通讯一次，10ms * 100 = 1000ms后，
        {
            CtrlOnCnt++;
        }
        else if (((ReadPara2(CP_SYS_ST_CODE) & 0x0001) == 0) && ((ReadPara2(CP_SYS_ST_CODE) & 0x0020) == 0)) // 如果充电器仍为关使能状态，且无故障
        {
            CtrlOnCnt = 0; // 则CtrlOnCnt清零，重新发送开使能指令
        }

        CtrlOffCnt = 0;
    }
    else
    {
        if (CtrlOffCnt < 3)//发送3次充电器关使能指令，确保关使能能成功
        {
            WriteChargPara(0xC004, 0x55AA); // 发送充电器关使能指令
            Charge_Ctrl_Flag = 0;
            CtrlOffCnt++;
        }
        else if (CtrlOffCnt < 100)//if ((TimeCnt % ChargCommTime) == 0) ==> 主控板10ms与充电器通讯一次，10ms * 100 = 1000ms后，
        {
            CtrlOffCnt++;
        }
        else if ((ReadPara2(CP_SYS_ST_CODE) & 1) == 1) // 如果充电器仍为使能状态，
        {
            CtrlOffCnt = 0; // 则CtrlOffCnt清零，重新发送关使能指令，
        }

        CtrlOnCnt = 50;//特殊处理：ChargeRunEnable=1，50*10ms=500ms后，ReadPara2(CP_SYS_ST_CODE) & 1 == 0充电器仍为关使能状态，CtrlOnCnt = 0,开使能
    }

    if ((InTxDataFlag == 0) && (WriteParaCnt != ReadParaCnt)) 				// 写数据到充电器：FIFO有数据要写入且可以写入
    {
        DSP_ComCMD = ChargParaTab[ReadParaCnt][0]; 							// 0x8000 写数据;
        DSP_ComData = ChargParaTab[ReadParaCnt][1];
        InTxDataFlag = 1;
        DSP_ComSta = 0x11;
        ReadParaCnt++;
        ReadParaCnt %= 64;
    }

	// 写充电器参数（轮询帧写）
    ReadPara2(CP_VOL) 		 			= ReadPara(C_VOL);
	ReadPara2(CP_V_SET)	     			= ReadPara(CHARG_V_SET);
	ReadPara2(CP_I_SET)	     			= ReadPara(CHARG_I_SET);	
    ReadPara2(CP_SCR_CTL)   			= ReadPara(CHARG_SCR_CTR); 
	
	if (CapTestFlag && CapTestState)
	{
		ReadPara2(CP_CAPTEST_CMD) |= 0x000A;
	}
	else
	{
		ReadPara2(CP_CAPTEST_CMD) &= 0xFFF5;
	}
	
    // 读充电器参数（轮询帧读）
    ReadPara(CHARG_DEFECT_CODE) 		= ReadPara2(CP_DEFECT_CODE); 		//告警码
    ReadPara(CHARG_SYS_ST_CODE) 		= ReadPara2(CP_SYS_ST_CODE); 		//系统状态
    ReadPara(CHARG_IGBT_TEMP)   		= ReadPara2(CP_IGBT_TEMP);   		//IGBT温度
    ReadPara(CHARG_VOL)         		= ReadPara2(CP_VOL);         		//母线电压
	ReadPara(CHARG_AC_VOL) 				= ReadPara2(CP_AC_VOL);				//电网电压
    ReadPara(CHARG_V_FEEDBACK)  		= ReadPara2(CP_V_FEEDBACK);  		//电容电压
    ReadPara(CHARG_I_FEEDBACK)  		= ReadPara2(CP_I_FEEDBACK);  		//充电电流
	ReadPara(CHARG_DC24V_TEMP)  		= ReadPara2(CP_DC24V_TEMP);  		//充电器DC24V电源温度
	ReadPara(CHARG_INTERSPACE_TEMP) 	= ReadPara2(CP_INTERSPACE_TEMP);	//充电器内腔温度
	ReadPara(CHARG_CAP)  				= ReadPara2(CP_CAP);				//超容容量
	ReadPara(CHARG_BTMVOL)  			= ReadPara2(CP_BTMVOL_FEEDBACK);	//BMT电压
	ReadPara(CHARG_ADC_RESET_TIMES)  			= ReadPara2(CP_ADC_RESET_TIMES);	//充电器AD模块复位次数

// 	ReadPara(CHARG_ALARM_AUTO_RST_EN)	= ReadPara2(CP_ALARM_AUTO_RST_EN);	//故障手自动复位
// 	ReadPara(CHARG_ALARM_EN)  			= ReadPara2(CP_ALARM_EN);			//故障启用禁止
// 	ChargerLowVol 						= ReadPara2(CP_AC_LOWVOL);  		//电网欠压值
// 	ChargerLowVolBack 					= ReadPara2(CP_AC_LOWVOL_BACK);  	//电网欠压回差值
// 	ChargerOverVol 						= ReadPara2(CP_AC_OVERVOL);  		//电网过压值
// 	ChargerOverVolBack 					= ReadPara2(CP_AC_OVERVOL_BACK);  	//电网过压回差值

	Charge_Scr();
    ComFaultDec();
    InUart2SelectFrameAsm();
    InUart2StartSend();
}
