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



static uint16_t scanKey(void);
static void displayUnionInfo(uint8_t unionIndex);

static KeyStruct glKey;				// key for forward/backward
static uint8_t echoFilter;
static uint8_t echoLight;
static uint8_t echoUnion;
static uint8_t echoGroup;
static uint8_t timerDownFlag;
static uint32_t counter_15m;
static uint16_t counter_ActionDelay;
static uint16_t counter_Waitingtime;
static uint8_t  waitingTimeoutFlag;
static uint8_t  timeoutFlag;
static uint8_t checkParameterFlag;
static uint8_t	_250msFlag;



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

	portA= (GPIOA->IDR & 0x1901); //GPIO port input data register "IDR"
	portB =(GPIOB->IDR & 0x13);
	portC= (GPIOC->IDR & 0x8040);
	tmpKeycode =  portA & 0x01;				// Key9 ---PA0
	tmpKeycode |= ((portA & 0x0100)>>7);	// Key3 --PA8
	tmpKeycode |= ((portA & 0x1800)>>9);	// Key5,Key6
	tmpKeycode |= ((portB & 0x0003)<<4);	// Key1,Key2
	tmpKeycode |= ((portB & 0x0010)<<2);	// Key7
	tmpKeycode |= ((portC & 0x0040)<<1);	// Key4
	tmpKeycode |= ((portC & 0x8000)>>7);	// Key8
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
	retrieveSavedParameter(&echoUnion,&echoFilter,&echoLight,&echoGroup);
	//printEchoUnion(echoUnion,echoFilter,echoLight);
	//printEchoFilter(echoFilter);
	//printEchoLight(echoLight);
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
			  if(keyDownCount>KEY_LONG_DELAY)
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

	tick_250ms++;
	counter_ActionDelay++;
	counter_15m++;

	if(tick_250ms>125)
	{
		_250msFlag=1;
		tick_250ms=0;
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
	pKeyStruct pkey=getKey();

	if(_250msFlag)
	{
		_250msFlag=0;
		if(getEchoFilterBlink()==ENABLE_BLINK) blinkEchoFilter(echoFilter);
		//else	;
		//blinkLed();
	}

	if(checkParameterFlag) //1s
	{
		checkParameterFlag=0;
		updateParameter(echoUnion,echoLight,echoFilter); //blue tooth --USART1
		//updateParameter -> UART2,UART1 Transmit interrupt process
	}

	if(waitingTimeoutFlag)	//120s  the motor board timeout
	{
		waitingTimeoutFlag=0;
		stopSelectFilter(); //UART_Transmit_IT--5 byte //4D 58 4D 53 30 
		setEchoFilterBlink(DISABLE_BLINK);
		printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
		//printEchoFilter(echoFilter);
		updateLight(echoLight);  //LED number turn on or off
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
			if(echoLight>=MAX_LIGHT_NUMBER-1) echoLight=0;
			else echoLight++;
			echoGroup=ECHO_GROUP_A;
			printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF); //echoLight = LED Name 
			//printEchoLight(echoLight);
			// turn on cw slowly
			//sendMotorCmd(MOTOR_CMD_RUN,MOTOR_SPEED_NORMAL,MOTOR_DIR_CW);

		}
		else if(!(pkey->keyCode & KEY_CODE_KEY1))	// change light -
		{
			if(echoLight==0) echoLight=MAX_LIGHT_NUMBER-1;
			else echoLight--;
			echoGroup=ECHO_GROUP_A;
			printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
			//printEchoLight(echoLight);
			// turn on cw quickly
			//sendMotorCmd(MOTOR_CMD_RUN,MOTOR_SPEED_HIGH,MOTOR_DIR_CW);
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY4))	// change filter +  
		{
			if(echoFilter>=MAX_FILTER_NUMBER-1) echoFilter=0;
			else echoFilter++;
			echoGroup=ECHO_GROUP_A;
			printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
			//printEchoFilter(echoFilter);
			// turn on ccw quickly
			//sendMotorCmd(MOTOR_CMD_RUN,MOTOR_SPEED_HIGH,MOTOR_DIR_CCW);

		}
		else if(!(pkey->keyCode & KEY_CODE_KEY3))	// change filter -
		{
			if(echoFilter==0) echoFilter=MAX_FILTER_NUMBER-1;
			else echoFilter--;
			echoGroup=ECHO_GROUP_A;
			printSettingInfo(echoUnion,echoFilter,echoLight,BLINK_OFF);
			//printEchoFilter(echoFilter);
			// turn on ccw slowly
			//sendMotorCmd(MOTOR_CMD_RUN,MOTOR_SPEED_NORMAL,MOTOR_DIR_CCW);
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY5))	// change union + //smart button +
		{
			if(echoUnion>=MAX_UNION_NUMBER-1) echoUnion=0;
			else echoUnion++;

			displayUnionInfo(echoUnion);
			// turn on ccw slowly
			//sendMotorCmd(MOTOR_CMD_RUN,MOTOR_SPEED_NORMAL,MOTOR_DIR_CCW);
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY6))	// change union - //smart button-
		{
			if(echoUnion==0) echoUnion=MAX_UNION_NUMBER-1;
			else echoUnion--;

			displayUnionInfo(echoUnion);
			// turn on ccw slowly
			//sendMotorCmd(MOTOR_CMD_RUN,MOTOR_SPEED_NORMAL,MOTOR_DIR_CCW);
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY7))	// turn off light
		{
			if(getLightOnoffState()==NOW_LIGHT_IS_ON)
			{
				turnoffAllLight();
			}
			else
			{
				setCurrentLightOn();
			}
			reportLightStatusChange();
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY8))	// brightness adj -
		{
			brightnessAdj(BRIGHTNESS_ADJ_DOWN);
			//motionCtrl(MOTION_CCW);
		}
		else if(!(pkey->keyCode & KEY_CODE_KEY9))	// brightness adj +
		{
			brightnessAdj(BRIGHTNESS_ADJ_UP);
			//motionCtrl(MOTION_CW);
		}
		else
		{

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
	//printEchoUnion(unionIndex,echoFilter,echoLight);
	//printEchoFilter(echoFilter);
	//printEchoLight(echoLight);
}
