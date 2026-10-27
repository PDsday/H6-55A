#include "sdio_sdcard.h"
#include "sdcard.h"
#include "stdbool.h"
#include "ParaProcess.h"
#include "ExternPrototype.h"
#include "ExterUart.H"
#include "IO.h"
#include "InterUart.H"
//======================SD卡存储参数变量定义区=========================
extern u16 UserParaValifyData[2];
extern u16 DeftParaValifyData[2];
extern u16 FlashValifyErr;
extern u8  ChargeSaveFlag;
extern u16 Func_0x06_Data[];
extern u16 ErrRecordDataArray[]; 																// 故障代码记录数组
extern u32 SysParaBuff[];
extern u32 EncryptParaBuff[];
extern u32 SysParaBuffBack[];
extern u32 EncryptParaBuffBack[];
extern u8  Sd_init_Errcode;

u32 RuningRecData;																				//数据记录地址
u32 AddrrecAddr;																				//地址记录地址
u8 	SdCardErrDataCnt = 0;																		//故障数据笔数记录		
u8 	Read_Write_Carrier[512];																	//读与写的载体，以512个字节为一包数据进行读写
u8 	Sd_Card_bare[12] = {0x01,0xc0,0x07,0x55,0xaa,0xaa,0x55,0x5a,0x5a,0xa5,0x84,0xac};			//固定数据以及CRC校验标志位
u8  SDRecordFlag = 0;																			//记录地址标志

//=========================SD卡读与写函数的统一参数=========================
u32 sd_check_start_addr = 0;			//首部校验标志的地址
u32 sd_check_end_addr = 0;				//尾部校验标志的地址
u32 sd_data_addr = 0;					//SD卡参数的起始地址以及地址
u16 sd_para_lentgh =0;					//需要写入的参数的长度
u16 sd_Pagenum = 0;						//需要写入参数整的页的数量
u16 sd_Remaind = 0;						//剩余参数个数，不足512补0xff
u16 sd_arry_start = 0;					//写入的参数在数组中的起始位置；例如在结构体中0——999为星辰用户参数，则该参数在结构体的起始地址为：0
u16 readwrite_count =0;					//读写计数
u16 add_countset = 0;					//递增值
u16 sd_get_para = 0;					//用于承接结构体的数据的值
u8  read_datah =0;						//读回来数据的高位
u8  read_datal =0;						//读回来数据的低位
u16 modeselect = 0;						//模式选择

extern u8 SD_rescount;                  //允许尝试SD卡初始化的最大次数
extern u16 SaveMode;

/*
************************************************************
*	函数名称：	SaveParaToSdCard
*
*	函数功能：	保存参数到ARM外挂的128M SD卡
*
*	入口参数：	写入SD卡的起始地址
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void SaveParaToSdCard(u32 StartAddr)
{
	u16 i,j;
	sd_data_addr = StartAddr;			//SD卡写参数的起始地址以及地址
	sd_check_start_addr = 0;			//首部校验标志的地址
	sd_check_end_addr = 0;				//尾部校验标志的地址
	sd_para_lentgh =0;					//需要写入的参数的长度
	sd_Pagenum = 0;						//需要写入参数整的页的数量
	sd_Remaind = 0;						//剩余参数个数，不足512补0xff
	sd_arry_start = 0;					//写入的参数在数组中的起始位置；例如在结构体中0——999为星辰用户参数，则该参数在结构体的起始地址为：0
	readwrite_count =0;					//写计数
	add_countset = 0;					//递增值
	sd_get_para = 0;					//用于承接结构体的数据的值
	
	switch(sd_data_addr)
    {
		case STARS_USER_PARA_ADDR:				//星辰用户参数
			sd_check_start_addr = 6144;		//校验首地址
			sd_check_end_addr = 11776;		//校验尾地址
			sd_para_lentgh = (STARS_USERDEFAULT_PARA_LENGTH *2);//数据长度 u16 = u8 *2
			sd_arry_start = 0;
			break;
		case STARS_DEFAULT_PARA_ADDR:			//星辰出厂参数
			sd_check_start_addr = 12288;	//校验首地址
			sd_check_end_addr = 17920;		//校验尾地址
			sd_para_lentgh = (STARS_USERDEFAULT_PARA_LENGTH *2);//数据长度 u16 = u8 *2
			sd_arry_start = 0;
			break;
		case WINDEY_AUTOSTUDY_PARA_ADDR:		//运达用户参数
			sd_check_start_addr = 0;		//校验首地址
			sd_check_end_addr = 2560;		//校验尾地址
			sd_para_lentgh =	(WINDEY_AUTOSTUDY_PARA_ADDR_LENGTH *2);	//数据长度 u16 = u8 *2
			sd_arry_start = 1300;
			break;
		case WINDEY_AUTOSTUDY_BACKUP_PARA_ADDR:	//运达出厂参数
			sd_check_start_addr = 3072;		//校验首地址
			sd_check_end_addr = 5632;		//校验尾地址
			sd_para_lentgh =   (WINDEY_AUTOSTUDY_PARA_ADDR_LENGTH *2);	//数据长度 u16 = u8 *2
			sd_arry_start = 1300;
			break;
		case WINDEY_UNLOCK_PARA_ADDR:			//运达激活参数
			sd_check_start_addr = 18432;	//校验首地址
			sd_check_end_addr = 19456;		//校验尾地址
			sd_para_lentgh = (WINDEY_UNLOCK_PARA_ADDR_LENGTH * 2);		//数据长度 u16 = u8 *2
			sd_arry_start = 1360;
			break;
		case WINDEY_UNLOCK_BACKUP_PARA_ADDR:	//运达激活参数备份
			sd_check_start_addr = 19968;	//校验首地址
			sd_check_end_addr = 20992;		//校验尾地址
			sd_para_lentgh =	(WINDEY_UNLOCK_PARA_ADDR_LENGTH * 2); 	//数据长度 u16 = u8 *2
			sd_arry_start = 1360;
			break;
		default:
			break;
    }
	
    sd_Pagenum = (sd_para_lentgh/512);		//需要写的页数
    sd_Remaind = (sd_para_lentgh%512);		//如果有剩余的不够512，则单独写一页
	
    if(sd_Remaind != 0)
    {
        sd_Pagenum = sd_Pagenum +1;			//如果有剩余的不够512，则单独写一页
    }

    //先写入首校验位
    Read_Write_Carrier[0] = 0x55;
    Read_Write_Carrier[1] = 0xaa;
    SD_WriteBlock((u8*)&Read_Write_Carrier,sd_check_start_addr,512);  								//写头的校验值
	
    for(i = 0; i<(sd_para_lentgh/2); i++)
    {
        sd_get_para = ReadPara(sd_arry_start);														//将systempara换成readpara即可
        sd_arry_start = sd_arry_start + 1;

        Read_Write_Carrier[add_countset] = ((sd_get_para >>8) & 0xff);
        Read_Write_Carrier[++add_countset] = (sd_get_para & 0x00ff);

        add_countset = add_countset+1;
        readwrite_count  = readwrite_count+1;

        if(readwrite_count > 512)
        {
            break;
        }

        if(readwrite_count == 256)																	//封装好一包数据了
        {
            SD_WriteBlock((u8*)&Read_Write_Carrier,sd_data_addr,512);                   			//按块操作写数据
            sd_data_addr = sd_data_addr + 512;
            readwrite_count = 0;
            add_countset = 0;
        }
        else if(i == ((sd_para_lentgh/2)-1) && readwrite_count == (sd_Remaind/2))					//不够512个字节的
        {
            for(j = sd_Remaind; j < 512; j++)
            {
                Read_Write_Carrier[add_countset] = 0xff;
                add_countset = add_countset + 1;
            }

            SD_WriteBlock((u8*)&Read_Write_Carrier,sd_data_addr,512);								//按块操作写数据
        }

    }
	Read_Write_Carrier[0] = 0x55;
    Read_Write_Carrier[1] = 0xaa;
    SD_WriteBlock((u8*)&Read_Write_Carrier,sd_check_end_addr,512);  						 		//写结尾的校验值

}



/*
************************************************************
*	函数名称：	ReadStarsSdcardPara
*
*	函数功能：	读取SD卡星辰参数
*
*	入口参数：	Mode  0 读星辰用户参数 1 读星辰出厂参数
*
*	返回参数：	1:读参成功    0：读参失败
*
*	说明：		需校验首尾部的校验标志位
************************************************************
*/
u16 ReadStarsSdcardPara(u16 Mode)
{   
	u16 i,j;
	sd_check_start_addr = 0;			//首部校验标志的地址
	sd_check_end_addr = 0;				//尾部校验标志的地址
	sd_data_addr = 0;					//SD卡写参数的起始地址以及地址
	sd_para_lentgh =0;					//需要写入的参数的长度
	sd_Pagenum = 0;						//需要写入参数整的页的数量
	sd_Remaind = 0;						//剩余参数个数，不足512补0xff
	sd_arry_start = 0;					//写入的参数在数组中的起始位置；例如在结构体中0——999为星辰用户参数，则该参数在结构体的起始地址为：0
	readwrite_count =0;					//写计数
	add_countset = 0;					//递增值
	read_datah =0;						//读回来数据的高位
	read_datal =0;						//读回来数据的低位
	modeselect = Mode;
	
    //加上头部与尾部校验值是否正
    SD_ReadBlock((u8*)&Read_Write_Carrier,6144,512);				//按块读取
    UserParaValifyData[0] = ((Read_Write_Carrier[0] * 256) + Read_Write_Carrier[1]);//用户参数的头部校验值
    SD_ReadBlock((u8*)&Read_Write_Carrier,11776,512);			//按块读取
    UserParaValifyData[1] = ((Read_Write_Carrier[0] * 256) + Read_Write_Carrier[1]);//用户参数的尾部校验值
    ReadPara(USERPARAVALIFYDATA0) = UserParaValifyData[0];
    ReadPara(USERPARAVALIFYDATA1) = UserParaValifyData[1];


    SD_ReadBlock((u8*)&Read_Write_Carrier,12288,512);				//按块读取
    DeftParaValifyData[0] =	((Read_Write_Carrier[0] * 256) + Read_Write_Carrier[1]);//出厂参数的头部校验值
    SD_ReadBlock((u8*)&Read_Write_Carrier,17920,512);				//按块读取
    DeftParaValifyData[1] = ((Read_Write_Carrier[0] * 256) + Read_Write_Carrier[1]);//出厂参数的尾部校验值

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

    switch(modeselect)
    {
		case 0:				//读星辰用户参数
			sd_check_start_addr = 6144;
			sd_check_end_addr   = 11776;
			sd_data_addr = STARS_USER_PARA_ADDR;

			break;
		case 1:				//读星辰出厂参数
			sd_check_start_addr = 12288;
			sd_check_end_addr   = 17920;
			sd_data_addr = STARS_DEFAULT_PARA_ADDR;

			break;
		default:
			break;
    }
    sd_arry_start = 0;
    sd_para_lentgh = (STARS_USERDEFAULT_PARA_LENGTH * 2);		//u16 = u8 *2
    sd_Pagenum = (sd_para_lentgh/512);						//需要读多少页
    sd_Remaind = (sd_para_lentgh % 512);
    if(sd_Remaind != 0)											//如果有剩余单独读一页
    {
        sd_Pagenum =sd_Pagenum +1;
    }
	
    //校验首尾的校验标志位
	SD_ReadBlock((u8*)&Read_Write_Carrier,sd_check_start_addr,512);			//按块读取
    if(Read_Write_Carrier[0] == 0x55 && Read_Write_Carrier[1] == 0xaa)	//首部校验正确
    {
        Read_Write_Carrier[0] = 0;
        Read_Write_Carrier[1] = 0;
		
        SD_ReadBlock((u8*)&Read_Write_Carrier,sd_check_end_addr,512);
		
        if(Read_Write_Carrier[0] != 0x55 || Read_Write_Carrier[1] != 0xaa)
        {
            return(0);														//尾部校验失败
        }
    }
    else																	//首部校验错误
    {
        return(0);
    }
    for(i=0; i<sd_Pagenum; i++)
    {
        SD_ReadBlock((u8*)&Read_Write_Carrier,sd_data_addr,512);					//按块读取
        for(j=0; j<256; j++)
        {
            read_datah = Read_Write_Carrier[add_countset];
            read_datal = Read_Write_Carrier[++add_countset];
			SysParaTable[sd_arry_start].DATA  = (read_datah * 256) + read_datal;	//把systemsavepara换成目标数组
            sd_arry_start = sd_arry_start +1;
            readwrite_count++;

            add_countset = add_countset +1;

            if(readwrite_count == ((sd_para_lentgh/2)))
            {			
                break;
            }
        }
        add_countset = 0;
        sd_data_addr = sd_data_addr + 512;
    }


    ReadPara(DEFTPARAVALIFYDATA0) = DeftParaValifyData[0];
    ReadPara(DEFTPARAVALIFYDATA1) = DeftParaValifyData[1];
    ReadPara(USERPARAVALIFYDATA0) = UserParaValifyData[0];
    ReadPara(USERPARAVALIFYDATA1) = UserParaValifyData[1];
    ReadPara(PARAVALIFYERR) = FlashValifyErr;

    return 1;	//成功
}


/*
************************************************************
*	函数名称：	ReadWindeySdcardPara
*
*	函数功能：	读取SD卡运达参数
*
*	入口参数：	Mode  0 读运达自学习参数 1 读运达加密激活参数
*
*	返回参数：	1:读参成功    0：读参失败
*
*	说明：		需校验首尾部不需要校验
************************************************************
*/
u16 ReadWindeySdcardPara(u16 Mode)
{
    u16 i,j;
	sd_check_start_addr = 0;			//首部校验标志的地址
	sd_check_end_addr = 0;				//尾部校验标志的地址
	sd_data_addr = 0;					//SD卡写参数的起始地址以及地址
	sd_para_lentgh =0;					//需要写入的参数的长度
	sd_Pagenum = 0;						//需要写入参数整的页的数量
	sd_Remaind = 0;						//剩余参数个数，不足512补0xff
	sd_arry_start = 0;					//写入的参数在数组中的起始位置；例如在结构体中0——999为星辰用户参数，则该参数在结构体的起始地址为：0
	readwrite_count =0;					//写计数
	add_countset = 0;					//递增值
	read_datah =0;						//读回来数据的高位
	read_datal =0;						//读回来数据的低位
	modeselect = Mode;

    switch(modeselect)
    {
    case 0:				//读运达自学习参数
        sd_check_start_addr = 0;
        sd_check_end_addr   = 2560;
        sd_data_addr = WINDEY_AUTOSTUDY_PARA_ADDR;
        sd_arry_start = 1300;
        sd_para_lentgh = (WINDEY_AUTOSTUDY_PARA_ADDR_LENGTH * 2);			//u16 = u8 *2

        break;
    case 1:				//读运达加密激活参数
        sd_check_start_addr = 18432;
        sd_check_end_addr   = 19456;
        sd_data_addr = WINDEY_UNLOCK_PARA_ADDR;
        sd_arry_start = 1360;
        sd_para_lentgh = (WINDEY_UNLOCK_PARA_ADDR_LENGTH * 2);				//u16 = u8 *2

        break;
    default:
        break;
    }
    sd_Pagenum = (sd_para_lentgh/512);										//需要读多少页
    sd_Remaind = (sd_para_lentgh % 512);
    if(sd_Remaind != 0)														//如果有剩余单独读一页
    {
        sd_Pagenum = sd_Pagenum +1;
    }

#if 0
    //校验首尾的校验标志位
    SD_ReadBlock((u8*)&Read_Write_Carrier,sd_check_start_addr,512);			//按块读取
    if(Read_Write_Carrier[0] == 0x55 && Read_Write_Carrier[1] == 0xaa)		//首部校验正确
    {
        Read_Write_Carrier[0] = 0;
        Read_Write_Carrier[1] = 0;
        SD_ReadBlock((u8*)&Read_Write_Carrier,sd_check_end_addr,512);
        if(Read_Write_Carrier[0] != 0x55 || Read_Write_Carrier[1] != 0xaa)
        {
            return(0);														//尾部校验失败
        }
    }
    else																	//首部校验错误
    {
        return(0);
    }
#endif

    for(i=0; i<sd_Pagenum; i++)
    {
        SD_ReadBlock((u8*)&Read_Write_Carrier,sd_data_addr,512);				//按块读取
        for(j=0; j<256; j++)
        {
            read_datah = Read_Write_Carrier[add_countset];
            read_datal = Read_Write_Carrier[++add_countset];
			SysParaTable[sd_arry_start].DATA = (read_datah * 256) + read_datal;	//把systemsavepara换成目标数组
            sd_arry_start = sd_arry_start +1;
            readwrite_count++;
            add_countset = add_countset +1;

            if(readwrite_count == ((sd_para_lentgh/2)))
            {             
                break;
            }
        }
        add_countset = 0;
        sd_data_addr = sd_data_addr + 512;
    }

    return 1;	//成功
}

/*******************************************************************************
* 函数名 :
* 描  述 : 更新SD卡参数存盘后的校验字节
* 输  入 :
* 输  出 :
* 返  回 :
*******************************************************************************/
void ReNewValifyData_SdCard(void)
{
    //加上头部与尾部校验值是否正
    SD_ReadBlock((u8*)&Read_Write_Carrier,6144,512);				//按块读取
    UserParaValifyData[0] = ((Read_Write_Carrier[0] * 256) + Read_Write_Carrier[1]);//用户参数的头部校验值
    SD_ReadBlock((u8*)&Read_Write_Carrier,11776,512);			//按块读取
    UserParaValifyData[1] = ((Read_Write_Carrier[0] * 256) + Read_Write_Carrier[1]);//用户参数的尾部校验值
    ReadPara(USERPARAVALIFYDATA0) = UserParaValifyData[0];
    ReadPara(USERPARAVALIFYDATA1) = UserParaValifyData[1];


    SD_ReadBlock((u8*)&Read_Write_Carrier,12288,512);				//按块读取
    DeftParaValifyData[0] =	((Read_Write_Carrier[0] * 256) + Read_Write_Carrier[1]);//出厂参数的头部校验值
    SD_ReadBlock((u8*)&Read_Write_Carrier,17920,512);				//按块读取
    DeftParaValifyData[1] = ((Read_Write_Carrier[0] * 256) + Read_Write_Carrier[1]);//出厂参数的尾部校验值

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
	
	if (SaveMode == 2)
	{
		FlashValifyErr = 0;
		ReadPara(USERPARAVALIFYDATA0) = SAVE_SUCCESS_FLAG;
		ReadPara(USERPARAVALIFYDATA1) = SAVE_SUCCESS_FLAG;
		ReadPara(DEFTPARAVALIFYDATA0) = SAVE_SUCCESS_FLAG;
		ReadPara(DEFTPARAVALIFYDATA1) = SAVE_SUCCESS_FLAG;
	}		

	ReadPara(PARAVALIFYERR) = FlashValifyErr;
}

/*
************************************************************
*	函数名称：	SavePara1
*
*	函数功能：	保存参数到ARM外挂的SD卡
*
*	入口参数：
*		Mode 	0 ：无需保存
* 				1 ：需要保存星辰用户参数（星辰上位机参数保存、或用户参数扇区已发生错误后将出厂参数数据写入用户参数扇区）
* 				2 : 需要保存星辰用户参数和出厂默认参数（星辰上位机参数保存+Ctrl、或用户扇区和出厂扇区同时发送错误后将程序默认值写入这两个扇区）
* 				3 : 需要保存运达HMI自学习参数
* 				4 : 需要保存运达加密激活参数
* 				5 : 需要保存星辰出厂默认参数（出厂参数扇区已发生错误，将用户参数数据写入出厂参数扇区）
*
*	返回参数：	1
*
*	说明：
************************************************************
*/
s16 SavePara1(u16 Mode)
{
	{
		u32 i = 0;
		u32 PVDLevel_Cnt = 0;
		
		for (i = 0; i < 2000; i++)
		{
			if ((PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET))
			{
				PVDLevel_Cnt = 0;
			}
			else
			{
				PVDLevel_Cnt ++;
			}
		}
				
		if (PVDLevel_Cnt < 1999)
		{
			i = 0;
			PVDLevel_Cnt = 0;	
			return;
		}
	}
	
    if(Mode)
    {
        switch (Mode) 	// 0 无需保存 1 需要保存用户参数
        {
			case 1:  	// 1 需要保存星辰用户参数（星辰上位机参数保存、或用户参数扇区已发生错误后将出厂参数数据写入用户参数扇区）
			{
				SaveParaToSdCard(STARS_USER_PARA_ADDR);
				ReNewValifyData_SdCard();					//复位一下标志
				break;
			}
			case 2:  	// 2 需要保存星辰用户参数和出厂默认参数（星辰上位机参数保存+Ctrl、或用户扇区和出厂扇区同时发送错误后将程序默认值写入这两个扇区）
			{
				SaveParaToSdCard(STARS_USER_PARA_ADDR);  	//先保存用户
				SaveParaToSdCard(STARS_DEFAULT_PARA_ADDR);	//再保存出厂
				ReNewValifyData_SdCard();					//复位一下标志
				break;
			}
			case 5:  	// 5 需要保存星辰出厂默认参数（出厂参数扇区已发生错误，将用户参数数据写入出厂参数扇区）
			{
				SaveParaToSdCard(STARS_DEFAULT_PARA_ADDR);
				ReNewValifyData_SdCard();					//复位一下标志
				break;
			}

			case 3:  	// 3 需要保存运达HMI自学习参数
			{
				SaveParaToSdCard(WINDEY_AUTOSTUDY_PARA_ADDR);
				ReNewValifyData_SdCard();					//复位一下标志
				break;
			}
			case 4:  	// 4 需要保存运达加密激活参数
			{
				SaveParaToSdCard(WINDEY_UNLOCK_PARA_ADDR);
				ReNewValifyData_SdCard();					//复位一下标志
				break;
			}
			
			default:
			{
				break;
			}
        }

        //ReNewValifyData_SdCard();						//复位一下标志
    }
    return(1);
}

/*******************************************************************************
* 函数名 : 	Sd_Card_Data_Handling
* 描  述 : 	SD卡参数处理函数
* 输  入 :
* 输  出 : 
* 返  回 :
*******************************************************************************/
void Sd_Card_Data_Handling(void)
{
	if ((ReadPara(SYS_ST_CODE) & 0x0004) == 0)			//确保关使能点保存才响应
	{
		if (GetSaveMode() != 0)							//返回保存指令 0 无需保存 1 需要保存用户参数 2 需要保存用户参数和出厂默认参数
		{
			if (GetSaveMode() >= 3)						//SaveMode=3代表需存盘HMI自学习相关参数、SaveMode=4代表需存盘加密激活数据
			{
				u16 i = 0;
				u16 j = 0;
				
				#ifndef TEST_FOR_STARS
					if (GetSaveMode() == 3)
					{
						j = 0;
						for (i = 0; i < 30; i = i+1)
						{
							ReadPara(j+SYS_PARA_AUTOSTUDY) = SysParaBuff[i] >> 16;
							ReadPara(j+SYS_PARA_AUTOSTUDY+1) = SysParaBuff[i];
							SysParaBuffBack[i] = SysParaBuff[i];
							j = j+2;
						}
						//ReadPara(MOTO_TYPE) = DoubleRAM.SelcetMotorType;
						ReadPara(MOTO_TYPE) = SysParaBuff[8];
					}
					else if (GetSaveMode() == 4)
					{
						j = 0;
						for (i = 0; i < 30; i = i+1)
						{
							ReadPara(j+1360) = EncryptParaBuff[i] >> 16;
							ReadPara(j+1360+1) = EncryptParaBuff[i];
							EncryptParaBuffBack[i] = EncryptParaBuff[i];
							j = j+2;
						}
					}
				#else
					//ReadPara(MOTO_TYPE) = Func_0x06_Data[36];
				#endif
			}

			SavePara1(GetSaveMode());
			//ResetSaveFlag();
			if (GetSaveMode() < 3)						//SaveMode=1或2，需存盘充电器参数
			{
				ChargeSaveFlag = 1;						//充电器参数保存标志
			}
			ResetSaveFlag();
		}
	}
	else												//开使能不响应
	{
		if ((GetSaveMode() != 0) && (TxToPCCompleteFlag == 1))
		{
			ResetSaveFlag();
		}
	}
	if(SDRecordFlag)
	{
		RecorddataAddr(RuningRecData);						//记录地址然后加上标志位
	}
	
// 	#if 1
// 		GEshihua();											//模拟参数存储失败
// 	#endif
}

/*
************************************************************
*	函数名称：	Sd_Judge_Flag
*
*	函数功能：	判断标志位
*
*	入口参数：	需要比较的数组
*
*	返回参数：	1：不相同  0：相同
*
*	说明：		
************************************************************
*/
u8 Sd_Judge_Flag(u8 *buf,u8 *buf1)
{
	u8 i= 0;
	for(;i<12;i++)
	{
		if(buf[i] != buf1[i])
		{
			return 1;
		}
	}
	return 0;
}


/*
************************************************************
*	函数名称：	Sd_CardReErrData_Int
*
*	函数功能：	Sd卡记录故障数据初始化
*
*	入口参数：	
*
*	返回参数：	
*
*	说明：		
************************************************************
*/
void Sd_CardReErrData_Int(void)
{
	u8 	i,j;
	u32	sdfindaddr = Geologic_Record_AndMin;
	u32 addr_temp;													//中间变量
	
	SD_ReadBlock((u8*)&Read_Write_Carrier,SDcardBareAdde,512);
	if(Sd_Judge_Flag(Sd_Card_bare,Read_Write_Carrier))				//判断为裸片
	{
		for(i=0;i<12;i++)
		{
			Read_Write_Carrier[i] = Sd_Card_bare[i];
		}
		SD_WriteBlock((u8*)&Read_Write_Carrier,SDcardBareAdde,512);		//写入裸片判断标志位
		RuningRecData = Failure_Data_AndMin;						//历史故障记录起始地址
		AddrrecAddr = Geologic_Record_AndMin;						//记录地址起始地址
	}
	else															//不是裸片
	{
		for(;sdfindaddr < (MaxAddrsAdd+512);)
		{
			SD_ReadBlock((u8*)&Read_Write_Carrier,sdfindaddr,512);
		
				if(Read_Write_Carrier[0] == 0x01 && Read_Write_Carrier[1]==0xc0 && Read_Write_Carrier[2] == 0x07 && Read_Write_Carrier[3] == 0x55 && Read_Write_Carrier[4] == 0xaa)	//找到了标志位
				{
					if(sdfindaddr == Geologic_Record_AndMin)
					{
						addr_temp = MaxAddrsAdd;
					}
					else
					{
						addr_temp = sdfindaddr - 512;				//往回推出记录地址数据的地址
					}
				
					SD_ReadBlock((u8*)&Read_Write_Carrier,addr_temp,512);
					
					RuningRecData = ((Read_Write_Carrier[0]*10000000000)+(Read_Write_Carrier[1]*1000000000)+(Read_Write_Carrier[2]*100000000)+(Read_Write_Carrier[3]*10000000)+(Read_Write_Carrier[4]*1000000)+
						  (Read_Write_Carrier[5]*100000 )+(Read_Write_Carrier[6]*10000)+(Read_Write_Carrier[7]*1000)+(Read_Write_Carrier[8]*100)+(Read_Write_Carrier[9]*10)+(Read_Write_Carrier[10]));		//历史故障记录起始地址
					
					SdCardErrDataCnt = Read_Write_Carrier[11];																												//获取故障数据的笔数				
									
					if(RuningRecData != MaxDataRecordAddr)
					{
						RuningRecData = RuningRecData;
					}
					else
					{
						RuningRecData = Failure_Data_AndMin;		//历史故障记录起始地址
					}
					#if 1
					if(addr_temp == MaxAddrsAdd)
					{
						AddrrecAddr = Geologic_Record_AndMin;
					}
					else
					{
					AddrrecAddr = (addr_temp+512);					//覆盖原来的标志位
					}
					#else
					AddrrecAddr = (addr_temp+512);					//覆盖原来的标志位
					#endif
					
					break;
				}
			
			sdfindaddr = sdfindaddr + 512;
		}
		if((RuningRecData == 0) || (sdfindaddr>MaxAddrsAdd))
		{
			RuningRecData = Failure_Data_AndMin;					//历史故障记录起始地址
			AddrrecAddr = Geologic_Record_AndMin;					//记录地址起始地址
		}
	}
}

/*
************************************************************
*	函数名称：	WriteErrRecorToSdCard
*
*	函数功能：	Sd卡记录故障数据函数
*
*	入口参数：	目标数组，数据长度
*
*	返回参数：	
*
*	说明：		
************************************************************
*/
void WriteErrRecorToSdCard(u16 *writedata,u16 length)
{
	u16 i,j;
	sd_Pagenum = 0;					
	sd_Remaind = 0;
	readwrite_count =0;
	sd_get_para =0;
	add_countset = 0;
	sd_para_lentgh = length *2;						//得到相对u8的数据长度
	sd_Pagenum = (sd_para_lentgh / 512);
	sd_Remaind = (sd_para_lentgh % 512);
	
	for(i= 0; i<(sd_para_lentgh/2);i++)
	{
		sd_get_para = writedata[i];
		Read_Write_Carrier[add_countset] =  ((sd_get_para >>8) & 0xff);
		Read_Write_Carrier[++add_countset] = (sd_get_para & 0x00ff);
		
		add_countset = add_countset+1;
        readwrite_count = readwrite_count+1;
		
		if(readwrite_count > 512)
		{
			break;
		}
		if(readwrite_count == 256)																//封装好一包数据了
        {
			SD_WriteBlock((u8*)&Read_Write_Carrier,RuningRecData,512);                   		//按块操作写数据
			RuningRecData = RuningRecData + 512;
			if(RuningRecData == 4344320)
			{
				RuningRecData = 150016;			
			}		
			readwrite_count = 0;
			add_countset = 0;				
        }
        else if(i == ((sd_para_lentgh/2)-1) && readwrite_count == (sd_Remaind/2))		//不够512个字节的
        {
            for(j = sd_Remaind; j < 512; j++)
            {
                Read_Write_Carrier[add_countset] = 0xff;
                add_countset = add_countset + 1;
            }

            SD_WriteBlock((u8*)&Read_Write_Carrier,RuningRecData,512);						//按块操作写数据
			
			RuningRecData = RuningRecData + 512;
			
			if(RuningRecData == 4344320)
			{
				RuningRecData = 150016;			
			}
        }
	}
	
	if (SdCardErrDataCnt < GZM_Num)
	{
		SdCardErrDataCnt=  SdCardErrDataCnt+1;			//记录一次加一次
	}
	SDRecordFlag = 1;									//开启地址记录
}


/*
************************************************************
*	函数名称：	RecorddataAddr
*
*	函数功能：	记录记录数据的地址
*
*	入口参数：	地址
*
*	返回参数：	
*
*	说明：		
************************************************************
*/
void RecorddataAddr(u32 aidadders)
{
	u8 i;
	sd_data_addr = aidadders;
	
	Read_Write_Carrier[0]=(sd_data_addr/10000000000)%10;      //百亿
    Read_Write_Carrier[1]=(sd_data_addr/1000000000)%10;       //十亿
    Read_Write_Carrier[2]=(sd_data_addr/100000000)%10;        //亿
    Read_Write_Carrier[3]=(sd_data_addr/10000000)%10;         //千万
    Read_Write_Carrier[4]=(sd_data_addr/1000000)%10;          //百万
    Read_Write_Carrier[5]=(sd_data_addr/100000)%10;           //十万
    Read_Write_Carrier[6]=(sd_data_addr/10000)%10;            //万
    Read_Write_Carrier[7]=(sd_data_addr/1000)%10;             //千
    Read_Write_Carrier[8]=(sd_data_addr/100)%10;              //百
    Read_Write_Carrier[9]=(sd_data_addr/10)%10;               //十位
    Read_Write_Carrier[10]=sd_data_addr%10;                   //个位
	Read_Write_Carrier[11]=SdCardErrDataCnt;				  //数据笔数	
	
	SD_WriteBlock((u8*)&Read_Write_Carrier,AddrrecAddr,512);                	//记录最新一笔数据的地址
	AddrrecAddr = AddrrecAddr +512;												//写入标志位的地址
	
	if(AddrrecAddr == 150016)
	{
		AddrrecAddr = 22528;
	}	
	
	for(i=0;i<12;i++)
	{
		Read_Write_Carrier[i] = Sd_Card_bare[i];
	}
	
	SD_WriteBlock((u8*)&Read_Write_Carrier,AddrrecAddr,512);                	//写入标志位
	
	SDRecordFlag = 0;
}


/*
************************************************************
*	函数名称：	ReadErrRecordfromSdcard
*
*	函数功能：	读取SD卡历史故障数据
*
*	入口参数：	
*
*	返回参数：	
*
*	说明：		
************************************************************
*/
void ReadErrRecordfromSdcard(void)
{
	u16 i,j;
	sd_data_addr = RuningRecData;
	sd_Pagenum = 0;					
	sd_Remaind = 0;	
	readwrite_count = 0;
	sd_arry_start = 0;
	//sd_para_lentgh = 570;
	sd_para_lentgh = GZM_InfoNum * GZM_Num;	
	add_countset = 0;
	
	if(sd_data_addr == 0)
	{
		return;
	}
	sd_Pagenum = ((sd_para_lentgh*2) / 512);
	sd_Remaind = ((sd_para_lentgh*2) % 512);
	if(sd_Remaind != 0)
	{
		sd_Pagenum = sd_Pagenum +1;
	}
	sd_data_addr = sd_data_addr -(512*sd_Pagenum);
	for(i=0;i<sd_Pagenum;i++)
	{
		SD_ReadBlock((u8*)&Read_Write_Carrier,sd_data_addr,512);				//按块读取
		for(j=0;j<256;j++)
		{
			read_datah = Read_Write_Carrier[add_countset];
			read_datal = Read_Write_Carrier[++add_countset];
			ErrRecordDataArray[sd_arry_start] = (read_datah * 256) + read_datal;
			sd_arry_start = sd_arry_start +1;
			add_countset = add_countset +1;
			readwrite_count++;
			
			if(readwrite_count == (sd_para_lentgh))
			{
				break;
			}
		}
		
		 add_countset = 0;
		 sd_data_addr = sd_data_addr +512;
	}
}





/*******************************************************************************
* 函数名 : 	GEshihua
* 描  述 : 	模拟参数错误
* 输  入 :
* 输  出 : 	Formatting 1: 清除用户参数标志位  2：清除出场参数的标志位
*					   3：清除用户与出厂参数的标志位
* 返  回 :
*******************************************************************************/
void GEshihua(void)
{
    u8 Formatting = 0;

    Formatting = ReadPara(CLEAR_STARS_PARA_CMD);
	
	ReadPara(SDCARD_ERRCODE) = Sd_init_Errcode;
	
    if(Formatting)
    {
        switch(Formatting)
        {
			case 1:
				SD_WriteBlock(Read_Write_Carrier,6144,512);	//用户参数
				ReadPara(CLEAR_STARS_PARA_CMD) = 0;
				Formatting =0;
				break;
			case 2:
				SD_WriteBlock(Read_Write_Carrier,12288,512);	//出厂参数
				ReadPara(CLEAR_STARS_PARA_CMD) = 0;
				Formatting =0;
				break;
			case 3:
				SD_WriteBlock(Read_Write_Carrier,6144,512);	//用户参数
				SD_WriteBlock(Read_Write_Carrier,12288,512);	//出厂参数
				ReadPara(CLEAR_STARS_PARA_CMD) = 0;
				Formatting =0;
				break;
        }
    }
}


/*
************************************************************
*	函数名称：	EraseErrRecordfromSdcard
*
*	函数功能：	擦除故障数据
*
*	入口参数：	
*
*	返回参数：	
*
*	说明：		
************************************************************
*/
void EraseErrRecordfromSdcard(void)
{
	SdCardErrDataCnt = 0;				//计数器清零
	SDRecordFlag = 1;
// 	RecorddataAddr(RuningRecData);
}


