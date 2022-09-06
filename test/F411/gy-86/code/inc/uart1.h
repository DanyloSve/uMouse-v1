#ifndef _UART1_H__
#define _UART1_H__

#include "stdint.h"

void UART1_Init(void);
void UART1_DMA_Send(void);

void    UART1_SendByte(uint8_t byte);
uint8_t UART1_ReadByte(void);

void UART1_WaitUntilDataIsSend(void);

extern volatile uint8_t UART1_DMAbufferTX[40];
extern volatile uint8_t UART1_DMAbufferRX[2];


#endif /*_UART1_H__*/
