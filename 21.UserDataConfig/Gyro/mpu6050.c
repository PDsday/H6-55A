#include "hardwareiic.h"
#include "mpu6050.h"
#include "ExternPrototype.h"
//#include "LevenbergMarquardt.h"
#include "mathTool.h"

float acc_param_k[3] = {0.9960,1.0052,0.9715};					//增益
//float acc_param_a[3] = {-0.0001,-0.0048,0.1661};				//零偏
float acc_param_a[3] = {-0.0001,-0.0048,-0.0140};				//新增零偏校准零偏
float gyro_offest_first[3] = {0,0,0};

s16 GyroTemp = 0;
u8 GyroCode =0;	
s8 MPU6050_ACC_X=0;
s8 MPU6050_ACC_Y=0;
s8 MPU6050_ACC_Z=0;

mpu6050_data acc,gyro;
mpu6050_seltest seltest_data;	
									                                        
extern u16  GyroCheck;
extern u16  GyroTime;

/*
************************************************************
*函数名称：MPU6050_WriteReg
*
*函数功能：写数据到MPU6050寄存器
*
*入口参数：无
*
*返回参数：无
*
*说明：
*************************************************************/
void MPU6050_WriteReg(u8 reg_add,u8 reg_dat)
{
    I2C_ByteWrite(reg_dat,reg_add);
}

/*
************************************************************
*	函数名称：	MPU6050_ReadData
*
*	函数功能：	从MPU6050寄存器读取数据
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：
************************************************************
*/
void MPU6050_ReadData(u8 reg_add,unsigned char* Read,u8 num)
{
    I2C_BufferRead(Read,reg_add,num);
}

uint8_t MPU6050_Init(void)
{
	;
}

/*
************************************************************
*	函数名称：	MPU6050Hardware_Selftest
*
*	函数功能：	
*
*	入口参数：	
*
*	返回参数：	
*
*	说明：		加速度计量程：±2g   陀螺仪量程：± 250 °/s
************************************************************
*/
uint8_t MPU6050Hardware_Selftest(void)
{
	;
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
uint8_t MPU6050ReadID(void)
{
    unsigned char Re = 0;
    MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);    //读器件地址
    if(Re != 0x68)
    {
        //printf("MPU6050 dectected error!\r\n检测不到MPU6050模块，请检查模块与开发板的接线");
        return 0;
    }
    else
    {
        //printf("MPU6050 ID = %d\r\n",Re);
        return 1;
    }

}


/*************************************************************
*	函数名称：	MPU6050ReadAcc
*
*	函数功能：	读取MPU6050的加速度计数据，寄存器59-64
*
*	入口参数：	无
*
*	返回参数：
*
*	说明：
*************************************************************/
void MPU6050ReadAcc(mpu6050_data *pAcc)
{
    u8 buf[6];
    int16_t rawAccData[3];

    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    rawAccData[0] = (buf[0] << 8) | buf[1];
    rawAccData[1] = (buf[2] << 8) | buf[3];
    rawAccData[2] = (buf[4] << 8) | buf[5];

    pAcc->x = ((float)rawAccData[0]) * acc_raw_to_g;
    pAcc->y = ((float)rawAccData[1]) * acc_raw_to_g;
    pAcc->z = ((float)rawAccData[2]) * acc_raw_to_g;      				//转换单位为g

#if 0
    pAcc->x = (acc_param_k[0] * pAcc->x + acc_param_a[0])*(-1.00);
    pAcc->y = (acc_param_k[1] * pAcc->y + acc_param_a[1])*(-1.00);
    pAcc->z = (acc_param_k[2] * pAcc->z + acc_param_a[2])*(-1.00);    	//校准
#endif

    pAcc->x = (acc_param_k[0] * pAcc->x + acc_param_a[0]);
    pAcc->y = (acc_param_k[1] * pAcc->y + acc_param_a[1]);
    pAcc->z = (acc_param_k[2] * pAcc->z + acc_param_a[2]);    			//校准


}

/*
************************************************************
*	函数名称：	MPU6050ReadGyro
*
*	函数功能：	读取MPU6050的陀螺仪测量值，寄存器67--72
*
*	入口参数：
*
*	返回参数：
*
*	说明：
************************************************************
*/
void MPU6050ReadGyro(mpu6050_data *pGyro)
{
    u8 buf[6];
    int16_t rawGyroData[3];

    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    rawGyroData[0] = (buf[0] << 8) | buf[1];
    rawGyroData[1] = (buf[2] << 8) | buf[3];
    rawGyroData[2] = (buf[4] << 8) | buf[5];

    pGyro->x = ((float)rawGyroData[0]) * gyro_raw_to_deg_s;
    pGyro->y = ((float)rawGyroData[1]) * gyro_raw_to_deg_s;
    pGyro->z = ((float)rawGyroData[2]) * gyro_raw_to_deg_s;

    pGyro->x -= gyro_offest_first[0];
    pGyro->y -= gyro_offest_first[1];
    pGyro->z -= gyro_offest_first[2];
}

/*
************************************************************
*	函数名称：	MPU6050ReadTemp
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
void MPU6050ReadTemp(short *tempData)
{
    u8 buf[2];
    MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2); 
    *tempData = (buf[0] << 8) | buf[1];
}

/*
************************************************************
*	函数名称：	MPU6050_ReturnTemp
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
void MPU6050_ReturnTemp(short*Temperature)
{
    short temp3;
    u8 buf[2];

    MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2); 
    temp3= (buf[0] << 8) | buf[1];
    *Temperature=(((double) (temp3 + 13200)) / 280)-13;
}

void MPU6050_Handle(void)
{
	;
}
