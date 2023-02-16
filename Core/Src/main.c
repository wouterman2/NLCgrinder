/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "defines.h"
#include "utils.h"
#include "work.h"
#include "serial.h"
#include "grinder.h"
#include "adjust.h"
#include "scale.h"
#include "hopper.h"
#include "recipe.h"
#include "consumable.h"
#include "serial.h"
#include "analog.h"
#include "clrc663.h"
#include "rfid.h"
#include "led.h"
#include "flash.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern void SER_SetBaudRate (uint32_t newBaudRate);
void MX_USART3_UART_9600_Init(void);
uint8_t CheckString (uint8_t *StringToCheck, char *CheckString);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t a,q,p;
uint8_t AT_Flag = 0;
uint8_t AT_Counter = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t CheckString (uint8_t *StringToCheck, char *CheckString)
{
	uint8_t ReturnValue = 1;
	for (uint8_t i = 0; i< strlen(CheckString); i++)
	{
		if (StringToCheck[i] != CheckString[i])
		{
			ReturnValue = 0;
		}
	}
	return ReturnValue;
}

void MX_USART3_UART_9600_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  Initialized = 0;
  SCB->VTOR = FLA_MAINADDRESS; /* Vector Table Relocation to partition 2 in Internal FLASH */
  SysTick->CTRL = 1;
  //Enable interrupts
  __enable_irq();
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
  MX_ADC_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_TIM5_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  MX_USART2_UART_Init();
#ifdef CHECKHC06
  //Init USART3 at 9600 baud
  MX_USART3_UART_9600_Init();
  SER_Init();
  AT_Flag=1; //So the return data is not deleted
  AT_Counter = 0; //Return data counter
  char* Test = "AT";
  HAL_UART_Transmit (&huart3,(uint8_t*)Test, strlen(Test) , 100);
  HAL_Delay(1000);
  if (CheckString(Message.RxData, "OK") == 1) //HC06 is responding with OK, so it is set at 9600 baud
  {
	AT_Counter = 0;
	//Send command to set to 115200 baud
	char* Test2 = "AT+BAUD8";
	memset(Message.RxData, 0, sizeof(Message.RxData));
	HAL_UART_Transmit (&huart3,(uint8_t*)Test2, strlen(Test2) , 100);
	HAL_Delay(2000);
	if (CheckString(Message.RxData, "OK115200") == 1)
	{
		//Do something with the result --> Send to debug port
	}
  }

  //Set USART3 to 115200 baud
  HAL_UART_DeInit(&huart3);
  HAL_Delay(1000);
#endif
  MX_USART3_UART_Init();
  SER_Init(); //Init (again) to start DMA receive
#ifdef CHECKHC06
  //Set the module name to "nunc-grinder"
  AT_Counter = 0;
  memset(Message.RxData, 0, sizeof(Message.RxData));
  //char* Test3 = "AT+NAMEnunc-grinder";
  char* Test3 = "AT+NAMEnunc-grinder";
  //char* Test3 = "AT+BAUD4"; uncomment to set to 9600 baud
  HAL_UART_Transmit (&huart3,(uint8_t*)Test3, strlen(Test3) , 100);
  HAL_Delay(1000);
  if (CheckString(Message.RxData, "OKsetname") == 1)
  {
	  //Do something with the result --> Send to debug port
  }
  memset(Message.RxData, 0, sizeof(Message.RxData));
  Message.RxBuffer = 0;
  Message.Counter = 0;
  AT_Flag=0; //Clear the flag for normal use
#endif
  MX_ADC_Init();
  WRK_Init();
  REC_Init();
  GND_Init();
  ADJ_Init();
  SCL_Init();
  CON_Init();
  HOP_Init();
  ANA_Init();
  LED_Init();
  FLA_Init();

  //HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  Initialized = 1;
  while (1)
  {
	HAL_IWDG_Refresh(&hiwdg);
	if (Flg1ms == 1)
	{
		Flg1ms = 0;
		WRK_HandleTickTime();
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
