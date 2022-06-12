#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "stm32f4xx.h"

#include "timer_delays.h"
#include "i2c1.h"

#define MPU6050_I2C_ADDR 	0x68
#define HMC5883L_I2C_ADDR 0x1E

void 		MPU6050_Init        (void);
void		MPU6050_Read        (float gyro[3]);
void 		MPU6050_ReadRawData (void);
uint8_t MPU6050_IsDataReady (void);
void		MPU6050_WaitUntilDataIsReceived  (void);


#endif /*__MPU6050_H__*/

