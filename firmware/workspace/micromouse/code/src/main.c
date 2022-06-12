#include <stdio.h>

#include "stm32f4xx.h"

#include "main.h"
#include "uart1.h"
#include "timer_delays.h"
#include "rgb_led.h"
#include "motors.h"
#include "adc.h"
#include "mpu6050.h"
#include "encoder.h"


uint8_t  UART1_RXdata[3];
float  gyro[3];
float  angle;

#define GYRO_THREESHOLD 5.0f
#define CNTR_40MS 	7

int main(void)
{
	uint8_t cntrToSendData = 0;
	
	RCC_Init();
	TIM_DelaysInit(100);

	Motors_Init();
	
	RGB_LED_Init();
	
	UART1_Init();
	
	ADC_Init();
	I2C1_Init();
	MPU6050_Init();
	Encoder_Init();

	
	
	//Motors_MR(180, MOTORS_DRCTN_FRWRD);
	//Motors_ML(180, MOTORS_DRCTN_FRWRD);
	
	ENC_RST();
	
	while (1)
	{
		
		if ( MPU6050_IsDataReady() ) // MPU6050
		{
			
			MPU6050_ReadRawData();

			MPU6050_WaitUntilDataIsReceived();
			MPU6050_Read(gyro);

			GPIOC->ODR ^= GPIO_ODR_ODR_13;

			if (gyro[2] > GYRO_THREESHOLD || gyro[2] < -GYRO_THREESHOLD)
			{
				angle += gyro[2] * 0.005;
			}

			cntrToSendData++;
			
		}
		
		//
		if (cntrToSendData == CNTR_40MS)
		{
			
			ADC_ReadSensors();
			
			UART1_DMAbufferTX[0]  =  ADC_bat >> 8;
			UART1_DMAbufferTX[0] |= (ADC_data[5] - ADC_data[1] > 3200) ? (1 << 7) : 0; // ls
			UART1_DMAbufferTX[0] |= (ADC_data[7] - ADC_data[3] > 3200) ? (1 << 6) : 0; // lf
			UART1_DMAbufferTX[0] |= (ADC_data[4] - ADC_data[0] > 3200) ? (1 << 5) : 0; // rf
			UART1_DMAbufferTX[0] |= (ADC_data[6] - ADC_data[2] > 3200) ? (1 << 4) : 0; // rs
			
			UART1_DMAbufferTX[1]  =  (uint8_t)ADC_bat;

			
			uint16_t angleTypeConverted = (uint16_t)((int16_t)(angle));
			UART1_DMAbufferTX[2]  =  (uint8_t)(angleTypeConverted >> 8);
			UART1_DMAbufferTX[3]  =  (uint8_t)(angleTypeConverted);
			UART1_DMAbufferTX[4]  =  (uint8_t)(ENC_ML_VAL >> 8);
			UART1_DMAbufferTX[5]  =  (uint8_t)(ENC_ML_VAL);
			
			UART1_DMA_Send();
			cntrToSendData = 0;
			
			
			ADC_ReadBattery();
			RGB_LED_XOR(RGB_GREEN);
		}
		
		if (UART1_DMAdataRXisReady)
		{
			UART1_ReadDMAdataRX(UART1_RXdata);
			Motors_Control(UART1_RXdata);
			
			
			if ((UART1_RXdata[2] & 0x01))
			{
				angle = 0.0f;
			}
			
			if ((UART1_RXdata[2] & 0x02))
			{
				ENC_RST();
			}
			RGB_LED_XOR(RGB_BLUE);
		}
		
		
	}
	
	
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


