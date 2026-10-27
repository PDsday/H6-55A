#include "ExternPrototype.h"
#include "UserMainTask.h"
#include "CL2351.h"
#include "AppFunc.h"

#ifndef TEST_FOR_STARS
	#include "HMI_Display.h"
	#include "GlobalVar_HMI.h"
	#include "GlobalVar_POUS.h"
#else
#endif

#define DISPLAY_LEN (sizeof(displayBuff) / sizeof(displayBuff[0]))

u16 DisplayMenu = 0;
u16 SysErrState = 0;
u16 ErrCode = 0;

//				   0	 1	   2	 3	   L	 4个led		故障码L3210
u8 selectBuf[6] = {0xef, 0xf7, 0xfb, 0xfd, 0xfe, 0xdf};
u8 displayBuff[6] = {0, 0, 0, 0, 0, 0};

extern u8 Init_flag3;
extern u8 LED_Flash_Flag;
extern u8 ERR_Flash_Flag;
extern u8 MEN_Flash_Flag;
extern u8 Init_flag;
extern u8 InUartErrFlag; // 与DSP通讯失败标志：1失败
extern u64 DecodeAll;
extern u64 ServerFault;
extern u16 ServerFault1;
extern u16 ServerFault2;
extern u16 ServerFault3;
extern u16 ServerFault4;
extern u16 ServerFault5;
extern u32 CiA402_Statusword;
extern u16 FlashValifyErr;
extern u16 CANAddr_Real; 

//selectBuf为位选数据，低电平选中
//selectBuf[0]为最右边的数码管，即g4=0，其他=1，就选择了g4数码管；其他以此类推
//selectBuf[5]为4个led指示灯，即g5=0，其他=1，就选择了g5指示灯
//displayBuff为段选数据，高电平选中

// MICC Define：
// #define    SYS_INIT           	0        	Bit 0                                                           DoubleRAM.StateWord & 0x01
// #define    PRE_CHG_MENU       	1        	Bit12                                                          	DoubleRAM.StateWord & 0x1000
// #define    FAULT_MENU         	2        	Bit 1                                                           DoubleRAM.StateWord & 0x02
// #define    STOP_MENU          	3        	Bit 3                                                          	DoubleRAM.StateWord & 0x08
// #define    NOR_RUN_MENU      	4       	Bit 2 & Bit6                                              		DoubleRAM.StateWord & 0x04    &   DoubleRAM.StateWord & 0x40
// #define    SR_RUN_MENU          	5       	Bit 2 & （bit 7 bit8 bit 9 bit10 bit11 ）     					DoubleRAM.StateWord & 0x04    &   DoubleRAM.StateWord & 0x0F80
// #define    BAT_TEST_MENU       	7        	Bit 4                                                           DoubleRAM.StateWord & 0x10
// #define    DSP_UP_DATA          	8                                                                         	DspRunState < 4

/*
   位：		  bit7	  bit6	  bit5	  bit4	  bit3	  bit2	  bit1	  bit0
   数码管：    dp      	g		f	   e	   d	   c	   b	   a
   LED：      3-G					  1-Y	  2-R	  4-G
	   a
	  --
    f|g |b
	 |--|
	e|  |c
	  -- .dp
	  d	
*/
	
u8 hexCode[] = //段码表
{
    0x3F, // 0		0
    0x06, // 1		1
    0x5B, // 2		2
    0x4F, // 3		3
    0x66, // 4		4
    0x6D, // 5		5
    0x7D, // 6		6
    0x07, // 7		7
    0x7F, // 8		8
    0x6F, // 9		9
    0x77, // A		10
    0x7C, // B		11
    0x39, // C		12
    0x5E, // D		13
    0x79, // E		14
    0x71, // F		15
    0x00, // 熄灭	16
    0x38, // L		17
    0x76, // H		18
    0x39, // C		19
    0x40, // -		20
    0x31, // T		21
    0x79, // E		22
    0x50, // r		23
    0x5E, // d		24
    0x54, // n		25
    0x04, // i		26
    0x78, // t		27
    0x80, // .		28
    0x73, // P		29
    0xF6, // H.		30
    0x9C, // u.		31
	0xFF, // 8.		32
};

void CL2351_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 配置SPI3引脚：SCK (PB.3)、MISO (PB.4)和MOSI (PB.5) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 配置PG.15 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*******************************************************************************
* 函数名 :
* 描  述 : 数码管状态机
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void Display_Menu(void)
{
    if ((DoubleRAM.StateWord >> 0) & 1)					//bit0		初始化（驱动器状态机为NRSO）
    {
        DisplayMenu = 1;
    }
    else if ((DoubleRAM.StateWord >> 12) & 1)			//bit12		预充电 缓上电过程
    {
        DisplayMenu = 2;
    }
    else if ((DoubleRAM.StateWord >> 3) & 1)			//bit3		停机（驱动器状态机不为Fault且无输出）			
    {
        DisplayMenu = 3;
    }
	else if ((DoubleRAM.StateWord >> 2) & 1)			//bit2		运行（正常运行或SR运行）
	{
		if ((DoubleRAM.StateWord >> 6) & 1)				//bit6		正常运行
		{
			DisplayMenu = 4;
		}
		else if ((DoubleRAM.StateWord >> 7) & 1)		//bit7		SR运行（SR1=1、或SR2=1、或SR3=1）
		{
			DisplayMenu = 5;
		}
	}
    else if ((DoubleRAM.StateWord >> 1) & 1)			//bit1		故障（驱动器状态机为Fault）
    {
        DisplayMenu = 6;
    }
    else if ((DoubleRAM.StateWord >> 4) & 1)			//bit4		后备电源容量测试
    {
        DisplayMenu = 7;
    }
    else
    {
        DisplayMenu = 0;
    }
}

/*******************************************************************************
* 函数名 : Display_Precess
* 描  述 : 3位数码管显示处理函数
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void Display_Precess(void)
{
    /*
    A、第一个指示灯为总线通讯是否正常，当总线通讯正常的情况下，显示橙色常亮状态，总线异常(持续800ms未收到主控RPDO的心跳位翻转)的情况下为间隔1s周期性闪烁状态；
    B、第二个指示灯为PLC_Err：当ARM自带FLASH硬件损坏或者双口RAM通讯故障的情况下显示红灯常亮，否则就是熄灭状态；
    C、第三个指示灯为PLC_Run：当变桨系统状态机处于急停状态情况下为间隔1s周期性闪烁状态，否则就是绿色常亮状态；
    D、第四个指示灯为InverterStatus：当驱动器自身触发故障时，为间隔1s周期性闪烁状态，否则就是绿色常亮状态；
    注意：在驱动器第一次上电软启的时候，满足闭合超级电容负荷开关的条件后，4个LED显示灯才会从全灭状态开始按照上述各自逻辑条件正常显示。
    */

 	if (CANAddr == 0)													// 0地址上电，星辰协议
	{
		if (ReadPara(PARAVALIFYERR) != 0) 								
		{
			displayBuff[4] = hexCode[22];    							// 数码管显示Err
			displayBuff[3] = hexCode[23];    							
			displayBuff[2] = hexCode[23]; 

			return;
		}
	
		if (ReadPara(PW_ON_DISP) == 4)									// 数码管全灭、LED全灭
		{
			displayBuff[2] = hexCode[16];
			displayBuff[3] = hexCode[16];
			displayBuff[4] = hexCode[16];
			displayBuff[5] = 0x00;  
		}
		else if (ReadPara(PW_ON_DISP) == 8)								// 数码管全亮、LED全亮
		{
			displayBuff[2] = hexCode[32];
			displayBuff[3] = hexCode[32];
			displayBuff[4] = hexCode[32];	
			displayBuff[5] = 0xFF;
		}
		else									
		{
			displayBuff[4] = hexCode[1];     							// i
			displayBuff[3] = hexCode[24];    							// d
			displayBuff[2] = hexCode[CANAddr_Real];  					// 显示实时拨码地址
			displayBuff[5] = 0xFF;										// LED全亮
		}
	}
	else
	{
		Display_Menu();

		if (Init_flag3)
		{
			#ifndef TEST_FOR_STARS								
				if (G_CommStatus_TCU)									
				{
					if (ERR_Flash_Flag == 0)	displayBuff[5] &= 0xEF;	// A-橙灭 displayBuff[5] &= 0xEF		驱动器与主控CANOpen通讯异常
					else						displayBuff[5] |= 0x10;	// A-橙亮 displayBuff[5] |= 0x10		驱动器与主控CANOpen通讯异常
				}
				else
				{
					displayBuff[5] |= 0x10;								// A-橙亮 displayBuff[5] |= 0x10		驱动器与主控CANOpen通讯正常
				}		
			
				if (G_PitchRunState == EMERGENCY)
				{
					if (ERR_Flash_Flag == 0)	displayBuff[5] &= 0x7F;	// C-绿灭 displayBuff[5] &= 0x7F		变桨系统状态机急停状态
					else						displayBuff[5] |= 0x80;	// C-绿亮 displayBuff[5] |= 0x80		变桨系统状态机急停状态
				}
				else
				{
					displayBuff[5] |= 0x80;								// C-绿亮 displayBuff[5] |= 0x80		变桨系统状态机正常状态
				}
			#else
			#endif
				
			if (InUartErrFlag || FlashValifyErr)displayBuff[5] |= 0x08;	// B-红亮 displayBuff[5] |= 0x08		ARM自带FLASH硬件损坏或ARM与DSP通讯异常
			else			   					displayBuff[5] &= 0xF7;	// B-红灭 displayBuff[5] &= 0xF7		ARM自带FLASH硬件正常且ARM与DSP通讯正常

			if (ServerFault1 || ServerFault2 || ServerFault3 || ServerFault4 || ServerFault5)
			{
				if (ERR_Flash_Flag == 0)	displayBuff[5] &= 0xFB;  	// D-绿灭 displayBuff[5] &= 0xFB		驱动器自身触发故障
				else						displayBuff[5] |= 0x04;  	// D-绿亮 displayBuff[5] |= 0x04		驱动器自身触发故障
			}
			else
			{
				displayBuff[5] |= 0x04;								 	// D-绿亮 displayBuff[5] |= 0x04		驱动器无故障
			}
		}
		else
		{
			displayBuff[5] = 0x00;    		 							// 4个LED全灭
		}

		/*
		if (InUartErrFlag)												// ARM与DSP通讯异常
		{
			displayBuff[4] = hexCode[21];    							// 数码管显示T
			displayBuff[3] = hexCode[22];    							// 数码管显示E
			displayBuff[2] = hexCode[23];    							// 数码管显示r
			return;
		}
		*/

		if (Init_flag == 0) 											// Init_flag = 0 系统初始化中，界面显示CAN节点号
		{
			displayBuff[5] = 0x00;    		 							// 4个LED全灭
			displayBuff[4] = hexCode[1];     							// i
			displayBuff[3] = hexCode[24];    							// d
			displayBuff[2] = hexCode[CANAddr];  						// X

			return;
		}

		if (ReadPara(PARAVALIFYERR) != 0) 								
		{
			displayBuff[4] = hexCode[22];    							// 数码管显示E
			displayBuff[3] = hexCode[23];    							// 数码管显示r
			displayBuff[2] = hexCode[23];    							// 数码管显示r

			return;
		}
		
		#ifndef TEST_FOR_STARS
			if (MEN_Flash_Flag == 0)									// 显示故障码5S
			{
				displayBuff[4] = hexCode[(CAN_T.ErrorWord)/ 100 % 10];  // 2
				displayBuff[3] = hexCode[(CAN_T.ErrorWord)/ 10 % 10];   // 0
				displayBuff[2] = hexCode[(CAN_T.ErrorWord)/ 1 % 10];   	// 4	代表故障码204，对应驱动器过载	
			}
			else														// 显示状态机5S
			{
				displayBuff[4] = hexCode[30];    						// H.
				displayBuff[3] = hexCode[0];    						// 0
				displayBuff[2] = hexCode[DisplayMenu];    				// X
			}
		#else
		#endif
	}
}

/*******************************************************************************
* 函数名 : Up_CL2351_Disp
* 描  述 : 数码管显示
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void Up_CL2351_Disp(void)
{

    static u8 i, j;
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
			i++;
			break;
    }

    if (i == 16)
    {
        i = 0;
        data = ((u16)displayBuff[j] << 8) + selectBuf[j];//段码 +　位码
        if (++j == DISPLAY_LEN)		// DISPLAY_LEN = 6
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




