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

#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_rtc.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

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
#define WAT_LOW_Pin LL_GPIO_PIN_13
#define WAT_LOW_GPIO_Port GPIOC
#define TEMP_2_Pin LL_GPIO_PIN_0
#define TEMP_2_GPIO_Port GPIOA
#define TEMP_1_Pin LL_GPIO_PIN_1
#define TEMP_1_GPIO_Port GPIOA
#define TEMP_R_Pin LL_GPIO_PIN_2
#define TEMP_R_GPIO_Port GPIOA
#define WAT_HI_Pin LL_GPIO_PIN_3
#define WAT_HI_GPIO_Port GPIOA
#define BUT_START_Pin LL_GPIO_PIN_4
#define BUT_START_GPIO_Port GPIOA
#define BUT_STOP_Pin LL_GPIO_PIN_5
#define BUT_STOP_GPIO_Port GPIOA
#define BUT_MENU_Pin LL_GPIO_PIN_6
#define BUT_MENU_GPIO_Port GPIOA
#define BUT_LEFT_Pin LL_GPIO_PIN_7
#define BUT_LEFT_GPIO_Port GPIOA
#define BUT_RIGHT_Pin LL_GPIO_PIN_0
#define BUT_RIGHT_GPIO_Port GPIOB
#define AC_MOTOR_Pin LL_GPIO_PIN_1
#define AC_MOTOR_GPIO_Port GPIOB
#define HEATER_Pin LL_GPIO_PIN_2
#define HEATER_GPIO_Port GPIOB
#define DISP_DB2_Pin LL_GPIO_PIN_10
#define DISP_DB2_GPIO_Port GPIOB
#define LED_R_Pin LL_GPIO_PIN_11
#define LED_R_GPIO_Port GPIOB
#define DISP_BS2_Pin LL_GPIO_PIN_12
#define DISP_BS2_GPIO_Port GPIOB
#define DISP_BS1_Pin LL_GPIO_PIN_13
#define DISP_BS1_GPIO_Port GPIOB
#define DISP_RS_Pin LL_GPIO_PIN_14
#define DISP_RS_GPIO_Port GPIOB
#define DISP_VO_Pin LL_GPIO_PIN_15
#define DISP_VO_GPIO_Port GPIOB
#define COOLER_Pin LL_GPIO_PIN_8
#define COOLER_GPIO_Port GPIOA
#define DC_MOTOR_Pin LL_GPIO_PIN_9
#define DC_MOTOR_GPIO_Port GPIOA
#define BACKLIGHT_Pin LL_GPIO_PIN_10
#define BACKLIGHT_GPIO_Port GPIOA
#define DISP_E_Pin LL_GPIO_PIN_11
#define DISP_E_GPIO_Port GPIOA
#define DISP_RW_Pin LL_GPIO_PIN_12
#define DISP_RW_GPIO_Port GPIOA
#define SPEAKER_Pin LL_GPIO_PIN_15
#define SPEAKER_GPIO_Port GPIOA
#define DISP_DB3_Pin LL_GPIO_PIN_3
#define DISP_DB3_GPIO_Port GPIOB
#define DISP_DB4_Pin LL_GPIO_PIN_4
#define DISP_DB4_GPIO_Port GPIOB
#define DISP_DB5_Pin LL_GPIO_PIN_5
#define DISP_DB5_GPIO_Port GPIOB
#define DISP_DB0_Pin LL_GPIO_PIN_6
#define DISP_DB0_GPIO_Port GPIOB
#define DISP_DB1_Pin LL_GPIO_PIN_7
#define DISP_DB1_GPIO_Port GPIOB
#define DISP_DB7_Pin LL_GPIO_PIN_8
#define DISP_DB7_GPIO_Port GPIOB
#define DISP_DB6_Pin LL_GPIO_PIN_9
#define DISP_DB6_GPIO_Port GPIOB
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
