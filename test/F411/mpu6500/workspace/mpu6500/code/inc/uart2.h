#ifndef _UART1_H__
#define _UART1_H__

#include "stm32f4xx.h"



void UART2_Init(void);
void UART2_DMA_Send(void);

void UART2_SendByte(uint8_t byte);
uint8_t UART2_ReadByte(void);

void UART2_SetBufferString(uint8_t *str);

static void GPIO_Init(void);
static void UART2_DMA1_Init(void);
static void UART2_Interrupt_RXNE(void);

#endif /*_UART1_H__*/
