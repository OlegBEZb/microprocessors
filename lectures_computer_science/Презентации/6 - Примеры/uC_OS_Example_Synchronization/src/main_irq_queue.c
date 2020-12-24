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

#define UART1_QUEUE_SIZE   100

// это очередь сообщений для уарта
OS_EVENT * Uart1_Queue; 

// собственно, сама очередь - это просто массив
void * Uart1_QueueStorage[ UART1_QUEUE_SIZE ];


// этот семафор свободен, если сейчас никакое сообщение не отправляется
OS_EVENT * Uart1_IRQSem;


/***************************************************************************************************
                                       Стеки для тасков
***************************************************************************************************/

static  OS_STK         App_TaskStartStk [ APP_TASK_START_STK_SIZE ];

static  OS_STK         App_TaskLed1Stk [ APP_TASK_LED1_STK_SIZE ];

static  OS_STK         App_TaskLed2Stk [ APP_TASK_LED2_STK_SIZE ];

static  OS_STK         App_TaskUartPhraseStk [ APP_TASK_UART_PHRASE_STK_SIZE ];

static  OS_STK         App_TaskButtonStatusMonitorStk [ APP_TASK_BUTTON_STATUS_MONITOR_STK_SIZE ];

static  OS_STK         App_TaskUartTransmitterStk [ APP_TASK_UART_TRANSMITTER_STK_SIZE ];


/***************************************************************************************************
                                       Таски - объявления
***************************************************************************************************/

static void App_TaskStart (void *p_arg);

static void App_TaskLed1 (void *p_arg);

static void App_TaskLed2 (void *p_arg);

static void App_TaskUartPhrase (void *p_arg);

static void App_TaskButtonStatusMonitor(void *p_arg);


static void App_TaskUartTransmitter(void *p_arg);

static void App_TaskUartTransmitter(void *p_arg);


/***************************************************************************************************
                                           Локальные функции
***************************************************************************************************/


static void addMessageToTransmitter(Message * msg)
{
	OSQPost(Uart1_Queue, (void *)msg );
}



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

		
		// проинициализируем примитивы синхронизации
		
		// создаем семафор, принимающий значения 1 или 0
		Uart1_IRQSem = OSSemCreate(1);
		
		// создаем очередь
		Uart1_Queue = OSQCreate( Uart1_QueueStorage, UART1_QUEUE_SIZE );
		
		
		// проинициализируем прерывания
		
		// проставляем заглушки
		BSP_IntInit();
	
		BSP_IntVectSet(BSP_INT_ID_USART1, Uart1_Irq_handler);
		
		NVIC_EnableIRQ(USART1_IRQn);




	// дальше создаются пользовательские таски
	
	
	OSTaskCreate((void (*)(void *)) App_TaskLed1,           
			 (void          * ) 0,              
			 (OS_STK        * )&App_TaskLed1Stk[APP_TASK_LED1_STK_SIZE - 1], 
			 (INT8U           ) APP_TASK_LED1_PRIO 
			);
			
	OSTaskCreate((void (*)(void *)) App_TaskLed2,           
		 (void          * ) 0,              
		 (OS_STK        * )&App_TaskLed2Stk[APP_TASK_LED2_STK_SIZE - 1], 
		 (INT8U           ) APP_TASK_LED2_PRIO 
		);
		
	OSTaskCreate((void (*)(void *)) App_TaskUartPhrase,           
		 (void          * ) 0,              
		 (OS_STK        * )&App_TaskUartPhraseStk[APP_TASK_UART_PHRASE_STK_SIZE - 1], 
		 (INT8U           ) APP_TASK_UART_PHRASE_PRIO 
		);
		
	OSTaskCreate((void (*)(void *)) App_TaskButtonStatusMonitor,           
		 (void          * ) 0,              
		 (OS_STK        * )&App_TaskButtonStatusMonitorStk[APP_TASK_BUTTON_STATUS_MONITOR_STK_SIZE - 1], 
		 (INT8U           ) APP_TASK_BUTTON_STATUS_MONITOR_PRIO 
		);
		
	OSTaskCreate((void (*)(void *)) App_TaskUartTransmitter,           
		 (void          * ) 0,              
		 (OS_STK        * )&App_TaskUartTransmitterStk[APP_TASK_UART_TRANSMITTER_STK_SIZE - 1], 
		 (INT8U           ) APP_TASK_UART_TRANSMITTER_PRIO 
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
		// засыпаем на 50 миллисекунд
		OSTimeDlyHMSM(0, 0, 0, 50);
	}

}


// этот таск периодически отправляет строчку текста в Uart1
static void App_TaskUartPhrase (void *p_arg)
{
	// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	
	// подготовим сообщение
	
	// заметим, что сейчас создается буфер в 256 байт на стеке потока
	// т.е. стек должен быть соответствующего размера
	
	// это называется "designated initializer" 
	Message message = {.buf = {"Vivat Academia!\nVivant professores!\n\n"}, .size = 0};
	
	// размер нужно указать
	message.size = strlen(message.buf);

	
	while(1)
	{

		// отправляем сообщение "асинхронно" - добавляя его в очередь
		addMessageToTransmitter(& message);		
		
		
		// засыпаем на 500 миллисекунд
		OSTimeDlyHMSM(0, 0, 0, 500);
		
			
	}

}

// этот таск периодически проверяет, нажата ли кнопка и сообщает ее статус по Uart
static void App_TaskButtonStatusMonitor(void *p_arg)
{
	// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	
	// подаем тактирование на кнопку
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// делаем РС.9 - выходом пуш-пулл
	GPIO_InitTypeDef gpioA;
	gpioA.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioA.GPIO_Pin = GPIO_Pin_0;
	gpioA.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_Init(GPIOA, &gpioA);
	
	// создадим сообщения - помним, что это целых два буфера в 256 байт каждый и они лежат на стеке!
	
	
	Message msgPressed = {.buf = {"Button is pressed!\n\n"}, .size = 0};
	
	// заполним размер
	msgPressed.size = strlen(msgPressed.buf);
	
	Message msgNotPressed = {.buf = {"Button is not pressed!\n\n"}, .size = 0};
	
	
	msgNotPressed.size = strlen(msgNotPressed.buf);
			
	
	while(1)
	{
	
		// теперь мы отправляем эти сообщения "асинхронно" - добавляя их в очередь
	
		if( GPIOA->IDR & 1)
			addMessageToTransmitter(& msgPressed);
		else
			addMessageToTransmitter(& msgNotPressed);
			
	
		// засыпаем
		OSTimeDlyHMSM(0, 0, 1, 100);
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


// этот таск просыпается, если что-то нужно отправить по uart
static void App_TaskUartTransmitter(void *p_arg)
{
		// это чтобы убрать warning о неиспользуемом аргументе
	(void)p_arg;
	
	Message * message;
	
	INT8U err;
	
	while(1)
	{
		// вынимаем сообщение из очереди
		message = OSQPend(Uart1_Queue, 0, &err);
		
		// теперь ждем, пока доотправиться текущее сообщение
		OSSemPend(Uart1_IRQSem, 0, &err);

		
		// дождались! отдаем прерыванию новое сообщение и включаем прерывание
		usart1_SendMessage(message);				
		
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

