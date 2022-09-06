#ifndef _RGB_LED_H__
#define _RGB_LED_H__

#include "stm32f4xx.h"

#define	RGB_RED   GPIO_ODR_ODR_13
#define	RGB_GREEN GPIO_ODR_ODR_14
#define	RGB_BLUE  GPIO_ODR_ODR_15

#define RGB_LED_Set(color) GPIOB->ODR |= (color);
#define RGB_LED_Reset(color) GPIOB->ODR &= ~(color);
#define RGB_LED_XOR(color) GPIOB->ODR ^= (color);

inline void RGB_LED_Init(void)
{
	// A8 B15 A14
	RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOBEN;

	GPIOB->MODER |=  GPIO_MODER_MODER13_0 |
									 GPIO_MODER_MODER14_0 | 
									 GPIO_MODER_MODER15_0;
	
	GPIOB->MODER &=  ~(GPIO_MODER_MODER13_1 |
										 GPIO_MODER_MODER14_1 | 
										 GPIO_MODER_MODER15_1);
	
}

#endif /*_RGB_LED_H__*/
