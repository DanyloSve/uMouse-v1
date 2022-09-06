#include "timer_delays.h"

static void TIM11_delay_us_init(uint8_t sysFrequencyMHz);
static void TIM9_2_delay_ms_init(uint8_t sysFrequencyMHz);

void TIM_DelaysInit(uint8_t sysFrequencyMHz)
{
	TIM11_delay_us_init(sysFrequencyMHz);
	TIM9_2_delay_ms_init(sysFrequencyMHz);
}

void TIM11_delay_us_init(uint8_t sysFrequencyMHz)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;	
	
	TIM11->PSC = sysFrequencyMHz - 1;
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


void TIM9_2_delay_ms_init(uint8_t sysFrequencyMHz)
{
	// TIM 10 - master
	//===============================================>
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	
	
	TIM2->CR2 |= TIM_CR2_MMS_1;
	
	TIM2->PSC = sysFrequencyMHz - 1; // Timer freq is 1 MHz
	TIM2->ARR = 1000            - 1; // 1 ms 
	
	// TIM 9 - slave
	//================================================>
	RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;	
	
	TIM9->PSC = 0x00;
	TIM9->ARR = 0xffff - 0x01;
	
	TIM9->SMCR &= ~TIM_SMCR_TS; // trigger connection - ITR-2 (TIM11)
	TIM9->SMCR  |= TIM_SMCR_SMS;   // Rising edges of the selected trigger
	
	TIM2->EGR |= TIM_EGR_UG;
	TIM9->EGR  |= TIM_EGR_UG;
	
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM9->CR1  |= TIM_CR1_CEN;
}

void delay_ms(uint32_t ms)
{
	TIM2->CNT = 0;
	TIM9->CNT = 0;
	
	while (TIM9->CNT < ms)
	{
	
	}
}


void TIM9_10_delay_ms_init(uint8_t sysFrequencyMHz)
{
	// TIM 10 - master
	//===============================================>
	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;	
	
	TIM10->CR2 |= TIM_CR2_MMS_1;
	
	TIM10->PSC = sysFrequencyMHz - 1; // Timer freq is 1 MHz
	TIM10->ARR = 1000            - 1; // 1 ms 
	
	TIM10->CCR1 = 1000            - 1;
	// TIM 9 - slave
	//================================================>
	RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;	
	
	TIM9->PSC = 0x00;
	TIM9->ARR = 0xffff - 0x01;
	
	TIM9->SMCR &= ~TIM_SMCR_TS; // trigger connection - ITR-2 (TIM11)
	TIM9->SMCR  |= TIM_SMCR_SMS;   // Rising edges of the selected trigger
	
	TIM2->EGR |= TIM_EGR_UG;
	TIM9->EGR  |= TIM_EGR_UG;
	
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM9->CR1  |= TIM_CR1_CEN;
}


