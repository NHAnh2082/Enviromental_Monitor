#include "com_UI.h"

char txbuff[BUFF_SIZE_TX];
char rxbuff[BUFF_SIZE_RX];

static void UART_GPIO_Config(void)
{		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	// Configure GPIO pin for UART Tx & Rx
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin   = UART_GPIO_TX | UART_GPIO_RX;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UART_PORT, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
       
	/* USARTx configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	
	// Configure UART4
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;;
	USART_Init(UART4, &USART_InitStruct);

	// Enable UART
	USART_Cmd(UART4, ENABLE);
}

static void txDMA1_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Stream4);
  while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)txbuff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = BUFF_SIZE_TX;								
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// Default configure
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);
	
	/* Enable DMA1, Stream 4 */
	DMA_Cmd(DMA1_Stream4, ENABLE);
	
	/* Enable UART Tx DMA */
	USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
}

static void rxDMA1_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rxbuff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = BUFF_SIZE_RX;								
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// Default configure
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream2, ENABLE);
	
	/* Enable UART Rx DMA */
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Transfer complete interrupt mask */
	DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
}

void UART_Config(void)
{
	UART_GPIO_Config();
	txDMA1_Config();
//	rxDMA1_Config();
}

void UART4_SendString(const char* str)
{
	uint16_t len = strlen(str);
	if (len == 0 || len >= BUFF_SIZE_TX) return;

	memcpy(txbuff, str, len);
	
	DMA_Cmd(DMA1_Stream4, DISABLE);
	DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
	DMA_SetCurrDataCounter(DMA1_Stream4, len);
	DMA_Cmd(DMA1_Stream4, ENABLE);
	while (DMA_GetFlagStatus(DMA1_Stream4, DMA_FLAG_TCIF4) == RESET);
}
