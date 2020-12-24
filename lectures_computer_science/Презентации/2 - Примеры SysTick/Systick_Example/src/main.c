#include <stm32f10x.h>



int main(void)
{

	__disable_irq(); // немножко магии

	RCC->APB2ENR |= (1<<4); // тактирование порта С
	GPIOC->CRH &= ~(0xFF);
	GPIOC->CRH |= (1<<0) | (1<<4); // РС8 и РС9 - выходы

	SysTick_Config( 10000 ); // настраиваем таймер, задаем период
	
	while(1)
	{
		;
	}
	
	return 0;
}
