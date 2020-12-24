#include "cmsis/stm32f10x.h"

#include "includes.h"

// типовой вариант функции assert
void assert_failed(uint8_t* file, uint32_t line)
{ 
  while (1)
  {
    ;
  } 
}


/***************************************************************************************************
  Стеки для тасков
***************************************************************************************************/

static  OS_STK         App_TaskStartStk [ APP_TASK_START_STK_SIZE ];

static  OS_STK         App_TaskLed1Stk [ APP_TASK_LED1_STK_SIZE ];

static  OS_STK         App_TaskLed2Stk [ APP_TASK_LED2_STK_SIZE ];


/***************************************************************************************************
  Таски - объявления
***************************************************************************************************/

static void App_TaskStart (void *p_arg);

static void App_TaskLed1 (void *p_arg);

static void App_TaskLed2 (void *p_arg);

/***************************************************************************************************
  Таски
***************************************************************************************************/

// стартовый таск
static void App_TaskStart (void *p_arg)
{
  // это чтобы убрать warning о неиспользуемом аргументе
  (void)p_arg;
  
  //  Фактически - только настройка RCC - 72 МГц от ФАПЧ  (Initialize BSP functions)
  BSP_Init();   
  
  // настройка СисТика
  OS_CPU_SysTickInit();
  
  // таск для сбора статистики - если он нужен              
  #if (OS_TASK_STAT_EN > 0)
    OSStatInit(); 
  #endif
  
  // дальше создаются пользовательские таски
  
  
  OSTaskCreate( App_TaskLed1,    // указатель на функци       
                NULL,            // параметр - без параметра              
                &App_TaskLed1Stk[APP_TASK_LED1_STK_SIZE - 1], // указатель на массив для стека 
                APP_TASK_LED1_PRIO // приоритет
                );
  
  OSTaskCreate( App_TaskLed2,           
                NULL,              
                &App_TaskLed2Stk[APP_TASK_LED2_STK_SIZE - 1], 
                APP_TASK_LED2_PRIO 
                );
  
  // этот таск больше не нужен 
  OSTaskDel(OS_PRIO_SELF);
  
}

// все что делает этот таск - мигает светодиодом РС8
static void App_TaskLed1 (void *p_arg)
{
  // это чтобы убрать warning о неиспользуемом аргументе
  (void)p_arg;
  
  // настройка порта светодиода - делается при первом вызове таска
  // подаем тактирование
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  // делаем РС.8 - выходом пуш-пулл
  GPIO_InitTypeDef gpioC;
  gpioC.GPIO_Mode = GPIO_Mode_Out_PP;
  gpioC.GPIO_Pin = GPIO_Pin_8;
  gpioC.GPIO_Speed = GPIO_Speed_10MHz;
  
  GPIO_Init(GPIOC, &gpioC);
  
  
  // таск никогда не должен завершаться через закрывающую скобку
  // поэтому внутри или удаление или бесконечный цикл
  while(1)
  {
    // собственно мигание
    GPIOC->ODR ^= 1<<8;
    
    // а до следующего мигания делать нечего
    // засыпаем на 1 секунду
    OSTimeDlyHMSM(0, 0, 1, 0);
  }
  
}

// все что делает этот таск - мигает светодиодом РС9
static void App_TaskLed2 (void *p_arg)
{
  // это чтобы убрать warning о неиспользуемом аргументе
  (void)p_arg;
  
  // настройка порта светодиода - делается при первом вызове таска
  // да, в другом таске порт тоже настраивается - но ведь другой таск может
  // быть и не активен!
  
  // подаем тактирование
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  // делаем РС.9 - выходом пуш-пулл
  GPIO_InitTypeDef gpioC;
  gpioC.GPIO_Mode = GPIO_Mode_Out_PP;
  gpioC.GPIO_Pin = GPIO_Pin_9;
  gpioC.GPIO_Speed = GPIO_Speed_10MHz;
  
  GPIO_Init(GPIOC, &gpioC);
  
  
  // таск никогда не должен завершаться через закрывающую скобку
  // поэтому внутри или удаление или бесконечный цикл
  while(1)
  {
    // собственно мигание
    GPIOC->ODR ^= 1<<9;
    
    // а до следующего мигания делать нечего
    // засыпаем на 500 миллисекунд
    OSTimeDlyHMSM(0, 0, 0, 500);
  }
  
}




int main(void)
{
  
  // запрет прерываний и инициализация ОС 
  BSP_IntDisAll();                                         
  OSInit();                                                
  
  
  // создание стартового таска - в нем создаются все остальные
  
  OSTaskCreate( App_TaskStart,     // указатель на таск         
                NULL,              // параметр вызова (без параметра)
                &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1], // указатель на стек
                APP_TASK_START_PRIO // приоритет
                );
  
  // запуск многозадачности
  OSStart();                                               
  
  
  return 0;
}

