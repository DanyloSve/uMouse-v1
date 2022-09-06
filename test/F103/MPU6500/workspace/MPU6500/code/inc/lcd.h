#ifndef LCD_H_
#define LCD_H_

//#include "stm32f10x.h"
#include "timer_delays.h"
#include "i2c1.h"

#define LCD_I2C_ADDR 0x27

// RS - P0 [0 - for instruction; 1 - for data]
#define LCD_RS 0x01

// RW - P1 [0 - for write; 1 - for read]
#define LCD_RW 0x02

// EN or CS - P2 [front shows that data are going to be read or written]
#define LCD_EN 0x04
// [0 - backlight off; 1 - backlight on]
#define LCD_BACKLIGHT 0x08

#define LCD_MODE_DATA 1
#define LCD_MODE_COMMAND 0

void LCD_Init(void);
void LCD_Clear(void);
void LCD_String(char *str); //
void LCD_SetPos(uint8_t col, uint8_t row);

static void sendByte(uint8_t data, uint8_t mode);//
static void sendHalfByte(uint8_t halfByte); //

static uint8_t control4LSBits = 0; // bits that control backlight, R/W etc
												           // only 4 LSB are used

#endif /*LCD_H_*/
