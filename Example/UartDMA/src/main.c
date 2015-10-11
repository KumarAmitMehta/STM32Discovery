/**
  ******************************************************************************
  * @file    src/main.c
  * @brief   DMA enanbled USART reception
  ******************************************************************************
  *  
  */
  
/* Includes ------------------------------------------------------------------*/

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include "stm32f0xx.h"

/* Private variables ---------------------------------------------------------*/
#define DMA_MAX_LEN    4
static __IO uint8_t buff[DMA_MAX_LEN];

static void Configure_GPIOC(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* configure pins */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void Configure_GPIOA(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* configure pins */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
}

static void USART1_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  /* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);

  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
}

static void DMA1_Configure(void)
{
  DMA_InitTypeDef DMA_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Reset state */
  DMA_DeInit(DMA1_Channel3);

  /* Configure Channel Parameters */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->RDR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&buff;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = DMA_MAX_LEN;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   
  /* Initialize the DMA Channel */
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);

  /* Enable the USART's DMA receive request interface */
  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

  /* Enable DMA Stream Transfer Complete interrupt */
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

  DMA_Cmd(DMA1_Channel3, ENABLE);
}

void DMA1_Channel2_3_IRQHandler(void) 
{
  /* DMA Transfer Complete */
  if (DMA_GetITStatus(DMA1_IT_TC3))
  {
    /* Toggle LEDs */
    GPIOC->ODR ^= GPIO_ODR_9;
    GPIOC->ODR ^= GPIO_ODR_8;

    /* Clear interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_IT_TC3);
  }
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&NVIC_InitStructure);
}

int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);

  /* Initialize the System Timer and its interrupt, and start the
   * System Tick Timer. Refer SysTick_Handler in startup_stm32f072.s
   * and stm32f0xx_it.c
   */
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

  NVIC_Configuration();

  /* Configure GPIOC port and pins: Green and Blue LED On evaluation board */
  Configure_GPIOC();

  /* Configure GPIOA port and pins: PA.9 and PA.1O as USART */
  Configure_GPIOA();
 
  /* Configure USART1 */
  USART1_Configuration();
  DMA1_Configure();

  while (1)
  {
    __WFI();

  }

}

/**
  * @}
  */ 


///*****************************END OF FILE************************/
