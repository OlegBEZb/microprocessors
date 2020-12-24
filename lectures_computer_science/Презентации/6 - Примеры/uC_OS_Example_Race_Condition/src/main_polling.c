#include <CMSIS/stm32f10x.h>

#include "includes.h"
#include "uart.h"

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

static  OS_STK         App_Task1Stk [ APP_TASK_1_STK_SIZE ];

static  OS_STK         App_Task2Stk [ APP_TASK_2_STK_SIZE ];

static  OS_STK         App_Task3Stk [ APP_TASK_3_STK_SIZE ];

static  OS_STK         App_Task4Stk [ APP_TASK_4_STK_SIZE ];


/***************************************************************************************************
                                       Таски - объявления
***************************************************************************************************/

static void App_TaskStart (void *p_arg);

static void App_Task1 (void *p_arg);

static void App_Task2 (void *p_arg);

static void App_Task3 (void *p_arg);

static void App_Task4(void *p_arg);



void Uart1_Handler(void);

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
		
		// настроить всю остальную периферию тоже можно здесь
		initUsart1();
	
	// настройка СисТика
    OS_CPU_SysTickInit();

    // таск для сбора статистики - если он нужен							
#if (OS_TASK_STAT_EN > 0)
    OSStatInit(); 
#endif

	// дальше создаются пользовательские таски
	
	OSTaskCreate( App_Task1,           
			      0,              
			     &App_Task1Stk[APP_TASK_1_STK_SIZE - 1], 
			      APP_TASK_1_PRIO 
			);
			
	OSTaskCreate( App_Task2,           
		          0,              
		         &App_Task2Stk[APP_TASK_2_STK_SIZE - 1], 
		          APP_TASK_2_PRIO 
		);
		
	OSTaskCreate( App_Task3,           
		          0,              
		         &App_Task3Stk[APP_TASK_3_STK_SIZE - 1], 
		          APP_TASK_3_PRIO 
		);
		
	OSTaskCreate( App_Task4,           
		          0,              
		         &App_Task4Stk[APP_TASK_4_STK_SIZE - 1], 
		          APP_TASK_4_PRIO 
		);
		
		
				
		
	// этот таск больше не нужен 
	OSTaskDel(OS_PRIO_SELF);

}


int commonVariable = 0.0;




static void App_Task1 (void *p_arg)
{
	// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	

	for(uint32_t i = 0; i<10000; i++)
	{
		commonVariable++;

		OSTimeDlyHMSM(0, 0, 0, 1 );
	}
	
	OSTaskDel(OS_PRIO_SELF);
	

}



static void App_Task3 (void *p_arg)
{
	// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	
	for(uint32_t i = 0; i<10000; i++)
	{
		commonVariable++;

		OSTimeDlyHMSM(0, 0, 0, 1 );
	}
	
	OSTaskDel(OS_PRIO_SELF);

}


static void App_Task4(void *p_arg)
{
	// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	

	
	for(uint32_t i = 0; i<10000; i++)
	{
		commonVariable++;

		OSTimeDlyHMSM(0, 0, 0, 3 );
	}
	
	OSTaskDel(OS_PRIO_SELF);

}


static void App_Task2 (void *p_arg)
{
	// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	

	for(uint32_t i = 0; i<10000; i++)
	{
		commonVariable++;

		OSTimeDlyHMSM(0, 0, 0, 2 );
	}
	
	OSTaskDel(OS_PRIO_SELF);

}




int main(void)
{
	
	// запрет прерываний и инициализация ОС 
	BSP_IntDisAll();                                         
	OSInit();                                                


	// создание стартового таска - в нем создаются все остальные
	
    OSTaskCreate( App_TaskStart,  // указатель на таск         
                  0,              // параметр вызова (без параметра)
                 &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1], // указатель на стек
                  APP_TASK_START_PRIO // приоритет
                );

	// запуск многозадачности
    OSStart();                                               

	
	return 0;
}

