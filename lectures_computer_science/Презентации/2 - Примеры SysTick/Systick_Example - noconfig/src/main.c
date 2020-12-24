#include <stm32f10x.h>

int main(void)
{

  RCC->APB2ENR |= (1<<4); // тактирование порта С
  GPIOC->CRH &= ~(0xFF);
  GPIOC->CRH |= (1<<0) | (1<<4); // РС8 и РС9 - выходы

  // Задаем начало отсчета
  SysTick->LOAD  = 10000 - 1;      
  //SysTick->LOAD  = 90 - 1;      

  // выбираем источник тактирования, и запускаем таймер
  SysTick->CTRL  = 1<<2 | 1<<0;


  while(1)
  {
    if(SysTick->VAL == 0)
    {
      GPIOC->ODR ^= (1<<8) | (1<<9);
    }
  }

  return 0;
}
