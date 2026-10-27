#include "stm32f4xx_conf.h"

#define SLOW_TICK_PERIOD 		1												//低速节拍周期	 1ms
#define SLOW_TIMER_PERIOD 		100 											//低速节拍生成定时器周期	 100us
#define SLOWTIMEBASECNTTOP 	 	(SLOW_TICK_PERIOD * 1000) / SLOW_TIMER_PERIOD 	//	

struct SLOWTIME_FLAG_BIT{
unsigned int INUART:1;			//与DSP内部通讯超时节拍标志
unsigned int EXUART:1;			//与上位机RS485通讯超时节拍标志
unsigned int SLOW:1;			//低速层节拍标志
unsigned int resvd1:13;			//保留
};

union SLOWTIMEFLAG{
struct SLOWTIME_FLAG_BIT bit;
unsigned int all;
};
