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
		if(auxiliary_t.Auxiliary_flag==1){
			if(subIO==0) //SWDIO update softe GPIO initial
			 {
			           subIO++;
			  		   //initial subIO delay 2s
			  		   //HAL_Delay(3000);
			           SideBotton_Init();
			 }
			 SideButtonSub_KEY();
		}


	}
}

