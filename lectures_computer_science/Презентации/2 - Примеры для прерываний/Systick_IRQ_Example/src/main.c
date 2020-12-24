#include <stm32f10x.h>

void SysTick_Handler(void)
{
  GPIOC->ODR ^= 1<<8;
}


int main(void)
{

  // прерывания по-умолчанию разрешены

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
