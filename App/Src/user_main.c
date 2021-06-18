/*
 * user_main.c
 *
 *  Created on: Jan 12, 2021
 *      Author: om
 */

#include "oled.h"
#include "handleKey.h"
#include "cmd_link.h"



void user_main(void)
{
	 static uint8_t subIO;
	oledInit();
	cmdInit();
	keyInit();
	while(1)
	{
		handleInput();
		decode();
		 /* USER CODE END WHILE */
		
		// if( HAL_GPIO_ReadPin(KEY11_GPIO_Port, KEY11_Pin)==0)	// auxiliary SideBotton select sub item 2021.05.31
		SideButtonSub_KEY();
//			  if(subIO==0) //SWDIO update softe GPIO initial
//			  {
//			             subIO++;
//			  		   //initial subIO delay 2s
//			  		   HAL_Delay(3000);
//			            SideBotton_Init();
//			  }


	}
}

