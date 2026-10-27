#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#define SHUTDOWN        			0x0006
#define SHUTDOWN_MARK    			0x0087
#define SWITCH_ON        			0x0007
#define SWITCH_ON_MARK   			0x008F
#define DISABLE_VOLTAGE  			0x0000
#define DISABLE_VOLTAGE_MARK 		0x0082
#define DISABLE_OPERATION 			0x0007
#define DISABLE_OPERATION_MARK 		0x008F
#define ENABLE_OPERATION   			0x000F
#define ENABLE_OPERATION_MARK 		0x008F
#define QUICK_STOP       			0x0002
#define QUICK_STOP_MARK  			0x0086
#define FAULT_RESET           		0x0080
#define FAULT_RESET_MARK      		0x0080

//					 STARS		   MICC		State					  xxxx xxxx 7654 3210	Key Bit Value
#define NRSO_VALUE  0x0000    	// 0x0000	Not Ready to Switch On	= xxxx xxxx x0xx 0000	bit6：Switch on disable=0
#define SOD_VALUE   0x0050    	// 0x0040	Switch on disable		= xxxx xxxx x1xx 0000	bit6：Switch on disable=1
#define RSO_VALUE   0x0031    	// 0x0021	Ready to Switch On		= xxxx xxxx x01x 0001	bit0：Ready to switch on=1
#define SO_VALUE    0x0033    	// 0x0023	Switched On 			= xxxx xxxx x01x 0011	bit1：Switch on=1
#define OP_VALUE    0x0037    	// 0x0027	Operation Enable		= xxxx xxxx x01x 0111	bit2：Operation enable=1
#define QSA_VALUE   0x0007    	// 0x0007	Quick Stop Active		= xxxx xxxx x00x 0111	bit5：Quick stop=0
#define FRA_VALUE   0x000F    	// 0x000F	Fault Reaction Active	= xxxx xxxx x0xx 1111	bit3：Fault=1
#define FT_VALUE    0x0008    	// 0x0008	Fault					= xxxx xxxx x0xx 1000	bit3：Fault=1

#define ST  								0    				// Start
#define NRSO  								1    				// Not Ready to Switch On
#define SOD   								2    				// Switch on disable
#define RSO   								3    				// Ready to Switch On
#define SO    								4    				// Switched On 
#define OP    								5    				// Operation Enable
#define QSA   								6    				// Quick Stop Active
#define FRA   								7    				// Fault Reaction Active
#define FT   								8    				// Fault

#define STATUS_START						0					// 上电
#define STATUS_NOT_READY_TO_SWITCH_ON		1					// 初始化
#define STATUS_SWITCH_ON_DISABLED			2					// 开关未使能
#define STATUS_READY_TO_SWITCH_ON			3					// 准备闭合开关
#define STATUS_SWITCHED_ON					4					// 闭合开关
#define STATUS_OPERATION_ENABLED			5					// 使能电机通电运行
#define STATUS_QUICK_STOP_ACTIVE			6					// 急停激活
#define STATUS_FAULT_REACTION_ACTIVE		7					// 故障响应
#define STATUS_FAULT						8					// 故障

#endif