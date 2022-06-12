#include "adc.h"
#include "timer_delays.h"

#include "rgb_led.h"
#include "timer_delays.h"

static void GPIO_Init (void);

//volatile uint16_t ADC_data;

volatile uint16_t ADC_data[8]; //  4 variables for ir-transistor without ir led; 4 variables for ir-transistor with ir led  
volatile uint16_t ADC_bat;

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

	// B0 - [2] CH8 left side
	// A7 - [0] CH7 left front

	// A3 - [3] CH3 right front
	// A2 - [1] CH2 right side
	

	// B1 - [6] CH9 for battery

	ADC1->SQR1 &= ~ADC_SQR1_L;

	// adc coversaton takes 12-13 cycles
	// total conversation - (28 + 13) / (25 * 10^6) = 0.13804ms
	ADC1->SMPR2 |= ADC_SMPR2_SMP8_1 |		// 28 cycles
								 ADC_SMPR2_SMP7_1 |		// 28 cycles
								 ADC_SMPR2_SMP3_1 |		// 28 cycles
								 ADC_SMPR2_SMP2_1 |		// 28 cycles
								 ADC_SMPR2_SMP9 ; // 480 cycles

	ADC1->CR1 &= ~ADC_CR1_RES;   // 12 bit
	ADC1->CR2 &= ~ADC_CR2_ALIGN; // right alignment

	ADC1->CR1 |= ADC_CR1_AWDCH_0 | ADC_CR1_AWDCH_3; // ch 9
	ADC1->CR1 |= ADC_CR1_AWDIE; //analog watchdoe in en
	ADC1->CR1 |= ADC_CR1_AWDEN; // enable on regular channels
	ADC1->CR1 |= ADC_CR1_AWDSGL; // single channel

	ADC1->HTR = 3061; // 7.4V - maximum operating voltage
	ADC1->LTR = 2482; // 6.0V - minimum operating voltage

	NVIC_EnableIRQ(ADC_IRQn);

	// start
	ADC1->CR2 |= ADC_CR2_ADON; // enable conversation
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
	// B0, A7, A3, A2 - ACD IN
	
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

	// PA2, PA3, PA7
	// analog in

	GPIOA->MODER |= (GPIO_MODER_MODER2 |
									 GPIO_MODER_MODER3 |
									 GPIO_MODER_MODER7);

	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2 |
									  GPIO_PUPDR_PUPDR3 |
									  GPIO_PUPDR_PUPDR7 );

	// PB0, PB1
	GPIOB->MODER |= (GPIO_MODER_MODER0 |
									 GPIO_MODER_MODER1 );

	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR0 |
									  GPIO_PUPDR_PUPDR1 );

	// ir led gpio
	//  B2, B10, C15, C14 - GPIO OUT

	// gpio push-pull low speed
	GPIOB->MODER |= (GPIO_MODER_MODER2_0  | GPIO_MODER_MODER10_0);
	GPIOC->MODER |= (GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0);

	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR2  | GPIO_OSPEEDER_OSPEEDR10);
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15);
	
}

void ADC_ReadSensors(void)
{ 
	
	
	//GPIOC->ODR |=  GPIO_ODR_ODR_15;
	
	//  6 variables for ir-transistor without ir led
	for (uint8_t i = 0; i < sizeof(ADC_data) / ( sizeof(uint16_t) * 2); ++i) // 4 variables
	{
		switch (i)
		{
			case 0:
			{
				ADC1->SQR3 =  0x07;
			}
			break;

			case 1:
			{
				ADC1->SQR3 =  0x02;
			}
			break;

			case 2:
			{
				ADC1->SQR3 =  0x08;
			}
			break;
			
			case 3:
			{
				ADC1->SQR3 =  0x03;
			}
			break;

		}

		ADC1->CR2 |= ADC_CR2_SWSTART; // start conversation

		while( !(ADC1->SR & ADC_SR_EOC) )
		{

		}

		ADC_data[i] = ADC1->DR;
	}

	
	// 6 variables for ir-transistor with ir led
	for (uint8_t i = sizeof(ADC_data) / ( sizeof(uint16_t) * 2); i < sizeof(ADC_data) / ( sizeof(uint16_t)); ++i) // 4 variables
	{
		switch (i)
		{
			case 4: // LF
			{
				GPIOB->ODR |= GPIO_ODR_ODR_2;
				
				delay_us(60); // wait 60 us to settle down voltage
				
				ADC1->SQR3 =  0x07;
			}
			break;

			case 5: //RS
			{
				GPIOB->ODR &= ~GPIO_ODR_ODR_2;
				delay_us(80); // wait untill reflection is gone
				
				GPIOC->ODR |= GPIO_ODR_ODR_14;		
				delay_us(60); // wait 60 us to settle down voltage
				
				ADC1->SQR3 =  0x02;
			}
			break;

			case 6: //LS
			{
				GPIOC->ODR &= ~GPIO_ODR_ODR_14;
				delay_us(80); // wait untill reflection is gone
				
				GPIOB->ODR |=  GPIO_ODR_ODR_10;
				delay_us(60); // wait 60 us to settle down voltage
				
				ADC1->SQR3 =  0x08;
			}
			break;

			case 7: //RF
			{
				GPIOB->ODR &= ~GPIO_ODR_ODR_10;
				delay_us(80); // wait untill reflection is gone
				
				GPIOC->ODR |=  GPIO_ODR_ODR_15;
				delay_us(60); // wait 60 us to settle down voltage
				ADC1->SQR3 =  0x03;
			}
			break;


		}
		

		ADC1->CR2 |= ADC_CR2_SWSTART; // start conversation

		while( !(ADC1->SR & ADC_SR_EOC) )
		{

		}

		ADC_data[i] = ADC1->DR;
	}

	GPIOC->ODR &=  ~GPIO_ODR_ODR_15;
	

}

void ADC_ReadBattery(void)
{
		// read data for batery voltage

	ADC1->SQR3 =  0x09;
	ADC1->CR2 |= ADC_CR2_SWSTART; // start conversation
	
	while( !(ADC1->SR & ADC_SR_EOC) )
	{

	}
	ADC_bat =  ADC1->DR; // read battery
}

