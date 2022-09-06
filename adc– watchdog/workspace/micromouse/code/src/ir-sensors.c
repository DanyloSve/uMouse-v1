#include "ir-sensors.h"
#include "timer_delays.h"
#include "rgb_led.h"
#include "stm32f4xx.h"

static void GPIO_Init (void);
static void DMA_Init  (void);

volatile uint16_t ADC_data;

volatile uint16_t ADC_DMAdata[6]; // without bat


static void GPIO_InitS (void);
/*
	Maximum ADC freq = 36MHz
	Typical ADC freq = 30MHz
	
	Setted ADC val is F[AP2] / ADCPRE = 100MHz / 4 = 25MHz

	For Vbat analog watchdog event should be used
*/
void ADC_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	
	GPIO_Init();
	// CH4 - A4, CH5-A5, CH6 - A6, CH7-PA7
	// CH8 - B0, CH9-B1
	// F[ADC] = 25MHz
	
	ADC->CCR |=	 ADC_CCR_ADCPRE_0;
	ADC->CCR &=	~ADC_CCR_ADCPRE_1;
	
	//    [Order] 
	// B0 - [0] CH8 left forward
	// A7 - [2] CH7 left front
	// A6 - [4] CH6 left side
	
	// A5 - [1] CH5 right side
	// A4 - [5] CH4 right diagonal
	// A3 - [3] CH3 right front
	
	// B1 - [6] CH9 for battery
	
	ADC1->SQR1 &= ~ADC_SQR1_L;
	
//	ADC1->SQR3 |=  0x08 	         | // CH8
//								 0x05 << (5 * 1) | // CH5
//								 0x07 << (5 * 2) | // CH7
//								 0x03 << (5 * 3) | // CH3		
//								 0x06 << (5 * 4) | // CH6		
//								 0x04 << (5 * 5) ; // CH4
//	ADC1->SQR2 |=  0x09;					   // CH9
	
	// adc coversaton takes 12-13 cycles
	// total conversation - (480 + 13) / (25 * 10^6) = 0.13804ms
 	ADC1->SMPR2 |= ADC_SMPR2_SMP8 |		// 480 cycles
								 ADC_SMPR2_SMP5 |
								 ADC_SMPR2_SMP7 |
								 ADC_SMPR2_SMP3 |
								 ADC_SMPR2_SMP6 |
								 ADC_SMPR2_SMP4 |
								 ADC_SMPR2_SMP9 ;
	
	ADC1->CR1 &= ~ADC_CR1_RES;   // 12 bit
	ADC1->CR2 &= ~ADC_CR2_ALIGN; // right alignment
	
	//ADC1->CR1 |= ADC_CR1_EOCIE;  // end of 
	//ADC1->CR1 |= ADC_CR1_SCAN;
	
	ADC1->CR1 |= ADC_CR1_AWDCH_0 | ADC_CR1_AWDCH_3; // ch 9	
	ADC1->CR1 |= ADC_CR1_AWDIE; //analog watchdoe in en
	ADC1->CR1 |= ADC_CR1_AWDEN; // enable on regular channels
	ADC1->CR1 |= ADC_CR1_AWDSGL; // single channel
	
	ADC1->HTR = 0xC00;
	ADC1->LTR = 0x500;
	
	NVIC_EnableIRQ(ADC_IRQn);
	
	// start
	//ADC1->CR2 |= ADC_CR2_CONT; // contimuous mode
	ADC1->CR2 |= ADC_CR2_ADON; // enable conversation
	
	delay_us(50000);
	
	
	// stm32f411 has no nned in calibration
	
	//DMA_Init();
}


void ADC_IRQHandler(void)
{
	if (ADC1->SR & ADC_SR_AWD) // watchdog
	{
		ADC1->CR1 &= ~ADC_CR1_AWDIE;
		
		RGB_LED_XOR(RGB_RED);
		
		ADC1->SR &= ~ADC_SR_AWD;
		ADC1->CR1 |= ADC_CR1_AWDIE;
	}

}

void GPIO_Init(void)
{
	// PA4, PA5, PA6, PA7, PB0, PB1
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
	
	// PA4, PA5, PA6, PA7,
	// analog in
	
	GPIOA->MODER |= (GPIO_MODER_MODER4 | 
									 GPIO_MODER_MODER5 |
									 GPIO_MODER_MODER6 |
									 GPIO_MODER_MODER7 );
	
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4 | 
									  GPIO_PUPDR_PUPDR5 |
									  GPIO_PUPDR_PUPDR6 |
									  GPIO_PUPDR_PUPDR7 );
	
	// PB0, PB1
	GPIOB->MODER |= (GPIO_MODER_MODER0 | 
									 GPIO_MODER_MODER1 );
	
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR0 | 
									  GPIO_PUPDR_PUPDR1 );
										
	// ir led gpio
	// B2, B10, A2, C13, C14, C15
	
	// gpio push-pull low speed
	GPIOB->MODER |= (GPIO_MODER_MODER2_0 | GPIO_MODER_MODER10_0);
	GPIOA->MODER |=  GPIO_MODER_MODER2_0;
	GPIOC->MODER |= (GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0);

	
}

// B2, B10, A2, C13, C14, C15


//----------------------------------------------------------

void ADC_InitS(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	
	GPIO_InitS();
	// CH4 - A4, CH5-A5, CH6 - A6, CH7-PA7
	// CH8 - B0, CH9-B1
	// F[ADC] = 25MHz
	
	ADC->CCR |=	 ADC_CCR_ADCPRE_0;
	ADC->CCR &=	~ADC_CCR_ADCPRE_1;
	
	//    [Order] 
	// B0 - [0] CH8 left forward
	// A7 - [2] CH7 left front
	// A6 - [4] CH6 left side
	
	// A5 - [1] CH5 right side
	// A4 - [5] CH4 right diagonal
	// A3 - [3] CH3 right front
	
	// B1 - [6] CH9 for battery
	
	ADC1->SQR1 &= ~ADC_SQR1_L;
	
	ADC1->SQR3 |=  0x08 ;
	
	// adc coversaton takes 12-13 cycles
	// total conversation - (480 + 13) / (25 * 10^6) = 0.13804ms
	// 1 cycle is 19.7
	ADC1->SMPR2 |= ADC_SMPR2_SMP8 ;
	
	ADC1->CR1 &= ~ADC_CR1_RES;   // 12 bit
	ADC1->CR2 &= ~ADC_CR2_ALIGN; // right alignment
	
	//ADC1->CR1 |= ADC_CR1_EOCIE;  // end of 
	
	
	ADC1->CR1 |= ADC_CR1_AWDCH_3; // ch 9	
	ADC1->CR1 |= ADC_CR1_AWDIE; //analog watchdoe in en
	ADC1->CR1 |= ADC_CR1_AWDEN; // enable on regular channels
	ADC1->CR1 |= ADC_CR1_AWDSGL; // single channel
	
	ADC1->HTR = 0xC00;
	ADC1->LTR = 0xA;
	
	NVIC_EnableIRQ(ADC_IRQn);
	
	//DMA_Init();
	//ADC1->CR2 |= ADC_CR2_DMA;
	// start
//	ADC1->CR2 |= ADC_CR2_CONT; // contimuous mode
	ADC1->CR2 |= ADC_CR2_ADON; // enable conversation
	
	delay_us(50000);
	
	//ADC1->CR2 |= ADC_CR2_SWSTART;
	// stm32f411 has no nned in calibration
	
	//DMA_Init();
}


void GPIO_InitS(void)
{
	// PA4, PA5, PA6, PA7, PB0, PB1
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
	
	// PA4, PA5, PA6, PA7,
	// analog in
	
	GPIOA->MODER |= (GPIO_MODER_MODER4 | 
									 GPIO_MODER_MODER5 |
									 GPIO_MODER_MODER6 |
									 GPIO_MODER_MODER7 );
	
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4 | 
									  GPIO_PUPDR_PUPDR5 |
									  GPIO_PUPDR_PUPDR6 |
									  GPIO_PUPDR_PUPDR7 );
	
	// PB0, PB1
	GPIOB->MODER |= (GPIO_MODER_MODER0 | 
									 GPIO_MODER_MODER1 );
	
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR0 | 
									  GPIO_PUPDR_PUPDR1 );
		
}
