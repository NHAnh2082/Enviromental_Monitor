#ifndef BLINKLED_H
#define BLINKLED_H

#include "stm32f4xx.h"                  // Device header

#define LED_PORT    	GPIOD
#define LED_PIN     	GPIO_Pin_13
#define LED_RCC_GPIO 	RCC_AHB1Periph_GPIOD

void LED_Config(void);

#endif	// BLINKLED_H
