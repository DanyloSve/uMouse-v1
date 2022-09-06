/*
	SPI_1
	PA4 NSS  - slave select 
	PA5 SCK  - serial clock
	PA6 MISO - master in  / slave out 
	PA7 MOSI - master out / slave in
	
	CS goes low (active) at the start of transmission and goes back high (inactive) at the end. Only one CS line is active at a 
	time, ensuring that only one slave is selected at any given time. The CS lines of the non-selected slave devices are held 
	high, causing their SDO lines to remain in a high-impedance (high-z) state so that they do not interfere with any active 
	devices. 
	
	SPI Configuration for MPU-6500
		1. Data is delivered MSB first and LSB last 
		2. Data is latched on the rising edge of SCLK 
		3. Data should be transitioned on the falling edge of SCLK 
		4. The maximum frequency of SCLK is 1MHz 
		5. SPI read and write operations are completed in 16 or more clock cycles (two or more bytes). The first byte 
				contains the SPI Address, and the following byte(s) contain(s) the SPI data.  The first bit of the first byte 
				contains the Read/Write bit and indicates the Read (1) or Write (0) operation. The following 7 bits contain the 
				Register Address.  In cases of multiple-byte Read/Writes, data is two or more bytes: 
*/
/*
#include "spi2.h"

#define SPI1_NSS_SET  (GPIOA->ODR &= ~GPIO_ODR_ODR4);
#define SPI1_NSS_RST  (GPIOA->ODR |= GPIO_ODR_ODR4);

void SPI1_Init(void)
{
	GPIO_Init();
	
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; 
	
	SPI1->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1); // Fpclk / 128 ~ 0.5Mhz
	//SPI1->CR1 |= SPI_CR1_BR_2;
	SPI1->CR1 |= SPI_CR1_MSTR; 									// master configuration
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
	
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST; 						// MSB first
	
	SPI1->CR1 |= SPI_CR1_CPOL; 									// CLK in idle state is high !!!![not specified by MPU-6500 datasheet]
	SPI1->CR1 |= SPI_CR1_CPHA; 									// Data is latched on the rising edge of SCLK

	SPI1->CR1 |= SPI_CR1_DFF; 									// 16-bit data frame
	
	SPI1->CR1 |= SPI_CR1_SSM;										// software slave management 
	SPI1->CR1 |= SPI_CR1_SSI;										// internal slave select
	
	//NVIC_EnableIRQ(SPI1_IRQn);
	
	SPI1->CR1 |= SPI_CR1_SPE; 									// SPI en
}

void GPIO_Init(void)
{
	
		PA5 SCK  (master) - alternative function push-pull 
		PA7 MOSI          - Alternative function push-pull
		PA6 MISO (master) - Input pull up
		PA4 NSS           - software controlled
	
	
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

		DMA1
	SPI1_RX Channel 2
	SPI1_TX Channel 3

	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}



uint8_t SPI1_TransmitReceiveByte(uint16_t data)
{	
	
	while ( !(SPI1->SR & SPI_SR_TXE) )
	{
	
	}
	
	SPI1_NSS_SET;	
	SPI1->DR = data;
	
	while ( !(SPI1->SR & SPI_SR_RXNE) )
	{
	
	}
	data = SPI1->DR;
	SPI1_NSS_RST;
	
	return (uint8_t)data;
}

void SPI1_TransmitString(uint8_t *data)
{
	while(*data)
	{
		SPI1_TransmitReceiveByte(*data++);
	}
}
*/
