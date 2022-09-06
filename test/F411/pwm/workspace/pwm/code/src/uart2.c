#include "uart2.h"

#include <stdio.h>
/*
	PA2 - TX alternative function output push-pull
	PA3 - RX pull-up input
	
	UART - 115 200baud
	
	DMA - low priority
*/

static void GPIO_Init            (void);
static void UART2_DMA1_Init      (void);

volatile uint8_t UART2_DMAbufferTX[9];
volatile uint8_t UART2_DMAbufferRX[3];
	
#define DEBUG_MODE

#if defined(DEBUG_MODE)
	volatile  char 	 UART2_debugArr[64];
#endif /*DEBUG_MODE*/

void UART2_Init(void)
{
	GPIO_Init();
	
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	USART2->BRR = 434; // 50 MHz / 115'200
	
	USART2->CR1 |= USART_CR1_UE;
	USART2->CR1 |= USART_CR1_TE;
	USART2->CR1 |= USART_CR1_RE;
	
	
	UART2_DMA1_Init();
}

void GPIO_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	// PA2 - TX USART2 - AF7 
	//---------------------------------------------
	GPIOA->MODER |= GPIO_MODER_MODER2_1;
	GPIOA->MODER &= ~GPIO_MODER_MODER2_0;
	
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_2;
	
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2;
	
	GPIOA->AFR[0] |= (0x07 << 8);
	
	// PA3- RX 
	//----------------------------------------------
	GPIOA->MODER |= GPIO_MODER_MODER3_1;
	GPIOA->MODER &= ~GPIO_MODER_MODER3_0;
	GPIOA->AFR[0] |= (0x07 << 4 * 3);
	
}

void UART2_DMA1_Init(void)
{
	/*
		USART2_RX DMA1 Streem 5 Channel 4
		USART2_TX DMA1 Streem 6 Channel 4
	*/
	
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	
	
	// USART2_RX DMA1 Streem 5 Channel 4 Periph-To-Mem----------------------
	DMA1_Stream5->CR &= ~DMA_SxCR_EN;
	
	DMA1_Stream5->PAR  = (uint32_t) &(USART2->DR);
	DMA1_Stream5->M0AR = (uint32_t)(UART2_DMAbufferRX);
	
	DMA1_Stream5->NDTR = (uint16_t)(sizeof(UART2_DMAbufferRX) / sizeof(uint8_t));
	
	DMA1_Stream5->CR &= ~DMA_SxCR_CHSEL; 					// channel 4 is selected
	DMA1_Stream5->CR |= DMA_SxCR_CHSEL_2;
	
	DMA1_Stream5->CR &= ~DMA_SxCR_PL_0;						// medium
	DMA1_Stream5->CR &= ~DMA_SxCR_PL_1;	
	
	DMA1_Stream5->CR &= ~DMA_SxCR_MSIZE; 					// 8bit mem data size
	DMA1_Stream5->CR &= ~DMA_SxCR_PSIZE;
	
	DMA1_Stream5->CR |= DMA_SxCR_MINC;   				// mem increment
	//DMA1_Stream5->CR &= ~DMA_SxCR_MINC;
	DMA1_Stream5->CR &= ~DMA_SxCR_PINC; 					// no periph inc
	
	DMA1_Stream5->CR &= ~DMA_SxCR_CIRC;
	
	DMA1_Stream5->CR &= ~DMA_SxCR_DIR;   					// periph to mem
	
	DMA1_Stream5->CR |= DMA_SxCR_TCIE;   					// transfer complete int en
	DMA1->HIFCR      |= DMA_HIFCR_CTCIF5;     		// clear transfer complete flag
	
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	
	DMA1_Stream5->CR |= DMA_SxCR_EN;
	
	USART2->CR3 |= USART_CR3_DMAR;
	
	// DMA1 Stream 6 TX Ch4 Mem-to-Periph----------------------
	DMA1_Stream6->CR &= ~DMA_SxCR_EN;
	
	DMA1_Stream6->PAR  = (uint32_t) &(USART2->DR);
	DMA1_Stream6->M0AR = (uint32_t)(UART2_DMAbufferTX);
	
	DMA1_Stream6->NDTR = (uint16_t)(sizeof(UART2_DMAbufferTX) / sizeof(uint8_t));

	
	DMA1_Stream6->CR &= ~DMA_SxCR_CHSEL; // channel 4 is selected
	DMA1_Stream6->CR |= DMA_SxCR_CHSEL_2;
	
	DMA1_Stream6->CR &= ~DMA_SxCR_MBURST;
	DMA1_Stream6->CR &= ~DMA_SxCR_PBURST;
	
	DMA1_Stream6->CR &= ~DMA_SxCR_PL; // low priority
	
	DMA1_Stream6->CR &= ~DMA_SxCR_MSIZE; // 8bit mem data size
	DMA1_Stream6->CR &= ~DMA_SxCR_PSIZE;
	
	DMA1_Stream6->CR |= DMA_SxCR_MINC; // mem increment
	DMA1_Stream6->CR &= ~DMA_SxCR_PINC; // no periph inc
	
	DMA1_Stream6->CR &= ~DMA_SxCR_CIRC;
	
	DMA1_Stream6->CR |= DMA_SxCR_DIR_0; // mem to periph
	DMA1_Stream6->CR &= ~DMA_SxCR_DIR_1;
	
	DMA1->HIFCR |= DMA_HIFCR_CTCIF6; // clear transfer complete flag
	
	USART2->CR3 |= USART_CR3_DMAT;
}

void DMA1_Stream5_IRQHandler(void)
{
	if ( (DMA1->HISR & DMA_HISR_TCIF5))
	{
		DMA1_Stream5->CR &= ~DMA_SxCR_EN;
		GPIOC->ODR ^= GPIO_ODR_ODR_13;
		sprintf((char *)(&UART2_DMAbufferTX[0]), "hello");
		UART2_DMA_Send();
		DMA1->HIFCR      |= DMA_HIFCR_CTCIF5;     		// clear transfer complete flag
		DMA1_Stream5->CR |= DMA_SxCR_EN;
	
	}
}

void UART2_DMA_Send(void)
{
	DMA1_Stream6->CR &= ~DMA_SxCR_EN;
	DMA1->HIFCR |= DMA_HIFCR_CTCIF6; // Stream x clear transfer complete interrupt flag
	DMA1_Stream6->CR |= DMA_SxCR_EN;
}



void UART2_SendByte(uint8_t byte)
{
	while ( !(USART2->SR & USART_SR_TXE))
	{
	
	}
	
	USART2->DR = byte;
}


uint8_t UART2_ReadByte(void)
{
	while ( !(USART2->SR & USART_SR_RXNE))
	{
	
	}
	
	return USART2->DR;
}

void UART2_WaitUntilDataIsSend(void)
{
	while ( !(DMA1->HISR & DMA_HISR_TCIF6)) 		
	{
			// wait until transfer is finished
	}
		
	DMA1->HIFCR      |= DMA_HIFCR_CTCIF6;  
}