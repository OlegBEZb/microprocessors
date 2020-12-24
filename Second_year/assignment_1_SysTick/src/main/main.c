#include "mcu_support_package/inc/stm32f10x.h"

//#define twinkling "this is working mode"
//#define running_fire "this is working mode"
#define hysteresis "this is working mode"

volatile int led_number=1;
volatile int led_direction=1;

volatile int led_status=0;

volatile int handler_counter=0;
void SysTick_Handler(void);

int main(void)
{
#ifdef twinkling//switches the led every 144*10^6 ticks of every 2 seconds
    __disable_irq();
    SysTick_Config(8000000);//(8*10^6)*18=(72*10^6)*2
    __enable_irq();
    RCC->APB2ENR|=1<<4;//power on port C
    GPIOC->CRH&=~(0xF);//setting last 4 bits to zero
    GPIOC->CRH|=0x3;//output push pull at 50 MHz. PC.8

#elif defined running_fire//every led lights 18*10^6 ticks or 0.25 seconds. From first to 10th and upside-down
    __disable_irq();
    SysTick_Config(1000000);//(1*10^6)*18=(72*10^6)*0.25
    __enable_irq();
    RCC-> APB2ENR|=1<<2;//power on the A port
    GPIOA->CRL&=~(0xFFFFFFF0);//setting 4-31 bits to zero
    GPIOA->CRL|=0x33333330;//output push pull at 50 MHz at PA.1-7
    GPIOA->CRH&=~(0xFFF);//0-11 bits sets to zero
    GPIOA->CRH|=0x333;//output push pull at 50 MHz at PA.8-10

#elif defined hysteresis//turns on the led if the button is pressed for 2 sec in a row and turns off if it is now pressed for 3 seconds in a row
    RCC-> APB2ENR|=1<<2;//power on the A port
    GPIOA->CRL&=~(0xF);//setting last 4 bits to zero
    GPIOA->CRL|=0x4;//input floating PA.0
    RCC-> APB2ENR|=(1<<4);//power on the C port
    GPIOC->CRH&=~(0xF);//setting last 4 bits to zero
    GPIOC->CRH|=0x3;//output push pull at 50 MHz. PC.8
#endif

    while(1)
    {
#ifdef hysteresis
        if (GPIOA->IDR & 1)//0th pin. Button is pressed
        {
            if(led_status==0)//is the led off or in the procces to become off?
            {
                led_status=1;
                handler_counter=0;//resets timer
                __disable_irq();
                SysTick_Config(8000000);//(8*10^6)*18=(72*10^6)*2
                __enable_irq();
            }
        }
        else//not pressed
        {
            if(led_status)//is the led on or in the process to become on?
            {
                led_status=0;
                handler_counter=0;//resets timer
                __disable_irq();
                SysTick_Config(12000000);//(12*10^6)*18=(72*10^6)*3
                __enable_irq();
            }
        }
#endif
    }
    return 0;
}
void SysTick_Handler(void)
{
    handler_counter++;
    if(handler_counter==18)
    {
#ifdef twinkling
        GPIOC->ODR^=1<<8;//status inversion of the 8th bit

#elif defined running_fire
        GPIOA->ODR&=~(0x7FE);//turn off all the LEDs, i.e. 1-10 pins
        GPIOA->ODR|=(1<<led_number);//turns on the "current" led
        led_number+=led_direction;
        if(led_number==1)
        {
            led_direction=1;//change direction to upstairs
        }
        if(led_number==10)
        {
            led_direction=-1;//change direction to downstairs
        }

#elif defined hysteresis
        if(led_status)
        {
            GPIOC->ODR|=1<<8;//8th pin HIGH
        }
        else
        {
            GPIOC->ODR&=~(1<<8);//8th pin LOW
        }
#endif
        handler_counter=0;
    }
}
