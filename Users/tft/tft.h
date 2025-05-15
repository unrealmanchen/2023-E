#ifndef __tft_196_spi_240x280_H
#define __tft_196_spi_240x280_H
#include <stdint.h>
#include "gpio.h"
#include "fonts.h"

#define u8 uint8_t 	 
#define u16 uint16_t 
#define u32 unsigned int
#define ST7789_SPI hspi1                                                                                                     
#define DISPLAY_OVERTURN 0			//是否翻转显示 1：开启翻转 0：不开启
#define DISPLAY_BOTTOM_TO_TOP 0		//屏幕显示方向 1：从下往上显示 0：从上往下显示
#define DISPLAY_RIGHT_TO_LEFT 0		//屏幕显示方向 1：从右往左显示 0：从左往右显示

#if DISPLAY_OVERTURN
#define TFT_SCREEN_WIDTH 	240
#define TFT_SCREEN_HEIGHT 	240
#define TFT_Y_OFFSET 		0	//Y轴（列）的偏移地址
#define TFT_X_OFFSET 		20	//X轴（行）的偏移地址
#else
#define TFT_SCREEN_WIDTH 	240
#define TFT_SCREEN_HEIGHT 	240
#define TFT_Y_OFFSET 		0	//Y轴（列）的偏移地址
#define TFT_X_OFFSET 		0	//X轴（行）的偏移地址
#endif

#define RED   	0XF800
#define GREEN   0X07E0
#define BLUE  	0X001F
#define WHITE	0xFFFF	
#define BLACK 	0x0000     
#define YELLOW 	0xFFE0 
#define CYAN 	0x05FF	 
#define MAGENTA 0xF81F 
#define GRAY 	0x8410	   
#define ORANGE 	0xFD20 
#define PURPLE 	0x8010  
#define PINK 	0xFC18	   
#define BROWN 	0xA145
#define BGR_color BLACK

#define TFT_BACKGROUND_COLOR WHITE	//背景颜色


#define TFT_BLK PCout(13) //背光引脚
#define TFT_CS PCout(14)  //片选引脚
//-----------------TFT端口定义----------------  	

#if LOW_SPEED
#define TFT_SCLK_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET)  //CLK
#define TFT_SCLK_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET)

#define TFT_SDIN_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET)  //SDA
#define TFT_SDIN_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET)

#define TFT_RST_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)   //RST
#define TFT_RST_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET)

#define TFT_DC_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET)    //D/C
#define TFT_DC_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET)

#define TFT_BLK_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET)  //背光
#define TFT_BLK_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET)

#define TFT_CS_Clr()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET)   //CS    片选线
#define TFT_CS_Set()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET)
#else

#define TFT_SCLK_Clr() GPIOA->BSRR = (uint32_t)GPIO_PIN_5 << 16u //CLK
#define TFT_SCLK_Set() GPIOA->BSRR = GPIO_PIN_5

#define TFT_SDIN_Clr() GPIOA->BSRR = (uint32_t)GPIO_PIN_7 << 16u   //SDA
#define TFT_SDIN_Set() GPIOA->BSRR = GPIO_PIN_7

#define TFT_RST_Clr() GPIOA->BSRR = (uint32_t)GPIO_PIN_6 << 16u    //RST
#define TFT_RST_Set() GPIOA->BSRR = GPIO_PIN_6

#define TFT_DC_Clr() GPIOA->BSRR = (uint32_t)GPIO_PIN_4 << 16u   //D/C
#define TFT_DC_Set() GPIOA->BSRR = GPIO_PIN_4

#define TFT_BLK_Clr() GPIOA->BSRR = (uint32_t)GPIO_PIN_8 << 16u //背光
#define TFT_BLK_Set() GPIOA->BSRR = GPIO_PIN_8

#define TFT_CS_Clr()  GPIOA->BSRR = (uint32_t)GPIO_PIN_9 << 16u   //CS    片选线
#define TFT_CS_Set()  GPIOA->BSRR = GPIO_PIN_9
#endif

void tft_init();
void open_backlight();
void close_backlight();
void TFT_full_color(u16 color);
void TFT_display_image(const uint16_t *address, uint16_t startX, uint16_t start_y);
void TFT_display_char16_16(const uint8_t *address ,uint16_t startX,uint16_t startY,
							uint16_t textColor, uint16_t backgroundColor);
void TFT_display_char16_16_noBackColor(const uint8_t *address ,uint16_t startX,uint16_t startY,uint16_t color);
void TFT_display_point(uint16_t startX, uint16_t startY, uint16_t color);
void TFT_display_en(const uint8_t *address ,const uint8_t char_en ,uint16_t startX,uint16_t startY,uint16_t textColor, 
					uint16_t backgroundColor);
void TFT_display_en_string(const uint8_t *address ,const uint8_t *str ,uint16_t startX,uint16_t startY,uint16_t textColor, 
					uint16_t backgroundColor);
int Screen_show_char(char letter, u16 start_pos_x, u16 start_pos_y, FontDef font, u16 color,int);
void Screen_show_string(char *str, u16 start_pos_x, u16 start_pos_y, FontDef font, u16 color,int);
#endif
