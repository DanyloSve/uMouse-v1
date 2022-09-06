#include "stm32f4xx.h"

void RCC_Init(void);
void TIM11_delay(uint8_t freq);
void delay_us(uint16_t us);


int main(void)
{
	RCC_Init();
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	
	GPIOC->MODER |= GPIO_MODER_MODER13_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER13_1;
	
	GPIOC->OTYPER &= GPIO_OTYPER_OT_13; // open-drain
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13; // high speed
	GPIOC->ODR ^= GPIO_ODR_ODR_13;
	
	TIM11_delay(100);
	
	while (1)
	{
			delay_us(50000);
			GPIOC->ODR ^= GPIO_ODR_ODR_13;
	}
	
}



void TIM11_delay(uint8_t freq)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;	
	
	TIM11->PSC = freq - 1;
	TIM11->ARR = 0xffff - 0x01;
	
	TIM11->EGR |= TIM_EGR_UG;
	
	TIM11->CR1 |= TIM_CR1_CEN;
}

void delay_us(uint16_t us)
{
	TIM11->CNT = 0;
	
	while (TIM11->CNT < us)
	{
	
	}
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

