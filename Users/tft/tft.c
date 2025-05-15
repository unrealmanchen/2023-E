
#include "tft.h"
#include "spi.h"
//#include "usart.h"
#include <string.h>
#include "main.h"
#define ROL 20
u16 display_buf[240*ROL];
int state_16 =0;


void SPI_SetDataSize_8bit(SPI_HandleTypeDef *hspi)
{
	hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	if (HAL_SPI_Init(hspi) != HAL_OK)
	{
		Error_Handler();
	}
}

void SPI_SetDataSize_16bit(SPI_HandleTypeDef *hspi)
{
	hspi->Init.DataSize = SPI_DATASIZE_16BIT;
	if (HAL_SPI_Init(hspi) != HAL_OK)
	{
		Error_Handler();
	}

}
#define USE_DMA
void TFT_SEND_CMD(u8 cmd)
{	
	#ifndef USE_DMA
	u8 i;			  
	TFT_DC_Clr();			
	TFT_CS_Clr();
	delay_us(2);
	for(i=0;i<8;i++)
	{			  
		TFT_SCLK_Clr();
		if(cmd&0x80)
		   TFT_SDIN_Set();
		else 
		   TFT_SDIN_Clr();
		TFT_SCLK_Set();
		cmd<<=1;   
	}				 		  
	TFT_CS_Set();
	TFT_DC_Set();   	
	#endif
	#ifdef USE_DMA
	if (state_16 == 1)
	{
		SPI_SetDataSize_8bit(&ST7789_SPI);
		state_16 = 0;
	}
	TFT_DC_Clr();
	//TFT_CS_Clr();
	delay_us(5);
	HAL_SPI_Transmit(&ST7789_SPI, &cmd, 1, HAL_MAX_DELAY);
	#endif
} 
/*************************************
*@brief TFT_SEND_DATA
*@details 写入数据
*@param[in] data 8位数据
*@return void
*@author zx
*@date 2022-12-18
***************************************/
void TFT_SEND_DATA(u8 data)
{	
	#ifndef USE_DMA
	u8 i;			  
	TFT_DC_Set();			
	TFT_CS_Clr();
	delay_us(2);
	for(i=0;i<8;i++)
	{			  
		TFT_SCLK_Clr();
		if(data&0x80)
		   TFT_SDIN_Set();
		else 
		   TFT_SDIN_Clr();

		TFT_SCLK_Set();

		data<<=1;   
	}				 		  
	TFT_CS_Set();
//	TFT_DC_Set();   	
	#endif
	#ifdef USE_DMA
	if (state_16 == 1){
		SPI_SetDataSize_8bit(&ST7789_SPI);
		state_16=0;
	}
		
	TFT_DC_Set();
	//TFT_CS_Clr();
	delay_us(5);
	//HAL_SPI_Transmit(&ST7789_SPI,&data,1,HAL_MAX_DELAY);
	HAL_SPI_Transmit_DMA(&ST7789_SPI, &data, sizeof(data));

	//TFT_CS_Set();
	#endif
}
/*************************************
 *@brief TFT_SEND_DATA_16
 *@details 写入16位数据
 *@param[in] data 16位数据
 *@return void
 *@author LOKI_chen
 *@date 2025-2-27
 ***************************************/
void TFT_SEND_DATA_16(u16 data){
	if (state_16 == 0){
		SPI_SetDataSize_16bit(&ST7789_SPI);
		state_16=1;
	}
	TFT_DC_Set();
	// TFT_CS_Clr();
	delay_us(5);
	HAL_SPI_Transmit(&ST7789_SPI,(u8*)&data,sizeof((u8)data),HAL_MAX_DELAY);
	// Amount of data需要输入8位有效传输而不是sizeof
}

/*************************************
* @brief   LCD_SetWindows
* @details  设置LCD显示窗口,设置完成后就可以连续发
			送颜色数据了，无需再一次一次设置坐标
* @param  startX：窗口起点x轴坐标
* 		   startY：窗口起点y轴坐标
* 		   width：窗口宽度
* 		   height：窗口高度
* @return void
*************************************/
void TFT_SetWindows(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height)
{
#if TFT_X_OFFSET
	startX += TFT_X_OFFSET;
#endif
#if TFT_Y_OFFSET
	startY += TFT_Y_OFFSET;
#endif		
#define SPI16
#ifdef SPI16
	TFT_SEND_CMD(0x2A);
	TFT_SEND_DATA_16(startX);
	TFT_SEND_DATA_16(startX + width - 1);
	TFT_SEND_CMD(0x2B);
	TFT_SEND_DATA_16(startY);
	TFT_SEND_DATA_16(startY + height - 1);
	TFT_SEND_CMD(0x2C);

#endif
#ifndef SPI16
	TFT_SEND_CMD(0x2A);		//发送设置X轴坐标的命令0x2A
	//参数SC[15:0]	->	设置起始列地址，也就是设置X轴起始坐标
	TFT_SEND_DATA(startX>>8);				//先写高8位
	TFT_SEND_DATA(startX&0xFF);			//再写低8位
	//参数EC[15:0]	->	设置窗口X轴结束的列地址，因为参数usXwidth是窗口长度，所以要转为列地址再发送
	TFT_SEND_DATA((startX+width-1)>>8);				//先写高8位
	TFT_SEND_DATA((startX+width-1)&0x00FF);			//再写低8位

	TFT_SEND_CMD(0x2B);		//发送设置Y轴坐标的命令0x2B
	//参数SP[15:0]	->	设置起始行地址，也就是设置Y轴起始坐标
	TFT_SEND_DATA(startY>>8);				//先写高8位
	TFT_SEND_DATA(startY&0x00FF);			//再写低8位
	//参数EP[15:0]	->	设置窗口Y轴结束的列地址，因为参数usYheight是窗口高度，所以要转为行地址再发送
	TFT_SEND_DATA((startY+height-1)>>8);				//先写高8位
	TFT_SEND_DATA((startY+height-1)&0x00FF);			//再写低8位
	TFT_SEND_CMD(0x2C);									// 开始往GRAM里写数据
#endif
}
/*************************************
*@brief TFT_display_point
*@details 画点函数
*@param[in] startX：X起始坐标
*			startY：Y起始坐标
*			color：点的颜色
*@return void
*@author zx
*@date 2023-06-04
***************************************/
void TFT_display_point(uint16_t startX, uint16_t startY, uint16_t color)
{
	TFT_SetWindows(startX,startY, 1, 1);
	TFT_SEND_DATA_16(color);
}

/*************************************
*@brief TFT_display_image
*@details 显示图片函数,16位颜色数据先发高8位再发低八位
*@param[in] address:图片数据地址
*			startX：X起始坐标
*			startY：Y起始坐标
*@return void
*@author zx
*@date 2023-06-03
***************************************/
void TFT_display_image(const uint16_t *address, uint16_t startX, uint16_t startY)
{
	uint16_t image_width;//图片宽
	uint16_t image_hight;//图片高
	uint16_t x,y;
	image_width = address[0];
	image_hight = address[1];
	TFT_SetWindows(startX, startY, image_width, image_hight);
	for(y = 0; y < image_hight; y++)
	{
		for(x = 0; x < image_width; x++)
		{
			//发送图片数据，每次发送16位,先发高八位
			TFT_SEND_DATA(address[y*image_width + x + 2]>>8);
			TFT_SEND_DATA(address[y*image_width + x + 2]&0xff);
		}
	}	
}

/*************************************
 *@brief Screen_show_char
 *@details 显示字符
 *@param[in] letter :字符
 *@param[in] start_pos_x:X起始坐标
 *@param[in] start_pos_y:Y起始坐标
 *@param[in] font:字体
 *@param[in] color:字体颜色
 *@param[in] func:是否覆盖 1表示区域覆盖 0表示分离覆盖 区域更快
 *@return int
 *@author LOKI_chen
 *@date 2025-2-27
 ***************************************/
int Screen_show_char(char letter, u16 start_pos_x,u16 start_pos_y, FontDef font, u16 color,int func)
{
	u32 temp;
	if (TFT_SCREEN_WIDTH <= (start_pos_x + font.FontWidth) ||
		TFT_SCREEN_HEIGHT <= ( start_pos_y+ font.FontHeight))
	{
		return 1;
	}

	if(func==1){
		memset(display_buf, 0, sizeof(display_buf));
		TFT_SetWindows(start_pos_x, start_pos_y, font.FontWidth, font.FontHeight);
		int buffer_ptr = 0;
		for (int i = 0; i < font.FontHeight; i++)
		{
			temp = font.data[(letter - 32) * font.FontHeight + i];
			for (int j = 0; j < font.FontWidth; j++)
			{
				if ((temp << j) & 0x8000)
				{
					display_buf[buffer_ptr] = color;
					// TFT_SEND_DATA_16(color);
				}
				else
				{
					display_buf[buffer_ptr] = BGR_color;
					// TFT_SEND_DATA_16(BGR_color);
				}
				buffer_ptr++;
			}
		}
		if (state_16 == 0)
		{
			SPI_SetDataSize_16bit(&ST7789_SPI);
			state_16 = 1;
		}
		TFT_DC_Set();
		delay_us(5);
		
		HAL_SPI_Transmit(&ST7789_SPI, (u8 *)display_buf, font.FontWidth * font.FontHeight,1000);
		
	}else{
		for (int i = 0; i < font.FontHeight; i++)
		{
			temp = font.data[(letter - 32) * font.FontHeight + i];
			for (int j = 0; j < font.FontWidth; j++)
			{
				if ((temp << j) & 0x8000)
				{
					TFT_display_point(start_pos_x + j, start_pos_y + i,color);
				}
			}
		}
	}
	
	

	
	return 0;
}
/*************************************
 *@brief Screen_show_string
 *@details 显示字符串
 *@param[in] str 字符串指针
 *@param[in] start_pos_x:X起始坐标
 *@param[in] start_pos_y:Y起始坐标
 *@param[in] font:字体
 *@param[in] color:字体颜色
 *@return void
 *@author LOKI_chen
 *@date 2025-2-27
 ***************************************/
void Screen_show_string(char *str, u16 start_pos_x, u16 start_pos_y, FontDef font, u16 color,int func){
	while(*str!=NULL){
		if(Screen_show_char(*str,start_pos_x,start_pos_y,font,color,func)==1)
			return;
		start_pos_x += font.FontWidth;
		str++;
	}
}
/*************************************
*@brief TFT_full_color
*@details 在整个屏幕上填充颜色
*@param[in] void
*@return void
*@author zx
*@date 2022-12-18
***************************************/
void TFT_full_color(u16 color)
{
	
	TFT_SetWindows(0,0,TFT_SCREEN_WIDTH,TFT_SCREEN_HEIGHT);
	#ifdef SPI_16
	for(ROW=0;ROW<TFT_SCREEN_HEIGHT;ROW++)             //ROW loop
	{ 
		for(column=0;column<TFT_SCREEN_WIDTH;column++) //column loop
		{

			TFT_SEND_DATA(color>>8);
			TFT_SEND_DATA(color);
		}
	}
	#endif
#ifndef SPI_16
	if (state_16 == 0)
	{
		SPI_SetDataSize_16bit(&ST7789_SPI);
		state_16 = 1;
	}
	for(int i=0;i<240/ROL;i++){
		for(int j=0;j<240*ROL;j++){
			display_buf[j] = color;
		}
		
		TFT_DC_Set();
		delay_us(5);
		
		//while (HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY);
		HAL_SPI_Transmit(&ST7789_SPI, (u8*)display_buf, 240*ROL,1000);
		//while (!(DMA2->LISR & DMA_LISR_TCIF3));
	}
		
		
		//
		
#endif
}
	

/*************************************
*@brief tft_196_240x280_init
*@details tft屏幕初始化函数，主要是定义引脚和启动驱动
*@param[in] void
*@return void
*@author zx
*@date 2022-12-18
***************************************/
void tft_init()
{
	/* GPIO_InitTypeDef GPIO_InitStruture;                                  //定义一个结构体变量
 	//硬件SPI引脚初始化
	GPIO_InitTypeDef  GPIO_InitStructure;
	 */
	uint8_t lcd_data = 0x00;
	uint16_t start_x = 0;
	uint16_t end_x = TFT_SCREEN_WIDTH;
	uint16_t start_y = 0;
	uint16_t end_y = TFT_SCREEN_HEIGHT;
#if TFT_X_OFFSET
	start_x += TFT_X_OFFSET;
	end_x += TFT_X_OFFSET;
#endif	
#if TFT_Y_OFFSET
	start_y += TFT_Y_OFFSET;
	end_y += TFT_Y_OFFSET;
#endif	

	//st7789驱动
	TFT_SCLK_Set();			//特别注意！！
	TFT_RST_Clr();
	//delay_ms(1000);
	TFT_RST_Set();
	//delay_ms(1000);
    TFT_SEND_CMD(0x11); 			//Sleep Out
	//delay_ms(120);               //DELAY120ms 
	 	  //-----------------------ST7789V Frame rate setting-----------------//
//************************************************
	TFT_SEND_CMD(0x3A);        //颜色数据格式RGB565 65k真彩色
	TFT_SEND_DATA(0x05);
	TFT_SEND_CMD(0xC5); 		//VCOM1
	TFT_SEND_DATA(0x1A);
/*		
	(0,0)*********240***********->
	*
	*
	*
	280           240x280
	*
	*
	*
	↓
**/
	TFT_SEND_CMD(0x36);                 // 屏幕显示方向设置

#if DISPLAY_BOTTOM_TO_TOP
	lcd_data |= (1<<7);
#else	
	lcd_data |= (0<<7);
#endif
#if DISPLAY_RIGHT_TO_LEFT
	lcd_data |= (1<<6);
#else	
	lcd_data |= (0<<6);
#endif

#if DISPLAY_OVERTURN//不翻转显示
	lcd_data |= (1<<5);//翻转显示
#else
	lcd_data |= (0<<5);	
#endif
	TFT_SEND_DATA(lcd_data);

/*****显示位置，注意，x和y是根据屏幕显示方向来定的，不一定统一**********/
	TFT_SEND_CMD(0x2B); //设置显示区域 x轴起始及结束坐标 
	TFT_SEND_DATA((start_x>>8)&0xff);
	TFT_SEND_DATA(start_x&0xff);
	TFT_SEND_DATA((end_x>>8)&0xff);
	TFT_SEND_DATA(end_x&0xff);

	TFT_SEND_CMD(0x2B); //设置显示区域 Y轴起始及结束坐标
	TFT_SEND_DATA((start_y>>8)&0xff);
	TFT_SEND_DATA(start_y&0xff);
	TFT_SEND_DATA((end_y>>8)&0xff);
	TFT_SEND_DATA(end_y&0xff);
	
	//-------------ST7789V Frame rate setting-----------//
	TFT_SEND_CMD(0xb2);		//Porch Setting
	TFT_SEND_DATA(0x05);
	TFT_SEND_DATA(0x05);
	TFT_SEND_DATA(0x00);
	TFT_SEND_DATA(0x33);
	TFT_SEND_DATA(0x33);

	TFT_SEND_CMD(0xb7);			//Gate Control
	TFT_SEND_DATA(0x05);			//12.2v   -10.43v
	//--------------ST7789V Power setting---------------//
	TFT_SEND_CMD(0xBB);//VCOM
	TFT_SEND_DATA(0x3F);

	TFT_SEND_CMD(0xC0); //Power control
	TFT_SEND_DATA(0x2c);

	TFT_SEND_CMD(0xC2);		//VDV and VRH Command Enable
	TFT_SEND_DATA(0x01);

	TFT_SEND_CMD(0xC3);			//VRH Set
	TFT_SEND_DATA(0x0F);		//4.3+( vcom+vcom offset+vdv)

	TFT_SEND_CMD(0xC4);			//VDV Set
	TFT_SEND_DATA(0x20);				//0v

	TFT_SEND_CMD(0xC6);				//Frame Rate Control in Normal Mode
	TFT_SEND_DATA(0X01);			//111Hz

	TFT_SEND_CMD(0xd0);				//Power Control 1
	TFT_SEND_DATA(0xa4);
	TFT_SEND_DATA(0xa1);

	TFT_SEND_CMD(0xE8);				//Power Control 1
	TFT_SEND_DATA(0x03);

	TFT_SEND_CMD(0xE9);				//Equalize time control
	TFT_SEND_DATA(0x09);
	TFT_SEND_DATA(0x09);
	TFT_SEND_DATA(0x08);
	//---------------ST7789V gamma setting-------------//
	TFT_SEND_CMD(0xE0); //Set Gamma
	TFT_SEND_DATA(0xD0);
	TFT_SEND_DATA(0x05);
	TFT_SEND_DATA(0x09);
	TFT_SEND_DATA(0x09);
	TFT_SEND_DATA(0x08);
	TFT_SEND_DATA(0x14);
	TFT_SEND_DATA(0x28);
	TFT_SEND_DATA(0x33);
	TFT_SEND_DATA(0x3F);
	TFT_SEND_DATA(0x07);
	TFT_SEND_DATA(0x13);
	TFT_SEND_DATA(0x14);
	TFT_SEND_DATA(0x28);
	TFT_SEND_DATA(0x30);
	 
	TFT_SEND_CMD(0XE1); //Set Gamma
	TFT_SEND_DATA(0xD0);
	TFT_SEND_DATA(0x05);
	TFT_SEND_DATA(0x09);
	TFT_SEND_DATA(0x09);
	TFT_SEND_DATA(0x08);
	TFT_SEND_DATA(0x03);
	TFT_SEND_DATA(0x24);
	TFT_SEND_DATA(0x32);
	TFT_SEND_DATA(0x32);
	TFT_SEND_DATA(0x3B);
	TFT_SEND_DATA(0x14);
	TFT_SEND_DATA(0x13);
	TFT_SEND_DATA(0x28);
	TFT_SEND_DATA(0x2F);

	TFT_SEND_CMD(0x21); 		//反显
   
	TFT_SEND_CMD(0x29);         //开启显示 
	
}

