#include "mpu6500.h"

/*
	This library was written with 
	https://github.com/kriswiner/MPU9250/blob/master/STM32F401/MPU9250.h
*/

//------------------------REGs------------------------>
#define MPU6500_REG_USR_CNRL          106
#define MPU6500_REG_PWR_MGMT_1        107
#define MPU6500_REG_PWR_MGMT_2				108
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

#define MPU6500_REG_WHOAMI  					117

#define MPU6500_REG_INT_EN						56
#define MPU6500_REG_FIFO_EN						35
#define MPU6500_REG_USER_CTRL					106

#define MPU6500_REG_FIFO_COUNT_H			114
#define MPU6500_REG_FIFO_COUNT_L			115
#define MPU6500_REG_FIFO_R_W					116

#define MPU6500_REG_XG_OFFS_H					19			
#define MPU6500_REG_XG_OFFS_L					20
#define MPU6500_REG_YG_OFFS_H					21
#define MPU6500_REG_YG_OFFS_L					22
#define MPU6500_REG_ZG_OFFS_H					23
#define MPU6500_REG_ZG_OFFS_L					24
//------------------------CMDs------------------------->
#define MPU6500_CMD_READ              	0x80

#define MPU6500_CMD_I2C_IF_DIS        	0x10
#define MPU6500_CMD_DEVICE_RESET      	0x80

#define MPU6500_CMD_TEMP_RST          	0x01
#define MPU6500_CMD_ACCEL_RST         	0x02
#define MPU6500_CMD_GYRO_RST          	0x04

#define MPU6500_CMD_CLKSEL_0          	0x00
#define MPU6500_CMD_CLKSEL_1          	0x01

#define MPU6500_CMD_DLPF_CFG_0         	0x01
#define MPU6500_CMD_DLPF_CFG_1        	0x02

#define MPU6500_CMD_GYRO_FS_SEL_250PS   0x00
#define MPU6500_CMD_GYRO_FS_SEL_500DPS  0x08
#define MPU6500_CMD_GYRO_FS_SEL_1000DPS 0x10
#define MPU6500_CMD_GYRO_FS_SEL_2000DPS 0x18

#define MPU6500_CMD_ACCEL_FS_SEL_2G			0x00
#define MPU6500_CMD_ACCEL_FS_SEL_4G			0x08
#define MPU6500_CMD_ACCEL_FS_SEL_8G			0x10
#define MPU6500_CMD_ACCEL_FS_SEL_16G    0x18	
			

//------------------------------------------------------->
#define MPU6500_GYRO_SENS_SCALE_FACTOR_250DPS 				((float)250.0/32768.0;)
#define MPU6500_GYRO_SENS_SCALE_FACTOR_500DPS 				((float)500.0/32768.0)
#define MPU6500_GYRO_SENS_SCALE_FACTOR_1000DPS 				((float)1000.0/32768.0)
#define MPU6500_GYRO_SENS_SCALE_FACTOR_2000DPS 				((float)2000.0/32768.0)

#define MPU6500_ACCEL_SENS_SCALE_FACTOR_2G 						((float)2.0/32768.0)
#define MPU6500_ACCEL_SENS_SCALE_FACTOR_4G	  				((float)4.0/32768.0)
#define MPU6500_ACCEL_SENS_SCALE_FACTOR_8G	 					((float)8.0/32768.0)
#define MPU6500_ACCEL_SENS_SCALE_FACTOR_16G			 			((float)16.0/32768.0)


static int16_t accelX;
static int16_t accelY;
static int16_t gyroZ;
static int16_t gyroY;

static float gyroBias[3];

void MPU6500_Init(void)
{
	delay_ms(100);
	
	SPI1_Init();
		
	// device reset
	/*
	sendCommand(MPU6500_REG_PWR_MGMT_1, MPU6500_CMD_DEVICE_RESET);
	delay_ms(100);
	sendCommand(MPU6500_REG_SIGNAL_PATH_RESET, MPU6500_CMD_TEMP_RST | MPU6500_CMD_ACCEL_RST | MPU6500_CMD_GYRO_RST);
	delay_ms(100);
	*/
	
	// change from i2c to spi
	sendCommand(MPU6500_REG_USR_CNRL, MPU6500_CMD_I2C_IF_DIS);
	delay_ms(1);
	
	calibarate();
	
	// internal 20MHz oscillator
	sendCommand(MPU6500_REG_PWR_MGMT_1, 0x00);
	delay_ms(1);
	
	sendCommand(MPU6500_REG_PWR_MGMT_2, 0x00);
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

uint8_t sendCommand(uint8_t reg, uint8_t data)
{

	data = SPI1_TransmitReceiveByte( (reg << 8) | data);
	//data = SPI1_TransmitReceiveByte(data);

	return data;	
}

uint8_t	MPU6500_TestRead(void)
{
	return sendCommand(MPU6500_CMD_READ | MPU6500_REG_WHOAMI, 0x00);
}

void MPU6500_ReadData(void)
{
	/*
	accelX  = sendCommand(MPU6500_CMD_READ | MPU6500_REG_ACCEL_XOUT_L, 0x00);
	accelX |= sendCommand(MPU6500_CMD_READ | MPU6500_REG_ACCEL_XOUT_H, 0x00) << 8;

	accelY  = sendCommand(MPU6500_CMD_READ | MPU6500_REG_ACCEL_YOUT_L, 0x00);
	accelY |= sendCommand(MPU6500_CMD_READ | MPU6500_REG_ACCEL_YOUT_H, 0x00) << 8;
	*/
	gyroY  	= (int16_t)((int16_t)sendCommand(MPU6500_CMD_READ | MPU6500_REG_GYRO_YOUT_H, 0x00)  << 8 
						| sendCommand(MPU6500_CMD_READ | MPU6500_REG_GYRO_YOUT_L, 0x00));
	
	gyroZ  	= (int16_t)((int16_t)sendCommand(MPU6500_CMD_READ | MPU6500_REG_GYRO_ZOUT_H, 0x00)  << 8 
						| sendCommand(MPU6500_CMD_READ | MPU6500_REG_GYRO_ZOUT_L, 0x00));
}

float MPU6500_AccelX(void)
{
	return accelX;
}

float MPU6500_AccelY(void)
{
	return accelY;
}

float MPU6500_GyroZ(void)
{
	return ((float)gyroZ) * MPU6500_GYRO_SENS_SCALE_FACTOR_2000DPS - gyroBias[2];
}

float MPU6500_GyroY(void)
{
	return ((float)gyroY) * MPU6500_GYRO_SENS_SCALE_FACTOR_2000DPS - gyroBias[1];
}


void calibarate(void)
{
	uint8_t  data[12]; // data array to hold accelerometer and gyro x, y, z, data

	uint16_t packet_count;
	uint16_t fifo_count;
	int32_t  gyro_bias[3]  = {0, 0, 0};
	int32_t  accel_bias[3] = {0, 0, 0};
	
	// reset
	sendCommand(MPU6500_REG_PWR_MGMT_1, MPU6500_CMD_DEVICE_RESET);
	delay_ms(100);
	
	// SPI
	sendCommand(MPU6500_REG_USR_CNRL, MPU6500_CMD_I2C_IF_DIS);
	delay_ms(1);
	
	// get stable time source; Auto select clock source to be PLL gyroscope reference if ready
	sendCommand(MPU6500_REG_PWR_MGMT_1, MPU6500_CMD_CLKSEL_1); // Auto selects the best available clock source 
	sendCommand(MPU6500_REG_PWR_MGMT_2, 0x00); // enable sensors
	delay_ms(200);
	
	// Configure device for bias calculation
	sendCommand(MPU6500_REG_INT_EN,     0x00);
	sendCommand(MPU6500_REG_FIFO_EN,    0x00); // Turn on internal clock source
	sendCommand(MPU6500_REG_PWR_MGMT_1, 0x00);
	sendCommand(MPU6500_REG_USER_CTRL,  0x00);
	sendCommand(MPU6500_REG_USER_CTRL,  0x0C);
	delay_ms(15);
	
	// Configure MPU6050 gyro and accelerometer for bias calculation
	sendCommand(MPU6500_REG_CONFIG,       0x01);
	sendCommand(MPU6500_REG_SMPLRT_DIV,   0x00);
	sendCommand(MPU6500_REG_GYRO_CONFIG,  0x00);
	sendCommand(MPU6500_REG_ACCEL_CONFIG, 0x00);
	
	uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
  uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g
	
	// Configure FIFO to capture accelerometer and gyro data for bias calculation
	sendCommand(MPU6500_REG_USER_CTRL, 0x40);
	sendCommand(MPU6500_REG_FIFO_EN,   0x78);
	delay_ms(40);
	
	// At end of sample accumulation, turn off FIFO sensor read
	sendCommand(MPU6500_REG_FIFO_EN, 0x00);
	
	data[0] = (uint16_t)(sendCommand(MPU6500_REG_FIFO_COUNT_L, 0x00));
	data[1] = (uint16_t)(sendCommand(MPU6500_REG_FIFO_COUNT_H, 0x00));
	fifo_count = ((uint16_t)data[1] << 8) | ((uint16_t)data[1]);
	packet_count = fifo_count / 12;
	
	for (uint32_t i = 0; i < packet_count; i++)
	{
		int16_t accel_temp[3] = {0, 0, 0};
		int16_t gyro_temp[3]  = {0, 0, 0};
		
		SPI1_ReceiveBytes(MPU6500_REG_FIFO_R_W, data, 12);
		
		accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO    
		accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
    accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;    
    
		gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
    gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
    gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;
        
    accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
    accel_bias[1] += (int32_t) accel_temp[1];
    accel_bias[2] += (int32_t) accel_temp[2];
    
		gyro_bias[0]  += (int32_t) gyro_temp[0];
    gyro_bias[1]  += (int32_t) gyro_temp[1];
    gyro_bias[2]  += (int32_t) gyro_temp[2];
	}
	
	accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
  accel_bias[1] /= (int32_t) packet_count;
  accel_bias[2] /= (int32_t) packet_count;
  gyro_bias[0]  /= (int32_t) packet_count;
  gyro_bias[1]  /= (int32_t) packet_count;
  gyro_bias[2]  /= (int32_t) packet_count;
	
	if(accel_bias[2] > 0L) 
	{
		accel_bias[2] -= (int32_t) accelsensitivity; // Remove gravity from the z-axis accelerometer bias calculation
	}  
  else 
	{
		accel_bias[2] += (int32_t) accelsensitivity;
	}
   
  // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
  data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
  data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
  data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
  data[3] = (-gyro_bias[1]/4)       & 0xFF;
  data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
  data[5] = (-gyro_bias[2]/4)       & 0xFF;
	
	// Push gyro biases to hardware registers
	/*
	sendCommand(MPU6500_REG_XG_OFFS_H, data[0]);
  sendCommand(MPU6500_REG_XG_OFFS_L, data[1]);
  sendCommand(MPU6500_REG_YG_OFFS_H, data[2]);
  sendCommand(MPU6500_REG_YG_OFFS_L, data[3]);
  sendCommand(MPU6500_REG_ZG_OFFS_H, data[4]);
  sendCommand(MPU6500_REG_ZG_OFFS_L, data[5]);
	*/
	
	gyroBias[0] = (float) gyro_bias[0]/(float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
  gyroBias[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
  gyroBias[2] = (float) gyro_bias[2]/(float) gyrosensitivity;
}
