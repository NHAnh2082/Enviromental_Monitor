#include "MQ135.h"
#include "stm32f4xx_gpio.h"

uint16_t adc_raw = 0;
float voltage = 0.0f;
float ppm = 0.0f;
float Rs = 0.0f;
//float Ro = 0.0f;

void MQ135_GPIO_Config(void)
{
	// Enable clock for GPIOA and ADC1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	// Configure PA1 as analog input
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = MQ135_AOUT_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(MQ135_AOUT_PORT, &GPIO_InitStruct);
	
	// Used to configure common features for all ADCs on the same ADC set (ADC1/ADC2/ADC3)
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	// Configure each ADC individually (ADC1, ADC2, ADC3)
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStruct);
	
	// Config channel ADC1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);

	// Enable ADC1
	ADC_Cmd(ADC1, ENABLE);

	// Start conversion use software
	ADC_SoftwareStartConv(ADC1);
}

uint16_t MQ135_ReadADC(void)
{
	// Start conversion use software
	ADC_SoftwareStartConv(ADC1);
	
	// Wait for flag
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	// Return value
	return ADC_GetConversionValue(ADC1); // 12-bit: 0..4095
}

float MQ135_GetPPM(uint16_t adc_value)
{
	voltage = (float)adc_value / 4095.0f * Vref;
	Rs = RL * (Vref - voltage) / voltage;
	float ratio = Rs/Ro;
	return powf(10,b) * powf(ratio, a);
}
