#include "hardwareiic.h"
#include "iim42652.h"
#include "mpu6050.h"
#include "ExternPrototype.h"

extern float acc_param_k[3]; //增益
extern float acc_param_a[3]; //零偏
extern float gyro_offest_first[3];
extern u16 GyroCheck;
extern u16 GyroTime;
extern s16 GyroTemp;
extern s8 MPU6050_ACC_X;
extern s8 MPU6050_ACC_Y;
extern s8 MPU6050_ACC_Z;

iim42652_data acc_iim42652, gyro_iim42652;

/*
************************************************************
*	函数名称：	IIM42652_WriteReg
*
*	函数功能：	写数据到IIM42652寄存器
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void IIM42652_WriteReg(u8 reg_add, u8 reg_dat)
{
	I2C_ByteWrite(reg_dat, reg_add);
}

/*
************************************************************
*	函数名称：	IIM42652_WriteRegBits
*
*	函数功能：	对寄存器进行位操作
*
*	入口参数：	入口参数：reg_addr:寄存器地址	banknum:扇区  startbit:起始位(0-7) length：长度 modle：模式
*
*   关于startbit和length的理解：
*   bit7     bit6     bit5     bit4    bit3     bit2     bit1     bit0
*                                |    length     |
*								 |<------------->|
*								 |
*                             startbit
*	返回参数：	无
*
*	说明：
************************************************************
*/
void IIM42652_WriteRegBits(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	uint8_t ReadTemp = 0; //局部变量
	uint8_t mask = 0;
	IIM42652_SetRegisterBank(banknum);		   //选择寄存器扇区
	IIM42652_ReadData(reg_addr, &ReadTemp, 1); //读相关寄存器
	mask = ((1 << length) - 1) << (startbit - length + 1);
	modle <<= (startbit - length + 1);	   // shift data into correct position
	modle &= mask;						   // zero all non-important bits in data
	ReadTemp &= ~(mask);				   // zero all important bits in existing byte
	ReadTemp |= modle;					   // combine data with existing byte
	IIM42652_WriteReg(reg_addr, ReadTemp); // 设置寄存器的值
}

/*
************************************************************
*	函数名称：	IIM42652_IIMWriteReg
*
*	函数功能：	对指定的BANK的寄存器进行操作
*
*	入口参数：	入口参数：reg_addr:寄存器地址	banknum:扇区  modle：数据
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void IIM42652_IIMWriteReg(uint8_t reg_addr, uint8_t banknum, uint8_t modle)
{
	IIM42652_SetRegisterBank(banknum);	//选择寄存器扇区
	IIM42652_WriteReg(reg_addr, modle); // 设置寄存器的值
}

/*
************************************************************
*	函数名称：	IIM42652_ReadData
*
*	函数功能：	从IIM42652寄存器读取数据
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void IIM42652_ReadData(u8 reg_add, unsigned char *Read, u8 num)
{
	I2C_BufferRead(Read, reg_add, num);
}

void IIM42652_DMA_ReadData(u8 reg_add, unsigned char *Read, u8 num)
{
}

/*
************************************************************
*	函数名称：	IIM42652_Delay
*
*	函数功能：	for循环的延时函数
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void IIM42652_Delay(u16 outside, u16 inside)
{
	u16 i, j;
	for (i = 0; i < outside; i++)
	{
		for (j = 0; j < inside; j++)
		{
			; //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
		}
	}
}

/*
************************************************************
*	函数名称：	IIM42652_Init
*
*	函数功能：	初始化IIM42652芯片
*
*	入口参数：	无
*
*	返回参数：	0成功  其他值对应故障码
*
*	说明：
************************************************************
*/
uint8_t IIM42652_Init(void)
{
	uint16_t dummy16;
	uint8_t dummy8;

	// PWX新增*****************************************************************************************
	IIC_DMA_FLAG = 0; //初始化过程不适用DMA
	// PWX新增*****************************************************************************************

#pragma region 初始化IIM42652
	//关掉ACC,GRY,temp
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 1, 2, 00); //关闭加速度计
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 3, 2, 00); //关闭角速度计
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 5, 1, 1);  //关闭传感器
	//重启设备软件复位
	IIM42652_WriteReg(IIM42652_RA_DEVICE_CONFIG, 0x01); //重启设备

	IIM42652_Delay(200, 1000); //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错

	/* Timestamp */
	IIM42652_SetTMSFSyncEnable(REG_TMST_CONFIG, IIM42652_BANKNUM0, 1, 1, IIM_DISABLE);
	IIM42652_SetTMSDeltaMode(REG_TMST_CONFIG, IIM42652_BANKNUM0, 2, 1, IIM_ENABLE);
	/* APEX/DMP configuration */
	IIM42652_SetDMPPowerSaveMode(REG_APEX_CONFIG0, IIM42652_BANKNUM0, 7, 1, IIM_DISABLE);
	IIM42652_SetDMPodr(REG_APEX_CONFIG0, IIM42652_BANKNUM0, 1, 2, IIM42652_DMP_100Hz);

	//适当延时
	IIM42652_Delay(20, 50);

	//开启ACC,GRY,temp
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 1, 2, 03); //加速度计
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 3, 2, 03); //角速度计
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 5, 1, 0);  //传感器

	IIM42652_SetUserConfig();
#pragma endregion

	// PWX新增***********************************************************************************************
	//使用中断+DMA的方式
	IIC_DMA_FLAG = 1;  //初始化结束后使用DMA方式访问ACC,GYRO,TEMP
	IIC_IRQn_FLAG = 1; //
	// PWX新增***********************************************************************************************

	return 0; //自检配置成功
}

/*
************************************************************
*	函数名称：	MPU6050ReadID
*
*	函数功能：	读取MPU6050的ID
*
*	入口参数：	无
*
*	返回参数：	正常返回1，异常返回0
*
*	说明：
************************************************************
*/
uint8_t IIM42652ReadID(void)
{
	unsigned char Re = 0;
	IIM42652_SetRegisterBank(IIM42652_BANKNUM0);	 //选择BANK0
	IIM42652_ReadData(IIM42652_RA_WHO_AM_I, &Re, 1); //读相关寄存器
	if (Re != IIM42652_WHO_AM_I_ID)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
************************************************************
*	函数名称：	IIM42652_SetRegisterBank
*
*	函数功能：	选择相应寄存器的BANK
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：	正常返回1，异常返回0
*
*	说明：		Bank选择寄存器的地址为：0x76
************************************************************
*/
uint8_t IIM42652_SetRegisterBank(uint8_t banknum) //扇区选择函数
{
	uint8_t Bankdata = banknum;
	if (Bankdata > 4)
	{
		return 0;
	}
	IIM42652_WriteReg(IIM42652_REG_REG_BANK_SEL, Bankdata); //选择Bank区
	return 1;
}

/*
************************************************************
*	函数名称：	IIM42652_SetAccelMode
*
*	函数功能：	设置加速度计的模式
*
*	入口参数：	入口参数：reg_addr:寄存器地址	banknum:扇区  startbit:起始位(0-7) length：长度 modle：模式
*
*   关于startbit和length的理解：
*   bit7     bit6     bit5     bit4    bit3     bit2     bit1     bit0
*                                |    length     |
*								 |<------------->|
*								 |
*                             startbit
*
*	返回参数：	正常返回1，异常返回0
*
*	说明：
* 	PWR_MGMT0 Register bits
* 		MSB		6		5			4		3:2			1:0LSB
*		RES		RES		TEMP_DIS	IDLE	GYRO_MODE	ACCEL_MODE
*
*		TEMP_DIS：0b: Temperature sensor is enabled (default)   1b: Temperature sensor is disabled
*		IDLE：
*		GYRO_MODE：00b: Turns gyroscope off (default)  01b: Places gyroscope in Standby Mode
*				   10b: Reserved						  11b: Places gyroscope in Low Noise (LN) Mode
*
*       ACCEL_MODE：00b: Turns accelerometer off (default)   01b: Turns accelerometer on
*					10b: Places accelerometer in Low Power (LP) Mode
*                   11b: Places accelerometer in Low Noise (LN) Mode
************************************************************
*/
uint8_t IIM42652_SetAccel_Gyro_Temp_Mode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetTMSFSyncEnable
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：		modle: iim_enable iim_disable
************************************************************
*/
uint8_t IIM42652_SetTMSFSyncEnable(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetTMSDeltaMode
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：		modle: iim_enable iim_disable
************************************************************
*/
uint8_t IIM42652_SetTMSDeltaMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetDMPPowerSaveMode
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：		modle: iim_enable iim_disable
************************************************************
*/
uint8_t IIM42652_SetDMPPowerSaveMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetDMPPowerSaveMode
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：		modle: iim_enable iim_disable
************************************************************
*/
uint8_t IIM42652_SetDMPodr(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetUserConfig
*
*	函数功能：
*
*	入口参数：
*
*	返回参数：
*
*	说明：
************************************************************
*/
void IIM42652_SetUserConfig(void)
{
	//关掉ACC,GRY,temp
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 1, 2, 00); 				 //关闭加速度计
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 3, 2, 00); 				 //关闭角速度计
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 5, 1, 1);  				 //关闭传感器

	/* Sensors ODR and FSR */
	IIM42652_SetGyroFSR(REG_GYRO_CONFIG0, IIM42652_BANKNUM0, 7, 3, IIM42652_Gyroscope_250dps); 			 //陀螺仪量程：± 250 °/s
	IIM42652_SetGyroODR(REG_GYRO_CONFIG0, IIM42652_BANKNUM0, 3, 4, IIM42652_Gyroscope_50Hz);   			 //陀螺仪采样 输出 频 率50hz

	IIM42652_SetAccelFSR(REG_ACCEL_CONFIG0, IIM42652_BANKNUM0, 7, 3, IIM42652_Accelerometer_2G);   		 //加速度量程：± 2g
	IIM42652_SetAccelODR(REG_ACCEL_CONFIG0, IIM42652_BANKNUM0, 3, 4, IIM42652_Accelerometer_50Hz); 		 //加速度计采样 输出 频  率50hz

	/* Sensors BW */																					 //设置带宽
	IIM42652_SetGyroBW(REG_GYRO_ACCEL_CONFIG0, IIM42652_BANKNUM0, 3, 4, IIM42652_Gyroscope_ODR_20);		 //陀螺仪带宽设置20HZ
	IIM42652_SetAccelBW(REG_GYRO_ACCEL_CONFIG0, IIM42652_BANKNUM0, 7, 4, IIM42652_Accelerometer_ODR_20); //加速度计带宽设置20HZ

	/* WOM configuration */
	IIM42652_SetWomXTh(REG_ACCEL_WOM_X_THR, IIM42652_BANKNUM4, 128);
	IIM42652_SetWomYTh(REG_ACCEL_WOM_Y_THR, IIM42652_BANKNUM4, 128);
	IIM42652_SetWomZTh(REG_ACCEL_WOM_Z_THR, IIM42652_BANKNUM4, 128);

	//适当延时
	IIM42652_Delay(20, 20);

	//setwomintmode
	//IIM42652_SetWOMIntMode(REG_SMD_CONFIG,IIM42652_BANKNUM0,3 ,1,IMU_WOMIntMode_OR);
	//IIM42652_SetWOMMode(REG_SMD_CONFIG,IIM42652_BANKNUM0,2 ,1,IMU_WOMMode_Previous);
	//IIM42652_SetSMDMode(REG_SMD_CONFIG,IIM42652_BANKNUM0,1 ,2,IMU_SMDMode_WOM);

	//开启ACC,GRY,temp
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 1, 2, 03); //加速度计
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 3, 2, 03); //角速度计
	IIM42652_SetAccel_Gyro_Temp_Mode(IIM42652_REG_PWR_MGMT0, IIM42652_BANKNUM0, 5, 1, 0);  //度传感器

	IIM42652_SetRegisterBank(IIM42652_BANKNUM0); //选择BANK0扇区
}

/*
************************************************************
*	函数名称：	IIM42652_SetGyroFSR
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetGyroFSR(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetGyroODR
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetGyroODR(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}
/*
************************************************************
*	函数名称：	IIM42652_SetAccelFSR
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetAccelFSR(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}
/*
************************************************************
*	函数名称：	IIM42652_SetAccelODR
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetAccelODR(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetGyroBW
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetGyroBW(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetAccelBW
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetAccelBW(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetWOMIntMode
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetWOMIntMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}
/*
************************************************************
*	函数名称：	IIM42652_SetWOMMode
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetWOMMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}
/*
************************************************************
*	函数名称：	IIM42652_SetSMDMode
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetSMDMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle)
{
	IIM42652_WriteRegBits(reg_addr, banknum, startbit, length, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetWomXTh
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetWomXTh(uint8_t reg_addr, uint8_t banknum, uint8_t modle)
{
	IIM42652_IIMWriteReg(reg_addr, banknum, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetWomYTh
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetWomYTh(uint8_t reg_addr, uint8_t banknum, uint8_t modle)
{
	IIM42652_IIMWriteReg(reg_addr, banknum, modle);
}

/*
************************************************************
*	函数名称：	IIM42652_SetWomZTh
*
*	函数功能：
*
*	入口参数：	目标Bank(0-4)
*
*	返回参数：
*
*	说明：
************************************************************
*/
uint8_t IIM42652_SetWomZTh(uint8_t reg_addr, uint8_t banknum, uint8_t modle)
{
	IIM42652_IIMWriteReg(reg_addr, banknum, modle);
}

/*
************************************************************
*	函数名称：	IIM42652ReadAcc
*
*	函数功能：	读取IIM42652的加速度计数据
*
*	入口参数：	无
*
*	返回参数：
*
*	说明：
************************************************************
*/
void IIM42652ReadAcc(iim42652_data *pAcc)
{
	u8 buf[6];
	int16_t rawAccData[3];

	// PWX新增*****************************************************************
	if (IIC_DMA_FLAG)
	{
		IS_READ_ACC_FLAG = 1;
		IS_READ_GYRO_FLAG = 0;
		IS_READ_TEMP_FLAG = 0;
		IIM42652_ReadData(IIM42652_ACC_OUT, buf, 6);
	}
	// PWX新增*****************************************************************
	else
	{
		IIM42652_ReadData(IIM42652_ACC_OUT, buf, 6);
		rawAccData[0] = (buf[0] << 8) | buf[1];
		rawAccData[1] = (buf[2] << 8) | buf[3];
		rawAccData[2] = (buf[4] << 8) | buf[5];

		pAcc->x = ((float)rawAccData[0]) * acc_raw_to_g;
		pAcc->y = ((float)rawAccData[1]) * acc_raw_to_g;
		pAcc->z = ((float)rawAccData[2]) * acc_raw_to_g; 					//转换单位为g
	}

	// IIM42652_ReadData(IIM42652_ACC_OUT, buf, 2);
	// rawAccData[0] = (buf[0] << 8) | buf[1];
	// rawAccData[1] = (buf[2] << 8) | buf[3];
	// rawAccData[2] = (buf[4] << 8) | buf[5];

	// pAcc->x = ((float)rawAccData[0]) * acc_raw_to_g;
	// pAcc->y = ((float)rawAccData[1]) * acc_raw_to_g;
	// pAcc->z = ((float)rawAccData[2]) * acc_raw_to_g;      				//转换单位为g

#if 0
	pAcc->x = (acc_param_k[0] * pAcc->x + acc_param_a[0])*(-1.00);
	pAcc->y = (acc_param_k[1] * pAcc->y + acc_param_a[1])*(-1.00);
	pAcc->z = (acc_param_k[2] * pAcc->z + acc_param_a[2])*(-1.00);    		//校准
#endif

	// 	pAcc->x = (acc_param_k[0] * pAcc->x + acc_param_a[0]);
	// 	pAcc->y = (acc_param_k[1] * pAcc->y + acc_param_a[1]);
	// 	pAcc->z = (acc_param_k[2] * pAcc->z + acc_param_a[2]);    			//校准
}

/*
************************************************************
*	函数名称：	IIM42652ReadGyro
*
*	函数功能：	读取IIM42652的陀螺仪测量值
*
*	入口参数：	无
*
*	返回参数：
*
*	说明：
************************************************************
*/
void IIM42652ReadGyro(iim42652_data *pGyro)
{
	u8 buf[6];
	int16_t rawGyroData[3];

	// PWX新增***************************************************************
	if (IIC_DMA_FLAG)
	{
		IS_READ_ACC_FLAG = 0;
		IS_READ_GYRO_FLAG = 1;
		IS_READ_TEMP_FLAG = 0;
		IIM42652_ReadData(IIM42652_GYRO_OUT, buf, 6);
	}
	// PWX新增***************************************************************
	else
	{
		IIM42652_ReadData(IIM42652_GYRO_OUT, buf, 6);
		rawGyroData[0] = (buf[0] << 8) | buf[1];
		rawGyroData[1] = (buf[2] << 8) | buf[3];
		rawGyroData[2] = (buf[4] << 8) | buf[5];

		pGyro->x = ((float)rawGyroData[0]) * gyro_raw_to_deg_s;
		pGyro->y = ((float)rawGyroData[1]) * gyro_raw_to_deg_s;
		pGyro->z = ((float)rawGyroData[2]) * gyro_raw_to_deg_s;
	}

	// pGyro->x -= gyro_offest_first[0];
	// pGyro->y -= gyro_offest_first[1];
	// pGyro->z -= gyro_offest_first[2];
}

void IIM42652_ReturnTemp(short *Temperature)
{
	short temp3;
	u8 buf[2];

	// PWX新增********************************************************************
	if (IIC_DMA_FLAG) //使用DMA方式读取
	{
		IS_READ_ACC_FLAG = 0;
		IS_READ_GYRO_FLAG = 0;
		IS_READ_TEMP_FLAG = 1;
		IIM42652_ReadData(IIM42652_RA_TEMP_OUT_H, buf, 2); 
	}
	// PWX新增*********************************************************************
	else //普通轮询方式读取
	{
		IIM42652_ReadData(IIM42652_RA_TEMP_OUT_H, buf, 2); 
		temp3 = (buf[0] << 8) | buf[1];
		//*Temperature = (((double)(temp3 / 132.48)) + 25);
		*Temperature = (((s16)(temp3 / 132)) + 25);
	}
}

void IIM42652_Handle(void)
{
	s16 TempExchange = 0;

	if (GyroCheck == 0x5555)
	{
		gyro_offest_first[0] = ReadPara(GYRO_ANG_X_OFFSET);
		gyro_offest_first[1] = ReadPara(GYRO_ANG_Y_OFFSET);
		gyro_offest_first[2] = ReadPara(GYRO_ANG_Z_OFFSET);

		if (GyroTime % 5 == 0)
		{
			IIM42652ReadAcc(&acc_iim42652); 
		}
		
		if (GyroTime % 7 == 0)
		{
			IIM42652ReadGyro(&gyro_iim42652); 
		}

// 		ReadPara(GYRO_ACC_X) = -((s16)(acc_iim42652.x * 32768 / 4) - (s16)ReadPara(GYRO_ACC_X_OFFSET)); 	//PWX新增 反向 X轴加速度
// 		ReadPara(GYRO_ACC_Y) = -((s16)(acc_iim42652.y * 32768 / 4) - (s16)ReadPara(GYRO_ACC_Y_OFFSET)); 	//PWX新增 反向 Y轴加速度
// 		ReadPara(GYRO_ACC_Z) = +((s16)(acc_iim42652.z * 32768 / 4) - (s16)ReadPara(GYRO_ACC_Z_OFFSET));

		ReadPara(GYRO_ACC_X) = -((s16)(acc_iim42652.x * 100) - (s16)ReadPara(GYRO_ACC_X_OFFSET)); 			//PWX新增 反向 X轴加速度
		ReadPara(GYRO_ACC_Y) = -((s16)(acc_iim42652.y * 100) - (s16)ReadPara(GYRO_ACC_Y_OFFSET));			//PWX新增 反向 Y轴加速度
		ReadPara(GYRO_ACC_Z) = +((s16)(acc_iim42652.z * 100) - (s16)ReadPara(GYRO_ACC_Z_OFFSET));
		//acc_iim42652.x = 1.01g，ReadPara(GYRO_ACC_X) 放大100倍=101
		
// 		ReadPara(GYRO_ANG_X) = (s16)(gyro_iim42652.x * 131) - (s16)ReadPara(GYRO_ANG_X_OFFSET);
// 		ReadPara(GYRO_ANG_Y) = (s16)(gyro_iim42652.y * 131) - (s16)ReadPara(GYRO_ANG_Y_OFFSET);
// 		ReadPara(GYRO_ANG_Z) = (s16)(gyro_iim42652.z * 131) - (s16)ReadPara(GYRO_ANG_Z_OFFSET); 	  		//250*131=32750
		
		ReadPara(GYRO_ANG_X) = (s16)(gyro_iim42652.x * 100) - (s16)ReadPara(GYRO_ANG_X_OFFSET);
		ReadPara(GYRO_ANG_Y) = (s16)(gyro_iim42652.y * 100) - (s16)ReadPara(GYRO_ANG_Y_OFFSET);
		ReadPara(GYRO_ANG_Z) = (s16)(gyro_iim42652.z * 100) - (s16)ReadPara(GYRO_ANG_Z_OFFSET); 	  	
		//gyro_iim42652.x = 250°/s（Max），ReadPara(GYRO_ANG_X)放大100倍=25000
	}
	else
	{
		ReadPara(GYRO_ACC_X) = 100;
		ReadPara(GYRO_ACC_Y) = 100;
		ReadPara(GYRO_ACC_Z) = 100;

		ReadPara(GYRO_ANG_X) = 250;
		ReadPara(GYRO_ANG_Y) = 250;
		ReadPara(GYRO_ANG_Z) = 250;
	}
}