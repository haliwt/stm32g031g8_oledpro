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
		if(SideButton_SubItem_Input())
		// if( HAL_GPIO_ReadPin(KEY11_GPIO_Port, KEY11_Pin)==0)	// auxiliary SideBotton select sub item 2021.05.31
		 {
			 	if(auxiliary_t.ManualMode ==1){ //manual mode "SPOT" "SIDE" "LEFT" "RIGHT"
			     
			
			    if(auxiliary_t.AuxiliarySubItem>=MAX_LIGHT_LR_NUMBER-1){
					auxiliary_t.AuxiliarySubItem=Main;
				
				    auxiliary_t.subMenuOne=1; //WT.EDIT 2021.06.03
					HAL_UART_Transmit(&CMD_LINKER,&auxiliary_t.AuxiliarySubItem,1,2);
			    }
				else{ 
					
					 auxiliary_t.subMenuOne=0; //WT.EDIT 2021.06.03
					auxiliary_t.AuxiliarySubItem ++;
			
					HAL_UART_Transmit(&CMD_LINKER,&auxiliary_t.AuxiliarySubItem,1,2);
				}
				
				//echoGroup=ECHO_GROUP_A;
				//echoLight = LED Name
				turnoffAllLight();
				printSettingInfo_SubItem();
						
				}


		 }
			  if(subIO==0)
			  {
			             subIO++;
			  		   //initial subIO delay 2s
			  		   HAL_Delay(3000);
			            SideBotton_Init();
			  }


}
}

