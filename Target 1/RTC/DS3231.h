#ifndef DS3231_H
#define DS3231_H

#include "stm32f4xx.h"                  // Device header

typedef struct 
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} RTC_Time;

extern RTC_Time currentTime;

extern char timeStr[32];

void DS3231_Write(uint8_t reg_addr, uint8_t data);

uint8_t DS3231_Read(uint8_t reg_addr);

uint8_t BCD_To_Dec(uint8_t val);

uint8_t Dec_To_BCD(uint8_t val);

void DS3231_GetTime(RTC_Time *time);

void DS3231_SetTime(RTC_Time *time);

//void FormatTimeString(void);

#endif	// DS3231_H
