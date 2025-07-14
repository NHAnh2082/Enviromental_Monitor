#include "DS3231.h"

RTC_Time currentTime;

char timeStr[32];

void DS3231_Write(uint8_t reg_addr, uint8_t data) 
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, 0x68 << 1, I2C_Direction_Transmitter); // 0x68 << 1 | 0
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(I2C1, reg_addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	I2C_SendData(I2C1, data);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);
}

uint8_t DS3231_Read(uint8_t reg_addr) 
{
	uint8_t data = 0;
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, 0x68 << 1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, reg_addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, 0xD0, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	data = I2C_ReceiveData(I2C1);

	I2C_AcknowledgeConfig(I2C1, DISABLE);
	I2C_GenerateSTOP(I2C1, ENABLE);

	return data;
}

uint8_t BCD_To_Dec(uint8_t val) 
{
	return ((val >> 4) * 10) + (val & 0x0F);
}

uint8_t Dec_To_BCD(uint8_t val) 
{
	return ((val / 10) << 4) | (val % 10);
}

void DS3231_GetTime(RTC_Time *time) 
{
	time->seconds = BCD_To_Dec(DS3231_Read(0x00));
	time->minutes = BCD_To_Dec(DS3231_Read(0x01));
	time->hours   = BCD_To_Dec(DS3231_Read(0x02));
	time->day     = BCD_To_Dec(DS3231_Read(0x03));
	time->date    = BCD_To_Dec(DS3231_Read(0x04));
	time->month   = BCD_To_Dec(DS3231_Read(0x05) & 0x1F);  // Mask century bit
	time->year    = BCD_To_Dec(DS3231_Read(0x06));
}

void DS3231_SetTime(RTC_Time *time) 
{
	DS3231_Write(0x00, Dec_To_BCD(time->seconds));
	DS3231_Write(0x01, Dec_To_BCD(time->minutes));
	DS3231_Write(0x02, Dec_To_BCD(time->hours));
	DS3231_Write(0x03, Dec_To_BCD(time->day));
	DS3231_Write(0x04, Dec_To_BCD(time->date));
	DS3231_Write(0x05, Dec_To_BCD(time->month));
	DS3231_Write(0x06, Dec_To_BCD(time->year));
}

//void FormatTimeString(void) 
//{
//	sprintf(timeStr, "%02d:%02d:%02d, %02d/%02d/20%02d",
//					currentTime.hours,
//					currentTime.minutes,
//					currentTime.seconds,
//					currentTime.date,
//					currentTime.month,
//					currentTime.year);
//}
