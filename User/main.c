/**
  ******************************************************************************
  * @file    user/main.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-August-2019
  * @brief   Main program body
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
RTC_InitTypeDef   RTC_InitStr;
RTC_TimeTypeDef   RTC_TimeStr;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void uart_sent(uint8_t *data, int len);
void IWDG_Config(void);
void time_get(uint8_t *dhours, uint8_t *dminutes, uint8_t *dseconds);
void Time_Init(void);
void LSI_StabTime(void);
void time_clear(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  int i=0;
  uint8_t sum_check;
  char str[UART_RX_SIZE-3];
  uint8_t upload_str[8]={0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x04, 0x00}; // char "¡ü"
  uint8_t download_str[8]={0x04, 0x04, 0x04, 0x04,0x15, 0x0E, 0x04, 0x00}; //char "¡ý"
  uint8_t thours,tminutes,tseconds;
  uint8_t timeout_minutes = 5;  //set time out 5minutes.
  uint8_t show_data_flag = 0;
  uint8_t timeout_minutes_conut = 0;
  
  /* Enable LSE */
  CLK_LSICmd(ENABLE);
  
  /* Wait for LSE clock to be ready */
  while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);
  
  /* wait for 1 second for the LSE Stabilisation */
  LSI_StabTime();
  
  /* Select LSI (38KHz) as RTC clock source */
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);

  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
  
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
  
  Time_Init();
  
  //welcome words
  sprintf(str, " Welcome to the ");
  show(0, 0, str, 16);
  sprintf(str, " Serial2Lcd1602 ");
  show(0, 1, str, 16);
      
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
       
       //clear timeout flag
       timeout_minutes_conut = 0;
       //clear
       rxcount = 0;
     }
     
     //Time every minute
     time_get(&thours,&tminutes,&tseconds);
     if((tseconds == 59)&& (show_data_flag == 1)){
       show_data_flag = 0;
       timeout_minutes_conut++;
     }else if(tseconds != 59){
       show_data_flag = 1;
     }
     
     //check timeout
     if(timeout_minutes_conut>=timeout_minutes){
      sprintf(str, "  No Available  ");
      show(0, 0, str, 16);
      sprintf(str, " Timeout %d min! ", timeout_minutes_conut);
      show(0, 1, str, 16);
      timeout_minutes_conut = 0;
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

/**
  * @brief  Wait 1 sec for LSI stabilization .
  * @param  None.
  * @retval None.
  * Note : TIM4 is configured for a system clock = 2MHz
  */
void LSI_StabTime(void)
{

  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);

  /* Configure TIM4 to generate an update event each 1 s */
  TIM4_TimeBaseInit(TIM4_Prescaler_16384, 123);
  /* Clear update flag */
  TIM4_ClearFlag(TIM4_FLAG_Update);

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);

  /* Wait 1 sec */
  while ( TIM4_GetFlagStatus(TIM4_FLAG_Update) == RESET );

  TIM4_ClearFlag(TIM4_FLAG_Update);

  /* Disable TIM4 */
  TIM4_Cmd(DISABLE);

  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE);
}

/**
  * @brief  Time Configuration.
  * @param  None
  * @retval None
  */
void Time_Init(void)
{
  RTC_InitStr.RTC_HourFormat = RTC_HourFormat_24;
  RTC_InitStr.RTC_AsynchPrediv = 0x7F;
  RTC_InitStr.RTC_SynchPrediv = 0x00FF;
  RTC_Init(&RTC_InitStr);

  RTC_TimeStructInit(&RTC_TimeStr);
  RTC_TimeStr.RTC_Hours   = 00;
  RTC_TimeStr.RTC_Minutes = 00;
  RTC_TimeStr.RTC_Seconds = 00;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);

}

/**
  * @brief  Time clear.
  * @param  None
  * @retval None
  */
void time_clear(void)
{
  RTC_TimeStr.RTC_Hours   = 00;
  RTC_TimeStr.RTC_Minutes = 00;
  RTC_TimeStr.RTC_Seconds = 00;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
}

/**
  * @brief  Time set.
  * @param  dhours      0xff is not set
            dminutes    0xff is not set
            dseconds    0xff is not set

  * @retval None
  */
void time_set(uint8_t dhours, uint8_t dminutes, uint8_t dseconds)
{
  int ret = 0;
  if(dhours != 0xff){
    RTC_TimeStr.RTC_Hours   = dhours;
    ret++;
  }
  if(dminutes != 0xff){
    RTC_TimeStr.RTC_Minutes = dminutes;
    ret++;
  }
  if(dminutes != 0xff){
    RTC_TimeStr.RTC_Seconds = dseconds;
    ret++;
  }
  
  if(ret){
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
  }
}


/**
  * @brief  Time get.
  * @param  dhours      
            dminutes    
            dseconds    

  * @retval None
  */
void time_get(uint8_t *dhours, uint8_t *dminutes, uint8_t *dseconds)
{
  /* Wait until the calendar is synchronized */
  while (RTC_WaitForSynchro() != SUCCESS);
  /* Get the current Time*/
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);
  
  *dhours = RTC_TimeStr.RTC_Hours;
  *dminutes = RTC_TimeStr.RTC_Minutes;
  *dseconds = RTC_TimeStr.RTC_Seconds;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
