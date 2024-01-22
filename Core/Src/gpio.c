/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(GPIOB, AC_MOTOR_Pin|HEATER_Pin|DISP_DB2_Pin|LED_R_Pin
                          |DISP_BS2_Pin|DISP_BS1_Pin|DISP_RS_Pin|DISP_VO_Pin
                          |DISP_DB3_Pin|DISP_DB4_Pin|DISP_DB5_Pin|DISP_DB0_Pin
                          |DISP_DB1_Pin|DISP_DB7_Pin|DISP_DB6_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, DISP_E_Pin|DISP_RW_Pin);

  /**/
  GPIO_InitStruct.Pin = WAT_LOW_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(WAT_LOW_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = WAT_HI_Pin|BUT_START_Pin|BUT_STOP_Pin|BUT_MENU_Pin
                          |BUT_LEFT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = BUT_RIGHT_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(BUT_RIGHT_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = AC_MOTOR_Pin|HEATER_Pin|DISP_DB2_Pin|LED_R_Pin
                          |DISP_BS2_Pin|DISP_BS1_Pin|DISP_RS_Pin|DISP_VO_Pin
                          |DISP_DB3_Pin|DISP_DB4_Pin|DISP_DB5_Pin|DISP_DB0_Pin
                          |DISP_DB1_Pin|DISP_DB7_Pin|DISP_DB6_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = DISP_E_Pin|DISP_RW_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
