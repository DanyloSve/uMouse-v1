#include "stm32f10x.h"

//----------------------------------------------------------
void RCC_init(void);
void GPIO_init(void);
void TIM1_delayInit(void);
void GPIO_init(void);

void delay_us(uint16_t us);

void TIM2_delay_us_init(uint8_t sysFreqMHz);
void TIM34_delay_ms_init(uint8_t sysFreqMHz);
void delay_ms(uint16_t ms);

int main(void)
{	
	RCC_init();
	TIM1_delayInit();
	TIM34_delay_ms_init(64);
	GPIO_init();
	

  while(1)
  {
			GPIOC->ODR ^= GPIO_ODR_ODR13;	
			delay_ms(500);
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
  //RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE_Div1; // SOURCE HSE/2 == 4 
  RCC->CFGR |= RCC_CFGR_PLLMULL8; // PLL = 4 * 7 == 28
    
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

void TIM1_delayInit(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  
  TIM1->SMCR &= ~TIM_SMCR_SMS;  // prescaler is clocked directly by the internal clock
  
  TIM1->PSC = 64 - 1;
  TIM1->ARR = 0xffff - 1;

  TIM1->EGR = TIM_EGR_UG;   // update generation - rinitialize the counter and update of the regidters

  TIM1->CR1 =  TIM_CR1_CEN; // tim en
}

void delay_us(uint16_t us)
{
	TIM1->CNT = 0x00;  // set the counter value to a 0
	
	while (TIM1->CNT < us)
	{
		// wait for the counter to reach the us input in the parameter
	}
}

void TIM2_delay_us_init(uint8_t sysFreqMHz)
{
	// TIM2
	//----------------------------------------------------
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // en clock for TIM2
	
	TIM2->CR1 &= ~TIM_CR1_DIR;   // count up
	TIM2->CR1 &= ~TIM_CR1_CMS;   // edge aligned mode
	
	TIM2->SMCR &= ~TIM_SMCR_SMS; // prescaler is clocked by the internal clock
	
	TIM2->PSC = sysFreqMHz - 1; // prescaler
	TIM2->ARR = 0xffff - 0x01;  // counter - max possible value
	
	TIM2->EGR |= TIM_EGR_UG;     // generation of update the registers
	TIM2->CR1 |= TIM_CR1_CEN;    // en tim2
}

void TIM34_delay_ms_init(uint8_t sysFreqMHz)
{
	// TIM3 - master
	//---------------------------------------------------
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // en clock for TIM3
	
	TIM3->CR2 |= TIM_CR2_MMS_1; 				// CNT_EN used as triger output
	
	TIM3->PSC = sysFreqMHz - 1;         // prescaler 
	TIM3->ARR = 1000 - 1;               // tim3 counts to 1 us
	
	TIM3->EGR |= TIM_EGR_UG;            // generation of update the registers
	TIM3->CR1 |= TIM_CR1_CEN;           // en tim3
	
	
	// TIM4 - slave
	//---------------------------------------------------
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // en clock for TIM4
	
	TIM4->SMCR |= TIM_SMCR_TS_1;        // ITR2 - TIM3
	TIM4->SMCR |= TIM_SMCR_SMS;         // rising edges of the selected trigger (TRGI) clock the counter
	
	TIM4->PSC = 0x00;
	TIM4->ARR = 0xffff - 0x01;        	// counter - max possible value
	
	TIM4->EGR |= TIM_EGR_UG;            // generation of update the registers
	TIM4->CR1 |= TIM_CR1_CEN;           // en tim4
}

void delay_ms(uint16_t ms)
{
	TIM3->CNT = 0x00; 
	TIM4->CNT = 0x00;
	
	while (TIM4->CNT < ms)
	{
		// wait for the counter to reach the ms input in the parameter 
	}
}
