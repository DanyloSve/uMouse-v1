/*
	Based on https://github.com/drcpattison/ICM-20948/blob/master/src/ICM20948.cpp

	SPI_2
	PB12 NSS  - slave select 
	PB13 SCK  - serial clock
	PB14 MISO - master in  / slave out 
	PB15 MOSI - master out / slave in
	
	PA8 - interrupt
	PA9 - FSY
	
	Using the SPI interface, when the AP/user 
	disables the gyroscope sensor (User Bank 0, PWR_MGMT_2 register, bits [2:0]=111)
	as part of a sequence of register read or write commands, 
	the AP/user will be required to subsequently 
	
	!!!!!!!!!!!!WAIT 22 µs!!!!!!!!!!!!!!!!!!!!!
	
	prior to any of the following operations:
	
(1) Writing to any of the following registers:
• USER BANK 0: All registers except LP_CONFIG, 
		PWR_MGMT_1,
		PWR_MGMT_2, 
		INT_PIN_CFG, 
		INT_ENABLE, 
		FIFO_COUNTH, 
		FIFO_COUNTL, 
		FIFO_R_W, 
		FIFO_CFG, 
		REG_BANK_SEL
• USER BANK 1: All registers except REG_BANK_SEL
• USER BANK 2: All registers except REG_BANK_SEL
• USER BANK 3: All registers except REG_BANK_SEL
(2) Reading data from FIFO
(3) Reading from memory
*/

#include "icm-20948.h"

#define READ_FLAG 0x88

// REGs
//----------------------------------------------------
// USER BANK 0 REGs MAP
#define REG_WHO_AM_I			0x00
#define REG_USER_CTRL		  0x03
#define REG_LP_CONFIG 		0x05
#define REG_PWR_MGMT_1	  0x06
#define REG_PWR_MGMT_2	  0x07
#define REG_INT_PIN_CFG	  0x0F
#define REG_INT_ENABLE 	 	0x10
#define REG_INT_ENABLE_1	0x11
#define REG_INT_ENABLE_2  0x12
#define REG_INT_ENABLE_3  0x13
#define REG_I2C_MST_STATUS	0x17
#define REG_INT_STATUS 	  0x19
#define REG_INT_STATUS_1  0x1A
#define REG_INT_STATUS_2  0x1B
#define REG_INT_STATUS_3  0x1C
#define REG_DELAY_TIMEH		0x28
#define REG_DELAY_TIMEL		0x29
#define REG_ACCEL_XOUT_H	0x2D
#define REG_ACCEL_XOUT_L	0x2E
#define REG_ACCEL_YOUT_H	0x2F
#define REG_ACCEL_YOUT_L	0x30
#define REG_ACCEL_ZOUT_H	0x31
#define REG_ACCEL_ZOUT_L	0x32
#define REG_GYRO_XOUT_H		0x33
#define REG_GYRO_XOUT_L		0x34
#define REG_GYRO_YOUT_H		0x35
#define REG_GYRO_YOUT_L		0x36
#define REG_GYRO_ZOUT_H		0x37
#define REG_GYRO_ZOUT_L		0x38
#define REG_TEMP_OUT_H		0x39
#define REG_TEMP_OUT_L		0x3A

#define REG_FIFO_EN_1			0x66
#define REG_FIFO_EN_2			0x67
#define REG_FIFO_RST			0x68
#define REG_FIFO_MODE			0x69
#define REG_FIFO_COUNTH		0x70
#define REG_FIFO_COUNTL		0x71
#define REG_FIFO_R_W			0x72
#define REG_DATA_RDY_STATUS		0x74
#define REG_FIFO_CFG			0x76

#define REG_REG_BANK_SEL	0x7F

// USER BANK 1 REGs MAP
#define REG_SELF_TEST_X_GYRO	0x02
#define REG_SELF_TEST_Y_GYRO	0x03
#define REG_SELF_TEST_Z_GYRO	0x04
#define REG_SELF_TEST_X_ACCEL	0x0E
#define REG_SELF_TEST_Y_ACCEL	0x0F
#define REG_SELF_TEST_Z_ACCEL	0x10
#define REG_XA_OFFS_H					0x14
#define REG_XA_OFFS_L					0x15
#define REG_YA_OFFS_H					0x17
#define REG_YA_OFFS_L					0x18
#define REG_ZA_OFFS_H					0x1A
#define REG_ZA_OFFS_L					0x1B
#define REG_TIMEBASE_CORRECTION_PLL		0x28


// USER BANK 2 REGs MAP
#define REG_GYRO_SMPLRT_DIV		0x00 
#define REG_GYRO_CONFIG_1			0x01
#define REG_GYRO_CONFIG_2			0x02
#define REG_XG_OFFS_USRH			0x03
#define REG_XG_OFFS_USRL			0x04
#define REG_YG_OFFS_USRH			0x05
#define REG_YG_OFFS_USRL			0x06
#define REG_ZG_OFFS_USRH			0x07
#define REG_ZG_OFFS_USRL			0x08
#define REG_ODR_ALIGN_EN			0x09
#define REG_ACCEL_SMPLRT_DIV_1	0x10
#define REG_ACCEL_SMPLRT_DIV_2	0x11
#define REG_ACCEL_INTEL_CTRL	0x12
#define REG_ACCEL_WOM_THR			0x13
#define REG_ACCEL_CONFIG			0x14
#define REG_ACCEL_CONFIG_2		0x15
#define REG_FSYNC_CONFIG			0x52
#define REG_TEMP_CONFIG				0x53
#define REG_MOD_CTRL_USR			0x54


// CMDs
//-----------------------------------------------------
#define CMD_I2C_IF_DIS 				0x10
#define CMD_DEVICE_RESET 			0x80
#define CMD_CLKSEL_BEST_AVAIL 0x01 

#define CMD_BANK_0_SEL 				0x00
#define CMD_BANK_1_SEL 				0x10
#define CMD_BANK_2_SEL 				0x20

#define CMD_ACCEL_GYRO_EN 		0x00
#define CMD_ACCEL_GYRO_DIS		0x3F


#define CMD_GYRO_FS_SEL_DPS_250		0x00
#define CMD_GYRO_FS_SEL_DPS_500		0x02
#define CMD_GYRO_FS_SEL_DPS_1000	0x04
#define CMD_GYRO_FS_SEL_DPS_2000	0x06

#define CMD_GYRO_EN_DLPF	0x01

#define CMD_GYRO_DLPFCFG_0	0x08
#define CMD_GYRO_DLPFCFG_1	0x10
#define CMD_GYRO_DLPFCFG_2	0x20

#define CMD_ACCEL_EN_DLPF	0x01

#define CMD_ACCEL_FS_SEL_G_2		0x00
#define CMD_ACCEL_FS_SEL_G_4		0x02
#define CMD_ACCEL_FS_SEL_G_8	  0x04
#define CMD_ACCEL_FS_SEL_G_16	  0x06

#define CMD_ACCEL_DLPFCFG_0	0x08
#define CMD_ACCEL_DLPFCFG_1	0x10
#define CMD_ACCEL_DLPFCFG_2	0x20

#define CMD_INT_ANYRD_2CLEAR_READ 		0x20

#define CMD_RAW_DATA_0_RDY_EN 0x01

void ICM20948_Init(void)
{
	SPI2_Init();
	GPIO_Init();
	
	// select BANK 0
	SPI2_ReadWriteReg(REG_REG_BANK_SEL, CMD_BANK_0_SEL);
	delay_us(200);
	
	// reset device
	SPI2_ReadWriteReg(REG_PWR_MGMT_1, CMD_DEVICE_RESET);
	delay_us(200);
	
	// select BANK 0
	SPI2_ReadWriteReg(REG_REG_BANK_SEL, CMD_BANK_0_SEL);
	delay_us(200);
	
	// SPI mode, DMP_DIS, FIFO_DIS
	SPI2_ReadWriteReg(REG_USER_CTRL, CMD_I2C_IF_DIS);
	delay_us(200);
	
	// Wake up,  LP_DIS, Clock configuration = autoselect best source
	SPI2_ReadWriteReg(REG_PWR_MGMT_1, CMD_CLKSEL_BEST_AVAIL );
	delay_us(200);
	
	
	// Gyro config
	//--------------------------------------------------
	// select BANK 2
	SPI2_ReadWriteReg(REG_REG_BANK_SEL, CMD_BANK_2_SEL);
	delay_us(200);
	
	// Set sample rate = gyroscope output rate/(1 + GYRO_SMPLRT_DIV)
  // Use a 220 Hz rate; a rate consistent with the filter update rate
  // determined inset in CONFIG .
	SPI2_ReadWriteReg(REG_GYRO_SMPLRT_DIV, 0x04);
	delay_us(200);
	
	// en gyro dlpf, fs_sel = 2000dps, dlpf bandwith = 119.5Hz, nyquist bandwidth = 154.3
	// Enable Digital low-pass filter (DLPF)
	// Set gyro bandwidth to 119.5Hz
	// respectively;
	// minimum delay time for this setting is 5.9 ms, which means sensor fusion
  // update rates cannot be higher than 1 / 0.0059 = 170 Hz
  // DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
  // With the ICM20948, it is possible to get gyro sample rates of 32 kHz (!),
  // 8 kHz, or 1 kHz
  // Set gyroscope full scale range to 250 dps
	
	/*
	SPI2_ReadWriteReg(REG_GYRO_CONFIG_1, CMD_GYRO_EN_DLPF 				| 
																			 CMD_GYRO_FS_SEL_DPS_2000 | 
																			 CMD_GYRO_DLPFCFG_0 | CMD_GYRO_DLPFCFG_1);
	*/																		 
	SPI2_ReadWriteReg(REG_GYRO_CONFIG_1, CMD_GYRO_EN_DLPF 				 | 
																			 CMD_GYRO_FS_SEL_DPS_250 | 
																			 CMD_GYRO_DLPFCFG_0 | CMD_GYRO_DLPFCFG_1);
	delay_us(200);
	
	// Accel config
	//--------------------------------------------------
	
	// Set gyroscope full scale range
  // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are
  // left-shifted into positions 4:3

  // Set accelerometer full-scale range configuration
  // Get current ACCEL_CONFIG register value
	// 3DB BW 111.4
	SPI2_ReadWriteReg(REG_ACCEL_CONFIG, CMD_ACCEL_EN_DLPF |
																			CMD_ACCEL_FS_SEL_G_2 |
																			CMD_ACCEL_DLPFCFG_0  | CMD_ACCEL_DLPFCFG_1);
	delay_us(200);
	
	// Set accelerometer sample rate configuration
  // It is possible to get a 4 kHz sample rate from the accelerometer by
  // choosing 1 for accel_fchoice_b bit [3]; in this case the bandwidth is
  // 1.13 kHz
	SPI2_ReadWriteReg(REG_ACCEL_SMPLRT_DIV_2, 0x04);
	delay_us(200);
	
	// The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
  // but all these rates are further reduced by a factor of 5 to 200 Hz because
  // of the GYRO_SMPLRT_DIV setting
	
	// Interrupt config
	//--------------------------------------------------
	SPI2_ReadWriteReg(REG_REG_BANK_SEL, CMD_BANK_0_SEL);
	delay_us(200);
	
	// Configure Interrupts and Bypass Enable
	
  // The logic level for INT1 pin is active high.
	// INT1 pin is configured as push-pull.
	// INT1 pin level held until interrupt status is cleared.
	// Interrupt status in INT_STATUS is cleared (set to 0) only by reading INT_STATUS register.
	// The logic level for the FSYNC pin as an interrupt to the ICM-20948 is active high.
	// This disables the FSYNC pin from causing an interrupt.
	SPI2_ReadWriteReg(REG_INT_PIN_CFG, CMD_INT_ANYRD_2CLEAR_READ);
	delay_us(500);
	
	SPI2_ReadWriteReg(REG_INT_ENABLE_1, 0x01);
	delay_us(200);
}

void GPIO_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	// PA8 ICM-20948 interrupt GPIO input
	GPIOA->MODER   &= ~GPIO_MODER_MODER8;
	GPIOA->PUPDR   &= ~GPIO_PUPDR_PUPDR8;
	
	// PA9 ICM-20948 FSY GPIO output
	GPIOA->MODER   |=  GPIO_MODER_MODER9_0;
	GPIOA->MODER   &= ~GPIO_MODER_MODER9_1;
	
	GPIOA->OTYPER  &= ~GPIO_OTYPER_OT_9;
	
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;
	
	GPIOA->PUPDR   &= ~GPIO_PUPDR_PUPDR8;
}
