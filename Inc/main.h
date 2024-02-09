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
#include "stm32f1xx_hal.h"

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
#define RTS2_0_Pin GPIO_PIN_1
#define RTS2_0_GPIO_Port GPIOA
#define TXD2_0_Pin GPIO_PIN_2
#define TXD2_0_GPIO_Port GPIOA
#define RXD2_0_Pin GPIO_PIN_3
#define RXD2_0_GPIO_Port GPIOA
#define LG1_Pin GPIO_PIN_4
#define LG1_GPIO_Port GPIOA
#define LG2_Pin GPIO_PIN_5
#define LG2_GPIO_Port GPIOA
#define LG3_Pin GPIO_PIN_6
#define LG3_GPIO_Port GPIOA
#define LG4_Pin GPIO_PIN_7
#define LG4_GPIO_Port GPIOA
#define LG5_Pin GPIO_PIN_0
#define LG5_GPIO_Port GPIOB
#define LG6_Pin GPIO_PIN_1
#define LG6_GPIO_Port GPIOB
#define BR0_Pin GPIO_PIN_12
#define BR0_GPIO_Port GPIOE
#define BR1_Pin GPIO_PIN_13
#define BR1_GPIO_Port GPIOE
#define RSV0_Pin GPIO_PIN_14
#define RSV0_GPIO_Port GPIOE
#define RSV1_Pin GPIO_PIN_15
#define RSV1_GPIO_Port GPIOE
#define TXD3_0_Pin GPIO_PIN_10
#define TXD3_0_GPIO_Port GPIOB
#define RXD3_0_Pin GPIO_PIN_11
#define RXD3_0_GPIO_Port GPIOB
#define LR1_Pin GPIO_PIN_12
#define LR1_GPIO_Port GPIOB
#define LR2_Pin GPIO_PIN_13
#define LR2_GPIO_Port GPIOB
#define RTS3_0_Pin GPIO_PIN_14
#define RTS3_0_GPIO_Port GPIOB
#define LR3_Pin GPIO_PIN_15
#define LR3_GPIO_Port GPIOB
#define LR4_Pin GPIO_PIN_10
#define LR4_GPIO_Port GPIOD
#define LR5_Pin GPIO_PIN_11
#define LR5_GPIO_Port GPIOD
#define LR6_Pin GPIO_PIN_13
#define LR6_GPIO_Port GPIOD
#define LR7_Pin GPIO_PIN_14
#define LR7_GPIO_Port GPIOD
#define LR8_Pin GPIO_PIN_15
#define LR8_GPIO_Port GPIOD
#define RTS4_0S_Pin GPIO_PIN_8
#define RTS4_0S_GPIO_Port GPIOC
#define RTS4_1S_Pin GPIO_PIN_9
#define RTS4_1S_GPIO_Port GPIOC
#define BEEP_Pin GPIO_PIN_8
#define BEEP_GPIO_Port GPIOA
#define RTS1_1S_Pin GPIO_PIN_11
#define RTS1_1S_GPIO_Port GPIOA
#define RTS1_0_Pin GPIO_PIN_12
#define RTS1_0_GPIO_Port GPIOA
#define TXD4_Pin GPIO_PIN_10
#define TXD4_GPIO_Port GPIOC
#define RXD4_Pin GPIO_PIN_11
#define RXD4_GPIO_Port GPIOC
#define TXD5_Pin GPIO_PIN_12
#define TXD5_GPIO_Port GPIOC
#define LR9_Pin GPIO_PIN_0
#define LR9_GPIO_Port GPIOD
#define PWR1_Pin GPIO_PIN_1
#define PWR1_GPIO_Port GPIOD
#define RXD5_Pin GPIO_PIN_2
#define RXD5_GPIO_Port GPIOD
#define RTS5S_Pin GPIO_PIN_3
#define RTS5S_GPIO_Port GPIOD
#define LG7_Pin GPIO_PIN_3
#define LG7_GPIO_Port GPIOB
#define LG8_Pin GPIO_PIN_4
#define LG8_GPIO_Port GPIOB
#define LG9_Pin GPIO_PIN_5
#define LG9_GPIO_Port GPIOB
#define TXD1_0_Pin GPIO_PIN_6
#define TXD1_0_GPIO_Port GPIOB
#define RXD1_0_Pin GPIO_PIN_7
#define RXD1_0_GPIO_Port GPIOB
#define PWR2_Pin GPIO_PIN_8
#define PWR2_GPIO_Port GPIOB
#define PWR3_Pin GPIO_PIN_9
#define PWR3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
