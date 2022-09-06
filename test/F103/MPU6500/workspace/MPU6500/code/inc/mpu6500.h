#ifndef __MPU6500_H_
#define __MPU6500_H_

#include "spi1.h"
#include "timer_delays.h"

void MPU6500_Init(void);
void MPU6500_ReadData(void);

uint8_t	MPU6500_TestRead(void);

float MPU6500_AccelX(void);
float MPU6500_AccelY(void);
float MPU6500_GyroZ(void);
float MPU6500_GyroY(void);

static uint8_t sendCommand(uint8_t reg, uint8_t data);

static void calibarate(void);

#endif /*MPU6500_H__*/

