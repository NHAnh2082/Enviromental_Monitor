#include "delay.h"

// Timer6 Configuration for Delay
void TIM6_Config(void) 
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1; // 1us tick (84MHz / 84)
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
	TIM_Cmd(TIM6, ENABLE);
}

void delay_us(uint32_t us) 
{
	TIM_SetCounter(TIM6, 0);
	while (TIM_GetCounter(TIM6) < us);
}

void delay_ms(uint32_t ms) 
{
	while (ms--) delay_us(1000);
}

void delay_s(uint32_t s) 
{
	while (s--) delay_ms(1000);
}
