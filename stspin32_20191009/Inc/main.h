/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f0xx_hal.h"
#include "stdint.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PWR_SEN_Pin GPIO_PIN_1
#define PWR_SEN_GPIO_Port GPIOF
#define PWR_CTL_Pin GPIO_PIN_0
#define PWR_CTL_GPIO_Port GPIOF
#define DIR_Pin GPIO_PIN_1
#define DIR_GPIO_Port GPIOF
#define U_BEMF_Pin GPIO_PIN_0
#define U_BEMF_GPIO_Port GPIOA
#define U_BEMF_EXTI_IRQn EXTI0_1_IRQn
#define V_BEMF_Pin GPIO_PIN_1
#define V_BEMF_GPIO_Port GPIOA
#define V_BEMF_EXTI_IRQn EXTI0_1_IRQn
#define W_BEMF_Pin GPIO_PIN_2
#define W_BEMF_GPIO_Port GPIOA
#define W_BEMF_EXTI_IRQn EXTI2_3_IRQn
#define SPEED_AD_Pin GPIO_PIN_3
#define SPEED_AD_GPIO_Port GPIOA
#define V_BUS_AD_Pin GPIO_PIN_4
#define V_BUS_AD_GPIO_Port GPIOA
#define C_BUS_AD_Pin GPIO_PIN_5
#define C_BUS_AD_GPIO_Port GPIOA
#define OC_SEL_Pin GPIO_PIN_11
#define OC_SEL_GPIO_Port GPIOA
#define W_LOW_Pin GPIO_PIN_15
#define W_LOW_GPIO_Port GPIOB
#define V_LOW_Pin GPIO_PIN_14
#define V_LOW_GPIO_Port GPIOB
#define U_LOW_Pin GPIO_PIN_13
#define U_LOW_GPIO_Port GPIOB
#define W_HIGH_Pin GPIO_PIN_10
#define W_HIGH_GPIO_Port GPIOA
#define V_HIGH_Pin GPIO_PIN_8
#define V_HIGH_GPIO_Port GPIOA
#define U_HIGH_Pin GPIO_PIN_9
#define U_HIGH_GPIO_Port GPIOA
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
