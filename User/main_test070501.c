/**
  ******************************************************************************
  * @file    GPIO/GPIO_Toggle/main.c
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    30-September-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"


/**
  * 0 default do noting
  * 1 D mode 1
  * 2 D mode 2
  * 3 D mode off
  * 4 A/B mode off
  */
uint8_t int_flag = 0;

#define LED_PORT GPIOA
#define LED_PIN GPIO_Pin_2

#define MODE_PORT GPIOA
#define MODE_PIN GPIO_Pin_3

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t start_flag = 0;
static uint8_t work_mode = 0;
/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint16_t nCount);
static void Delay_ms(uint32_t nCount);
static void led_spark(uint8_t mode);
static void mute_selct(uint8_t mode);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  /* Initialize SYS STATUS */
  GPIO_Init(LED_PORT, LED_PIN, GPIO_Mode_Out_PP_Low_Fast);
  /* int mute select */
  GPIO_Init(MODE_PORT, MODE_PIN, GPIO_Mode_Out_PP_Low_Fast);
  
  /* int mute select key input*/
  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_In_PU_IT);
  EXTI_SetPinSensitivity(EXTI_Pin_4, EXTI_Trigger_Falling);
  
  /* int go sleep key input*/
  GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_IT);
  EXTI_SetPinSensitivity(EXTI_Pin_0, EXTI_Trigger_Falling);
  
  enableInterrupts();   //enable interrupt
  
  GPIO_ResetBits(LED_PORT, LED_PIN);
  GPIO_ResetBits(MODE_PORT, MODE_PIN);
  
  //work_mode =0;
  Delay_ms(500);//waitting
  while (1)
  {
    if(int_flag ==1){
      Delay_ms(200);
      work_mode++;
      if(work_mode >= 5){
        work_mode=0;
      }
      int_flag = 0;
      start_flag = 0;
    }
    
    if( start_flag == 0){
       mute_selct(work_mode);
       led_spark(work_mode);
       start_flag = 1;
    }
    
    if(int_flag == 99){
      //set mute
      GPIO_ResetBits(MODE_PORT, MODE_PIN);
      //off led
      GPIO_SetBits(LED_PORT, LED_PIN);
      int_flag = 0;
      halt();
    }
    
    Delay_ms(100);
  }
}

static void mute_selct(uint8_t mode)
{
  int i;
  GPIO_ResetBits(MODE_PORT, MODE_PIN);
  Delay_ms(12);
  GPIO_SetBits(MODE_PORT, MODE_PIN);
  Delay_ms(20);
  
  for(i=1; i<=mode; i++){
    GPIO_ResetBits(MODE_PORT, MODE_PIN);
    Delay(8);
    GPIO_SetBits(MODE_PORT, MODE_PIN);
    Delay(8);
  }
}

static void led_spark(uint8_t mode)
{
  int i;
  
  for(i=0; i<=mode; i++){
    Delay_ms(800);
    GPIO_SetBits(LED_PORT, LED_PIN);
    Delay_ms(800);
    GPIO_ResetBits(LED_PORT, LED_PIN);
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * 1->6us  166->1ms
  * @retval None
  */
static void Delay(__IO uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

static void Delay_ms(uint32_t nCount)
{
  while (nCount != 0){
    Delay(150); //1ms
    nCount--;
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
