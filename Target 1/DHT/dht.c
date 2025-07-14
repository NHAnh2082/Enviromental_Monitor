#include "dht.h"
#include "delay.h"

volatile float temp, humi;

static void DHT_Pin_Output(void) 
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(DHT_RCC_GPIO, ENABLE);

	GPIO_InitStruct.GPIO_Pin   = DHT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(DHT_GPIO_PORT, &GPIO_InitStruct);
}

static void DHT_Pin_Input(void) 
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(DHT_RCC_GPIO, ENABLE);

	GPIO_InitStruct.GPIO_Pin  = DHT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(DHT_GPIO_PORT, &GPIO_InitStruct);
}

static uint8_t DHT_Read(void) 
{
	uint8_t data = 0;

	for (int i=0; i<8; i++) 
	{
		while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == Bit_RESET);
		delay_us(30);

		if (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == Bit_SET)
		{
			data |= (1 << (7 - i));
		}

		while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == Bit_SET);
	}

	return data;
}

static uint8_t wait_response(void) 
{
	uint32_t timeout = 200;

	while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) && timeout--) delay_us(1);
	if (!timeout) return 0;

	timeout = 200;
	while (!GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) && timeout--) delay_us(1);
	if (!timeout) return 0;

	timeout = 200;
	while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) && timeout--) delay_us(1);
	if (!timeout) return 0;

	return 1;
}

uint8_t DHT_GetData(DHT_TYPE type, volatile float *temp, volatile float *humi) 
{
	uint8_t buf[5];

	DHT_Pin_Output();
	GPIO_ResetBits(DHT_GPIO_PORT, DHT_GPIO_PIN);
	
	if (type == DHT11)
		delay_ms(20);
  else
		delay_ms(1);
        
	GPIO_SetBits(DHT_GPIO_PORT, DHT_GPIO_PIN);
	delay_us(30);
	DHT_Pin_Input();

	if (wait_response()) 
	{
		for (int i=0; i<5; i++)
		{
			buf[i] = DHT_Read();
		}

		if (buf[4] == ((buf[0] + buf[1] + buf[2] + buf[3]) & 0xFF))
		{
			if (type == DHT11)
			{
				// read humidity data
				*humi = buf[0];
				
				// read temperature data
				*temp = buf[2];
				return 1;
			}
			else if (type == DHT22)
			{
				// read humidity data
				*humi = ((buf[0] << 8) | buf[1]) / 10.0f;
				
				// read temperature data
				int16_t temperature = (buf[2] << 8) | buf[3];
				
				if (temperature & 0x8000)
				{
					temperature &= 0x7FFF;
					*temp = -((float)temperature / 10.0f);
				}
				else 
				{
					*temp = ((float)temperature / 10.0f);
				}
			}
		}
	}
	return 0;
}
