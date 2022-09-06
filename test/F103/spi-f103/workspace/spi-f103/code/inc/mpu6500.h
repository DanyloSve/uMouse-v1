#ifndef MPU6500_H_
#define MPU6500_H_

#include "spi1.h"
#include "timer_delays.h"

struct {
	uint16_t accelX;
	uint16_t accelY;
	uint16_t gyroZ;
} MPU6500;

void MPU6500_Init(void);
void MPU6500_ReadData(void);

static void sendCommand(uint8_t reg, uint8_t data);

#endif /*MPU6500_H_*/
