#include "PLCFunction.h"

PLC_TON_TOF_TypeDef  PLC_TON_TOF_Array[PLC_TON_ADD_TOF_NUM];
u8 PLC_TON_TOF_Num = 0;
PLC_CLK_TRIG_TypeDef  PLC_CLK_TRIG_Array[PLC_CLK_TRIG_NUM];
u8 PLC_CLK_Num = 0;

/*******************************************************************************
* 函数名 : PLC_TON
* 描  述 : PLC TON函数
* 输  入 : u8* PLC_TONOut    PLC TON函数输出布尔变量指针
		   u8* PLC_TONIn	    PLC TON函数输入布尔变量指针
		   u32 PLC_TON_Time 	PLC TON函数输出延时时间
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_TON(u8* PLC_TONOut, u8* PLC_TONIn, u32 PLC_TON_Time)
{
    if (PLC_TON_TOF_Num == PLC_TON_ADD_TOF_NUM) return;  // PLC_TON_TOF数组已满则返回
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_TON_TOF_Out     = PLC_TONOut;
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_TON_TOF_In      = PLC_TONIn;
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_TON_TOF_Time    = PLC_TON_Time;
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_TON_TOF_TimeCnt = 0;
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_Type           = 1;	  // PLC TON
    PLC_TON_TOF_Num++;
}

/*******************************************************************************
* 函数名 : PLC_TOF
* 描  述 : PLC TOF函数
* 输  入 : u8* PLC_TOFOut       PLC TOF函数输出布尔变量指针
		   u8* PLC_TOFIn	    PLC TOF函数输入布尔变量指针
		   u32 PLC_TOF_Time 	PLC TOF函数输出延时时间
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_TOF(u8* PLC_TOFOut, u8* PLC_TOFIn, u32 PLC_TOF_Time)
{
    if (PLC_TON_TOF_Num == PLC_TON_ADD_TOF_NUM) return;  // PLC_TON_TOF数组已满则返回
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_TON_TOF_Out     = PLC_TOFOut;
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_TON_TOF_In      = PLC_TOFIn;
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_TON_TOF_Time    = PLC_TOF_Time;
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_TON_TOF_TimeCnt = 0;
    PLC_TON_TOF_Array[PLC_TON_TOF_Num].PLC_Type           = 0;	  // PLC TOF
    PLC_TON_TOF_Num++;
}

/*******************************************************************************
* 函数名 : PLC_TON_TOF_Process
* 描  述 : PLC TON和TOF后台处理函数
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_TON_TOF_Process(void)
{
    u8 i, j;

    if (PLC_TON_TOF_Num == 0) return;	  // 无PLC TON处理则返回
    for (i = 0; i < PLC_TON_TOF_Num; i++)
    {
        j =  1 - PLC_TON_TOF_Array[i].PLC_Type;
        if (*(PLC_TON_TOF_Array[i].PLC_TON_TOF_In) == j)
        {
            *(PLC_TON_TOF_Array[i].PLC_TON_TOF_Out) = j;	// PLC TON输入为0则输出为0
            PLC_TON_TOF_Array[i].PLC_TON_TOF_TimeCnt = 0;												// PLC TOF相反
        }
        else if (PLC_TON_TOF_Array[i].PLC_TON_TOF_TimeCnt == PLC_TON_TOF_Array[i].PLC_TON_TOF_Time)
        {
            *(PLC_TON_TOF_Array[i].PLC_TON_TOF_Out) = PLC_TON_TOF_Array[i].PLC_Type;	// PLC TON输入为1保持设定时间后输出为1
        }																				// PLC TOF相反
        else
        {
            *(PLC_TON_TOF_Array[i].PLC_TON_TOF_Out) = j;	// PLC TON输入为1不够设定时间时则输出为0
            PLC_TON_TOF_Array[i].PLC_TON_TOF_TimeCnt++;												// PLC TOF相反
        }
    }
}

/*******************************************************************************
* 函数名 : PLC_CLK_R_TRIG
* 描  述 : PLC时钟上升沿检测函数
* 输  入 : u8* PLC_CLK_R_TRIG_Q     PLC 上升沿检测输出布尔变量指针
		   u8* PLC_CLK_TRIG_In	    PLC 时钟输入布尔变量指针
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_CLK_R_TRIG(u8* PLC_CLK_R_TRIG_Q, u8* PLC_CLK_TRIG_In)
{
    if (PLC_CLK_Num == PLC_CLK_TRIG_NUM) return;   // PLC时钟检测数组已满则返回
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_Out = PLC_CLK_R_TRIG_Q;
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_In  = PLC_CLK_TRIG_In;
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_InOld  = *PLC_CLK_TRIG_In;
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_TimeCnt = 0;
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_Type    = 1;
    PLC_CLK_Num++;
}

/*******************************************************************************
* 函数名 : PLC_CLK_F_TRIG
* 描  述 : PLC时钟下升沿检测函数
* 输  入 : u8* PLC_CLK_F_TRIG_Q     PLC 下升沿检测输出布尔变量指针
		   u8* PLC_CLK_TRIG_In	    PLC 时钟输入布尔变量指针
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_CLK_F_TRIG(u8* PLC_CLK_F_TRIG_Q, u8* PLC_CLK_TRIG_In)
{
    if (PLC_CLK_Num == PLC_CLK_TRIG_NUM) return;   // PLC时钟检测数组已满则返回
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_Out = PLC_CLK_F_TRIG_Q;
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_In  = PLC_CLK_TRIG_In;
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_InOld  = *PLC_CLK_TRIG_In;
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_TimeCnt = 0;
    PLC_CLK_TRIG_Array[PLC_CLK_Num].PLC_CLK_TRIG_Type    = 0;
    PLC_CLK_Num++;
}

/*******************************************************************************
* 函数名 : PLC_CLK_TRIG_Precoss
* 描  述 : PLC时钟检测后台处理函数  500us调用一次
* 输  入 : 无
* 输  出 : 无
* 返  回 : 无
*******************************************************************************/
void PLC_CLK_TRIG_Precoss(void)
{
    u8 i;

    if (PLC_CLK_Num == 0) return;	 // 无时钟检测则返回
    for (i = 0; i < PLC_CLK_Num; i++)
    {
        if (PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_TimeCnt == (PLC_CLK_TRIG_Q_WIDTH * 2))	  // Q置高设定时间后置低
        {
            *(PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_Out)  = 0;
            //PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_TimeCnt = 0;
        }

        if ((*(PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_Out))  == 1)		 // Q置高后开始计数
        {
            PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_TimeCnt++;
        }
        else
        {
            if ((PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_InOld) != (*(PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_In)))	// 检测边沿
            {
                if ((*(PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_In)) == PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_Type)
                {
                    *(PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_Out)  = 1;
                    PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_TimeCnt = 0;
                }
            }

            PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_InOld = *(PLC_CLK_TRIG_Array[i].PLC_CLK_TRIG_In);
        }
    }
}