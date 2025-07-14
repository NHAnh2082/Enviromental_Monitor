#ifndef I2C_H
#define I2C_H

#include "stm32f4xx.h"                  // Device header

/**
 * @brief   I2C1 LCD control pins and peripheral definitions
 */
#define I2C_Channel		I2C1
#define I2C1_SCL			GPIO_Pin_6
#define I2C1_SDA			GPIO_Pin_7
#define I2C1_PORT			GPIOB

void I2C1_Init(void);

#endif	// I2C_H
