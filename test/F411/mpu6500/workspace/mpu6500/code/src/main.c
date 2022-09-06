#include <stdio.h>

#include "stm32f4xx.h"
#include "timer_delays.h"
#include "spi2.h"
#include "uart2.h"
#include "mpu6500.h"

void RCC_Init(void);
void GPIO_Init(void);

extern char UART2_DMAbufferSend[128];
extern float DEBUG_DATA_OUT[64];
extern float gyroBias[3];

int main(void)
{
	
	RCC_Init();
	TIM_DelaysInit(100);
	delay_us(50000);
	delay_us(50000);
	
	GPIO_Init();
	SPI2_Init();
	UART2_Init();	
	
	MPU6500_Init(0x00, 0x00);
	delay_ms(100);
	GPIOC->ODR ^= GPIO_ODR_ODR_13;
	
	
	float accel[3];
	float gyro[3];
	
	while (1)
	{

		MPU6500_Read(gyro, accel);
		sprintf(&(UART2_DMAbufferSend[0]), "%.3f\t %.3f\t %.3f | %.3f\t %.3f\t %.3f\n", gyro[0], gyro[1], gyro[2], 
		gyroBias[0], gyroBias[1], gyroBias[2]);
		UART2_DMA_Send();
		delay_ms(200);
		

		sprintf(&(UART2_DMAbufferSend[0]), "%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
		DEBUG_DATA_OUT[0],DEBUG_DATA_OUT[1],
		DEBUG_DATA_OUT[2],DEBUG_DATA_OUT[3],DEBUG_DATA_OUT[4]); 
	
		
		//sprintf(&(UART2_DMAbufferSend[0]), "z %.3f\n", gyro[2]);
		UART2_DMA_Send();
		delay_ms(200);
		
	}
	
}

void GPIO_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	
	GPIOC->MODER |= GPIO_MODER_MODER13_0;
	GPIOC->MODER &= ~GPIO_MODER_MODER13_1;
	
	GPIOC->OTYPER &= GPIO_OTYPER_OT_13; // open-drain
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13; // high speed
	//GPIOC->ODR ^= GPIO_ODR_ODR_13;
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
	
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	
	while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) )
	{
	
	}
	
}

