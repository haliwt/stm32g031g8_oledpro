/*
 * cmd_link.c
 *
 *  Created on: Sep 23, 2020
 *      Author: om
 */

#include "cmd_link.h"
#include "oled.h"

#define CMD_LINKER	huart2
#define BLE_USART	huart1
#define USART_RX_TIMEOUT	5000	// 2s
#define USART_TX_TIMEOUT	5000	// 1s

#define BOARD_ADDR		0x43	// 'C' control board
#define BOARD_ADDR_BT	0x42	// 'B' for BT link

#define MAX_BUFFER_SIZE				32
#define MAX_CMD_PARA_SIZE			8

//#define MAX_FILTER_NUMBER	10

#define STATE_PREAMBLE1	0	// 'M'	fixed
#define STATE_PREAMBLE2	1	// 'X'	fixed
#define STATE_ADDR		2	// 'M' motor board , 'L' light board
#define STATE_CMD		3	// control command
#define STATE_SIZE		4	// length of command parameter
#define STATE_PARA		5	// parameter
#define STATE_CRC		6	// checksum

typedef struct settingUnion
{
	uint8_t light;
	uint8_t filter;
} settingUnion_t;

extern void startTimeDown(uint8_t en);
extern void setEchoFilter(uint8_t filterIndex);
extern void setEchoLight(uint8_t lightIndex);
extern void setEchoUnion(uint8_t unionIndex);
extern void trigParameterUpdateImmediate(void);


static void runCmd(void);
static void bleRunCmd(void);
static void initBtleModule(void);
static uint8_t checkBleModuleAVDData(void);
static void selectFilter(uint8_t index);
static void selectLight(uint8_t index);
static void notifyStatusToHost(uint8_t lightNum,uint8_t filterNum,uint8_t unionNum);
//static uint8_t suffBoardStatusToBuffer(void);
//static void getTargetStatus(uint8_t *pBuf);

static uint8_t currUnion,currLight,currFilter,tmpLight;
static uint8_t inputCmd[32],bleInputCmd[32];
static uint8_t cmdSize,bleCmdSize;
static uint8_t paraIndex,bleParaIndex;
static uint8_t crcCheck,bleCrcCheck;
static uint8_t state,bleState;
static uint8_t decodeFlag,bleDecodeFlag;
static uint8_t powerOnFlag;
static uint8_t nowLightState;
static uint8_t outputBuf[MAX_BUFFER_SIZE],bleOutputBuf[MAX_BUFFER_SIZE];
static uint8_t inputBuf[MAX_BUFFER_SIZE],bleBuf[MAX_BUFFER_SIZE];
static uint8_t transferSize,bleTransferSize;

static uint8_t bleTarget;
static uint8_t bleIndex;

static uint8_t swStr[3]={"+++"};
static uint8_t resetCmd[]={"AT+RESET"};
static uint8_t getAdvDataCmd[]={"AT+ADVDATA"};
static uint8_t advData[]={"AT+ADVDATA=03FF03FF"};

volatile static uint8_t transOngoingFlag;
volatile static uint8_t bleTransOngoingFlag;

void cmdInit(void)
{
	transferSize=0;
	state=STATE_PREAMBLE1;
	decodeFlag=0;
	bleDecodeFlag=0;
	transOngoingFlag=0;
	bleTransOngoingFlag=0;
	powerOnFlag=1;
	nowLightState=NOW_LIGHT_IS_OFF;
	HAL_UART_Abort(&CMD_LINKER);
	HAL_UART_Abort(&BLE_USART);
	initBtleModule();
	HAL_UART_Receive_IT(&CMD_LINKER,inputBuf,1);
	HAL_UART_Receive_IT(&BLE_USART,bleBuf,1);
}

void decode(void)
{
	if(decodeFlag)
	{
		decodeFlag=0;
		runCmd();
	}

	if(bleDecodeFlag)
	{
		bleDecodeFlag=0;
		bleRunCmd();
	}
}


void updateParameter(uint8_t unionIndex,uint8_t lightIndex,uint8_t filterIndex)
{

	if(unionIndex!=currUnion || filterIndex !=currFilter || lightIndex!=currLight)
	{
		notifyStatusToHost(lightIndex,filterIndex,unionIndex);
	}

	if(unionIndex!=currUnion)
	{
		currUnion=unionIndex;
	}
	if(filterIndex!=currFilter)
	{
		currFilter=filterIndex;
		tmpLight=lightIndex;
		setEchoFilterBlink(ENABLE_BLINK);
		selectFilter(filterIndex);
		startTimeDown(1);
	}
	else
	{
		updateLight(lightIndex);
	}
}

//void updateFilter(filterIndex)
//{
//	if(filterIndex!=currFilter)
//	{
//		currFilter=filterIndex;
//		setEchoFilterBlink(ENABLE_BLINK);
//		selectFilter(filterIndex);
//	}
//}

void updateLight(uint8_t lightIndex)
{
	if(lightIndex!=currLight)
	{
		currLight=lightIndex;
		//setEchoLightBlink(ENABLE_BLINK);
		if(powerOnFlag) powerOnFlag=0;	//need not turn on light when power on
		else
		{
			selectLight(lightIndex);
			//nowLightState=NOW_LIGHT_IS_ON;
		}
	}
}

uint8_t getLightOnoffState(void)
{
	return nowLightState;
}

void setCurrentLightOn(void)
{
	if(currLight<MAX_LIGHT_NUMBER)
	{
		selectLight(currLight);
		//nowLightState=NOW_LIGHT_IS_ON;
	}
}

uint8_t retrieveSavedParameter(uint8_t *revealUnion,uint8_t *revealFilter,uint8_t *revealLight,uint8_t *revealGroup)
{
	currUnion=0xff;
	currFilter=0xff;
	currLight=0xff;

	*revealUnion=9;
	*revealFilter=0;
	*revealLight=0;
	*revealGroup=ECHO_GROUP_B;

	return CMD_SUCCESS;
}


uint8_t getItemFromUnion(uint8_t unionIndex,uint8_t *filterIndex,uint8_t *lightIndex)
{
	settingUnion_t settingTable[MAX_UNION_NUMBER]={	{5,6},{4,6},{1,5},{2,5},{2,3},
												  	{1,2},{3,4},{3,3},{3,6},{0,0},
													{9,9},{2,4},{1,2},{2,1},{3,3},
													{4,6},{3,4},{1,0},{3,8},{8,9}};
	if(unionIndex<MAX_UNION_NUMBER)
	{
		*filterIndex=settingTable[unionIndex].filter;
		*lightIndex=settingTable[unionIndex].light;
		return CMD_SUCCESS;
	}
	return CMD_ERROR;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&CMD_LINKER)
	{
		switch(state)
		{
		case STATE_PREAMBLE1:
			if(inputBuf[0]=='M')
				state=STATE_PREAMBLE2;
			break;
		case STATE_PREAMBLE2:
			if(inputBuf[0]=='X')
			{
				state=STATE_ADDR;
			}
			else
				state=STATE_PREAMBLE1;
			break;
		case STATE_ADDR:
			if(inputBuf[0]==BOARD_ADDR)
			{
				state=STATE_CMD;
			}
			else
				state=STATE_PREAMBLE1;
			break;
		case STATE_CMD:
			inputCmd[0]=inputBuf[0];
			crcCheck = 0x55 ^ inputCmd[0];
			//decodeFlag=1;
			state=STATE_SIZE;
			break;
		case STATE_SIZE:
			cmdSize=inputBuf[0]-0x30;
			if(cmdSize>MAX_CMD_PARA_SIZE)	// out of range
			{
				state=STATE_PREAMBLE1;
			}
			else if(cmdSize>0)
			{
				inputCmd[1]=cmdSize;
				paraIndex=2;
				crcCheck ^= inputBuf[0];
				state=STATE_PARA;
			}
			else	// no parameter
			{
				inputCmd[1]=0;
				crcCheck ^= inputBuf[0];
				decodeFlag=1;
				state=STATE_PREAMBLE1;
			}
			break;
		case STATE_PARA:
			crcCheck ^= inputBuf[0];
			inputCmd[paraIndex]=inputBuf[0];
			paraIndex++;
			cmdSize--;
			if(cmdSize==0)
			{
				decodeFlag=1;
				state=STATE_PREAMBLE1;
			}
			break;
		case STATE_CRC:
			//if((crcCheck ^ inputBuf[0])==0)
			{
				//decodeFlag=1;
			}
			state=STATE_PREAMBLE1;
			break;
		default:
			state=STATE_PREAMBLE1;
			decodeFlag=0;
		}
		HAL_UART_Receive_IT(&CMD_LINKER,inputBuf,1);
	}
	else if(huart==&BLE_USART)
	{
		switch(bleState)
		{
		case STATE_PREAMBLE1:
			if(bleBuf[0]=='M')
				bleState=STATE_PREAMBLE2;
			break;
		case STATE_PREAMBLE2:
			if(bleBuf[0]=='X')
			{
				bleState=STATE_ADDR;
			}
			else
				bleState=STATE_PREAMBLE1;
			break;
		case STATE_ADDR:
			if(bleBuf[0]==BOARD_ADDR_BT)
			{
				bleState=STATE_CMD;
			}
			else
				bleState=STATE_PREAMBLE1;
			break;
		case STATE_CMD:
			bleInputCmd[0]=bleBuf[0];
			bleCrcCheck = 0xAA ^ bleInputCmd[0];
			//decodeFlag=1;
			bleState=STATE_SIZE;
			break;
		case STATE_SIZE:
			bleCmdSize=bleBuf[0];
			if(bleCmdSize>MAX_CMD_PARA_SIZE)	// out of range
			{
				bleState=STATE_PREAMBLE1;
			}
			else if(bleCmdSize>0)
			{
				bleParaIndex=1;
				bleCrcCheck ^= bleCmdSize;
				bleState=STATE_PARA;
			}
			else	// no parameter
			{
				bleState=STATE_CRC;
			}
			break;
		case STATE_PARA:
			bleInputCmd[bleParaIndex]=bleBuf[0];
			bleCrcCheck ^= bleInputCmd[bleParaIndex];
			bleParaIndex++;
			bleCmdSize--;
			if(bleCmdSize==0) bleState=STATE_CRC;
			break;
		case STATE_CRC:
			if((bleCrcCheck ^ bleBuf[0])==0)
			{
				bleDecodeFlag=1;
			}
			bleState=STATE_PREAMBLE1;
			break;
		default:
			bleState=STATE_PREAMBLE1;
			bleDecodeFlag=0;
		}
		HAL_UART_Receive_IT(&BLE_USART,bleBuf,1);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&CMD_LINKER)
	{
		transOngoingFlag=0;
	}
	else if(huart==&BLE_USART)
	{
		bleTransOngoingFlag=0;	// reset busy flag
	}
}

static void runCmd(void)
{
	uint8_t cmdType=inputCmd[0];
	uint8_t ret;

	switch(cmdType)
	{
	case 'R':	// motor board return state
		ret=inputCmd[2]-0x30;
		if(ret==1)	// change filter finished
		{
			setEchoFilterBlink(DISABLE_BLINK);
			startTimeDown(0);
			printSettingInfo(currUnion,currFilter,tmpLight,BLINK_OFF);
			//printEchoFilter(currFilter);
			updateLight(tmpLight);
		}

		break;
	case 'S':	//

		break;
	default:
		break;
	}
}

static void bleRunCmd(void)
{
//	uint8_t transfeSize=0;
//	uint8_t ledGroup,ledIndex;
	uint8_t cmdType=bleInputCmd[0];

	//static uint8_t keyBR_Counts=0;
	switch(cmdType)
	{
	case 'L':	// 0x4C,led control command
		bleTarget=bleInputCmd[1];
		bleIndex=bleInputCmd[2];
		switch(bleTarget)
		{
		case 0:
			if(bleIndex<MAX_LIGHT_NUMBER)
			{
				setEchoLight(bleIndex);
			}
			else if(bleIndex==MAX_LIGHT_NUMBER)
			{
				if(nowLightState==NOW_LIGHT_IS_ON)
				{
					turnoffAllLight();
				}
				else
				{
					setCurrentLightOn();
				}
				notifyStatusToHost(((nowLightState==NOW_LIGHT_IS_ON) ? currLight : 0xff ),currFilter,currUnion);
				return;
			}
			break;
		case 1:
			if(bleIndex<MAX_FILTER_NUMBER)	setEchoFilter(bleIndex);
			else if(bleIndex==MAX_FILTER_NUMBER) brightnessAdj(BRIGHTNESS_ADJ_UP);
			else brightnessAdj(BRIGHTNESS_ADJ_DOWN);
			break;
		case 2:
		case 3:
			if(bleTarget==3) bleIndex+=8;
			if(bleIndex<MAX_UNION_NUMBER)
			{
				setEchoUnion(bleIndex);
			}
			break;
		default:
			break;
		}
		trigParameterUpdateImmediate();
		break;
	case 'G':	// 0x47,only get leds status
		notifyStatusToHost(((nowLightState==NOW_LIGHT_IS_ON) ? currLight : 0xff ),currFilter,currUnion);
		break;
	default:
		break;
	}
}

static void initBtleModule(void)
{
	uint8_t tryTimes=3;

	while(tryTimes)
	{
		HAL_UART_Abort(&BLE_USART);
		HAL_Delay(50);
		HAL_UART_Transmit(&BLE_USART,swStr,3,USART_TX_TIMEOUT);
		HAL_Delay(500);
		if(checkBleModuleAVDData())	tryTimes=0;
		else tryTimes--;
	}
	HAL_UART_Abort(&BLE_USART);
}

static uint8_t checkBleModuleAVDData(void)
{
	HAL_StatusTypeDef ret;

	HAL_UART_Abort(&BLE_USART);
	HAL_UART_Transmit(&BLE_USART,getAdvDataCmd,10,USART_TX_TIMEOUT);
	ret=HAL_UART_Receive(&BLE_USART,inputBuf,24,USART_RX_TIMEOUT);
	if(ret==HAL_OK)
	{
		if(inputBuf[20]=='F' && inputBuf[21]=='F')
		{
			// switch to normal mode
			HAL_UART_Transmit(&BLE_USART,swStr,3,USART_TX_TIMEOUT);
		}
		else
		{
			HAL_UART_Transmit(&BLE_USART,advData,19,USART_TX_TIMEOUT);
			HAL_Delay(50);
			HAL_UART_Transmit(&BLE_USART,resetCmd,8,USART_TX_TIMEOUT);
			HAL_Delay(2000);
		}
		return 1;
	}
	return 0;
}

static void selectFilter(uint8_t index)
{
	//uint8_t i,crc;

	//crc=0x55;
	outputBuf[0]='M';
	outputBuf[1]='X';
	outputBuf[2]='M';	// 'M' for motor board
	outputBuf[3]='R';	// 'R' rotator motor for select filter
	outputBuf[4]='1';	// one command parameter
	outputBuf[5]=index+0x30;	// change to ascii number
	//for(i=3;i<6;i++) crc ^= outputBuf[i];
	//outputBuf[i]=crc;
	transferSize=6;
	if(transferSize)
	{
		while(transOngoingFlag);
		transOngoingFlag=1;
		HAL_UART_Transmit_IT(&CMD_LINKER,outputBuf,transferSize);
	}
}

void stopSelectFilter(void)
{
	//uint8_t i,crc;

	//crc=0x55;
	outputBuf[0]='M';
	outputBuf[1]='X';
	outputBuf[2]='M';	// 'M' for motor board
	outputBuf[3]='S';	// 'R' rotator motor for select filter
	outputBuf[4]='0';	// no command parameter
	//outputBuf[5]=index+0x30;	// change to ascii number
	//for(i=3;i<6;i++) crc ^= outputBuf[i];
	//outputBuf[i]=crc;
	transferSize=5;
	if(transferSize)
	{
		while(transOngoingFlag);
		transOngoingFlag=1;
		HAL_UART_Transmit_IT(&CMD_LINKER,outputBuf,transferSize);
	}
}

void turnoffAllLight(void)
{
	outputBuf[0]='M';
	outputBuf[1]='X';
	outputBuf[2]='L';	// 'L' for light board
	outputBuf[3]='C';	// 'S' select light command, 'C' close all light command
	outputBuf[4]='0';	// no command parameter
	//for(i=3;i<7;i++) crc ^= outputBuf[i];
	//outputBuf[i]=crc;
	transferSize=5;
	if(transferSize)
	{
		while(transOngoingFlag);
		transOngoingFlag=1;
		HAL_UART_Transmit_IT(&CMD_LINKER,outputBuf,transferSize);
	}
	nowLightState=NOW_LIGHT_IS_OFF;
}

void brightnessAdj(uint8_t dir)
{
	outputBuf[0]='M';
	outputBuf[1]='X';
	outputBuf[2]='L';	// 'L' for light board
	outputBuf[3]='A';	// 'S' select light command, 'C' close all light command, 'A' brightness adjust
	outputBuf[4]='1';	// one command parameter
	if(dir==BRIGHTNESS_ADJ_UP)
		outputBuf[5]='1';
	else
		outputBuf[5]='0';
	//for(i=3;i<7;i++) crc ^= outputBuf[i];
	//outputBuf[i]=crc;
	transferSize=6;
	if(transferSize)
	{
		while(transOngoingFlag);
		transOngoingFlag=1;
		HAL_UART_Transmit_IT(&CMD_LINKER,outputBuf,transferSize);
	}
}


void motionCtrl(uint8_t dir)
{
	outputBuf[0]='M';
	outputBuf[1]='X';
	outputBuf[2]='L';	// 'L' for light board
	outputBuf[3]='M';	// 'S' select light command, 'C' close all light command, 'A' brightness adjust, 'M' motor ctrl
	outputBuf[4]='1';	// one command parameter
	if(dir==MOTION_CW)
		outputBuf[5]='1';
	else
		outputBuf[5]='0';
	//for(i=3;i<7;i++) crc ^= outputBuf[i];
	//outputBuf[i]=crc;
	transferSize=6;
	if(transferSize)
	{
		while(transOngoingFlag);
		transOngoingFlag=1;
		HAL_UART_Transmit_IT(&CMD_LINKER,outputBuf,transferSize);
	}
}

void reportLightStatusChange(void)
{
	notifyStatusToHost(((nowLightState==NOW_LIGHT_IS_ON) ? currLight : 0xff ),currFilter,currUnion);
}

static void selectLight(uint8_t index)
{
	//uint8_t i,crc;
	uint8_t tenNum;

	tenNum=index/10;

	//crc=0x55;
	outputBuf[0]='M';
	outputBuf[1]='X';
	outputBuf[2]='L';	// 'L' for light board
	outputBuf[3]='S';	// 'S' select light command, 'C' close all light command
	outputBuf[4]='2';	// two command parameter
	outputBuf[5]=tenNum+0x30; // change to ascii number
	outputBuf[6]=(index-tenNum*10)+0x30;
	//for(i=3;i<7;i++) crc ^= outputBuf[i];
	//outputBuf[i]=crc;
	transferSize=7;
	if(transferSize)
	{
		while(transOngoingFlag);
		transOngoingFlag=1;
		HAL_UART_Transmit_IT(&CMD_LINKER,outputBuf,transferSize);
	}
	nowLightState=NOW_LIGHT_IS_ON;
}

static void notifyStatusToHost(uint8_t lightNum,uint8_t filterNum,uint8_t unionNum)
{
	uint8_t i,crc=0xAA;

	while(bleTransOngoingFlag);

	bleOutputBuf[0]=BOARD_ADDR_BT;
	bleOutputBuf[1]='L';	// leds status
	bleOutputBuf[2]=lightNum;
	bleOutputBuf[3]=filterNum;
	if(unionNum>7)
	{
		bleOutputBuf[4]=0xff;
		bleOutputBuf[5]=unionNum-8;
	}
	else
	{
		bleOutputBuf[4]=unionNum;
		bleOutputBuf[5]=0xff;
	}
	for(i=2;i<6;i++) crc ^= bleOutputBuf[i];
	bleOutputBuf[i]= crc;	// checksum
	bleTransferSize=i+1;

	if(HAL_UART_Transmit_IT(&BLE_USART,bleOutputBuf,bleTransferSize)==HAL_OK)
	{
		bleTransOngoingFlag=1;
	}
}

//static uint8_t suffBoardStatusToBuffer(void)
//{
//	uint8_t i,crc=0xAA;
//
//	bleOutputBuf[0]=BOARD_ADDR_BT;
//	bleOutputBuf[1]='L';	// leds status
//	for(i=2;i<6;i++) crc ^= bleOutputBuf[i];
//	bleOutputBuf[i]= crc;	// checksum
//	return i+1;	// 7 bytes
//}
//
//static void getTargetStatus(uint8_t *pBuf)
//{
//	uint8_t tmp;
//	tmp=retrieveEchoUnion();
//	*pBuf++=retrieveEchoLight();
//	*pBuf++=retrieveEchoFilter();
//	if(tmp>=8)
//	{
//		*pBuf++=0xff;
//		*pBuf=tmp-8;
//	}
//	else
//	{
//		*pBuf++=tmp;
//		*pBuf=0xff;
//	}
//}
