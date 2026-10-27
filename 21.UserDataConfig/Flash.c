#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "ParaProcess.h"
#include "ExternPrototype.h"

volatile FLASH_Status FLASHStatus;
u16* MotoroffsetAddr;
u16* MotoroffStartAddr;
u16* MotoroffEndAddr;
u16* MotoroffEraAddr;
u16 MotoroffNum = 0;
u16 Power = 0;

u16 FlashValifyErr = 0;
u16 *UserParaAdrr[2] = {0};
u16 *DeftParaAdrr[2] = {0};
u16 UserParaValifyData[2] = {0};
u16 DeftParaValifyData[2] = {0};

bool AngleCrcErr = 0;
bool AngleCrcErr_TidyFlag = 0;
bool AngleCrcErr_VloopRunFlag = 0;

extern UNS16 UserPara4;
extern s32 MotorPowerOffset;
extern u8 MotorPowerOffsetCRC[6];
extern u16 MotorPowerOffsetH;
extern u16 MotorPowerOffsetL;
extern u16 MotorPowerOffsetC;
extern u16 ErrRecordDataArray[GZM_InfoNum * GZM_Num];
extern union DEFECT_FLAGH DefectFlagInH;

#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

void FlashInit(void)
{
    FLASH_SetLatency(FLASH_Latency_2);
}

u32 GetSector(u32 Address)
{
    u32 sector = 0;

    if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_Sector_0;
    }
    else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_Sector_1;
    }
    else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_Sector_2;
    }
    else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_Sector_3;
    }
    else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_Sector_4;
    }
    else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_Sector_5;
    }
    else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_Sector_6;
    }
    else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_Sector_7;
    }
    else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_Sector_8;
    }
    else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_Sector_9;
    }
    else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_Sector_10;
    }
    else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
    {
        sector = FLASH_Sector_11;
    }

    return sector;
}

/*******************************************************************************
* 函数名 :
* 描  述 : 更新ARM自带FLASH参数存盘后的校验字节
* 输  入 :
* 输  出 :
* 返  回 :
*******************************************************************************/
void ReNewValifyData(void)
{
    u16* Addr;
    u16 i;

    Addr = (u16*)USER_PARA_ADDR;
    UserParaAdrr[0] = (u16*)Addr;
    UserParaAdrr[1] = ((u16*)((u32)Addr + SAVE_PARA_FLAG_OFFSET));
    UserParaValifyData[0] = *((u16*)((u32)Addr));
    UserParaValifyData[1] = *((u16*)((u32)Addr + SAVE_PARA_FLAG_OFFSET));
    ReadPara(USERPARAVALIFYDATA0) = UserParaValifyData[0];
    ReadPara(USERPARAVALIFYDATA1) = UserParaValifyData[1];

    Addr = (u16*)DEFAULT_PARA_ADDR;
    DeftParaAdrr[0] = (u16*)Addr;
    DeftParaAdrr[1] = ((u16*)((u32)Addr + SAVE_PARA_FLAG_OFFSET));
    DeftParaValifyData[0] = *((u16*)((u32)Addr));							//调出厂参数，Mode=1，Addr=0x080E 0000，TTTTTT=0x080F FFFE，TTTTTT1=0xAA55，
    DeftParaValifyData[1] = *((u16*)((u32)Addr + SAVE_PARA_FLAG_OFFSET));	//程序判断尾部校验合法，但JLINK仿真界面看到的0x080F FFFE地址数据为0xAAAA
    ReadPara(DEFTPARAVALIFYDATA0) = DeftParaValifyData[0];
    ReadPara(DEFTPARAVALIFYDATA1) = DeftParaValifyData[1];

    if (ReadPara(DEFTPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG)
    {
        FlashValifyErr = 4;
    }
    else if (ReadPara(DEFTPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)
    {
        FlashValifyErr = 3;
    }
    else if (ReadPara(USERPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG)
    {
        FlashValifyErr = 2;
    }
    else if (ReadPara(USERPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)
    {
        FlashValifyErr = 1;
    }
    else
    {
        FlashValifyErr = 0;
    }

    ReadPara(PARAVALIFYERR) = FlashValifyErr;
}

/*******************************************************************************
* 函数名 : SaveParaToFlash
* 描  述 : 保存参数到ARM自带的128K FLASH
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void SaveParaToFlash(u32 StartAddr)
{
    vu32 NbrOfPage = 0x00;
    u32 EraseCounter = 0x00;
    u32 Address = StartAddr;
    u16 i;
    FLASHStatus = FLASH_COMPLETE;

    /* 解锁FLASH编写擦除控制器 */
    FLASH_Unlock();
    NbrOfPage = PARA_PAGE_NUM;

    /* 清除所有FLASH待处理标志 */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR |FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);	//f405 m4

    /* 定义需擦除的页数 */
    /* 擦除FLASH页 */
    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
		#ifdef PARA_SECTOR_SELECT_16K
				FLASHStatus = FLASH_EraseSector(GetSector(Address + ((u32)FLASH_PAGE_SIZE_16K * EraseCounter)),VoltageRange_3);
		#else
				FLASHStatus = FLASH_EraseSector(GetSector(Address + ((u32)FLASH_PAGE_SIZE * EraseCounter)),VoltageRange_3);
		#endif
    }

    /* 设置FLASH写入首地址 */
    FLASHStatus = FLASH_ProgramHalfWord(StartAddr,SAVE_SUCCESS_FLAG); //头部校验码写入
    Address = StartAddr+2;

    //for(i = 0; ((i < SYS_PARA_LEN_STARS) 	   && (FLASHStatus == FLASH_COMPLETE)); i++)
    for(i = 0; ((i < SYS_PARA_LEN_MAX) && (FLASHStatus == FLASH_COMPLETE)); i++)
    {
        FLASHStatus = FLASH_ProgramHalfWord(Address, ReadPara(i));   // 写FLASH
        Address  += 2;
    }

    if(FLASHStatus == FLASH_COMPLETE)
    {
        FLASH_ProgramHalfWord(StartAddr + SAVE_PARA_FLAG_OFFSET, SAVE_SUCCESS_FLAG);//写保存成功标志尾部校验
    }

    /* 锁定FLASH编写擦除控制器 */
    FLASH_Lock();

    ReNewValifyData();
}

/*******************************************************************************
* 函数名 : SaveAttrToFlash
* 描  述 : 保存参数属性到FLASH
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void SaveAttrToFlash(u32 StartAddr)
{
    vu32 NbrOfPage = 0x00;
    u32 EraseCounter = 0x00;
    u32 Address = StartAddr;
    u16 i;
    FLASHStatus = FLASH_COMPLETE;

    /* 解锁FLASH编写擦除控制器 */
    FLASH_Unlock();

    /* 定义需擦除的页数 */
    NbrOfPage = ATTR_PAGE_NUM;

    /* 清除所有FLASH待处理标志 */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR |FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);	//f405 m4

    /* 擦除FLASH页 */
    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FLASH_EraseSector(GetSector(Address + ((u32)FLASH_PAGE_SIZE * EraseCounter)),VoltageRange_3);
    }

    /* 设置FLASH写入首地址 */
    Address = StartAddr;

    for(i = 0; ((i < SYS_PARA_LEN_STARS) && (FLASHStatus == FLASH_COMPLETE)); i++)
    {
        FLASHStatus = FLASH_ProgramHalfWord(Address, SysParaTable[i].ATTR.all);   	// 写FLASH
        Address  += 2;
        FLASHStatus = FLASH_ProgramHalfWord(Address, SysParaTable[i].UPLIMIT);   	// 写FLASH
        Address  += 2;
        FLASHStatus = FLASH_ProgramHalfWord(Address, SysParaTable[i].LOWLIMIT);   	// 写FLASH
        Address  += 2;
    }

    if(FLASHStatus == FLASH_COMPLETE)
    {
        FLASH_ProgramHalfWord(StartAddr + SAVE_ATTR_FLAG_OFFSET, SAVE_SUCCESS_FLAG);//写保存成功标志
    }

    /* 锁定FLASH编写擦除控制器 */
    FLASH_Lock();
}

/*******************************************************************************
* 函数名 : ReadFlashPara
* 描  述 : 读取参数
* 输  入 : Mode  0 读用户参数 1 读出厂参数
* 输  出 : 1 读参数成功 0 读参数失败
* 返  回 : 无
*******************************************************************************/
u16 ReadFlashPara(u16 Mode)
{
    u16* Addr;
    u16 i;

    Addr = (u16*)USER_PARA_ADDR;
    UserParaAdrr[0] = (u16*)Addr;
    UserParaAdrr[1] = ((u16*)((u32)Addr + SAVE_PARA_FLAG_OFFSET));
    UserParaValifyData[0] = *((u16*)((u32)Addr));
    UserParaValifyData[1] = *((u16*)((u32)Addr + SAVE_PARA_FLAG_OFFSET));
    ReadPara(USERPARAVALIFYDATA0) = UserParaValifyData[0];
    ReadPara(USERPARAVALIFYDATA1) = UserParaValifyData[1];

    Addr = (u16*)DEFAULT_PARA_ADDR;
    DeftParaAdrr[0] = (u16*)Addr;
    DeftParaAdrr[1] = ((u16*)((u32)Addr + SAVE_PARA_FLAG_OFFSET));
    DeftParaValifyData[0] = *((u16*)((u32)Addr));							//调出厂参数，Mode=1，Addr=0x080E 0000，TTTTTT=0x080F FFFE，TTTTTT1=0xAA55，
    DeftParaValifyData[1] = *((u16*)((u32)Addr + SAVE_PARA_FLAG_OFFSET));	//程序判断尾部校验合法，但JLINK仿真界面看到的0x080F FFFE地址数据为0xAAAA
    ReadPara(DEFTPARAVALIFYDATA0) = DeftParaValifyData[0];
    ReadPara(DEFTPARAVALIFYDATA1) = DeftParaValifyData[1];

    if (ReadPara(DEFTPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG)
    {
        FlashValifyErr = 4;
    }
    else if (ReadPara(DEFTPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)
    {
        FlashValifyErr = 3;
    }
    else if (ReadPara(USERPARAVALIFYDATA1) != SAVE_SUCCESS_FLAG)
    {
        FlashValifyErr = 2;
    }
    else if (ReadPara(USERPARAVALIFYDATA0) != SAVE_SUCCESS_FLAG)
    {
        FlashValifyErr = 1;
    }

    ReadPara(PARAVALIFYERR) = FlashValifyErr;

// 	#define	USERPARAVALIFYDATA0           	904	//	用户参数扇区头地址校验字节，正常时应为：0x080C 0000=0xAA、0x080C 0001=0x55
// 	#define	USERPARAVALIFYDATA1           	905	//	用户参数扇区尾地址校验字节，正常时应为：0x080D FFFE=0xAA、0x080D FFFF=0x55
// 	#define	DEFTPARAVALIFYDATA0           	906	//	出厂参数扇区头地址校验字节，正常时应为：0x080E 0000=0xAA、0x080E 0001=0x55
// 	#define	DEFTPARAVALIFYDATA1           	907	//	出厂参数扇区尾地址校验字节，正常时应为：0x080F FFFE=0xAA、0x080F FFFF=0x55
// 	#define	PARAVALIFYERR                	908	//	0=校验全部正确；1=用户参数头地址校验错误；2=用户参数头尾地址校验错误；3=用户和出厂参数头地址校验错误；4=用户和出厂参数头尾地址校验错误

    if (Mode == 0)
    {
        Addr = (u16*)USER_PARA_ADDR;
    }
    else
    {
        Addr = (u16*)DEFAULT_PARA_ADDR;
    }

    if( *((u16*)((u32)Addr)) != SAVE_SUCCESS_FLAG)								//头部校验
    {
        return(0);				//FLASH区参数不合法，返回0
    }

    if( *((u16*)((u32)Addr + SAVE_PARA_FLAG_OFFSET)) != SAVE_SUCCESS_FLAG)		//尾部校验
    {
        return(0);				//FLASH区参数不合法，返回0
    }

    Addr = Addr+1;//首部的两个字节用来进行头部校验
	
    //for(i = 0; i < SYS_PARA_LEN_STARS; i++)
    for(i = 0; i < SYS_PARA_LEN_MAX; i++)
    {
        SysParaTable[i].DATA = *Addr;
        Addr++;
    }

    ReadPara(USERPARAVALIFYDATA0) = UserParaValifyData[0];
    ReadPara(USERPARAVALIFYDATA1) = UserParaValifyData[1];
    ReadPara(DEFTPARAVALIFYDATA0) = DeftParaValifyData[0];
    ReadPara(DEFTPARAVALIFYDATA1) = DeftParaValifyData[1];
    ReadPara(PARAVALIFYERR) = FlashValifyErr;

    return(1);		//1 读参数成功
}

/*******************************************************************************
* 函数名 : EraseFlash
* 描  述 : 擦除FLASH
* 输  入 : u32 StartAddr: 32位待擦除页起始地址；
*          u8  FlashNum:  8位待擦除Flash页数
* 输  出 : 无
* 返  回 : 0：擦除失败；1：擦除成功
*******************************************************************************/
u8 EraseFlash(u32 StartAddr, u8 FlashNum)
{
    vu32 NbrOfPage = 0x00;
    u32 EraseCounter = 0x00;
    u32 Address = StartAddr;
    FLASHStatus = FLASH_COMPLETE;

    /* 解锁FLASH编写擦除控制器 */
    FLASH_Unlock();

    /* 定义需擦除的页数 */
    NbrOfPage = FlashNum;

    /* 清除所有FLASH待处理标志 */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR |FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);	//f405 m4
    /* 擦除FLASH页 */
    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
		#ifdef PARA_SECTOR_SELECT_16K
				FLASHStatus = FLASH_EraseSector(GetSector(Address + ((u32)FLASH_PAGE_SIZE_16K * EraseCounter)),VoltageRange_3);
		#else
				FLASHStatus = FLASH_EraseSector(GetSector(Address + ((u32)FLASH_PAGE_SIZE * EraseCounter)),VoltageRange_3);
		#endif
    }

    /* 锁定FLASH编写擦除控制器 */
    FLASH_Lock();
    return (1);
}

/*******************************************************************************
* 函数名 : EraseFlash_IAP
* 描  述 : 擦除BootLoader 16K扇区
* 输  入 : u32 StartAddr: 32位待擦除页起始地址；
*          u8  FlashNum:  8位待擦除Flash页数
* 输  出 : 无
* 返  回 : 0：擦除失败；1：擦除成功
*******************************************************************************/
u8 EraseFlash_IAP(u32 StartAddr, u8 FlashNum)
{
    vu32 NbrOfPage = 0x00;
    u32 EraseCounter = 0x00;
    u32 Address = StartAddr;
    FLASHStatus = FLASH_COMPLETE;

    /* 解锁FLASH编写擦除控制器 */
    FLASH_Unlock();

    /* 定义需擦除的页数 */
    NbrOfPage = FlashNum;

    /* 清除所有FLASH待处理标志 */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR |FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);	//f405 m4
    /* 擦除FLASH页 */
    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
		FLASHStatus = FLASH_EraseSector(GetSector(Address + ((u32)FLASH_PAGE_SIZE_16K * EraseCounter)),VoltageRange_3);
    }

    /* 锁定FLASH编写擦除控制器 */
    FLASH_Lock();
    return (1);
}

/*******************************************************************************
* 函数名 : WriteHalfWordToFlash
* 描  述 : 按半字即16位写FLASH
* 输  入 : u32 StartAddr: 32位写起始地址；
*          u16* pWrData: 16位待写入数据的起始指针
*          u32 Len: 32位写数据长度
* 输  出 : 无
* 返  回 : 0：擦除失败；1：擦除成功
*******************************************************************************/
void WriteHalfWordToFlash(u32 StartAddr, u16* pWrData, u32 Len)
{
    vu32 NbrOfPage = 0x00;
    u32 Address = StartAddr;
    u16 i;
    FLASHStatus = FLASH_COMPLETE;

    /* 解锁FLASH编写擦除控制器 */
    FLASH_Unlock();

    /* 清除所有FLASH待处理标志 */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR |FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);	//f405 m4

    /* 设置FLASH写入首地址 */
    Address = StartAddr;

    for(i = 0; ((i < Len) && (FLASHStatus == FLASH_COMPLETE)); i++)
    {
        FLASHStatus = FLASH_ProgramHalfWord(Address, pWrData[i]);   // 写FLASH
        Address  += 2;
    }

    /* 锁定FLASH编写擦除控制器 */
    FLASH_Lock();
}

/*******************************************************************************
* 函数名 : WriteOptionByteDataToFlash
* 描  述 : 按字节即8位写FLASH
* 输  入 : u32 StartAddr: 32位写起始地址
*          u16* pWrData: 8位待写入数据的起始指针
*          u32 Len: 32位写数据长度
* 输  出 : 无
* 返  回 : 0：擦除失败；1：擦除成功
*******************************************************************************/
void WriteOptionByteDataToFlash(u32 StartAddr, u8* pWrData, u32 Len)
{
    vu32 NbrOfPage = 0x00;
    //u32 EraseCounter = 0x00;
    u32 Address = StartAddr;
    u16 i;
    FLASHStatus = FLASH_COMPLETE;

    /* 解锁FLASH编写擦除控制器 */
    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR |FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);	//f405 m4

    /* 设置FLASH写入首地址 */
    Address = StartAddr;

    for(i = 0; ((i < Len) && (FLASHStatus == FLASH_COMPLETE)); i++)
    {
        FLASHStatus = FLASH_ProgramByte(Address, pWrData[i]);   // 写FLASH
        Address  += 1;
    }

    /* 锁定FLASH编写擦除控制器 */
    FLASH_Lock();
}

void InitMotorOffAddr(void)
{
    u32 i;

    MotoroffsetAddr = (u16*)MOTOR_ADDR;						// 0x0804 0000
    MotoroffStartAddr = (u16*)MOTOR_ADDR;					// 0x0804 0000

    MotoroffEndAddr = (u16*)MOTOR_ADDR_END;					// 0x0805 FF00（接近128K处）
    MotoroffEraAddr = (u16*)MOTOR_ADDR_ERA;					// 0x0804 FFFF（64K处）

    MotorPowerOffsetH = *((u16*)((u32)MotoroffsetAddr));
    MotoroffsetAddr++;										// 0x0804 0002
    MotorPowerOffsetL = *((u16*)((u32)MotoroffsetAddr));
    MotoroffsetAddr++;										// 0x0804 0004
    MotorPowerOffsetC = *((u16*)((u32)MotoroffsetAddr));

    MotoroffsetAddr = (u16*)MOTOR_ADDR;						// 0x0804 0000

    if ((MotorPowerOffsetH == 0xFFFF) && (MotorPowerOffsetL == 0xFFFF) && (MotorPowerOffsetC == 0xFFFF)) // 扇区头连续6个字节为0xFF，则扇区为空，从0x0804 0000开始写
    {
        Power = 1;

        ReadPara(MOTO_ANGL_DISPH) = (u16)MotorPowerOffsetH;
        ReadPara(MOTO_ANGL_DISPL) = (u16)MotorPowerOffsetL;
        ReadPara(MOTO_ANGL_DISPC) = (u16)MotorPowerOffsetC;

        ReadPara(MOTO_ADDR_DISPH) = 0x0804;
        ReadPara(MOTO_ADDR_DISPL) = 0x0000;
    }
    else
    {
        for (i = 0; i < (FLASH_PAGE_SIZE / 6); i++)
        {
            MotorPowerOffsetH = *((u16*)((u32)MotoroffsetAddr));
            MotoroffsetAddr++;
            MotorPowerOffsetL = *((u16*)((u32)MotoroffsetAddr));
            MotoroffsetAddr++;
            MotorPowerOffsetC = *((u16*)((u32)MotoroffsetAddr));
            MotoroffsetAddr++;

            if ((MotorPowerOffsetH == 0xFFFF) && (MotorPowerOffsetL == 0xFFFF)  && (MotorPowerOffsetC == 0xFFFF))	// 检测到连续6个字节都为0xFF
            {
                break;
            }
        }
    }
}

void InitMotorOffPara(void)
{
    static u32 MotoroffsetAddr_Disp=0;

    if (MotoroffsetAddr - MotoroffStartAddr >= 6)
    {
        MotoroffsetAddr = MotoroffsetAddr - 6;
    }

    if (Power != 1)//首次上电不读参数值
    {
        MotorPowerOffsetH = *((u16*)((u32)MotoroffsetAddr));
        MotoroffsetAddr++;
        MotorPowerOffsetL = *((u16*)((u32)MotoroffsetAddr));
        MotoroffsetAddr++;
        MotorPowerOffsetC = *((u16*)((u32)MotoroffsetAddr));

        MotorPowerOffset = (s32)(((u32)MotorPowerOffsetH<<16) | MotorPowerOffsetL);

        MotorPowerOffsetCRC[0] = (u8)(MotorPowerOffsetH);
        MotorPowerOffsetCRC[1] = (u8)(MotorPowerOffsetH/256);
        MotorPowerOffsetCRC[2] = (u8)(MotorPowerOffsetL);
        MotorPowerOffsetCRC[3] = (u8)(MotorPowerOffsetL/256);
        MotorPowerOffsetCRC[4] = (u8)(MotorPowerOffsetC);
        MotorPowerOffsetCRC[5] = (u8)(MotorPowerOffsetC/256);

        //if (CRC16(MotorPowerOffsetCRC, 4) == (MotorPowerOffsetCRC[5] * 256 + MotorPowerOffsetCRC[4]) + 1)	// Only For Test
        if (CRC16(MotorPowerOffsetCRC, 4) == (MotorPowerOffsetCRC[5] * 256 + MotorPowerOffsetCRC[4]))
        {
            AngleCrcErr = 0;
        }
        else
        {
            AngleCrcErr = 1;
        }

        ReadPara(MOTO_ANGL_DISPH) = (u16)MotorPowerOffsetH;
        ReadPara(MOTO_ANGL_DISPL) = (u16)MotorPowerOffsetL;
        ReadPara(MOTO_ANGL_DISPC) = (u16)MotorPowerOffsetC;

        MotoroffsetAddr++;
        MotoroffsetAddr_Disp = (u32)MotoroffsetAddr;//当前地址送上位机显示
        ReadPara(MOTO_ADDR_DISPH) = (u32)MotoroffsetAddr_Disp >> 16;
        ReadPara(MOTO_ADDR_DISPL) = (u32)MotoroffsetAddr_Disp & 0xffff;
    }
    else
    {
        ReadPara(MOTO_ADDR_DISPH) = 0x0804;//强行写
        ReadPara(MOTO_ADDR_DISPL) = 0x0000;
    }
}