#include "stm32f4xx.h"
#include "timer_delays.h"
#include "spi2.h"
#include "uart2.h"
#include "icm-20948.h"

void RCC_Init(void);
void GPIO_Init(void);

int main(void)
{
	RCC_Init();
	TIM_DelaysInit(100);
	GPIO_Init();
	SPI2_Init();
	UART2_Init();

	
	delay_ms(500);
	
	while (1){		
		//read from register WHO_AM_I 0x00
		UART2_SendByte(SPI2_ReadWriteReg(0x80 | 0x00, 0x00));
		delay_ms(500);
		
		// write to register 0x10
		SPI2_ReadWriteReg(0x10, 0x01);
		delay_ms(500);

		//read from register  0x10
		UART2_SendByte(SPI2_ReadWriteReg(0x80 | 0x10, 0x00));
		delay_ms(500);		
		GPIOC->ODR ^= GPIO_ODR_ODR_13;
		
		
		// write to reg 0x11
		///////////////////////////////////////////////////////
		SPI2_ReadWriteReg(0x11, 0x01);
		delay_ms(500);

		//read from register  0x11
		UART2_SendByte(SPI2_ReadWriteReg(0x80 | 0x11, 0x00));
		delay_ms(500);		

		
		// write to reg 0x12
		///////////////////////////////////////////////////////
		SPI2_ReadWriteReg(0x12, 0x01);
		delay_ms(500);
		
		//read from register  0x12
		UART2_SendByte(SPI2_ReadWriteReg(0x80 | 0x12, 0x00));
		delay_ms(500);		
		UART2_SendByte('a');
		GPIOC->ODR ^= GPIO_ODR_ODR_13;
	}
	
}

void GPIO_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	
	GPIOC->MODER |= GPIO_MODER_MODER13_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER13_1;
	
	GPIOC->OTYPER &= GPIO_OTYPER_OT_13; // open-drain
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13; // high speed
	
	//GPIOC->ODR &= ~GPIO_ODR_ODR_13;
	//GPIOC->ODR |= GPIO_ODR_ODR_13;
}

void RCC_Init(void)
{
	RCC->CR |= RCC_CR_HSEON;
	
	while ( !(RCC->CR & RCC_CR_HSERDY))
	{
	
	}
	
	FLASH->ACR |= FLASH_ACR_ICEN; 
	FLASH->ACR |= FLASH_ACR_LATENCY_3WS; // Latency 2.7V-3.6V for 100MHz should be 4 cycles (3WS)
	
	// right side of  clock tree
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB2 =  50MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB1 = 100MHz
	
	RCC->CFGR &= ~RCC_CFGR_HPRE;  // AHB  = 100MHz
	
	// left side of clock tree
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC; // HSE 
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4; // 16   25     /  16 = 1.5625
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_3; // 128  1.5625 * 128 = 200
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;  // 2		 200    /   2 = 100  
	
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ_2; // USB OTG FS
	
	RCC->CR |= RCC_CR_PLLON;
	
	while( !(RCC->CR & RCC_CR_PLLRDY) )
	{
	
	}
	
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	
	while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) )
	{
	
	}
	
}

