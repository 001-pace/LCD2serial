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
#include "stdio.h"
#include "delay.h"
#include "urat.h"
#include "lcd1602.h"
#include "stm8l15x_iwdg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UART_RX_SIZE   35
#define UART_TX_SIZE   35
#define RELOAD_VALUE   254
#define LSI_PERIOD_NUMBERS         10
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t tx_size = 0;
uint8_t rx_size = 0;
uint8_t txcount = 0; 
uint8_t rxcount = 0; 
uint8_t rxbuffer[UART_RX_SIZE] = {0};
uint8_t txbuffer[UART_TX_SIZE] = {0};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void uart_sent(uint8_t *data, int len);
void IWDG_Config(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  int i=0;
  uint8_t sum_check;
  uint8_t str[UART_RX_SIZE-3];
  uint8_t upload_str[8]={0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x04, 0x00}; // char "��"
  uint8_t download_str[8]={0x04, 0x04, 0x04, 0x04,0x15, 0x0E, 0x04, 0x00}; //char "��"
  /*High speed internal clock prescaler: 1*/
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
   /* EVAL COM (USARTx) configuration -----------------------------------------*/
  /* USART configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - USART Clock disabled
  */
  STM_EVAL_COMInit(COM1, (uint32_t)115200, USART_WordLength_8b, USART_StopBits_1,
                   USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
  
   /* Enable general interrupts */
  enableInterrupts();

  /* Enable USART */
  USART_Cmd(EVAL_COM1, ENABLE);
  
  lcd1602_init();
  
  customer_word(upload_str, 0x40, 8); // upload_str set code 0x00
  customer_word(download_str, 0x48, 8); // download_str set code 0x01
  
  rx_size = UART_RX_SIZE;
   
  /* Check if the MCU has resumed from IWDG reset */
  if (RST_GetFlagStatus(RST_FLAG_IWDGF) != RESET)
  {
    /* Clear IWDG Flag */
    RST_ClearFlag(RST_FLAG_IWDGF);
  }

  /* IWDG Configuration */
  IWDG_Config();
  
  while (1)
  {
     // Enable serial rx interrupt event
     USART_ITConfig(EVAL_COM1, USART_IT_RXNE, ENABLE);
     
     //Check header and data length
     // data format: header(1byte) + first line data(16byte) + 
     // Second line data (16byte)+ data lines(1byte) + sum number(1byte)
     if(rxcount == UART_RX_SIZE && rxbuffer[0] == 0xff){
       sum_check = 0;
       for(i=1;i<(rxcount-1);i++){
        sum_check += rxbuffer[i];
        
        if(rxbuffer[i] == 0xff){
          str[i-1] = 0x00;
        }else if(rxbuffer[i] == 0xfe){
          str[i-1] = 0x01;
        }else{
          str[i-1] = rxbuffer[i];
        }
       }
       if(sum_check == rxbuffer[UART_RX_SIZE-1]){       // check ok
        sum_check = 1;
        //show first line str.
        show(0, 0, str, 16);
        
        if(rxbuffer[UART_RX_SIZE-2] == 1){
          //show second line str.
          show(0, 1, &str[16], 16);
        }
          
       }else{   //check fail
          sum_check = 0;
       }
       //reply result: 0 is fail, 1 is ok
       uart_sent(&sum_check, 1);
       //clear
       rxcount = 0;
     }
      //set timer dog, keep system fine.
      IWDG_ReloadCounter();  
  }
}

void uart_sent(uint8_t *data, int len)
{
  int i;
  
    for(i=0U; i<len; i++){
      txbuffer[i] = data[i];
    }
    tx_size = len;
    
    USART_ITConfig(EVAL_COM1, USART_IT_TC, ENABLE);
}

/**
  * @brief  Configures the IWDG to generate a Reset if it is not refreshed at the
  *         correct time. 
  * @param  None
  * @retval None
  */
void IWDG_Config(void)
{
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable(); 
  
  /* IWDG timeout equal to 214 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  /* IWDG configuration: IWDG is clocked by LSI = 38KHz */
  IWDG_SetPrescaler(IWDG_Prescaler_32);
  
  /* IWDG timeout equal to 214.7 ms (the timeout may varies due to LSI frequency dispersion) */
  /* IWDG timeout = (RELOAD_VALUE + 1) * Prescaler / LSI 
                  = (254 + 1) * 32 / 38 000 
                  = 214.7 ms */
  IWDG_SetReload((uint8_t)RELOAD_VALUE);
  
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
