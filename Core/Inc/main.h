/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

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
#define BT_SLEEP_Pin 				GPIO_PIN_14
#define BT_SLEEP_GPIO_Port 			GPIOC



#define CS_Pin 						GPIO_PIN_1
#define CS_GPIO_Port 				GPIOA

#define RESET_Pin 					GPIO_PIN_4
#define RESET_GPIO_Port 			GPIOA
#define SCK_Pin 					GPIO_PIN_5
#define SCK_GPIO_Port 				GPIOA
#define DC_Pin 						GPIO_PIN_6
#define DC_GPIO_Port 				GPIOA
#define MOSI_Pin 					GPIO_PIN_7
#define MOSI_GPIO_Port 				GPIOA

//swtich key define
#define KEY1_Pin 					GPIO_PIN_0   //PB0 LED number be select KEY -
#define KEY1_GPIO_Port 				GPIOB

#define KEY2_Pin 					GPIO_PIN_1  //PB1 LED  number be select KEY +
#define KEY2_GPIO_Port 				GPIOB

#define KEY3_Pin 					GPIO_PIN_8   //PA8 filter number key -
#define KEY3_GPIO_Port 				GPIOA

#define KEY4_Pin 					GPIO_PIN_6  //PC6  filter number key+ 
#define KEY4_GPIO_Port 				GPIOC

#define KEY5_Pin 					GPIO_PIN_11  //PA11 "smart  button smart Mode "  be selected KEY +
#define KEY5_GPIO_Port 				GPIOA

#define KEY6_Pin 					GPIO_PIN_12 //PA12 "smart  button smart Mode " be selected KEY-
#define KEY6_GPIO_Port 				GPIOA

#define KEY7_Pin 					GPIO_PIN_4  //PB4 ---LED turn off 
#define KEY7_GPIO_Port 				GPIOB

#define KEY8_Pin 					GPIO_PIN_15  //PC15 ---LED bright decrease -
#define KEY8_GPIO_Port 				GPIOC

#define KEY9_Pin 					GPIO_PIN_0   //PA0 ----LED bright increase +
#define KEY9_GPIO_Port 				GPIOA


#define KEY10_Pin                   GPIO_PIN_15  //PA15  ---Spot board Mode switch 
#define KEY10_GPIO_Port             GPIOA

#define KEY11_Pin                   GPIO_PIN_13  //PA13  ---SWDIO update Soft GPIO
#define KEY11_GPIO_Port             GPIOA







//oled screen define
#define ENVCC_Pin 					GPIO_PIN_3  //PB3 --driver oled screen
#define ENVCC_GPIO_Port 			GPIOB


#define BT_RST_Pin 					GPIO_PIN_5
#define BT_RST_GPIO_Port 			GPIOB

#define BT_TX_Pin 					GPIO_PIN_6
#define BT_TX_GPIO_Port 			GPIOB

#define BT_RX_Pin 					GPIO_PIN_7
#define BT_RX_GPIO_Port 			GPIOB

#define BT_MRDY_Pin 				GPIO_PIN_8
#define BT_MRDY_GPIO_Port 			GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
