
#include "urat.h"
/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter should be COM1.
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
    
    
USART_TypeDef* COM_USART[COMn] =
  {
    EVAL_COM1
  };

GPIO_TypeDef* COM_PORT[COMn] =
  {
    EVAL_COM1_GPIO
  };
const uint8_t COM_USART_CLK[COMn] =
  {
    EVAL_COM1_CLK
  };
const uint8_t COM_TX_PIN[COMn] =
  {
    EVAL_COM1_TxPin
  };
const uint8_t COM_RX_PIN[COMn] =
  {
    EVAL_COM1_RxPin
  };

void STM_EVAL_COMInit(COM_TypeDef COM, uint32_t USART_BaudRate,
                      USART_WordLength_TypeDef USART_WordLength,
                      USART_StopBits_TypeDef USART_StopBits,
                      USART_Parity_TypeDef USART_Parity,
                      USART_Mode_TypeDef USART_Mode)
{
  /* Enable USART clock */
  CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef)COM_USART_CLK[COM], ENABLE);

  /* Configure USART Tx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(COM_PORT[COM], COM_TX_PIN[COM], ENABLE);

  /* Configure USART Rx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(COM_PORT[COM], COM_RX_PIN[COM], ENABLE);

  /* USART configuration */
  USART_Init(COM_USART[COM], USART_BaudRate,
             USART_WordLength,
             USART_StopBits,
             USART_Parity,
             USART_Mode);
}
