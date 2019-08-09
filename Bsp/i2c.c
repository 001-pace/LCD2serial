#include "i2c.h"

uint8_t i2c_w_reg(uint8_t reg)
{
  uint8_t ret =0;
  
  /* Send LCD_I2C START condition */
  I2C_GenerateSTART(LCD_I2C, ENABLE);
  
   /* Test on LCD_I2C EV5 and clear it */
  while (!I2C_CheckEvent(LCD_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
    ret++;
    delay_us(5);
    if(ret>100){
      return ret;
    }
  }
  ret=0;
  
  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(LCD_I2C, LCD_ADDR, I2C_Direction_Transmitter);
  
  /* Test on LCD_I2C EV6 and clear it */
  while (!I2C_CheckEvent(LCD_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
 {
    ret++;
    delay_us(5);
    if(ret>100){
      return ret;
    }
  }  
  ret=0;
  
   /* Send the specified register data pointer */
  I2C_SendData(LCD_I2C, reg);
  
  /* Test on LCD_I2C EV8 and clear it */
  while (!I2C_CheckEvent(LCD_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
 {
    ret++;
    delay_us(10);
    if(ret>100){
      return ret;
    }
  }
  ret=0;

  I2C_GenerateSTOP(LCD_I2C, ENABLE);
  

  return ret;
}


void i2c_init(void)
{
   /*!< Disable LCD_I2C */
  //I2C_Cmd(LCD_I2C, DISABLE);
  /*!< DeInitializes the LCD_I2C */
  //I2C_DeInit(LCD_I2C);

  /*!< LCD_I2C Periph clock disable */
  //CLK_PeripheralClockConfig(LCD_I2C_CLK, DISABLE);

  /*!< Configure LCD_I2C pins: SCL */
  GPIO_Init(LCD_I2C_SCL_GPIO_PORT, LCD_I2C_SCL_PIN, GPIO_Mode_In_PU_No_IT);

  /*!< Configure LCD_I2C pins: SDA */
  GPIO_Init(LCD_I2C_SDA_GPIO_PORT, LCD_I2C_SDA_PIN, GPIO_Mode_In_PU_No_IT);


   /*!< LCD_I2C Periph clock enable */
  CLK_PeripheralClockConfig(LCD_I2C_CLK, ENABLE);


  /* I2C configuration */
  I2C_Init(LCD_I2C, LCD_I2C_SPEED, 0x00, I2C_Mode_I2C,
           I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);

  /*!< Enable SMBus Alert interrupt */
  //I2C_ITConfig(LCD_I2C, I2C_IT_ERR, ENABLE);

  /*!< LCD_I2C Init */
  I2C_Cmd(LCD_I2C, ENABLE);
}
