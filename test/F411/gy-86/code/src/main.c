#include <stdio.h>


#include "stm32f4xx.h"

#include "i2c1.h"
#include "uart2.h"
#include "uart1.h"
#include "timer_delays.h"
#include "mpu6050.h"

#include "MadgwickAHRS.h"
#include "MahonyAHRS.h"

#include <math.h>

//#define ARM_MATH_CM4

//#include "arm_math.h"

static void RCC_Init(void);
static void GPIO_Init(void);
static void KEY_Init(void);

extern char UART2_DMAbufferSend[128];
extern volatile uint8_t UART1_DMAbufferSend[128];

static volatile uint8_t rawDataQMC883L[6];

#define PI 3.141592f



int main(void)
{
	RCC_Init();
	TIM_DelaysInit(100);
	
	GPIO_Init();
	UART1_Init();
	KEY_Init();
	
	I2C1_Init();
	MPU6050_Init();
	
	const uint8_t addrsCnt = 3;
	uint8_t buff[addrsCnt];

	float gyro[3];
	float accel[3];
	float mag[3];
	float angle = 0.0f;
	uint8_t cntr = 0;
	while (1)
	{
//		if (HMC5883L_IsDataReady())
//		{
//			
//			HMC5883L_ReadRawData();
//			HMC5883L_WaitUntilDataIsReceived();
//		}
		
		if (MPU6050_IsDataReady())
		{
			MPU6050_ReadRawData();
			MPU6050_WaitUntilDataIsReceived();
			MPU6050_Read(accel, gyro);
			GPIOC->ODR ^= GPIO_ODR_ODR_13;
			
			if (gyro[2] > 0.9f || gyro[2] <= -0.9f)
			{
				angle += gyro[2] * 0.005;
			}
			cntr++;

		}
		
		if (cntr == 8)
		{
			sprintf((char *)UART1_DMAbufferTX, "%.2f %.2f\n", angle, gyro[2]);
			UART1_DMA_Send();
			
			cntr = 0;
		}
		
//		if (MPU6050_IsDataReady())
//		{
//			MPU6050_ReadRawData();
//			MPU6050_WaitUntilDataIsReceived();
//			MPU6050_Read(accel, gyro);
//			GPIOC->ODR |= GPIO_ODR_ODR_13;
			
//			if (gyro[2] > 1.0f)
//			{
//				angle += gyro[2] * 0.005;
//			}
					
//		}
		

	
	}
	
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

void KEY_Init(void)
{
	// GPIO
	//-------------------------------------->
	//RCC
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	//
	GPIOA->MODER &= ~GPIO_MODER_MODER0; // input
	
	// pullup
	GPIOA->PUPDR |=  GPIO_PUPDR_PUPDR0_0; 
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0_1;
	
	// EXTI
	//-------------------------------------->
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
	EXTI->RTSR |= EXTI_FTSR_TR0;
	EXTI->PR |= EXTI_PR_PR0;
	
	NVIC_EnableIRQ(EXTI0_IRQn);
	EXTI->IMR |= EXTI_IMR_MR0;
	
}

void EXTI0_IRQHandler(void)
{
	sprintf(&(UART2_DMAbufferSend[0]),  "Key\n");
	UART2_DMA_Send();
	//GPIOC->ODR ^= GPIO_ODR_ODR_13;
	EXTI->PR |= EXTI_PR_PR0;
			
}


