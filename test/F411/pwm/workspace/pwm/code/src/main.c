#include <stdio.h>

#include "stm32f4xx.h"


#include "uart2.h"
#include "timer_delays.h"
#include "rgb_led.h"

#include "pwm.h"

#define ARM_MATH_CM4

#include "arm_math.h"

static void RCC_Init(void);
static void GPIO_Init(void);
static void SYSTick_Int_Init(void);

volatile extern uint8_t UART2_DMAbufferTX[9];
volatile extern uint8_t UART2_DMAbufferRX[3];

#define DEBUG_MODE
#if defined(DEBUG_MODE)
	extern volatile int16_t MPU6050_debugArr[12];
	extern volatile char 	 UART2_debugArr[64];
#endif /*DEBUG_MODE*/

int main(void)
{
	RCC_Init();
	TIM_DelaysInit(100);
	
	GPIO_Init();
	RGB_LED_Init();
	
	//UART2_Init();
	
	
	//SYSTick_Int_Init();
	
	#if defined (DEBUG_MODE)
	
		PWM_Init();

		PWM_SetDutyTim3(650);
		PWM_SetDutyTim4(100);

	#else
		SYSTick_Int_Init();
	#endif
	
	while (1)
	{

	}
	
}

void SYSTick_Int_Init(void)
{
	__disable_irq();
	
	SysTick->CTRL = 0x00;
	SysTick->LOAD = (100000 * 2.5) - 1; // delay every 2.5ms
	SysTick->VAL  = 0x00;
	SysTick->CTRL = 0x07;

	__enable_irq();
	
}

/*
	UART send:
	angle (2byte 0 - 360 1.5byte)
	speed (3 bits for integer part(from 0. to 8. V) 8 bits for floating(from .0 to .99) 1.5bytes)
	4xIR sensor (uint8_t 4 bits x 8 sensors = 4bytes)
	Battary level (6.45 {4 bits for integer part(from 0. to 16. V) 8 bits for floating(from .0 to .99)} 1.5 bytes)
	
	total 9 bits

	UART recieve
	angle (2byte 0 - 360 1.5bytes)
	speed (3 bits for integer part(from 0. to 8. V) 8 bits for floating(from .0 to .99) 1.5bytes)
	total 3 bits

	DMA pririty level:
	MPU6050 I2C - very high
	QMC         - very high
	UART TX		- medium
	UART RX		- low

TODO:--------------------------------------------------------------------------------------------------------------------->
	corret uart bytes  or SysTick_Handler  interrupt due to time taken for calculation angle of rotation
*/

void SysTick_Handler(void)
{
	GPIOC->ODR ^= GPIO_ODR_ODR_13;
	
}

void GPIO_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	
	GPIOC->MODER |= GPIO_MODER_MODER13_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER13_1;
	
	GPIOC->OTYPER &= GPIO_OTYPER_OT_13; // open-drain
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13; // high speed
	GPIOC->ODR ^= GPIO_ODR_ODR_13;
}

void RCC_Init(void)
{
	RCC->CR |= RCC_CR_HSEON;
	
	while ( !(RCC->CR & RCC_CR_HSERDY))
	{
	
	}
	
	FLASH->ACR |= FLASH_ACR_ICEN; 
	FLASH->ACR |= FLASH_ACR_LATENCY_3WS; // Latency 2.7V-3.6V for 100MHz should be 4 cycles (3WS)
	
	// right side of  clock tree
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB2 =  50MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB1 = 100MHz
	
	RCC->CFGR &= ~RCC_CFGR_HPRE;  // AHB  = 100MHz
	
	// left side of clock tree
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC; // HSE 
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4; // 16   25     /  16 = 1.5625
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_3; // 128  1.5625 * 128 = 200
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;  // 2		 200    /   2 = 100  
	
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ_2; // USB OTG FS
	
	RCC->CR |= RCC_CR_PLLON;
	
	while( !(RCC->CR & RCC_CR_PLLRDY) )
	{
	
	}
	
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	
	while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) )
	{
	
	}
	
}


