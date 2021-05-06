 /*
 * oled.c
 *
 *  Created on: Jan 12, 2021
 *      Author: om
 */

#include "oled.h"
#include "posDefine.h"
#include "handlekey.h"

#define ENABLE_VCC_LEVEL	GPIO_PIN_RESET
#define DISABLE_VCC_LEVEL	GPIO_PIN_SET

extern uint8_t retrieveEchoFilter(void);
extern uint8_t retrieveEchoLight(void);
extern uint8_t retrieveEchoUnion(void);
extern uint8_t retrieveEchoGroup(void);

static void printFrame(void);
static void printFrame_Manual(void);

static void printLogo(u8g2_t *pU8g2);
static void printWithFmt(u8g2_t *pU8g2,uint8_t frmOrigX,uint8_t frmOrigY,uint8_t frmWidth,uint8_t frmHeight,uint8_t fmtCode,const char* pStr);

u8g2_t u8g2;

/*filter name Modify */
const char filterStr[MAX_FILTER_INDEX][MAX_FILTER_STR_LEN+1]={"VIS\0","BPS32\0","LP595\0","LP635\0","LP665\0",
														"LP695\0","LP715\0","LP730\0","LP780\0","LP850\0"};
/*LED name Modify */
const char lightStr[MAX_LIGHT_INDEX][MAX_LIGHT_STR_LEN+1]={"White\0","UV365\0","Violet\0","Blue1\0","Blue2\0","Cyan\0",
													 "Green\0","Orange\0","Red\0","640\0","690\0","720\0","750\0","770\0","840\0","930\0"};
/*******************************************************************************************************************/
/*Auxiliary Board SPOT LED Name*/
const char lightStr_AU[MAX_LIGHT_INDEX][MAX_LIGHT_STR_LEN+1]={"Green\0","Blue\0","UV310\0","UV275\0","White\0","Red\0"};

/*Auxiliary Board linear LED Name*/													
const char lightStr_LN[MAX_LIGHT_INDEX][MAX_LIGHT_STR_LEN+1]={"IR730\0","IR850\0","IR940\0","White\0"};

/***************************************************************************************************************************************/
/*LED name axuiliary WT.EDIT */
const char lightStr_LR[MAX_LIGHT_LR_INDEX][MAX_LIGHT_STR_LR_LEN+1]={"Right\0","Main\0","Spot\0","Side\0","Left\0"};													 


/***************************************************************************************************************************************/
const char unionInfo1[MAX_UNION_INDEX][MAX_UNION_INFO1_STR_LEN+1]={"Fibers&Hair,Tooth&Bone\0","Fibers&Hair,Tooth&Bone\0","Fibers&Hair,Bruises&Bite\0","Bruises&Bite,BodyFluids\0","Bruises&Bite,BodyFluids\0",
																"BodyFluids,Bruises&Bite\0","BodyFluids,Tooth&Bone\0","BodyFluids\0","BodyFluids\0","General Research\0",
																"Blood,GSR\0","Blood,GSR\0","Blood\0","Blood\0","Blood\0",
																"GSR,TraceDrugs\0","GSR,Oil\0","TraceDrugs,DochExam\0","DochExam\0","Blood\0"};

const char unionInfo2[MAX_UNION_INDEX][MAX_UNION_INFO2_STR_LEN+1]={"FingerPrint\0","FingerPrint\0","BodyFluids,FingerPrint\0","FingerPrint\0","Fibers&Hair,FingerPrint\0",
																"Tooth&Bone,Fibers&Hair,FP\0","Bruises&Bite,FingerPrint\0","FingerPrint\0","Tooth&Bone,FingerPrint\0","Normal Camera Mode\0",
																"DochExam,IR FingerPrint\0","DochExam,FingerPrint\0","FingerPrint\0","*\0","FingerPrint\0",
																"FingerPrint\0","FingerPrint\0","FingerPrint\0","FingerPrint\0","DochExam\0"};


/********************************************************************************************************************
 * *
 *Name :
 *
 *
 *
 *
********************************************************************************************************************/
const char unionInfoAU1[MAX_UNION_INDEX][MAX_UNION_INFO1_STR_LEN + 1] = {"Spot,Board\0", "Side,Board\0", "Left,Board\0", "Right,Bord\0"};
																	 
																	   
																	

const char unionInfoAU2[MAX_UNION_INDEX][MAX_UNION_INFO1_STR_LEN + 1] = {"Spot,IR640\0", "Spot,IR750\0", "Fibers&Hair,Bruises&Bite\0", "Bruises&Bite,BodyFluids\0", "Bruises&Bite,BodyFluids\0"};
																	  

/******************************************************************************************************/
uint8_t enableBlinkEchoFilter,enableBlinkEchoLight;

uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg){
		//Initialize SPI peripheral
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			/* HAL initialization contains all what we need so we can skip this part. */

		break;

		//Function which implements a delay, arg_int contains the amount of ms
		case U8X8_MSG_DELAY_MILLI:
		HAL_Delay(arg_int);

		break;
		//Function which delays 10us
		case U8X8_MSG_DELAY_10MICRO:
		for (uint16_t n = 0; n < 320; n++)
		{
			__NOP();
			__NOP();
			__NOP();
		}

		break;
		//Function which implements a delay, arg_int contains the amount of ms
		case U8X8_MSG_DELAY_NANO:
			while(arg_int)
			{
				__NOP();
				arg_int--;
			}
		break;
		//Function which delays 100ns
		case U8X8_MSG_DELAY_100NANO:
		__NOP();
		__NOP();
		__NOP();

		break;
		//Function to define the logic level of the clockline
		case U8X8_MSG_GPIO_SPI_CLOCK:
			if (arg_int) HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, SET);
			else HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, RESET);

		break;
		//Function to define the logic level of the data line to the display
		case U8X8_MSG_GPIO_SPI_DATA:
			if (arg_int) HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, SET);
			else HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, RESET);

		break;
		// Function to define the logic level of the CS line
		case U8X8_MSG_GPIO_CS:
			if (arg_int) HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, SET);
			else HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, RESET);

		break;
		//Function to define the logic level of the Data/ Command line
		case U8X8_MSG_GPIO_DC:
			if (arg_int) HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, SET);
			else HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, RESET);

		break;
		//Function to define the logic level of the RESET line
		case U8X8_MSG_GPIO_RESET:
			if (arg_int) HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, SET);
			else HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, RESET);

		break;
		default:
			return 0; //A message was received which is not implemented, return 0 to indicate an error
	}

	return 1; // command processed successfully.
}

uint8_t u8x8_byte_4wire_hw_spi_stm32(u8x8_t* u8g2, uint8_t msg, uint8_t arg_int,void* arg_ptr)
{

	switch (msg)
	{
	case U8X8_MSG_BYTE_INIT:
		// Init.
		break;
	case U8X8_MSG_BYTE_SET_DC:
		u8x8_gpio_SetDC(u8g2, arg_int); // Pass the new set value of DC (in arg_int) to the gpio function.
		break;
	case U8X8_MSG_BYTE_START_TRANSFER:
		u8x8_gpio_SetCS(u8g2, u8g2->display_info->chip_enable_level);
		break;
	case U8X8_MSG_BYTE_SEND:

		HAL_SPI_Transmit(&hspi1,(uint8_t*)arg_ptr,arg_int,5000);
		break;
	case U8X8_MSG_BYTE_END_TRANSFER:
		u8x8_gpio_SetCS(u8g2, u8g2->display_info->chip_disable_level);
		break;
	default:
		return 0; // Return 0 if the message is not implemented (as specified in porting guide)
	}
	return 1;
}

void oledTurnOnVcc(void)
{
	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(ENVCC_GPIO_Port, ENVCC_Pin, ENABLE_VCC_LEVEL);
}


void oledInit(void)
{

	u8g2_Setup_ssd1322_nhd_256x64_f(&u8g2, U8G2_R2, u8x8_byte_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32);
	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
	u8g2_SetPowerSave(&u8g2, 0); // wake up display
	oledTurnOnVcc();
	HAL_Delay(500);

	printLogo(&u8g2);

}
/************************************************************************************************************
***
*Function Name:void printSettingInfo(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex)
*Function : menu of main board for 16 group
*
Input Ref:
*
Return Ref:
*
*************************************************************************************************************/
void printSettingInfo(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex)
{
	char tmpStr[MAX_UNION_STR_LEN+1];
	uint8_t tmpUnion,tmpFilter,tmpLight,i,j,tenNum;
	uint8_t group;
	

	tmpUnion=unionIndex+1;
	tmpFilter=filterIndex+1;
	tmpLight=lightIndex+1; 
	group=retrieveEchoGroup();

	if (auxiliary_t.Auxiliary_flag == 1)
		printFrame_Manual();//
	else printFrame();

	switch(group)
	{
	case ECHO_GROUP_A:
		i=0;
		if(tmpLight<16)//if(tmpLight<10)
			tmpStr[i++]=tmpLight+0x30; 
		else
		{
			tenNum=tmpLight/16;//tenNum=tmpLight/10;
			tmpStr[i++]=tenNum+0x30;
			tmpLight-= tenNum*16;//tmpLight-= tenNum*10;
			tmpStr[i++]=tmpLight+0x30;
		}
		tmpStr[i++]=0;
		LedMainNumber = tmpLight ;
		
			u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
			printWithFmt(&u8g2,LIGHT_INFO_X,LIGHT_INFO_Y,WIDTH_LIGHT,LIGHT_INFO_HEIGHT,ALIGN_MID_ALL,lightStr[LedMainNumber]);
			//printWithFmt(&u8g2,LIGHT_NUM_X,LIGHT_NUM_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,tmpStr);
			printWithFmt(&u8g2,LIGHT_NUM_X,LIGHT_NUM_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,lightStr_LR[LedSpotNumber]);
		
        //filter
		i=0;
		if(tmpFilter<10) tmpStr[i++]=tmpFilter+0x30;
		else
		{
			tenNum=tmpFilter/10;
			tmpStr[i++]=tenNum+0x30;
			tmpFilter-= tenNum*10;
			tmpStr[i++]=tmpFilter+0x30;
		}
		tmpStr[i++]=0;
		if(blinkIndex!=BLINK_ALL && blinkIndex!=BLINK_FILTER)
		{
			printWithFmt(&u8g2,FILTER_INFO_X,FILTER_INFO_Y,WIDTH_FILTER,FILTER_INFO_HEIGHT,ALIGN_MID_ALL,filterStr[filterIndex]);
			printWithFmt(&u8g2,FILTER_NUM_X,FILTER_NUM_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,tmpStr);
		}
		break;
		
	case ECHO_GROUP_B:
		i=0;
		tmpStr[i++]='#';
		if(tmpUnion<10) tmpStr[i++]=tmpUnion+0x30;
		else
		{
			tenNum=tmpUnion/10;  //filter has ten 
			tmpStr[i++]=tenNum+0x30;
			tmpUnion-= tenNum*10;
			tmpStr[i++]=tmpUnion+0x30;
		}
		tmpStr[i++]=' ';

		j=0; //LED number 
		while(lightStr[lightIndex][j]!=0)
		{
			tmpStr[i++]=lightStr[lightIndex][j];
			j++;
		}
		tmpStr[i++]='+';

		j=0;while(filterStr[filterIndex][j]!=0)
		{
			tmpStr[i++]=filterStr[filterIndex][j];
			j++;
		}
		
		tmpStr[i++]=0;
		u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
		printWithFmt(&u8g2,UNION_INFO1_X,UNION_INFO1_Y,WIDTH_UNION,UNION_INFO1_HEIGHT,ALIGN_MID_ALL,unionInfo1[unionIndex]);
		printWithFmt(&u8g2,UNION_INFO2_X,UNION_INFO2_Y,WIDTH_UNION,UNION_INFO2_HEIGHT,ALIGN_MID_ALL,unionInfo2[unionIndex]);
		if(blinkIndex!=BLINK_ALL && blinkIndex!=BLINK_FILTER)
		{
			u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
			printWithFmt(&u8g2,UNION_INFO3_X,UNION_INFO3_Y,WIDTH_UNION,UNION_INFO3_HEIGHT,ALIGN_MID_ALL,tmpStr);
		}
		break;
	default:
		break;
	}
	u8g2_SendBuffer(&u8g2);

}
/************************************************************************************************************
***
	*Function Name:void printSettingInfo_LR_Led(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex)
	*Function : menu be selected axiliary left or right led board
	*Input Ref:
	*Return Ref:
*
*************************************************************************************************************/
void printSettingInfo_LR_Led(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex_lr,uint8_t blinkIndex)
{
	char tmpStr[MAX_UNION_STR_LEN+1];
	uint8_t tmpUnion, tmpFilter, tmpLight, i,j, tenNum;
	uint8_t group;

	tmpUnion = unionIndex + 1;
	tmpFilter=filterIndex+1;
	tmpLight=lightIndex_lr+1;
	group=retrieveEchoGroup();

	printFrame_Manual();//printFrame();

	switch(group)
	{
	case ECHO_GROUP_A:
		i=0;
		if(tmpLight<5)
			tmpStr[i++]=tmpLight+0x30; 
		else
		{
			tenNum=tmpLight/5;
			tmpStr[i++]=tenNum+0x30;
			tmpLight-= tenNum*5;//tmpLight-= tenNum*10;
			tmpStr[i++]=tmpLight+0x30;
		}
		tmpStr[i++]=0;
		LedSpotNumber = tmpLight;
		if(LedSpotNumber ==1) auxiliary_t.mainLedKey =0;
		if(LedSpotNumber ==2) auxiliary_t.mainLedKey =1;
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
		printWithFmt(&u8g2,LIGHT_INFO_X,LIGHT_INFO_Y,WIDTH_LIGHT,LIGHT_INFO_HEIGHT,ALIGN_MID_ALL,lightStr[LedMainNumber]);
		printWithFmt(&u8g2,LIGHT_NUM_X,LIGHT_NUM_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,lightStr_LR[LedSpotNumber]);

		i=0;
		if(tmpFilter<10) tmpStr[i++]=tmpFilter+0x30;
		else
		{
			tenNum=tmpFilter/10;
			tmpStr[i++]=tenNum+0x30;
			tmpFilter-= tenNum*10;
			tmpStr[i++]=tmpFilter+0x30;
		}
		tmpStr[i++]=0;
		if(blinkIndex!=BLINK_ALL && blinkIndex!=BLINK_FILTER)
		{
			printWithFmt(&u8g2,FILTER_INFO_X,FILTER_INFO_Y,WIDTH_FILTER,FILTER_INFO_HEIGHT,ALIGN_MID_ALL,filterStr[filterIndex]);
			printWithFmt(&u8g2,FILTER_NUM_X,FILTER_NUM_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,tmpStr);
		}
		break;

	case ECHO_GROUP_B:
		i = 0;
		tmpStr[i++] = '#';
		if (tmpUnion < 10)
			tmpStr[i++] = tmpUnion + 0x30;
		else
		{
			tenNum = tmpUnion / 10; //filter has ten
			tmpStr[i++] = tenNum + 0x30;
			tmpUnion -= tenNum * 10;
			tmpStr[i++] = tmpUnion + 0x30;
		}
		tmpStr[i++] = ' ';

		j = 0; //LED number
		while (lightStr[lightIndex_lr][j] != 0)
		{
			tmpStr[i++] = lightStr[lightIndex_lr][j];
			j++;
		}
		tmpStr[i++] = '+';

		j = 0;
		while (filterStr[filterIndex][j] != 0)
		{
			tmpStr[i++] = filterStr[filterIndex][j];
			j++;
		}
		tmpStr[i++] = 0;
		u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
		printWithFmt(&u8g2, UNION_INFO1_X, UNION_INFO1_Y, WIDTH_UNION, UNION_INFO1_HEIGHT, ALIGN_MID_ALL, unionInfoAU1[LedSpotNumber]);
		printWithFmt(&u8g2, UNION_INFO2_X, UNION_INFO2_Y, WIDTH_UNION, UNION_INFO2_HEIGHT, ALIGN_MID_ALL, unionInfo2[unionIndex]);

		if (blinkIndex != BLINK_ALL && blinkIndex != BLINK_FILTER)
		 {
		 	u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
		 	printWithFmt(&u8g2, UNION_INFO3_X, UNION_INFO3_Y, WIDTH_UNION, UNION_INFO3_HEIGHT, ALIGN_MID_ALL, tmpStr);
		 }
		break;
		default:
		break;
	}
	u8g2_SendBuffer(&u8g2);

}
/************************************************************************************************************
***
	*Function Name:void printSettingInfo_Auxiliary(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex)
	*Function : Auxiliary board  spot  and linear LED number
	*Input Ref:
	*Return Ref:
*
*************************************************************************************************************/
void printSettingInfo_Auxiliary(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex_au,uint8_t blinkIndex) //echoLight = LED Name
{
	char tmpStr[MAX_UNION_STR_LEN+1];
	uint8_t tmpUnion, tmpFilter, tmpLight, i,j,z,tenNum;
	uint8_t group;

	tmpUnion = lightIndex_au+1;//unionIndex + 1;
	tmpFilter=filterIndex+1;
	tmpLight=lightIndex_au+1;
	group=retrieveEchoGroup();

	printFrame_Manual();

	switch(group)
	{
	case ECHO_GROUP_A:
		i=0;
		if(tmpLight<9) //auxiliary board has nine LED 
			tmpStr[i++]=tmpLight+0x30; 
		else
		{
			tenNum=tmpLight/9;
			tmpStr[i++]=tenNum+0x30;
			tmpLight-= tenNum*9;//tmpLight-= tenNum*10;
			tmpStr[i++]=tmpLight+0x30;
		}
		tmpStr[i++]=0;
		//LedSpotNumber = tmpLight;
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
		printWithFmt(&u8g2,LIGHT_INFO_X,LIGHT_INFO_Y,WIDTH_LIGHT,LIGHT_INFO_HEIGHT,ALIGN_MID_ALL,lightStr[LedMainNumber]);
		printWithFmt(&u8g2,LIGHT_NUM_X,LIGHT_NUM_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,lightStr_LR[LedSpotNumber]);

		i=0;
		if(tmpFilter<10) tmpStr[i++]=tmpFilter+0x30;
		else
		{
			tenNum=tmpFilter/10;
			tmpStr[i++]=tenNum+0x30;
			tmpFilter-= tenNum*10;
			tmpStr[i++]=tmpFilter+0x30;
		}
		tmpStr[i++]=0;
		if(blinkIndex!=BLINK_ALL && blinkIndex!=BLINK_FILTER)
		{
			printWithFmt(&u8g2,FILTER_INFO_X,FILTER_INFO_Y,WIDTH_FILTER,FILTER_INFO_HEIGHT,ALIGN_MID_ALL,filterStr[filterIndex]);
			printWithFmt(&u8g2,FILTER_NUM_X,FILTER_NUM_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,tmpStr);
		}
		//break;

	//case ECHO_GROUP_B:
		z = 0;
		tmpStr[z++] = '#';
		if (tmpUnion < 10)
			tmpStr[z++] = tmpUnion + 0x30;
		else
		{
			tenNum = tmpUnion / 10; //filter has ten
			tmpStr[z++] = tenNum + 0x30;
			tmpUnion -= tenNum * 10;
			tmpStr[z++] = tmpUnion + 0x30;
		}
		tmpStr[z++] = ' ';

		j = 0; //LED number
		if(auxiliary_t.AuxiliarySubItem ==Spot){
			while (lightStr_AU[lightIndex_au][j] != 0)
			{
				tmpStr[z++] = lightStr_AU[lightIndex_au][j];
				j++;
			}
//			tmpStr[z++] = '+';
//
//			j = 0;
//			while (filterStr[filterIndex][j] != 0)
//			{
//				tmpStr[z++] = filterStr[filterIndex][j];
//				j++;
//			}
//			tmpStr[z++] = 0;
		}
		else{
			while (lightStr_LN[lightIndex_au][j] != 0)
			{
				tmpStr[z++] = lightStr_LN[lightIndex_au][j];
				j++;
			}
//			tmpStr[z++] = '+';
//
//			j = 0;
//			while (filterStr[filterIndex][j] != 0)
//			{
//				tmpStr[z++] = filterStr[filterIndex][j];
//				j++;
//			}
//			tmpStr[z++] = 0;

		}
		u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
		if(auxiliary_t.AuxiliarySubItem ==Spot){
			printWithFmt(&u8g2, UNION_INFO1_X, UNION_INFO1_Y, WIDTH_UNION, UNION_INFO1_HEIGHT, ALIGN_MID_ALL, unionInfoAU1[0]); //The one rows
			//printWithFmt(&u8g2, UNION_INFO2_X, UNION_INFO2_Y, WIDTH_UNION, UNION_INFO2_HEIGHT, ALIGN_MID_ALL, unionInfoAU2[0]);  //The two rows
		}
		if(auxiliary_t.AuxiliarySubItem ==Side){
			printWithFmt(&u8g2, UNION_INFO1_X, UNION_INFO1_Y, WIDTH_UNION, UNION_INFO1_HEIGHT, ALIGN_MID_ALL, unionInfoAU1[1]); //The one rows
			//printWithFmt(&u8g2, UNION_INFO2_X, UNION_INFO2_Y, WIDTH_UNION, UNION_INFO2_HEIGHT, ALIGN_MID_ALL, unionInfoAU2[1]);  //The two rows
		}
		if(auxiliary_t.AuxiliarySubItem ==Left){
			printWithFmt(&u8g2, UNION_INFO1_X, UNION_INFO1_Y, WIDTH_UNION, UNION_INFO1_HEIGHT, ALIGN_MID_ALL, unionInfoAU1[2]); //The one rows
			//printWithFmt(&u8g2, UNION_INFO2_X, UNION_INFO2_Y, WIDTH_UNION, UNION_INFO2_HEIGHT, ALIGN_MID_ALL, unionInfoAU2[2]);  //The two rows
		}
		if(auxiliary_t.AuxiliarySubItem ==Right){
			printWithFmt(&u8g2, UNION_INFO1_X, UNION_INFO1_Y, WIDTH_UNION, UNION_INFO1_HEIGHT, ALIGN_MID_ALL, unionInfoAU1[3]); //The one rows
			//printWithFmt(&u8g2, UNION_INFO2_X, UNION_INFO2_Y, WIDTH_UNION, UNION_INFO2_HEIGHT, ALIGN_MID_ALL, unionInfoAU2[3]);  //The two rows
		}

		 if (blinkIndex != BLINK_ALL && blinkIndex != BLINK_FILTER)
		 {
		 	u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
		 	printWithFmt(&u8g2, UNION_INFO3_X, UNION_INFO3_Y, WIDTH_UNION, UNION_INFO3_HEIGHT, ALIGN_MID_ALL, tmpStr);
		 }
		break;
		default:
		break;
	}
	u8g2_SendBuffer(&u8g2);



}
void blinkEchoFilter(uint8_t filterIndex)
{
	static uint8_t sw=0;

	sw ^= 0x01;
	if(sw)
	{
		printSettingInfo(retrieveEchoUnion(),filterIndex,retrieveEchoLight(),BLINK_OFF);
	}
	else
	{
		printSettingInfo(retrieveEchoUnion(),filterIndex,retrieveEchoLight(),BLINK_FILTER);
	}
}

void blinkEchoLight(uint8_t lightIndex)
{
	static uint8_t sw=0;

	sw ^= 0x01;
	if(sw)
	{
		printSettingInfo(retrieveEchoUnion(),retrieveEchoLight(),lightIndex,BLINK_OFF);
	}
	else
	{
		printSettingInfo(retrieveEchoUnion(),retrieveEchoLight(),lightIndex,BLINK_LIGHT);
	}
}

void setEchoFilterBlink(uint8_t state)
{
	enableBlinkEchoFilter=state;
}

uint8_t getEchoFilterBlink(void)
{
	return enableBlinkEchoFilter;
}

void setEchoLightBlink(uint8_t state)
{
	enableBlinkEchoLight=state;
}

uint8_t getEchoLightBlink(void)
{
	return enableBlinkEchoLight;
}
/*****************************************************************************************
**
*Function Name:static void printFrame(void)
*Function: diplay title words " SmartButton"
*
*
******************************************************************************************/
static void printFrame(void)
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawFrame(&u8g2,0,0,256,64);

	u8g2_DrawVLine(&u8g2,VLINE1_X,0,63);
	u8g2_DrawVLine(&u8g2,VLINE2_X,0,63);

	u8g2_DrawHLine(&u8g2,HLINE1_X,HLINE1_Y,255);
	u8g2_DrawHLine(&u8g2,HLINE2_X,HLINE2_Y,HLINE_LEN);
	u8g2_DrawHLine(&u8g2,HLINE3_X,HLINE3_Y,HLINE_LEN);

	u8g2_SetFontMode(&u8g2, 1);
	u8g2_SetFontDirection(&u8g2, 0);
	u8g2_SetFont(&u8g2, u8g2_font_7x13_tr);
	printWithFmt(&u8g2,TITLE_LIGHT_X,TITLE_LIGHT_Y,WIDTH_LIGHT,TITLE_LIGHT_HEIGHT,ALIGN_MID_ALL,"Light");
	printWithFmt(&u8g2,TITLE_FILTER_X,TITLE_FILTER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,"Filter");
	printWithFmt(&u8g2,TITLE_UNION_X,TITLE_UNION_Y,WIDTH_UNION,TITLE_UNION_HEIGHT,ALIGN_MID_ALL,"SmartButton");
}
/*****************************************************************************************
**
*Function Name:static void printFrame(void)
*Function: diplay title words  manual " ManualButton"
*
*
******************************************************************************************/
static void printFrame_Manual(void)
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawFrame(&u8g2,0,0,256,64);

	u8g2_DrawVLine(&u8g2,VLINE1_X,0,63);
	u8g2_DrawVLine(&u8g2,VLINE2_X,0,63);

	u8g2_DrawHLine(&u8g2,HLINE1_X,HLINE1_Y,255);
	u8g2_DrawHLine(&u8g2,HLINE2_X,HLINE2_Y,HLINE_LEN);
	u8g2_DrawHLine(&u8g2,HLINE3_X,HLINE3_Y,HLINE_LEN);

	u8g2_SetFontMode(&u8g2, 1);
	u8g2_SetFontDirection(&u8g2, 0);
	u8g2_SetFont(&u8g2, u8g2_font_7x13_tr);
	printWithFmt(&u8g2,TITLE_LIGHT_X,TITLE_LIGHT_Y,WIDTH_LIGHT,TITLE_LIGHT_HEIGHT,ALIGN_MID_ALL,"Light");
	printWithFmt(&u8g2,TITLE_FILTER_X,TITLE_FILTER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,"Filter");
	printWithFmt(&u8g2,TITLE_UNION_X,TITLE_UNION_Y,WIDTH_UNION,TITLE_UNION_HEIGHT,ALIGN_MID_ALL,"ManualtButton");
}

static void printLogo(u8g2_t *pU8g2)
{
	u8g2_ClearBuffer(pU8g2);
	u8g2_SetFont(pU8g2, u8g2_font_logisoso32_tr);
	printWithFmt(pU8g2,0,0,0xff,63,ALIGN_MID_ALL,"ForenScope");
	u8g2_SendBuffer(pU8g2);
	HAL_Delay(1500);
}

static void printWithFmt(u8g2_t *pU8g2,uint8_t frmOrigX,uint8_t frmOrigY,
						uint8_t frmWidth,uint8_t frmHeight,uint8_t fmtCode,const char* pStr)
{
	uint8_t charHeight,charAscent,strWidth;
	uint8_t posX,posY;

	charHeight=u8g2_GetMaxCharHeight(pU8g2);
	charAscent=u8g2_GetAscent(pU8g2);
	strWidth=u8g2_GetStrWidth(pU8g2,pStr);

	switch(fmtCode & 0x0f)
	{
	case ECHO_FORMAT_MID_X:
		if(strWidth<frmWidth-1)
		{
			posX=(frmWidth-strWidth-1)/2+frmOrigX+1;
		}
		else
		{
			posX=frmOrigX+1;
		}
		break;
	default:
		break;
	}

	switch((fmtCode & 0xf0)>>4)
	{
	case ECHO_FORMAT_MID_Y:
		if(charHeight<frmHeight-1)
		{
			posY=(frmHeight-charHeight-1)/2+frmOrigY+charAscent+3;
		}
		else
		{
			//posY=frmOrigY+charAscent+1;
			posY=frmOrigY+frmHeight-1;
		}
		break;
	default:
		break;
	}

	u8g2_DrawStr(pU8g2,posX,posY,pStr);
}

