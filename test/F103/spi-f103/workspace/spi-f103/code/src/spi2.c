/*
	Is not configured for SPI2!!!
*/

#include "spi2.h"

#define SPI2_NSS_SET  (GPIOA->ODR &= ~GPIO_ODR_ODR4);
#define SPI2_NSS_RST  (GPIOA->ODR |= GPIO_ODR_ODR4);

void SPI2_Init(void)
{
	GPIO_Init();
	
	RCC->APB2ENR |= RCC_APB1ENR_SPI2EN; 
	
	//SPI2->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1); // Fpclk / 128 ~ 0.5Mhz
	SPI2->CR1 |= SPI_CR1_BR;
	SPI2->CR1 |= SPI_CR1_MSTR; 									// master configuration
	
	SPI2->CR1 &= ~SPI_CR1_LSBFIRST; 						// MSB first
	
	SPI2->CR1 |= SPI_CR1_CPOL; 									// CLK in idle state is high !!!![not specified by MPU-6500 datasheet]
	SPI2->CR1 |= SPI_CR1_CPHA; 									// Data is latched on the rising edge of SCLK

	SPI2->CR1 &= ~SPI_CR1_DFF; 									// 8-bit data frame
	
	SPI2->CR1 |= SPI_CR1_SSM;										// software slave management 
	SPI2->CR1 |= SPI_CR1_SSI;										// internal slave select
	
	//NVIC_EnableIRQ(SPI2_IRQn);
	
	SPI2->CR1 |= SPI_CR1_SPE; 									// SPI en
}

void SPI2_IRQHandler(void)
{
	if (!(SPI2->SR & SPI_SR_BSY))
	{
		delay_us(1);
		SPI2->CR2 &= ~SPI_CR2_TXEIE;
		SPI2_NSS_RST;
	}
}

void GPIO_Init(void)
{
	/*
		PA5 SCK  (master) - alternative function push-pull 
		PA7 MOSI          - Alternative function push-pull
		PA6 MISO (master) - Input pull up
		PA4 NSS           - software controlled
	*/
	
	// clock configuration
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // 
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	// PA5 SCK
	GPIOA->CRL |= GPIO_CRL_MODE5; // output mode max speed 50MHz
	GPIOA->CRL |= GPIO_CRL_CNF5_1;  // alfio push-pull
	GPIOA->CRL &= ~GPIO_CRL_CNF5_0;
	
	// PA7 MOSI
	GPIOA->CRL |= GPIO_CRL_MODE7; // output mode max spedd 10MHz
	GPIOA->CRL |= GPIO_CRL_CNF7_1;  // afio push-pull
	GPIOA->CRL &= ~GPIO_CRL_CNF7_0;
	
	// PA6 MISO
	GPIOA->CRL &= ~GPIO_CRL_MODE6; // input mode
	GPIOA->CRL |= GPIO_CRL_CNF6_1; // pull-up/pull-down configuration
	GPIOA->CRL &= ~GPIO_CRL_CNF6_0;
	GPIOA->ODR |= GPIO_ODR_ODR6;   //pull-up

	// PA4 NSS
	GPIOA->CRL &= ~GPIO_CRL_CNF4;  //  push-pull
	GPIOA->CRL |= GPIO_CRL_MODE4;  // output mode max speed 50MHz
}

void DMA1_Init(void)
{
/*
		DMA1
	SPI2_RX Channel 2
	SPI2_TX Channel 3
*/
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	
}



uint8_t SPI2_TransmitReceiveByte(uint8_t data)
{	
	
	while ( !(SPI2->SR & SPI_SR_TXE) )
	{
	
	}
	
	SPI2_NSS_SET;	
	SPI2->DR = data;
	
	while ( !(SPI2->SR & SPI_SR_RXNE) )
	{
	
	}
	data = SPI2->DR;
	SPI2_NSS_RST;
	
	return data;
}



void SPI2_TransmitString(uint8_t *data)
{
	while(*data)
	{
		SPI2_TransmitReceiveByte(*data++);
	}
}
