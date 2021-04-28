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

typedef struct _auxiliary_
{
    uint8_t AuxiliaryItem;
	uint8_t AuxiliaryLed_id;
	uint8_t AuxiliarySubItem;
	uint8_t Auxiliary_flag;
    uint8_t SmartMuenFlag;

}auxiliary;

extern auxiliary auxiliary_t;

enum _aux_{Main,Spot,Side,Left,Right};


#endif /* HANDLEKEY_H_ */
