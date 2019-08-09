#include "delay.h"
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * 1->0.8us  1210->1ms
  * @retval None
  */
void delay_us(__IO uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * 1->1ms
  * @retval None
  */
void delay_ms(uint32_t nCount)
{
  while (nCount != 0){
    delay_us(1210); //1ms
    nCount--;
  }
}
