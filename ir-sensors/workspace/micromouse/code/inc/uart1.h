#ifndef _UART1_H__
#define _UART1_H__


#include "stm32f4xx.h"

void UART1_Init(void);
//void UART1_DMA_Send(void);

void    UART1_SendByte(uint8_t byte);
uint8_t UART1_ReadByte(void);

void UART1_WaitUntilDataIsSend(void);
void UART1_WaitUntilDataIsReceived(void);

inline void UART1_DMA_Send(void)
{
	DMA2_Stream7->CR &= ~DMA_SxCR_EN;
	DMA2->HIFCR |= DMA_HIFCR_CTCIF7; // Stream x clear transfer complete interrupt flag
	DMA2_Stream7->CR |= DMA_SxCR_EN;
}

inline void UART1_ClearTXIntFlag(void)
{
	DMA2_Stream2->CR &= ~DMA_SxCR_EN;
	DMA2->LIFCR      |= DMA_LIFCR_CTCIF2;
	DMA2_Stream2->CR |= DMA_SxCR_EN;
}

#endif /*_UART1_H__*/
