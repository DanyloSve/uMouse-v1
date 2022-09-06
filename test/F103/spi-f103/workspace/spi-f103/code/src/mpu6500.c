#include "mpu6500.h"
/*
//------------------------REGs------------------------>
#define MPU6500_REG_USR_CNRL          106
#define MPU6500_REG_PWR_MGMT_1        107
#define MPU6500_REG_SIGNAL_PATH_RESET 104
#define MPU6500_REG_SMPLRT_DIV        25
#define MPU6500_REG_CONFIG						26
#define MPU6500_REG_GYRO_CONFIG				27
#define MPU6500_REG_ACCEL_CONFIG			28

#define MPU6500_REG_ACCEL_XOUT_H			59
#define MPU6500_REG_ACCEL_XOUT_L			60

#define MPU6500_REG_ACCEL_YOUT_H			61
#define MPU6500_REG_ACCEL_YOUT_L			62

#define MPU6500_REG_ACCEL_ZOUT_H			63
#define MPU6500_REG_ACCEL_ZOUT_L			64

#define MPU6500_REG_GYRO_XOUT_H				67
#define MPU6500_REG_GYRO_XOUT_L				68

#define MPU6500_REG_GYRO_YOUT_H				69
#define MPU6500_REG_GYRO_YOUT_L				70

#define MPU6500_REG_GYRO_ZOUT_H				71
#define MPU6500_REG_GYRO_ZOUT_L				72

//------------------------CMDs------------------------->
#define MPU6500_CMD_READ              	0x80

#define MPU6500_CMD_I2C_IF_DIS        	0x10
#define MPU6500_CMD_DEVICE_RESET      	0x08

#define MPU6500_CMD_TEMP_RST          	0x01
#define MPU6500_CMD_ACCEL_RST         	0x02
#define MPU6500_CMD_GYRO_RST          	0x04

#define MPU6500_CMD_CLKSEL_0          	0x00
#define MPU6500_CMD_DLPF_CFG_0         	0x01
#define MPU6500_CMD_DLPF_CFG_1        	0x02
#define MPU6500_CMD_GYRO_FS_SEL_2000DPS 0x18

#define MPU6500_CMD_ACCEL_FS_SEL_16G    0x18	
#define MPU6500_CMD_ACCEL_FS_SEL_8G			0x10
#define MPU6500_CMD_ACCEL_FS_SEL_4G			0x08
//------------------------------------------------------->



void MPU6500_Init(void)
{
	delay_ms(100);
	
	SPI1_Init();
		
	// device reset
	sendCommand(MPU6500_REG_PWR_MGMT_1, MPU6500_CMD_DEVICE_RESET);
	delay_ms(100);
	sendCommand(MPU6500_REG_SIGNAL_PATH_RESET, MPU6500_CMD_TEMP_RST | MPU6500_CMD_ACCEL_RST | MPU6500_CMD_GYRO_RST);
	delay_ms(100);
	
	
	// change from i2c to spi
	sendCommand(MPU6500_REG_USR_CNRL, MPU6500_CMD_I2C_IF_DIS);
	delay_ms(1);
	
	// internal 20MHz oscillator
	sendCommand(MPU6500_REG_PWR_MGMT_1, MPU6500_CMD_CLKSEL_0);
	delay_ms(1);
	
	// Sample Rate - 1kHz
	sendCommand(MPU6500_REG_SMPLRT_DIV, 0x00);
	delay_ms(1);
	
	// digital low pass filter = DLPF 1kHz
	sendCommand(MPU6500_REG_CONFIG, MPU6500_CMD_DLPF_CFG_0 | MPU6500_CMD_DLPF_CFG_1);
	delay_ms(1);
	
	// gyroscope configuration 2000dps
	sendCommand(MPU6500_REG_GYRO_CONFIG, MPU6500_CMD_GYRO_FS_SEL_2000DPS);
	delay_ms(1);
	
	// accelerometer configuration +- 8g
	sendCommand(MPU6500_REG_ACCEL_CONFIG, 0x10);
	delay_ms(1);
}

void sendCommand(uint8_t reg, uint8_t data)
{
	SPI1_TransmitReceiveByte(reg);
	SPI1_TransmitReceiveByte(data);
}

void MPU6500_ReadData(void)
{
	MPU6500.accelX = SPI1_TransmitReceiveByte(MPU6500_CMD_READ | MPU6500_REG_ACCEL_XOUT_L);
	MPU6500.accelX = SPI1_TransmitReceiveByte(MPU6500_CMD_READ | MPU6500_REG_ACCEL_XOUT_H) << 8;

	MPU6500.accelY = SPI1_TransmitReceiveByte(MPU6500_CMD_READ | MPU6500_REG_ACCEL_YOUT_L);
	MPU6500.accelY = SPI1_TransmitReceiveByte(MPU6500_CMD_READ | MPU6500_REG_ACCEL_YOUT_H) << 8;
	
	MPU6500.gyroZ = SPI1_TransmitReceiveByte(MPU6500_CMD_READ | MPU6500_REG_GYRO_ZOUT_L);
	MPU6500.gyroZ = SPI1_TransmitReceiveByte(MPU6500_CMD_READ | MPU6500_REG_GYRO_ZOUT_H)   << 8;
}
*/
