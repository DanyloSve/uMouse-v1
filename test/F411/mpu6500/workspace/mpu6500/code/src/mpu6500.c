/*
	SPI_2
	PB12 NSS  - slave select 
	PB13 SCK  - serial clock
	PB14 MISO - master in  / slave out 
	PB15 MOSI - master out / slave in
	
	PA8 - interrupt
	
	fSync - to GND
	
	Communication with all registers of the device 
	is performed using either I2C at 400kHz or SPI at 1MHz. 
	
	For applications requiring faster communications, 
	the sensor and interrupt registers may be read using SPI at 20MHz.
	
	SPI speed is set 50MHz / 64 == 0.78125MHz
	
	gyro max sesitivity when 250DPS
	accel max sensitivity when 2g
	
	Z-axis of gyro and accel is used
	
	output data rate = 5ms
	
	https://github.com/kriswiner/MPU9250/blob/master/MPU9250_BME280_SPIFlash_Ladybug/MPU9250.cpp#L427
*/

#include "mpu6500.h"

//=========REGISTERS========================================================>
//==========================================================================>
#define REG_SELF_TEST_X_ACCEL 0x0D
#define REG_SELF_TEST_Y_ACCEL 0x0E    
#define REG_SELF_TEST_Z_ACCEL 0x0F

#define REG_SELF_TEST_A      0x10

#define REG_XG_OFFSET_H      0x13  // User-defined trim values for gyroscope
#define REG_XG_OFFSET_L      0x14
#define REG_YG_OFFSET_H      0x15
#define REG_YG_OFFSET_L      0x16
#define REG_ZG_OFFSET_H      0x17
#define REG_ZG_OFFSET_L      0x18
#define REG_SMPLRT_DIV       0x19
#define REG_CONFIG           0x1A
#define REG_GYRO_CONFIG      0x1B
#define REG_ACCEL_CONFIG     0x1C
#define REG_ACCEL_CONFIG2    0x1D
#define REG_LP_ACCEL_ODR     0x1E   
#define REG_WOM_THR          0x1F   

#define REG_MOT_DUR          0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define REG_ZMOT_THR         0x21  // Zero-motion detection threshold bits [7:0]
#define REG_ZRMOT_DUR        0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms

#define REG_FIFO_EN          0x23

#define REG_INT_PIN_CFG      0x37
#define REG_INT_ENABLE       0x38
#define REG_DMP_INT_STATUS   0x39  // Check DMP interrupt
#define REG_INT_STATUS       0x3A
#define REG_ACCEL_XOUT_H     0x3B
#define REG_ACCEL_XOUT_L     0x3C
#define REG_ACCEL_YOUT_H     0x3D
#define REG_ACCEL_YOUT_L     0x3E
#define REG_ACCEL_ZOUT_H     0x3F
#define REG_ACCEL_ZOUT_L     0x40
#define REG_TEMP_OUT_H       0x41
#define REG_TEMP_OUT_L       0x42
#define REG_GYRO_XOUT_H      0x43
#define REG_GYRO_XOUT_L      0x44
#define REG_GYRO_YOUT_H      0x45
#define REG_GYRO_YOUT_L      0x46
#define REG_GYRO_ZOUT_H      0x47
#define REG_GYRO_ZOUT_L      0x48

#define REG_SIGNAL_PATH_RESET  0x68
#define REG_MOT_DETECT_CTRL  0x69
#define REG_USER_CTRL        0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define REG_PWR_MGMT_1       0x6B // Device defaults to the SLEEP mode
#define REG_PWR_MGMT_2       0x6C
#define REG_DMP_BANK         0x6D  // Activates a specific bank in the DMP
#define REG_DMP_RW_PNT       0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define REG_DMP_REG          0x6F  // Register in DMP from which to read or to which to write
#define REG_DMP_REG_1        0x70
#define REG_DMP_REG_2        0x71 
#define REG_FIFO_COUNTH      0x72
#define REG_FIFO_COUNTL      0x73
#define REG_FIFO_R_W         0x74
#define REG_WHO_AM_I_MPU9250 0x75 // Should return 0x71
#define REG_XA_OFFSET_H      0x77
#define REG_XA_OFFSET_L      0x78
#define REG_YA_OFFSET_H      0x7A
#define REG_YA_OFFSET_L      0x7B
#define REG_ZA_OFFSET_H      0x7D
#define REG_ZA_OFFSET_L      0x7E

// Set initial input parameters
#define FLAG_READ 0x80

#define  AFS_2G  0
#define  AFS_4G  1
#define  AFS_8G  2
#define  AFS_16G 3

#define  GFS_250DPS  0
#define  GFS_500DPS  1
#define  GFS_1000DPS 2
#define  GFS_2000DPS 3

static volatile uint8_t gyroScale;
static volatile uint8_t accelScale;

static volatile uint8_t rawData[14]; // x/y/z accel register data stored here

static volatile float   accelBias[3];
volatile float   gyroBias[3];
float DEBUG_DATA_OUT[64];

void MPU6500_Init(uint8_t gScale, uint8_t aScale)
{	
	delay_us(50000);
	delay_us(50000);
	
	gyroScale = gScale;
	accelScale = aScale;
	
	PowerUp();
	
	Gyro_Init();
	Accel_Init();
	
	Interrupt_Init();
	calibrate();
	
	EXTI_Init();
	
}


/*
	//////////////////////////////////////////////////////
	Power up
	//////////////////////////////////////////////////////
*/
#define CMD_DEVICE_RESET 0x80
#define CMD_MPU_PATH_RST 0x07
#define CMD_I2C_IF_DIS   0x08
#define CMD_CLKSEL_BEST_CLK_SRC 0x01

#define CMD_DLPF_CFG_3 0x03
#define CMD_RESET_FIFO 0x04
#define CMD_RESET_DMP  0x0

#define CMD_FIFO_EN 0x40

void PowerUp(void)
{
	// MPU reset
	SPI2_ReadWriteReg(REG_PWR_MGMT_1, CMD_DEVICE_RESET);	
	delay_us(50000);
	delay_us(50000);
	delay_us(50000);
	delay_us(50000);
	
	
  // GYRO/ACCEL/TEMP path reset
	SPI2_ReadWriteReg(REG_SIGNAL_PATH_RESET, CMD_MPU_PATH_RST);
	delay_us(50000);
	delay_us(50000);
	delay_us(50000);
	delay_us(50000);
	
	
	// set SPI 
	SPI2_ReadWriteReg(REG_USER_CTRL, CMD_I2C_IF_DIS);
	delay_us(50000);
	delay_us(50000);
	
	// MPU wake up
	SPI2_ReadWriteReg(REG_PWR_MGMT_1, 0x00);
	delay_us(50000);
	delay_us(50000);
	delay_us(50000);
	delay_us(50000);
	
	// auto select best available clock source
	SPI2_ReadWriteReg(REG_PWR_MGMT_1, CMD_CLKSEL_BEST_CLK_SRC );
	delay_us(50000);
	delay_us(50000);

	
	// Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively; 
	// DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
	// Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
	SPI2_ReadWriteReg(REG_CONFIG, CMD_DLPF_CFG_3);
	delay_us(50000);
	delay_us(50000);
	
//	// Set up FIFO for gyro and accelerometer
//	// disable FIFO 
//	SPI2_ReadWriteReg(REG_FIFO_EN, 0x00);
//	delay_us(50000);
//	
//	// reset DMP & FIFO
//	SPI2_ReadWriteReg(REG_USER_CTRL, CMD_I2C_IF_DIS|CMD_RESET_DMP|CMD_RESET_FIFO);
//	delay_us(50000);
//	delay_us(50000);
//	
//	
//	// FIFO en 0 Z-gyro & accel
//	SPI2_ReadWriteReg(REG_FIFO_EN, 0x18);
//	delay_us(50000);
//	
//	// fifo en
//	SPI2_ReadWriteReg(REG_USER_CTRL, CMD_I2C_IF_DIS|CMD_FIFO_EN);
//	delay_us(50000);
//	delay_us(50000);

}



/*
	//////////////////////////////////////////////////////////
	Gyro initialization
	//////////////////////////////////////////////////////////
*/

#define CMD_GYRO_FS_SEL_250DPS 0x00
#define CMD_GYRO_FS_SEL_500DPS 0x08
#define CMD_GYRO_FS_SEL_1000DPS 0x10
#define CMD_GYRO_FS_SEL_2000DPS 0x18

void Gyro_Init(void)
{	
	// Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
	// Use a 200 Hz rate; the same rate set in CONFIG above
	SPI2_ReadWriteReg(REG_SMPLRT_DIV, 0x04);
	delay_us(50000);
	delay_us(50000);
	
	// Set gyroscope full scale range
	SPI2_ReadWriteReg(REG_GYRO_CONFIG, CMD_GYRO_FS_SEL_250DPS);	
	delay_us(50000);
	delay_us(50000);
}

#define MEASURMENTS 50
void calibrate(void)
{	
		//After a quick chip reload, these registers may be 
	// not completely erased
	// thus it should be manualy done
//	SPI2_ReadWriteReg(REG_XG_OFFSET_H, 0x00);
//	delay_us(10000);
//  SPI2_ReadWriteReg(REG_XG_OFFSET_L, 0x00);
//  delay_us(10000);
//	SPI2_ReadWriteReg(REG_YG_OFFSET_H, 0x00);
//  delay_us(10000);
//	SPI2_ReadWriteReg(REG_YG_OFFSET_L, 0x00);
//  delay_us(10000);
	SPI2_ReadWriteReg(REG_ZG_OFFSET_H, 0x00);
  delay_us(10000);
	SPI2_ReadWriteReg(REG_ZG_OFFSET_L, 0x00);
	delay_us(10000);
	
//+
	
	
	
	
	int32_t gyro_bias  = 0;
	int32_t accel_bias[3]  = {0, 0, 0};
	
	int j = 0;
	
	for (int i = 0; i < MEASURMENTS; i++)
	{
		 //int16_t accel_temp[3] = {0, 0, 0};
				 
		while ( !(GPIOA->IDR & GPIO_IDR_IDR_8))
		{
				// wait untill interrupt
		}
		SPI2_BurstRead(FLAG_READ|REG_ACCEL_XOUT_H, &(rawData[0]), 6);
		int16_t gyro_temp  =  0;
		int16_t accel_temp[3]  = {0, 0, 0};
		
		accel_temp[0] = (int16_t) (((int16_t)rawData[0] << 8) | rawData[1]  ) ;
    accel_temp[1] = (int16_t) (((int16_t)rawData[2] << 8) | rawData[3]  ) ;
		accel_temp[2] = (int16_t) (((int16_t)rawData[4] << 8) | rawData[5]  ) ;
		
		accel_bias[0]  += (int32_t) accel_temp[0];
    accel_bias[1]  += (int32_t) accel_temp[1];
    accel_bias[2]  += (int32_t) accel_temp[2];
		
    //gyro_temp = (int16_t) (((int16_t)rawData[8] << 8)  | rawData[9]  ) ;
    
		//gyro_bias  += (int32_t) gyro_temp;
		
		//DEBUG_DATA_OUT[j++] = (int32_t) accel_temp;
	}
	
	
	gyro_bias  /= (int32_t) MEASURMENTS;
	
	accel_bias[0]  /= (int32_t) MEASURMENTS;
	accel_bias[1]  /= (int32_t) MEASURMENTS;
	accel_bias[2]  /= (int32_t) MEASURMENTS;
	
	gyroBias[0] = ((float)gyro_bias); // construct gyro bias in deg/s for later manual subtraction

	uint8_t data[12]; //  gyro x, y, z, data
	// Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
  data[0] = (-gyro_bias/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
  data[1] = (-gyro_bias/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
//  data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
//  data[3] = (-gyro_bias[1]/4)       & 0xFF;
//  data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
//  data[5] = (-gyro_bias[2]/4)       & 0xFF;

//	delay_us(50000);
//	SPI2_ReadWriteReg(REG_XG_OFFSET_H, data[0]);
//	delay_us(50000);

//  SPI2_ReadWriteReg(REG_XG_OFFSET_L, data[1]);
//  delay_us(50000);

//	SPI2_ReadWriteReg(REG_YG_OFFSET_H, data[2]);
//  delay_us(50000);

//	SPI2_ReadWriteReg(REG_YG_OFFSET_L, data[3]);
//  delay_us(50000);

	SPI2_ReadWriteReg(REG_ZG_OFFSET_H, data[4]);
  delay_us(50000);

	SPI2_ReadWriteReg(REG_ZG_OFFSET_L, data[5]);
	delay_us(50000);
	
	// Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
// factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
// non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
// compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
// the accelerometer biases calculated above must be divided by 8.

//  int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
//	data[0] = SPI2_ReadWriteReg(FLAG_READ  | REG_XA_OFFSET_H, 0x00); // Read factory accelerometer trim values
//	delay_us(10000);
//	data[1] = SPI2_ReadWriteReg(FLAG_READ  | REG_XA_OFFSET_L, 0x00);
//  delay_us(10000);
//	accel_bias_reg[0] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
//  
//	data[0] = SPI2_ReadWriteReg(FLAG_READ  | REG_YA_OFFSET_H, 0x00); // Read factory accelerometer trim values
//	delay_us(10000);
//	data[1] = SPI2_ReadWriteReg(FLAG_READ  | REG_YA_OFFSET_L, 0x00);
//  delay_us(10000);
//	accel_bias_reg[1] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
//	
//  data[0] = SPI2_ReadWriteReg(FLAG_READ  | REG_ZA_OFFSET_H, 0x00); // Read factory accelerometer trim values
//	delay_us(10000);
//	data[1] = SPI2_ReadWriteReg(FLAG_READ  | REG_ZA_OFFSET_L, 0x00);
//  delay_us(10000);
//	accel_bias_reg[2] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
//  
//	 uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g
//	
//	 if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}  // Remove gravity from the z-axis accelerometer bias calculation
//  else {accel_bias[2] += (int32_t) accelsensitivity;}
//  uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
//  uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis
//  
//  for(int i = 0; i < 3; i++) 
//	{
//    if((accel_bias_reg[i] & mask)) mask_bit[i] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
//  }
//	
//	// Construct total accelerometer bias, including calculated average accelerometer bias from above
//  accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
//  accel_bias_reg[1] -= (accel_bias[1]/8);
//  accel_bias_reg[2] -= (accel_bias[2]/8);
	
//	data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
//  data[1] = (accel_bias_reg[0])      & 0xFF;
//  data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
//  data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
//  data[3] = (accel_bias_reg[1])      & 0xFF;
//  data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
//  data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
//  data[5] = (accel_bias_reg[2])      & 0xFF;
//  data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers
// 
	DEBUG_DATA_OUT[0] = (float)accel_bias[0]; 
  DEBUG_DATA_OUT[1] = (float)accel_bias[1];
  DEBUG_DATA_OUT[2] = (float)accel_bias[2];
}




/*
	//////////////////////////////////////////////////////////
	Accelerometer initialization
	//////////////////////////////////////////////////////////
*/

#define CMD_ACCEL_FS_SEL_2G 0x00
#define CMD_ACCEL_FS_SEL_4G 0x08
#define CMD_ACCEL_FS_SEL_8G 0x10
#define CMD_ACCEL_FS_SEL_16G 0x18

void Accel_Init(void)
{
	// Set accelerometer sample rate configuration
	// It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
	// accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
	SPI2_ReadWriteReg(REG_ACCEL_CONFIG2, CMD_ACCEL_FS_SEL_2G);
	delay_us(50000);
	delay_us(50000);
	
	// Set accelerometer sample rate configuration
	// It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
	// accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
	SPI2_ReadWriteReg(REG_ACCEL_CONFIG2, 0x03);
	delay_us(50000);
	delay_us(50000);
}


/*
	//////////////////////////////////////////////////////////
	Interrupt initialization
	//////////////////////////////////////////////////////////
*/

#define CMD_INT_ANYRD_2CLEAR 0x01
#define CMD_LATCH_INT_EN     0x02

#define CMD_DATA_READY 0x01

void EXTI1_Init(void);
void EXTI1_IRQHandler(void);

void Interrupt_Init(void)
{
	// STM32 configuarion
	//=================================================================
	// PA8 MPU6500 interrupt GPIO input
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER   &= ~GPIO_MODER_MODER8;
	GPIOA->PUPDR   &= ~GPIO_PUPDR_PUPDR8;
	
	EXTI_Init();
	
	
	// MPU configuration
	//=================================================================
	// Configure Interrupts and Bypass Enable
	
  // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS
	// INT pin level held until interrupt status is cleared
	
	SPI2_ReadWriteReg(REG_INT_PIN_CFG, 0x10);
	delay_us(50000);
	delay_us(50000);
	
	// Enable data ready (bit 0) interrupt
	SPI2_ReadWriteReg(REG_INT_ENABLE, CMD_DATA_READY);
	delay_us(50000);
	delay_us(50000);
}

void EXTI_Init(void)
{
	// PA8 
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PA;
	EXTI->RTSR |= EXTI_RTSR_TR8; // rising trigger
	EXTI->PR |= EXTI_PR_PR8; 		 // clearing interrupt bit 
	
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	EXTI->IMR |= EXTI_IMR_MR8;
}

void EXTI9_5_IRQHandler(void)
{	
		GPIOC->ODR ^= GPIO_ODR_ODR_13;
		SPI2_BurstRead(FLAG_READ|REG_ACCEL_XOUT_H, &(rawData[0]), 6);
		EXTI->PR |= EXTI_PR_PR8;	
}

float getGyroResolution(uint8_t gScale)
{
	// Possible gyro scales (and their register bit settings) are:
  // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
	switch (gScale)
  {
  // Possible gyro scales (and their register bit settings) are:
  // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
    case GFS_250DPS:  return  250.0f/32768.0f;
    case GFS_500DPS:  return  500.0f/32768.0f;
    case GFS_1000DPS: return  1000.0f/32768.0f;
    case GFS_2000DPS: return 	2000.0f/32768.0f;
  }
}

float getAccelResolution(uint8_t aScale)
{
	switch (aScale)
  {
  // Possible accelerometer scales (and their register bit settings) are:
  // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case AFS_2G:  return 2.0f/32768.0f;
    case AFS_4G:  return 4.0f/32768.0f;
    case AFS_8G:  return 8.0f/32768.0f;
    case AFS_16G: return 16.0f/32768.0f; 
  }
}

float *MPU6500_GetGyroBias(void)
{
		return (float *)&(gyroBias[0]);
}

void MPU6500_Read(float gyroData[3], float accelData[3])
{
//	accelData[0] = (float)( ((uint16_t)rawData[0] << 8) | (uint16_t)rawData[1]) * getAccelResolution(AFS_2G);
//	accelData[1] = (float)( ((uint16_t)rawData[2] << 8) | (uint16_t)rawData[3]) * getAccelResolution(AFS_2G);
//	accelData[2] = (float)( ((uint16_t)rawData[4] << 8) | (uint16_t)rawData[5]) * getAccelResolution(AFS_2G);
//	// 6 - 7 termperature
//	gyroData[0] = (((float)( ((uint16_t)rawData[ 8] << 8) | ((uint16_t)rawData[ 9])) )  / 131.0f);
//	gyroData[1] = (((float)( ((uint16_t)rawData[10] << 8) | ((uint16_t)rawData[11])) ) / 131.0f);
//	gyroData[2] = (((float)( ((uint16_t)rawData[12] << 8) | ((uint16_t)rawData[13])) ) / 131.0f);
	
		int16_t gyro_temp[3];
	  gyro_temp[0] = ((int16_t)rawData[0] << 8) | rawData[1];  // Form signed 16-bit integer for each sample in FIFO
    gyro_temp[1] = ((int16_t)rawData[2] << 8) | rawData[3];
    gyro_temp[2] = ((int16_t)rawData[4] << 8) | rawData[5]; 
	
	gyroData[0] = (float)gyro_temp[0];
	gyroData[1] = (float)gyro_temp[1];
	gyroData[2] = (float)gyro_temp[2];
}
