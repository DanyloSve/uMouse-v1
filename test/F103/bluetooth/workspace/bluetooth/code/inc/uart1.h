#ifndef __UART1_H_
#define __UART1_H_

#include "stm32f10x.h"

void UART1_Init(void);
void UART1_SendByte(uint8_t data);
void UART1_SendString(char *data);
uint8_t UART1_ReadByte(void);

static void GPIO_Init(void);

#endif /*__UART1_H_*/