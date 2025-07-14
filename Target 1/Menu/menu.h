#ifndef MENU_H
#define MENU_H

#include "stm32f4xx.h"                  // Device header
#include "dht.h"
#include "delay.h"
#include "BlinkLed.h"
#include "MQ135.h"
#include "i2c.h"
#include "DS3231.h"
#include "LCD.h"
#include "DataConvert.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

typedef enum
{
	STATE_MENU,				// main menu
	STATE_VIEW_TEMP,	// read temperature and humidity
	STATE_VIEW_CO2,		// read CO2 concentration
	STATE_VIEW_TIME,	// read real-time
	STATE_ALL
} State_t;

extern State_t state;
	
extern uint8_t menu_index;
extern uint32_t btn_press_time;
extern uint8_t btn_pressed;
extern uint8_t selecting;
	
void BUTTON_Config(void);

uint8_t BUTTON_Read(void);

void OLED_DrawMenu(uint8_t selected);

void Menu_Handler(void);

#endif	// MENU_H
