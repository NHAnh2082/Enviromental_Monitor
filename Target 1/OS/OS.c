#include "OS.h"

osMessageQueueId_t sensorQueue;
osMutexId_t dataMutex;

void Task_ReadTemp(void *arg)
{
	while (1)
	{
		SensorMessage_t msg;
		msg.type = Message_DHT;

		osKernelLock();
		DHT_GetData(DHT11, &msg.data.dht.temp, &msg.data.dht.humi);
//		if (DHT_GetData(DHT11, &msg.data.dht.temp, &msg.data.dht.humi))
//			GPIO_SetBits(LED_PORT, LED_PIN);
//		else 
//			GPIO_ResetBits(LED_PORT, LED_PIN);
		osKernelUnlock();
		
		osMessageQueuePut(sensorQueue, &msg, 0, 0);
		osDelay(2000);
	}
}

void Task_MQ135(void *arg)
{
	while (1)
	{	
		SensorMessage_t msg;
		msg.type = Message_MQ135;
		
		osKernelLock();
		adc_raw = MQ135_ReadADC();
		msg.data.mq135.ppm = MQ135_GetPPM(adc_raw);
		osKernelUnlock();
		
		osMessageQueuePut(sensorQueue, &msg, 0, 0);
		osDelay(1000);
	}
}

void Task_RTC(void *arg)
{
	while (1)
	{		
		SensorMessage_t msg;
		msg.type = Message_RTC;
		
		RTC_Time now;
		osKernelLock();
		DS3231_GetTime(&now);
		osKernelUnlock();
		
		msg.data.rtc.hour = now.hours;
		msg.data.rtc.min  = now.minutes;
		msg.data.rtc.sec  = now.seconds;
		msg.data.rtc.day  = now.day;
		msg.data.rtc.date = now.date;
		msg.data.rtc.mon  = now.month;
		msg.data.rtc.year = now.year;
		
		osMessageQueuePut(sensorQueue, &msg, 0, 0);
		osDelay(300);
	}
}

void Task_OLED(void *arg)
{	
	SensorMessage_t msg;
	
	OLED_Clear();
	
	while (1)
	{
//		if (osMessageQueueGet(sensorQueue, &msg, NULL, osWaitForever) == osOK) 
		if (osMessageQueueGet(sensorQueue, &msg, NULL, osWaitForever) == osOK)
		{
			osMutexAcquire(dataMutex, osWaitForever);
			switch (msg.type)
			{
				case Message_DHT:
//					if (osMutexAcquire(dataMutex, osWaitForever) == osOK)
//					{
						temp = msg.data.dht.temp;
						humi = msg.data.dht.humi;
//						osMutexRelease(dataMutex);
//					}
					break;

				case Message_MQ135:
//					if (osMutexAcquire(dataMutex, osWaitForever) == osOK)
//					{
						ppm = msg.data.mq135.ppm;
//						osMutexRelease(dataMutex);
//					}
					break;

				case Message_RTC:
//					if (osMutexAcquire(dataMutex, osWaitForever) == osOK)
//					{
						currentTime.hours   = msg.data.rtc.hour;
						currentTime.minutes = msg.data.rtc.min;
						currentTime.seconds = msg.data.rtc.sec;
						currentTime.day     = msg.data.rtc.day;
						currentTime.date    = msg.data.rtc.date;
						currentTime.month   = msg.data.rtc.mon;
						currentTime.year    = msg.data.rtc.year;
//						osMutexRelease(dataMutex);
//					}
					break;
			}
			osMutexRelease(dataMutex);
		}
		
		osDelay(500);
	}
}

void Task_Menu(void *arg)
{
	
//	Menu_Animation(0, 0);          
	OLED_DrawMenu(menu_index);
	
	while (1)
	{
		Menu_Handler();  // ki?m tra tr?ng thái nút và c?p nh?t FSM
		osDelay(100);     // delay tránh chi?m CPU
	}
}

void Task_SendData(void *arg)
{
	char buffer[BUFF_SIZE_TX];
	
	while (1)
	{
//		if (osMutexAcquire(dataMutex, osWaitForever) == osOK)
//		{
//			convert(buffer, "DATA_T=%f,H=%f,C=%f,DAY=%d,DATE=%d/%d/20%d,TIME=%d/%d/%d_END\r\n",
//							temp,
//							humi,
//							ppm,
//							currentTime.day,
//							currentTime.date,
//							currentTime.month,
//							currentTime.year,
//							currentTime.hours,
//							currentTime.minutes,
//							currentTime.seconds);
//			osMutexRelease(dataMutex);
//		}

//		UART4_SendString(buffer);
//		osDelay(1000);
		SensorMessage_t msg;
		if (osMessageQueueGet(sensorQueue, &msg, NULL, osWaitForever) == osOK)
		{
			osMutexAcquire(dataMutex, osWaitForever);
			convert(buffer, "DATA_T=%f,H=%f,C=%f,DAY=%d,DATE=%d/%d/20%d,TIME=%d/%d/%d_END\r\n",
							temp,
							humi,
							ppm,
							currentTime.day,
							currentTime.date,
							currentTime.month,
							currentTime.year,
							currentTime.hours,
							currentTime.minutes,
							currentTime.seconds);
			osMutexRelease(dataMutex);
		}
		UART4_SendString(buffer);
		osDelay(1000);
	}
}
