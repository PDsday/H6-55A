#include "ExternPrototype.h"
#include "includes.h"
#include "sdcard.h"
#include "rtcdata.h"

#ifndef TEST_FOR_STARS				// 未定义星辰厂内测试版
	#include "PLC_PRG.h"
#else
#endif

u32 ChipUniqueID[3];
u16 BuildYear1; 	// 年
u16 BuildMonth1; 	// 月
u16 BuildDay1; 		// 日
u16 BuildDate1; 	// 月日
u16 BuildHour1; 	// 时
u16 BuildMinute1; 	// 分
u16 BuildTime1; 	// 时分
u16 BOR_Flag = 0;

/*******************************************************************************
* 函数名 : GetBuildInfo1
* 描  述 : 获取编译信息
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void GetBuildInfo1(void)
{
    u16 mm,dd,yy;
    u16 hh,mms;
	char *p = __DATE__;		//__DATE__为字符串 格式 "mm dd yyyy" 其中mm为3个英文字符表示 (示例"Jan 14 1997")

    /* 获取编译日期 */
    if(*(p + 4) == ' ') // 日
	{
		dd = *(p + 5) - '0';	//日期只有个位数时十位数为空白字符，只取个位数的有效数字
	}
	else
	{
		dd = 10 * (*(p + 4) - '0') + (*(p + 5) - '0');//取日期转换
	}

    if(strncmp(__DATE__,"Jan",3) == 0) // 月
	{
		mm = 1;
	}
	else if(strncmp(__DATE__,"Feb",3) == 0)
	{
		mm = 2;
	}
	else if(strncmp(__DATE__,"Mar",3) == 0)
	{
		mm = 3;
	}
	else if(strncmp(__DATE__,"Apr",3) == 0)
	{
		mm = 4;
	}
	else if(strncmp(__DATE__,"May",3) == 0)
	{
		mm = 5;
	}
	else if(strncmp(__DATE__,"Jun",3) == 0)
	{
		mm = 6;
	}
	else if(strncmp(__DATE__,"Jul",3) == 0)
	{
		mm = 7;
	}
	else if(strncmp(__DATE__,"Aug",3) == 0)
	{
		mm = 8;
	}
	else if(strncmp(__DATE__,"Sep",3) == 0)
	{
		mm = 9;
	}
	else if(strncmp(__DATE__,"Oct",3) == 0)
	{
		mm = 10;
	}
	else if(strncmp(__DATE__,"Nov",3) == 0)
	{
		mm = 11;
	}
	else if(strncmp(__DATE__,"Dec",3) == 0)
	{
		mm = 12;
	}
	else
	{
		mm = 0;
	}

    yy  = *(p + 7) - '0';			//年份
    yy *= 10;
    yy += *(p + 8) - '0';			//年份
    yy *= 10;
    yy += *(p + 9) - '0';			//年份
    yy *= 10;
	yy += *(p + 10) - '0';			//年份

    p = __TIME__;		//为字符串 格式 "hh:mm:ss" (示例 "13:58:17")

	hh  = 10 * (*(p + 0) - '0') + (*(p + 1) - '0');//取小时转换
	mms = 10 * (*(p + 3) - '0') + (*(p + 4) - '0');//取分钟转换

    BuildYear1 = yy;
    BuildMonth1 = mm;
    BuildDay1 = dd;
    BuildHour1 = hh;
    BuildMinute1 = mms;

    BuildDate1 = mm * 100 + dd;
    BuildTime1 = hh * 100 + mms;
		
		Get_BootVersion();
}

void Get_ChipID(void)
{
	ChipUniqueID[0] = *(__IO u32 *) (0x1FFF7A10);  
	ChipUniqueID[1] = *(__IO u32 *) (0x1FFF7A14);  
	ChipUniqueID[2] = *(__IO u32 *) (0x1FFF7A18);  
}

void DoubleRAM_Data_Monitor(void)
{
	ReadPara(DOUBLERAM_DATA_01) = DoubleRAM.StateWord;
	ReadPara(DOUBLERAM_DATA_02) = DoubleRAM.ChargerState;
	ReadPara(DOUBLERAM_DATA_03) = DoubleRAM.DSP_StateWord;
// 	ReadPara(DOUBLERAM_DATA_04) = DoubleRAM.?;
// 	ReadPara(DOUBLERAM_DATA_05) = DoubleRAM.?;
// 	ReadPara(DOUBLERAM_DATA_06) = DoubleRAM.?;
// 	ReadPara(DOUBLERAM_DATA_07) = DoubleRAM.?;
// 	ReadPara(DOUBLERAM_DATA_08) = DoubleRAM.?;
// 	ReadPara(DOUBLERAM_DATA_09) = DoubleRAM.?;
// 	ReadPara(DOUBLERAM_DATA_10) = DoubleRAM.?;
}

void Set_BOR_Level_3(void)
{
	FLASH_Status status = FLASH_COMPLETE;


	if (BOR_Flag == 0) 
	{	
		if (((ReadPara(CHARG_SYS_ST_CODE) & 0x0100) == 0x0100) && (ReadPara(C_VOL) > 500) && ((ReadPara(SYS_ST_CODE) & 0x0004) == 0))	 //半控已开启、母线电压大于500VDC、驱动器为关使能状态
		{								
			/* 检查当前BOR级别，如果不是LEVEL3则进行配置 */
			if ((FLASH_OB_GetBOR() != OB_BOR_LEVEL3) && (PWR_GetFlagStatus(PWR_FLAG_PVDO) == RESET))
			{
				/* 1. 解锁Flash控制寄存器 */
				FLASH_Unlock();

				/* 2. 清除所有挂起的Flash操作标志位 */
				FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

				/* 3. 解锁选项字节 */
				FLASH_OB_Unlock();

				/* 4. 设置BOR级别为LEVEL3 */
				FLASH_OB_BORConfig(OB_BOR_LEVEL3);

				/* 5. 启动选项字节加载过程 */
				status = FLASH_OB_Launch();

				if(status != FLASH_COMPLETE)
				{
					/* 选项字节编程错误处理 */
					while(1);
				}

				/* 6. 重新锁定选项字节 */
				FLASH_OB_Lock();

				/* 7. 重新锁定Flash控制器 */
				FLASH_Lock();  

				BOR_Flag = 1;
			}
			else if (FLASH_OB_GetBOR() == OB_BOR_LEVEL3) 
			{
				BOR_Flag = 1;
			}
		}
	}
}

/*******************************************************************************
* 函数名 : main
* 描  述 : 主程序
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
int main(void)
{
    SysINit();	 										// 系统初始化
    GetBuildInfo1(); 									// 获取编译日期时间信息
	ReadPara(USER_ARM_BUILD_YEAR) = BuildYear1; 		// 年
	ReadPara(USER_ARM_BUILD_DATE) = BuildDate1; 		// 月日
	ReadPara(USER_ARM_BUILD_TIME) = BuildTime1; 		// 时分

	#ifndef TEST_FOR_STARS								// 未定义星辰厂内测试版
		TaskInit();
		AddUserTask(&UserTask1ms, 1,1); 				// 定义用户任务1：调用周期1ms，优先级1（注意：优先级0已被UserMainTask定义）	
		AddUserTask(&UserTask4ms, 4,2); 				// 定义用户任务2：调用周期4ms，优先级2
		AddUserTask(&UserTask10ms, 10,3); 				// 定义用户任务3：调用周期10ms，优先级3
		AddUserTask(&UserTask1000ms, 1000,4); 			// 定义用户任务4：调用周期1000ms，优先级4
	#else
	#endif

    while (1)
    {
		Time_PollRtc();
		GetErrRecord();
		WriteErrRecord();		
		/*
		if (Rs485ModbusSndEn_Temp == 1)
		{
			ReadErrRecord();					
			Rs485ModbusSndEn_Temp = 2;
		}
		*/
		Sd_Card_Data_Handling();
		DoubleRAM_Data_Monitor();
		Set_BOR_Level_3();
    }
}

