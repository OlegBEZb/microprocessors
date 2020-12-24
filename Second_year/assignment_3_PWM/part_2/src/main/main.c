#include "mcu_support_package/inc/stm32f10x.h"
#include <math.h>

#define TIME_INCREASE_FREQUENCY 160000.0
#define PI 3.14159265
#define PWM_FREQUENCY 180000

/*frequences are multiplied by 100 to reduce the execution time of calculating.
To see the explanation of this move go to the while cycle in main function*/
#define C6_FREQUENCY 104600
#define E6_FREQUENCY 131800
#define G6_FREQUENCY 156800


uint16_t sineC6_duty, sineE6_duty, sineG6_duty;

void TIM2_IRQHandler(void);

float  time=0;//может переполниться

int main(void)
{
	//Connecting speaker to the PC.6 pin
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//power on C port
  GPIO_InitTypeDef led;
  led.GPIO_Mode=GPIO_Mode_AF_PP;
  led.GPIO_Pin=GPIO_Pin_6;
  led.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &led);//Initialization of PC.6 with LOW voltage
  
  
  //configure pwm on TIM3 with a given frequency and with 100 setps
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseInitTypeDef pwmTimer;
  TIM_TimeBaseStructInit(&pwmTimer);//set values by default
  pwmTimer.TIM_Prescaler=4-1;
  pwmTimer.TIM_CounterMode=TIM_CounterMode_Down;
  pwmTimer.TIM_Period=100-1;
  TIM_TimeBaseInit(TIM3, &pwmTimer);//initialize timer
  
	//configure time channel for pwm 
  TIM_OCInitTypeDef timerChannel;
  TIM_OCStructInit(&timerChannel);
  timerChannel.TIM_OCMode=TIM_OCMode_PWM1;
  timerChannel.TIM_OutputState=TIM_OutputState_Enable;
  TIM_OC1Init(TIM3, &timerChannel);
  
  TIM_Cmd(TIM3, ENABLE);//runs timer
  
  
  //configure TIM2 for sin
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//set clock on timer
  TIM_TimeBaseInitTypeDef AZAZA;//sin timer
  TIM_TimeBaseStructInit(&AZAZA);//set values by default
  AZAZA.TIM_Prescaler=50-1;//sets frequency of 1.6MHz
  AZAZA.TIM_CounterMode=TIM_CounterMode_Down;
  AZAZA.TIM_Period=10-1;//1.6MHz 10 times slower=160KHz
  TIM_TimeBaseInit(TIM2, &AZAZA);//initialize timer
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//enables overflow interrupt
  __disable_irq();
  NVIC_EnableIRQ(TIM2_IRQn);//enables interrups from TIM2
  TIM_Cmd(TIM2, ENABLE);//runs timer
  
  
  __enable_irq();
  
  float sinTab[100];
	for(int i=0;i<100;i++)
	{
		sinTab[i]=sin(2*PI/100*i);
	}
	

  while(1)
  {
    static float duty_rawC6,
								 duty_rawE6,
                 duty_rawG6,
								 duty;
		
		
		/*
		id of the desired sine gets by this way: we multiply the frequence by 100 to
		get two digits after the dot (two digits because we divided sine on 100 parts).
		Then the number is converted to int type to allow % operation. By this operation
		we can get the only two digits that we need
		*/
		
		duty_rawC6=sinTab[(int)(C6_FREQUENCY*time)%100];
		duty_rawE6=sinTab[(int)(E6_FREQUENCY*time)%100];
		duty_rawG6=sinTab[(int)(G6_FREQUENCY*time)%100];
		
    sineC6_duty=(duty_rawC6+1)*50;//make sine from 0 to 100%
		sineE6_duty=(duty_rawE6+1)*50;
		sineG6_duty=(duty_rawG6+1)*50;
		
		duty=(sineC6_duty+sineE6_duty+sineG6_duty)/3;
		TIM_SetCompare1(TIM3, duty);
  }
  
  return 0;
}


// классический ассерт для STM32
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t * file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
     
    (void)file;
    (void)line;

    __disable_irq();
    while(1)
    {
        // это ассемблерная инструкция "отладчик, стой тут"
        // если вы попали сюда, значит вы ошиблись в параметрах. Смотрите в call stack
        __BKPT(0xAB);
    }
}
#endif


void TIM2_IRQHandler()
{
    time+=1.0/144000;//increments on period of sin
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);//clears overflow flag
}
