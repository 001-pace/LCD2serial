#include "stm8l15x.h"
#include "delay.h"
#include "i2c.h"

/**
*  stm8 | lcd1602
   PB0  - D0    
    ~
   PB7 - D7
   PC0 - E
   PC1 - RW
   PC4 - RS
*//*
#define D0_GPIO GPIOB
#define D0_PIN GPIO_Pin_0

#define D1_GPIO GPIOB
#define D1_PIN GPIO_Pin_1

#define D2_GPIO GPIOB
#define D2_PIN GPIO_Pin_2

#define D3_GPIO GPIOB 
#define D3_PIN GPIO_Pin_3

#define D4_GPIO GPIOB 
#define D4_PIN GPIO_Pin_4

#define D5_GPIO GPIOB 
#define D5_PIN GPIO_Pin_5

#define D6_GPIO GPIOB 
#define D6_PIN GPIO_Pin_6

#define D7_GPIO GPIOB 
#define D7_PIN GPIO_Pin_7

#define EN_GPIO GPIOC 
#define EN_PIN GPIO_Pin_0

#define RW_GPIO GPIOC 
#define RW_PIN GPIO_Pin_1

#define RS_GPIO GPIOC
#define RS_PIN GPIO_Pin_4
*/
#define LOW     0
#define HIGH    1

#define Rs 0x1  // Register select bit
#define Rw 0x2  // Read/Write bit
#define En 0x4  // Enable bit

// flags for backlight control
#define LCD_BACKLIGHT 0x01<<3
#define LCD_NOBACKLIGHT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

//#define LCDn                    11


//public-----
void lcd1602_init(void);
void show(int x, int y, uint8_t *dat, int len);
void customer_word(uint8_t *dat, uint8_t gaddr,int len);

//private-----
static void lcd_write4bit(uint8_t fdata, int mode);
static void set_data_gpio(uint8_t dat, int num);
static void set_rs(int mode);
static void set_rw(int mode);
static void set_en(int mode);
static void lcd_set_xy( int x, int y );

