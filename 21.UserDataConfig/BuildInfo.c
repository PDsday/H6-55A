#include "ExternPrototype.h"
#include "string.h"

#define  LITTLE_END 1;

u16 BuildYear; // 年
u16 BuildMonth; // 一月：Jan；二月：Feb；三月：Mar；四月：Apr；五月：May；六月：Jun；七月：Jul；八月：Aug；九月：Sep；十月：Oct；十一月：Nov；十二月：Dec；
u16 BuildDay; // 日
u16 BuildDate; // 月日
u16 BuildHour; // 时
u16 BuildMinute; // 分
u16 BuildTime; // 时分

/*******************************************************************************
* 函数名 : GetBuildInfo
* 描  述 : 获取编译信息
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void GetBuildInfo(void)
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

    p = __TIME__;					//为字符串 格式 "hh:mm:ss" (示例 "13:58:17")

    hh  = 10 * (*(p + 0) - '0') + (*(p + 1) - '0');//取小时转换
    mms = 10 * (*(p + 3) - '0') + (*(p + 4) - '0');//取分钟转换

    BuildYear = yy;
    BuildMonth = mm; //
    BuildDay = dd;
    BuildHour = hh;
    BuildMinute = mms;

    BuildDate = mm * 100 + dd;
    BuildTime = hh * 100 + mms;
}
