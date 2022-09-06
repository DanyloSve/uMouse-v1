#ifndef __SPI1_H_
#define __SPI1_H_

#include "stm32f10x.h"
#include "timer_delays.h"

void SPI1_Init(void);

uint8_t SPI1_TransmitReceiveByte(uint16_t data);
void 		SPI1_TransmitString(uint8_t *data);
void 		SPI1_ReceiveBytes(uint8_t addr, uint8_t *data, uint32_t size);

static void GPIO_Init(void);
static void DMA1_Init(void);
static void Interrupt_Init(void);
static void SPI1_IRQHandler(void);
	
#endif /*SPI1*/
