#ifndef LCD_H
#define LCD_H

#include "stm32f4xx.h"                  // Device header
#include "i2c.h"
#include "delay.h"

#define OLED_Address  			0x3C

#define OLED_WIDTH      	128
#define OLED_HEIGHT     	64
#define OLED_MAX_LINE   	7

void OLED_Init(void);

void OLED_WriteCmd(uint8_t command);

void OLED_WriteData(uint8_t data);

void OLED_Clear(void);

void OLED_DrawHorizontalLine(void);

void OLED_Puts(char *str);

void OLED_SetCursor(uint8_t line, uint8_t column);

//void I2C_Send_Data(uint8_t data);
//uint8_t I2C_Read_Data(void);

//void LCD_Init(void);
//void LCD_Write_Byte(char data);
//void LCD_Control_Write(char data);
//void LCD_Data_Write(char data);
//void LCD_Send_String(char *str);
//void LCD_Clear(void);
//void LCD_SetCursor(uint8_t row, uint8_t col);

#endif	// LCD_H
