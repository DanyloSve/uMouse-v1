#include "lcd.h"

void LCD_Init(void)
{
	I2C1_Init();
  delay_ms(50);
	
	I2C1_SendByte(LCD_I2C_ADDR, 0);
	I2C1_SendByte(LCD_I2C_ADDR, 0);
	delay_ms(100);
  
	sendHalfByte(0x03);
	delay_us(4500);
  
	sendHalfByte(0x03);
	delay_us(4500);
  
	sendHalfByte(0x03);
	delay_us(200);
  
	sendHalfByte(0x02);
  
	sendByte(0x28, LCD_MODE_COMMAND);//????? 4 ???, 2 ????? (??? ?????? ???????? ??????? ??? 4 ?????, ????? 5?8
  sendByte(0x08, LCD_MODE_COMMAND);//??????? ???? ?????????
  
	delay_ms(1);
  sendByte(0x01, LCD_MODE_COMMAND);// ?????? ?????
  delay_ms(2);
  
	sendByte(0x06, LCD_MODE_COMMAND);// ????? ?????
  delay_ms(1);
  
	sendByte(0x0C, LCD_MODE_COMMAND);//??????? ???????? (D=1), ??????? ??????? ?? ?????
  sendByte(0x02, LCD_MODE_COMMAND);//?????? ?? ?????
  delay_ms(2);
  
	control4LSBits |= LCD_BACKLIGHT;
	I2C1_SendByte(LCD_I2C_ADDR, control4LSBits);
}

void LCD_Clear(void)
{
	#define CLEAR_COMMAND 0x01
	sendByte(CLEAR_COMMAND, LCD_MODE_COMMAND);
	delay_us(1550);
}

void LCD_SendChar(char data)
{
	sendByte(data,1);
}


void LCD_String(char *str)
{
	while (*str)
	{
		sendByte(*(str++), LCD_MODE_DATA);

	}
}

void sendHalfByte(uint8_t halfByte)
{
	halfByte <<= 4;	

	// making impulse of LCD_EN to show that data is being passed  
	I2C1_SendByte( LCD_I2C_ADDR, (control4LSBits | LCD_EN) | halfByte);
	delay_us(1);
	I2C1_SendByte(LCD_I2C_ADDR, (control4LSBits) | halfByte);
}

void sendByte(uint8_t data, uint8_t mode)
{	
	uint8_t majorHalfByte = data>>4;
	
	if(mode == LCD_MODE_COMMAND)
	{
		control4LSBits &= ~LCD_RS;
	}
	else
	{
		control4LSBits |= LCD_RS;
	}

	sendHalfByte(majorHalfByte);
	sendHalfByte(data);
}


// 1602
void LCD_SetPos(uint8_t col, uint8_t row)
{
	// For address
	#define DB7 0x80
	#define SECOND_ROW 0x40
	
	switch(row)
	{
		case 0:
		{
			sendByte(col|DB7, LCD_MODE_COMMAND);
		}
		break;
		
		case 1:
		{
			sendByte((col + SECOND_ROW) | DB7, LCD_MODE_COMMAND);
		}
		break;
	}
}

