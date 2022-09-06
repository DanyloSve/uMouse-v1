#ifndef _SPI2_H__
#define _SPI2_H__

#include "stm32f4xx.h"
#include "timer_delays.h"

void SPI2_Init(void);
uint8_t SPI2_TransmitReceiveByte(uint8_t data);
uint8_t SPI2_ReadWriteReg(uint8_t reg, uint8_t data);

void 		SPI2_BurstRead(uint8_t addr, uint8_t *data, uint32_t size);

static void GPIO_Init(void);
#endif /*_SPI2_H__*/
