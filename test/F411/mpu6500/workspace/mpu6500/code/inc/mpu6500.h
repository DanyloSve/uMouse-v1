#ifndef __MPU6500_H__
#define __MPU6500_H__

#include "spi2.h"
#include "stm32f4xx.h"
#include "timer_delays.h"

void MPU6500_Init(uint8_t gScale, uint8_t aScale);
void MPU6500_Read(float gyroData[3], float accelData[3]);

float *MPU6500_GetGyroBias(void);

static void PowerUp(void);

static void Gyro_Init(void);
static void Accel_Init(void);

static void calibrate(void);

static void Interrupt_Init(void);
static void EXTI_Init(void);

static float getGyroResolution(uint8_t gScale);
static float getAccelResolution(uint8_t aScale);

static void readData(void);

#endif
