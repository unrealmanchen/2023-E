#include "main.h"
#include "gpio.h"
#include "func.h"
#include "PID.h"
#include "math.h"
#include "ssd1306.h"
#include <stm32f407xx.h>

// typedef struct BUTTON{
//     GPIO_TypeDef* GPIO;
//     uint16_t GPIO_Pin;
//     int delay_loop;
//     int change;
// }button_struct;
// button_struct buttonlist[6]={
//     {GPIOA,GPIO_PIN_12,0,0},
//     {GPIOA,GPIO_PIN_11,0,0},
//     {GPIOA,GPIO_PIN_8,0,0},
//     {GPIOC,GPIO_PIN_8,0,0},
//     {GPIOC,GPIO_PIN_6,0,0},
//     {GPIOG,GPIO_PIN_8,0,0}
// };

int cursor[2]={0,0};

// void button_check(){
//     int i,a;
//     a=-1;
//     for(i=0;i<6;i++){
//         if(HAL_GPIO_ReadPin(buttonlist[i].GPIO,buttonlist[i].GPIO_Pin)== GPIO_PIN_RESET){
//             buttonlist[i].change=0;
//         }
//         if(buttonlist[i].delay_loop>0)
//             buttonlist[i].delay_loop--;
//     }
//     for(i=0;i<6;i++){
//         if(HAL_GPIO_ReadPin(buttonlist[i].GPIO,buttonlist[i].GPIO_Pin)== GPIO_PIN_SET
//         &&buttonlist[i].delay_loop==0&&buttonlist[i].change==0){
//             buttonlist[i].delay_loop=150000;
//             buttonlist[i].change=1;
//             a=i;
//             break;
//         }
        
            
//     }
//     switch (a)
//     {
//         case 0:
//         screen_move(0);
//         break;

//         case 1:
//         screen_move(1);
//         break;

//         case 2:
//         screen_move(2);
//         break;

//         case 3:
//         screen_move(3);
//         break;
//     default:
//         break;
//     }
// }



void draw(int func)
{
  #define draw1
  #ifdef draw1
  char sprintf_buffer[40];
  if (func == 0)
  {
    //Screen_show_string(char *str, u16 start_pos_x, u16 start_pos_y, FontDef font, u16 color,int func)
    Screen_show_string("PWMR1",10,10,Font_11x18,WHITE,1);
    Screen_show_string("PWMR2",10,30,Font_11x18,WHITE,1);
    Screen_show_string("PWMG1",10,50,Font_11x18,WHITE,1);
    Screen_show_string("PWMG2",10,70,Font_11x18,WHITE,1);
    sprintf(sprintf_buffer, "%4d  ", rotation_red[0]);
    Screen_show_string(sprintf_buffer,70,10,Font_11x18,ORANGE,1);
    sprintf(sprintf_buffer, "%4d  ", rotation_red[1]);
    Screen_show_string(sprintf_buffer,70,30,Font_11x18,ORANGE,1);
    sprintf(sprintf_buffer, "%4d  ", rotation_green[0]);
    Screen_show_string(sprintf_buffer,70,50,Font_11x18,ORANGE,1);
    sprintf(sprintf_buffer, "%4d  ", rotation_green[1]);
    Screen_show_string(sprintf_buffer,70,70,Font_11x18,ORANGE,1);
  }
  #endif
}
#define Rotate_MAX 2000
#define Rotate_MIN 1000
#define Dim_X 161
#define Dim_Y 136




