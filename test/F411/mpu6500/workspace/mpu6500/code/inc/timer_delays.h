/*
	TIM11        - used for delay_us()
	TIM9 & TIM10 - used for delay_ms()
*/

#ifndef _TIMER_DELAYS_H__
#define _TIMER_DELAYS_H__

#include "stm32f4xx.h"

void TIM_DelaysInit(uint8_t sysFrequencyMHz);

void delay_ms(uint32_t ms);
void delay_us(uint16_t us);

static void TIM11_delay_us_init(uint8_t sysFrequencyMHz);
static void TIM9_2_delay_ms_init(uint8_t sysFrequencyMHz);

#endif /*_TIMER_DELAYS_H__*/
