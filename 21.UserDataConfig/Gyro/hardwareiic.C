#include "hardwareiic.h"
#include "stm32f4xx_i2c.h"
#include "mpu6050.h"
#include "iim42652.h"
#include "ExternPrototype.h"
#include "misc.h"
#include "stm32f4xx.h"

// PWX新增**********************************************************
u8 IS_IRQn_READ = 0;  // 中断函数用于区分 EV5事件后执行的读写操作
u8 IIC_IRQn_FLAG = 0; // I2C读取流程选择是否启用中断+DMA方式发送
u8 IRQn_REG_ADDR = 0; // EV6事件后发送iim42652的寄存器地址

u8 IS_READ_ACC_FLAG = 0;
u8 IS_READ_GYRO_FLAG = 0;
u8 IS_READ_TEMP_FLAG = 0;

u8 IIC_DMA_FLAG = 0; //是否启用中断+DMA
// u8 I2C2_DMA_Rx_Buffer[12] = {0};
// u8 I2C2_DMA_Rx_Len = 12;
u8 I2C2_DMA_Rx_Buffer[6] = {0};
u8 I2C2_DMA_Rx_Len = 6;
// PWX新增**********************************************************

u16 GyroCheck = 0xAAAA;
u16 GyroChip = 0;
extern u8 GyroCode;
extern s16 GyroTemp;

/*
************************************************************
*	函数名称：	MPU6050_I2c_Bus_Init
*
*	函数功能：	总线初始化
*
*	入口参数：	无
*
*	返回参数：	 0
*
*	说明：		PB6--SCL			 PB7---SDA
************************************************************
*/
void MPU6050_I2c_Bus_Init(void)
{
	GPIO_ConfigurationI2C(); // IO
	
	GyroChip = 0;								//ReadPara(GYRO_CHIP);芯片选择
	
	if (MPU6050ReadID() != 0) 					//自检
	{
		if (MPU6050Hardware_Selftest() == 0) 	//自检
		{
			if (MPU6050_Init() == 0) 			//初始化
			{
				GyroCheck = 0x5555;
			}
		}
		else
		{
			GyroCheck = 0xAAAA;
		}
	}
	else
	{
		GyroCode |= 0x04;
		GyroCheck = 0xAAAA;
	}
	
	ReadPara(GYRO_CODE) = GyroCode; 
}

void IIM42652_I2c_Bus_Init(void)
{
	GPIO_ConfigurationI2C();					// IO
	
	GyroChip = 1;				 				// ReadPara(GYRO_CHIP);芯片选择
	
	if (IIM42652_Init() == 0) 					// 自检
	{
		GyroCheck = 0x5555;
	}
	else
	{
		GyroCheck = 0xAAAA;
	}
}

/*
************************************************************
*	函数名称：	GPIO_ConfigurationI2C
*
*	函数功能：	硬件IIC端口配置
*
*	入口参数：	无
*
*	返回参数：	 0
*
*	说明：		PF1--SCL			 PF0---SDA
************************************************************
*/
void GPIO_ConfigurationI2C(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	// PWX新增**************************************
	DMA_InitTypeDef DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	// PWX新增**************************************

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); //使能端口的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);	/** CODEC_I2C peripheral configuration */

	/* Configure I2C2 pins: SCL and SDA */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; //< 开漏输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	 //< 无上拉下拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_I2C2);

	I2C_Cmd(I2C2, DISABLE);
	I2C_DeInit(I2C2);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;									//设置I2C2为I2C模式
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;							//I2C快速Tlow/Thigh = 2，高电平数据稳定，低电平数据变化SCL时钟线的占空比
	I2C_InitStructure.I2C_OwnAddress1 = 0x0A;									//设置第一个设备自身地址·这个地址只需要与STM32外挂的IIC器件的地址不一样即可
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;									//使能应答ACK
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 	//定义应答地址7bit
	I2C_InitStructure.I2C_ClockSpeed = 400000;									//设置时钟频率
	I2C_Init(I2C2, &I2C_InitStructure);											//初始化IIC1

	// PWX新增**************************************
	DMA_Cmd(DMA1_Stream2, DISABLE);
	// while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);
	DMA_InitStruct.DMA_Channel = DMA_Channel_7;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&I2C2->DR;
	DMA_InitStruct.DMA_Memory0BaseAddr = (u32)I2C2_DMA_Rx_Buffer;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory; //从外设到内存
	DMA_InitStruct.DMA_BufferSize = I2C2_DMA_Rx_Len;		 // IIC_BUFF_LEN;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream2, &DMA_InitStruct);
	DMA_ITConfig(DMA1_Stream2, DMA_IT_TC | DMA_IT_TE, ENABLE); //传输完成中断以及传输错误中断关使能

	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Stream2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;				
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = I2C2_EV_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;				
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = I2C2_ER_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;				
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, DISABLE); 	//初始化过程不可启用中断
	I2C_ITConfig(I2C2, I2C_IT_ERR, DISABLE);				//初始化过程不可启用中断

	I2C_DMACmd(I2C2, ENABLE);
	DMA_Cmd(DMA1_Stream2, DISABLE); 						//初始化不需要启动DMA，待软件运行后再交付DMA托管
	I2C_DMALastTransferCmd(I2C2, ENABLE);
	// while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);
	// PWX新增**************************************

	I2C_Cmd(I2C2, ENABLE);
}

/*
************************************************************
*	函数名称：	I2C_ByteWrite
*
*	函数功能：	写一个字节到I2C设备中
*
*	入口参数：	pBuffer:缓冲区指针	 WriteAddr:写地址
*
*	返回参数：	 正常返回1，异常返回0
*
*	说明：
************************************************************
*/
u16 ErrorTimes[6] = {0, 0, 0, 0, 0, 0}; // 0：Ev5 		1:EV6 	2:EV7 	3:EV8	4:Busy_times 5:Busy_Err

uint8_t I2C_ByteWrite(u8 pBuffer, u8 WriteAddr)
{
	u32 I2CTimeout = 0; //超时时间

	I2CTimeout = I2C_MAX_DEAD_TIME; // 0.2ms
	while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == SET)
	{
		if ((I2CTimeout--) == 0)
		{
			ErrorTimes[4]++;

			if (ErrorTimes[4] >= 250)
			{
				// I2C_Cmd(I2C2,DISABLE);
				// GPIO_ConfigurationI2C();
				ErrorTimes[5]++;
				I2C_GenerateSTOP(macI2Cx, ENABLE);
			}
			return 0;
		}
	}

	ErrorTimes[4] = 0;

	/* Send STRAT condition */
	I2C_GenerateSTART(macI2Cx, ENABLE); //产生IIC1传输START条件

	I2CTimeout = I2C_MAX_DEAD_TIME; // 0.2ms

	/* Test on EV5 and clear it */
	while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_MODE_SELECT)) //检查EV5
	{

		if ((I2CTimeout--) == 0)
		{
			ErrorTimes[0]++; // EV5超时次数
			I2C_GenerateSTOP(macI2Cx, ENABLE);
			return 0;
		}
	}

	/* Send slave address for write */
	I2C_Send7bitAddress(macI2Cx, MPU6050_SLAVE_ADDRESS, I2C_Direction_Transmitter); //发送器件地址

	I2CTimeout = I2C_MAX_DEAD_TIME;
	/* Test on EV6 and clear it */
	while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) //检查VE6
	{
		if ((I2CTimeout--) == 0)
		{
			ErrorTimes[1]++; // EV6超时次数
			I2C_GenerateSTOP(macI2Cx, ENABLE);
			return 0;
		}
	}

	/* Send the slave's internal address to write to */
	I2C_SendData(macI2Cx, WriteAddr); //发送写高字节地址

	I2CTimeout = I2C_MAX_DEAD_TIME;
	/* Test on EV8 and clear it */
	while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) //检查VE8
	{
		if ((I2CTimeout--) == 0)
		{
			ErrorTimes[3]++; // EV8超时次数
			I2C_GenerateSTOP(macI2Cx, ENABLE);
			return 0;
		}
	}

	/* Send the byte to be written */
	I2C_SendData(macI2Cx, pBuffer); //发送写低字节地址

	I2CTimeout = I2C_MAX_DEAD_TIME;

	/* Test on EV8 and clear it */
	while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) //检查VE8
	{
		if ((I2CTimeout--) == 0)
		{
			ErrorTimes[3]++; // EV8超时次数
			I2C_GenerateSTOP(macI2Cx, ENABLE);
			return 0;
		}
	}

	/* Send STOP condition */
	I2C_GenerateSTOP(macI2Cx, ENABLE); //产生IIC传输STOP条件

	return 1; //正常返回1
}

/*
****************************************************************
*	函数名称：	I2C_BufferRead
*
*	函数功能：	从I2C设备里面读取一块数据
*
*	入口参数：	pBuffer:存放从slave读取的数据的缓冲区指针
*				WriteAddr:接收数据的从设备的地址
*				NumByteToWrite:要从从设备读取的字节数
*
*	返回参数：	 正常返回1，异常返回0
*
*	说明：
***************************************************************
*/

u16 ReadyTime[6] = {0xFFFF};

uint8_t I2C_BufferRead(u8 *pBuffer, u8 ReadAddr, u16 NumByteToRead)
{

	u32 I2CTimeout = 0; //超时时间
	if (IIC_IRQn_FLAG == NONE_USE_IRQn)
	{
		I2CTimeout = I2C_MAX_DEAD_TIME;
		while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == SET)
		{
			if ((I2CTimeout--) == 0)
			{
				ErrorTimes[4]++;

				if (ErrorTimes[4] >= 250)
				{
					// I2C_Cmd(I2C2,DISABLE);
					// GPIO_ConfigurationI2C();
					ErrorTimes[5]++;
					I2C_GenerateSTOP(macI2Cx, ENABLE);
				}
				return 0;
			}
		}

		ErrorTimes[4] = 0;
		ReadyTime[0] = I2CTimeout;
		I2C_AcknowledgeConfig(macI2Cx, ENABLE); //允许应答
		I2C_GenerateSTART(macI2Cx, ENABLE);		 //产生IIC1传输START条
		I2CTimeout = I2C_MAX_DEAD_TIME;
		/* Test on EV5 and clear it */
		while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_MODE_SELECT)) //检查EV5
		{
			if ((I2CTimeout--) == 0)
			{
				ErrorTimes[0]++; // EV5超时次数
				I2C_GenerateSTOP(macI2Cx, ENABLE);
				return 0;
			}
		}
		ReadyTime[1] = I2CTimeout;
		/* Send slave address for write */
		I2C_Send7bitAddress(macI2Cx, MPU6050_SLAVE_ADDRESS, I2C_Direction_Transmitter); //发送器件地址
		I2CTimeout = I2C_MAX_DEAD_TIME;
		/* Test on EV6 and clear it */
		while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) //检查VE6
		{
			if ((I2CTimeout--) == 0)
			{
				ErrorTimes[1]++; // EV6超时次数
				I2C_GenerateSTOP(macI2Cx, ENABLE);
				return 0;
			}
		}
		ReadyTime[2] = I2CTimeout;
		/* Clear EV6 by setting again the PE bit */
		I2C_Cmd(macI2Cx, ENABLE);
		/* Send the slave's internal address to write to */
		I2C_SendData(macI2Cx, ReadAddr);
		I2CTimeout = I2C_MAX_DEAD_TIME;
		/* Test on EV8 and clear it */
		while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) //检查VE8
		{
			if ((I2CTimeout--) == 0)
			{
				ErrorTimes[3]++; // EV6超时次数
				I2C_GenerateSTOP(macI2Cx, ENABLE);
				return 0;
			}
		}
		ReadyTime[3] = I2CTimeout;
		/* Send STRAT condition a second time */
		I2C_GenerateSTART(macI2Cx, ENABLE); //产生IIC1传输START条件
		I2CTimeout = I2C_MAX_DEAD_TIME;
		/* Test on EV5 and clear it */
		while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_MODE_SELECT)) //检查EV5
		{
			if ((I2CTimeout--) == 0)
			{
				ErrorTimes[0]++; // EV5超时次数
				I2C_GenerateSTOP(macI2Cx, ENABLE);
				return 0;
			}
		}
		ReadyTime[4] = I2CTimeout;
		/* Send slave address for read */
		I2C_Send7bitAddress(macI2Cx, MPU6050_SLAVE_ADDRESS, I2C_Direction_Receiver); //接收地址
		I2CTimeout = I2C_MAX_DEAD_TIME;
		/* Test on EV6 and clear it */
		while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) //检查VE6
		{
			if ((I2CTimeout--) == 0)
			{
				ErrorTimes[1]++; // EV6超时次数
				I2C_GenerateSTOP(macI2Cx, ENABLE);
				return 0;
			}
		}
		ReadyTime[5] = I2CTimeout;
	}
	else
	{
		// PWX新增***************************************************************************************************
		I2CTimeout = I2C_MAX_DEAD_TIME;
		while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) == SET)
		{
			if ((I2CTimeout--) == 0)
			{
				ErrorTimes[4]++;

				if (ErrorTimes[4] >= 500)
				{
					// I2C_Cmd(I2C2,DISABLE);
					// GPIO_ConfigurationI2C();
					// ErrorTimes[5]++;
					I2C_GenerateSTOP(macI2Cx, ENABLE);
					GPIO_ConfigurationI2C();
					ErrorTimes[4] = 0;
					ErrorTimes[5] = 0;
				}
				return 0;
			}
		}

		ErrorTimes[4] = 0;
		ReadyTime[0] = I2CTimeout;

		IS_IRQn_READ = 0;
		IRQn_REG_ADDR = ReadAddr;
		I2C_AcknowledgeConfig(macI2Cx, ENABLE); //允许应答
		I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
		I2C_ITConfig(I2C2, I2C_IT_ERR, ENABLE);
		I2C_GenerateSTART(macI2Cx, ENABLE); //产生IIC1传输START条
	}

	// PWX新增******************************************************************************************************
	/* While there is data to be read */
	if (IIC_DMA_FLAG == NONE_USE_DMA1_STREAM2)
	{
		while (NumByteToRead)
		{
			if (NumByteToRead == 1)
			{
				/* Disable Acknowledgement */
				I2C_AcknowledgeConfig(macI2Cx, DISABLE); //最后一位后要关闭应答的	//

				/* Send STOP Condition */
				I2C_GenerateSTOP(macI2Cx, ENABLE);
			}

			/* Test on EV7 and clear it */
			I2CTimeout = I2C_MAX_DEAD_TIME;

			while (!I2C_CheckEvent(macI2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) //检测VE7	//修改此处代码，避免总线锁死，等不到对应的标志位而死循环
			{
				if ((I2CTimeout--) == 0)
				{
					ErrorTimes[2]++; // EV7超时次数
					I2C_GenerateSTOP(macI2Cx, ENABLE);
					return 0;
				}
			}

			/* Read a byte from the slave */
			*pBuffer = I2C_ReceiveData(macI2Cx);

			/* Point to the next location where the byte read will be saved */
			pBuffer++;

			/* Decrement the read bytes counter */
			NumByteToRead--;
		}

		/* Enable Acknowledgement to be ready for another reception */
		I2C_AcknowledgeConfig(macI2Cx, ENABLE); //再次允许应答模式
		return 1;															 //正常，返回1
	}
	else
	{
		// PWX新增*********************************************************************************************
		DMA_Cmd(DMA1_Stream2, ENABLE);
		return 1;
		// PWX新增*********************************************************************************************
	}
}

// PWX新增**************************************************************************
// DMA中断处理
void DMA1_Stream2_IRQHandler(void)
{
	int16_t rawData[3];
	if (DMA_GetITStatus(DMA1_Stream2, DMA_IT_TCIF2) == SET)
	{
		//传输完成
		I2C_GenerateSTOP(I2C2, ENABLE);
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
		DMA_Cmd(DMA1_Stream2, DISABLE);

		//通过标志位区别加速度、角速度、Temp
		if (IS_READ_ACC_FLAG)
		{
			rawData[0] = (I2C2_DMA_Rx_Buffer[0] << 8) | I2C2_DMA_Rx_Buffer[1];
			rawData[1] = (I2C2_DMA_Rx_Buffer[2] << 8) | I2C2_DMA_Rx_Buffer[3];
			rawData[2] = (I2C2_DMA_Rx_Buffer[4] << 8) | I2C2_DMA_Rx_Buffer[5];
			acc_iim42652.x = ((float)rawData[0]) * acc_raw_to_g;
			acc_iim42652.y = ((float)rawData[1]) * acc_raw_to_g;
			acc_iim42652.z = ((float)rawData[2]) * acc_raw_to_g;
		}
		else if (IS_READ_GYRO_FLAG)
		{
			rawData[0] = (I2C2_DMA_Rx_Buffer[0] << 8) | I2C2_DMA_Rx_Buffer[1];
			rawData[1] = (I2C2_DMA_Rx_Buffer[2] << 8) | I2C2_DMA_Rx_Buffer[3];
			rawData[2] = (I2C2_DMA_Rx_Buffer[4] << 8) | I2C2_DMA_Rx_Buffer[5];
			gyro_iim42652.x = ((float)rawData[0]) * gyro_raw_to_deg_s;
			gyro_iim42652.y = ((float)rawData[1]) * gyro_raw_to_deg_s;
			gyro_iim42652.z = ((float)rawData[2]) * gyro_raw_to_deg_s;
		}
		else if (IS_READ_TEMP_FLAG)
		{
			rawData[0] = (I2C2_DMA_Rx_Buffer[0] << 8) | I2C2_DMA_Rx_Buffer[1];
			//GyroTemp = ((double)(rawData[0]/132.48))+25;
			GyroTemp = ((s16)(rawData[0]/132))+25;
		}
	}

	if (DMA_GetITStatus(DMA1_Stream2, DMA_IT_TEIF2)) //传输错误
	{
		I2C_GenerateSTART(I2C2, ENABLE);
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TEIF2);
		DMA_Cmd(DMA1_Stream2, DISABLE);
	}
}

//事件中断处理
void I2C2_EV_IRQHandler()
{
	switch (I2C_GetLastEvent(I2C2))
	{
	case I2C_EVENT_MASTER_MODE_SELECT: // EV5
	{
		if (IS_IRQn_READ == 0)
		{
			I2C_Send7bitAddress(I2C2, IIM42652_SLAVE_ADDRESS, I2C_Direction_Transmitter);
		}
		else
		{
			I2C_Send7bitAddress(I2C2, IIM42652_SLAVE_ADDRESS, I2C_Direction_Receiver); //接收地址
			IS_IRQn_READ = 1;
		}
		break;
	}
	case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: // EV6
	{
		if (IS_IRQn_READ == 0)
		{
			I2C_SendData(I2C2, IRQn_REG_ADDR);
		}
		else
		{
			I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
			I2C_ITConfig(I2C2, I2C_IT_ERR, DISABLE);
			DMA_Cmd(DMA1_Stream2, ENABLE);
		}

		break;
	}
	case I2C_EVENT_MASTER_BYTE_TRANSMITTED: // EV8
	{
		I2C_GenerateSTART(I2C2, ENABLE); //产生IIC2传输START条件
		IS_IRQn_READ = 1;
		break;
	}

	default:
		break;
	}
}

//错误中断处理
void I2C2_ER_IRQHandler()
{
	if (I2C_GetITStatus(I2C2, I2C_IT_AF))
	{
		I2C_ClearITPendingBit(I2C2, I2C_IT_AF);
		I2C_GenerateSTOP(I2C2, ENABLE);
		//重启I2C操作
	}

	if (I2C_GetITStatus(I2C2, I2C_IT_BERR))
	{
		I2C_ClearITPendingBit(I2C2, I2C_IT_BERR);
		I2C_GenerateSTOP(I2C2, ENABLE);
		//重启I2C操作
	}

	if (I2C_GetITStatus(I2C2, I2C_IT_ARLO))
	{
		I2C_ClearITPendingBit(I2C2, I2C_IT_ARLO);
		I2C_GenerateSTOP(I2C2, ENABLE);
		//重启I2C操作
	}

	if (I2C_GetITStatus(I2C2, I2C_IT_TIMEOUT))
	{
		I2C_ClearITPendingBit(I2C2, I2C_IT_TIMEOUT);
		I2C_GenerateSTOP(I2C2, ENABLE);
		//重启I2C操作
	}
}

// PWX新增******************************************************************************************************
