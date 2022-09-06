#ifndef TIMER_DELAYS_H_
#define TIMER_DELAYS_H_

#include "stm32f10x.h"

void delays_init(uint8_t sysFreqMHz);

static void TIM2_delay_us_init(uint8_t sysFreqMHz);
static void TIM34_delay_ms_init(uint8_t sysFreqMHz);

//-----------------------------------------------------
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);

#endif /*TIMER_DELAYS_H_*/
