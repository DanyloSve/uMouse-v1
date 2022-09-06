#include "uart1.h"

#include <stdio.h>
/*
	PA9 - TX alternative function output push-pull
	PA10 - RX pull-up input
	
	UART - 460'800 baud ~ 46080 bytes/s
	
	DMA - low priority
*/

static void GPIO_Init            (void);
static void UART1_DMA2_Init      (void);

volatile uint8_t UART1_DMAbufferTX[9];
volatile uint8_t UART1_DMAbufferRX[3];

void UART1_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	GPIO_Init();
	
	
	USART1->BRR = 217; // 100 MHz / 460'800
	//USART1->BRR = 0x271;
	
	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	
	USART1->CR1 |= USART_CR1_RXNEIE;
	UART1_DMA2_Init();
}

void GPIO_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	// PA9  - TX USART1 - AF7 
	// PA10 - RX
	//---------------------------------------------
	GPIOA->MODER |= GPIO_MODER_MODER9_1  |
									GPIO_MODER_MODER10_1 ;
	
	GPIOA->MODER &= ~(GPIO_MODER_MODER9_0 |
									  GPIO_MODER_MODER10_0);
	
	GPIOA->OSPEEDR &= ~( GPIO_OSPEEDER_OSPEEDR9 |
											 GPIO_OSPEEDER_OSPEEDR10 );
	
	GPIOA->AFR[1] |= (0x07 << 4) | (0x07 << 4 * 2);	
}

void UART1_DMA2_Init(void)
{
	/*
		USART1_RX DMA2 Stream 2 Channel 4
		USART1_TX DMA2 Stream 7 Channel 4
	*/
	
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	
	// USART1_RX DMA2 Streem 2 Channel 4 Periph-To-Mem----------------------
	DMA2_Stream2->CR &= ~DMA_SxCR_EN;
	
	DMA2_Stream2->PAR  = (uint32_t)(&(USART1->DR));
	DMA2_Stream2->M0AR = (uint32_t)(&(UART1_DMAbufferRX[0]));
	
	DMA2_Stream2->NDTR = (uint16_t)(sizeof(UART1_DMAbufferRX) / sizeof(uint8_t));
	
	DMA2_Stream2->CR &= ~DMA_SxCR_CHSEL; 					// channel 4 is selected
	DMA2_Stream2->CR |= DMA_SxCR_CHSEL_2;
	
	DMA2_Stream2->CR &= ~DMA_SxCR_PL_0;						// medium
	DMA2_Stream2->CR &= ~DMA_SxCR_PL_1;	
	
	DMA2_Stream2->CR &= ~DMA_SxCR_MSIZE; 					// 8bit mem data size
	DMA2_Stream2->CR &= ~DMA_SxCR_PSIZE;
	
	DMA2_Stream2->CR |= DMA_SxCR_MINC;   				// mem increment
	//DMA2_Stream5->CR &= ~DMA_SxCR_MINC;
	DMA2_Stream2->CR &= ~DMA_SxCR_PINC; 					// no periph inc
	
	DMA2_Stream2->CR &= ~DMA_SxCR_CIRC;
	
	DMA2_Stream2->CR &= ~DMA_SxCR_DIR;   					// periph to mem
	
	DMA2_Stream2->CR |= DMA_SxCR_TCIE;   					// transfer complete int en
	
	DMA2->LIFCR      |= DMA_LIFCR_CTCIF2;     		// clear transfer complete flag
	

	
	DMA2_Stream2->CR |= DMA_SxCR_EN;
	
	USART1->CR3 |= USART_CR3_DMAR;
	
	// DMA2 Stream 7 TX Ch4 Mem-to-Periph----------------------
	DMA2_Stream7->CR &= ~DMA_SxCR_EN;
	
	DMA2_Stream7->PAR  = (uint32_t) &(USART1->DR);
	DMA2_Stream7->M0AR = (uint32_t)(&(UART1_DMAbufferTX[0]));
	
	DMA2_Stream7->NDTR = (uint16_t)(sizeof(UART1_DMAbufferTX) / sizeof(uint8_t));

	
	DMA2_Stream7->CR &= ~DMA_SxCR_CHSEL; // channel 4 is selected
	DMA2_Stream7->CR |= DMA_SxCR_CHSEL_2;
	
	DMA2_Stream7->CR &= ~DMA_SxCR_MBURST;
	DMA2_Stream7->CR &= ~DMA_SxCR_PBURST;
	
	DMA2_Stream7->CR &= ~DMA_SxCR_PL; // low priority
	
	DMA2_Stream7->CR &= ~DMA_SxCR_MSIZE; // 8bit mem data size
	DMA2_Stream7->CR &= ~DMA_SxCR_PSIZE;
	
	DMA2_Stream7->CR |= DMA_SxCR_MINC; // mem increment
	DMA2_Stream7->CR &= ~DMA_SxCR_PINC; // no periph inc
	
	DMA2_Stream7->CR &= ~DMA_SxCR_CIRC;
	
	DMA2_Stream7->CR |= DMA_SxCR_DIR_0; // mem to periph
	DMA2_Stream7->CR &= ~DMA_SxCR_DIR_1;
	
	DMA2->HIFCR |= DMA_HIFCR_CTCIF7; // clear transfer complete flag
	
	USART1->CR3 |= USART_CR3_DMAT;
}


void UART1_SendByte(uint8_t byte)
{
	while ( !(USART1->SR & USART_SR_TXE))
	{
	
	}
	
	USART1->DR = byte;
}


uint8_t UART1_ReadByte(void)
{
	while ( !(USART1->SR & USART_SR_RXNE))
	{
	
	}
	
	return USART1->DR;
}

void UART1_WaitUntilDataIsSend(void)
{
	while ( !(DMA2->HISR & DMA_HISR_TCIF7)) 		
	{
			// wait until transfer is finished
	}
		
	DMA2->HIFCR      |= DMA_HIFCR_CTCIF7;  
}

void UART1_WaitUntilDataIsReceived(void)
{
	while( !(DMA2->LISR & DMA_LISR_TCIF2) )
	{
		
	}	

	UART1_ClearTXIntFlag();	
}
