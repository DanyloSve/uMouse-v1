#include "encoder.h"



static void GPIO_Init(void);

void Encoder_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM5EN;
	GPIO_Init();
	
	// TIM 2 CH1 CH2 ML
	
	TIM2->CR1 |= TIM_CR1_ARPE; // autoreload enable
	TIM2->CR1 &= ~TIM_CR1_CMS; // edge aligned mode
	TIM2->CR1 &= ~TIM_CR1_DIR; // upcounting
	
	
	TIM2->SMCR |= TIM_SMCR_SMS_0; // Counter counts up/down on TI2FP1 edge depending on TI1FP2
	TIM2->SMCR &= ~TIM_SMCR_SMS_1;
	
	// CC1 channel is configured as input, IC1 is mapped on TI1.
	TIM2->CCMR1 |=  TIM_CCMR1_CC1S_1;
	TIM2->CCMR1 &= ~TIM_CCMR1_CC1S_0;
	

	TIM2->CCER  &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
	TIM2->CCMR1 &= ~TIM_CCMR1_IC1F;
	
	TIM2->CCER  &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
	TIM2->CCMR1 &= ~TIM_CCMR1_IC2F;
	
	TIM2->EGR |= TIM_EGR_UG;  // update generation
	TIM2->CR1 |= TIM_CR1_CEN;	// counter 
	
	// TIM 5 CH1 CH2 MR
	TIM5->CR1 |= TIM_CR1_ARPE; // autoreload enable
	TIM5->CR1 &= ~TIM_CR1_CMS; // edge aligned mode
	TIM5->CR1 &= ~TIM_CR1_DIR; // upcounting
	
	
	TIM5->SMCR |= TIM_SMCR_SMS_0; // Counter counts up/down on TI2FP1 edge depending on TI1FP2
	TIM5->SMCR &= ~TIM_SMCR_SMS_1;
	
	// CC1 channel is configured as input, IC1 is mapped on TI1.
	TIM5->CCMR1 |=  TIM_CCMR1_CC1S_1;
	TIM5->CCMR1 &= ~TIM_CCMR1_CC1S_0;
	

	TIM5->CCER  &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
	TIM5->CCMR1 &= ~TIM_CCMR1_IC1F;

	TIM5->CCER  &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
	TIM5->CCMR1 &= ~TIM_CCMR1_IC2F;
	
	TIM5->EGR |= TIM_EGR_UG;  // update generation
	TIM5->CR1 |= TIM_CR1_CEN;	// counter 
}

void GPIO_Init(void)
{
	RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
	
	// TIM 2 CH1 CH2
	// PB3 PA15
	// input afio AF1
	// Motor Left
	
	// PA15 
	GPIOA->MODER   |=  GPIO_MODER_MODER15_1;
	GPIOA->MODER   &= ~GPIO_MODER_MODER15_0;
	
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR15;

	GPIOA->AFR[1] |= (0x01 << 4 * 7);

	// PB3
	GPIOB->MODER   |=  GPIO_MODER_MODER3_1;
	GPIOB->MODER   &= ~GPIO_MODER_MODER3_0;
	
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR3;
	
	GPIOB->AFR[0] |= (0x01 << 4 * 3);
	
	
	// TIM 5 CH1 CH2
	// PA0 PA1
	// input afio AF2
	// Motor Right
	GPIOA->MODER   |=   GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1;
	GPIOA->MODER   &= ~(GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0);
	
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR1);
	
	GPIOA->AFR[0] |= ( (0x02) | (0x02 << 4 * 1) );
}
