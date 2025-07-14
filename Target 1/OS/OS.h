#ifndef OS_H
#define OS_H

#include "stm32f4xx.h"                  // Device header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "dht.h"
#include "delay.h"
#include "BlinkLed.h"
#include "MQ135.h"
#include "i2c.h"
#include "DS3231.h"
#include "LCD.h"
#include "DataConvert.h"
#include "menu.h"
#include "com_UI.h"

typedef enum
{
	Message_DHT,
	Message_MQ135,
	Message_RTC
} MessageType_t;

typedef struct 
{
	MessageType_t type;
	
	union
	{
		struct
		{
			float temp;
			float humi;
		} dht;
		
		struct
		{
			float ppm;
		} mq135;
		
		struct
		{
			uint8_t hour, min, sec;
			uint8_t day;
			uint8_t date, mon, year;
		} rtc;
		
	} data;
	
} SensorMessage_t;

#define QUEUE_SIZE 20
extern osMessageQueueId_t sensorQueue;
extern osMutexId_t dataMutex;

void Task_ReadTemp(void *arg);

void Task_MQ135(void *arg);

void Task_RTC(void *arg);

void Task_OLED(void *arg);

void Task_Menu(void *arg);

void Task_SendData(void *arg);

#endif	// OS_H
