#include <stm32f10x.h>

uint32_t count = 0;

void SysTick_Handler(void)
{
  count++;
}


int main(void)
{

  // прерывания по-умолчанию разрешены

  RCC->APB2ENR |= (1<<4); // тактирование порта С
  GPIOC->CRH &= ~(0xFF);
  GPIOC->CRH |= (1<<0) | (1<<4); // РС8 и РС9 - выходы

  SysTick_Config( SystemCoreClock/1000 ); // настраиваем таймер, задаем период

  while(1)
  {
    // светодиод загорится после 1000 прерываний..
    if(count > 1000)
    {
      GPIOC->ODR |= 1<<8;
    }
  }

  return 0;
}
