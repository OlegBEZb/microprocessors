#include <CMSIS/stm32f10x.h>

#include "includes.h"

void initUsart1(void);

void usart1_SendString(char * str);

typedef struct
{
	uint8_t buf[255];
	
	uint8_t size;
	
} Message;


void usart1_SendMessage(Message * msg);

void Uart1_Irq_handler(void);


// вот так делать вообще-то не хорошо
extern OS_EVENT * Uart1_IRQSem;
