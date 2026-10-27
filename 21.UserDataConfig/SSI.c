#include "SSI.h"

/* SSI编码器时钟输出 */
#define	GPIO_SSI_CLK 	   GPIOB		// 引脚定义
#define	GPIO_SSI_CLK_PIN   GPIO_Pin_13	// 引脚定义 

/* SSI编码器数据输入 */
#define	GPIO_SSI_MISO 	   GPIOB		// 引脚定义 
#define	GPIO_SSI_MISO_PIN  GPIO_Pin_14	// 引脚定义 

#define SPI2_DR_BASE  	0x4000380C
#define SSI_SPI_RX_LEN  2
#define SSI_SPI_TX_LEN  2

u16 SSI_SPI_RxBuf[SSI_SPI_RX_LEN];
u16 SSI_SPI_TxBuf[] = {0x55AA, 0x55AA};
u32 SSI_Data = 0;
s32 SSI_PosReg = 0;           	// 圈数*65536 + 位置
s16 SSI_PositionNum = 0;	  	// 圈数
u16 SSI_Position    = 0;	  	// 位置
u8  Read_SSI_Finish=0;			// SSI反馈解码完成， Get_SSI_POS(void)中才允许重新初始化SSI：SSI_DMA_Configuration();
u16 SecPosMax = 32767; 			// 360度对应第二编码器的圈数
u32 SecCoderAngle_Record_Cnt = 0;
INTEGER32 SecCoderAngle_Old = 0;
INTEGER32 SecCoderAngle_Record1 = 0;
INTEGER32 SecCoderAngle_Record2 = 0;
INTEGER32 SecCoderAngle_Temp = 0;
extern long SecCoderAngle;

/*******************************************************************************
* 函数名 : UpPOSLimit
* 描  述 : 更新位置范围
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void UpPOSLimit(void)
{
    u32 PosMaxBuf = 0;

    PosMaxBuf = ReadPara(SEC_SCALE_B) * 65536;
    PosMaxBuf = PosMaxBuf / ReadPara(SEC_SCALE_A);
    PosMaxBuf += 32768; // 四舍五入
    SecPosMax = (u16)(PosMaxBuf / 65536);
}

void SSI_DMA_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    /* DMA1 通道4 SPI2 接收 */
    DMA_DeInit(DMA1_Stream3);
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = SPI2_DR_BASE;                    //外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)SSI_SPI_RxBuf;                 //内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     //dma传输方向方向，外设作为传输的来源
    DMA_InitStructure.DMA_BufferSize = SSI_SPI_RX_LEN;                          //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //设置DMA的内存递增模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         //内存数据字长
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 						        //设置DMA的传输模式：普通模式；//连续不断的循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //设置DMA的优先级别
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream3, &DMA_InitStructure);

    SPI_I2S_ReceiveData(SPI2); // 读SPI接收寄存器用于清除SPI接收中断等标志
    DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3); //DMA_ClearFlag (DMA1_FLAG_TC4);

    /* 允许DMA通道0传输完成中断 */
    DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);

    /* 允许SPI DMA接收 */
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

    /* 允许DMA通道0 */
    DMA_Cmd(DMA1_Stream3, ENABLE);
}

void DMA1_Stream3_IRQHandler(void)
{
    u8 i = 0;
    u32 SSI_DataBuf = 0;
    s32 SSI_DataBuf0 = 0;
    double SecCoderAngleBuf = 0;

    if(DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3) != RESET)
    {
        /* 禁止DMA通道0传输完成中断 */
        DMA_ITConfig(DMA1_Stream3, DMA_IT_TCIF3, DISABLE);
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3); //DMA_ClearFlag (DMA1_FLAG_TC4);
        SPI_Cmd(SPI2, DISABLE);
        if (((SSI_SPI_RxBuf[0] & SSI_SPI_RxBuf[1]) == 0xFFFF) || ((SSI_SPI_RxBuf[0] | SSI_SPI_RxBuf[1]) == 0x0000))
        {
            SSI_LinkFlag = 0; // SSI未连接
        }
        else
        {
            SSI_LinkFlag = 1; // SSI已连接
        }

        SSI_Data = SSI_SPI_RxBuf[0]; //  * 65536 + SSI_SPI_RxBuf[1];
        SSI_Data = (SSI_Data << 16) | SSI_SPI_RxBuf[1];
        SSI_Data &= 0x7FFFFF80; // SPI读回的数据第二位为有效数据
        SSI_Data >>= 3; // 12位圈数 & 位置

        if (ReadPara(GRAY_EN) == 1)  // 格雷码转二进制
        {
            SSI_DataBuf = SSI_Data & 0x08000000; // 取出最高位：
            for (i = 27; i > 4; i--)
            {
                SSI_DataBuf |= (((SSI_DataBuf >> 1) ^ SSI_Data) & (1<<(i-1)));
            }
        }
        else
        {
            SSI_DataBuf = SSI_Data;
        }

        SSI_DataBuf0 = SSI_DataBuf - ((s32)ReadPara(SEC_CODER_OFFSET) * 65536); // 12位圈数 & 位置  (s32)SSI_PosReg - 0x08000000;
        SSI_DataBuf0 = SSI_DataBuf0<<4; // 提取符号位
        SSI_DataBuf0 = SSI_DataBuf0>>4;
        SSI_PosReg = SSI_DataBuf0;
        if (ReadPara(SEC_CODER_DIR) == 0)
        {
            SSI_PosReg = -SSI_PosReg;
        }

        SSI_PositionNum = (s16)(SSI_PosReg >> 16);
        SSI_Position    = (u16)(SSI_PosReg & 0x0000FFFF);

        SecCoderAngleBuf = (double)SSI_PosReg;
        SecCoderAngleBuf *= 36000;
        SecCoderAngleBuf *= (u16)ReadPara(SEC_SCALE_A);
        SecCoderAngleBuf /= (u16)ReadPara(SEC_SCALE_B);

        if (SecCoderAngleBuf < 0)
        {
            SecCoderAngleBuf -= 32768; // 四舍五入
        }
        else
        {
            SecCoderAngleBuf += 32768; // 四舍五入
        }

        SecCoderAngle_Temp = (s32)(SecCoderAngleBuf / 65536);//SecCoderAngle = 36000则代表当前SSI的角度为360°
        ReadPara(SEC_CODER_ANGLE)  = (u16)(SecCoderAngle_Temp & 0x0000FFFF);	//ID318
        ReadPara(SEC_CODER_ANGLEH) = (u16)(SecCoderAngle_Temp >> 16);		//ID366

        //示例：SSI齿轮比=30:182，SSI_PositionNum = 1758，SSI_Position = 9440
        //==> SecCoderAngle = (1758 * 65536 + 9440) * 30 * 36000 / （182 * 65536） = 10432943 = 0x9F31AF
        //ReadPara(SEC_CODER_ANGLE)  = 0x31AF，ReadPara(SEC_CODER_ANGLEH) = 0x009F

        WritePara(SEC_POS_NUM, SSI_PositionNum); 	   	// 写圈数
        WritePara(SEC_POS, SSI_Position); 	   			// 写相对位置
        //SSI_DMA_Configuration();        				// 重新初始化DMA确保DMA传输目的起始地址
        Read_SSI_Finish = 1;
    }
}

/*******************************************************************************
* 函数名 : SSI_RCC_Configuration
* 描  述 : 配置SSI系统时钟
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void SSI_RCC_Configuration(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);

    /* 允许SPI2时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* DMA1 clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
}

/*******************************************************************************
* 函数名 : SSI_NVIC_Configuration
* 描  述 : 配置SSI中断向量表基地址
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void SSI_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 允许DMA1 通道0中断 */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = SPI2_DMA1_0_3_PREPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SPI2_DMA1_0_3_SBUPRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* 函数名 : SSI_GPIO_Configuration
* 描  述 : 配置SSI_GPIO端口
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void SSI_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //与STM32F10X不同，STM32F40X使用端口复用时需要使用GPIO_PinAFConfig函数进行指定引脚的挂载,
    //除ADC和DAC外的所有非GPIO功能，都要使用GPIO_PinAFConfig函数进行端口复用，端口复用没有时钟，直接使用即可

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);

    /* 配置SPI2引脚： */
    GPIO_InitStructure.GPIO_Pin = GPIO_SSI_CLK_PIN |GPIO_SSI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIO_SSI_CLK, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名 : SSI_Configuration
* 描  述 : 配置SSI
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void SSI_Configuration(void)
{
    SPI_InitTypeDef  SPI_InitStructure;

    /* 配置SSI_RCC */
    SSI_RCC_Configuration();

    /* 配置SSI_NVTC */
    SSI_NVIC_Configuration();

    /* 配置SSI_GPIOs */
    SSI_GPIO_Configuration();

    /* 配置SPI2为主设备 */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_RxOnly;		//仅接收模式
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 						// 时钟悬空为高
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 						// 数据捕获于第二个时钟沿
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    /* 允许SPI2 */
    SPI_Cmd(SPI2, ENABLE);
    SSI_DMA_Configuration();        // 初始化DMA确保DMA传输目的起始地址
}

/*******************************************************************************
* 函数名 : SSI_SPI_Write
* 描  述 : 向SPI2总线写入并读取数据
* 输  入 : u16 dat: 16位待写入数据
* 输  出 : 无
* 返  回 : 16位读取到的数据
*******************************************************************************/
u16 SSI_SPI_Write(u16 dat)
{
    /* 发送SPI2数据 */
    SPI_I2S_SendData(SPI2, dat);

    return (1);
}

/*******************************************************************************
* 函数名 : SPI2_IRQHandler
* 描  述 : SPI2 中断处理函数
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void SPI2_IRQHandler(void)
{

}

/*******************************************************************************
* 函数名 : Get_SSI_POS
* 描  述 : 读取SSI位置
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void Get_SSI_POS(void)
{
    SPI_Cmd(SPI2, ENABLE);

// 	GPIO_SSI_CLK->MODER &= ~0x0C000000;
// 	GPIO_SSI_CLK->MODER |= 0x08000000;//恢复CLK为复用功能模式(B13)		//MPC增加

    if(Read_SSI_Finish)	//SSI反馈解码完成才允许重新初始化SSI，避免SecCoderAngle_Temp有突变的现象
    {   //void DMA1_Channel4_IRQHandler(void)中正在进行SecCoderAngle_Temp的计算，被此处中断打断，初始化SSI，导致SecCoderAngle_Temp计算被打乱
        SSI_DMA_Configuration();        // 重新初始化DMA确保DMA传输目的起始地址
        Read_SSI_Finish = 0;
    }
}