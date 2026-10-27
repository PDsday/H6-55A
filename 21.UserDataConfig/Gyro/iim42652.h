#ifndef _IIM42652_H
#define _IIM42652_H

#include "stdio.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"

#define IIM_ENABLE 0X01
#define IIM_DISABLE 0X00

//=============�Ĵ����궨��====================================
// #define IIM42652_ADDRESS         	 0x68 //�豸��ַ
#define IIM42652_SLAVE_ADDRESS 0xd0 // 0xD0 //MPU6050��������ַ

#define IIM42652_RA_WHO_AM_I 0x75 //��ѯ�������ڷ��ʵ��豸�ĵ�ַ
#define IIM42652_WHO_AM_I_ID 0x6F

#define IIM42652_RA_DEVICE_CONFIG 0X11 //��������λ����������
#define IIM42652_REG_REG_BANK_SEL 0X76 // Bankѡ��ļĴ�����ַ
#define IIM42652_REG_PWR_MGMT0 0X4E	   //���ü��ٶȼƣ����ٶȣ��¶�ģʽ���üĴ�����ַ
#define REG_ACCEL_CONFIG_STATIC2 0x03
#define REG_TMST_CONFIG 0x54
#define REG_APEX_CONFIG0 0x56
#define REG_GYRO_CONFIG0 0x4F
#define REG_ACCEL_CONFIG0 0x50
#define REG_GYRO_ACCEL_CONFIG0 0x52
#define REG_ACCEL_WOM_X_THR 0x4A
#define REG_ACCEL_WOM_Y_THR 0x4B
#define REG_ACCEL_WOM_Z_THR 0x4C
#define REG_SMD_CONFIG 0x57

#define IIM42652_GYRO_OUT 0x25 // MPU6050���������ݼĴ�����ַ
#define IIM42652_ACC_OUT 0x1F  // MPU6050���ٶȼ����ݼĴ�����ַ

#define IIM42652_RA_TEMP_OUT_H 0x1D //�¶�ֵ�߰�λ�Ĵ���
#define IIM42652_RA_TEMP_OUT_L 0x1E //�¶�ֵ��8λ�Ĵ���

#define IIM42652_RA_ACCEL_XOUT_H 0x1F //���ٶ�ֵ,X���8λ�Ĵ���
#define IIM42652_RA_ACCEL_XOUT_L 0x20 //���ٶ�ֵ,X���8λ�Ĵ���
#define IIM42652_RA_ACCEL_YOUT_H 0x21 //���ٶ�ֵ,Y���8λ�Ĵ���
#define IIM42652_RA_ACCEL_YOUT_L 0x22 //���ٶ�ֵ,Y���8λ�Ĵ���
#define IIM42652_RA_ACCEL_ZOUT_H 0x23 //���ٶ�ֵ,Z���8λ�Ĵ���
#define IIM42652_RA_ACCEL_ZOUT_L 0x24 //���ٶ�ֵ,Z���8λ�Ĵ���

#define IIM42652_RA_GYRO_XOUT_H 0x25 //������ֵ,X���8λ�Ĵ���
#define IIM42652_RA_GYRO_XOUT_L 0x26 //������ֵ,X���8λ�Ĵ���
#define IIM42652_RA_GYRO_YOUT_H 0x27 //������ֵ,Y���8λ�Ĵ���
#define IIM42652_RA_GYRO_YOUT_L 0x28 //������ֵ,Y���8λ�Ĵ���
#define IIM42652_RA_GYRO_ZOUT_H 0x29 //������ֵ,Z���8λ�Ĵ���
#define IIM42652_RA_GYRO_ZOUT_L 0x2A //������ֵ,Z���8λ�Ĵ���

//======�����궨��==================================================
#define IIM42652_DMP_25Hz 0x00
#define IIM42652_DMP_500Hz 0x01
#define IIM42652_DMP_50Hz 0x02 /* Default */
#define IIM42652_DMP_100Hz 0x03

#define IIM42652_BANKNUM0 0X00
#define IIM42652_BANKNUM1 0X01
#define IIM42652_BANKNUM2 0X02
#define IIM42652_BANKNUM3 0X03
#define IIM42652_BANKNUM4 0X04

#define USER_DATA_VALID_VALUE 0xAC
#define IIM42652_Accelerometer_32kHz 0x01 /*!< ODR is 32kHz */
#define IIM42652_Accelerometer_16kHz 0x02
#define IIM42652_Accelerometer_8kHz 0x03	 /* Require LN mode */
#define IIM42652_Accelerometer_4kHz 0x04	 /* Require LN mode */
#define IIM42652_Accelerometer_2kHz 0x05	 /* Require LN mode */
#define IIM42652_Accelerometer_1kHz 0x06	 /* Default value, require LN mode */
#define IIM42652_Accelerometer_200Hz 0x07	 /* Require LN or LP mode */
#define IIM42652_Accelerometer_100Hz 0x08	 /* Require LN or LP mode */
#define IIM42652_Accelerometer_50Hz 0x09	 /* Require LN or LP mode */
#define IIM42652_Accelerometer_25Hz 0x0A	 /* Require LN or LP mode */
#define IIM42652_Accelerometer_12_5Hz 0x0B	 /* Require LN or LP mode */
#define IIM42652_Accelerometer_6_25Hz 0x0C	 /* Require LP mode */
#define IIM42652_Accelerometer_3_125Hz 0x0D	 /* Require LP mode */
#define IIM42652_Accelerometer_1_5625Hz 0x0E /* Require LP mode */
#define IIM42652_Accelerometer_500Hz 0x0F	 /* Require LN or LP mode */

#define IIM42652_Accelerometer_16G 0x00 /*!< Range is +- 16G */
#define IIM42652_Accelerometer_8G 0x01
#define IIM42652_Accelerometer_4G 0x02
#define IIM42652_Accelerometer_2G 0x03

#define IIM42652_Gyroscope_32kHz 0x01 /*!< ODR is 32kHz */
#define IIM42652_Gyroscope_16kHz 0x02
#define IIM42652_Gyroscope_8kHz 0x03
#define IIM42652_Gyroscope_4kHz 0x04
#define IIM42652_Gyroscope_2kHz 0x05
#define IIM42652_Gyroscope_1kHz 0x06 /* Default value */
#define IIM42652_Gyroscope_200Hz 0x07
#define IIM42652_Gyroscope_100Hz 0x08
#define IIM42652_Gyroscope_50Hz 0x09
#define IIM42652_Gyroscope_25Hz 0x0A
#define IIM42652_Gyroscope_12_5Hz 0x0B
#define IIM42652_Gyroscope_500Hz 0x0F

#define IIM42652_Gyroscope_2000dps 0x00 /*!< Range is +- 2000degree/sec */
#define IIM42652_Gyroscope_1000dps 0x01
#define IIM42652_Gyroscope_500dps 0x02
#define IIM42652_Gyroscope_250dps 0x03
#define IIM42652_Gyroscope_125dps 0x04
#define IIM42652_Gyroscope_62_5dps 0x05
#define IIM42652_Gyroscope_31_25dps 0x06
#define IIM42652_Gyroscope_15_625dps 0x07

#define IIM42652_Gyroscope_ODR_2 0x00
#define IIM42652_Gyroscope_ODR_4 0x01 /* Default value */
#define IIM42652_Gyroscope_ODR_5 0x02
#define IIM42652_Gyroscope_ODR_8 0x03
#define IIM42652_Gyroscope_ODR_10 0x04
#define IIM42652_Gyroscope_ODR_16 0x05
#define IIM42652_Gyroscope_ODR_20 0x06
#define IIM42652_Gyroscope_ODR_40 0x07
#define IIM42652_Gyroscope_ODR_LL 0x0E
#define IIM42652_Gyroscope_ODR_LLx8 0x0F

#define IIM42652_Accelerometer_ODR_2 0x00
#define IIM42652_Accelerometer_ODR_4 0x01 /* Default value */
#define IIM42652_Accelerometer_ODR_5 0x02
#define IIM42652_Accelerometer_ODR_8 0x03
#define IIM42652_Accelerometer_ODR_10 0x04
#define IIM42652_Accelerometer_ODR_16 0x05
#define IIM42652_Accelerometer_ODR_20 0x06
#define IIM42652_Accelerometer_ODR_40 0x07
#define IIM42652_Accelerometer_ODR_LL 0x0E
#define IIM42652_Accelerometer_ODR_LLx8 0x0F

#define IMU_WOMIntMode_OR 0x00
#define IMU_WOMIntMode_AND 0x01

#define IMU_WOMMode_Initial 0x00
#define IMU_WOMMode_Previous 0x01

#define IMU_SMDMode_Disabled 0x00
#define IMU_SMDMode_WOM 0x01
#define IMU_SMDMode_WOM_short 0x02 /* two WOM are detected 1 sec apart */
#define IMU_SMDMode_WOM_long 0x03  /* two WOM are detected 3 sec apart */

typedef struct
{
	float x;
	float y;
	float z;
} iim42652_data;

uint8_t IIM42652_Init(void);
uint8_t IIM42652ReadID(void);

void IIM42652_SetUserConfig(void);
void IIM42652_WriteReg(u8 reg_add, u8 reg_dat);
void IIM42652_Delay(u16 outside, u16 inside);
void IIM42652_WriteRegBits(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
void IIM42652_IIMWriteReg(uint8_t reg_addr, uint8_t banknum, uint8_t modle);

void IIM42652_ReadData(u8 reg_add, unsigned char *Read, u8 num);

uint8_t IIM42652_SetRegisterBank(uint8_t banknum);
uint8_t IIM42652_SetAccel_Gyro_Temp_Mode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);

uint8_t IIM42652_SetTMSFSyncEnable(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
uint8_t IIM42652_SetTMSDeltaMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
uint8_t IIM42652_SetDMPPowerSaveMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
uint8_t IIM42652_SetDMPodr(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);

uint8_t IIM42652_SetGyroFSR(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
uint8_t IIM42652_SetGyroODR(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
uint8_t IIM42652_SetAccelFSR(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
uint8_t IIM42652_SetAccelODR(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);

uint8_t IIM42652_SetGyroBW(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
uint8_t IIM42652_SetAccelBW(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);

uint8_t IIM42652_SetWomXTh(uint8_t reg_addr, uint8_t banknum, uint8_t modle);
uint8_t IIM42652_SetWomYTh(uint8_t reg_addr, uint8_t banknum, uint8_t modle);
uint8_t IIM42652_SetWomZTh(uint8_t reg_addr, uint8_t banknum, uint8_t modle);

uint8_t IIM42652_SetWOMIntMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
uint8_t IIM42652_SetWOMMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);
uint8_t IIM42652_SetSMDMode(uint8_t reg_addr, uint8_t banknum, uint8_t startbit, uint8_t length, uint8_t modle);

void IIM42652ReadAcc(iim42652_data *pAcc);

void IIM42652ReadGyro(iim42652_data *pGyro);

void IIM42652_ReturnTemp(short *Temperature);

extern iim42652_data acc_iim42652;
extern iim42652_data gyro_iim42652;
extern s16 gMPU6050_Temp;
#endif
