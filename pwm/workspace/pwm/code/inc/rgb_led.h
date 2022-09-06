#ifndef _RGB_LED_H__
#define _RGB_LED_H__

#include "stm32f4xx.h"

#define	RGB_RED   GPIO_ODR_ODR_10
#define	RGB_GREEN GPIO_ODR_ODR_11
#define	RGB_BLUE  GPIO_ODR_ODR_12

#define RGB_LED_Set(color) GPIOA->ODR |= (color);
#define RGB_LED_Reset(color) GPIOA->ODR &= ~(color);

inline void RGB_LED_Init(void)
{
	// A10 A11 A12
	// pullup drain
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER   |=  (GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER12_0);
	GPIOA->MODER   &= ~(GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1);
	
}

#endif /*_RGB_LED_H__*/
