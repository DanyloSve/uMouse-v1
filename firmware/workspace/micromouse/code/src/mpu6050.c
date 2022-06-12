#include "mpu6050.h"

#define MPU6050_XGOFFS_TC         0x00 // Bit 7 PWR_MODE, bits 6:1 XG_OFFS_TC, bit 0 OTP_BNK_VLD                 
#define MPU6050_YGOFFS_TC         0x01                                                                          
#define MPU6050_ZGOFFS_TC         0x02
#define MPU6050_X_FINE_GAIN       0x03 // [7:0] fine gain
#define MPU6050_Y_FINE_GAIN       0x04
#define MPU6050_Z_FINE_GAIN       0x05
#define MPU6050_XA_OFFSET_H       0x06 // User-defined trim values for accelerometer
#define MPU6050_XA_OFFSET_L_TC    0x07
#define MPU6050_YA_OFFSET_H       0x08
#define MPU6050_YA_OFFSET_L_TC    0x09
#define MPU6050_ZA_OFFSET_H       0x0A
#define MPU6050_ZA_OFFSET_L_TC    0x0B
#define MPU6050_SELF_TEST_X       0x0D
#define MPU6050_SELF_TEST_Y       0x0E    
#define MPU6050_SELF_TEST_Z       0x0F
#define MPU6050_SELF_TEST_A       0x10
#define MPU6050_XG_OFFS_USRH      0x13  // User-defined trim values for gyroscope; supported in MPU-6050?
#define MPU6050_XG_OFFS_USRL      0x14
#define MPU6050_YG_OFFS_USRH      0x15
#define MPU6050_YG_OFFS_USRL      0x16
#define MPU6050_ZG_OFFS_USRH      0x17
#define MPU6050_ZG_OFFS_USRL      0x18
#define MPU6050_SMPLRT_DIV        0x19
#define MPU6050_CONFIG            0x1A
#define MPU6050_GYRO_CONFIG       0x1B
#define MPU6050_ACCEL_CONFIG      0x1C
#define MPU6050_FF_THR            0x1D  // Free-fall
#define MPU6050_FF_DUR            0x1E  // Free-fall
#define MPU6050_MOT_THR           0x1F  // Motion detection threshold bits [7:0]
#define MPU6050_MOT_DUR           0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define MPU6050_ZMOT_THR          0x21  // Zero-motion detection threshold bits [7:0]
#define MPU6050_ZRMOT_DUR         0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms
#define MPU6050_FIFO_EN           0x23
#define MPU6050_I2C_MST_CTRL      0x24   
#define MPU6050_I2C_SLV0_ADDR     0x25
#define MPU6050_I2C_SLV0_REG      0x26
#define MPU6050_I2C_SLV0_CTRL     0x27
#define MPU6050_I2C_SLV1_ADDR     0x28
#define MPU6050_I2C_SLV1_REG      0x29
#define MPU6050_I2C_SLV1_CTRL     0x2A
#define MPU6050_I2C_SLV2_ADDR     0x2B
#define MPU6050_I2C_SLV2_REG      0x2C
#define MPU6050_I2C_SLV2_CTRL     0x2D
#define MPU6050_I2C_SLV3_ADDR     0x2E
#define MPU6050_I2C_SLV3_REG      0x2F
#define MPU6050_I2C_SLV3_CTRL     0x30
#define MPU6050_I2C_SLV4_ADDR     0x31
#define MPU6050_I2C_SLV4_REG      0x32
#define MPU6050_I2C_SLV4_DO       0x33
#define MPU6050_I2C_SLV4_CTRL     0x34
#define MPU6050_I2C_SLV4_DI       0x35
#define MPU6050_I2C_MST_STATUS    0x36
#define MPU6050_INT_PIN_CFG       0x37
#define MPU6050_INT_ENABLE        0x38
#define MPU6050_DMP_INT_STATUS    0x39  // Check DMP interrupt
#define MPU6050_INT_STATUS        0x3A
#define MPU6050_ACCEL_XOUT_H      0x3B
#define MPU6050_ACCEL_XOUT_L      0x3C
#define MPU6050_ACCEL_YOUT_H      0x3D
#define MPU6050_ACCEL_YOUT_L      0x3E
#define MPU6050_ACCEL_ZOUT_H      0x3F
#define MPU6050_ACCEL_ZOUT_L      0x40
#define MPU6050_TEMP_OUT_H        0x41
#define MPU6050_TEMP_OUT_L        0x42
#define MPU6050_GYRO_XOUT_H       0x43
#define MPU6050_GYRO_XOUT_L       0x44
#define MPU6050_GYRO_YOUT_H       0x45
#define MPU6050_GYRO_YOUT_L       0x46
#define MPU6050_GYRO_ZOUT_H       0x47
#define MPU6050_GYRO_ZOUT_L       0x48
#define MPU6050_EXT_SENS_DATA_00  0x49
#define MPU6050_EXT_SENS_DATA_01  0x4A
#define MPU6050_EXT_SENS_DATA_02  0x4B
#define MPU6050_EXT_SENS_DATA_03  0x4C
#define MPU6050_EXT_SENS_DATA_04  0x4D
#define MPU6050_EXT_SENS_DATA_05  0x4E
#define MPU6050_EXT_SENS_DATA_06  0x4F
#define MPU6050_EXT_SENS_DATA_07  0x50
#define MPU6050_EXT_SENS_DATA_08  0x51
#define MPU6050_EXT_SENS_DATA_09  0x52
#define MPU6050_EXT_SENS_DATA_10  0x53
#define MPU6050_EXT_SENS_DATA_11  0x54
#define MPU6050_EXT_SENS_DATA_12  0x55
#define MPU6050_EXT_SENS_DATA_13  0x56
#define MPU6050_EXT_SENS_DATA_14  0x57
#define MPU6050_EXT_SENS_DATA_15  0x58
#define MPU6050_EXT_SENS_DATA_16  0x59
#define MPU6050_EXT_SENS_DATA_17  0x5A
#define MPU6050_EXT_SENS_DATA_18  0x5B
#define MPU6050_EXT_SENS_DATA_19  0x5C
#define MPU6050_EXT_SENS_DATA_20  0x5D
#define MPU6050_EXT_SENS_DATA_21  0x5E
#define MPU6050_EXT_SENS_DATA_22  0x5F
#define MPU6050_EXT_SENS_DATA_23  0x60
#define MPU6050_MOT_DETECT_STATUS 0x61
#define MPU6050_I2C_SLV0_DO       0x63
#define MPU6050_I2C_SLV1_DO       0x64
#define MPU6050_I2C_SLV2_DO       0x65
#define MPU6050_I2C_SLV3_DO       0x66
#define MPU6050_I2C_MST_DELAY_CTRL 0x67
#define MPU6050_SIGNAL_PATH_RESET  0x68
#define MPU6050_MOT_DETECT_CTRL   0x69
#define MPU6050_USER_CTRL         0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define MPU6050_PWR_MGMT_1        0x6B // Device defaults to the SLEEP mode
#define MPU6050_PWR_MGMT_2        0x6C
#define MPU6050_DMP_BANK          0x6D  // Activates a specific bank in the DMP
#define MPU6050_DMP_RW_PNT        0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define MPU6050_DMP_REG           0x6F  // Register in DMP from which to read or to which to write
#define MPU6050_DMP_MPU6050_1     0x70
#define MPU6050_DMP_MPU6050_2     0x71
#define MPU6050_FIFO_COUNTH       0x72
#define MPU6050_FIFO_COUNTL       0x73
#define MPU6050_FIFO_R_W          0x74
#define MPU6050_WHO_AM_I_MPU6050  0x75 // Should return 0x68


/*
	Notes:
	
	Gyro and accel should be configured due to speed of turn
	gyro  scale  250 DPS
	accel scale    2 G
	
	int active high, push-pull
	
	PB5 - INTA 
	PB6 - DRDY impulse!!!!
	
	Freq = 100Hz
	
*/

static void Interrupt_Init       (void);
static void GPIO_Init            (void);

static void MPU6050_Calibration  (void);

static uint8_t rawDataMPU6050  [6];
static int16_t gyroBias        [3];

#define DEBUG_MODE
#if defined(DEBUG_MODE)
	volatile int16_t MPU6050_debugArr[6];
#endif /*DEBUG_MODE*/

/*
	----------------------------------------------------------
	MPU6050 SetBits
	----------------------------------------------------------
	NOTES:
	Function, that is used for setting particular bits in register  WITH keeping others bits setted.
*/
static inline void SetBits(const uint8_t addr, const uint8_t reg, const uint8_t cmd)
{
	I2C1_WriteReg(addr, reg, (I2C1_ReadReg(addr, reg) | cmd));
}

/*
	----------------------------------------------------------
	MPU6050 SetBits
	----------------------------------------------------------
	NOTES:
	Function, that is used for clearing particular bits in register WITH keeping others bits setted.
*/
static inline void ClearBits(const uint8_t addr, const uint8_t reg, const uint8_t cmd)
{
	I2C1_WriteReg(addr, reg, (I2C1_ReadReg(addr, reg) & (~cmd) ));
}

/*
	//////////////////////////////////////////////////////////
	MPU initialization
	//////////////////////////////////////////////////////////
	
	NOTES:
	Function
		void     I2C1_WriteReg(const uint8_t addr, const uint8_t reg, const uint8_t data) 
	should be used only in MPU6050_Init().

	In order to avoid clearing important bits by mistake, function 
		static inline void MPU6050_SetBits(uint8_t reg, uint8_t cmd)
	should be used to set particular bits in the sensor.

	In order to make library portable, data should be send by using pointer to I2Cx_WriteReg(...) function. 
	
	FIFO is used in order not to read temperature data while burst read
*/

#define CMD_DEVICE_RESET   0x80
#define CMD_GYRO_RESET     0x01
#define CMD_ACCEL_RESET    0x02
#define CMD_TEMP_RESET     0x04

#define CMD_FSYNC_ALL      0x38
#define CMD_GYRO_SCALE_250 0x00
#define CMD_ACCEL_SCALE_2  0x00

#define CMD_XG_FIFO_EN  	 0x40
#define CMD_YG_FIFO_EN  	 0x20
#define CMD_ZG_FIFO_EN  	 0x10
#define CMD_ACCEL_FIFO_EN  0x08

#define CMD_FIFO_EN 0x40

#define CMD_DATA_RDY_EN 0x01

void MPU6050_Init(void)
{	
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_PWR_MGMT_1,        CMD_DEVICE_RESET); 
	
	delay_us(50000);
	delay_us(50000);
	
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_SIGNAL_PATH_RESET, CMD_GYRO_RESET  | 
																														 CMD_ACCEL_RESET | 
																														 CMD_TEMP_RESET); 
	
	delay_us(50000);
	delay_us(50000);
	
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_PWR_MGMT_1,   0x00); // clear sleep mode
	
	delay_us(50000);
	delay_us(50000);
	
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_PWR_MGMT_1,   0x03); // pll z-gyro
	delay_us(50000);
	
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_CONFIG,       0x03); // 4.9 smapling delay
	
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_SMPLRT_DIV,   0x04); // 1kHz / (SMPLRT_DIV + 1) = 200 Hz rate
				
	// gyro scale 2000DPS
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_GYRO_CONFIG,  0x18);
	
	// accel scale 2G
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_ACCEL_CONFIG, CMD_ACCEL_SCALE_2);
	
	
	Interrupt_Init();
	
	SetBits(MPU6050_I2C_ADDR, MPU6050_INT_ENABLE, CMD_DATA_RDY_EN);
	

	// FIFO - gyro and accel enable
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_FIFO_EN,   CMD_XG_FIFO_EN | 
																									   CMD_YG_FIFO_EN | 
																									   CMD_ZG_FIFO_EN);
	MPU6050_Calibration();
	
	I2C1_WriteReg(MPU6050_I2C_ADDR, MPU6050_USER_CTRL, CMD_FIFO_EN);
}

/*
	----------------------------------------------------------
	Interrupt initialization
	----------------------------------------------------------
	NOTES:
	This function  configures  STM32/MPU6050/HMC588L interrupt
	
*/
#define CMD_INT_LEVEL_L           0x80
#define CMD_INT_OPEN              0x40
#define CMD_LATCH_INT_EN          0x20
#define CMD_INT_RD_CLEAR_ANY_READ 0x10
#define CMD_FSYNC_INT_EN          0x04
#define CMD_I2C_BYPASS_EN         0x02



void Interrupt_Init(void)
{
	// interrupt config	
	SetBits(MPU6050_I2C_ADDR, MPU6050_INT_PIN_CFG, CMD_LATCH_INT_EN          | 
																								 CMD_INT_RD_CLEAR_ANY_READ | 
																								 CMD_I2C_BYPASS_EN);
	
	GPIO_Init();
	
}



/*
	----------------------------------------------------------
	GPIO initialization
	----------------------------------------------------------
*/
void GPIO_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	
	
	// INTA PB5 MPU6050
	// input
	//--------------------------------->
	GPIOB->MODER &= ~GPIO_MODER_MODER5; // input
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR5;
	
}


/*
	----------------------------------------------------------
	MPU6050 Calibration
	----------------------------------------------------------
	Interrupt should be enabled both in MPU and in STM32, to calibrate MPU6050
	
*/

#define CMD_FIFO_RESET 0x04
#define MEASURMENTS    128

void MPU6050_Calibration(void)
{
	// calibration of gyro
	int32_t gyroMeanBias  [3] = {0};
	
	// FIFO reset
	//.................................................................................	
	ClearBits (MPU6050_I2C_ADDR, MPU6050_USER_CTRL, CMD_FIFO_EN);			 						// FIFO dis
	SetBits   (MPU6050_I2C_ADDR, MPU6050_USER_CTRL, CMD_FIFO_RESET);   						// FIFO reset
	SetBits   (MPU6050_I2C_ADDR, MPU6050_USER_CTRL, CMD_FIFO_EN);			 						// FIFO en
	
	// Read data for averaging
	//.................................................................................	
	for (uint8_t i = 0; i < MEASURMENTS; i++)
	{			 
		while ( !MPU6050_IsDataReady()) 												 						// wait untill data ready interrupt
		{
				
		}
		
		MPU6050_ReadRawData();
		MPU6050_WaitUntilDataIsReceived();
		
		int16_t gyro_temp  [3] = {0};
		
		 // Form signed 16-bit integer for each sample in FIFO
		
		gyro_temp[0]  = (int16_t) (((uint16_t)rawDataMPU6050[ 0] << 8) | rawDataMPU6050[1]  );
    gyro_temp[1]  = (int16_t) (((uint16_t)rawDataMPU6050[ 2] << 8) | rawDataMPU6050[3]  );
		gyro_temp[2]  = (int16_t) (((uint16_t)rawDataMPU6050[ 4] << 8) | rawDataMPU6050[5] );
		
//		// Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
	
		gyroMeanBias[0] 	+= (int32_t) gyro_temp[0];
		gyroMeanBias[1] 	+= (int32_t) gyro_temp[1];
		gyroMeanBias[2] 	+= (int32_t) gyro_temp[2];
 	}
	
	// FIFO disable
	//.................................................................................	
	ClearBits (MPU6050_I2C_ADDR, MPU6050_USER_CTRL, CMD_FIFO_EN);
	SetBits   (MPU6050_I2C_ADDR, MPU6050_USER_CTRL, CMD_FIFO_RESET);
	
	
	// Normalize sums to get average count biases
//	//.................................................................................	

	gyroMeanBias[0]   /= (int32_t) MEASURMENTS;
	gyroMeanBias[1]   /= (int32_t) MEASURMENTS;
	gyroMeanBias[2]   /= (int32_t) MEASURMENTS;

	
	gyroBias[0] = gyroMeanBias[0];
	gyroBias[1] = gyroMeanBias[1];
	gyroBias[2] = gyroMeanBias[2];
	
}

/*
	----------------------------------------------------------
	MPU6050 read data
	----------------------------------------------------------
	Gyro
	case GFS_250DPS:  return  250.0f/32768.0f;
  case GFS_500DPS:  return  500.0f/32768.0f;
  case GFS_1000DPS: return  1000.0f/32768.0f;
  case GFS_2000DPS: return 	2000.0f/32768.0f;	

	Accel
	case AFS_2G:  return 2.0f/32768.0f;
  case AFS_4G:  return 4.0f/32768.0f;
  case AFS_8G:  return 8.0f/32768.0f;
  case AFS_16G: return 16.0f/32768.0f; 
*/
void MPU6050_Read(float gyro[3])
{	
	int16_t gyroTemp[3]  = {((int16_t)((uint16_t)rawDataMPU6050[ 0] << 8) | rawDataMPU6050[ 1] ),
													((int16_t)((uint16_t)rawDataMPU6050[ 2] << 8) | rawDataMPU6050[ 3] ),
													((int16_t)((uint16_t)rawDataMPU6050[ 4] << 8) | rawDataMPU6050[ 5] )
												 };

	gyro[0]  =  ((float)gyroTemp [0] - gyroBias[0]) * (2000.0f / 32768.0f);
	gyro[1]  =  ((float)gyroTemp [1] - gyroBias[1]) * (2000.0f / 32768.0f); 
	gyro[2]  =  ((float)gyroTemp [2] - gyroBias[2]) * (2000.0f / 32768.0f); 
}


void MPU6050_ReadRawData(void)
{
	I2C1_ReadBurst(MPU6050_I2C_ADDR, MPU6050_FIFO_R_W, 
																		(uint8_t *)(&rawDataMPU6050[0]), 
																		      sizeof(rawDataMPU6050) / sizeof(uint8_t));
}

void MPU6050_WaitUntilDataIsReceived(void)
{
	while ( !(DMA1->LISR & DMA_LISR_TCIF0)) 
	{
		// wait until transfer is finished
	}
		
	DMA1->LIFCR      |= DMA_LIFCR_CTCIF0;  
}



uint8_t MPU6050_IsDataReady(void)
{
	return (GPIOB->IDR & GPIO_IDR_IDR_5);
}
