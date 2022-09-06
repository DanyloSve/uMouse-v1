/*
	SPI_2
	PB12 NSS  - slave select 
	PB13 SCK  - serial clock
	PB14 MISO - master in  / slave out 
	PB15 MOSI - master out / slave in
	
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
				
		When MPU is configured, SPI speed is set 50MHz / 64 == 0.78125MHz
		
		MPU6500/MPU9250 max write speed - 1Mhz
*/


#include "spi2.h"

#define SPI2_NSS_SET  (GPIOB->ODR &= ~GPIO_ODR_ODR_12);
#define SPI2_NSS_RST  (GPIOB->ODR |= GPIO_ODR_ODR_12);

void SPI2_Init(void)
{
	// APB1 = 50MHz
	// SPI = 50MHz / 64 == 0.78125MHz
	
	// configuration should be done according to 
	// "20.3.3 Configuring the SPI in master mode"
	GPIO_Init();
	
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	
	SPI2->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_0); // 50 / 64 == 0.7812MHz---------------->
	//SPI2->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1); // 50 / 128 == 0.39MHz 
	
	SPI2->CR1 |= SPI_CR1_MSTR; // STM32 is master
	SPI2->CR1 &= ~SPI_CR1_BIDIMODE;
	SPI2->CR1 &= ~SPI_CR1_LSBFIRST; // MSB first
	SPI2->CR1 &= ~SPI_CR1_DFF; // 8 - bit
	
	SPI2->CR1 |= SPI_CR1_SSM; // software slave mngnmt
	SPI2->CR1 |= SPI_CR1_SSI;
	
	SPI2->CR1 |= SPI_CR1_CPOL; // clock in idle state is high
	SPI2->CR1 |= SPI_CR1_CPHA; // Data is latched on the rising edge of SCLK
	
	SPI2->CR1 |= SPI_CR1_SPE;  // SPI en
}

void GPIO_Init(void)
{
	/*
	PB12 NSS  - out push-pull 
	PB13 SCK  - alternative function push-pull
	PB14 MISO - Input pull up
	PB15 MOSI - alternative function push-pull
	*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// PB12 NSS  - out push-pull ------------------------------->
	// general purpose out
	GPIOB->MODER |= GPIO_MODER_MODER12_0;
	GPIOB->MODER &= ~GPIO_MODER_MODER12_1;
	
	GPIOB->OTYPER  &= GPIO_OTYPER_OT_12;       // push-pull
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13; // high speed
	
	// PB13 SCK  - alternative function push-pull -------------->
	
	GPIOB->MODER &= ~GPIO_MODER_MODER13_0;
	GPIOB->MODER |=  GPIO_MODER_MODER13_1;
	
	GPIOB->OTYPER &= GPIO_OTYPER_OT_13;        // push-pull
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13; // high speed
	
	GPIOB->AFR[1] |= (0x05 << 20); // AF5 SPI2 0010 af config
	
	// PB15 M0SI  - alternative function push-pull -------------->
	GPIOB->MODER &= ~GPIO_MODER_MODER15_0;
	GPIOB->MODER |=  GPIO_MODER_MODER15_1;
	
	GPIOB->OTYPER &= GPIO_OTYPER_OT_15;        // push-pull
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15; // high speed
	
	GPIOB->AFR[1] |= (0x05 << 28); // AF5 SPI2 0010 af config
	
	// PB14 MISO - Input pull up -------------->
	GPIOB->MODER &= ~GPIO_MODER_MODER14_0;
	GPIOB->MODER |=  GPIO_MODER_MODER14_1;
	GPIOB->AFR[1] |= (0x05 << 24);
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
	
	return (uint8_t)data;
}

uint8_t SPI2_ReadWriteReg(uint8_t reg, uint8_t data)
{
	while ( !(SPI2->SR & SPI_SR_TXE) )
	{
	
	}
	
	SPI2_NSS_SET;	
	
	SPI2->DR = reg;
	
	while ( !(SPI2->SR & SPI_SR_RXNE) )
	{
	
	}
	
	(void) SPI2->DR;
	
	while ( !(SPI2->SR & SPI_SR_TXE) )
	{
	
	}
	
	SPI2->DR = data;
	
	while ( !(SPI2->SR & SPI_SR_RXNE) )
	{
	
	}
	
	data = SPI2->DR;
	
	SPI2_NSS_RST;
	
	return (uint8_t)data;
}


void 		SPI2_BurstRead(uint8_t addr, uint8_t *data, uint32_t size)
{
	while ( !(SPI2->SR & SPI_SR_TXE) )
	{
	
	}
	
	SPI2_NSS_SET;	
	
	SPI2->DR = addr;
	
	while ( !(SPI2->SR & SPI_SR_RXNE) )
	{
	
	}
	
	(void) SPI2->DR;
	
	for (uint32_t i = 0; i < size; i++)
	{
		while ( !(SPI2->SR & SPI_SR_TXE) )
		{
	
		}
	
		SPI2->DR = 0x00;
	
		while ( !(SPI2->SR & SPI_SR_RXNE) )
		{
	
		}
	
		data[i] = SPI2->DR;
	}

	SPI2_NSS_RST;
}
