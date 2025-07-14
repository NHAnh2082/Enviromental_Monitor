#ifndef DHT_H
#define DHT_H

#include "stm32f4xx.h"                  // Device header

#define DHT_GPIO_PORT 	GPIOD
#define DHT_GPIO_PIN 		GPIO_Pin_0
#define DHT_RCC_GPIO 		RCC_AHB1Periph_GPIOD

typedef enum
{
	DHT11,
	DHT22
} DHT_TYPE;

extern volatile float temp, humi;

uint8_t DHT_GetData(DHT_TYPE type, volatile float *temp, volatile float *humi);

#endif	// DHT_H
