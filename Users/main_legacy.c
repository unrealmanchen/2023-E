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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define BUFF_MAX_SIZE 255
UART_X UART_X_1={0},UART_X_2={0},UART_X_3={0};
char a[]="start";
  int position[2]={0,0};
  int rotation[2]={1500,1500};
  int blank_time=0;
  char sprintf_buffer[400];
  


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UART_X *UART_temp = NULL;
  if (huart->Instance == USART1)
    UART_temp = &UART_X_1;
  else if (huart->Instance == USART2)
    UART_temp = &UART_X_2;
  else if (huart->Instance == USART3)
    UART_temp = &UART_X_3;

  char temp = UART_temp->IQR_buffer;
  if (temp == '#')
  {
    UART_temp->RxFlag = 1;
    UART_temp->buffer[UART_temp->buffer_pi] = '\0';
    UART_temp->buffer_pi = 0;
  }
  else
  {
    UART_temp->buffer[UART_temp->buffer_pi] = temp;
    UART_temp->buffer_pi++;
    if (UART_temp->buffer_pi > BUFFER_SIZE)
    {
      UART_temp->buffer_pi = 0;
    }
  }
  HAL_UART_Receive_IT(huart, (uint8_t *)&UART_temp->IQR_buffer, 1);
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  
  HAL_UART_Receive_IT(&huart2, (uint8_t *)&UART_X_2.IQR_buffer, 1);
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&UART_X_1.IQR_buffer, 1);
  
  
  HAL_UART_Transmit(&huart2, a, strlen(a), HAL_MAX_DELAY);
  
  ssd1306_Init(&hi2c1);
  draw(0);
  
  
  sprintf(sprintf_buffer, "{#000P1500T0100!#001P1500T0100!}");
	HAL_UART_Transmit_IT(&huart3,sprintf_buffer,strlen(sprintf_buffer));
  int startflag=0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    #define UART_DELAY 500
    #define NORMAL
    #ifdef NORMAL
    int empty_flag=0;
    button_check();
    if(UART_X_2.RxFlag==1){
      UART_X_2.RxFlag=0;
      HAL_UART_Transmit(&huart2,UART_X_2.buffer,strlen(UART_X_2.buffer),UART_DELAY);
    }
    if(UART_X_1.RxFlag==1){
      startflag=1;
      UART_X_1.RxFlag=0;
      sscanf(UART_X_1.buffer,"%d %d",&position[0],&position[1]);  
    }
    if(UART_X_3.RxFlag==1){
      UART_X_3.RxFlag=0;
      sscanf(UART_X_1.buffer,"%d",&position[0],&position[1]);  
    }
    if (position[0] == -1 || position[1] == -1)
		  {
        empty_flag=1;
				blank_time++;
        
			}
			else{
				blank_time = 0;
        empty_flag=0;
				//sprintf(sprintf_buffer, "pos0=%d pos1=%d\r\n", position[0], position[1]);
				//HAL_UART_Transmit(&huart2,sprintf_buffer,strlen(sprintf_buffer),UART_DELAY);
			}
    if (blank_time == 13)
		{
			HAL_UART_Transmit(&huart2,"reset\n",strlen("reset\n"),UART_DELAY);
		}
		else if (blank_time > 13)
		{
			reset();
      rotation[0]=1500;
      rotation[1]=1500;
      calcu_result[1]=0;
      calcu_result[0]=0;
			sprintf(sprintf_buffer, "{#000P1500T0100!#001P1500T0100!}");
			HAL_UART_Transmit(&huart3,sprintf_buffer,strlen(sprintf_buffer),UART_DELAY);
			//HAL_UART_Transmit(&huart2,sprintf_buffer,strlen(sprintf_buffer),HAL_MAX_DELAY);
    }else{
      if (empty_flag == 1||startflag==0)
      {
        HAL_Delay(milis_time);
        continue;
      }
      else
      {
        
        calcu(position, calcu_result, 0);
        calcu(position, calcu_result, 1);
        //draw(2);
        if (rotation[0] - calcu_result[0] > 2000)
          rotation[0] = 2000;
        else if (rotation[0] - calcu_result[0] < 1000)
          rotation[0] = 1000;
        else
          rotation[0] -= calcu_result[0];
        if (rotation[1] + calcu_result[1] > 2000)
          rotation[1] = 2000;
        else if (rotation[1] + calcu_result[1] < 1000)
          rotation[1] = 1000;
        else
          rotation[1] += calcu_result[1];

        sprintf(sprintf_buffer, "{#000P%dT00%d!#001P%dT00%d!}", rotation[0],milis_time, rotation[1], milis_time);
        HAL_UART_Transmit(&huart3, sprintf_buffer, strlen(sprintf_buffer), UART_DELAY);
        //sprintf(sprintf_buffer, "pos0=%d calcu=%d pos1=%d calcu=%d\r\n", rotation[0], calcu_result[0], rotation[1], calcu_result[1]);
        HAL_UART_Transmit(&huart2, sprintf_buffer, strlen(sprintf_buffer),UART_DELAY);
      }
    }

    HAL_Delay(milis_time);
    #endif


    #ifdef DEBUG
    
      
    #endif
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
