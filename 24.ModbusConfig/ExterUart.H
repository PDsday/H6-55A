#ifndef	_EXTERUART_
#define	_EXTERUART_
#include "stm32f4xx.h"

#define	EXUART_RX_CONFIG_FRAMELEN	11				//接收配置帧长度
#define	EXUART_TX_CONFIG_FRAMELEN	11				//发送配置帧长度

#define	EXUART_RX_DATA_FRAMELEN	7				//接收数据帧长度
#define	EXUART_TX_DATA_FRAMELEN	7				//发送数据帧长度

#define	EXUART_RX_MAX_FRAMELEN	11				//接送数据帧最大长度
#define	EXUART_TX_MAX_FRAMELEN	11				//发送数据帧最大长度

#define RS485_MB_ADDR 0
#define RS485_MB_FUNC 1

struct	EXUART_COMMAND_BIT{
u16 PARAID:14;			//参数ID					BIT0~13
u16 CONTENT:1;			//命令/ID	0 ID   1 命令	BIT14
u16 RWBIT:1;			//读写标志 	0 读   1 写		BIT15
};
union	EXUART_COMMAND{
u16 all;
struct	EXUART_COMMAND_BIT	bit;
};

extern u8 AdjModFlag; // 0：正常模式 1：调试模式
extern u8 LoadDefaultParaFlag; // 调出厂参数标志: 1调出厂参数
extern u8 DSPReturnPCFlag; // DSP返回数据标志：0未返回数据，1已返回数据

extern void Mdb_Rx_data(void);
extern void Mdb_Tx_data(void);

#endif
