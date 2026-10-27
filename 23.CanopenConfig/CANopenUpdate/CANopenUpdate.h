#ifndef __CANOPENUPDATE_H
#define __CANOPENUPDATE_H
////////////////////////////////////////////////////////////////////////////
struct _rxpdo1_data1
{
    unsigned short RXPDO1_1_0:1;        //变桨远程升级指令
    unsigned short RXPDO1_1_1:1;        //主控进入远程升级状态机
    unsigned short RXPDO1_1_2:1;        //主控开始下载APP命令
    unsigned short RXPDO1_1_3:1;        //升级芯片为DSP
    unsigned short RXPDO1_1_4:1;        //升级芯片为ARM
    unsigned short RXPDO1_1_5:1;        //升级芯片为充电器
    unsigned short RXPDO1_1_6:1;        //保留
    unsigned short RXPDO1_1_7:1;        //保留
    unsigned short RXPDO1_1_8:1;        //保留
    unsigned short RXPDO1_1_9:1;        //保留
    unsigned short RXPDO1_1_10:1;       //保留
    unsigned short RXPDO1_1_11:1;       //保留
    unsigned short RXPDO1_1_12:1;       //保留
    unsigned short RXPDO1_1_13:1;
		unsigned short RXPDO1_1_14:1;
		unsigned short RXPDO1_1_15:1;				//主控需求驱动器退出远程升级后，变桨到89°
	//to15:3;   //最后一段字节数
    
};

union _rxPDO1_DATA1
{
    unsigned short all;
    struct _rxpdo1_data1 bit;    
};

struct _RxPDO1
{
    union _rxPDO1_DATA1 DATA1;
    unsigned short DATA2;
    unsigned short DATA3;
    unsigned short DATA4;
};
///////////////////////////////////////////////////////////////////////////






///////////////////////////////////////////////////////////////////////////
struct _rxpdo2_data1
{
    unsigned short RXPDO2_1_0to6:7;     //本帧数据在包内的段号
    unsigned short RXPDO2_1_7to15:9;    //本帧数据发送数据包号
    
};

union _rxPDO2_DATA1
{
    unsigned short all;
    struct _rxpdo2_data1 bit;    
};

struct _RxPDO2
{
    union _rxPDO1_DATA1 DATA1;
    unsigned short DATA2;
    unsigned short DATA3;
    unsigned short DATA4;
};
////////////////////////////////////////////////////////////////////////////










///////////////////////////////////////////////////////////////////////////
struct _txpdo1_data1
{
    unsigned short TXPDO1_1_0:1;        //保留
    unsigned short TXPDO1_1_1:1;        //保留
    unsigned short TXPDO1_1_2:1;        //保留
    unsigned short TXPDO1_1_3:1;        //保留
    unsigned short TXPDO1_1_4:1;        //保留
    unsigned short TXPDO1_1_5:1;        //arm进入重启完成
    unsigned short TXPDO1_1_6:1;        //dsp进入重启完成
    unsigned short TXPDO1_1_7:1;        //充电器重启完成
    unsigned short TXPDO1_1_8:1;        //arm进入boot状态
    unsigned short TXPDO1_1_9:1;        //dsp进入boot状态
    unsigned short TXPDO1_1_10:1;       //充电器进入如boot状态
    unsigned short TXPDO1_1_11:1;       //所有包接收完成
    unsigned short TXPDO1_1_12:1;       //保留
    unsigned short TXPDO1_1_13to15:3;   //最有一段字节数回发
    
};

union _txPDO1_DATA1
{
    unsigned short all;
    struct _txpdo1_data1 bit;
};

struct _txpdo1_data2
{
    unsigned short TXPDO1_2_0:1;        //传输过程CAN中断
    unsigned short TXPDO1_2_1:1;        //驱动器切换APP失败
    unsigned short TXPDO1_2_2:1;        //驱动器状态（在rxpdo1_1_0 == 1且 rxpdo1_1_1==1情况下， 此位：0-驱动器arm运行app程序，1-驱动器运行boot程序）
    unsigned short TXPDO1_2_3:1;        //收数据包超时
    unsigned short TXPDO1_2_4:1;        //接收RxPDO超时
    unsigned short TXPDO1_2_5:1;        //桨叶位置保存错误
    unsigned short TXPDO1_2_6:1;        //重新开始升级流程
    unsigned short TXPDO1_2_7:1;        //APP数据包CRC校验错误
    unsigned short TXPDO1_2_8:1;        //APP数据包丢包
    unsigned short TXPDO1_2_9:1;        //APP包号段号异常
    unsigned short TXPDO1_2_10:1;       //FLASH擦除超时
    unsigned short TXPDO1_2_11:1;       //包写入成功
    unsigned short TXPDO1_2_12:1;       //FLASH擦除完成
    unsigned short TXPDO1_2_13:1;       //远程升级准备好
    unsigned short TXPDO1_2_14:1;       //变桨远程升级完成
    unsigned short TXPDO1_2_15:1;       //变桨远程升级异常
    
};
union _txPDO1_DATA2
{
    unsigned short all;
    struct _txpdo1_data2 bit;
};

struct _txpdo1_data3
{
    unsigned short TXPDO1_3_0to6:7;     //异常段号  
    unsigned short TXPDO1_3_7to15:9;    //异常包号
    
};
union _txPDO1_DATA3
{
    unsigned short all;
    struct _txpdo1_data3 bit;
};

struct _txpdo1_data4
{
    unsigned short TXPDO1_3_0to6:7;     //返回收到的段号 
    unsigned short TXPDO1_3_7to15:9;    //返回收到的包号
    
};
union _txPDO1_DATA4
{
    unsigned short all;
    struct _txpdo1_data4 bit;
};



struct _TxPDO1
{
    union _txPDO1_DATA1 DATA1;
    union _txPDO1_DATA2 DATA2;
    union _txPDO1_DATA3 DATA3;
    union _txPDO1_DATA4 DATA4;
};
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
struct _TxPDO2
{
    unsigned short DATA1;
    unsigned short DATA2;
    unsigned short DATA3;
    unsigned short DATA4;
};
////////////////////////////////////////////////////////////////////////////////


/*************** */


#endif


