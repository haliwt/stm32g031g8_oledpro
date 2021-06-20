/*
 * key.h
 *
 *  Created on: 2019��7��30��
 *      Author: nk
 */

#ifndef HANDLEKEY_H_
#define HANDLEKEY_H_

#define DEBUG   1

extern void keyInit(void);
extern void handleInput(void);
void AuxiliaryWhichOneLed_Plus(uint8_t wled);
void AuxiliaryWhichOneLed_Reduce(uint8_t wled);
extern uint8_t SideButton_SubItem_Input(void);
void TurnOnUnionSPOT_Light(void);
void SideButtonSub_KEY(void);


typedef struct _auxiliary_
{
   
	uint8_t AuxiliarySubItem;
	uint8_t Auxiliary_flag;

	uint8_t SmartMenuItem;
	uint8_t subMenuOne;
 
	uint8_t ManualMode;
	uint8_t filterID;
	uint8_t subSubmode_bits;

	uint8_t subSubItemLed_Num;
	uint8_t subsubItem_spot;
	uint8_t subsubItem_side;
	uint8_t subsubItem_left;
	uint8_t subsubItem_right;
	
	
}auxiliary;

typedef struct _mainLed_
{
   uint8_t ledoff_flag;
   uint8_t MainSpotUnion_Led;
   uint8_t MainLed_Num;
   uint8_t MainUnionSport_flag;
   uint8_t Same_23;
   uint8_t SW_Mode;
   uint8_t unionNum;
   

}mainled;


extern auxiliary auxiliary_t;

extern mainled  mainled_t;

//void selectLight_Union(uint8_t index);


enum _aux_{Main,Spot,Side,Left,Right};


#endif /* HANDLEKEY_H_ */
