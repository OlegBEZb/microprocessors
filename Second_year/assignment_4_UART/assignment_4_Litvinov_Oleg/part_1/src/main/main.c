#include "mcu_support_package/inc/stm32f10x.h"

#define BUTTON_ON 0xFF
#define BUTTON_OFF 0x00

int main(void)
{
  //configuring button at PA.0. Input floating
  RCC-> APB2ENR|=1<<2;
  GPIOA->CRL&=~(0xF);
  GPIOA->CRL|=0x4;

  //configuring led  at PC.8. Output PP
  RCC-> APB2ENR|=(1<<4);
  GPIOC->CRH&=~(0xF);
  GPIOC->CRH|=0x3;

  //configuring PA10 as a Receiver. GP input floating
  GPIO_InitTypeDef receivePin;
  receivePin.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  receivePin.GPIO_Pin = GPIO_Pin_10;
  receivePin.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &receivePin);

  //configuring PA9 as a Transmitter. Alternate output PP
  GPIO_InitTypeDef transmitPin;
  transmitPin.GPIO_Mode = GPIO_Mode_AF_PP;
  transmitPin.GPIO_Pin = GPIO_Pin_9;
  transmitPin.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &transmitPin);

  /* USART1 configuration:
        - baudRate = 9600 bit/s
        - Word length = 8 bit
        - 1 Stop-bit
        - Without parity check
        - Flow control disabled
        - Reception and transmission enabled
                */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  USART_InitTypeDef usartStruct;
  usartStruct.USART_BaudRate = 9600;
  usartStruct.USART_WordLength = USART_WordLength_8b;
  usartStruct.USART_StopBits = USART_StopBits_1;
  usartStruct.USART_Parity = USART_Parity_No;
  usartStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usartStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &usartStruct);

  //Receive Data register not empty interrupt
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  //Start usart
  USART_Cmd(USART1, ENABLE);

  //Enable usart interrupt
  NVIC_EnableIRQ(USART1_IRQn);


  while (1)
  {
    //If button is pressed
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==1)
    {
      // Transmit Data Register is empty
      if (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET)
      {
        //Transmission is complete
        if(USART_GetFlagStatus(USART1, USART_FLAG_TC) == SET)
        {
          //Send message that button is pressed
          USART_SendData(USART1,BUTTON_ON);
        }
      }
    }
    else
    {
      if (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET)
      {
        if(USART_GetFlagStatus(USART1, USART_FLAG_TC) == SET)
        {
          //Send message that button is not pressed
          USART_SendData(USART1,BUTTON_OFF);
        }
      }
    }
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
    // если вы попали сюда, значит вы ошиблись в параметрах, когда вызывали библиотечную функцию
    // cмотрите в call stack
    __BKPT(0xAB);
  }
}
#endif

void USART1_IRQHandler()
{
  // Receive Data register is not empty
  if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  {
    uint8_t ReceieveData = USART_ReceiveData(USART1);
    if (ReceieveData == BUTTON_ON)
    {
      GPIO_SetBits(GPIOC, GPIO_Pin_8);
    }
    else
    {
      GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    }
  }
}
