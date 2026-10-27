#include "stm32f4xx.h"
#include "CabinetTemp.h"
#include "ExternPrototype.h"

u16 Temp_i[6] = {0};
u16 Temp_Cnt[6] = {0};
s16 Temp_Now[6] = {0};
s16 Temp_Old[6] = {0};
s32 Temp_Sum[6] = {0};
u16 Pt100_GAIN[6] = {3300,3300,3300,3300,3300,3300};//温度校准增益

/*******************************************************************************
* 函数名 : 采样折线法求表格间的温度值
* 描  述 : 温度传感器类型：PT100
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
s16	GetCabinetTemp(u8 CabNum)
{
    u16 i;
    s16 j;
    u16 Pt100_AD_Value;

    Pt100_AD_Value = ADC_ConvValue[CabNum];
    Pt100_AD_Value = Pt100_AD_Value >> 4;

    Pt100_GAIN[0] = ReadPara(CABINET_TEMP_ADJ4);	//T4温度校准参数
    Pt100_GAIN[1] = ReadPara(CABINET_TEMP_ADJ2);	//T1温度校准参数
    Pt100_GAIN[2] = ReadPara(CABINET_TEMP_ADJ1);	//T2温度校准参数
    Pt100_GAIN[3] = ReadPara(CABINET_TEMP_ADJ0);	//T3温度校准参数
    Pt100_GAIN[4] = ReadPara(CABINET_TEMP_ADJ5);	//T0温度校准参数
  //Pt100_GAIN[5] = ReadPara(CABINET_TEMP_ADJ3);	//预留

    Pt100_AD_Value = Pt100_AD_Value * Pt100_GAIN[CabNum] / 3300;		//6个PT100温度通道使用6个校准系数

    if (Pt100_AD_Value <= CabinetTempAdTable[0]-10)
    {
		return 2000;     												//PT100短路时，进ARM的电压约为0.00V,AD值远小于CabinetTempAdTable[0]；短路显示200°
    }

	if (Pt100_AD_Value >= (CabinetTempAdTable[40]+10))
    {
        return 2000;													//PT100断线时，进ARM的电压约为3.25V,AD值远大于CabinetTempAdTable[40]；断线显示200°
    }

	if (Pt100_AD_Value >= CabinetTempAdTable[Temp_i[CabNum]])
	{
		if (Temp_i[CabNum] < 40)
		{
			Temp_i[CabNum]++;
		}
		return Temp_Old[CabNum];
	}
	else if (Temp_i[CabNum] > 0)
	{
		Temp_i[CabNum]--;
	}

    if ((Pt100_AD_Value > CabinetTempAdTable[0]) && (Pt100_AD_Value <= CabinetTempAdTable[1]))
    {
        Temp_i[CabNum] = 1;
    }

    j = CabinetTempAdTable[Temp_i[CabNum]-1]-CabinetTempAdTable[Temp_i[CabNum]];  	//折线法求表格间的温度值
    j = ((CabinetTempAdTable[Temp_i[CabNum]-1]-Pt100_AD_Value)*5)/j;
    j += ((Temp_i[CabNum]-1)*5);
	j -= 50;
	
    if (Temp_Cnt[CabNum] < 20)
    {
        Temp_Cnt[CabNum] ++;
        Temp_Sum[CabNum] += j;
    }
    else
    {
        Temp_Cnt[CabNum] = 0;
        Temp_Now[CabNum] = Temp_Sum[CabNum] / 20;
        Temp_Sum[CabNum] = 0;
    }

	Temp_Old[CabNum] = Temp_Now[CabNum]*10;
    return (Temp_Now[CabNum]*10);
}
