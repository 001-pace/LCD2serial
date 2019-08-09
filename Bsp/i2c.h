#include "stm8l15x.h"
#include "delay.h"


#define LCD_ADDR                 0x27<<1
#define LCD_I2C_SPEED           100000
#define I2C_TIMEOUT         (uint32_t)0x3FFFF /*!< I2C Time out */

#define LCD_I2C                         I2C1
#define LCD_I2C_CLK                     CLK_Peripheral_I2C1
#define LCD_I2C_SCL_PIN                 GPIO_Pin_1                  /* PC.01 */
#define LCD_I2C_SCL_GPIO_PORT           GPIOC                       /* GPIOC */
#define LCD_I2C_SDA_PIN                 GPIO_Pin_0                  /* PC.00 */
#define LCD_I2C_SDA_GPIO_PORT           GPIOC                       /* GPIOC */


void i2c_init(void);
uint8_t i2c_w_reg(uint8_t reg);