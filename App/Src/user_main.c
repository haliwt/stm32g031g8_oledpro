/*
 * user_main.c
 *
 *  Created on: Jan 12, 2021
 *      Author: om
 */

#include "oled.h"
#include "handleKey.h"
#include "cmd_link.h"

static uint16_t scanKey(void);
static void displayUnionInfo(uint8_t unionIndex);
static void displayUnionInfo_Manual(uint8_t unionIndex);


//static KeyStruct glKey;				// key for forward/backward
static uint8_t echoFilter;
static uint8_t echoLight;
static uint8_t  echoLight_LR; //WT.EDIT 2021.04.23
static uint8_t  echoLight_AU; //WT.EDIT 2021.04.28

static uint8_t echoUnion;
static uint8_t  echoUnion_manual; //WT.EDIT 2021.04.28
static uint8_t echoGroup;
static uint8_t timerDownFlag;
static uint32_t counter_15m;
static uint16_t counter_ActionDelay;
static uint16_t counter_Waitingtime;
static uint8_t  waitingTimeoutFlag;
static uint8_t  timeoutFlag;
static uint8_t checkParameterFlag;
static uint8_t	_250msFlag;
static uint8_t _500msFlag;

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
			     
				  auxiliary_t.SmartKey = 0;
			
			    if(auxiliary_t.AuxiliarySubItem>=MAX_LIGHT_LR_NUMBER-1){
					auxiliary_t.AuxiliarySubItem=0;
					auxiliary_t.mainLedKey =0;
					HAL_UART_Transmit(&CMD_LINKER,&auxiliary_t.AuxiliarySubItem,1,2);
			    }
				else{ 
					auxiliary_t.mainLedKey =1;
					auxiliary_t.AuxiliarySubItem ++;
				    //if(auxiliary_t.AuxiliarySubItem == 1) auxiliary_t.mainLedKey = 0;
					//else auxiliary_t.mainLedKey = 1;
					HAL_UART_Transmit(&CMD_LINKER,&auxiliary_t.AuxiliarySubItem,1,2);
				}
				
				//echoGroup=ECHO_GROUP_A;
				//echoLight = LED Name
				turnoffAllLight();
				printSettingInfo_LR_Led(echoUnion_manual,echoFilter,auxiliary_t.AuxiliarySubItem,BLINK_OFF); 
						
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

