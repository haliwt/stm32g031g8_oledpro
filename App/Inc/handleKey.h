/*
 * key.h
 *
 *  Created on: 2019��7��30��
 *      Author: nk
 */

#ifndef HANDLEKEY_H_
#define HANDLEKEY_H_



extern void keyInit(void);
extern void handleInput(void);
void AuxiliaryWhichOneLed_Plus(uint8_t wled);
void AuxiliaryWhichOneLed_Reduce(uint8_t wled);
extern uint8_t SideButton_SubItem_Input(void);
void TurnOnUnionSPOT_Light(void);


typedef struct _auxiliary_
{
    uint8_t AuxiliaryItem;
	uint8_t AuxiliaryLed_id;
	uint8_t AuxiliarySubItem;
	uint8_t Auxiliary_flag;
    uint8_t SmartMenuFlag;
    uint8_t SmartMenuItem;
    uint8_t SmartKey;
	uint8_t SmartMode;
	uint8_t ManualMode;
	uint8_t mainLedKey;
	uint8_t filterID;
	uint8_t filterRunNum;
	uint8_t filterIDInit;
}auxiliary;

typedef struct _mainLed_
{
   uint8_t ledoff_flag;
   uint8_t MainSpotUnion_Led;
   uint8_t MainLed_Num;
   uint8_t MainUnionSport_flag;
   uint8_t SW_Mode;

}mainled;


extern auxiliary auxiliary_t;

extern mainled  mainled_t;

//void selectLight_Union(uint8_t index);


enum _aux_{Main,Spot,Side,Left,Right};


#endif /* HANDLEKEY_H_ */
