#include "timer_delays.h"
#include "stm32f4xx.h"


void TIM_DelaysInit(uint8_t sysFrequencyMHz)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;	
	
	TIM11->PSC = sysFrequencyMHz - 0x01;
	TIM11->ARR = 0xffff          - 0x01;
	
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


void delay_ms(uint16_t ms)
{
	
	for (volatile uint16_t i = 0; i < ms; ++i)
	{
		TIM11->CNT = 0;
		while (TIM11->CNT < 1000)
		{
		
		}
	}
}

