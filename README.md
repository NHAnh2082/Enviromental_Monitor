# STM32F407 Real-Time Environmental Monitor

A real-time embedded system project on STM32F407, measuring and displaying temperature, humidity, and CO₂ concentration, with data logging and UI support.

## Features

- Measure **temperature** and **humidity** using DHT11 sensor (GPIO interface)
- Measure **CO₂ concentration** using MQ135 sensor (ADC interface)
- Read **real-time clock** from DS3231 module (I2C interface)
- Display all sensor data and a simple menu on a 0.96-inch OLED SSD1306 (I2C)
- Simple user interface/menu navigation with button input
- Communicate with PC or Qt/QML desktop UI via UART (DMA supported)
- Multitasking and real-time data acquisition with **CMSIS-RTOS2**
- Modular code using **STM32 Standard Peripheral Library** (SPL) on KeilC

## Hardware

- **MCU:** STM32F407
- **Sensors:** DHT11 (temperature & humidity), MQ135 (CO₂, analog)
- **RTC:** DS3231 (I2C)
- **Display:** SSD1306 OLED 0.96", 128x64 (I2C)
- **Button:** Single button for menu navigation
- **UART:** For data communication with PC/Qt UI

## Software Architecture

- **Multithreaded (RTOS) design:** Each task (sensor reading, OLED update, UART, menu, etc.) runs in its own thread.
- **Message Queue & Mutexes** for safe data sharing between tasks.
- **Data Formatting & Conversion:** Convert sensor readings to display/logging format.
- **Qt UI (optional):** UI built with QML communicates with STM32 via UART for data visualization.

## Directory Structure

```cpp
Environmental_Monitor/
│
├── CMSIS/                   # CMSIS RTOS configuration and headers (real-time OS for STM32)
│   ├── RTX_Config.c
│   ├── RTX_Config.h
│
├── Conversion/              # Data conversion and formatting utilities (e.g., number to string)
│   ├── DataConvert.c
│   ├── DataConvert.h
│
├── DHT/                     # DHT11 sensor driver (temperature & humidity)
│   ├── dht.c
│   ├── dht.h
│
├── Display/                 # SSD1306 OLED display driver
│   ├── LCD.c
│   ├── LCD.h
│
├── I2C/                     # I2C communication interface functions
│   ├── i2c.c
│   ├── i2c.h
│
├── LEDs/                    # LED control (status/debug indicator)
│   ├── BlinkLed.c
│   ├── BlinkLed.h
│
├── Libraries/               # STM32F4xx standard peripheral libraries (ADC, DMA, GPIO, I2C, RCC, TIM, USART, etc.)
│   ├── misc.c
│   ├── stm32f4xx_adc.c
│   ├── stm32f4xx_dma.c
│   ├── stm32f4xx_gpio.c
│   ├── stm32f4xx_i2c.c
│   ├── stm32f4xx_rcc.c
│   ├── stm32f4xx_tim.c
│   ├── stm32f4xx_usart.c
│
├── Menu/                    # Menu system and user interface logic for the OLED
│   ├── menu.c
│   ├── menu.h
│
├── MQ135/                   # MQ135 gas sensor driver (CO₂, via ADC)
│   ├── MQ135.c
│   ├── MQ135.h
│
├── OS/                      # RTOS tasks, message queue, mutex, and kernel initialization
│   ├── OS.c
│   ├── OS.h
│
├── RTC/                     # DS3231 real-time clock (RTC) module driver (I2C)
│   ├── DS3231.c
│   ├── DS3231.h
│
├── UART/                    # UART communication (with DMA support) for PC/UI interface
│   ├── com_UI.c
│   ├── com_UI.h
│
├── User/                    # Main entry point and core system files for STM32
│   ├── main.c                   # Main application logic, system initialization, RTOS startup
│   ├── startup_stm32f40_41xxx.s 
│   ├── system_stm32f4xx.c       
│   ├── system_timetick.c        
│
├── UtilsDelay/              # Timer-based delay utilities
│   ├── delay.c
│   ├── delay.h
│
├── UI/                      # UI (Qt/QML), communicates with MCU via UART
│   ├── Header Files/
│   │   └── SerialPortManger.h
│   ├── Source Files/
│   │   ├── main.cpp
│   │   └── SerialPortManger.cpp
│   ├── CO2_icon.qml
│   ├── Main.qml
│   ├── MonitorTab.qml
│   ├── SerialConfigTab.qml
│   └── TimeSettingTab.qml
│
├── CMakeLists.txt           # CMake configuration file (for building the Qt UI)
└── README.md                # Project overview and documentation

```

## Usage

- The device starts, reads all sensors, and displays data on OLED.
- Use the button to navigate menu (temperature/humidity, CO₂, time).
- Data can be sent to PC/UI via UART for further processing or display.

## Dependencies

- [STM32F4 Standard Peripheral Library (SPL)](https://www.st.com/en/embedded-software/stsw-stm32065.html)
- [CMSIS-RTOS2](https://arm-software.github.io/CMSIS_5/RTOS2/html/index.html)
- Qt (for desktop UI, optional)

## Result

![Image](https://github.com/user-attachments/assets/142a1d98-a188-4dff-a327-0d7762df8cbc)

<img width="481" height="357" alt="Image" src="https://github.com/user-attachments/assets/e956e321-2b80-4ab8-94a8-6b6838174434" />

## License

This project is for educational purposes.
