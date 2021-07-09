/*
 * handleKey.c
 *
 *  Created on: 2019��7��30��
 *      Author: nk
 */
#include "tim.h"
#include "keyDef.h"
#include "cmd_link.h"
#include "oled.h"
#include "handlekey.h"
//#include "gpio.h"
//#include "tim.h"
//#include "usart.h"
//#include "camCmdDef.h"
//#include "ledCmdDef.h"
//#include "saveParam.h"

//#define BUFFER_SIZE	64
//
//#define LED_BOARD_ADDR	35
//#define CAM_BOARD_ADDR	87
//
//#define LEVEL_DEFAULT	10
//#define LEVEL_MAX		20
//#define LEVEL_MIN		0
//#define MODE_NORMAL	0
//#define MODE_ADJ	1
//
//#define LEVEL_ADJ_UP	0
//#define LEVEL_ADJ_DOWN	1

#define LIGHT_ON_LEVEL	GPIO_PIN_SET
#define LIGHT_OFF_LEVEL	GPIO_PIN_RESET

#define LIGHT_ON_INDICATE_LEVEL		GPIO_PIN_RESET
#define LIGHT_OFF_INDICATE_LEVEL	GPIO_PIN_SET

#define LED_INDEX_1		0
#define LED_INDEX_2		1
#define LED_INDEX_3		2
#define LED_INDEX_4		3
#define LED_INDEX_5		4
#define LED_INDEX_6		5

#define LED_NUMBER 6

_filterSt filter_t;


static uint16_t scanKey(void);
static void displayUnionInfo(uint8_t unionIndex);



static KeyStruct glKey;				// key for forward/backward
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
static uint8_t checkParameterFlag_500ms;
static uint8_t	_250msFlag;
static uint8_t _500msFlag;




//static uint16_t scanKey(void)
//{
//	volatile uint16_t tmpKeycode,portA,portC;
//
//	portA= (GPIOA->IDR & 0x1900)>>8;
//	portC= (GPIOC->IDR & 0x40)>>3;
//	tmpKeycode=  GPIOB->IDR & 0x03;
//	tmpKeycode |= ((portA & 0x01)<<2);
//	tmpKeycode |= ((portA & 0x18)<<1);
//	tmpKeycode |= portC;
//	tmpKeycode &= NO_KEY;
//
//	return tmpKeycode;
//}

static uint16_t scanKey(void)
{
	volatile uint16_t tmpKeycode,portA,portB,portC;

	portA= (GPIOA->IDR & 0x9901);//9901 //(GPIOA->IDR & 0x1901); //GPIO port input data register "IDR"
	portB =(GPIOB->IDR & 0x13);
	portC= (GPIOC->IDR & 0x8040);
	tmpKeycode =  portA & 0x01;				// Key9 ---PA0 ->          0B     0000  0001
	tmpKeycode |= ((portA & 0x0100)>>7);	// Key3 --PA8 ->           0B     0000  0010
	tmpKeycode |= ((portA & 0x1800)>>9);	// Key5,Key6,-->PA11,PA12  0B     0000  1100
    tmpKeycode |= ((portA & 0x8000)>>6);    //Key10--PA15              0B  10 0000  0000
	tmpKeycode |= ((portB & 0x0003)<<4);	// Key1,Key2-->PB0 ,PB1 -->0B     0011  0000
	tmpKeycode |= ((portB & 0x0010)<<2);	// Key7  --PB4->           0B     0100  0000 
	tmpKeycode |= ((portC & 0x0040)<<1);	// Key4  --PC6->           0B     1000  0000
	tmpKeycode |= ((portC & 0x8000)>>7);	// Key8 ---->PC15          0B  01 0000  000
	tmpKeycode &= NO_KEY;

	return tmpKeycode;
}
/*
*intitial keyInit 
*/
void keyInit(void)
{
	glKey.keyCode=NO_KEY;
	glKey.status=KEY_STATUS_NOPRESSED;
	glKey.long_pressed=0;
	glKey.multi_pressed=0;
	filter_t.filterCallBack_flag=1;
	retrieveSavedParameter(&echoUnion,&echoFilter,&echoLight,&echoLight_LR,&echoLight_AU,&echoGroup);
	
	printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
	
	HAL_TIM_Base_Start_IT(&htim17);
	
}

pKeyStruct getKey(void)
{
	return &glKey;
}

void startTimeDown(uint8_t en)
{
	counter_Waitingtime=0; //>120s,timer17 count 
	timerDownFlag=en; //> 120s, timerDownFlag=0 ->counter_Waitingtime=0->;
}
/**********************************************************************************
**
*Function Name:void updateKeyStatus(void)
*Function: in timer callback funciton process 2ms 
*
*
***********************************************************************************/
void updateKeyStatus(void)
{
  uint16_t tmpKey;
  static uint16_t preKeycode;	//save the previous keycode
  static uint16_t keyDownCount;	//count the Key down time
  static uint16_t typematicRate;
  static uint8_t keyUpCount;	//count the Key up time
  static uint8_t keyPressedFlag;//have a key pressed flag

  tmpKey=scanKey();
  if(tmpKey==NO_KEY)
  {
	  keyDownCount=0;	// reset key-down counter
	  typematicRate=0;
	  if(keyPressedFlag)
      {
		  keyUpCount++;
		  if(keyUpCount>KEY_USEFUL_DELAY)
		  {
			  keyPressedFlag=0;
			  glKey.keyCode=NO_KEY;
			  glKey.status=KEY_STATUS_NOPRESSED;
		  }
		  else if(keyUpCount>KEY_JITTER_DELAY)
		  {
			  //keyPressedFlag=0;
			  //keyUpCount=0;
			  glKey.keyCode=preKeycode;
			  glKey.status=KEY_STATUS_UP;
			  preKeycode=NO_KEY;
		  }
      }
	  else
	  {
    	  //keyInit();
		  glKey.long_pressed=0;
		  glKey.multi_pressed=0;
      }
  }
  else
  {
	  keyUpCount=0;	// reset key-up counter
	  counter_15m=0;	// reset 15m timeout
	  counter_ActionDelay=0;	// reset the counter
      if(tmpKey==preKeycode)
      {
    	  keyDownCount++;
    	  if(keyDownCount>KEY_JITTER_DELAY)
    	  {
    		  keyPressedFlag=1;
			  glKey.keyCode = tmpKey;
			  if(glKey.status!=KEY_STATUS_FREEZE)
			  {
				  glKey.status=KEY_STATUS_DOWN;
			  }
			  if(keyDownCount>KEY_MULTI_DELAY)
			  {
				  typematicRate++;
				  if(typematicRate>KEY_TYPEMATIC_RATE)
				  {
					  typematicRate=0;
					  glKey.status=KEY_STATUS_DOWN;
				  }
				  glKey.multi_pressed=1;
			  }
			  if(keyDownCount>KEY_LONG_DELAY)//1500 * 2ms 
			  {
				  glKey.long_pressed=1;
				  keyDownCount--;
			  }
    	  }
      }
      else
      {
		  preKeycode=tmpKey;
		  keyPressedFlag=0;
		  keyDownCount=0;
		  typematicRate=0;
		  glKey.keyCode=NO_KEY;
		  glKey.status=KEY_STATUS_NOPRESSED;
		  glKey.long_pressed=0;
		  glKey.multi_pressed=0;
      }
  }
}

/*****************************************************************************************
***
*Function Name:void AuxiliaryWhichOneLed(void)
*Function : auxiliary board execute which sub item which one led 
*Input Ref:NO
*Return Ref:NO
*
*******************************************************************************************/
void AuxiliaryWhichOneLed_Plus(uint8_t wled)
{
   switch(wled){

	case Main:
          
   break;

	case Spot:
		  
		  if(auxiliary_t.subSubItemLed_Num>=MAX_SPOT_NUMBER-1){
		  	   
				auxiliary_t.subSubItemLed_Num=0;
		  	}
			else {
				
				auxiliary_t.subSubItemLed_Num++;
			}
		   auxiliary_t.subsubItem_spot=1;
		   auxiliary_t.subsubItem_side=0;
		   auxiliary_t.subsubItem_left=0;
		   auxiliary_t.subsubItem_right=0;
     break;

	case Side:
		if(auxiliary_t.subSubItemLed_Num>=MAX_SIDE_NUMBER-1) {
		
			auxiliary_t.subSubItemLed_Num=0;
		}
		else{
			
			auxiliary_t.subSubItemLed_Num++;
		}
		auxiliary_t.subsubItem_spot=0;
		auxiliary_t.subsubItem_side=1;
		auxiliary_t.subsubItem_left=0;
		auxiliary_t.subsubItem_right=0;
		
	break;

	case Left:
		if(auxiliary_t.subSubItemLed_Num>=MAX_LEFT_NUMBER-1){
			auxiliary_t.subSubItemLed_Num=0;
		}
		else{
			auxiliary_t.subSubItemLed_Num++;
			}
		   auxiliary_t.subsubItem_spot=0;
		   auxiliary_t.subsubItem_side=0;
		   auxiliary_t.subsubItem_left=1;
		   auxiliary_t.subsubItem_right=0;
	break;

	case Right:
		if(auxiliary_t.subSubItemLed_Num>=MAX_RIGHT_NUMBER-1)
			auxiliary_t.subSubItemLed_Num=0;
		else auxiliary_t.subSubItemLed_Num++;

		auxiliary_t.subsubItem_spot=0;
		 auxiliary_t.subsubItem_side=0;
		 auxiliary_t.subsubItem_left=0;
		 auxiliary_t.subsubItem_right=1;

	break;

	default :
	  

	break;
  }

}
/*****************************************************************************************
***
*Function Name:void AuxiliaryWhichOneLed(void)
*Function : auxiliary board execute which sub item which one led 
*Input Ref:NO
*Return Ref:NO
*
*******************************************************************************************/
void AuxiliaryWhichOneLed_Reduce(uint8_t wled)
{
	uint8_t temp;
	switch(wled){

   case Main:
		
          
   break;

   case Spot: //01
		 
		  if(auxiliary_t.subSubItemLed_Num==0) auxiliary_t.subSubItemLed_Num=MAX_SPOT_NUMBER-1;
		  else auxiliary_t.subSubItemLed_Num--;
		 auxiliary_t.subsubItem_spot=1;
		 auxiliary_t.subsubItem_side=0;
		 auxiliary_t.subsubItem_left=0;
		 auxiliary_t.subsubItem_right=0;
	
         
	break;

	case Side: //02
		
		if(auxiliary_t.subSubItemLed_Num==0) auxiliary_t.subSubItemLed_Num=MAX_SIDE_NUMBER-1;
		else auxiliary_t.subSubItemLed_Num--;
		auxiliary_t.subsubItem_spot=0;
		 auxiliary_t.subsubItem_side=1;
		 auxiliary_t.subsubItem_left=0;
		 auxiliary_t.subsubItem_right=0;
	break;

	case Left: //03
		
		if(auxiliary_t.subSubItemLed_Num ==0) auxiliary_t.subSubItemLed_Num=MAX_LEFT_NUMBER-1;
		else auxiliary_t.subSubItemLed_Num--;
		 auxiliary_t.subsubItem_spot=0;
		 auxiliary_t.subsubItem_side=0;
		 auxiliary_t.subsubItem_left=1;
		 auxiliary_t.subsubItem_right=0;
	break;

	case Right://04
		
		if(auxiliary_t.subSubItemLed_Num == 0) auxiliary_t.subSubItemLed_Num=MAX_RIGHT_NUMBER-1;
		else auxiliary_t.subSubItemLed_Num--;
		auxiliary_t.subsubItem_spot=0;
		 auxiliary_t.subsubItem_side=0;
		 auxiliary_t.subsubItem_left=0;
		 auxiliary_t.subsubItem_right=1;

	break;

	default :
	  

	break;
  }



}
/*****************************************************************************************
***
*Function Name:void handleInput(void)
*Function : be dected key input status
*Input Ref:NO
*Return Ref:NO
*
*******************************************************************************************/
void handleInput(void)
{
	static uint8_t keySmartflag;
	static uint8_t power=0;
	uint8_t temp;
	pKeyStruct pkey=getKey();

	if(_250msFlag)
	{
		_250msFlag=0;
		if(auxiliary_t.Auxiliary_flag==0)
		     if(getEchoFilterBlink()==ENABLE_BLINK) blinkEchoFilter(echoFilter);
		
	}

	if(checkParameterFlag_500ms)////if(checkParameterFlag) //1s--update UART1 and UART2 data
	{
		checkParameterFlag_500ms=0;//checkParameterFlag=0;
		updateParameter(echoUnion,echoLight,echoLight_LR,auxiliary_t.subSubItemLed_Num,echoFilter); //blue tooth --USART1
		//updateParameter -> UART2,UART1 Transmit interrupt process
		if(power < 6){
			 power++;
		     if(power==5)
		       setCurrentLightOn(); //WT.EDIT 2021.05.08 main board led
		    
		}
	}

	if(waitingTimeoutFlag)	//120s  the motor board timeout
	{
		waitingTimeoutFlag=0;
		stopSelectFilter(); //UART_Transmit_IT--5 byte //4D 58 4D 53 30 
		setEchoFilterBlink(DISABLE_BLINK);
		
		
		if (auxiliary_t.Auxiliary_flag==1){
			
			printSettingInfo_Auxiliary(echoUnion_manual,echoFilter,echoLight_LR,BLINK_OFF);//WT.EDIT 2021.06.04
			 
		}
		else
			printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
	}

	if(timeoutFlag) //15 minute 
	{
		timeoutFlag=0;
		turnoffAllLight();
	}
	if(pkey->status==KEY_STATUS_DOWN)
	{
		pkey->status=KEY_STATUS_FREEZE;

		if(!(pkey->keyCode & KEY_CODE_KEY2))	// change light +  //LED  
		{
			
		     mainled_t.MainSpotUnion_Led=0;
			 
			  if(auxiliary_t.Auxiliary_flag==1){ //switch auxiliary board 
					filter_t.filterInKey=0;
					if(auxiliary_t.AuxiliarySubItem != Main){ //"Manual Menu" -'SPOT,SIDE ,LEFT, RIGHT'
							
						turnoffAllLight();//WT.EDIT 2021.06.03
						HAL_Delay(200);
						AuxiliaryWhichOneLed_Plus(auxiliary_t.AuxiliarySubItem);
						printSettingInfo_Auxiliary(echoUnion,echoFilter,auxiliary_t.subSubItemLed_Num,BLINK_OFF); //echoLight = LED Name 
		            
					}
					else{
						turnoffAllLight();//WT.EDIT 2021.06.03
						HAL_Delay(200);
						if(mainled_t.MainLed_Num>=MAX_LIGHT_NUMBER-1) {
							echoLight=0;
							mainled_t.MainLed_Num=0;
						}
						else {
							echoLight++;
							mainled_t.MainLed_Num++;
						}
						echoGroup=ECHO_GROUP_A;
						
						printSettingInfo_Auxiliary(echoUnion,echoFilter,echoLight,BLINK_OFF); //echoLight = LED Name 
					
					}
			}

		}
		else if(!(pkey->keyCode & KEY_CODE_KEY1))	// change light - //LED
		{
		
			 mainled_t.MainSpotUnion_Led=0;
		
			if(auxiliary_t.Auxiliary_flag==1){ //switch auxiliary board change light + spot and lin
				filter_t.filterInKey=0;
				if(auxiliary_t.AuxiliarySubItem != Main){

				    turnoffAllLight();//WT.EDIT 2021.06.03
					HAL_Delay(200);
					AuxiliaryWhichOneLed_Reduce(auxiliary_t.AuxiliarySubItem);
				    printSettingInfo_Auxiliary(echoUnion,echoFilter,auxiliary_t.subSubItemLed_Num,BLINK_OFF); //echoLight = LED Name 
					
				}
			   else{ //Main Board
			  		//mainLed switch
			        turnoffAllLight();//WT.EDIT 2021.06.03
					HAL_Delay(200);
					if(mainled_t.MainLed_Num==0){
						echoLight=MAX_LIGHT_NUMBER-1;
						mainled_t.MainLed_Num =MAX_LIGHT_NUMBER-1;
					}
					else{
						echoLight--;
						mainled_t.MainLed_Num --;
					}
					echoGroup=ECHO_GROUP_A;
					
					//printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
					//printSettingInfo_MainLed(echoUnion,echoFilter,echoLight,BLINK_OFF); //echoLight = LED Name 
				    printSettingInfo_Auxiliary(echoUnion,echoFilter,echoLight,BLINK_OFF);
			   }
		   }
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY4))	// change filter +  //Only "Manul" Menu
		{
			
			   
			   if (auxiliary_t.Auxiliary_flag == 1){
					 filter_t.filterInKey=1;
					  mainled_t.MainSpotUnion_Led=0;
				 if (echoFilter >= MAX_FILTER_NUMBER - 1)
					    echoFilter = 0;
				 else
					   echoFilter++;
				 
				//  if(filter_t.filterSendData_flag ==1)
				  {
					 filter_t.filterSendData_flag =0;
				    printSettingInfo_Auxiliary(echoUnion, echoFilter, echoLight, BLINK_OFF);
				  }
				
			 }
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY3))	// change filter -
		{
	
			 
			
			if (auxiliary_t.Auxiliary_flag == 1)
			 {
					filter_t.filterInKey=1;
					mainled_t.MainSpotUnion_Led=0;
				 if (echoFilter == 0)
					 echoFilter = MAX_FILTER_NUMBER - 1;
				 else
					 echoFilter--;
				   
				// if(filter_t.filterSendData_flag ==1)
				 {
				  	filter_t.filterSendData_flag =0;
				    printSettingInfo_Auxiliary(echoUnion, echoFilter, echoLight, BLINK_OFF);
				 }
			}
		}
		//Smart Mode
		else if(!(pkey->keyCode & KEY_CODE_KEY5))	// change union + "Smart Button"
		{
			 
			 if (auxiliary_t.Auxiliary_flag == 0)
			 {
				mainled_t.MainSpotUnion_Led=1;
				 if (echoUnion >= MAX_UNION_NUMBER - 1){
					  echoUnion = 0;
					  mainled_t.ledoff_flag =0;
				 }
				 else{
					 echoUnion++;
					 mainled_t.ledoff_flag++;
				    #if DEBUG
				      HAL_UART_Transmit(&CMD_LINKER,&echoUnion,1,2);
					#endif
				 }
				 turnoffAllLight();
				 HAL_Delay(200);
				 displayUnionInfo(echoUnion);
		
				 if(echoUnion==4){ //display "#5"
					    mainled_t.ledoff_flag=4;
					   
 					}
					else if(echoUnion==5){
					 mainled_t.ledoff_flag=5; //display '#6'
					  
					}
					else if(echoUnion ==19){//display '#20'			
				
					    mainled_t.ledoff_flag=19;
					}
					else if(echoUnion== 20){
					    mainled_t.ledoff_flag=20;
						
					  }
					else{
					  if(echoUnion ==2)//oled display "#3"
					      mainled_t.Same_23 =0;
					  else if(echoUnion ==3) 
					  	 mainled_t.Same_23 =1;
					     mainled_t.MainUnionSport_flag =0;
				    }

			}
		}
		//Smart Mode 
		else if(!(pkey->keyCode & KEY_CODE_KEY6))	// change union - //"smart Button"
		{
			
		      
			 if (auxiliary_t.Auxiliary_flag == 0)
			 {
				mainled_t.MainSpotUnion_Led=1;
				 if (echoUnion == 0){
					 echoUnion = MAX_UNION_NUMBER - 1;
					 mainled_t.ledoff_flag =MAX_UNION_NUMBER - 1;

				 }
				 else{
					  echoUnion--;
					  mainled_t.ledoff_flag--;
				     // HAL_UART_Transmit(&CMD_LINKER,&echoUnion,1,2);
				 }
				 turnoffAllLight();
				 HAL_Delay(200);
				 displayUnionInfo(echoUnion);
				
				  if(echoUnion==4){ //display "#5"
					    mainled_t.ledoff_flag=4;
					   

					}
					else if(echoUnion==5){
					  mainled_t.ledoff_flag=5; //display '#6'
					  
					  
					}
					else if(echoUnion ==19){//white					
				
					    mainled_t.ledoff_flag=19; //"#20"
						
					   
                    }
					else if(echoUnion== 20){
					    mainled_t.ledoff_flag=20;
						
				

					}
					else{
					  mainled_t.MainUnionSport_flag =0;
					  if(echoUnion ==2)//oled display "#3"
					      mainled_t.Same_23 =0;
					  else if(echoUnion ==3)  mainled_t.Same_23 =1;
					

				
				 	}
				 
			 }
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY7))	// turn off light
		{
		
			
			if(getLightOnoffState()==NOW_LIGHT_IS_ON)  //if(keyTurnOnflag ==1)
			{
				 filter_t.filterInKey=0;
				turnoffAllLight();
				HAL_Delay(200);
			}
			else
			{
                
			     if(auxiliary_t.Auxiliary_flag==0 ){//WT.EDIT	2021.06.02
			         
							   
					if(mainled_t.MainUnionSport_flag ==1){
							mainled_t.ledoff_flag =mainled_t.ledoff_flag-5;
							TurnOnUnionSPOT_Light();
					}
					else
						setCurrentLightOn();
				}
			     else{ 
				 	 filter_t.filterInKey=0;
					if(auxiliary_t.AuxiliarySubItem==Main){
							setCurrentLightOn();
					}
					else 
					   setCurrentLightOn_AU(); //WT.EDIT 2021.06.02

			     }
				
				
			}
			reportLightStatusChange();
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY8))	// brightness adj -
		{
			 filter_t.filterInKey=0;
			 if(auxiliary_t.Auxiliary_flag==0){
			 	if(mainled_t.MainUnionSport_flag ==1 && (mainled_t.unionNum !=5 && mainled_t.unionNum !=6))
				{
					brightnessAdj_AuxiliaryLed(BRIGHTNESS_ADJ_DOWN);
				}
			    else if( mainled_t.unionNum!=5 && mainled_t.unionNum !=6)
			    	brightnessAdj(BRIGHTNESS_ADJ_DOWN);
			 }
			 else{
				 
				if(auxiliary_t.AuxiliarySubItem != Main)
				   brightnessAdj_AuxiliaryLed(BRIGHTNESS_ADJ_DOWN);
				else{
					
					   brightnessAdj(BRIGHTNESS_ADJ_DOWN);
				}

			 }
			//motionCtrl(MOTION_CCW);
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY9))	// brightness adj +
		{
			 filter_t.filterInKey=0;
			 if(auxiliary_t.Auxiliary_flag==0){
			 		if(mainled_t.MainUnionSport_flag ==1 && (mainled_t.unionNum !=5 && mainled_t.unionNum !=6))
					{
						brightnessAdj_AuxiliaryLed(BRIGHTNESS_ADJ_DOWN);
					}
					else if(mainled_t.unionNum !=5 && mainled_t.unionNum !=6)
			       		brightnessAdj(BRIGHTNESS_ADJ_UP);
			 }
			 else {

				 if(auxiliary_t.AuxiliarySubItem != Main)
					 brightnessAdj_AuxiliaryLed(BRIGHTNESS_ADJ_UP);
				 else {
					
						
						  brightnessAdj(BRIGHTNESS_ADJ_UP);
				 }

			 }
			//motionCtrl(MOTION_CW);
		}
	    else if(!(pkey->keyCode & KEY_CODE_KEY10))	// Switch auxiliary BOARD Menu button WT.EDIT 
		{
             
			  	    mainled_t.MainSpotUnion_Led=0;
					keySmartflag = keySmartflag ^ 0x1;
					if(keySmartflag ==1 ){ //ManualMode
					
					    auxiliary_t.SmartMenuItem=1;
						auxiliary_t.ManualMode =1;
						auxiliary_t.AuxiliarySubItem=Main ;
						LedMainNumber=0;//WT.EDIT 2021.06.01
						auxiliary_t.filterID=0;  //WT.EDIT 2021.06.01
				    
						auxiliary_t.Auxiliary_flag=1;
						mainled_t.MainLed_Num=0;
						mainled_t.SW_Mode =1;
                        auxiliary_t.subMenuOne=0;//WT.EDIT 2021.06.30
						echoGroup=ECHO_GROUP_A;
						turnoffAllLight();
						
						printSettingInfo_Auxiliary(echoUnion_manual,echoFilter,auxiliary_t.AuxiliarySubItem,BLINK_OFF);
					}
					else { //"Smart "
 						 auxiliary_t.SmartMenuItem =0; //default "Smart Mode "
					     auxiliary_t.SmartMenuItem=0;
					     auxiliary_t.ManualMode=0;
					     auxiliary_t.Auxiliary_flag=0;
						  auxiliary_t.subMenuOne++;
						  filter_t.filterInKey=0;
						  turnoffAllLight();
						  displayUnionInfo(echoUnion);
						  mainled_t.MainSpotUnion_Led=1;
						  if( mainled_t.ledoff_flag >0)//WT.EDIT 2021.06.02
						  	mainled_t.ledoff_flag =mainled_t.ledoff_flag-5;
						 
						}
			       
		
		}
	
	    
	}
	else if(pkey->status==KEY_STATUS_UP)
	{
		pkey->status=KEY_STATUS_NOPRESSED;
		if(!(pkey->keyCode & KEY_CODE_KEY1))
		{
			//sendMotorCmd(MOTOR_CMD_STOP,MOTOR_SPEED_NORMAL,MOTOR_DIR_CW);
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY2))
		{
			//sendMotorCmd(MOTOR_CMD_STOP,MOTOR_SPEED_NORMAL,MOTOR_DIR_CW);
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY3))
		{
			//sendMotorCmd(MOTOR_CMD_STOP,MOTOR_SPEED_NORMAL,MOTOR_DIR_CW);
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY4))
		{
			//sendMotorCmd(MOTOR_CMD_STOP,MOTOR_SPEED_NORMAL,MOTOR_DIR_CW);
		}
		else
		{

		}
	}
}

void trigParameterUpdateImmediate(void)
{
	checkParameterFlag=1;
	counter_15m=(uint32_t)0L;
}

uint8_t retrieveEchoFilter(void)
{
	return echoFilter;
}

uint8_t retrieveEchoLight(void)
{
	return echoLight;
}

uint8_t retrieveEchoUnion(void)
{
	return echoUnion;
}

uint8_t retrieveEchoGroup(void)
{
	return echoGroup;
}

void setEchoFilter(uint8_t filterIndex)
{
	echoFilter=filterIndex;
	printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
}

void setEchoLight(uint8_t lightIndex)
{
	echoLight=lightIndex;
	printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
}

void setEchoUnion(uint8_t unionIndex)
{
	echoUnion=unionIndex;
	displayUnionInfo(echoUnion);
}

static void displayUnionInfo(uint8_t unionIndex)
{
	echoGroup=ECHO_GROUP_B;
	getItemFromUnion(unionIndex,&echoFilter,&echoLight);
	printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
	
}
/*****************************************************************************************************
	**
	*Function Name:void displayUnionInfo_Menu(uint8_t unionIndex)
	*Function : display auxiliary "menu" --LED 
	*
	*
	*
******************************************************************************************************/

/*****************************************************************************************************
	**
	*Function Name:static void TurnOnUnionSPOT_Light(void)
	*Function : 
	*Input Ref:
	*Return Ref:
	*
******************************************************************************************************/
void TurnOnUnionSPOT_Light(void)
{
       
		switch(mainled_t.ledoff_flag){

		  case 4: //"#5"
		         selectLight_SpotBoard(2);
		         mainled_t.ledoff_flag =mainled_t.ledoff_flag+5;
				 mainled_t.MainUnionSport_flag =1;
				 mainled_t.unionNum =5;
				 
		      break;

		   case 5: //"#6"
                selectLight_SpotBoard(3);
				mainled_t.ledoff_flag =mainled_t.ledoff_flag+5;
				 mainled_t.MainUnionSport_flag =1;
				 mainled_t.unionNum =6;
				break;

			case 19: //display "#20"
		         selectLight_LinearBoard(3);
				 mainled_t.ledoff_flag =mainled_t.ledoff_flag+5;
				 mainled_t.MainUnionSport_flag =1;
			  
				break;

			case 20: //display "#21"
			    selectLight_LinearBoard(1);
				mainled_t.ledoff_flag =mainled_t.ledoff_flag+5;
				mainled_t.MainUnionSport_flag =1;
			  break;

			default :

			 

			break;
          }
         
		

}
/*****************************************************************************************************
	**
	*Function Name:SideButton_SubItem_Input(void)
	*Function : 
	*
	*
	*
******************************************************************************************************/
uint8_t SideButton_SubItem_Input(void)
{
	static uint16_t k11=0;
	static uint8_t cnt;
	uint8_t value=0;
	if(HAL_GPIO_ReadPin(KEY11_GPIO_Port, KEY11_Pin)==0){

        if(k11<10000)
		cnt=0;
		k11++;

    }
	else{
          k11=0;
  
	}
    
    if(k11==9990){
	   	 value = 1;
	    k11=0;
	    return value;

    }
    return 0;
}
/*****************************************************************************************************
	**
	*Function Name:SideButtonSub_KEY(void)
	*Function : GPIO -SWDIO for update GPIO 
	*Input Ref:
	*Return Ref:
	*
******************************************************************************************************/
void SideButtonSub_KEY(void)
{
	 if(SideButton_SubItem_Input()){


			   turnoffAllLight();
			   HAL_Delay(200);
			   if(auxiliary_t.ManualMode ==1){ //manual mode "SPOT" "SIDE" "LEFT" "RIGHT"
				
		   
			   if(auxiliary_t.AuxiliarySubItem>=MAX_LIGHT_LR_NUMBER-1){
				   auxiliary_t.AuxiliarySubItem=Main;
			   
				  
				  }
			   else{ 
				   
				
				    auxiliary_t.AuxiliarySubItem ++;
		   
			  }
			   
			   
				printSettingInfo_Auxiliary(echoUnion_manual,echoFilter,auxiliary_t.subSubItemLed_Num,BLINK_OFF);	   
			   }

		}
}

/*****************************************************************************************************
	**
	*Function Name:void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	*Function : timer of callback function ,timer of times is 2ms
	*
	*
	*
******************************************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	// 2ms
{
	static uint8_t tick_250ms=0;
	static uint16_t tick_500ms=0;
	

	tick_250ms++;
	tick_500ms++;
	counter_ActionDelay++;
	counter_15m++;

	if(tick_250ms>125)
	{
		_250msFlag=1;
		tick_250ms=0;
		
		
	}
	if(tick_500ms>250){ //250*2 =500ms
		tick_500ms=0;
		_500msFlag=1;
	   checkParameterFlag_500ms=1;
	}
	
	if(counter_ActionDelay>500)//500 * 2ms = 1000ms =1s
	{
		counter_ActionDelay=0;
		checkParameterFlag=1;
	}
	if(counter_15m >= (uint32_t)450000L) //15 minute 
	{
		timeoutFlag=1;
		counter_15m=(uint32_t)0L;
	}
	if(timerDownFlag)
	{
		counter_Waitingtime++;
		if(counter_Waitingtime>=60000) //2ms * 60000=120s 
		{
			timerDownFlag=0;
			waitingTimeoutFlag=1;
		}
	}
	else
	{
		counter_Waitingtime=0;
	}
	updateKeyStatus(); //key scan every 2ms be detect
}


