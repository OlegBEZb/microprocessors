#include "uart.h"

void initUsart1(void)
{
	
	/***************************************************************************************************
                                      Настройка USART1
	***************************************************************************************************/
	
	// уарту нужны два вывода - RX и TX
	// настраиваем USART1 
	
	// RX - PA10
	// TX - PA9
	
	// подаем тактирование на порт А
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// RX должен быть плавающим входом
	GPIO_InitTypeDef gpioInit;
	
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInit.GPIO_Pin = GPIO_Pin_10;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &gpioInit);
	
	// TX - альтернативным пуш-пуллом
	// используем ту же структуру еще раз
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Pin = GPIO_Pin_9;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &gpioInit);
	
	
	// выводы настроены
	
	
	// теперь настраиваем сам USART
	
	// подаем тактирование на USART
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// создаем структуру для настройки
	USART_InitTypeDef usartInit;

	// выбираем режим - 9600 бит/с, и прием и передача, 1 стоп-бит, 8 бит в пакете
	// без проверки четности и управления потоком
	
	// при настройке SPL опирается на значение макроса HSE_VALUE - частота внешнего кварца. 
	// убедитесь, что его значение верно
	
	usartInit.USART_BaudRate = 9600;
	usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usartInit.USART_Parity = USART_Parity_No;
	usartInit.USART_StopBits = USART_StopBits_1;
	usartInit.USART_WordLength = USART_WordLength_8b;
	
	// применяем настройки
	USART_Init(USART1, &usartInit);
	
	// прерывания - принят байт и отправлен байт - в данном примере они не используются
	// поэтому строчки закомментированы
	
	// USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	 NVIC_EnableIRQ(USART1_IRQn);
	

	// включаем USART
	USART_Cmd(USART1, ENABLE);

}
