#ifndef _SWSDCARD_H_
#define _SWSDCARD_H_
#include "stm32f4xx_conf.h"

//SD卡大小128M，扇区地址定义如下：
#define STARS_USER_PARA_ADDR		  		((u32)6656)												//星辰参数（即用户参数）
#define STARS_DEFAULT_PARA_ADDR	  			((u32)12800)											//星辰备份参数（即出厂参数）	
#define WINDEY_AUTOSTUDY_PARA_ADDR	  		((u32)512)												//运达自学习参数
#define WINDEY_AUTOSTUDY_BACKUP_PARA_ADDR	((u32)3584)												//运达自学习备份参数(暂时不需要)
#define WINDEY_UNLOCK_PARA_ADDR	  			((u32)18944)											//运达激活参数
#define WINDEY_UNLOCK_BACKUP_PARA_ADDR	  	((u32)20480)											//运达激活备份参数(暂时不需要)

#define STARS_USERDEFAULT_PARA_LENGTH        1000													//星辰用户参数的个数（也是出厂参数的个数）
#define WINDEY_AUTOSTUDY_PARA_ADDR_LENGTH    60														//运达自学习参数的个数
#define WINDEY_UNLOCK_PARA_ADDR_LENGTH       60														//运达激活参数的个数

#define Geologic_Record_AndMin				((u32)22528)											//历史故障记录地址存储区起始地址,同时也是地址记录区的最小地址
#define Failure_Data_AndMin					((u32)150016)											//历史故障数据存储区起始地址，同时也是数据记录区的最小地址

#define MaxAddrsAdd							((u32)149504)											//地址记录区的最大地址
#define MaxDataRecordAddr					((u32)4343808)											//故障数据记录区的最大地址
#define SDcardBareAdde						((u32)4349952)											//裸片标志位的地址


//======================函数说明==========================
//	Mode 	0 ：无需保存   
// 			1 ：需要保存星辰用户参数（星辰上位机参数保存、或用户参数扇区已发生错误后将出厂参数数据写入用户参数扇区）
// 			2 : 需要保存星辰用户参数和出厂默认参数（星辰上位机参数保存+Ctrl、或用户扇区和出厂扇区同时发送错误后将程序默认值写入这两个扇区）
// 			3 : 需要保存运达HMI自学习参数
// 			4 : 需要保存运达加密激活参数
// 			5 : 需要保存星辰出厂默认参数（出厂参数扇区已发生错误，将用户参数数据写入出厂参数扇区）

s16 SavePara1(u16 Mode);											//保存参数到ARM外挂的SD卡 
void SaveParaToSdCard(u32 StartAddr);								//保存参数到ARM外挂的128M SD卡		StartAddr:为写数据的起始地址

u16 ReadStarsSdcardPara(u16 Mode);									//Mode  0 读星辰用户参数 1 读星辰出厂参数
u16 ReadWindeySdcardPara(u16 Mode);									//Mode  0 读运达自学习参数 1 读运达加密激活参数

void ReNewValifyData_SdCard(void);

void Sd_CardReErrData_Int(void);									//SD卡记录故障数据初始化
u8 Sd_Judge_Flag(u8 *buf,u8 *buf1);									//标志位判断函数
void WriteErrRecorToSdCard(u16 *writedata,u16 length);				//SD卡记录故障数据
void RecorddataAddr(u32 aidadders);									//记录记录数据的地址

void ReadErrRecordfromSdcard(void);									//读取SD卡故障数据
void EraseErrRecordfromSdcard(void);						//擦除故障数据


#endif

