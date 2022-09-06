#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "stm32f4xx.h"

#include "timer_delays.h"
#include "i2c1.h"

#define MPU6050_I2C_ADDR 	0x68
#define HMC5883L_I2C_ADDR 0x1E

void 		MPU6050_Init        (void);
void		MPU6050_Read        (float accel[3], float gyro[3]);
void 		MPU6050_ReadRawData (void);
uint8_t MPU6050_IsDataReady (void);
void		MPU6050_WaitUntilDataIsReceived  (void);

void    HMC5883L_Init        (void);
void 		HMC5883L_ReadRawData (void);
void 		HMC5883L_Read        (float mag[3]);
uint8_t HMC5883L_IsDataReady (void);
void 		HMC5883L_WaitUntilDataIsReceived (void);


#endif /*__MPU6050_H__*/

