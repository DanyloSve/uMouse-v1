#include "uart1.h"

void UART1_Init(void)
{
	GPIO_Init();
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	USART1->BRR = 625; // baud - 38'400 freq - 72MHz
	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	
	USART1->CR1 |= USART_CR1_RXNEIE;
	
}

void GPIO_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	
	// PA9 - TX
	GPIOA->CRH |= GPIO_CRH_MODE9_1; // output max speed 50MHz
	GPIOA->CRH &= ~GPIO_CRH_MODE9_0;
	GPIOA->CRH |= GPIO_CRH_CNF9_1; // alter func out push-pull
	GPIOA->CRH &= ~GPIO_CRH_CNF9_0;
	// PA10 - RX
	GPIOA->CRH &= ~GPIO_CRH_MODE10; // in
	GPIOA->CRH &= ~GPIO_CRH_CNF10_0; // 
	GPIOA->CRH |= GPIO_CRH_CNF10_0; // floating in
	GPIOA->ODR |= GPIO_ODR_ODR10;
}

void UART1_SendByte(uint8_t data)
{
	while ( (USART1->SR & USART_SR_TXE) !=  USART_SR_TXE)
	{
	
	}
	
	USART1->DR = data;
}

uint8_t UART1_ReadByte(void)
{
	while ( !(USART1->SR & USART_SR_RXNE))
	{
	
	}
	
	return USART1->DR;
}

void UART1_SendString(char *data)
{
	while(*data != '\0')
	{
		UART1_SendByte((uint8_t)*(data++));
	}
}
