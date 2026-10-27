#include "CANopenUpdate.h"
#include "stm32f4xx.h"
#include "UserMainTask.h"
#include "includes.h"
#include "data.h"
#include "ExternPrototype.h"
#include "state_machine.h"
#include "IO.h"
#include "stm32f4xx_conf.h"

// #define TEST_FOR_S16
// #define TEST_FOR_S17
// #define TEST_FOR_S18

// #define TEST_FOR_S23
// #define TEST_FOR_S37
// #define TEST_FOR_S38

extern union DI_STATUS DIStatus;
extern u8 EraseFlash_IAP(u32 StartAddr, u8 FlashNum);


extern u8 Tpdo1_Data[8];
extern u8 Tpdo2_Data[8];
extern u8 Tpdo3_Data[8];
extern u8 Tpdo4_Data[8];
extern u8 Rpdo1_Data[8];
extern u8 Rpdo2_Data[8];
extern u8 Rpdo3_Data[8];
extern u8 Rpdo4_Data[8];

unsigned char ready_to_jump = 0;

static unsigned char update_step = 0;

struct _RxPDO1 *RxPDO1 = (struct _RxPDO1 *)&Rpdo1_Data[0];
struct _RxPDO2 *RxPDO2 = (struct _RxPDO2 *)&Rpdo2_Data[0];
struct _TxPDO1 *TxPDO1 = (struct _TxPDO1 *)&Tpdo1_Data[0];
struct _TxPDO2 *TxPDO2 = (struct _TxPDO2 *)&Tpdo2_Data[0];




u16 Return_0x666_Cnt = 0;

// unsigned char Return_0x551_Flag = 0;
// unsigned char Return_0x551_Delay = 0;
// unsigned char Return_0x551_Cnt = 0;

// 放入user_main，2ms执行，在变桨处理之后


unsigned char _Version_Check_()
{
    {
        // 版本查询不会将 RXPDO1_1_0置位。
        switch (RxPDO2->DATA4)
        {
        // 回复ARM的版本
        case 100:
            TxPDO2->DATA4 = (21458);
            break;

        // 回复DSP版本
        case 110:
            TxPDO2->DATA4 = ReadPara(SOFT_VER);	//DSP版本回访，改用版本号，不再使用DSP编译日期
            break;

        // 回复充电器版本
        case 120:
            TxPDO2->DATA4 = ReadPara2(CP_ARM_BUILD_DATE);
            break;

        // 访问齿轮比
        case 131:
            TxPDO2->DATA4 = 2272;
            break;

        // 变桨退出限位，桨叶变桨到89°
        case 0x555:

            TxPDO1->DATA2.bit.TXPDO1_2_14 = 0;

            // 【后续考虑增加次数限制，若主控未清掉RXPDO1_1_13to15和RxPDO2->DATA4 == 0x555，这里会一直发
            TxPDO2->DATA4 = 0x666;
            Return_0x666_Cnt++;
            ReadPara(ARM_RESERVE_PARA_837) = Return_0x666_Cnt;
						update_step = 0; //下载结束
            break;

//         // 其他未约定的数值
        default:
            return 0; // 响应正常变桨的RXPDO
            break;
        }
        return 1;
    }
}

unsigned char RPDO_Recv_link_analysis()
{
    static unsigned int counter = 0;


    // 以下为测试内容，未带实际电机，未合并运达程序前，临时配合杨工需求的三个芯片版本号。

    if (DIStatus.bit.DI00 != 0) // 升级指令必须在压限位情况下执行
    {
        counter = 0;
        update_step = 0;
        ready_to_jump = 0;
        return 0;
    }
    else
    {
        if (RxPDO1->DATA1.bit.RXPDO1_1_0 == 1)
        {
            // 10ms一个同步帧的情况下，累积
            if (update_step == 0)
            {
                counter++;
                // 升级完成后，Boor收到0x531，返回APP，主控不会清除 RXPDO1_1_0
                // 回到APP后，需要持续回复一定量的0x551，直到主控退出远程升级 RXPDO1_1_15 = 1
                if ((RxPDO1->DATA1.bit.RXPDO1_1_15 == 0))
                {
                    TxPDO2->DATA4 = 0x551;
                    //若有握手指令，继续执行下方握手逻辑即可
                }
                else // 直到主控发送退出远程升级后，需要持续更新角度位置与 TXPDO3_3
                {
                    //确认退出限位，开始变桨
                    update_step = 0xFF; //标记为升级接收流程，且限位退出（临时）
                    // 以下为测试内容，未带实际电机，未合并运达程序前，临时配合杨工需求的驱动器返回89°。
                    Tpdo3_Data[4] = 0xC4;
                    Tpdo3_Data[5] = 0x22;
                }

                
            }

            if (update_step == 0xFF)
            {
							// 以下为测试内容，未带实际电机，未合并运达程序前，临时配合杨工需求的驱动器返回89°。
								Tpdo3_Data[4] = 0xC4;
                Tpdo3_Data[5] = 0x22;
							// 测试结束--------------------------------------------------------------------
								if(RxPDO2->DATA4 == 0x555)
								{
										 //客户发送角度查询命令，需要回复角度到达确认，这部分再RXPDO1_1_0取消后才执行，同版本查询坐一桌。
										 //此处保留，避免后续客户修改。
                    TxPDO2->DATA4 = 0x666;
                    TxPDO1->DATA2.bit.TXPDO1_2_14 = 0;  //清除升级完成状态
								}
                   
            }

						//不再处理误触发，
//             if (counter > 2500 && update_step == 0) // 5s内依旧未获得第一个握手回应，认为误触发升级流程。客户进入升级流程需要给予XPDO1_1_0进行跳变
//             {
//                 // 升级结束流程与此误触发判断流程不冲突
//                 return 0;
//             }
						
						_Version_Check_();	//现在升级完成后，会保持RXPDO1-1-0置位，直到查询所有需要信息后，才取消。

            if (RxPDO2->DATA4 == 0x255 || update_step == 1)
            {
                update_step = 1;
                TxPDO2->DATA4 = 0x256;
            }

            if (RxPDO2->DATA4 == 0x268 || update_step == 2)
            {
                update_step = 2;
                TxPDO2->DATA4 = 0x269;
            }

            if (RxPDO2->DATA4 == 0x299 || update_step == 3)
            {
                update_step = 3;
                // 升级前，若有动作任务，在这里启动
            }

            if (update_step == 3)
            {
                if (DIStatus.bit.DI00 == 0 && 1) // 确认升级条件已经满足，比如角度到达，限位满足，角度已保存等。目前杨工PLC程序下载前并不会访问驱动器当前的角度
                {
										//canInit(250);
                    TxPDO1->DATA2.bit.TXPDO1_2_13 = 1; // 汇报boot远程升级准备好

                    if (RxPDO1->DATA1.bit.RXPDO1_1_1 == 1) // 等待主控进入远程升级状态机后，跳转boot
                    {
                        if (ready_to_jump == 1 && counter > 6)
                        {
                            // 跳转boot失败了（实际无法执行到此处）
                            ready_to_jump = 0;
                            counter = 0;
														//TxPDO1->DATA2.bit.TXPDO1_2_15 = 1; // 汇报远程升级异常
                        }
                        else
                        {
                            if (ready_to_jump == 1)
                            {
                                counter++;
                                if (counter == 6) // 升级前置条件满足，且延迟12ms后，准许进入Boot程序中,确保送出TPDO信息
                                {
                                    CANopen_Jump_bootloader();

                                }
                            }
                            else
                            {
                                counter = 0;
                                ready_to_jump = 1; // 跳转Boot;
                            }
                        }
                    }
                }
            }

            return 1;
        }
        else
        {
            // 取消了升级标志置位，则清除超时累计
            counter = 0;
            update_step = 0;
            ready_to_jump = 0;
					  //版本访问时，RXPDO1_1_0不会置位，非置位状态下也会访问版本信息，
					
            return _Version_Check_();
        }
    }

}







