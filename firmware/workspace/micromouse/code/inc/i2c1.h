#ifndef _I2C1_H__
#define _I2C1_H__

#include "stm32f4xx.h"
#include "timer_delays.h"

#define I2C1_DEBUG_MODE // -------------------------------------------------------------------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void     I2C1_Init(void);

void     I2C1_WriteReg(const uint8_t addr, const uint8_t reg, const uint8_t data);
uint8_t  I2C1_ReadReg (const uint8_t addr, const uint8_t reg);

void I2C1_ReadBurst (const uint8_t addr, const uint8_t reg, uint8_t *buffer, const uint8_t bufferSize);
void I2C1_WriteBurst(const uint8_t addr, const uint8_t reg, uint8_t *buffer, const uint8_t bufferSize);
	
	#if defined(I2C1_DEBUG_MODE)
uint8_t I2C1_Scan(void);
uint8_t I2C1_Search(uint8_t addr);
void I2C1_ScanAddrs(uint8_t* addrs, const uint8_t addrCntr);
	
	#endif /*I2C1_DEBUG_MODE*/

#endif /*_I2C1_H__*/
