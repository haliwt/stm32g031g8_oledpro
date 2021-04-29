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
}auxiliary;

extern auxiliary auxiliary_t;

enum _aux_{Spot,Side,Left,Right,Main};


#endif /* HANDLEKEY_H_ */
