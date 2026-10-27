#include "InterUart.H"
#include "stm32f4xx_conf.h"
#include "ExternPrototype.h"
#include "TimeBase.h"
#include "CL2351.h"
#include "state_machine.h"

#define USART2_DR_Base 0x40004404
#define INUART_RX_OVERTIME 10
#define INUART_TX_OVERTIME 10
#define QUEUE_LEN (sizeof(ReadDspFrameQueue)/sizeof(u16))
#define FILTER_NUM  1024

extern  u16 SavePitchAngleCmd;
extern u8 DspAppState;

u16	ReadDspFrameQueue[] =
{
	DSP_HEARTBEAT_COUNT,
	WORK_MOD_FEB,
	SYS_ST_CODE,
	V_SET,
	C_VOL,
	IGBT_TEMP,
	V_FEEDBACK,
	I_FEEDBACK,
	T_SET,
	WORK_MOD,
	DSP_HEARTBEAT_COUNT,
	POSITIONSK,
	ACTUALTORQUE,
	OVER_I_TOTAL,
	SYS_ST_CODE,
	(MOTOR_ANGLE_OFFSETH | 0x8000),
	(MOTOR_ANGLE_OFFSETL | 0x8000),
	(MOTOR_POS_RST | 0x8000),
	MOTO_TEMP,
	OVER_I_TOTAL,
	MOTOOVERLOADDISP,
	DEFECT_CODET,
	(MOTO_TYPE | 0x8000),
	MOTOR_ANGLE,
	SOFT_VER,
	(SAFE_CLOSE_SPEED | 0x8000),// 2026-01-26 取消DISCHARGE_TIM轮询，改为SAFE_CLOSE_SPEED下传
	DISCHARGE_ACCTIM,
	(ALARM_AUTO_RST_EN | 0x8000),
	(ARM_DSP_COMMPARA0 | 0x8000),
	(P_PV | 0x8000)
	//若后续还有其他新增参数轮询或下传的需求，DISCHARGE_ACCTIM也是可以直接取消占用的；
	//若有更多参数需求，可考虑把IGBT_TEMP、MOTO_TEMP、MOTOR_ANGLE(慢速用角度)、SOFT_VER等N个参数统一到一个慢速顺序轮询的函数中处理，更新时间=30ms*N个参数
};

u8	InUart_RxBuf[INUART_RX_DATA_FRAMELEN],InUart_TxBuf[INUART_TX_DATA_FRAMELEN];
u16	QueueLoc = 0;
u8	InUartRxLoc = 0,InUartTxLoc = 0;
u8	InUartRxFullFlag = 0;
u16 DdtaChannel = 0;				//0 发送本地查询帧 1 发送PC通讯帧
u16 ActiveDdtaChannel = 0;			//0 发送本地查询帧 1 发送PC通讯帧
u16 DSPErrCode = 0;
s32	DspSet = 0;
u16	InUartStartFlag = 0;
u16	InUartResendCnt = 0;
u16	InUartTxOverTimer = 0;
u16	InUartTxOverFlag = 0;
u16	InUartRxOverTimer = 0;
u16	InUartAckFlag = 1;				// 0 DSP未返回 1 DSP已返回
u32 InUartIdeCnt = 0; // 与DSP通讯空闲计数
u16	PcFrameWaitFlag = 0;
u8 DSPReturnPCFlag = 1; 			// DSP返回PC数据标志：0未返回数据，1已返回数据 初始值应为1
//u8 InUartErrFlag = 0; 				// 与DSP通讯失败标志：1失败
u8 InUartErrFlag = 2; 				// 与DSP通讯失败标志：1失败
u8 DspBootVerReadEn = 1;			// 2026-10-09新增：1=需要读一次DSP Boot版本(ID386)，读取后清0
u8 InUartErrFlagOld = 2;			// 2026-10-09新增：上一周期与DSP通讯失败标志（识别通讯恢复）
u16 CrcPassFlag666Old = 0;			// 2026-10-09新增：上一周期打包校验标志（识别握手成功沿）
u16	PcFrameCmd = 0, PcFrameData = 0;
u16 Filter_V_Back = 0;        		// 平均值滤波后的反馈速度
u16 Filter_I_Back = 0;        		// 平均值滤波后的反馈电流
u16 Filter_T_Set  = 0;        		// 平均值滤波后的力矩设定
s32 PosReg = 0;           			// 圈数*65536 + 位置
s16 PositionNum = 0;	  			// 圈数
u16 Position    = 0;	  			// 位置 
s16 FastVol = 0;
u16 TxCrc = 0;
u16 RxCrc = 0;
u16 CrcErrCnt = 0;
u16 CrcPassFlag = 0;
u16 VsetTemp = 0;
u16 VfebTemp = 0;
u8 DSP_Status = 0;					// Bit7=抱闸实际状态反馈；Bit6=底座风机状态反馈；Bit5桨叶角度初始化完成标志；Bit4DSP心跳位(2ms翻转一次)；Bit3DSP泄放状态标志
u16 DspCommErrOverTime_Cnt = 0;
u16 DspCommErrOverTime_Cnt1 = 0;
long long Delay = 0;
bool DspHeartBeatFlagOld = 0;

extern u8 displayBuff[];
extern u8 hexCode[];
extern u8 selectBuf[];
extern u8 State_Machine;
extern u8 InUart2ErrFlag;
extern u8 Arm_Dsp_CommErr_Flag;
extern u8 Arm_Dsp_CommErr_Step;
extern u16 Controlword_Out;
extern s32 MotorAngle;
union STMSTATUS	StmStatus;
extern bool DO_DrvSafChain;

/*******************************************************************************
* Function Name  : DMA_Configuration
* Description    : Configures the DMA.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Stream5);
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_DR_Base;                  //外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)InUart_RxBuf;                  //内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     //dma传输方向方向
    DMA_InitStructure.DMA_BufferSize = INUART_RX_DATA_FRAMELEN;                 //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //设置DMA的内存递增模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存数据字长
    DMA_InitStructure.DMA_Mode =DMA_Mode_Circular;        						//设置DMA的传输模式：连续不断的循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     //设置DMA的优先级别
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold =  DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst =  DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);

    DMA_Cmd(DMA1_Stream5, ENABLE);
    DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
}

/*******************************************************************************
* 函数名 : USART_Configuration
* 描  述 : 配置与DSP相连的USART
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void InUart_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //与STM32F10X不同，STM32F40X使用端口复用时需要使用GPIO_PinAFConfig函数进行指定引脚的挂载,
    //除ADC和DAC外的所有非GPIO功能，都要使用GPIO_PinAFConfig函数进行端口复用，端口复用没有时钟，直接使用即可

    /*AF功能的映射*/
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource6, GPIO_AF_USART2);

    /* Configure USART2 Tx (PD.05) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Configure USART2 Rx (PD.06) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* 允许USART2中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART2_TX_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART2_TX_SBUPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 允许DMA1中断 */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART2_DMA1_4_5_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART2_DMA1_4_5_SBUPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART2 clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* DMA1 clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    USART_InitStructure.USART_BaudRate = INTER_USART_BAUDRATE; //
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    /* 配置USART2 */
    USART_Init(USART2, &USART_InitStructure);

    /* 允许USART2接收、发送中断 */
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);   	//DISABLE
    USART_ITConfig(USART2, USART_IT_ERR, ENABLE);      	//允许串口接收错误中断

    DMA_Configuration();

    /* 允许 USART2 */
    USART_Cmd(USART2, ENABLE);
}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 Channel 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Stream5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5) != RESET)
    {
        DMA_ClearITPendingBit (DMA1_Stream5,DMA_IT_TCIF5);

        InUartRxFullFlag = 1;
        InUartRxOverTimer = 0;      // 超时计数清零
        InUartRxLoc = 0;

        DMA_Configuration();        // 重新初始化DMA确保DMA传输目的起始地址为InUart_RxBuf[0]
    }
}

void USART2_IRQHandler(void)
{
    CPU_CRITICAL_ENTER();         // 关总中断

    if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
    {
        if(InUartTxLoc > (INUART_TX_DATA_FRAMELEN - 1))
        {
            InUartTxLoc = 0;
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
        }
        else
        {
            USART_SendData(USART2, InUart_TxBuf[InUartTxLoc]);
            InUartTxLoc++;
        }
    }

    /* 串口错误中断处理 */
    if(USART_GetITStatus(USART2, USART_IT_ORE) != RESET) 	// 溢出错误
    {
        USART_GetFlagStatus(USART2, USART_FLAG_ORE);       	// 清除错误标志：先读取错误标志, 再读取接收寄存器的内容
        USART_ReceiveData(USART2);    //

        InUartRxOverTimer = 0;                           	// 超时计数清零
        InUartRxLoc = 0;                                 	// 接收计数清零  重发
    }

    CPU_CRITICAL_EXIT();          // 开总中断
}

void	UpdateInUartTxFrame(u16 Cmd,u16 Data)				// 更新发往DSP的数据帧
{
    DdtaChannel = 1;										// 0 发送本地查询帧 1 发送PC通讯帧
    PcFrameCmd = Cmd;
    PcFrameData = Data;
    PcFrameWaitFlag = 1;
}

void	InUartTxFrameAsm(u16 Cmd,u16 Data)
{
    u16	i;

    InUart_TxBuf[SEND_CMDH_NO] = (u8)(Cmd/256);
    InUart_TxBuf[SEND_CMDL_NO] = (u8)Cmd;
    InUart_TxBuf[SEND_DATAH_NO] = Data/256;
    InUart_TxBuf[SEND_DATAL_NO] = (u8)Data;
    InUart_TxBuf[SEND_SETH_NO] = (u8)((u32)DspSet/256);
    InUart_TxBuf[SEND_SETL_NO] = (u8)DspSet;
    InUart_TxBuf[SEND_EXSETH_NO] = (u8)((u32)DspSet/65536/256);
    InUart_TxBuf[SEND_EXSETL_NO] = (u8)((u32)DspSet/65536);

    StmStatus.bit.IO_IN1 = 0;																//Bit01：ARM呼叫DSP升级（DSP侧命名IO_CANopenUpdate）；CANopen升级功能后，ARM APP中绝对不允许置位！！！置位将使DSP进入Bootloader
    StmStatus.bit.IO_IN2 = (ReadPara(DEFECT_CODE)  || ReadPara(DEFECT_CODEH) ||				//Bit02 驱动器故障
						    ReadPara(DEFECT_CODEM) || ReadPara(DEFECT_CODET)) ? 1 : 0;		
    StmStatus.bit.IO_IN3 = (ReadPara(CHARG_DEFECT_CODE)) ? 1 : 0; 							//Bit03 充电器故障
    StmStatus.bit.IO_IN4 = (State_Machine == FT) ? 1 : 0;									//Bit04 驱动器Fualt状态=1
    StmStatus.bit.IO_IN5 = InUart2ErrFlag;													//Bit05 充电器连接
    StmStatus.bit.IO_IN6 = (Controlword_Out >> 6) & 0x1;									//Bit06 软停止（DI0=0、或DI3=0、或DI5=1，此位都=1）
	
#ifndef TEST_FOR_STARS			
	if (CANAddr != 0)
	{
		StmStatus.bit.IO_IN7 = (DO_DrvSafChain == TRUE) ? 1 : 0;							//Bit07 2023-06-05增加：变桨系统触发紧急停机故障状态，下传给DSP
		/*
		DO_DrvSafChain=1代表变桨系统触发紧急停机故障；=0代表无紧急停机故障。此标志下传给到DSP做处理：
		在紧急停机过程中，首先限制输出电流为1.9倍额定输出电流，电流值104.5A，
		同时计算输出过载I^2t累加值，当达到过载累加值的0.9倍时（持续输出104.5A电流9秒左右），
		继续下调输出电流限,制值至驱动器额定输出电流55A，确保紧急顺桨过程中不触发过流和过载类故障。
		*/
	}
	else
	{
		StmStatus.bit.IO_IN7 = 0;
	}
#else
	StmStatus.bit.IO_IN7 = 0;															
#endif	

    StmStatus.bit.MOTOGR = 0;																//Bit11 预留
    StmStatus.bit.SSIBK = 0;																//Bit13 预留
	
	if (((DSP_Status >> 5) & 0x01) == 0)
	{		
		StmStatus.bit.STM_SN_CTRLFLAG = 0;
	}
	
    InUart_TxBuf[SEND_STMSTATUSH_NO] = StmStatus.all/256;
    InUart_TxBuf[SEND_STMSTATUSL_NO] = (u8)StmStatus.all;
    InUart_TxBuf[SEND_DECODEH_NO] = 0;
    InUart_TxBuf[SEND_DECODEL_NO] = 0;
	
    i=CRC16(InUart_TxBuf, INUART_TX_DATA_FRAMELEN-2);
    InUart_TxBuf[INUART_TX_DATA_FRAMELEN-2]	= (u8)(i/256);
    InUart_TxBuf[INUART_TX_DATA_FRAMELEN-1]	= (u8)i;
    ActiveDdtaChannel = DdtaChannel;	//0 发送本地查询帧 1 发送PC通讯帧
}

u16 IsWaitStatus(void)
{
    return(PcFrameWaitFlag);
}

void InUartSelectFrameAsm(void)
{
    if(InUartAckFlag == 1)
    {
        if(DdtaChannel == 0)//0 发送本地查询帧 1 发送PC通讯帧
        {
            if(++QueueLoc >= QUEUE_LEN)
            {
                QueueLoc = 0;
            }

            InUartTxFrameAsm(ReadDspFrameQueue[QueueLoc],ReadPara(ReadDspFrameQueue[QueueLoc]&0x3FFF)); // 0x3FFF提取命令中的ID
        }
        else
        {
            InUartTxFrameAsm(PcFrameCmd,PcFrameData);
        }
    }
}

void InUartStartSend()

{
    InUartRxLoc = 0;
    DMA_Configuration();        // 发送前重新初始化DMA确保DMA传输目的起始地址为InUart_RxBuf[0]

    InUartStartFlag = 1;
    InUartTxOverFlag = 0;
    InUartTxOverTimer = 0;
    InUartAckFlag = 0;
    InUartTxLoc = 1;
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    USART_SendData(USART2, InUart_TxBuf[0]);
}

void InUartOverHandel(void)
{
    if((InUartStartFlag == 1) && (InUartAckFlag == 0))//发送超时处理
    {
        if(SlowTimeBaseFlag.bit.INUART == 1)
        {
            if(InUartTxOverTimer < INUART_TX_OVERTIME)
            {
                InUartTxOverTimer++;
            }
            else
            {
                InUartStartFlag = 0;
                InUartTxOverFlag = 1;
            }
            SlowTimeBaseFlag.bit.INUART = 0;
        }
    }
    else
    {
        InUartTxOverTimer = 0;
    }

    if(InUartRxLoc != 0)			//接收超时处理
    {
        if(InUartRxOverTimer < INUART_RX_OVERTIME)
        {
            InUartRxOverTimer++;
        }
        else  // 超时处理
        {
            InUartRxOverTimer = 0;
            //InUartRxLoc = 0;
        }
    }
    else
    {
        InUartRxOverTimer = 0;
    }
}

void InUartRevDataProcess(void)
{
    u16 Data;
    //static u32 InUartIdeCnt = 0; // 与DSP通讯空闲计数

    if(InUartRxFullFlag == 1)
    {
        InUartRxFullFlag = 0;
        InUartStartFlag = 0;

        if(CRC16(InUart_RxBuf,INUART_RX_DATA_FRAMELEN - 2)
        == InUart_RxBuf[INUART_RX_DATA_FRAMELEN - 2] * 256 + InUart_RxBuf[INUART_RX_DATA_FRAMELEN - 1])
        {
            InUartIdeCnt = 0; 	// 与DSP通讯空闲计数清零
			if (InUartErrFlag == 1) 
			{
				InUartErrFlag = 2; 	// 从1到2，说明ARM需拉低WKO，与DSP同步时序后，打包数据给DSP
			}

            Data = InUart_RxBuf[REV_DATAH_NO] * 256 + InUart_RxBuf[REV_DATAL_NO];          					// 数据
            MoutValue[1] =  InUart_RxBuf[REV_VFBH_NO] * 256 + InUart_RxBuf[REV_VFBL_NO];   					// 速度反馈(bit15=1)或速度设定(bit15=0) 精度0.01°/S
			
			if (MoutValue[1] & 0x8000)										// 速度反馈
			{
				if (MoutValue[1] & 0x4000)
				{
					VfebTemp = MoutValue[1] | 0x8000;
				}
				else
				{
					VfebTemp = MoutValue[1] & 0x7FFF;
				}
			}
			else															// 速度设定
			{
				if (MoutValue[1] & 0x4000)
				{
					VsetTemp = MoutValue[1] | 0x8000;
				}
				else
				{
					VsetTemp = MoutValue[1] & 0x7FFF;
				}
			}
			
            MoutValue[3] 	= InUart_RxBuf[REV_IFBH_NO] * 256 + InUart_RxBuf[REV_IFBL_NO];   				// 电流反馈
            FastVol 		= InUart_RxBuf[REV_TSETH_NO] * 256 + InUart_RxBuf[REV_TSETL_NO]; 				// 母线电压（原力矩设定）
            DSPErrCode 		= InUart_RxBuf[REV_DSPSTATUSH_NO] * 256 + InUart_RxBuf[REV_DSPSTATUSL_NO];  	// 驱动器故障码L（DSP故障码）
			PosReg = (s32)((u32)(InUart_RxBuf[REV_POSITIONHH_NO] << 24) | (u32)(InUart_RxBuf[REV_POSITIONLH_NO] << 16) | (u32)(InUart_RxBuf[REV_POSITIONLL_NO] << 8)) / 256;	
			DSP_Status = InUart_RxBuf[REV_EX_NO];// Bit7=抱闸实际状态反馈；Bit6=底座风机状态反馈；Bit5桨叶角度初始化完成标志；Bit4DSP心跳位(2ms翻转一次)
			
			if (((DSP_Status >> 4) & 0x01) != DspHeartBeatFlagOld)
			{
				ReadPara(DSP_HEARTBEAT_COUNT1) = ReadPara(DSP_HEARTBEAT_COUNT1) + 1;
			}
			DspHeartBeatFlagOld = (DSP_Status >> 4) & 0x01;                   	
			
            if(ActiveDdtaChannel == 0)
            {
                WritePara((ReadDspFrameQueue[QueueLoc]&0x3FFF), Data); // 0x3FFF提取命令中的ID
            }
            else
            {
                if(PcFrameData != Data)
                {
                    WritePara((PcFrameCmd & 0x7fff),Data);
                }
                PcFrameWaitFlag = 0;
                DdtaChannel = 0;		// 恢复发送本地查询帧状态
                DSPReturnPCFlag = 1;    // 置DSP返回PC数据标志
            }
            InUartAckFlag = 1;
            InUartResendCnt = 0;
        }
        else
        {
            InUartTxOverFlag = 1;      // 接收CRC错误置发送超时标志重发
            InUartTxOverTimer = 0;
        }
    }

    if (InUartIdeCnt < 1000)			// 300us*1000=300ms：DSP判断与ARM通讯中断大于300ms则自主收桨（215故障）；系统层判断DSP的心跳计数器保持不变大于100ms则报129双口RAM故障
    {
        InUartIdeCnt++; 				// 与DSP通讯中断计数
    }
    else
    {
        InUartErrFlag = 1; 				// 置与DSP通讯失败标志
    }
}

void InUartDataProcess(void)			// 数据帧处理
{
	if ((InUartErrFlag == 2) || (Arm_Dsp_CommErr_Flag == 0))
	{
		Arm_Dsp_CommErr_Handle();
	}
	
    if(InUartAckFlag)
    {
        InUartSelectFrameAsm();
		
		//if (InUartErrFlag != 2)
		if (Arm_Dsp_CommErr_Step == 0)
		{
			InUartStartSend();
		}
    }
    else
    {
        InUartRevDataProcess();
    }
    if(InUartTxOverFlag == 1)
    {
		//if (InUartErrFlag != 2)
		if (Arm_Dsp_CommErr_Step == 0)
		{
			InUartStartSend();
		}
		
        if(InUartResendCnt < 100)
        {
            InUartResendCnt++;
        }

        InUartTxOverFlag = 0;
        InUartTxOverTimer = 0;
    }
		
			/* 2026-09-22新增：DSP是否运行在APP
	   与DSP通讯正常(无通讯失败标志)且ARM/DSP参数打包校验通过 → DSP在APP；
	   DSP跳Bootloader期间不响应本串口协议，超时后 InUartErrFlag 置1，本标志随之清0。*/
	DspAppState = ((InUartErrFlag != 1) && (CrcPassFlag == 666)) ? 1 : 0;
	
	/* 2026-10-09新增：DSP Boot版本（参数ID386）一次性读取，不进常规轮询队列（避免每轮占用数据帧）。
	   触发时机：与DSP打包握手成功（上电/重连）、或与DSP通讯从失败恢复（DSP升级完成回到APP）；
	   读回值由PC通道自动写入ARM的同ID参数386，客户读ID386即为DSP Boot版本。*/
	if((InUartErrFlagOld == 1) && (InUartErrFlag != 1))	DspBootVerReadEn = 1;
	if((CrcPassFlag666Old != 666) && (CrcPassFlag == 666))	DspBootVerReadEn = 1;
	InUartErrFlagOld = InUartErrFlag;
	CrcPassFlag666Old = CrcPassFlag;
	
	if(DspBootVerReadEn && (InUartErrFlag != 1) && (CrcPassFlag == 666) && (InUartAckFlag == 1) && (Arm_Dsp_CommErr_Step == 0))
	{
		DspBootVerReadEn = 0;
		UpdateInUartTxFrame(RESERVE_PARA_386, 0);//一次性读DSP的386（DSP Boot版本）
	}
}

void DspCommErrOverTimeClearHandle(void)
{
	if (Arm_Dsp_CommErr_Flag ==0)
	{
		Arm_Dsp_CommErr_Flag = 1;
	}		
	InUartIdeCnt = 0; 	
	InUartErrFlag = 0;
	Arm_Dsp_CommErr_Step = 0;
	InUartAckFlag = 1;
	Delay = 0;
    WritePara(CTRL_CMD, 0);
    InUart_Configuration();
    TIM_Cmd(TIM3, ENABLE);
	__enable_irq() ; 
}

/*******************************************************************************
* 函数名 : SendParaPackToDsp
* 描  述 : 将参数发送到DSP初始化并返回DSP参数
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void SendParaPackToDsp(void)//将参数发送到DSP初始化并返回DSP参数
{
    u16 i,j;
	u16 k = 0;
    static u32 InitCnt = 0;
    static u32 InitErrCnt = 0;
	static u32 cnt = 0;
	
	//ForceChangeNodeState(&FD_ObjDict_Data);

	__disable_irq() ; //关闭总中断
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);//禁用中断方式
	USART_DMACmd(USART2, USART_DMAReq_Rx, DISABLE);
	TIM_Cmd(TIM3, DISABLE); 
	
	//LEDON;
	
	ReadPara(MOTO_ANGLE_SET) = 65535;			// ARM打包数据下传给DSP（如DSP以太网升级完成后、ARM升级APP、通讯故障恢复等），部分重要参数需确保重置
    ReadPara(MOTOR_POS_RST)  = 0;
    ReadPara(SYS_ST_CODE) = 0;
	ReadPara(DEFECT_CODE) = 0;
	ReadPara(DEFECT_CODET) = 0;
    ReadPara(FLASHSAVEFLAG) = 0;
    ReadPara(ARM_DSP_COMMPARA0) = 0;
    ReadPara(ERRSAVE_ENABLE) = 0;
	ReadPara(MOTOOVERLOADDISP) = 0;

	CRC16ForSingleReset();
	
	//t4 ~= 5.4ms（429*2Byte*10bit/1565127~=5.4ms）
	for(i = 0; i < DSP_PARA_LEN; i++)
	{
		USART_SendData(USART2, SysParaTable[i].DATA/256);		
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)	{}			
		USART_SendData(USART2, SysParaTable[i].DATA);			
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)	{}
			
		TxCrc = CRC16ForSingle(SysParaTable[i].DATA);
	}
	
	{
		USART_SendData(USART2, TxCrc/256);		
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)	{}			
		USART_SendData(USART2, TxCrc);			
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)	{}	
	}
			
	CRC16ForSingleReset();
	
	Delay = 0;
	
	//t5 ~= 9.2ms（ARM打包数据发送完毕到接收DSP回复数据时间）
	
	//t6 ~= 5.4ms（429*2Byte*10bit/1.565127M ~= 5.4ms）
	//for(i = 0; i < (DSP_PARA_LEN + 1); i++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
		{
			{
				DspCommErrOverTime_Cnt1 ++;
				
				if (Delay ++ > 75000)	//t7 ~= 33ms（75000对应约33ms）
				{
					Delay = 0;
					DspCommErrOverTime_Cnt ++;			//超时计数器
					DspCommErrOverTimeClearHandle();
					InUartErrFlag = 1;
					//LEDOFF;
					return;
				}
			}				
		}
		j =  USART_ReceiveData(USART2)*256;

		while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
		{		
			{
				DspCommErrOverTime_Cnt1 ++;
				
				if (Delay ++ > 75000)	//t7 ~= 33ms（75000对应约33ms）
				{
					Delay = 0;
					DspCommErrOverTime_Cnt ++;			//超时计数器
					DspCommErrOverTimeClearHandle();
					InUartErrFlag = 1;
					//LEDOFF;
					return;
				}
			}	
		}
		j +=  USART_ReceiveData(USART2);

// 		if (i < DSP_PARA_LEN)				//系统数据
// 		{	
// 			SysParaTable[i].DATA = j;
// 			RxCrc = CRC16ForSingle(j);
// 		}
// 		else								//校验数据
		{
		  //if (j == RxCrc)
			if (j == TxCrc)
			{ 
				u16 i = 0;
				
				/*
				for(i=0; i<SYS_PARA_LEN_STARS; i++)
				{
					CalculPara(i);
				}
				*/
				
				CalculPara1();
				
				WritePara(CRC_ERRCNT, CrcErrCnt);
				CrcPassFlag = 666;
				WritePara(CRC_RIGCNT, CrcPassFlag);
				DspCommErrOverTimeClearHandle();
			}
			else
			{
				if (CrcErrCnt < 65535)
				{
					CrcErrCnt ++;
				}			
				WritePara(CRC_ERRCNT, CrcErrCnt);
				CrcPassFlag = 0;
				WritePara(CRC_RIGCNT, CrcPassFlag);
				DspCommErrOverTimeClearHandle();
				InUartErrFlag = 1;
			}
		}
	}
}
