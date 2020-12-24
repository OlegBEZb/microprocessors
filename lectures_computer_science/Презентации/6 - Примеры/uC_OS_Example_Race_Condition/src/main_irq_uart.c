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
                                       Объекты синхронизации
***************************************************************************************************/

// этот семафор сигнализирует, что байт пришел
OS_EVENT * Uart1_IRQSem;


/***************************************************************************************************
                                       Стеки для тасков
***************************************************************************************************/

static  OS_STK         App_TaskStartStk [ APP_TASK_START_STK_SIZE ];

static  OS_STK         App_Task1Stk [ APP_TASK_1_STK_SIZE ];

static  OS_STK         App_Task2Stk [ APP_TASK_2_STK_SIZE ];

static  OS_STK         App_Task3Stk [ APP_TASK_3_STK_SIZE ];

static  OS_STK         App_Task4Stk [ APP_TASK_4_STK_SIZE ];

static  OS_STK         App_TaskUartListenerStk [ APP_TASK_UART_LISTENER_STK_SIZE ];


/***************************************************************************************************
                                       Таски - объявления
***************************************************************************************************/

static void App_TaskStart (void *p_arg);

static void App_Task1 (void *p_arg);

static void App_Task2 (void *p_arg);

static void App_Task3 (void *p_arg);

static void App_Task4(void *p_arg);


static void App_TaskUartListener(void *p_arg);




/***************************************************************************************************
                                           Таски
***************************************************************************************************/


int commonVariable = 0;




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

		
		// проинициализируем примитивы синхронизации
		
		// создаем семафор, принимающий значения 1 или 0
		Uart1_IRQSem = OSSemCreate(0);

				
		
		// проинициализируем прерывания
		
		// проставляем заглушки
		BSP_IntInit();
	
		BSP_IntVectSet(BSP_INT_ID_USART1, Uart1_Irq_handler);
		
		NVIC_EnableIRQ(USART1_IRQn);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		



	// дальше создаются пользовательские таски
	
	
	OSTaskCreate((void (*)(void *)) App_Task1,           
			 (void          * ) 0,              
			 (OS_STK        * )&App_Task1Stk[APP_TASK_1_STK_SIZE - 1], 
			 (INT8U           ) APP_TASK_1_PRIO 
			);
			
	OSTaskCreate((void (*)(void *)) App_Task2,           
		 (void          * ) 0,              
		 (OS_STK        * )&App_Task2Stk[APP_TASK_2_STK_SIZE - 1], 
		 (INT8U           ) APP_TASK_2_PRIO 
		);
		
	OSTaskCreate((void (*)(void *)) App_Task3,           
		 (void          * ) 0,              
		 (OS_STK        * )&App_Task3Stk[APP_TASK_3_STK_SIZE - 1], 
		 (INT8U           ) APP_TASK_3_PRIO 
		);
		
	OSTaskCreate((void (*)(void *)) App_Task4,           
		 (void          * ) 0,              
		 (OS_STK        * )&App_Task4Stk[APP_TASK_4_STK_SIZE - 1], 
		 (INT8U           ) APP_TASK_4_PRIO 
		);
		
	OSTaskCreate((void (*)(void *)) App_TaskUartListener,           
		 (void          * ) 0,              
		 (OS_STK        * )&App_TaskUartListenerStk[APP_TASK_UART_LISTENER_STK_SIZE - 1], 
		 (INT8U           ) APP_TASK_UART_LISTENER_PRIO 
		);
		
		
				
		
	// этот таск больше не нужен 
	OSTaskDel(OS_PRIO_SELF);

}


static void App_Task1 (void *p_arg)
{
	// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	

	for(uint32_t i = 0; i<10000; i++)
	{
		commonVariable = commonVariable + 1;

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
		commonVariable = commonVariable + 1;

		OSTimeDlyHMSM(0, 0, 0, 2 );
	}
	
	OSTaskDel(OS_PRIO_SELF);
	

}


static void App_Task2 (void *p_arg)
{
	// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	

	for(uint32_t i = 0; i<10000; i++)
	{
		commonVariable = commonVariable + 1;

		OSTimeDlyHMSM(0, 0, 0, 3 );
	}
	
	OSTaskDel(OS_PRIO_SELF);
	

}


static void App_Task4(void *p_arg)
{
	// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	

	for(uint32_t i = 0; i<10000; i++)
	{
		commonVariable = commonVariable + 1;

		OSTimeDlyHMSM(0, 0, 0, 1 );
	}
	
	OSTaskDel(OS_PRIO_SELF);
	

}





static void App_TaskUartListener(void *p_arg)
{
		// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	
	
	INT8U err;
	
	while(1)
	{
		for(uint32_t i = 0; i<10000; i++)
		{

			commonVariable = commonVariable + 1;
			
			// ждем пока придет байт
			OSSemPend(Uart1_IRQSem, 0, &err);
			//OSTimeDlyHMSM(0, 0, 0, 3 );
		}
		
		OSTaskDel(OS_PRIO_SELF);
				
	}
	
}




int main(void)
{
	
	// запрет прерываний и инициализация ОС 
	BSP_IntDisAll();                                         
	OSInit();                                                


	// создание стартового таска - в нем создаются все остальные
	
    OSTaskCreate((void (*)(void *)) App_TaskStart,  // указатель на таск         
                 (void          * ) 0,              // параметр вызова (без параметра)
                 (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1], // указатель на стек
                 (INT8U           ) APP_TASK_START_PRIO // приоритет
                );

	// запуск многозадачности
    OSStart();                                               

	
	return 0;
}

