#ifndef MQ135_H
#define MQ135_H

#include "stm32f4xx.h"                  // Device header
#include "math.h"

#define RL 		10000.0f
#define Vref 	3.3f
#define Ro 		30791.7832f

#define a			1.322f
#define b			3.000f

#define MQ135_AOUT_PORT		GPIOA
#define MQ135_AOUT_PIN		GPIO_Pin_1
#define MQ135_ADC_Channel	ADC_Channel_1

extern uint16_t adc_raw;
extern float voltage;
extern float ppm;
extern float Rs;
//extern float Ro;

// Configure GPIO Pin (AOUT and DOUT)
void MQ135_GPIO_Config(void);

// Read ADC value return from AOUT
uint16_t MQ135_ReadADC(void);

// Get PPM value
float MQ135_GetPPM(uint16_t adc_value);

#endif	// MQ135_H
