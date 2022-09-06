#include <stdio.h>

#include "stm32f10x.h"

#include "uart1.h"
#include "timer_delays.h"


//----------------------------------------------------------
void RCC_init(void);
void GPIO_init(void);


int main(void)
{	
	char str[16];
	float angle = 0;
	
	RCC_init();
	delays_init(72);
	GPIO_init();
	UART1_Init();

	
	//LCD_SetPos(0, 0);
	//LCD_String("Hello!");
	
	delay_ms(500);
	
	char s[] = "Hello world!\n"; 
	
  while(1)
  {
		GPIOC->ODR ^= GPIO_ODR_ODR13;
		UART1_SendString(s);
		//UART1_SendByte(UART1_ReadByte());
		delay_ms(100);
	}
}
//----------------------------------------------------------

void RCC_init(void)
{
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);
    
  while (!(RCC->CR & RCC_CR_HSERDY))
  {
    
  }
    
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
    
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV1; // APB1
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2
    
  RCC->CFGR |= RCC_CFGR_PLLSRC_HSE; // SOURCE HSE
  RCC->CFGR |= RCC_CFGR_PLLMULL9;   // PLL = 4 * 7 == 28
    
  RCC->CR |= RCC_CR_PLLON; // PLL EN
    
  while ((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
   
  }
        
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL; // select source SYSCLK == PLL
    
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1)
  {
    
  }
}

void GPIO_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // enabling clock to C port
    
  GPIOC->CRH |= GPIO_CRH_CNF13_0;  // configure for push-up output 
  GPIOC->CRH |= GPIO_CRH_MODE13_0; // max clock freaquency 10MHz
}

