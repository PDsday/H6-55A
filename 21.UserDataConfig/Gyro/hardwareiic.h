#ifndef _HARDWAREIIC_H_
#define _HARDWAREIIC_H_
#include "stm32f4xx_conf.h"

//#define macI2Cx     I2C1		//
#define macI2Cx I2C2 // 2023-12-29

#define I2CTIMEOUT 18000 //

#define I2C_MAX_DEAD_TIME 60 //  0.18ms

// PWX新增***********************************************************
#define USE_DMA1_STREAM2 1
#define NONE_USE_DMA1_STREAM2 0
#define USE_TRQn 1
#define NONE_USE_IRQn 0

extern u16 ErrorTimes[6];
// PWX新增***********************************************************

void I2C_Configuration(void);

void GPIO_ConfigurationI2C(void);

uint8_t I2C_ByteWrite(u8 pBuffer, u8 WriteAddr);

void I2C_WaitStandbyState(void);

uint8_t I2C_BufferRead(u8 *pBuffer, u8 ReadAddr, u16 NumByteToRead);

void MPU6050_I2c_Bus_Init(void);

// PWX新增***********************************************************
void I2C_DMA_Rx_Init(void);
void I2C_DMA_Recvice(u8 *pBuffer, u8 Rx_Len);
u8 IIC_DMA_ReadData(u8 *pBuffer, u8 ReadAddr, u16 NumByteToRead);

extern u8 IIC_IRQn_FLAG;
extern u8 IS_READ_ACC_FLAG;
extern u8 IS_READ_GYRO_FLAG;
extern u8 IS_READ_TEMP_FLAG;
extern u8 IIC_DMA_FLAG;

// PWX新增***********************************************************
#endif
