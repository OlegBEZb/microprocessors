#include <stm32f10x.h>

uint32_t count = 0;

void SysTick_Handler(void)
{
  static uint32_t i = 0;

  if(i < 1000 )
  {
    count++;
    i++;
  }
}


int main(void)
{

  // прерывания по-умолчанию разрешены

  RCC->APB2ENR |= (1<<4); // тактирование порта С
  GPIOC->CRH &= ~(0xFF);
  GPIOC->CRH |= (1<<0) | (1<<4); // РС8 и РС9 - выходы

  SysTick_Config( 100 ); // настраиваем таймер, задаем период

  static uint32_t j = 0;

  while(1)
  {
    for(; j<1000; j++)
    {
      count++;
    }
  }

  return 0;
}
