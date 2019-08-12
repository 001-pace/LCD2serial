#include "lcd1602.h"
/*
GPIO_TypeDef* LCD1602_GPIO[LCDn] =
{
  D0_GPIO, 
  D1_GPIO, 
  D2_GPIO, 
  D3_GPIO, 
  D4_GPIO, 
  D5_GPIO, 
  D6_GPIO, 
  D7_GPIO, 
  EN_GPIO, 
  RW_GPIO, 
  RS_GPIO
};

const uint8_t LCD1602_PIN[LCDn] =
{
  D0_PIN,
  D1_PIN,
  D2_PIN,
  D3_PIN,
  D4_PIN,
  D5_PIN,
  D6_PIN,
  D7_PIN,
  EN_PIN,
  RW_PIN,
  RS_PIN
};
*/
static uint8_t backligh_statu = LCD_BACKLIGHT;

void isBacklight(uint8_t mode)
{
  if (mode == 0) { //off
    i2c_w_reg(LCD_NOBACKLIGHT);
    backligh_statu = LCD_NOBACKLIGHT;
  } else { //on
    i2c_w_reg(LCD_BACKLIGHT);
    backligh_statu = LCD_BACKLIGHT;
  }
}

void lcd_write_cmd(uint8_t data)
{
  lcd_write4bit(data, 0);
  delay_us(50);
}

void lcd_write_data(uint8_t data)
{
  lcd_write4bit(data, Rs);
}

void write_data_with_en(uint8_t idata)
{
  idata |= En;
  i2c_w_reg(idata);
  delay_us(50);
  idata &= ~En;
  i2c_w_reg(idata);
  delay_us(50);
}

static void lcd_write4bit(uint8_t fdata, int mode)
{
  uint8_t hdata, ldata;
  
  hdata = fdata & 0xf0;
  ldata = (fdata << 4) & 0xf0;

  hdata |= backligh_statu;
  ldata |= backligh_statu;

  hdata |= mode;
  ldata |= mode;

  write_data_with_en(hdata);
  write_data_with_en(ldata);
}
/*

static void set_data_gpio(uint8_t dat, int num)
{
  
  if(dat == 0 ){
     GPIO_ResetBits(LCD1602_GPIO[num], LCD1602_PIN[num]);
  }else{
     GPIO_SetBits(LCD1602_GPIO[num], LCD1602_PIN[num]);
  }
}

static void set_rs(int mode)
{
  if(mode == 0){
    GPIO_ResetBits(RS_GPIO, RS_PIN);
  }else{
    GPIO_SetBits(RS_GPIO, RS_PIN);
  }
}

static void set_en(int mode)
{
  if(mode == 0){
    GPIO_ResetBits(EN_GPIO, EN_PIN);
  }else{
    GPIO_SetBits(EN_GPIO, EN_PIN);
  }
}

static void set_rw(int mode)
{
  if(mode == 0){
    GPIO_ResetBits(RW_GPIO, RW_PIN);
  }else{
    GPIO_SetBits(RW_GPIO, RW_PIN);
  }
}
*/


void lcd1602_init(void)
{
  uint8_t buf;
  
  i2c_init();
  
   //clear
  lcd_write_cmd(LCD_CLEARDISPLAY);
  delay_ms(5);
  
  isBacklight(1);
  
  delay_ms(20);
  
  write_data_with_en(0x03 << 4);
  delay_ms(5);
  write_data_with_en(0x03 << 4);
  delay_ms(5);
  write_data_with_en(0x03 << 4);
  delay_ms(5);

  //set 4bit mode
  write_data_with_en(0x02 << 4);
  delay_ms(1);
  
  //set line
  buf = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS| LCD_FUNCTIONSET;
  lcd_write_cmd(buf);
  delay_ms(1);
  
  //
  buf = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF | LCD_DISPLAYCONTROL;
  lcd_write_cmd(buf);
  delay_ms(2);

  //sset text style
  buf = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT | LCD_ENTRYMODESET;
  lcd_write_cmd(buf);
  delay_ms(2);
  
  //set cursor zero
  buf = LCD_RETURNHOME;
  lcd_write_cmd(buf);
  delay_ms(2);
}


void show(int x, int y, char *dat, int len)
{
  int i=0;
  lcd_set_xy(x,y);    //set addr
  while (i<len) {          //write string
    lcd_write_data((uint8_t)dat[i]);
    i++;
  }
}

static void lcd_set_xy( int x, int y )
{
  int address;
  if (y == 0) {
    address = 0x80 + x;
  } else {
    address = 0xC0 + x;
  }
  lcd_write_cmd(address);
}

void customer_word(uint8_t *dat, uint8_t gaddr,int len)
{
  int i;
  lcd_write_cmd(gaddr);
  for(i=0; i<len; i++){
    lcd_write_data(dat[i]);
  }
}