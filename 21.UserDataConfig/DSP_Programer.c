#include "stm32f4xx_conf.h"
#include "io.h"
#include "CL2351.h"
#define CANRX_FIFO_NUM  64

extern void SPI_Configuration(void);
extern u8 CANBus_Init(u8 tmpNodeID, u16 bitrate);
extern u8 displayBuff[6];
extern u8 hexCode[];
extern u8 selectBuf[6];

/*******************************************************************************
* 函数名 : USART_RCC_Configuration
* 描  述 : 配置USART的系统时钟
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void USART_RCC_Configuration(void)
{
    /* 允许GPIOA的时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD, ENABLE);

    /* 允许USART1的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    /* 允许USART2的时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

/*******************************************************************************
* 函数名 : USART_NVIC_Configuration
* 描  述 : 配置USART中断向量表基地址
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void USART_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* 允许USART2中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;   //USART2_Priority
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* 函数名 : USART_GPIO_Configuration
* 描  述 : 配置USART_GPIO端口
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void USART_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource6, GPIO_AF_USART2);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Rx (PD.06) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Tx (PD.05) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名 : USART_Scanf
* 描  述 : 从USART获取一个数据
* 输  入 : 无
* 输  出 : 无
* 返  回 : USART接收到的数据
*******************************************************************************/
u8 USART_Scanf(void)
{
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
    {}
    return(USART_ReceiveData(USART1));
}

/*******************************************************************************
* 函数名 : USART_Configuration
* 描  述 : 配置USART
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
u32 USART_BaudRate = 115200;
//u32 USART_BaudRate = 9600;
void USART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;

    /* 配置USART_RCC */
    USART_RCC_Configuration();
    /* 配置USART_NVTC */
    USART_NVIC_Configuration();
    /* 配置USART_GPIOs */
    USART_GPIO_Configuration();

    USART_InitStructure.USART_BaudRate = USART_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    /* 配置USART1 */
    USART_Init(USART1, &USART_InitStructure);
    /* 配置USART2 */
    USART_Init(USART2, &USART_InitStructure);
    /* 允许 USART1 */
    USART_Cmd(USART1, ENABLE);
    /* 允许 USART2 */
    USART_Cmd(USART2, ENABLE);
}

/*******************************************************************************
* 函数名 : DSP_Programer
* 描  述 : 建立DSP串口编程透明传输通道
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void DSP_Programer(void)
{
    u8 i;
    u8 LED_Flag = 1;
    u32 LED_Cnt = 0;
    u32 DSPCnt = 0;
    u8 CommMode = 0; // 通讯模式选择：0为串口通讯，1为CAN通讯
    CanRxMsg CANm;
    u32 CANCnt = 0;    // 串口接收间隔计数
    CanTxMsg CANTxMessage;    // CAN发送缓存
    u8 CANi = 0;
    u8 CANRxFIFO[CANRX_FIFO_NUM];
    u8 *pRead = CANRxFIFO;
    u8 *pWrite = CANRxFIFO;
    u8 Rx2FIFO[CANRX_FIFO_NUM];
    u8 *pRead2 = Rx2FIFO;
    u8 *pWrite2 = Rx2FIFO;

    /* 配置USART */
    USART_Configuration();

    /* 配置CAN */
    CANBus_Init(0, 500);	//节点0，默认波特率：500Kbit/s
    CANTxMessage.RTR   = 0;
    CANTxMessage.IDE   = 0;
    CANTxMessage.DLC   = 8;
    CANTxMessage.StdId = 0;

    displayBuff[4] = hexCode[29];    // P
    displayBuff[3] = hexCode[23];    // r
    displayBuff[2] = hexCode[24];    // d
    displayBuff[1] = hexCode[20];    // -
    displayBuff[0] = hexCode[24];    // d
    displayBuff[5] = 0x98;    		 // 红灯亮 黄灯亮 绿灯亮

    while (DSPCnt < 200000)  // 上电延时
    {
        DSPCnt++;
    }
    DSPCnt = 0;

    while (1)
    {
        {   //显示：Prg-d
            static u8 k,j;
            static u8 step;
            static u16 data;

            switch (step)
            {
            case 0:
                CLEAR_SRCLK;
                data & 0x8000 ? SET_SER : CLEAR_SER;
                data <<= 1;
                step++;
                break;
            case 1:
                SET_SRCLK;    // 在SCK的上升沿将数据储存到存储器里
                step = 0;
                k++;
                break;
            }

            if (k == 16)
            {
                k = 0;
                data = ((u16)displayBuff[j] << 8) + selectBuf[j];//段码 +　位码
                if (++j == 6)		// DISPLAY_LEN = 6
                {
                    j = 0;
                }
                SET_RCLK;       // 在RCK的上升沿将数据储存到锁存器里
            }
            else
            {
                CLEAR_RCLK;
            }
        }

        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) // 查询串口1接收标志 PC
        {
            /* 接收USART1的数据 */
            //RxBuffer1 = USART_ReceiveData(USART1);
            *pWrite = USART_ReceiveData(USART1);     // 写入FIFO
            if (pWrite == (&CANRxFIFO[CANRX_FIFO_NUM-1]))
            {
                pWrite = CANRxFIFO;
            }
            else
            {
                pWrite++;
            }
            if (pWrite == pRead)
            {
                if (pRead == (&CANRxFIFO[CANRX_FIFO_NUM-1]))
                {
                    pRead = CANRxFIFO;
                }
                else
                {
                    pRead++;
                }
            }

            CommMode = 0; // 置通讯模式为串口

            LED_Flag = 1;
        }
        else if (CAN_MessagePending(CAN1, CAN_FIFO0)) // 查询CAN接收数据标志
        {
            CANm.DLC = 0;
            CAN_Receive(CAN1, CAN_FIFO0, &CANm);
            if (CANm.DLC)
            {
                for (i = 0; i < (CANm.DLC); i++)
                {
                    /* 发送给USART2 */
                    //while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); // 循环检测直到发送为空
                    //USART_SendData(USART2, CANm.Data[i]);

                    *pWrite = CANm.Data[i];    // 写入FIFO
                    if (pWrite == (&CANRxFIFO[CANRX_FIFO_NUM-1]))
                    {
                        pWrite = CANRxFIFO;
                    }
                    else
                    {
                        pWrite++;
                    }
                    if (pWrite == pRead)
                    {
                        if (pRead == (&CANRxFIFO[CANRX_FIFO_NUM-1]))
                        {
                            pRead = CANRxFIFO;
                        }
                        else
                        {
                            pRead++;
                        }
                    }
                }

                //CANTxMessage.DLC = CANm.DLC;
                //CAN_Transmit(&CANTxMessage);

                CommMode = 1; // 置通讯模式为CAN
            }
            LED_Flag = 1;
        }

        //if (CommMode == 1)
        //{
        if (pRead != pWrite)    // 从FIFO中读出数据发送给DSP
        {
            if (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != RESET) // 检测到发送为空
            {
                USART_SendData(USART2, *pRead);
                if (pRead == (&CANRxFIFO[CANRX_FIFO_NUM-1]))
                {
                    pRead = CANRxFIFO;
                }
                else
                {
                    pRead++;
                }
            }
        }
        //}

        if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET) // 查询串口2接收标志
        {
            /* 接收USART2的数据 */
            //RxBuffer1 = USART_ReceiveData(USART2);
            *pWrite2 = USART_ReceiveData(USART2); // 写入FIFO2
            if (pWrite2 == (&Rx2FIFO[CANRX_FIFO_NUM-1]))
            {
                pWrite2 = Rx2FIFO;
            }
            else
            {
                pWrite2++;
            }
            if (pWrite2 == pRead2)
            {
                if (pRead2 == (&Rx2FIFO[CANRX_FIFO_NUM-1]))
                {
                    pRead2 = Rx2FIFO;
                }
                else
                {
                    pRead2++;
                }
            }

            LED_Flag = 1;
        }

        if (pRead2 != pWrite2)
        {
            CANCnt = 0;
            if (CommMode == 0)    // 用串口发送
            {
                if (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != RESET) // 检测到发送为空
                {
                    USART_SendData(USART1, *pRead2);   // 从FIFO2中读出数据用串口1发送
                    if (pRead2 == (&Rx2FIFO[CANRX_FIFO_NUM-1]))
                    {
                        pRead2 = Rx2FIFO;
                    }
                    else
                    {
                        pRead2++;
                    }
                }
                CANi = 0;
            }
            else                 // 用CAN发送
            {
                CANTxMessage.Data[CANi] = *pRead2;     // 从FIFO2中读出数据用CAN发送
                CANi++;
                if (CANi == 8)
                {
                    CANi = 0;
                    CANTxMessage.DLC = 8;
                    CAN_Transmit(CAN1, &CANTxMessage);
                }
                if (pRead2 == (&Rx2FIFO[CANRX_FIFO_NUM-1]))
                {
                    pRead2 = Rx2FIFO;
                }
                else
                {
                    pRead2++;
                }
            }
        }

        if ((CommMode == 1) && (CANi != 0))  // 帧间隔计数，超出后发送最后一帧数据
        {

            if (CANCnt < 100)
            {
                CANCnt++;
            }
            else if (CANCnt == 100)
            {
                CANCnt++;
                CANTxMessage.DLC = CANi;
                CAN_Transmit(CAN1, &CANTxMessage);
                CANi = 0;
            }
        }
    }
}