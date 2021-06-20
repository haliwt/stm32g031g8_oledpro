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

char gtmpStr[MAX_UNION_STR_LEN+1];
char SubtmpStr[MAX_UNION_STR_LEN+1];


extern uint8_t retrieveEchoFilter(void);
extern uint8_t retrieveEchoLight(void);
extern uint8_t retrieveEchoUnion(void);
extern uint8_t retrieveEchoGroup(void);

static void printFrame(void);
static void NewPrintFrame(void); //WT.EDIT 2021.06.18
static void printFrame_Manual(void);

static void printLogo(u8g2_t *pU8g2);
static void printWithFmt(u8g2_t *pU8g2,uint8_t frmOrigX,uint8_t frmOrigY,uint8_t frmWidth,uint8_t frmHeight,uint8_t fmtCode,const char* pStr);

u8g2_t u8g2;

/*filter name Modify */
const char filterStr[MAX_FILTER_INDEX][MAX_FILTER_STR_LEN+1]={"VIS\0","BP532\0","LP595\0","LP635\0","LP665\0",
														"LP695\0","LP715\0","LP730\0","LP780\0","LP850\0"};

const char filterStr_sub[MAX_FILTER_INDEX][MAX_FILTER_STR_SUB_LEN+1]={"#1 VIS\0","#2 BP532\0","#3 LP595\0","#4 LP635\0","#5 LP665\0",
														"#6 LP695\0","#7 LP715\0","#8 LP730\0","#9 LP780\0","#10 LP850\0"};

/*LED name Modify */
const char lightStr[MAX_LIGHT_INDEX][MAX_LIGHT_STR_LEN+1]={"#1 White\0","#2 UV365\0","#3 Violet\0","#4 Blue1\0","#5 Blue2\0","#6 Cyan\0",
													 "#7 Green\0","#8 Orange\0","#9 Red\0","#10 640\0","#11 690\0","#12 720\0","#13 750\0",
													 "#14 770\0","#15 840\0","#16 930\0"};

/*SmartBotton union menu*/
/*LED name Modify */
const char lightStr_MainUnion[MAX_LIGHT_INDEX][MAX_LIGHT_STR_LEN+1]={"White\0","UV310\0","UV275\0","Blue1\0","Blue2\0","Cyan\0",
													 "Green\0","Orange\0","Red\0","640\0","690\0","720\0","750\0","770\0","840\0","930\0"};

/*LED name Modify--Manual Button */

const char lightStr_ML[MAX_LIGHT_INDEX][MAX_LIGHT_STR_LEN+1]={"930\0","White\0","UV365\0","Violet\0","Blue1\0","Blue2\0","Cyan\0",
													 "Green\0","Orange\0","Red\0","640\0","690\0","720\0","750\0","770\0","840\0"};

/*******************************************************************************************************************/
/*Auxiliary Board SPOT LED Name*/
//const char lightStr_AU[MAX_LIGHT_INDEX][MAX_LIGHT_STR_LEN+1]={"Green\0","Blue \0","UV310\0","UV275\0","White\0","Red  \0"};

/*Auxiliary Board linear LED Name*/													
//const char lightStr_LN[MAX_LIGHT_INDEX][MAX_LIGHT_STR_LEN+1]={"IR730\0","IR850\0","IR940\0","White\0"};

/**********************/
const char light_SPOT[MAX_LIGHT_SPOT_INDEX][MAX_LIGHT_LINEAR_STR_LEN+1]={"#1 Green\0","#2 Blue\0","#3 UV310\0","#4 UV275\0","#5 White\0","#6 Red\0"};
const char light_LINEAR[MAX_LIGHT_LINEAR_INDEX][MAX_LIGHT_LINEAR_STR_LEN+1]={"#1 IR730\0","#2 IR850\0","#3 IR940\0","#4 White\0"};
/***************************************************************************************************************************************/
/*LED name axuiliary WT.EDIT */
const char lightStr_LR[MAX_LIGHT_LR_INDEX][MAX_LIGHT_STR_LR_LEN+1]={"mainlight\0","spot light\0","both linear light\0","left linear light\0","right linear light\0"};													 


/***************************************************************************************************************************************/
const char unionInfo1[MAX_UNION_INDEX][MAX_UNION_INFO1_STR_LEN+1]={"Deleted Articles\0","Deleted Articles\0","Deleted Articles\0","Deleted Articles\0","Passport UVC+Money\0",
																"Passport UVB+Money\0","Passport IR+ID IR+Money\0","Passport IR+ID IR+Money\0","Passport IR+ID IR+Money \0","Passport UVA+ID,UVA+Money\0",
																"General Research Normal Picture\0","Black Pen\0","Black Pen\0","Black Pen+Blue Pen+Red Pen\0","Black Pen+Blue Pen+Red Pen\0",
																"Black Pen+Blue Pen+Red Pen\0","Black Pen+Blue Pen+Red Pen\0","Black Pen+Blue Pen+Red Pen\0","Black Pen+Blue Pen+Red Pen\0","Scribble Marks White\0",
																"Scribble Marks IR\0"};

const char unionInfo2[MAX_UNION_INDEX][MAX_UNION_INFO2_STR_LEN+1]={"\0","\0","\0","\0","UVC+Driving License UVC\0",
																"UVB+Driving License UVB\0","Checker Driving License\0","Checker Driving License\0","Checker Driving License\0","Checker Driving License\0",
																"\0","\0","\0","\0","\0",
																" \0","\0","\0","\0","\0",
																"\0"};
/***MainLed of words ****/
const char unionInfo_mainled[MAX_UNION_INDEX][MAX_UNION_INFO2_STR_LEN+1]={"Main,Board\0"};
																

/********************************************************************************************************************
 * *
 *Name :
 *
 *
 *
 *
********************************************************************************************************************/
const char unionInfoAU1[MAX_UNION_MAIN_INDEX][MAX_UNION_INFO1_STR_LEN + 1] = {"Main,Board\0","Spot,Board\0", "Side,Board\0", "Left,Board\0", "Right,Bord\0"};
																	 
																	   
																	

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
	*Input Ref:
	*Return Ref:
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

	
	NewPrintFrame();//printFrame();

	switch(group)
	{
	
		
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

		 if(mainled_t.MainSpotUnion_Led==1){
			while(lightStr_MainUnion[lightIndex][j]!=0)
			{
	            
	            tmpStr[i++]=lightStr_MainUnion[lightIndex][j];
				
				j++;
			}

		 }
		 else{
			while(lightStr_MainUnion[lightIndex][j]!=0)
			{
	            tmpStr[i++]=lightStr_MainUnion[lightIndex][j];
				j++;
			}
		 }
		tmpStr[i++]='+';
		

		j=0;while(filterStr[filterIndex][j]!=0)
		{
			tmpStr[i++]=filterStr[filterIndex][j];
			j++;
		}
		
		tmpStr[i++]=0;
		u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
		printWithFmt(&u8g2,UNION_INFO1_X,UNION_INFO1_Y,WIDTH_UNION_SMART,UNION_INFO1_HEIGHT,ALIGN_MID_ALL,unionInfo1[unionIndex]);
		printWithFmt(&u8g2,UNION_INFO2_X,UNION_INFO2_Y,WIDTH_UNION_SMART,UNION_INFO2_HEIGHT,ALIGN_MID_ALL,unionInfo2[unionIndex]);
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
	*Function Name:void printSettingInfo_MainLed
	             (uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex)
	*Function : menu of main board for 16 group
	*Input Ref:
	*Return Ref:
    *
*************************************************************************************************************/
void printSettingInfo_MainLed(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex)
{
	char tmpStr[MAX_UNION_STR_LEN+1];
	uint8_t tmpUnion,tmpFilter,tmpLight,i,tenNum;
	uint8_t group;
	

	tmpUnion=unionIndex+1;
	tmpFilter=filterIndex+1;
	tmpLight=lightIndex+1;
	group=retrieveEchoGroup();


	printFrame_Manual();//
	
	
		
        u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
			
		printWithFmt(&u8g2,LIGHT_SUBINFO_USER_X,LIGHT_SUBINFO_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
		printWithFmt(&u8g2,UNION_INFO3_SUB_X, UNION_INFO3_SUB_Y, WIDTH_UNION, UNION_INFO3_HEIGHT, ALIGN_MID_ALL,lightStr[mainled_t.MainLed_Num]);//the second line 


		
     
	    if(blinkIndex!=BLINK_ALL && blinkIndex!=BLINK_FILTER)
		{
			 if(auxiliary_t.filterID!=9)
				printWithFmt(&u8g2,FILTER_NAME_USER_0_X,FILTER_NAME_USER_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,filterStr_sub[auxiliary_t.filterID]);
			else
				printWithFmt(&u8g2,FILTER_NAME_USER_X,FILTER_NAME_USER_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,filterStr_sub[auxiliary_t.filterID]);
				
		}
			
		
		

	u8g2_SendBuffer(&u8g2);


}

/************************************************************************************************************
***
*Function Name:void printSettingInfo_filter(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex)
*Function : menu of main board for 16 group
*
Input Ref:
*
Return Ref:
*
*************************************************************************************************************/
void printSettingInfo_filter(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex)
{
	char tmpStr[MAX_UNION_STR_LEN+1];
	uint8_t tmpFilter,tmpLight;
	uint8_t group;
	
	auxiliary_t.filterInit_Swtich=1;

	auxiliary_t.filterID = filterIndex;   
    
	tmpFilter=filterIndex+1;
	group=retrieveEchoGroup();
	tmpLight = auxiliary_t.subSubItemLed_Num;

	
	printFrame_Manual();
    switch(group)
	{
	case ECHO_GROUP_A:
		
		
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
		
		
		
		if(blinkIndex!=BLINK_ALL && blinkIndex!=BLINK_FILTER)
		{
			
			 if(auxiliary_t.filterID!=9)
				printWithFmt(&u8g2,FILTER_NAME_USER_0_X,FILTER_NAME_USER_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,filterStr_sub[auxiliary_t.filterID]);
			else
				printWithFmt(&u8g2,FILTER_NAME_USER_X,FILTER_NAME_USER_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,filterStr_sub[auxiliary_t.filterID]);

		}

	   //sub Item led add number
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
		if(auxiliary_t.AuxiliarySubItem ==Main){
			
           printWithFmt(&u8g2,LIGHT_SUBINFO_USER_X,LIGHT_SUBINFO_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
		   printWithFmt(&u8g2,UNION_INFO3_SUB_X, UNION_INFO3_SUB_Y, WIDTH_UNION, UNION_INFO3_HEIGHT, ALIGN_MID_ALL,lightStr[mainled_t.MainLed_Num]);//the second line
		
			
			auxiliary_t.subSubmode_bits=0;
		}
		if(auxiliary_t.AuxiliarySubItem ==Spot){

		    printWithFmt(&u8g2,LIGHT_SUBINFO_USER_X,LIGHT_SUBINFO_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);

			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_SPOT[tmpLight]);
		    //auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=1;
		}
		if(auxiliary_t.AuxiliarySubItem ==Side){
		
			printWithFmt(&u8g2,LIGHT_SIDE_USER_X,LIGHT_SIDE_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_LINEAR[tmpLight]);
			//auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=2;
		}
		if(auxiliary_t.AuxiliarySubItem ==Right){ //Left is error
			
			printWithFmt(&u8g2,LIGHT_SIDE_USER_X,LIGHT_SIDE_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_LINEAR[tmpLight]);
		    //auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=3;
		}
		if(auxiliary_t.AuxiliarySubItem ==Left){ //Right is error 
			printWithFmt(&u8g2,LIGHT_SIDE_USER_X,LIGHT_SIDE_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_LINEAR[tmpLight]);
			//auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=4;
		}

		
		break;
		default:
		break;
	}

      
	
	u8g2_SendBuffer(&u8g2);

}

/************************************************************************************************************
***
	*Function Name:void printSettingInfo_Manual(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex)
	*Function : menu be selected axiliary left or right led board
	*Input Ref:
	*Return Ref:
*
*************************************************************************************************************/
void printSettingInfo_Manual(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex_lr,uint8_t blinkIndex)
{
	char tmpStr[MAX_UNION_STR_LEN+1],tmpSfliter[1];
	uint8_t tmpFilter, tmpLight, i,tenNum;
	uint8_t group;
    auxiliary_t.filterID=0x00;
    tenNum=0;
	group=retrieveEchoGroup();

	printFrame_Manual();//printFrame();

	switch(group)
	{
	case ECHO_GROUP_A:
		
		LedSpotNumber = lightIndex_lr;
		

		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);

        printWithFmt(&u8g2,LIGHT_SUBINFO_USER_X,LIGHT_SUBINFO_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[LedSpotNumber]);
		printWithFmt(&u8g2,UNION_INFO3_SUB_X, UNION_INFO3_SUB_Y, WIDTH_UNION, UNION_INFO3_HEIGHT, ALIGN_MID_ALL,lightStr[tenNum]);//the second line 
		

		if(blinkIndex!=BLINK_ALL && blinkIndex!=BLINK_FILTER)
		{
			printWithFmt(&u8g2,FILTER_NAME_USER_X,FILTER_NAME_USER_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,filterStr_sub[auxiliary_t.filterID]);
			
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
	*Function : Auxiliary board  spot  and linear of sub which is  LED number turn ON ?
	*Input Ref:
	*Return Ref:
*
*************************************************************************************************************/
void printSettingInfo_Auxiliary(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex_au,uint8_t blinkIndex) //echoLight = LED Name
{
	//char SubtmpStr[MAX_UNION_STR_LEN+1];
	uint8_t  tmpLight;
	uint8_t group;

	tmpLight = lightIndex_au;//unionIndex + 1;
	
	group=retrieveEchoGroup();

	printFrame_Manual();

	
		//filter Name :
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
		
		if(blinkIndex!=BLINK_ALL && blinkIndex!=BLINK_FILTER)
		{
			if(auxiliary_t.filterID!=9)
				printWithFmt(&u8g2,FILTER_NAME_USER_0_X,FILTER_NAME_USER_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,filterStr_sub[auxiliary_t.filterID]);
			else
				printWithFmt(&u8g2,FILTER_NAME_USER_X,FILTER_NAME_USER_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,filterStr_sub[auxiliary_t.filterID]);
			

		}

	   //sub Item led add number
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
		if(auxiliary_t.AuxiliarySubItem ==Main){
			
           printWithFmt(&u8g2,LIGHT_SUBINFO_USER_X,LIGHT_SUBINFO_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
		   printWithFmt(&u8g2,UNION_INFO3_SUB_X, UNION_INFO3_SUB_Y, WIDTH_UNION, UNION_INFO3_HEIGHT, ALIGN_MID_ALL,lightStr[mainled_t.MainLed_Num]);//the second line
		
			
			auxiliary_t.subSubmode_bits=0;
		}
		if(auxiliary_t.AuxiliarySubItem ==Spot){

		    printWithFmt(&u8g2,LIGHT_SUBINFO_USER_X,LIGHT_SUBINFO_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);

			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_SPOT[tmpLight]);
		    //auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=1;
		}
		if(auxiliary_t.AuxiliarySubItem ==Side){
		
			printWithFmt(&u8g2,LIGHT_SIDE_USER_X,LIGHT_SIDE_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_LINEAR[tmpLight]);
			//auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=2;
		}
		if(auxiliary_t.AuxiliarySubItem ==Right){ //Left is error
			
			printWithFmt(&u8g2,LIGHT_SIDE_USER_X,LIGHT_SIDE_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_LINEAR[tmpLight]);
		    //auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=3;
		}
		if(auxiliary_t.AuxiliarySubItem ==Left){ //Right is error 
			printWithFmt(&u8g2,LIGHT_SIDE_USER_X,LIGHT_SIDE_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_LINEAR[tmpLight]);
			//auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=4;
		}
        
		
	
	u8g2_SendBuffer(&u8g2);



}
/************************************************************************************************************
	***
	*Function Name:void printSettingInfo_SubItem(uint8_t lightIndex,uint8_t blinkIndex)
	*Function : Auxiliary sub main
	*Input Ref:
	*Return Ref:
*
*************************************************************************************************************/
void printSettingInfo_SubItem(void)
{
  
	uint8_t group;
    group=retrieveEchoGroup();

	printFrame_Manual();//printFrame();

	switch(group)
	{
	case ECHO_GROUP_A:
		
    
		 u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
        //if (blinkIndex != BLINK_ALL && blinkIndex != BLINK_FILTER)
		{
		  if(auxiliary_t.filterID!=9)
				printWithFmt(&u8g2,FILTER_NAME_USER_0_X,FILTER_NAME_USER_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,filterStr_sub[auxiliary_t.filterID]);
			else
				printWithFmt(&u8g2,FILTER_NAME_USER_X,FILTER_NAME_USER_Y,WIDTH_FILTER,FILTER_NUM_HEIGHT,ALIGN_MID_ALL,filterStr_sub[auxiliary_t.filterID]);
			
		}

		//Group B
	
        
		u8g2_SetFont(&u8g2, u8g2_font_7x13B_tr);
		if(auxiliary_t.AuxiliarySubItem ==Main){
			
           printWithFmt(&u8g2,LIGHT_SUBINFO_USER_X,LIGHT_SUBINFO_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
		   printWithFmt(&u8g2,UNION_INFO3_SUB_X, UNION_INFO3_SUB_Y, WIDTH_UNION, UNION_INFO3_HEIGHT, ALIGN_MID_ALL,lightStr[mainled_t.MainLed_Num]);//the second line
		
			
			auxiliary_t.subSubmode_bits=0;
		}
		if(auxiliary_t.AuxiliarySubItem ==Spot){

		    printWithFmt(&u8g2,LIGHT_SUBINFO_USER_X,LIGHT_SUBINFO_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);

			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_SPOT[0]);
		    auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=1;
		}
		if(auxiliary_t.AuxiliarySubItem ==Side){
		
			printWithFmt(&u8g2,LIGHT_SIDE_USER_X,LIGHT_SIDE_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_LINEAR[0]);
			auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=2;
		}
		if(auxiliary_t.AuxiliarySubItem ==Right){ //Left is error
			
			printWithFmt(&u8g2,LIGHT_SIDE_USER_X,LIGHT_SIDE_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_LINEAR[0]);
		    auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=3;
		}
		if(auxiliary_t.AuxiliarySubItem ==Left){ //Right is error 
			printWithFmt(&u8g2,LIGHT_SIDE_USER_X,LIGHT_SIDE_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,lightStr_LR[auxiliary_t.AuxiliarySubItem]);
			printWithFmt(&u8g2,LIGHT_NUM_SPOT_X,LIGHT_NUM_SPOT_Y,WIDTH_LIGHT,LIGHT_NUM_HEIGHT,ALIGN_MID_ALL,light_LINEAR[0]);
			auxiliary_t.subSubItemLed_Num=0;
			auxiliary_t.subSubmode_bits=4;
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
*Function Name:static void NewPrintFrame(void)
*Function: diplay title words New " SmartButton"
*
*
******************************************************************************************/
static void NewPrintFrame(void)
{
	u8g2_ClearBuffer(&u8g2);
	u8g2_DrawFrame(&u8g2,0,0,256,64);

	//u8g2_DrawVLine(&u8g2,VLINE1_X,0,63);  //绘制垂直线
	//u8g2_DrawVLine(&u8g2,VLINE2_X,0,63);

	u8g2_DrawHLine(&u8g2,HLINE1_X,HLINE1_Y,255); //	绘制水平线--长恒线
	u8g2_DrawHLine(&u8g2,HLINE1_X,HLINE3_Y,255); //

	u8g2_SetFontMode(&u8g2, 1);
	u8g2_SetFontDirection(&u8g2, 0);
	u8g2_SetFont(&u8g2, u8g2_font_7x13_tr);
	//printWithFmt(&u8g2,TITLE_LIGHT_X,TITLE_LIGHT_Y,WIDTH_LIGHT,TITLE_LIGHT_HEIGHT,ALIGN_MID_ALL,"Light");
	//printWithFmt(&u8g2,TITLE_FILTER_X,TITLE_FILTER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,"Filter");
	printWithFmt(&u8g2,TITLE_UNION_X,TITLE_UNION_Y,WIDTH_UNION,TITLE_UNION_HEIGHT,ALIGN_MID_ALL,"Smart  Mode");
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

	u8g2_DrawVLine(&u8g2,VLINE1_USER_X,HLINE1_Y,48); //绘制垂直线
	//u8g2_DrawVLine(&u8g2,VLINE2_X,0,63);

//	u8g2_DrawHLine(&u8g2,HLINE1_X,HLINE1_Y,255);
//	u8g2_DrawHLine(&u8g2,HLINE2_X,HLINE2_Y,HLINE_LEN);
//	u8g2_DrawHLine(&u8g2,HLINE3_X,HLINE3_Y,HLINE_LEN);
	u8g2_DrawHLine(&u8g2,HLINE1_X,HLINE1_Y,255); //	绘制水平线--长恒线
	u8g2_DrawHLine(&u8g2,HLINE1_X,HLINE3_Y,255); //

	u8g2_SetFontMode(&u8g2, 1);
	u8g2_SetFontDirection(&u8g2, 0);
	u8g2_SetFont(&u8g2, u8g2_font_7x13_tr);
	//printWithFmt(&u8g2,TITLE_LIGHT_USER_X,TITLE_LIGHT_USER_Y,WIDTH_USER_LIGHT,TITLE_LIGHT_USER_HEIGHT,ALIGN_MID_ALL,"mainlight");
	printWithFmt(&u8g2,TITLE_FILTER_USER_X,TITLE_FILTER_USER_Y,WIDTH_FILTER,TITLE_FILTER_HEIGHT,ALIGN_MID_ALL,"Filter");
	printWithFmt(&u8g2,TITLE_UNION_X,TITLE_UNION_Y,WIDTH_UNION,TITLE_UNION_HEIGHT,ALIGN_MID_ALL,"User Defined Mode");
}
/*****************************************************************************************
**
*Function Name:static void printFrame(void)
*Function: diplay title words  manual " ManualButton"
*
*
******************************************************************************************/
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

	charHeight=u8g2_GetMaxCharHeight(pU8g2);//获取当前字体立的最大字符的高度
	charAscent=u8g2_GetAscent(pU8g2); //获取基准线以上的高度
	strWidth=u8g2_GetStrWidth(pU8g2,pStr); //获取字付串的像素宽带

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

