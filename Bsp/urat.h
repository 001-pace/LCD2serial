#include "stm8l15x.h"


/** @addtogroup STM8L1526_EVAL_LOW_LEVEL_COM
  * @{
  */
#define COMn                        1

typedef enum
{
  COM1 = 0
} COM_TypeDef;

/**
 * @brief Definition for COM port1
 */
#define EVAL_COM1                   USART1
#define EVAL_COM1_GPIO              GPIOC
#define EVAL_COM1_CLK               CLK_Peripheral_USART1
#define EVAL_COM1_RxPin             GPIO_Pin_6
#define EVAL_COM1_TxPin             GPIO_Pin_5

void STM_EVAL_COMInit(COM_TypeDef COM, uint32_t USART_BaudRate, USART_WordLength_TypeDef USART_WordLength,\
                      USART_StopBits_TypeDef USART_StopBits,\
                      USART_Parity_TypeDef USART_Parity,\
                      USART_Mode_TypeDef USART_Mode);