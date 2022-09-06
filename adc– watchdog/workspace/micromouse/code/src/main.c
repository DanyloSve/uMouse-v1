#include <stdio.h>

#include "stm32f4xx.h"

#include "main.h"
#include "uart1.h"
#include "timer_delays.h"
#include "rgb_led.h"
#include "motors.h"
#include "ir-sensors.h"

volatile extern uint8_t UART1_DMAbufferTX[9];
volatile extern uint8_t UART1_DMAbufferRX[3];
volatile extern uint16_t ADC_DMAdata[6]; 
volatile extern uint16_t ADC_data;

int main(void)
{
	RCC_Init();
	TIM_DelaysInit(100);

	RGB_LED_Init();
	
	UART1_Init();
	Motors_Init();
	
	ADC_InitS();
	
	while (1)
	{
		ADC1->CR2 |= ADC_CR2_SWSTART;
//		
		RGB_LED_XOR(RGB_GREEN);

		while ( !(ADC1->SR & ADC_SR_EOC))
		{
			
		}
		ADC_data = ADC1->DR;
//		sprintf((char *)UART1_DMAbufferTX, "h%d %d %d\n\r", ADC_DMAdata[0], ADC_DMAdata[1], ADC_DMAdata[2]);
		sprintf((char *)UART1_DMAbufferTX, "%d\n", ADC_data);
		UART1_DMA_Send();
		delay_ms(100);
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
	uint8_t isDMAWorking = 0;
	
	/*read MPU6050*/
	//--------------------------------------------------------------------------->
//	if ( MPU6050_IsDataReady() ) // MPU6050
//	{
//		MPU6050_ReadRawData();
//		isDMAWorking = 1;
//	}
//	/* Read ADC*/
//	
//	/*Wait until DMA became idle*/
//	if (isDMAWorking)
//	{
//		MPU6050_WaitUntilDataIsReceived();
//		isDMAWorking = 0;
//	}
//	
//	/*read QMC */ 
//	//--------------------------------------------------------------------------->
//  if ( QMC5883L_IsDataReady() ) // QMC5883L
//	{
//		QMC5883L_ReadRawData();
//		isDMAWorking = 1;
//	}
//	
//	/*Calculate speed*/
//	
//	/*Wait until DMA became idle*/
//	if (isDMAWorking)
//	{
//		QMC5883L_WaitUntilDataIsReceived(); 
//		isDMAWorking = 0;
//	}
//	
//	
//	
//	/*calculate angle*/
//	
//// this section should be commented before debuuging uart rx------------------------------------------------------->	
//	/*UART TX*/
//	sprintf((char *)(UART2_DMAbufferTX), "123456789");
//	UART2_DMA_Send();
//	
//	UART2_WaitUntilDataIsSend();
//	
//	
//	/*UART RX*/
//	uint8_t uartDataIsReceived = 0;
//	volatile uint32_t waitCntr = 0;
//	
//	while ( waitCntr < 0x4a0 ) // 260us delay or 60us between 2 writes
//	{
//		if ((DMA1->HISR & DMA_HISR_TCIF5))
//		{
//			uartDataIsReceived =  1;
//			
//			break;
//		}
//		
//		waitCntr += 1;
//	}
//	
//	if (!uartDataIsReceived)
//	{
//		sprintf((char *)(UART2_DMAbufferTX), "errorabcd");
//		UART2_DMA_Send();
//	}
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

void ADC1_BatMonitoring(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	
	// B1 - [6] CH9 for battery
	ADC1->SQR2 |=  0x09;					   // CH9
	
	// PB1
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN);
	

	// analog in
	// PB1
	GPIOB->MODER |= GPIO_MODER_MODER1;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR1;

	ADC->CCR |=	 ADC_CCR_ADCPRE_0;
	ADC->CCR &=	~ADC_CCR_ADCPRE_1;
	
	ADC1->CR1 &= ~ADC_CR1_RES;   // 12 bit
	ADC1->CR2 &= ~ADC_CR2_ALIGN; // right alignment
	
	//ADC1->CR1 |= ADC_CR1_EOCIE;  // end of 
	ADC1->CR1 |= ADC_CR1_SCAN;
	
	
	// start
	ADC1->CR2 |= ADC_CR2_CONT; // contimuous mode
	ADC1->CR2 |= ADC_CR2_ADON; // enable conversation
	
	delay_us(50000);
	
	ADC1->CR2 |= ADC_CR2_SWSTART;
}
