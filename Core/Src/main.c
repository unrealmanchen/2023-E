/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stm32f407xx.h>
#include "PID.h"
#include "func.h"
#include "tft.h"
#include "fonts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int mode=0;
int NVIC_flag=0;
int stopflag=0;
int Timer_counter=0;
int dif[2]={0};
int dif_dot[2]={0};
int rotation_green[2]={1100,700};
int rotation_red[2]={1700,900};
UART_X UART_cam={0},UART_PC={0},UART_Mot={0}; //串口缓冲区结构体
void delay_us(uint32_t nus)
{
 uint32_t temp;
 SysTick->LOAD = 10*nus;
 SysTick->VAL=0X00;
 SysTick->CTRL=0X01;
 do
 {
  temp=SysTick->CTRL;
 }while((temp&0x01)&&(!(temp&(1<<16))));
     SysTick->CTRL=0x00; 
    SysTick->VAL =0X00; 
}

//毫秒延时
void delay_ms(uint16_t nms)
{
 uint32_t temp;
 SysTick->LOAD = 9000*nms;
 SysTick->VAL=0X00;
 SysTick->CTRL=0X01;
 do
 {
  temp=SysTick->CTRL;
 }while((temp&0x01)&&(!(temp&(1<<16))));
    SysTick->CTRL=0x00;
    SysTick->VAL =0X00; 
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//1ms计时器 pid运算用
{

	if(htim->Instance == TIM1)
	{
		Timer_counter++;
	}

}
/*
  UART1--电脑通信
  UART2--树莓派通信
  UART3没用 
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//串口回调
{
  UART_X *UART_temp = NULL;
  if (huart->Instance == USART2)
    UART_temp = &UART_cam;
  else if (huart->Instance == USART1)
    UART_temp = &UART_PC;
  else if (huart->Instance == USART3)
    UART_temp = &UART_Mot;

  char temp = UART_temp->IQR_buffer;
  if (temp == '!')
  {
    UART_temp->RxFlag = 1;
    UART_temp->buffer[UART_temp->buffer_pi] = '!';
    UART_temp->buffer[++UART_temp->buffer_pi] = '\0';
    UART_temp->buffer_pi = 0;
  }
  else if (temp == '#'){
    UART_temp->buffer[0] = '#';
    UART_temp->buffer_pi = 1;
  }
  else
  {
    UART_temp->buffer[UART_temp->buffer_pi] = temp;
    UART_temp->buffer_pi++;

  }
  if (UART_temp->buffer_pi > BUFFER_SIZE-2)
    {
      UART_temp->buffer_pi = 0;
    }
  HAL_UART_Receive_IT(huart, (uint8_t *)&UART_temp->IQR_buffer, 1);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{ 
	if(GPIO_Pin==GPIO_PIN_7&&NVIC_flag==1)
	{ 
    if(mode==0||mode==1){
      if(stopflag==0){
        stopflag=1;
        char str_ssd[20]="STOP   ";
        Screen_show_string(str_ssd,20,210,Font_16x26,RED,1);
        rotation_red[0]=1700;
        rotation_red[1]=900;
        
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
        
      }
      else{
        stopflag=0;
        char str_ssd[20]="Actived";
        Screen_show_string(str_ssd,20,210,Font_16x26,GREEN,1);
        
      }
    }
    if(mode==2){
      if(stopflag==0){
        stopflag=1;
        char str_ssd[20]="STOP    ";
        Screen_show_string(str_ssd,20,210,Font_16x26,RED,1);
      }
      else{
        stopflag=0;
        char str_ssd[20]="Actived";
        Screen_show_string(str_ssd,20,210,Font_16x26,GREEN,1);
        
      }
    }
    
    
	}
  if(GPIO_Pin==GPIO_PIN_8&&NVIC_flag==1)
	{ 
    char string1[4]="R";
    //TFT_full_color(RED);
    HAL_UART_Transmit(&huart2,string1,strlen(string1),1000);
	}
  
  delay_ms(2000);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, (uint8_t *)&UART_Mot.IQR_buffer, 1);
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&UART_PC.IQR_buffer, 1);
  HAL_UART_Receive_IT(&huart2, (uint8_t *)&UART_cam.IQR_buffer, 1);
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
  
  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,rotation_green[0]);
  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,rotation_green[1]);
  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
  int calcu_flag=0;

  
  tft_init(); 
  TFT_full_color(BLACK);
  char str_ssd[20]="Actived";
  Screen_show_string(str_ssd,20,210,Font_16x26,GREEN,1);
  char str_ssd12[20]="MODE 0";
  Screen_show_string(str_ssd12,20,170,Font_16x26,YELLOW,1);
  //HAL_Delay(200);
  NVIC_flag=1;
  
  draw(0);
  // Screen_show_char('^',70,100,Font_11x18,RED,1);
  // Screen_show_char('<',120,10,Font_11x18,GREEN,1);
  // while(1){
  //   button_check();
  // }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /*
    前部分内容时解析串口数据
    串口数据格式为 #M X Y!
    M代表解析模式 1、2、3、4、5
    XY分别为整数型数据
    */
    if (UART_Mot.RxFlag == 1)
    {
      UART_Mot.RxFlag = 0;
      HAL_UART_Transmit(&huart2, (uint8_t *)UART_Mot.buffer, strlen(UART_Mot.buffer), 1000);
    }
    if (UART_PC.RxFlag == 1) 
    {
      UART_PC.RxFlag = 0;
      int buffer,temp;
      sscanf(UART_PC.buffer, " #%d %d!",&temp,&buffer);
      switch (temp)
      {
        
      case 1://case 1--case 4均用于debug调试舵机旋转角度 #1 x y!格式
        rotation_red[0] = buffer;
       break;
      case 2:
        rotation_red[1] = buffer;
        break;
      case 3:
        rotation_green[0] = buffer;
        break;
      case 4:
        rotation_green[1] = buffer;
        break;
      case 5://切换模式 格式 #5 x! x代表模式几
        char buffer123='R';
        HAL_UART_Transmit(&huart2,&buffer123,1,1000);//告诉opencv重新寻找矩形
        mode=buffer;
        switch (mode)
        {
        case 0:
          char str_ssd1[20]="MODE 0";
          Screen_show_string(str_ssd1,20,170,Font_16x26,YELLOW,1);
          break;
        case 1:
          char str_ssd2[20]="MODE 1";
          Screen_show_string(str_ssd2,20,170,Font_16x26,YELLOW,1);
          break;
        case 2:
          char str_ssd3[20]="MODE 2";
          Screen_show_string(str_ssd3,20,170,Font_16x26,YELLOW,1);
          break;
        default:
          break;
        }
        char a[2]=0;
        sprintf(a,"%d",mode);
        HAL_UART_Transmit(&huart2,a,1,1000);
        rotation_red[0]=1700;
        rotation_red[1]=900;
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
        
        rotation_green[0]=1100;
        rotation_green[1]=700;
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,rotation_green[0]);
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,rotation_green[1]);
        break;
      case 6://告诉opencv重新寻找矩形
        rotation_red[0]=1700;
        rotation_red[1]=900;
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
        
        rotation_green[0]=1100;
        rotation_green[1]=700;
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,rotation_green[0]);
        __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,rotation_green[1]);
        char buffer1='R';
        HAL_UART_Transmit(&huart2,&buffer1,1,1000);
        break;
      default:
        break;
      }
      draw(0);
    }
    if (UART_cam.RxFlag == 1){//解析opencv数据 
      UART_cam.RxFlag = 0;
      //HAL_UART_Transmit(&huart1, (uint8_t *)UART_cam.buffer, strlen(UART_cam.buffer), 1000);
      if(UART_cam.buffer[1]=='a'){
        sscanf(UART_cam.buffer, " #a%d %d!",&dif[1],&dif[0]);
        calcu_flag=1;
      }else if(UART_cam.buffer[1]=='b'){
        sscanf(UART_cam.buffer, " #b%d %d!",&dif_dot[1],&dif_dot[0]);
      }
      
    }
begin:
    if(mode==0&&stopflag==0){//沿着铅笔框走
      rotation_red[0]=1500;
      rotation_red[1]=710;
      if(stopflag==1)
        goto begin;
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
      delay_ms(800);
      rotation_red[0]=1500;
      rotation_red[1]=1070;
      if(stopflag==1)
        goto begin;
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
      delay_ms(800);
      rotation_red[0]=1900;
      rotation_red[1]=1070;
      if(stopflag==1)
        goto begin;
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
      delay_ms(800);
      rotation_red[0]=1900;
      rotation_red[1]=710;
      if(stopflag==1)
        goto begin;
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
      delay_ms(800);
    }
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,rotation_green[0]);
    // __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,rotation_green[1]);
    
   
    if(Timer_counter>=CYCLE_TIME&&stopflag==0&&mode==1){//红色激光循迹
      Timer_counter=0;
      calcu(dif,rotation_red,&PID_red);
      dif[0]=0;
      dif[1]=0;
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
     // HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 1000);
      //sprintf(sprintf_buffer, "%d %d \n", rotation[0], rotation[1]);
    }
    if(Timer_counter>=CYCLE_TIME&&stopflag==0&&mode==2){//绿色激光跟随红色激光
      Timer_counter=0;
      calcu(dif,rotation_red,&PID_red);
      calcu(dif_dot,rotation_green,&PID_green);
      char buffer[30];
      sprintf(buffer,"%d %d %d %d",rotation_red[0],rotation_red[1],dif[0],dif[1]);
      dif[0]=0;
      dif[1]=0;
      dif_dot[0]=0;
      dif_dot[1]=0;
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,rotation_green[0]);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,rotation_green[1]);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,rotation_red[0]);
      __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,rotation_red[1]);
     // HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 1000);
      //sprintf(sprintf_buffer, "%d %d \n", rotation[0], rotation[1]);
    }
   
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
