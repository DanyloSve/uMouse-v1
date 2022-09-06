#include "timer_delays.h"

void delays_init(uint8_t sysFreqMHz)
{
	TIM2_delay_us_init(sysFreqMHz);
	TIM34_delay_ms_init(sysFreqMHz);
}

void TIM2_delay_us_init(uint8_t sysFreqMHz)
{
	// TIM2
	//----------------------------------------------------
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // en clock for TIM2
	
	TIM2->CR1 &= ~TIM_CR1_DIR;   // count up
	TIM2->CR1 &= ~TIM_CR1_CMS;   // edge aligned mode
	
	TIM2->SMCR &= ~TIM_SMCR_SMS; // prescaler is clocked by the internal clock
	
	TIM2->PSC = sysFreqMHz - 1; // prescaler
	TIM2->ARR = 0xffff - 0x01;  // counter - max possible value
	
	TIM2->EGR |= TIM_EGR_UG;     // generation of update the registers
	TIM2->CR1 |= TIM_CR1_CEN;    // en tim2
}

void TIM34_delay_ms_init(uint8_t sysFreqMHz)
{
	// TIM3 - master
	//---------------------------------------------------
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // en clock for TIM3
	
	TIM3->CR2 |= TIM_CR2_MMS_1; 				// CNT_EN used as triger output
	
	TIM3->PSC = sysFreqMHz - 1;         // prescaler 
	TIM3->ARR = 1000 - 1;               // tim3 counts to 1 us
	
	TIM3->EGR |= TIM_EGR_UG;            // generation of update the registers
	TIM3->CR1 |= TIM_CR1_CEN;           // en tim3
	
	
	// TIM4 - slave
	//---------------------------------------------------
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // en clock for TIM4
	
	TIM4->SMCR |= TIM_SMCR_TS_1;        // ITR2 - TIM3
	TIM4->SMCR |= TIM_SMCR_SMS;         // rising edges of the selected trigger (TRGI) clock the counter
	
	TIM4->PSC = 0x00;
	TIM4->ARR = 0xffff - 0x01;        	// counter - max possible value
	
	TIM4->EGR |= TIM_EGR_UG;            // generation of update the registers
	TIM4->CR1 |= TIM_CR1_CEN;           // en tim4
}


