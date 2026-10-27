#include "PARAPROCESS.H"
#include "ExternPrototype.h"
#include "InterUart.H"
#include "sdio_sdcard.h"
#include "sdcard.h"
#include "AppFunc.h"

#ifndef TEST_FOR_STARS							// 未定义星辰厂内测试版
	#include "AppInputOutPut.h"
	#include "GlobalVar_POUS.h"
#else
#endif

#define  MOTOR_ID_NUM	  10    						//电机参数ID数目
#define  MOTOR_DA_NUM	  5    							//电机参数DATA数目

extern u8 err_rst1;
extern u16 CrcPassFlag;
extern u16 BuildYear;
extern u16 BuildMonth;
extern u16 BuildDay;
extern u16 BuildDate;
extern u16 BuildHour;
extern u16 BuildMinute;
extern u16 BuildTime;
extern u16 SSI_Type;
extern u16 ErrRecordDataArray[GZM_InfoNum * GZM_Num]; 	//故障代码记录数组
extern u16 ReturnSelect;								//0 返回ARM数据 1 返回DSP数据
extern u16 PendParaID;									//需要DSP返的帧ID
extern void GetBuildInfo(void);
extern union DEFECT_FLAGL DefectFlagInL;				//故障输入
extern union DEFECT_FLAGH DefectFlagInH;				//故障输入
extern union DEFECT_FLAGL DefectFlagOutL;				//故障输出
extern union DEFECT_FLAGH DefectFlagOutH;				//故障输出
extern u32 ChipUniqueID[];
extern u16 Func_0x03_Data[];
extern u16 Func_0x06_Data[];
extern u16 Func_0x10_Data[];
extern u32 SysParaBuffBack[];
extern u32 EncryptParaBuffBack[];
extern u8 DataDiffFlag;
extern u16 MotorType_Old;
extern u16 FlashValifyErr;
extern u16 UserParaValifyData[2];
extern u16 DeftParaValifyData[2];

u64 SafeRunEn = 0;										//安全收桨
u64 StopRunEn = 0;										//停机抱闸
u64 NormRunEn = 0;										//正常运行
u64 SNstopRunEn = 0;									//停机抱闸
u64 SafeOutFlag = 0 ;									//开环收桨叶
s32 MotorPowerOffset = 0;								//数据
u8 MotorPowerOffsetCRC[6] = {0};						//校验
u16 MotorPowerOffsetH = 0;								//字节0、1
u16 MotorPowerOffsetL = 0;								//字节2、3
u16 MotorPowerOffsetC = 0;								//字节4、5
u16	RefreshFlag=0;										//数据刷新标志	1 需要刷新 0 不需要刷新
u16	UpDateFlag=0;										//参数更改标志	0 参数未更改 1 参数已更改
u16	UpDateParaID=0;										//待更改参数ID
u8 DownLoadToDsp=0;										//0=无需下传到DSP的ID；1=需下传到DSP的ID

void	CalculPara(u16 ParaID);
void	CalculPara1(void);

struct  SysPara  SysParaTable[SYS_PARA_LEN_MAX];
struct  SysPara  SysParaTable2[PWR_PARA_LEN];

DefectConfigType DefectConfigParaData[44]=
{
	/*
	00=故障位
	01=故障启用禁止：0禁止；1启用
	02=故障处理方式：0或1抱闸停机；2安全收桨；3忽略故障；4开环收桨
	03=故障置位延时
	04=故障复位延时（复位方式为自动时有效）
	05=故障复位方式：0手动复位；1自动复位
	06=故障触发条件：16381=高电平触发；16382=低电平触发；16383=上升沿触发；16384=下降沿触发；其它直接对应阀值，如电机过热触发条件：140°
	07=故障是否断安全链：0不断；1断开
	08=定时器的置1定时计数
	09=定时器的置0定时计数
	10=第一次判断后故障状态
	11=返回状态
	*/
	
	//	0		1		2		3		4		5		6		7		8		9		10		11
    {	0	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	1	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	2	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	3	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	4	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	5	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	6	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	7	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	8	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	9	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	10	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	11	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	12	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	13	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	14	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	15	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,

    {	16	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	17	,	1	,	0	,	100	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	18	,	1	,	0	,	100	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	19	,	1	,	0	,	100	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	20	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	21	,	1	,	0	,	100	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	22	,	1	,	0	,	100	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	23	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	24	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	25	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	26	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	27	,	1	,	0	,	100	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	28	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	29	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	30	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	31	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,

    {	32	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	33	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	34	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	35	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	36	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	37	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	38	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	39	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	40	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	41	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	42	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
    {	43	,	1	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
};

DefectConfigType DefectConfigParaID[44]=
{
    {	0	,	430	,	431	,	432	,	433	,	434	,	435	,	436	,	0	,	0	,	0	,	0	}	,
    {	1	,	437	,	438	,	439	,	440	,	441	,	442	,	443	,	0	,	0	,	0	,	0	}	,
    {	2	,	444	,	445	,	446	,	447	,	448	,	449	,	450	,	0	,	0	,	0	,	0	}	,
    {	3	,	451	,	452	,	453	,	454	,	455	,	456	,	457	,	0	,	0	,	0	,	0	}	,
    {	4	,	458	,	459	,	460	,	461	,	462	,	463	,	464	,	0	,	0	,	0	,	0	}	,
    {	5	,	465	,	466	,	467	,	468	,	469	,	470	,	471	,	0	,	0	,	0	,	0	}	,
    {	6	,	472	,	473	,	474	,	475	,	476	,	477	,	478	,	0	,	0	,	0	,	0	}	,
    {	7	,	479	,	480	,	481	,	482	,	483	,	484	,	485	,	0	,	0	,	0	,	0	}	,
    {	8	,	486	,	487	,	488	,	489	,	490	,	491	,	492	,	0	,	0	,	0	,	0	}	,
    {	9	,	493	,	494	,	495	,	496	,	497	,	498	,	499	,	0	,	0	,	0	,	0	}	,
    {	10	,	500	,	501	,	502	,	503	,	504	,	505	,	506	,	0	,	0	,	0	,	0	}	,
    {	11	,	507	,	508	,	509	,	510	,	511	,	512	,	513	,	0	,	0	,	0	,	0	}	,
    {	12	,	514	,	515	,	516	,	517	,	518	,	519	,	520	,	0	,	0	,	0	,	0	}	,
    {	13	,	521	,	522	,	523	,	524	,	525	,	526	,	527	,	0	,	0	,	0	,	0	}	,
    {	14	,	528	,	529	,	530	,	531	,	532	,	533	,	534	,	0	,	0	,	0	,	0	}	,
    {	15	,	535	,	536	,	537	,	538	,	539	,	540	,	541	,	0	,	0	,	0	,	0	}	,

    {	16	,	542	,	543	,	544	,	545	,	546	,	547	,	548	,	0	,	0	,	0	,	0	}	,
    {	17	,	549	,	550	,	551	,	552	,	553	,	554	,	555	,	0	,	0	,	0	,	0	}	,
    {	18	,	556	,	557	,	558	,	559	,	560	,	561	,	562	,	0	,	0	,	0	,	0	}	,
    {	19	,	563	,	564	,	565	,	566	,	567	,	568	,	569	,	0	,	0	,	0	,	0	}	,
    {	20	,	570	,	571	,	572	,	573	,	574	,	575	,	576	,	0	,	0	,	0	,	0	}	,
    {	21	,	577	,	578	,	579	,	580	,	581	,	582	,	583	,	0	,	0	,	0	,	0	}	,
    {	22	,	584	,	585	,	586	,	587	,	588	,	589	,	590	,	0	,	0	,	0	,	0	}	,
    {	23	,	591	,	592	,	593	,	594	,	595	,	596	,	597	,	0	,	0	,	0	,	0	}	,
    {	24	,	598	,	599	,	600	,	601	,	602	,	603	,	604	,	0	,	0	,	0	,	0	}	,
    {	25	,	605	,	606	,	607	,	608	,	609	,	610	,	611	,	0	,	0	,	0	,	0	}	,
    {	26	,	612	,	613	,	614	,	615	,	616	,	617	,	618	,	0	,	0	,	0	,	0	}	,
    {	27	,	619	,	620	,	621	,	622	,	623	,	624	,	625	,	0	,	0	,	0	,	0	}	,
    {	28	,	626	,	627	,	628	,	629	,	630	,	631	,	632	,	0	,	0	,	0	,	0	}	,
    {	29	,	633	,	634	,	635	,	636	,	637	,	638	,	639	,	0	,	0	,	0	,	0	}	,
    {	30	,	640	,	641	,	642	,	643	,	644	,	645	,	646	,	0	,	0	,	0	,	0	}	,
    {	31	,	647	,	648	,	649	,	650	,	651	,	652	,	653	,	0	,	0	,	0	,	0	}	,

    {	32	,	654	,	655	,	656	,	657	,	658	,	659	,	660	,	0	,	0	,	0	,	0	}	,
    {	33	,	661	,	662	,	663	,	664	,	665	,	666	,	667	,	0	,	0	,	0	,	0	}	,
    {	34	,	668	,	669	,	670	,	671	,	672	,	673	,	674	,	0	,	0	,	0	,	0	}	,
    {	35	,	675	,	676	,	677	,	678	,	679	,	680	,	681	,	0	,	0	,	0	,	0	}	,
    {	36	,	682	,	683	,	684	,	685	,	686	,	687	,	688	,	0	,	0	,	0	,	0	}	,
    {	37	,	689	,	690	,	691	,	692	,	693	,	694	,	695	,	0	,	0	,	0	,	0	}	,
    {	38	,	696	,	697	,	698	,	699	,	700	,	701	,	702	,	0	,	0	,	0	,	0	}	,
    {	39	,	703	,	704	,	705	,	706	,	707	,	708	,	709	,	0	,	0	,	0	,	0	}	,
    {	40	,	710	,	711	,	712	,	713	,	714	,	715	,	716	,	0	,	0	,	0	,	0	}	,
    {	41	,	717	,	718	,	719	,	720	,	721	,	722	,	723	,	0	,	0	,	0	,	0	}	,
    {	42	,	724	,	725	,	726	,	727	,	728	,	729	,	730	,	0	,	0	,	0	,	0	}	,
    {	43	,	731	,	732	,	733	,	734	,	735	,	736	,	737	,	0	,	0	,	0	,	0	}	,
};

void DefectProcess(u64 in,DefectConfigType *DefectTable)
{
    u8 i = 0;
    static u64 inold = 0;

    for (i = 16; i < 32; i++)//for(i=0; i<32; i++)//DSP、充电器故障不在此处理
    {
        if (i < 32)
        {
            if (ReadPara((DefectTable+i)->EnID)==1)//如果是允许故障
            {
				if (((u64)1<<(DefectTable+i)->DefectBit&in)!= 0)
				{
					(DefectTable+i)->DefectStateTemp = 1;
				}
				else
				{
					(DefectTable+i)->DefectStateTemp = 0;
				}

                if ((DefectTable+i)->DefectStateTemp != 0)//如果是确定是有故障的位
                {
                    (DefectTable+i)->CountRST = 0;
                    if ((DefectTable+i)->CountEN <= ReadPara((DefectTable+i)->EnDlyID))			//进入if分支，就是消抖时间未到，则return sta就是调用函数时的值
                    {
                        (DefectTable+i)->CountEN++;
                    }
                    else							//进入else分支就更新sta
                    {
                        (DefectTable+i)->CountEN = 0;
                        (DefectTable+i)->DefectState= 1;
                    }
                }
                else       //低电平
                {
                    (DefectTable+i)->CountEN = 0;
                    if (ReadPara((DefectTable+i)->RstTypID)==1)//如果是自动复位时就执行复位延时否则就马上清除故障
                    {
                        if ((DefectTable+i)->CountRST <= ReadPara((DefectTable+i)->RstDlyID))
                        {
                            (DefectTable+i)->CountRST ++;
                        }
                        else
                        {
                            (DefectTable+i)->CountRST = 0;
                            (DefectTable+i)->DefectState = 0;
                        }
                    }
                    else//手动复位
                    {
                        if ((ReadPara(ERR_RST) != 0)||(err_rst1==1))
                        {
                            StmStatus.bit.DefectCodeClear = 1; // 1清除故障告警
                            DefectCodeClear = 1 ;
                            (DefectTable+i)->CountRST = 0;
                        }
                        else
                        {
                            DefectCodeClear = 0 ;
                            StmStatus.bit.DefectCodeClear = 0; // 锁住故障告警
                        }
                        (DefectTable+i)->DefectState = 0;//发送故障到DSP中
                    }
                }
            }
            else//禁止故障
            {
                (DefectTable+i)->CountEN = 0;
                (DefectTable+i)->CountRST = 0;
                (DefectTable+i)->DefectState = 0;
            }
        }
    }

    inold = in;
}

void DefectParaProcess(DefectConfigType *DefectTable)
{
    u8 i = 0;

    for (i=0; i<44; i++)
    {
        if(ReadPara(DefectConfigParaID[i].DefModeID)==0)			//抱闸停机
        {
            SNstopRunEn |= (u64)1<<(DefectTable+i)->DefectBit;
            NormRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            StopRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            SafeRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
        }
        else if(ReadPara(DefectConfigParaID[i].DefModeID)==1)		//减速停机
        {
            StopRunEn |= (u64)1<<(DefectTable+i)->DefectBit;
            SNstopRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            SafeRunEn &=~((u64)1<<(DefectTable+i)->DefectBit);
            NormRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
        }
        else if(ReadPara(DefectConfigParaID[i].DefModeID)==2)		//安全收桨
        {
            SafeRunEn |=(u64)1<<(DefectTable+i)->DefectBit;
            SNstopRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            NormRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            StopRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
        }
        else if(ReadPara(DefectConfigParaID[i].DefModeID)==3)		//忽略故障
        {
            NormRunEn |= (u64)1<<(DefectTable+i)->DefectBit;
            SNstopRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            StopRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            SafeRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
        }
        else
        {
            SNstopRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            NormRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            StopRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
            SafeRunEn &= ~((u64)1<<(DefectTable+i)->DefectBit);
        }

        if(ReadPara(DefectConfigParaID[i].SafeID)==1)
        {
            SafeOutFlag |= (u64)1<<((DefectTable+i)->DefectBit);
        }
        else
        {
            SafeOutFlag &= ~((u64)1<<((DefectTable+i)->DefectBit));
        }
    }
}

void DefectParaUp(u16 data, DefectConfigType *DefectTable) //返回
{
    u8 i = 0;
    u8 add  = 0;

    for(i=0; i<16; i++)
    {
        if(data==ALARM_MASK)
        {
            add = 0;
            ReadPara(DefectConfigParaID[add+i].EnID)=1&&(~ReadPara(ALARM_MASK)&((u16)1<<i));
        }
        else if (data == ALARM_AUTO_RST_EN)
        {
            add = 0;
            ReadPara(DefectConfigParaID[add+i].RstTypID)=1&&(ReadPara(ALARM_AUTO_RST_EN)&((u16)1<<i));
        }
        else if(data==ALARM_MASKH)
        {
            add = 16;
            ReadPara(DefectConfigParaID[add+i].EnID)=1&&(~ReadPara(ALARM_MASKH)&((u16)1<<i));
        }
        else if(data==ALARM_AUTO_RST_ENH)
        {
            add = 16;
            ReadPara(DefectConfigParaID[add+i].RstTypID)=1&&(ReadPara(ALARM_AUTO_RST_ENH)&((u16)1<<i));
        }
        else if(data==CHARG_ALARM_AUTO_RST_EN)
        {
            add = 32;
            if((add+i)<44)
            {
                ReadPara(DefectConfigParaID[add+i].RstTypID)=1&&( ReadPara(CHARG_ALARM_AUTO_RST_EN)&((u16)1<<i));
            }
        }
        else if(data==CHARG_ALARM_EN)
        {
            add = 32;
            if((add+i)<44)
            {
                ReadPara(DefectConfigParaID[add+i].EnID) = (1&&(~ReadPara(CHARG_ALARM_EN)&((u16)1<<i)));
            }
        }
    }
}

u16	ParaProcess(u16 ProcessCode,u16 ParaID,u16 NewPara)
{
    u16 ReSult;
    union ATTR Attr;

    if (ParaID >= SYS_PARA_LEN_STARS) // 参数ID超出范围
    {
        //if ((ParaID >= GZM_StartID) && (ParaID <= GZM_EndID))
        if ((ParaID >= GZM_StartID) && (ParaID <= (SYS_PARA_LEN_MAX - 1)))
        {
            ReSult=SysParaTable[ParaID].DATA;
        }
        else
        {
            ReSult = 0;
        }

        return (ReSult);
    }

    if (ParaID == COMM_ADDR) // 通讯波特率不允许更改
    {
        return (SysParaTable[ParaID].DATA);
    }

    ReSult=SysParaTable[ParaID].DATA;	//读参数或更改参数不成功均返回当前实际参数值

    if(ProcessCode)
    {
        Attr.all=SysParaTable[ParaID].ATTR.all;
        if(Attr.bit.WREN || AdjModFlag)
        {
            if(Attr.bit.RUNWREN || ((ReadPara(SYS_ST_CODE) & (1 << 2)) == 0))
            {
                UpDateParaID=ParaID;	//待更改参数ID

                if(Attr.bit.SIGN)//有符号数
                {
                    if(((s16)NewPara>=(s16)SysParaTable[ParaID].LOWLIMIT) && ((s16)NewPara<=(s16)SysParaTable[ParaID].UPLIMIT))
                    {   //有符号数参数未超限
                        SysParaTable[ParaID].DATA=NewPara;
                        ReSult=NewPara;
                        UpDateFlag=1;		//参数有更改，待慢速层中进行后续计算
                    }
                    else
                    {
                        ReSult=SysParaTable[ParaID].DATA;
                    }
                }
                else//无符号数
                {
                    if(NewPara>=SysParaTable[ParaID].LOWLIMIT&&NewPara<=SysParaTable[ParaID].UPLIMIT)//无符号数参数未超限
                    {
                        SysParaTable[ParaID].DATA=NewPara;
                        ReSult=NewPara;
                        UpDateFlag=1;		//参数有更改，待慢速层中进行后续计算
                    }
                    else
                    {
                        ReSult=SysParaTable[ParaID].DATA;
                    }
                }
            }
        }

        if (AdjModFlag) // 调试模式下不受属性表限制
        {
            SysParaTable[ParaID].DATA=NewPara;
            ReSult=NewPara;
            UpDateFlag=1;		//参数有更改，待慢速层中进行后续计算
        }
    }
    return	(ReSult);
}

void Read_Para_From_ROM(void)
{
    u16 i = 0;
    u16 j = 0;

	{
		u32 Delay = 0;
		u32 PVDLevel_Cnt = 0;
		
		while (PVDLevel_Cnt < 277)
		{	
			if ((PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET))
			{
				PVDLevel_Cnt = 0;
			}
			else
			{
				PVDLevel_Cnt ++;
			}
			
			Delay = (long long)Delay * 20 * 22 / 8 / 13 / 19;
		}
	}
	
	if (ReadStarsSdcardPara(0) == 0)					// 0 读SD卡星辰用户参数失败
	{
		for (i=SYS_PARA_AUTOSTUDY; i<SYS_PARA_LEN_MAX; i++)
		{
			SysParaTable[i].DATA = 0;      				// 全部初始化为0
		}
			
		if (ReadStarsSdcardPara(1) == 0)				// 1 读SD卡星辰出厂参数失败
		{
			for (i=0; i<SYS_PARA_LEN_STARS; i++)
			{
				SysParaTable[i].DATA = 0;      			// 全部初始化为0
			}
			
			for (i=SYS_PARA_AUTOSTUDY; i<SYS_PARA_LEN_MAX; i++)
			{
				SysParaTable[i].DATA = 0;      			// 全部初始化为0
			}

			for (i=0; i < DefaultParaTableNum; i++)
			{
				SysParaTable[DefaultParaTable[i].ParaID].DATA = DefaultParaTable[i].DATA; // 读默认参数
			}

			for (i=0; i<44; i++)
			{
				SysParaTable[DefectConfigParaID[i].EnID].DATA =DefectConfigParaData[i].EnID;
				SysParaTable[DefectConfigParaID[i].DefModeID].DATA =DefectConfigParaData[i].DefModeID;
				SysParaTable[DefectConfigParaID[i].EnDlyID].DATA =DefectConfigParaData[i].EnDlyID;
				SysParaTable[DefectConfigParaID[i].RstDlyID].DATA =DefectConfigParaData[i].RstDlyID;
				SysParaTable[DefectConfigParaID[i].EnTrigID].DATA =DefectConfigParaData[i].EnTrigID;
				SysParaTable[DefectConfigParaID[i].SafeID].DATA =DefectConfigParaData[i].SafeID;
				SysParaTable[DefectConfigParaID[i].RstTypID].DATA =DefectConfigParaData[i].RstTypID;
			}
			
			ReadPara(PARAVALIFYERR) = FlashValifyErr;
			ReadPara(USERPARAVALIFYDATA0) = UserParaValifyData[0];
			ReadPara(USERPARAVALIFYDATA1) = UserParaValifyData[1];
			ReadPara(DEFTPARAVALIFYDATA0) = DeftParaValifyData[0];
			ReadPara(DEFTPARAVALIFYDATA1) = DeftParaValifyData[1];
		}
	}
	else
	{
		ReadWindeySdcardPara(0);						// 0 读SD卡运达自学习参数
		ReadWindeySdcardPara(1);						// 1 读SD卡运达加密激活参数
	}
}

void ParaInit(void)	//内部参数初始化
{
    u16 i = 0;
    u16 j = 0;

    /* 读默认参数初始化 */
    for(i=0; i<SYS_PARA_LEN_STARS; i++)
    {
        SysParaTable[i].DATA = 0;      		// 全部初始化为0
        SysParaTable[i].ATTR.all = 0x0003;  // 全部初始化为可更改，无符号
        SysParaTable[i].UPLIMIT = 65535;   	// 全部初始化为65535
        SysParaTable[i].LOWLIMIT = 0;      	// 全部初始化为0
    }
    for(i=SYS_PARA_AUTOSTUDY; i<SYS_PARA_LEN_MAX; i++)
    {
        SysParaTable[i].DATA = 0;      		// 全部初始化为0
        SysParaTable[i].ATTR.all = 0x0003;  // 全部初始化为可更改，无符号
        SysParaTable[i].UPLIMIT = 65535;   	// 全部初始化为65535
        SysParaTable[i].LOWLIMIT = 0;      	// 全部初始化为0
    }
    for(i=0; i < DefaultParaTableNum; i++)
    {
        SysParaTable[DefaultParaTable[i].ParaID].DATA = DefaultParaTable[i].DATA; 			// 读默认参数值
        SysParaTable[DefaultParaTable[i].ParaID].ATTR.all = DefaultParaTable[i].ATTR.all;	// 读默认参数属性
        SysParaTable[DefaultParaTable[i].ParaID].UPLIMIT = DefaultParaTable[i].UPLIMIT; 	// 读默认参数上限
        SysParaTable[DefaultParaTable[i].ParaID].LOWLIMIT = DefaultParaTable[i].LOWLIMIT; 	// 读默认参数下限
    }
    for(i=0; i<44; i++)//初始化35个故障
    {
        SysParaTable[DefectConfigParaID[i].EnID].DATA =DefectConfigParaData[i].EnID;
        SysParaTable[DefectConfigParaID[i].DefModeID].DATA =DefectConfigParaData[i].DefModeID;
        SysParaTable[DefectConfigParaID[i].EnDlyID].DATA =DefectConfigParaData[i].EnDlyID;
        SysParaTable[DefectConfigParaID[i].RstDlyID].DATA =DefectConfigParaData[i].RstDlyID;
        SysParaTable[DefectConfigParaID[i].EnTrigID].DATA =DefectConfigParaData[i].EnTrigID;
        SysParaTable[DefectConfigParaID[i].SafeID].DATA =DefectConfigParaData[i].SafeID;
        SysParaTable[DefectConfigParaID[i].RstTypID].DATA =DefectConfigParaData[i].RstTypID;
    }

    Read_Para_From_ROM();

    Get_ChipID();
    GetBuildInfo(); 							// 获取编译日期时间信息
    ReadPara(ARM_BUILD_YEAR) = BuildYear; 		// 年
    ReadPara(ARM_BUILD_DATE) = BuildDate; 		// 月日
    ReadPara(ARM_BUILD_TIME) = BuildTime; 		// 时分

    WritePara(CO_NODE_ID, CANAddr); 			// CAN地址写入参数
	ChangeCommBDR(9);
	
    if (CANAddr == 0)
    {
        WritePara(CAN_SEL, 0);
    }
    else
    {
        WritePara(CAN_SEL, 1);
    }

    DefectFlagInL.all=0;
    DefectFlagInH.all=0;
    DefectFlagOutL.all=0;
    DefectFlagOutH.all=0;
	
	ReadPara(551) = 100;
	ReadPara(558) = 100;
	ReadPara(565) = 100;
	ReadPara(579) = 100;
	ReadPara(586) = 5000;
	ReadPara(621) = 100;

    ReadPara(MOTO_ANGLE_SET) = 65535;
    ReadPara(MOTOR_POS_RST)  = 0;
	ReadPara(MOTO_TEMP)  = 0;
	ReadPara(C_VOL)  = 0;
	ReadPara(IGBT_TEMP)  = 0;
	ReadPara(V_FEEDBACK)  = 0;
	ReadPara(I_FEEDBACK)  = 0;
	ReadPara(OVER_I_TOTAL)  = 0;
	ReadPara(DEFECT_CODE) = 0;
	ReadPara(DEFECT_CODET) = 0;
	ReadPara(DEFECT_CODEH) = 0;
	ReadPara(DEFECT_CODEM) = 0;	
    ReadPara(SYS_ST_CODE) = 0;
    ReadPara(FLASHSAVEFLAG) = 0;
    ReadPara(ARM_DSP_COMMPARA0) = 0;
    ReadPara(ERRSAVE_ENABLE) = 0;
    ReadPara(CONFIGWORDS) = 0;
	ReadPara(CLEAR_STARS_PARA_CMD) = 0;
	ReadPara(ERR_RST) = 0;
	ReadPara(RESOLVER_ERR_RESET) = 0;
	ReadPara(MOTOOVERLOADDISP) = 0;
	ReadPara(DI_STA) = 0xFFFF;
	ReadPara(DIH_STA) = 0xFFFF;
	ReadPara(ACTUALTORQUE) = 0;
	
    ReadPara(CHARG_AC_VOL) 			= 0;		//电网电压值
    ReadPara(CHARG_DEFECT_CODE)		= 0; 		//告警码
    ReadPara(CHARG_SYS_ST_CODE) 	= 0; 		//系统状态
    ReadPara(CHARG_IGBT_TEMP)   	= 0;   		//IGBT温度
    ReadPara(CHARG_VOL)         	= 0;        //母线电压
    ReadPara(CHARG_V_FEEDBACK)  	= 0;  		//电容电压
    ReadPara(CHARG_I_FEEDBACK)  	= 0;  		//充电电流
    ReadPara(CHARG_DC24V_TEMP)  	= 0;  		//充电器DC24V电源温度
    ReadPara(CHARG_INTERSPACE_TEMP) = 0;		//充电器内腔温度
	ReadPara(CHARG_CTRL_REG) 		= 0;		//充电器控制寄存器

    ReadPara2(CP_DEFECT_CODE)		= 0; 		//告警码
    ReadPara2(CP_SYS_ST_CODE)		= 0; 		//系统状态
    ReadPara2(CP_IGBT_TEMP)			= 0;   		//IGBT温度
    ReadPara2(CP_VOL)				= 0;        //母线电压
    ReadPara2(CP_V_FEEDBACK)		= 0;  		//电容电压
    ReadPara2(CP_I_FEEDBACK)		= 0;  		//充电电流
    ReadPara2(CP_DC24V_TEMP)		= 0;  		//充电器DC24V电源温度
    ReadPara2(CP_INTERSPACE_TEMP)	= 0;		//充电器内腔温度
    ReadPara2(CP_AC_LOWVOL) 		= 100;  	//电网欠压值
    ReadPara2(CP_AC_LOWVOL_BACK) 	= 40;  		//电网欠压回差值
    ReadPara2(CP_AC_OVERVOL) 		= 540;  	//电网过压值
    ReadPara2(CP_AC_OVERVOL_BACK) 	= 40;  		//电网过压回差值
    ReadPara2(CP_ARM_BUILD_YEAR) 	= 0;  		//充电器ARM编译年
    ReadPara2(CP_ARM_BUILD_DATE) 	= 0;  		//充电器ARM编译月日
	ReadPara2(CP_ARM_BUILD_TIME) 	= 0;  		//充电器ARM编译时分
	
	ReadPara(GYRO_SPARE1) = 0;
	ReadPara(GYRO_SPARE2) = 0;
	ReadPara(GYRO_SPARE3) = 0;
	
	#ifndef TEST_FOR_STARS
		for (i = 0; i < 30; i=i+2)
		{
			SysParaBuffBack[j] = (u32)(SysParaTable[i+SYS_PARA_AUTOSTUDY].DATA << 16) | SysParaTable[i+SYS_PARA_AUTOSTUDY+1].DATA;
			j ++;
		}

		j = 0;
		for (i = 0; i < 30; i=i+2)
		{
			EncryptParaBuffBack[j] = (u32)(SysParaTable[i+1360].DATA << 16) | SysParaTable[i+1360+1].DATA;
			j ++;
		}

		G_Device_Serial_Num = ChipUniqueID[2] + ChipUniqueID[1] + ChipUniqueID[0];		//获取芯片唯一ID
		
		ReadPara(ARM_RESERVE_PARA_864) = G_Device_Serial_Num >> 16;
		ReadPara(ARM_RESERVE_PARA_865) = G_Device_Serial_Num >> 0;						
		
		ReadPara(MOTO_TYPE) = SysParaBuffBack[8];										//从SD卡中的自学习参数区读出电机类型数据

		//Control_unit_Version = 221024;
		//Control_unit_Version = (BuildYear % 100) * 10000 + (BuildMonth * 100) + BuildDay;	//编译工程时的电脑系统时间，如221013代表22年10月13日
		
	#else
		//ReadPara(MOTO_TYPE) =  (u32)(SysParaTable[16+SYS_PARA_AUTOSTUDY].DATA << 16) | SysParaTable[16+SYS_PARA_AUTOSTUDY+1].DATA;
	#endif

    DoubleRAM.SelcetMotorType = ReadPara(MOTO_TYPE);
    Func_0x06_Data[36] = ReadPara(MOTO_TYPE);
    MotorType_Old = ReadPara(MOTO_TYPE);
}

void CalculPara(u16 ParaID)
{
    static u16 temp[5] = {0,0,0,0,0};
    static u16 tempold[5] = {0,0,0,0,0};
    static u16 temp1[5] = {0,0,0,0,0};
    static u16 tempold1[5] = {0,0,0,0,0};

    switch(ParaID)
    {
		case COMM_ADDR:
		{
			ChangeCommAddr(ReadPara(COMM_ADDR));
			break;
		}
		case COMM_BDR:
		{
			if (CrcPassFlag == 666)
			{
				ChangeCommBDR(ReadPara(COMM_BDR));
			}
			break;
		}
		case MOD_CTRL:
		{
			if((ReadPara(MOD_CTRL)&0xf0)==0x40)
			{
				WritePara(WORK_MOD, 3);			//变频器模式时强制系统在力矩环下工作
			}
			break;
		}
		case CHARG_V_SET:
		{
			WriteChargPara((CP_V_SET | 0x8000), ReadPara(CHARG_V_SET));
			break;
		}
		case CHARG_I_SET:
		{
			WriteChargPara((CP_I_SET | 0x8000), ReadPara(CHARG_I_SET));
			break;
		}
		case ERR_RST:
		{
			WriteChargPara((CP_ERR_RST | 0x8000), ReadPara(ERR_RST));
			break;
		}
		case CHARG_ALARM_AUTO_RST_EN:
		{
			WriteChargPara((CP_ALARM_AUTO_RST_EN | 0x8000), ReadPara(CHARG_ALARM_AUTO_RST_EN));
			break;
		}
		case CHARG_ALARM_EN:
		{
			WriteChargPara((CP_ALARM_EN | 0x8000), ReadPara(CHARG_ALARM_EN));
			break;
		}
		default:
		{
			break;
		}
    }

    UpDateFlag=0;		//参数已更改完毕
}

void CalculPara1 (void)
{
	if (CrcPassFlag == 666)
	{
		ChangeCommBDR(ReadPara(COMM_BDR));
	}
	
	if((ReadPara(MOD_CTRL)&0xf0)==0x40)
	{
		WritePara(WORK_MOD, 3);			//变频器模式时强制系统在力矩环下工作
	}

	ChangeCommAddr(ReadPara(COMM_ADDR));
	WriteChargPara((CP_V_SET | 0x8000), ReadPara(CHARG_V_SET));
	WriteChargPara((CP_I_SET | 0x8000), ReadPara(CHARG_I_SET));
	WriteChargPara((CP_ERR_RST | 0x8000), ReadPara(ERR_RST));
	WriteChargPara((CP_ALARM_AUTO_RST_EN | 0x8000), ReadPara(CHARG_ALARM_AUTO_RST_EN));
	WriteChargPara((CP_ALARM_EN | 0x8000), ReadPara(CHARG_ALARM_EN));
}

void DownLoadToDsp_Judge (u16 PARAID)			
{
	//union	EXUART_COMMAND Command;
	
	switch (PARAID)
	{
		case	DEFECT_CODE	:{	DownLoadToDsp = 0;	break;}
		case	SYS_ST_CODE	:{	DownLoadToDsp = 0;	break;}
		case	V_SET	:{	DownLoadToDsp = 0;	break;}
		case	C_VOL	:{	DownLoadToDsp = 0;	break;}
		case	IGBT_TEMP	:{	DownLoadToDsp = 0;	break;}
		case	MOTO_TEMP	:{	DownLoadToDsp = 0;	break;}
		case	TEMP0	:{	DownLoadToDsp = 0;	break;}
		case	TEMP1	:{	DownLoadToDsp = 0;	break;}
		case	TEMP2	:{	DownLoadToDsp = 0;	break;}
		case	V_FEEDBACK	:{	DownLoadToDsp = 0;	break;}
		case	I_FEEDBACK	:{	DownLoadToDsp = 0;	break;}
		case	T_SET	:{	DownLoadToDsp = 0;	break;}
		case	SEC_POS_NUM	:{	DownLoadToDsp = 0;	break;}
		case	SEC_POS	:{	DownLoadToDsp = 0;	break;}
		case	SEC_CODER_ANGLE	:{	DownLoadToDsp = 0;	break;}
		//case	DISCHARGE_TIM	:{	DownLoadToDsp = 0;	break;}	//2026-01-26 InterUart中增加SAFE_CLOSE_SPEED(ID313:DSP自主收桨速度)的下传,此处需屏蔽
		case	DISCHARGE_ACCTIM	:{	DownLoadToDsp = 0;	break;}
		case	ALARM_LOCKH	:{	DownLoadToDsp = 0;	break;}
		case	ALARM_AUTO_RST_ENH	:{	DownLoadToDsp = 0;	break;}
		case	CAP_VOLT	:{	DownLoadToDsp = 0;	break;}
		case	ANALOG_IN0	:{	DownLoadToDsp = 0;	break;}
		case	ALARM_MASKH	:{	DownLoadToDsp = 0;	break;}
		case	DEFECT_CODEH	:{	DownLoadToDsp = 0;	break;}
		case	MOTO_SAFE_TEMP	:{	DownLoadToDsp = 0;	break;}
		case	PITCH_PLC_STATE	:{	DownLoadToDsp = 0;	break;}
		case	DIH_STA	:{	DownLoadToDsp = 0;	break;}
		case	DI_STA	:{	DownLoadToDsp = 0;	break;}
		case	DO_STA	:{	DownLoadToDsp = 0;	break;}
		case	USER_ARM_BUILD_YEAR	:{	DownLoadToDsp = 0;	break;}
		case	USER_ARM_BUILD_DATE	:{	DownLoadToDsp = 0;	break;}
		case	USER_ARM_BUILD_TIME	:{	DownLoadToDsp = 0;	break;}
		case	USER_PARA1_H	:{	DownLoadToDsp = 0;	break;}
		case	USER_PARA1_L	:{	DownLoadToDsp = 0;	break;}
		case	USER_PARA2_H	:{	DownLoadToDsp = 0;	break;}
		case	USER_PARA2_L	:{	DownLoadToDsp = 0;	break;}
		case	USER_PARA3_H	:{	DownLoadToDsp = 0;	break;}
		case	USER_PARA3_L	:{	DownLoadToDsp = 0;	break;}
		case	CONTROLWORD_DISP	:{	DownLoadToDsp = 0;	break;}
		case	TARGET_POSITION_L	:{	DownLoadToDsp = 0;	break;}
		case	TARGET_POSITION_H	:{	DownLoadToDsp = 0;	break;}
		case	DATAINIT_SELECT	:{	DownLoadToDsp = 0;	break;}
		case	SAVE_PARA	:{	DownLoadToDsp = 0;	break;}
		case	SEC_CODER_ANGLEH	:{	DownLoadToDsp = 0;	break;}
		case	REAL_ANGLE	:{	DownLoadToDsp = 0;	break;}
		case	REAL_ANGLEH	:{	DownLoadToDsp = 0;	break;}
		case	PITCH_SPEED	:{	DownLoadToDsp = 0;	break;}
		case	FAN_ON_DELAY	:{	DownLoadToDsp = 0;	break;}
		case	CO_CONSUMER_HEARTBEAT_TIME	:{	DownLoadToDsp = 0;	break;}
		case	ZLG7289_RES	:{	DownLoadToDsp = 0;	break;}
		case	BOFFCNT	:{	DownLoadToDsp = 0;	break;}
		case	TECCNT	:{	DownLoadToDsp = 0;	break;}
		case	ARM_DSP_COMMPARA0	:{	DownLoadToDsp = 0;	break;}
		case	C_VOL_ONOFF	:{	DownLoadToDsp = 0;	break;}
		case	C_VOL_ONOFF_SEL	:{	DownLoadToDsp = 0;	break;}
		case	ERRSAVE_ADDR_DISPL	:{	DownLoadToDsp = 0;	break;}
		case	ERRSAVE_ADDR_DISPH	:{	DownLoadToDsp = 0;	break;}
		default		:{	DownLoadToDsp = 1;	break;}
	}
}

void	ChangePara(void)
{
    if(UpDateFlag)		//参数有更改
    {
        CalculPara(UpDateParaID);
    }
}

const s16 crc_ta[256]=
{
    // CRC 余式表
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

u16 CRC16(u8 *ptr, u8 len)
{
    u16 crc;
    unsigned char da;
    crc=0;
    while(len--!=0)
    {
        da=(unsigned char) (crc/0x100);		// 以8 位二进制数的形式暂存CRC 的高8 位
        crc<<=8; 							// 左移8 位，相当于CRC 的低8 位乘以28
        crc^=crc_ta[da^*ptr]; 				// 高8 位和当前字节相加后再查表求CRC,再加上以前的CRC
        ptr++;
    }
    return(crc);
}

unsigned int crcForSingle;
unsigned int CRC16ForSingle(unsigned int Data) //不允许同一时刻在不同的地方调用，调用前必须用CRC16ForSingleReset清零
{
    unsigned char da;
    {
        //高8 位
        da=(unsigned char) (crcForSingle/0x100); // 以8 位二进制数的形式暂存CRC 的高8 位
        crcForSingle<<=8;                            // 左移8 位，相当于CRC 的低8 位乘以28
        crcForSingle^=crc_ta[da^(unsigned char)(Data / 256)]; //高8 位和当前字节相加后再查表求CRC,再加上以前的CRC

        //低8 位
        da=(unsigned char) (crcForSingle/0x100); // 以8 位二进制数的形式暂存CRC 的高8 位
        crcForSingle<<=8;                            // 左移8 位，相当于CRC 的低8 位乘以28
        crcForSingle^=crc_ta[da^(unsigned char) (Data & 0xff)]; //低8 位和当前字节相加后再查表求CRC,再加上以前的CRC
    }
    return(crcForSingle);
}

void CRC16ForSingleReset(void)
{
    crcForSingle = 0;
}

/*******************************************************************************
* 函数名 : SavePara
* 描  述 : 保存参数到ARM自带的FLASH扇区
* 输  入 : Mode :1、3、4=只存用户参数；2=同时存用户参数和出厂参数；5=只存出厂参数
* 输  出 : 无
* 返  回 : 1
*******************************************************************************/
s16 SavePara(u16 Mode)
{
    switch (Mode) 	// 0 无需保存 1 需要保存用户参数
    {
    case 1:  	// 1 需要保存用户参数（星辰上位机参数保存、或用户参数扇区已发生错误后将出厂参数数据写入用户参数扇区）
    case 3:  	// 3 需要保存用户参数（HMI自学习参数发生变化）
    case 4:  	// 4 需要保存用户参数（加密激活参数发生变化）
    {
        SaveParaToFlash(USER_PARA_ADDR);
        break;
    }

    case 2:  	// 2 需要保存用户参数和出厂默认参数（星辰上位机参数保存+Ctrl、或用户扇区和出厂扇区同时发送错误后将程序默认值写入这两个扇区）
    {
        SaveParaToFlash(USER_PARA_ADDR);
        SaveParaToFlash(DEFAULT_PARA_ADDR);
        break;
    }

    case 5:  	// 5 需要保存出厂默认参数（出厂参数扇区已发生错误，将用户参数数据写入出厂参数扇区）
    {
        SaveParaToFlash(DEFAULT_PARA_ADDR);
        break;
    }

    default:
    {
        break;
    }
    }
    return(1);
}

