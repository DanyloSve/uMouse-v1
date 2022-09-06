/*
	TIM11        - used for delay_us()
	TIM9 & TIM10 - used for delay_ms()
*/

#ifndef _TIMER_DELAYS_H__
#define _TIMER_DELAYS_H__

#include "stdint.h"

void TIM_DelaysInit(uint8_t sysFrequencyMHz);

void delay_ms(uint16_t ms);
void delay_us(uint16_t us);


#endif /*_TIMER_DELAYS_H__*/
