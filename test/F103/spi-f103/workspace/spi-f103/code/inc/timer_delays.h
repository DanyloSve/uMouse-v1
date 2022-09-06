#ifndef TIMER_DELAYS_H_
#define TIMER_DELAYS_H_

#include "stm32f10x.h"

void delays_init(uint8_t sysFreqMHz);

static void TIM2_delay_us_init(uint8_t sysFreqMHz);
static void TIM34_delay_ms_init(uint8_t sysFreqMHz);


//-----------------------------------------------------
__inline void delay_us(uint16_t us)
{
	TIM2->CNT = 0x00;
	
	while (TIM2->CNT < us)
	{
		// wait for the counter to reach the us input in the parameter
	}
}

__inline void delay_ms(uint16_t ms)
{
	TIM3->CNT = 0x00; 
	TIM4->CNT = 0x00;
	
	while (TIM4->CNT < ms)
	{
		// wait for the counter to reach the ms input in the parameter 
	}
}

#endif /*TIMER_DELAYS_H_*/
