#include "stm32f4xx.h"
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
#include "OS.h"

// Bi?n toàn c?c d? theo dõi k?t qu? trong Watch Window
//volatile uint8_t found_address = 0;
//volatile uint8_t found_addresses[10];
//volatile uint8_t found_count = 0;

//#define I2C_TIMEOUT     10000

//void delay(volatile uint32_t time)
//{
//    while (time--) __NOP();
//}

//void I2C_Scanner_Init(void)
//{
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

//    GPIO_InitTypeDef GPIO_InitStructure;
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // PB6=SCL, PB7=SDA
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

//    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); // PB6 -> I2C1_SCL
//    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // PB7 -> I2C1_SDA

//    I2C_InitTypeDef I2C_InitStructure;
//    I2C_DeInit(I2C1);
//    I2C_InitStructure.I2C_ClockSpeed = 100000;
//    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
//    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//    I2C_Init(I2C1, &I2C_InitStructure);

//    I2C_Cmd(I2C1, ENABLE);
//}

//uint8_t I2C_Scanner_ScanAddress(uint8_t address)
//{
//    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

//    I2C_GenerateSTART(I2C1, ENABLE);
//    uint32_t timeout = I2C_TIMEOUT;
//    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
//        if ((timeout--) == 0) return 0;
//    }

//    I2C_Send7bitAddress(I2C1, address << 1, I2C_Direction_Transmitter);
//    timeout = I2C_TIMEOUT;
//    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
//        if ((timeout--) == 0) {
//            I2C_GenerateSTOP(I2C1, ENABLE);
//            return 0;
//        }
//    }

//    I2C_GenerateSTOP(I2C1, ENABLE);
//    return 1;
//}

//void I2C_Scanner_Run(void)
//{
//    for (uint8_t addr = 1; addr < 127; addr++) {
//        if (I2C_Scanner_ScanAddress(addr)) {
//            found_address = addr;
//            found_addresses[found_count++] = addr;
//            delay(50000);  // Tránh loop quá nhanh, xem d? hon trong Watch
//        }
//    }
//}

int main(void) 
{
	SystemInit();              // CMSIS kh?i t?o system clock
	SystemCoreClockUpdate();
	
	TIM6_Config();
	LED_Config();
	MQ135_GPIO_Config();
	BUTTON_Config();
	UART_Config();
	I2C1_Init();
	OLED_Init();
	OLED_Clear();
		
	
	osKernelInitialize();
	
//	const osMutexAttr_t myMutexAttr = 
//	{
//    .name = "MyMutex",
//    .attr_bits = osMutexRecursive,  // ho?c 0 n?u không c?n recursive
//    .cb_mem = NULL,
//    .cb_size = 0
//	};
//	
	dataMutex = osMutexNew(NULL);
	
	if (dataMutex == NULL) 
	{
    UART4_SendString("Failed to create mutex\r\n");
    GPIO_SetBits(LED_PORT, LED_PIN);  // LED d? báo l?i ch?ng h?n
		delay_s(3);
    while (1);  // Treo t?i ch? d? d? nh?n ra
	}
	
	sensorQueue = osMessageQueueNew(QUEUE_SIZE, sizeof(SensorMessage_t), NULL);
	if (sensorQueue == NULL)
	{
		UART4_SendString("Error: MessageQueue creation failed\r\n");
		GPIO_SetBits(LED_PORT, LED_PIN);
		while (1);
	}

	
	osThreadNew(Task_ReadTemp, NULL, &(osThreadAttr_t)
	{
		.name = "DHT11",
		.priority = osPriorityNormal,
		.stack_size = 512
	});

	osThreadNew(Task_MQ135, NULL, &(osThreadAttr_t)
	{
		.name = "MQ135",
		.priority = osPriorityNormal,
		.stack_size = 512
	});

	osThreadNew(Task_RTC, NULL, &(osThreadAttr_t)
	{
		.name = "RTC",
		.priority = osPriorityNormal,
		.stack_size = 512
	});

	osThreadNew(Task_OLED, NULL, &(osThreadAttr_t)
	{
		.name = "OLED",
		.priority = osPriorityHigh,
		.stack_size = 1024
	});
	
	
	
	osThreadNew(Task_Menu, NULL, &(osThreadAttr_t)
	{
		.name = "Menu",
		.priority = osPriorityAboveNormal,
		.stack_size = 512
	});
	
	
	osThreadNew(Task_SendData, NULL, &(osThreadAttr_t)
	{
    .name = "SendData",
    .priority = osPriorityBelowNormal,
    .stack_size = 512
  });
	
	osKernelStart();
		
//	DHT_TYPE type = DHT11;
	
//  Write time once
//	RTC_Time newTime = {00, 24, 17, 6, 20, 06, 25}; // 12:33:30, 20/06/2025
//	DS3231_SetTime(&newTime);
	
//	float rs_sum = 0.0f;
//	int n = 100;
//	
//	for (int i=0; i<n; i++)
//	{
//		adc_raw = MQ135_ReadADC();
//		voltage = ((float)adc_raw / 4095.0f) * 3.3f;
//		Rs = RL * (Vref - voltage) / voltage;
//		rs_sum += Rs;
//		delay_s(1);
//	}
//	
//	Ro = rs_sum / n;

//	SystemInit();
//	I2C_Scanner_Init();
//  I2C_Scanner_Run();

//	char buffer[BUFF_SIZE_TX];
	
	while(1) 
	{
//		adc_raw = MQ135_ReadADC();
//    voltage = ((float)adc_raw / 4095.0f) * 3.3f;
//		ppm = MQ135_GetPPM(adc_raw);
//		delay_s(1);
		
//		
//		if (DHT_GetData(type, &temp, &humi)) 
//		{
//			GPIO_SetBits(LED_PORT, LED_PIN); 
//		}
//		else 
//		{
//			GPIO_ResetBits(LED_PORT, LED_PIN); 
//		}

		
//		OLED_SetCursor(0,0);
//		convert(buffer, "T:%f oC  H:%f%", temp, humi);
//    OLED_Puts(buffer);
//		
//		OLED_SetCursor(2,0);
//		convert(buffer, "CO2:%f ppm", ppm);
//    OLED_Puts(buffer);

//		delay_s(1);
//		GPIO_ResetBits(LED_PORT, LED_PIN); 


//		DS3231_GetTime(&currentTime);


//		OLED_SetCursor(4,0);
//		if (currentTime.day == 1)
//			convert(buffer, "%s", "Sunday");
//		else if (currentTime.day == 2)
//			convert(buffer, "%s", "Monday");
//		else if (currentTime.day == 3)
//			convert(buffer, "%s", "Tuesday");
//		else if (currentTime.day == 4)
//			convert(buffer, "%s", "Wednesday");
//		else if (currentTime.day == 5)
//			convert(buffer, "%s", "Thursday");
//		else if (currentTime.day == 6)
//			convert(buffer, "%s", "Friday");
//		else if (currentTime.day == 7)
//			convert(buffer, "%s", "Saturday");
//		else
//			convert(buffer, "Day: Unknown");
//		OLED_Puts(buffer);

//		OLED_SetCursor(6,0);
//		convert(buffer, "%d:%d:%d, %d/%d/20%d", 
//						currentTime.hours,
//						currentTime.minutes,
//						currentTime.seconds,
//						currentTime.date,
//						currentTime.month,
//						currentTime.year);
//		OLED_Puts(buffer);


//		convert(buffer, "DATA_T=%f,H=%f,C=%f,DAY=%d,DATE=%d/%d/20%d,TIME=%d/%d/%d_END\r\n",
//						temp,
//						humi,
//						ppm,
//						currentTime.day,
//						currentTime.date,
//						currentTime.month,
//						currentTime.year,
//						currentTime.hours,
//						currentTime.minutes,
//						currentTime.seconds);
//		UART4_SendString(buffer);
//		GPIO_SetBits(LED_PORT, LED_PIN); 
//		delay_s(1);
//		GPIO_ResetBits(LED_PORT, LED_PIN); 
//		delay_s(1);
	}
}
