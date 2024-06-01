/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define WATER_LEVEL_LOW_Pin GPIO_PIN_13
#define WATER_LEVEL_LOW_GPIO_Port GPIOC
#define SENSE_TEMP_2_Pin GPIO_PIN_0
#define SENSE_TEMP_2_GPIO_Port GPIOA
#define SENSE_TEMP_1_Pin GPIO_PIN_1
#define SENSE_TEMP_1_GPIO_Port GPIOA
#define SENSE_TEMP_RADIATOR_Pin GPIO_PIN_2
#define SENSE_TEMP_RADIATOR_GPIO_Port GPIOA
#define WATER_LEVEL_HIGH_Pin GPIO_PIN_3
#define WATER_LEVEL_HIGH_GPIO_Port GPIOA
#define START_BUTTON_Pin GPIO_PIN_4
#define START_BUTTON_GPIO_Port GPIOA
#define STOP_BUTTON_Pin GPIO_PIN_5
#define STOP_BUTTON_GPIO_Port GPIOA
#define MENU_BUTTON_Pin GPIO_PIN_6
#define MENU_BUTTON_GPIO_Port GPIOA
#define LEFT_BUTTON_Pin GPIO_PIN_7
#define LEFT_BUTTON_GPIO_Port GPIOA
#define RIGHT_BUTTON_Pin GPIO_PIN_0
#define RIGHT_BUTTON_GPIO_Port GPIOB
#define AC_MOTOR_Pin GPIO_PIN_1
#define AC_MOTOR_GPIO_Port GPIOB
#define HEATER_Pin GPIO_PIN_2
#define HEATER_GPIO_Port GPIOB
#define DISP_DB2_Pin GPIO_PIN_10
#define DISP_DB2_GPIO_Port GPIOB
#define LED_R_Pin GPIO_PIN_11
#define LED_R_GPIO_Port GPIOB
#define DISP_RS_Pin GPIO_PIN_14
#define DISP_RS_GPIO_Port GPIOB
#define COOLER_Pin GPIO_PIN_8
#define COOLER_GPIO_Port GPIOA
#define DC_MOTOR_Pin GPIO_PIN_9
#define DC_MOTOR_GPIO_Port GPIOA
#define BACKLIGHT_Pin GPIO_PIN_10
#define BACKLIGHT_GPIO_Port GPIOA
#define DISP_E_Pin GPIO_PIN_11
#define DISP_E_GPIO_Port GPIOA
#define DISP_RW_Pin GPIO_PIN_12
#define DISP_RW_GPIO_Port GPIOA
#define SPEAKER_Pin GPIO_PIN_15
#define SPEAKER_GPIO_Port GPIOA
#define DISP_DB3_Pin GPIO_PIN_3
#define DISP_DB3_GPIO_Port GPIOB
#define DISP_DB4_Pin GPIO_PIN_4
#define DISP_DB4_GPIO_Port GPIOB
#define DISP_DB5_Pin GPIO_PIN_5
#define DISP_DB5_GPIO_Port GPIOB
#define DISP_DB7_Pin GPIO_PIN_8
#define DISP_DB7_GPIO_Port GPIOB
#define DISP_DB6_Pin GPIO_PIN_9
#define DISP_DB6_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
