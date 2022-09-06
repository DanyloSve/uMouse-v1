/*
	Is not configured for SPI2!!!
*/

#ifndef SPI2_H_
#define SPI2_H_

#include "stm32f10x.h"
#include "timer_delays.h"

void SPI2_Init(void);

uint8_t SPI1_TransmitReceiveByte(uint8_t data);
void 		SPI1_TransmitString(uint8_t *data);

static void GPIO_Init(void);

static void Interrupt_Init(void);
static void SPI2_IRQHandler(void);
	
#endif /*SPI*/
