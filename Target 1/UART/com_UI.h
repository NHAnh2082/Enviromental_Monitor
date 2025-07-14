#ifndef COM_UI_H
#define COM_UI_H

#include "stm32f4xx.h"                  // Device header
#include "string.h"
#include "BlinkLed.h"

#define UART_PORT			GPIOC
#define UART_GPIO_TX	GPIO_Pin_10
#define UART_GPIO_RX	GPIO_Pin_11

#define BUFF_SIZE_TX 	128
#define BUFF_SIZE_RX 	128

extern char txbuff[BUFF_SIZE_TX];
extern char rxbuff[BUFF_SIZE_RX];

void UART_Config(void);

void DMA1_Stream2_IRQHandler(void);

void UART4_SendString(const char* str);

#endif	// COM_UI_H
