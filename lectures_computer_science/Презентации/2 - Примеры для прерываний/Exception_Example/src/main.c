#include <stm32f10x.h>

// я объявил тип - указатель на функцию
typedef void (*foo)(void);

foo f = 0;

int main(void)
{
  
  RCC->APB2ENR |= (1<<4); // тактирование порта С
  GPIOC->CRH &= ~(0xFF);
  GPIOC->CRH |= (1<<0) | (1<<4); // РС8 и РС9 - выходы
    
  f(); // вызываю функцию по нулевому указателю


  return 0;
}
