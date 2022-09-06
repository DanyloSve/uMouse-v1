/*
	I2C1
	
	PB9 -> SDA
	PB8 -> SCL
	
	I2C1 speed - 400kHz
	
	DMA - medium priority
*/

#include "i2c1.h"

static void     I2C1_SendByte    (const uint8_t addr, const uint8_t data);
static uint8_t  I2C1_ReceiveByte (const uint8_t addr);

static void     DMA_Init         (void);
static void     GPIO_Init        (void);

#define         DMA_BUFFER_RX_DEFAULT_SIZE 0xFF
static volatile uint8_t dmaBufferReceive[DMA_BUFFER_RX_DEFAULT_SIZE];


void I2C1_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;							// I2C RCC
	
	I2C1->CR1 |=  I2C_CR1_SWRST; 										// software reset
	delay_us(50000);
	delay_us(50000);
	
	I2C1->CR1 &= ~I2C_CR1_SWRST;
	delay_us(50000);
	delay_us(50000);
	
	// GPIO Init 
	//------------------------------------------------------------>
	GPIO_Init();
	
	// CR
	//------------------------------------------------------------>
	I2C1->CR1   &= ~I2C_CR1_PE;      	  							// disable periph
	
	I2C1->CR1   |=  I2C_CR1_ACK; 	 										// ACK en
	I2C1->CR1   &= ~I2C_CR1_SMBUS; 										// I2C
	
	I2C1->CR1   &= ~I2C_CR1_ENGC;     								// no general call
	I2C1->CR1   &= ~I2C_CR1_NOSTRETCH;								// disable stretch
	
	
	I2C1->CR2   |= (I2C_CR2_FREQ & 50); 							// must be dividede by power by 2
	
	I2C1->OAR2  &= ~I2C_OAR2_ENDUAL; 								// disable dual address
	
	// Frequency
	//------------------------------------------------------------>
	
	I2C1->CCR   |=  I2C_CCR_FS; 												// Fast mode 400kHz
	I2C1->CCR   |=  I2C_CCR_DUTY; 											// duty = 1
	
	
	I2C1->CCR   |= (I2C_CCR_CCR & 5); 								// 50MHz / ((16 + 9) * 5) = 400kHz

	
	I2C1->TRISE |= ( (16) & I2C_TRISE_TRISE);				//	Tr[max] for MPU6050 400KHz = 300ns
																									//  I2C_CR2_FREQ = 50MHz
																									//	Trise = 300ns / (1 / I2C_CR2_FREQ) + 1 = 
																									//   = 300 ns / (1 / 50MHz) + 1 = 15 + 1 = 16
	
	//I2C1->TRISE |= ( (50) & I2C_TRISE_TRISE);
	 			
	// Start--------------------------------------------------------->
	I2C1->CR1 |= I2C_CR1_PE; // enable periph		
	
	DMA_Init();
}

void  I2C1_WriteReg(const uint8_t addr, const uint8_t reg, const uint8_t data)
{ 
	I2C1_SendByte(addr, reg);
		
	I2C1->DR = data;
	while (!(I2C1->SR1 & I2C_SR1_TXE)) 
	{
		// wait until data are being transmitted ...
	}
  //I2C_StopCondition();
  I2C1->CR1 |= I2C_CR1_STOP; 																		// stop condition
}

uint8_t  I2C1_ReadReg(const uint8_t addr, const uint8_t reg)
{
	I2C1_SendByte(addr, reg);
	return I2C1_ReceiveByte(addr);
}

void GPIO_Init(void)
{
	// RCC 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	// SDA PB9 AF4 
	//---------------------------------------------------------------->
	
	
	GPIOB->MODER   |=  GPIO_MODER_MODER9_1;						// Configure the I2C PINs for ALternate Functions
	GPIOB->MODER   &= ~GPIO_MODER_MODER9_0;
	
	GPIOB->OTYPER  |=  GPIO_OTYPER_OT_9; 							// Output open-drain
	
	GPIOB->OSPEEDR |=  GPIO_OSPEEDER_OSPEEDR9; 		  // max speed
	
	GPIOB->PUPDR   |=  GPIO_PUPDR_PUPDR9_0;						// pull - up
	GPIOB->PUPDR   &= ~GPIO_PUPDR_PUPDR9_1;
	
	// SCL -> PB8 AF4
	//---------------------------------------------------------------->
	GPIOB->MODER   |=  GPIO_MODER_MODER8_1;  					// Configure the I2C PINs for ALternate Functions
	GPIOB->MODER   &= ~GPIO_MODER_MODER8_0;
	
	GPIOB->OTYPER  |=  GPIO_OTYPER_OT_8;							// Output open-drain
	
	GPIOB->OSPEEDR |=  GPIO_OSPEEDER_OSPEEDR8;				// max speed
	
	
	GPIOB->PUPDR   |=  GPIO_PUPDR_PUPDR8_0;						// pull - up
	GPIOB->PUPDR   &= ~GPIO_PUPDR_PUPDR8_1;
		
	GPIOB->AFR[1] |= (4<<0) | (4<<4);  							// AFIO
}

void     I2C1_SendByte(const uint8_t addr, const uint8_t data)
{
	#define I2C_REQUEST_WRITE   0x00

	// ref man p760
	// S 
	I2C1->CR1      &= ~I2C_CR1_POS;
	I2C1->CR1      |=  I2C_CR1_ACK;
	
	I2C1->CR1      |=  I2C_CR1_START; 															   // generate START flag
	
	// EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	while ( !(I2C1->SR1 & I2C_SR1_SB) ) 											   // while START is not generated... 
	{
	 // wait until START condition is not generated ...
	}
	
	(void)I2C1->SR1;
	
	I2C1->DR = (addr << 1) | I2C_REQUEST_WRITE;
	
	while ( !(I2C1->SR1 & I2C_SR1_ADDR) ) 
	{
		// wait until address is being transmitted ...
	}
// EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
	(void) I2C1->SR1;
  (void) I2C1->SR2;
	
	I2C1->DR = data;
	while (!(I2C1->SR1 & I2C_SR1_TXE)) 
	{
		// wait until data are being transmitted ...
	}
	
	// Here should be No STOP!
	// STOP is in another functions
}

uint8_t  I2C1_ReceiveByte(const uint8_t addr)
{
	#define I2C_REQUEST_READ   0x01
	
	// S
	
	I2C1->CR1 |= I2C_CR1_START; 													// generate START
	I2C1->CR1 |= I2C_CR1_ACK;														  // generate ACK after one byte is received
	// EV5
	while ( !(I2C1->SR1 & I2C_SR1_SB) )
	{
		// wait until START condition is being generated ...
	}
	
	(void)I2C1->SR1; 
	
	// Address
	I2C1->DR = ( (addr << 1) | I2C_REQUEST_READ);

	// EV6
	while ( !(I2C1->SR1 & I2C_SR1_ADDR) )
	{
		// wait until address is being transmitted ...
	}
	(void)I2C1->SR1;
	(void)I2C1->SR2;
	
	
	// EV6
	I2C1->CR1 &= ~I2C_CR1_ACK;
	I2C1->CR1 |=  I2C_CR1_STOP;
	
	while ( !(I2C1->SR1 & I2C_SR1_RXNE) )
	{
		// wait until data is being transmitting
	}
	
	return I2C1->DR;
}


void I2C1_ReadBurst(const uint8_t addr, const uint8_t reg, uint8_t *buffer, const uint8_t bufferSize)
{
	I2C1_SendByte(addr, reg);
	
	#define I2C_REQUEST_READ   0x01
	
	// S
	DMA1_Stream0->M0AR = (uint32_t)buffer;
	DMA1_Stream0->NDTR = (uint16_t)(bufferSize);
	
	I2C1->CR1 |= I2C_CR1_START; 													// generate START
	I2C1->CR1 |= I2C_CR1_ACK;														  // generate ACK after one byte is received
	// EV5
	while ( !(I2C1->SR1 & I2C_SR1_SB) )
	{
		// wait until START condition is being generated ...
	}
	
	(void)I2C1->SR1; 
	
	// Address
	I2C1->DR = ( (addr << 1) | I2C_REQUEST_READ);

	// EV6
	while ( !(I2C1->SR1 & I2C_SR1_ADDR) )
	{
		// wait until address is being transmitted ...
	}
	(void)I2C1->SR1;
	(void)I2C1->SR2;
	

	DMA1_Stream0->CR |= DMA_SxCR_EN; 

}

void I2C1_WriteBurst(const uint8_t addr, const uint8_t reg, uint8_t *buffer, const uint8_t bufferSize)
{
	I2C1_SendByte(addr, reg);
	
	for (uint32_t i = 0; i < bufferSize; ++i)
	{		
		I2C1->DR = buffer[i];
		// EV7
		while ( !(I2C1->SR1 & I2C_SR1_TXE) )
		{
				// wait until data is being transmitting
		}
	}
	
	// EV 7_1
	I2C1->CR1 &= ~I2C_CR1_ACK; 														// no ack	
	I2C1->CR1 |=  I2C_CR1_STOP; 														// stop condition
}

/*
	I2C1_RX DMA
	DMA1 Channel 1 Stream 0
	
*/


void DMA_Init(void)
{
	RCC->AHB1ENR       |=  RCC_AHB1ENR_DMA1EN;
	DMA1_Stream0->PAR   = (uint32_t) &(I2C1->DR);
	DMA1_Stream0->M0AR  = (uint32_t)(dmaBufferReceive);
	
	DMA1_Stream0->NDTR  = (uint16_t)(sizeof(uint8_t)); // some default value
	DMA1_Stream0->CR   |=  DMA_SxCR_CHSEL_0;// channel 1
	
	DMA1_Stream0->CR   |=  DMA_SxCR_PL; // very high priority
	
	
	DMA1_Stream0->CR   &= ~DMA_SxCR_MSIZE; // 8-bit mem
	DMA1_Stream0->CR   &= ~DMA_SxCR_PSIZE; // 8-bit periph
	
	DMA1_Stream0->CR   |=  DMA_SxCR_MINC; // mem increment
	DMA1_Stream0->CR   &= ~DMA_SxCR_PINC; // periph increment

	DMA1_Stream0->CR   &= ~DMA_SxCR_CIRC; // no circ
	DMA1_Stream0->CR   &= ~DMA_SxCR_DIR; // periph to mem
	
	DMA1_Stream0->CR   |=  DMA_SxCR_TCIE;  // // transfer complete int en
	DMA1->LIFCR        |=  DMA_LIFCR_CTCIF0; 
	

	I2C1->CR2          |= I2C_CR2_LAST;
	I2C1->CR2          |= I2C_CR2_DMAEN;
	
}


// Debug mode
// -------------------------------------------------------------------------------------------------->
#if defined(I2C1_DEBUG_MODE)

uint8_t I2C1_Scan(void)
{
	// ref man p760
	// S 
	uint8_t addr;

	
	#define I2C_REQUEST_WRITE   0x00

	
	// ref man p760
	// S 
	I2C1->CR1 &= ~I2C_CR1_POS;
	I2C1->CR1 |= I2C_CR1_ACK;
	
	for (addr = 0x0; addr < 0x80; ++addr)
	{
		I2C1->CR1 |= I2C_CR1_START; // generate START flag
		
		// EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
		while ( !(I2C1->SR1 & I2C_SR1_SB) ) // while START is not generated... 
		{
		 // wait until START condition is not generated ...
		}
		
		(void)I2C1->SR1;
		
		I2C1->DR = (addr << 1) | I2C_REQUEST_WRITE;
		
		delay_us(100);
		
		if( I2C1->SR1 & I2C_SR1_ADDR ) 
		{
			(void) I2C1->SR1;
			(void) I2C1->SR2;
			
			I2C1->CR1 &= ~I2C_CR1_ACK; 
			I2C1->CR1 |= I2C_CR1_STOP;
			
			break;
			
		}
	// EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
		
	}
	
	
	return addr;
}

uint8_t I2C1_Search(uint8_t addr)
{
	#define I2C_REQUEST_WRITE   0x00
	uint16_t isAddressFound = 0;
	
	// ref man p760
	// S 
	I2C1->CR1 &= ~I2C_CR1_POS;
	I2C1->CR1 |= I2C_CR1_ACK;
	
	I2C1->CR1 |= I2C_CR1_START; 															   // generate START flag
	
	// EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	while ( !(I2C1->SR1 & I2C_SR1_SB) ) 											   // while START is not generated... 
	{
	 // wait until START condition is not generated ...
	}
	
	(void)I2C1->SR1;
	
	I2C1->DR = (addr << 1) | I2C_REQUEST_WRITE;
	
	delay_us(100);
	
	isAddressFound = I2C1->SR1;
	(void)I2C1->SR2;
	
	I2C1->CR1 |= I2C_CR1_STOP;
	I2C1->CR1 &= ~I2C_CR1_ACK;
	
	return ((isAddressFound & I2C_SR1_ADDR) == I2C_SR1_ADDR);
}

void I2C1_ScanAddrs(uint8_t* addrs, const uint8_t addrCntr)
{
	// ref man p760
	// S 
	uint8_t currAddr;
	uint8_t cntr = 0;
	uint16_t isAddressFound = 0;
	
	#define I2C_REQUEST_WRITE   0x00

	
	// ref man p760
	// S 
	I2C1->CR1 &= ~I2C_CR1_POS;
	I2C1->CR1 |= I2C_CR1_ACK;
	
	for (currAddr = 0x00; currAddr< 0x80; ++currAddr)
	{
		I2C1->CR1 |= I2C_CR1_START; // generate START flag
		
		// EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
		while ( !(I2C1->SR1 & I2C_SR1_SB) ) // while START is not generated... 
		{
		 // wait until START condition is not generated ...
		}
		
		(void)I2C1->SR1;
		
		I2C1->DR = (currAddr << 1) | I2C_REQUEST_WRITE;
		
		delay_us(100);
		
		isAddressFound = I2C1->SR1;
		(void)I2C1->SR2;
		
		I2C1->CR1 |= I2C_CR1_STOP;
		
		if( isAddressFound & I2C_SR1_ADDR) 
		{
			
			
			addrs[cntr++] = currAddr;
			
			if(cntr == addrCntr)
			{
				break;
			}
			
		}
	// EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
		
	}
	I2C1->CR1 &= ~I2C_CR1_ACK; 
}
#endif /*(I2C1_DEBUG_MODE)*/
