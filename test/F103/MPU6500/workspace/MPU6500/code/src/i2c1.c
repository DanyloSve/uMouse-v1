#include "i2c1.h"

void I2C1_Init(void)
{
	// GPIO init----------------------------------------------------->
	// PB6 SCL;  PB7 SDA
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // clock for I2C
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // clock for GPIO
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	
	
	// PB6 SCL  
	GPIOB->CRL |= GPIO_CRL_MODE6; // alternative function open drain
	GPIOB->CRL |= GPIO_CRL_CNF6;  // max speed
	
	// PB7 SDA 
	GPIOB->CRL |= GPIO_CRL_MODE7; // alternative function open drain
	GPIOB->CRL |= GPIO_CRL_CNF7;  // max speed
	
	// CR------------------------------------------------------------>
	I2C1->CR1 &= ~I2C_CR1_PE;       // disable periph
	
	I2C1->OAR2 &= ~I2C_OAR2_ENDUAL; // disable dual address
	I2C1->CR1 &= ~I2C_CR1_ENGC;     // no general call
	I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;// en stretch
	I2C1->CR1 &= ~I2C_CR1_SMBTYPE;  // i2c
	I2C1->CR1 |= I2C_CR1_ACK;		    // allow ack
	
	// Frequency---------------------------------------------------->
	I2C1->CCR |= I2C_CCR_FS; // Fast mode
	I2C1->CCR &= ~I2C_CCR_DUTY; // duty = 0
	
	I2C1->CR2 |= (36 & I2C_CR2_FREQ); // APB1 = 36MHz; 
	I2C1->TRISE |= ( (11) & I2C_TRISE_TRISE); // fast mode, 36MHz;
	
	
	I2C1->CCR |= (I2C_CCR_CCR & 0x1E); // 400kHz
	
	// Start--------------------------------------------------------->
	I2C1->CR1 |= I2C_CR1_PE; // enable periph

}


void I2C1_SendByte(uint16_t addr, uint8_t data)
{
	#define I2C_REQUEST_WRITE   0x00

	
	// ref man p760
	// S 
	I2C1->CR1 &= ~I2C_CR1_POS;
	I2C1->CR1 |= I2C_CR1_ACK;
	
	I2C1->CR1 |= I2C_CR1_START; // generate START flag
	
	// EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	while ( !(I2C1->SR1 & I2C_SR1_SB) ) // while START is not generated... 
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
  //I2C_StopCondition();
  I2C1->CR1 |= I2C_CR1_STOP; // stop condition
}

uint8_t I2C1_ReceiveByte(uint16_t addr)
{
	#define I2C_REQUEST_READ   0x01
	
	uint8_t readData;
	
	// S
	I2C1->CR1 |= I2C_CR1_ACK; // generate ACK after one byte is received
	I2C1->CR1 |= I2C_CR1_START; // generate START
	
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
	
	I2C1->CR1 &= ~I2C_CR1_ACK; // no ack bz only one byte will be read	
		
	// EV6
	while ( !(I2C1->SR1 & I2C_SR1_RXNE) )
	{
		// wait until data is being transmitting
	}
	
	// reading the data
	readData = I2C1->DR;
	
	I2C1->CR1 |= I2C_CR1_STOP; // stop condition
	
	return readData;
}


int I2C1_Scan(void)
{
	// ref man p760
	// S 
	int addr;

	
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



//-------------------------------------------------------------------------

int I2C1_scan(void)
{
	#define I2C_REQUEST_READ   0x01
	
	int addr;
	
	// S
	I2C1->CR1 &= ~I2C_CR1_ACK; // no data is needed
	
	for (addr = 0x00; addr < 0x80; ++addr)
	{
		I2C1->CR1 |= I2C_CR1_START; // generate START
		
		// EV5
		while ( !(I2C1->SR1 & I2C_SR1_SB) )
		{
			// wait until START condition is being generated ...
		}
		
		(void)I2C1->SR1; 
		
		// Address
		I2C1->DR = ( (addr << 1) | I2C_REQUEST_READ);
		
		delay_us(100);
		// EV6
		if ( (I2C1->SR1 & I2C_SR1_ADDR) )
		{
				
			(void)I2C1->SR1;
			(void)I2C1->SR2;
			
			//I2C1->CR1 &= ~I2C_CR1_ACK; // no ack bz only one byte will be read	
				
			// EV6
			while ( !(I2C1->SR1 & I2C_SR1_RXNE) )
			{
				// wait until data is being transmitting
			}
			
			// reading the data
			(void)I2C1->DR;
			
			break;
		}
	}
	I2C1->CR1 |= I2C_CR1_STOP; // stop condition

	return addr;
}



void I2C1_Write(uint16_t addr, uint8_t *buffer, uint32_t bufferSize)
{
	#define I2C_REQUEST_WRITE   0x00
	uint8_t i;
	
	// ref man p760
	// S 
	I2C1->CR1 &= ~I2C_CR1_POS;
	I2C1->CR1 |= I2C_CR1_ACK;
	
	I2C1->CR1 |= I2C_CR1_START; // generate START flag
	
	// EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	while ( !(I2C1->SR1 & I2C_SR1_SB) ) // while START is not generated... 
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
	
	for (i = 0; i < bufferSize; ++i)
	{
		I2C1->DR = buffer[i];
		
		while (!(I2C1->SR1 & I2C_SR1_TXE)) 
		{
			// wait until data are being transmitted ...
		}
	}

  //I2C_StopCondition();
  I2C1->CR1 |= I2C_CR1_STOP; // stop condition
}

void I2C1_Read(uint16_t addr, uint8_t *buffer, uint32_t bufferSize)
{
	#define I2C_REQUEST_READ   0x01

	uint16_t i;
	
	// S
	I2C1->CR1 |= I2C_CR1_ACK; // generate ACK after one byte is received
	I2C1->CR1 |= I2C_CR1_START; // generate START
	
	// EV5
	while ( !(I2C1->SR1 & I2C_SR1_SB) )
	{
		// wait untill START condition is being generated ...
	}
	
	(void)I2C1->SR1; 
	
	// Address
	I2C1->DR |= ( (addr << 1) | I2C_REQUEST_READ);
	
	// EV6
	while ( !(I2C1->SR1 & I2C_SR1_ADDR) )
	{
		// wait until address is being transmitted ...
	}
	(void)I2C1->SR1;
	(void)I2C1->SR2;
	
	for (i = 0; i < bufferSize; ++i)
	{		
		if ( i == bufferSize - 1)
		{	
			// EV 7_1
			I2C1->CR1 &= ~I2C_CR1_ACK; // no ack	
			I2C1->CR1 |= I2C_CR1_STOP; // stop condition
		}
		
		// EV7
		while ( !(I2C1->SR1 & I2C_SR1_RXNE) )
		{
				// wait until data is being transmitting
		}
		buffer[i] = I2C1->DR;
	}
	
	//I2C1->CR1 |= I2C_CR1_STOP; // stop condition
}
