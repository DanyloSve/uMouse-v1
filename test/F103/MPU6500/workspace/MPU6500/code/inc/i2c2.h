#ifndef I2C2_H_
#define I2C2_H_

#include "stm32f10x.h"
#include "timer_delays.h"

void I2C2_Init(void);
void I2C2_SendByte(uint16_t addr, uint8_t data);
uint8_t I2C2_ReceiveByte(uint16_t addr);

int I2C2_Scan(void);
int I2C2_scan(void);

void I2C2_Write(uint16_t addr, uint8_t *buffer, uint32_t bufferSize);
void I2C2_Read (uint16_t addr, uint8_t *buffer, uint32_t bufferSize);

	
#endif /*I2C_H_*/
