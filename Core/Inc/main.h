/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "eeprom.h"
#include "analog.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NTC1_Pin GPIO_PIN_0
#define NTC1_GPIO_Port GPIOC
#define SW5_Pin GPIO_PIN_1
#define SW5_GPIO_Port GPIOC
#define SW6_Pin GPIO_PIN_2
#define SW6_GPIO_Port GPIOC
#define SW7_Pin GPIO_PIN_3
#define SW7_GPIO_Port GPIOC
#define ENC2A_Pin GPIO_PIN_0
#define ENC2A_GPIO_Port GPIOA
#define ENC2B_Pin GPIO_PIN_1
#define ENC2B_GPIO_Port GPIOA
#define LCDATA_Pin GPIO_PIN_4
#define LCDATA_GPIO_Port GPIOA
#define LCCLK_Pin GPIO_PIN_5
#define LCCLK_GPIO_Port GPIOA
#define M3A_Pin GPIO_PIN_6
#define M3A_GPIO_Port GPIOA
#define M3B_Pin GPIO_PIN_7
#define M3B_GPIO_Port GPIOA
#define HW1_Pin GPIO_PIN_4
#define HW1_GPIO_Port GPIOC
#define CUR1_Pin GPIO_PIN_5
#define CUR1_GPIO_Port GPIOC
#define M2A_Pin GPIO_PIN_0
#define M2A_GPIO_Port GPIOB
#define M2B_Pin GPIO_PIN_1
#define M2B_GPIO_Port GPIOB
#define HW0_Pin GPIO_PIN_2
#define HW0_GPIO_Port GPIOB
#define SPI2_NSS_Pin GPIO_PIN_12
#define SPI2_NSS_GPIO_Port GPIOB
#define HW3_Pin GPIO_PIN_6
#define HW3_GPIO_Port GPIOC
#define HW2_Pin GPIO_PIN_7
#define HW2_GPIO_Port GPIOC
#define CONN_Pin GPIO_PIN_8
#define CONN_GPIO_Port GPIOC
#define RFID_RES_Pin GPIO_PIN_9
#define RFID_RES_GPIO_Port GPIOC
#define RFID_IRQ_Pin GPIO_PIN_10
#define RFID_IRQ_GPIO_Port GPIOA
#define RFID_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define M1_Pin GPIO_PIN_11
#define M1_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_12
#define LED_GPIO_Port GPIOA
#define SW1_Pin GPIO_PIN_15
#define SW1_GPIO_Port GPIOA
#define SW1_EXTI_IRQn EXTI15_10_IRQn
#define SW2_Pin GPIO_PIN_10
#define SW2_GPIO_Port GPIOC
#define SW3_Pin GPIO_PIN_11
#define SW3_GPIO_Port GPIOC
#define SW4_Pin GPIO_PIN_4
#define SW4_GPIO_Port GPIOB
#define ENC1A_Pin GPIO_PIN_5
#define ENC1A_GPIO_Port GPIOB
#define ENC3B_Pin GPIO_PIN_6
#define ENC3B_GPIO_Port GPIOB
#define ENC3A_Pin GPIO_PIN_7
#define ENC3A_GPIO_Port GPIOB
#define ENC1B_Pin GPIO_PIN_9
#define ENC1B_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define GRINDER
#define CHECKHC06
#define HAL0		ANA_GetSensorValue(ADC_SW5)//ADC_Voltage_Values[3]//(HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin) == GPIO_PIN_RESET)
#define HAL1		ANA_GetSensorValue(ADC_SW6)//ADC_Voltage_Values[4]//(HAL_GPIO_ReadPin(SW6_GPIO_Port, SW6_Pin) == GPIO_PIN_RESET)
#define HAL2		ANA_GetSensorValue(ADC_SW7)//ADC_Voltage_Values[5]//(HAL_GPIO_ReadPin(SW7_GPIO_Port, SW7_Pin) == GPIO_PIN_RESET)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
