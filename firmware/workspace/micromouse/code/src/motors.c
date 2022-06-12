#include "motors.h"


static uint16_t TIM4_dutyCycle;
static uint16_t TIM3_dutyCycle;

static void GPIO_Init(void);

/*
  DRV8838 max freq 250kHz
	https://learn.adafruit.com/improve-brushed-dc-motor-performance/choosing-decay-mode-and-pwm-frequency
	Optimal freq for dc motor is between 50 and 100Hz

	PWM freq = Fclk / (PSC*ARR) [100MHz / (1000 * 1000)]
	PWM duty = CCR4 / (ARR) 	
*/

#define INITIAL_DUTY_PWM 150

void Motors_Init(void)
{
	// max 250kHz
	// timer clocks - 100MHz
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM3EN;
	
	GPIO_Init();
	
	// TIM4 CH2
	TIM4->CR1 &= ~TIM_CR1_CMS; // Edge-aligned mode.
	TIM4->CR1 &= ~TIM_CR1_DIR; // Counter used as upcounter
	
	// PWM mode 1 In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1
	TIM4->CCMR1 |=  TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	TIM4->CCMR1 &= ~TIM_CCMR1_OC2M_0;
	
	
	TIM4->CCMR1 |= TIM_CCMR1_OC2PE; // Output compare 1 preload enable
	
	TIM4->CCER  |= TIM_CCER_CC2E; // 2 channel en
	
	TIM4->CR1   |= TIM_CR1_ARPE;   // autoreload en
	
	TIM4->PSC  = (uint16_t) 1000 - 1;
	TIM4->ARR  = (uint16_t) 1000 - 1;
	
	// TIM3 CH1
	TIM3->CR1 &= ~TIM_CR1_CMS; // Edge-aligned mode.
	TIM3->CR1 &= ~TIM_CR1_DIR; // Counter used as upcounter
	
	// PWM mode 1 In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1
	TIM3->CCMR1 |=  TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM3->CCMR1 &= ~TIM_CCMR1_OC1M_0;
	
	
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE; // Output compare 1 preload enable
	
	TIM3->CCER  |= TIM_CCER_CC1E; // 1 channel en
	
	TIM3->CR1   |= TIM_CR1_ARPE;   // autoreload en
	
	TIM3->PSC  = (uint16_t) 1000 - 1;
	TIM3->ARR  = (uint16_t) 1000 - 1;
	
  // TIM en

	TIM4->CR1 |= TIM_CR1_CEN;
	TIM3->CR1 |= TIM_CR1_CEN;
	
	TIM3->CCR1 = 0;
	TIM4->CCR1 = 0;
	
	TIM4->EGR |= TIM_EGR_UG;
	TIM3->EGR |= TIM_EGR_UG;
}

void GPIO_Init(void)
{
	RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOAEN;
	
	// PB7 TIM4 CH2 
	// AFIO
	GPIOB->MODER |=   GPIO_MODER_MODER7_1 | GPIO_MODER_MODER4_1;
	GPIOB->MODER &=  ~(GPIO_MODER_MODER7_0 | GPIO_MODER_MODER4_0);
	
	// push-pull
	GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPDR11   | GPIO_PUPDR_PUPDR12);
	
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_7;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_4;
	
	// high speed
	//GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR7 | GPIO_OSPEEDER_OSPEEDR4);
	
	// AF1
	GPIOB->AFR[0] |= (0x02 << (4 * 7)) | (0x02 << (4 * 4));
	

	// AF2
	
	// motor direction
	// A12 A11 output
	GPIOA->MODER |=   GPIO_MODER_MODER11_0 | GPIO_MODER_MODER12_0;
	GPIOA->MODER &= ~(GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1);
	
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_12);
	
	GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPDR11   | GPIO_PUPDR_PUPDR12);
	//GPIOA->PUPDR  |=  (GPIO_PUPDR_PUPDR11_0 | GPIO_PUPDR_PUPDR12_0);
	
	GPIOA->ODR &= ~GPIO_ODR_ODR_12; // direction forward
	GPIOA->ODR &= ~GPIO_ODR_ODR_11; // direction forward
	
	
}

void Motors_MR(const uint16_t speed, const uint8_t direction)
{
	// B7 A12
	TIM4->CCR2 = (uint16_t)speed;
	(direction == MOTORS_DRCTN_FRWRD) ? (GPIOA->ODR &= ~GPIO_ODR_ODR_12) : (GPIOA->ODR |= GPIO_ODR_ODR_12);
	
}

void Motors_ML(const uint16_t speed, const uint8_t direction)
{
	// A11
	TIM3->CCR1 = (uint16_t)speed;
	(direction == MOTORS_DRCTN_FRWRD) ? (GPIOA->ODR &= ~GPIO_ODR_ODR_11) : (GPIOA->ODR |= GPIO_ODR_ODR_11);
}

void Motors_Control(uint8_t command[2])
{
	int8_t motorLCmd = command[0];
	int8_t motorRCmd = command[1];
	
	if (motorLCmd < 0)
	{
		Motors_ML((uint16_t)(motorLCmd * -1)*10 , MOTORS_DRCTN_FRWRD);
	}
	else
	{
		Motors_ML((uint16_t)(motorLCmd)*10, MOTORS_DRCTN_BCKWRD);
	}
	
	if (motorRCmd < 0)
	{
		Motors_MR((uint16_t)(motorRCmd * -1)*10, MOTORS_DRCTN_BCKWRD);
	}
	else
	{
		Motors_MR((uint16_t)(motorRCmd)* 10, MOTORS_DRCTN_FRWRD);
	}
}



