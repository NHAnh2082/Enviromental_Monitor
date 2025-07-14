#include "menu.h"

const char* menu_items[] = 
{
	"1. Temp & Humi",
  "2. CO2 Level",
  "3. Time",
	"4. Watch All"
};

#define MENU_SIZE (sizeof(menu_items) / sizeof(menu_items[0]))
#define IDLE_TIMEOUT_MS 3000

State_t state = STATE_MENU;

//uint8_t menu_index = 0;
//uint32_t btn_press_time = 0;
//uint8_t btn_pressed = 0;
uint8_t menu_index = 0;
uint8_t btn_pressed = 0;
uint32_t idle_time = 0;
uint8_t selecting = 0;
static uint8_t last_index = 0xFF;


void BUTTON_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t BUTTON_Read(void)
{
	return (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET);
}

void OLED_DrawMenu(uint8_t selected)
{	
	if (last_index == 0xFF)
	{
		for (uint8_t i=0; i<MENU_SIZE; i++) 
		{
			OLED_SetCursor(i * 2, 0);  
			
			if (i == selected)
				OLED_Puts("> "); // Highlight
			else
				OLED_Puts("  ");
			
			OLED_Puts((char*)menu_items[i]);
		}	
	}
	else 
	{
		// delete old highlight
		OLED_SetCursor(last_index * 2, 0);
		OLED_Puts("  ");                   // Xóa d?u '>'
		OLED_Puts((char*)menu_items[last_index]);

		// new highlight 
		OLED_SetCursor(selected * 2, 0);
		OLED_Puts("> ");
		OLED_Puts((char*)menu_items[selected]);
	}
	last_index = selected;
}

void Menu_Handler(void)
{
//	static State_t last_state = STATE_MENU;
//	static uint8_t last_index = 0;
//	static uint8_t selecting = 0;
//	static uint8_t btn_pressed = 0;
//	static uint32_t btn_press_start = 0;
//	char buf[32];

//	if (BUTTON_Read()) // dang nh?n
//	{
//		if (!btn_pressed)
//		{
//			btn_pressed = 1;
//			btn_press_start = osKernelGetTickCount();  // ghi l?i th?i di?m b?t d?u nh?n
//		}
//		else
//		{
//			uint32_t held_time = osKernelGetTickCount() - btn_press_start;

//			if (held_time >= 3000)
//			{
//				if (state == STATE_MENU && selecting == 0)
//				{
//					state = (State_t)(menu_index + 1);  // chuy?n sang VIEW
//					selecting = 1;
//					OLED_Clear();
//				}
//				else if (selecting == 1)
//				{
//					state = STATE_MENU;
//					selecting = 0;
//					OLED_Clear();
//					Menu_Animation(menu_index, menu_index);
//					OLED_DrawMenu(menu_index);
//				}

//				while (BUTTON_Read());  // ch? nh? nút ra
//				btn_pressed = 0;
//			}
//		}
//	}
//	else // dã th? nút
//	{
//		if (btn_pressed)
//		{
//			uint32_t held_time = osKernelGetTickCount() - btn_press_start;

//			if (held_time < 3000)  // nh?n ng?n ? d?i dòng menu
//			{
//				if (state == STATE_MENU && selecting == 0)
//				{
//					last_index = menu_index;
//					menu_index = (menu_index + 1) % MENU_SIZE;
//					Menu_Animation(last_index, menu_index);
//					OLED_DrawMenu(menu_index);
//				}
//			}

//			btn_pressed = 0;
//		}
//	}
//	
//	char debug[32];
//	convert(debug, "State: %d", state);
//	OLED_SetCursor(7, 0);
//	OLED_Puts(debug);

//	// FSM hi?n th? d? li?u
//	switch (state)
//	{
//		case STATE_MENU:
//			// Không làm gì vì dã hi?n th? menu b?ng OLED_DrawMenu()
//			break;

//		case STATE_VIEW_TEMP:
//			OLED_Clear();
//			
//			OLED_SetCursor(0, 0);
//			
//			convert(buf, "Temp: %f oC", temp);
//			OLED_Puts(buf);
//		
//			OLED_SetCursor(2, 0);
//			convert(buf, "Humi: %f %", humi);
//			OLED_Puts(buf);
//			break;

//		case STATE_VIEW_CO2:
//			OLED_Clear();
//			OLED_SetCursor(0, 0);
//			convert(buf, "CO2: %f ppm", ppm);
//			OLED_Puts(buf);
//			break;

//		case STATE_VIEW_TIME:
//			OLED_Clear();
//			OLED_SetCursor(0, 0);
//			convert(buf, "Time: %d:%d:%d", currentTime.hours, currentTime.minutes, currentTime.seconds);
//			OLED_Puts(buf);
//			OLED_SetCursor(2, 0);
//			convert(buf, "Date: %d/%d/20%d", currentTime.date, currentTime.month, currentTime.year);
//			OLED_Puts(buf);
//			break;
//	}

	static uint8_t btn_pressed = 0;
	static uint32_t btn_debounce_tick = 0;
	#define BTN_DEBOUNCE_TIME 30  // ms
	static uint32_t last_tick = 0;
	uint32_t now = osKernelGetTickCount();
	
	// L?n d?u vào menu thì v? menu
		static uint8_t first_draw = 1;

	if (state == STATE_MENU && selecting == 0)
	{
		if (first_draw)
		{
			OLED_Clear();
			last_index = 0xFF;
			OLED_DrawMenu(menu_index); // V? t?i option 0
			first_draw = 0;
			last_tick = now;
		}
		
		// X? lý nút nh?n: chuy?n dòng menu
		if (BUTTON_Read())
		{
			if (!btn_pressed && (now - btn_debounce_tick) > BTN_DEBOUNCE_TIME)
			{
				btn_pressed = 1;
				last_tick = now;
				menu_index = (menu_index + 1) % MENU_SIZE;
				OLED_DrawMenu(menu_index);
			}
		}
		else
		{
			if (btn_pressed&& (now - btn_debounce_tick) > BTN_DEBOUNCE_TIME)
			{
				btn_pressed = 0;
				btn_debounce_tick = now;
			}
		}

		// Timeout không tuong tác thì chuy?n sang view
		if ((now - last_tick) >= IDLE_TIMEOUT_MS)
		{
			state = (State_t)(menu_index + 1); // STATE_VIEW_*
			selecting = 1;
			OLED_Clear();
		}
	}
	else if (selecting == 1)
	{
		// Ðang ? tr?ng thái xem d? li?u
		char buf[32];
		
		switch (state)
		{
			case STATE_VIEW_TEMP:
				OLED_SetCursor(0, 0);
				convert(buf, "Temp: %f oC", temp);
				OLED_Puts(buf);
				OLED_SetCursor(2, 0);
				convert(buf, "Humi: %f %", humi);
				OLED_Puts(buf);
				break;
			
			case STATE_VIEW_CO2:
				OLED_SetCursor(0, 0);
				convert(buf, "CO2: %f ppm", ppm);
				OLED_Puts(buf);
				break;
			
			case STATE_VIEW_TIME:
				OLED_SetCursor(0,0);
				if (currentTime.day == 1)
					convert(buf, "%s", "Sunday");
				else if (currentTime.day == 2)
					convert(buf, "%s", "Monday");
				else if (currentTime.day == 3)
					convert(buf, "%s", "Tuesday");
				else if (currentTime.day == 4)
					convert(buf, "%s", "Wednesday");
				else if (currentTime.day == 5)
					convert(buf, "%s", "Thursday");
				else if (currentTime.day == 6)
					convert(buf, "%s", "Friday");
				else if (currentTime.day == 7)
					convert(buf, "%s", "Saturday");
				else
					convert(buf, "Day: Unknown");
				OLED_Puts(buf);
				
				OLED_SetCursor(2,0);
				convert(buf, "Time: %d:%d:%d", currentTime.hours, currentTime.minutes, currentTime.seconds);
				OLED_Puts(buf);
				
				OLED_SetCursor(4,0);
				convert(buf, "Date: %d/%d/20%d", currentTime.date, currentTime.month, currentTime.year);
				OLED_Puts(buf);
				break;
				
			case STATE_ALL:
				OLED_SetCursor(0,0);
				convert(buf, "T: %f oC  H: %f %", temp, humi);
				OLED_Puts(buf);
				
				OLED_SetCursor(2,0);
				convert(buf, "CO2: %f ppm", ppm);
				OLED_Puts(buf);
				
				OLED_SetCursor(4,0);
				if (currentTime.day >= 1 && currentTime.day <= 7) 
				{
					static const char *dayStr[] = 
						{"", "Sunday", 
						 "Monday", "Tuesday", 
						 "Wednesday", "Thursday", 
						 "Friday", "Saturday"
						};
					convert(buf, "%s", dayStr[currentTime.day]);
				} 
				else 
				{
					convert(buf, "%s", "Day: Unknown");
				}
//				OLED_Puts("                ");    // Clear dòng tru?c
//				OLED_SetCursor(4,0);            // Ð?t l?i cursor
				OLED_Puts(buf);
				
				OLED_SetCursor(6,0);
				convert(buf, "%d:%d:%d, %d/%d/20%d", 
								currentTime.hours, currentTime.minutes, currentTime.seconds, 
								currentTime.date, currentTime.month, currentTime.year);
				OLED_Puts(buf);
				break;
			
			default:
				break;
		}

		// B?m nút b?t k? d? quay v? menu
		if (BUTTON_Read())
		{
			if (!btn_pressed && (now - btn_debounce_tick) > BTN_DEBOUNCE_TIME)
			{
				btn_pressed = 1;
				btn_debounce_tick = now;
				state = STATE_MENU;
				selecting = 0;
				OLED_Clear();
				last_index = 0xFF;  // force to draw all menu
				OLED_DrawMenu(menu_index);
				last_tick = osKernelGetTickCount();
				first_draw = 1;
			}
		}
		else
		{
			if (btn_pressed && (now - btn_debounce_tick) > BTN_DEBOUNCE_TIME)
			{
				btn_pressed = 0;
				btn_debounce_tick = now;
			}
		}
	}
}
