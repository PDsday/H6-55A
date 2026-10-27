#include "stm32f4xx.h"
#include "MotoTemp.h"
#include "ExternPrototype.h"

u16 MotoHotFlag = 0;
u16 MotoOffLineFlag = 0;
u16 MotoOffLineVal = 65535;
u16 MotoHotCnt = 1500;

u16 MotoTemp_Cnt = 0;
s16 MotoTemp_Now = 0;
s32 MotoTemp_Sum = 0;

/*******************************************************************************
* 函数名 : GetCabinetTemp1运放电路,采样折线法求表格间的温度值
* 描  述 : 温度传感器类型：KTY84
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
s16	GetCabinetTemp1(u8 CabNum)
{
    u16 i;
    s16 j;
    u16 AD_Value;

    AD_Value = ADC_ConvValue[CabNum];
    AD_Value = AD_Value >> 4;

    AD_Value = AD_Value * ReadPara(CABINET_TEMP_ADJ3) / 3300;		//T5(DJWD)

    if(AD_Value <= MotoTempAdTable[0])
    {
        return -400;     											//只显示-40°以上温度
    }

    if(AD_Value >= MotoTempAdTable[48])
    {
        return 2000;												//断线显示200°(最高检测温度为200°)
    }

    for(i=1; i<49; i++) //查表
    {
        if(AD_Value>MotoTempAdTable[i])
        {
            break;
        }
    }

    /*
    现象描述：
    假设Pt100_AD_Value = 1930（对应实际温度约为-39°），而CabinetTempAdTable[0] = 1920，CabinetTempAdTable[1] = 1967，
    由于for循环从i=1开始，进入循环后，一直不满足条件：if(Pt100_AD_Value>CabinetTempAdTable[i]),i就会一直递增到38，
    最终计算得出的温度是150°左右，而Pt100_AD_Value = 1930，理论上应该是-39°左右的温度，导致温度显示错乱！

    解决办法：
    1、参考DSP的IGBT温度检测处理，CabinetTempAdTable采用降序的方式，并且在j-=40语句之前增加j = 150 - j语句处理即可。
    2、增加以下语句处理，避开CabinetTempAdTable[0]和CabinetTempAdTable[1])直接的AD值即可：
    */
    if((AD_Value > MotoTempAdTable[0]) && (AD_Value <= MotoTempAdTable[1]))
    {
        i = 1;
    }

    j=MotoTempAdTable[i-1]-MotoTempAdTable[i];  				// 折线法求表格间的温度值
    j=((MotoTempAdTable[i-1]-AD_Value)*5)/j;
    j+=((i-1)*5);
    j-=40;

    if(MotoTemp_Cnt < 512)
    {
        MotoTemp_Cnt ++;
        MotoTemp_Sum += j;
    }
    else
    {
        MotoTemp_Cnt = 0;
        MotoTemp_Now = MotoTemp_Sum / 512;
        MotoTemp_Sum = 0;
    }

    //return (j*10);			//0.1°精度
    return (MotoTemp_Now*10);	//0.1°精度
}

s16	GetMotoTemp(u16 AdcVal)
{
    static s16 Temp;

    Temp = GetCabinetTemp1(AdcVal);					   	   		// T5(DJWD) 温度传感器类型：KTY84
    //Temp = GetCabinetTemp(AdcVal);					   		// T5(DJWD) 温度传感器类型：PT100

    //if ((Temp/10 > ReadPara(MOTO_SAFE_TEMP)) && (Temp/10 < 200))//大于130°，且小于200°，才报过热；断线时为350°，报断线，不应报过热
    if (Temp/10 > ReadPara(MOTO_SAFE_TEMP))						//大于130°
    {
        MotoHotFlag = 1;                                   		// 置电机过热保护标志
        MotoOffLineFlag = 0;
    }
    else if (Temp/10 < (ReadPara(MOTO_SAFE_TEMP) - 5))
    {
        MotoHotFlag = 0;                                   		// 取消电机过热保护标志
        MotoOffLineFlag = 0;
    }
// 	else if (Temp/10 >= 201)
// 	{
// 		MotoOffLineFlag = 1;                               		// 置电机温度断线标志
// 		MotoHotFlag = 0;                                   		// 断线时不用报电机过热
// 	}

    return(Temp);
}

u16 IsMotoHot(void)     										// 置电机过热标志
{
    return (MotoHotFlag);
}

u16 IsMotoOffLine(void)  										// 置电机温度断线标志
{
    return (MotoOffLineFlag);
}