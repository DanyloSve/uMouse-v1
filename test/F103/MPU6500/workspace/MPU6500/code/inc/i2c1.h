#ifndef I2C1_H_
#define I2C1_H_

#include "stm32f10x.h"
#include "timer_delays.h"

void I2C1_Init(void);
void I2C1_SendByte(uint16_t addr, uint8_t data);
uint8_t I2C1_ReceiveByte(uint16_t addr);
int I2C1_Scan(void);
int I2C1_scan(void);

void I2C1_Write(uint16_t addr, uint8_t *buffer, uint32_t bufferSize);
void I2C1_Read(uint16_t addr, uint8_t *buffer, uint32_t bufferSize);
	
#endif /*I2C_H_*/
