/*
 * keyDef.h
 *
 *  Created on: 2019��6��18��
 *      Author: nk
 */

#ifndef INC_KEYDEF_H_
#define INC_KEYDEF_H_



typedef struct _KeyStruct
{
  unsigned int keyCode;
  unsigned char status;
  unsigned char long_pressed;
  unsigned char multi_pressed;
} KeyStruct,*pKeyStruct;

#define KEY_JITTER_DELAY	10		// jitter timeout when the key pressed
#define KEY_MULTI_DELAY		400		// indicate multiple key pressed
#define KEY_LONG_DELAY		3000	// indicate the key pressed for a long time
#define KEY_USEFUL_DELAY	10		// key up status useful-life
#define KEY_TYPEMATIC_RATE	150		// typematic rate

#define KEY_STATUS_NOPRESSED	0
#define KEY_STATUS_DOWN			1
#define KEY_STATUS_FREEZE		5
#define KEY_STATUS_UP			2

#define NO_KEY				(0x3ff)//(0x1ff) WT.EDIT 2021.04.23
#define KEY_CODE_KEY9		(0x0001) //PA0 
#define KEY_CODE_KEY3		(0x0002)
#define KEY_CODE_KEY5		(0x0004)
#define KEY_CODE_KEY6		(0x0008)
#define KEY_CODE_KEY1		(0x0010)
#define KEY_CODE_KEY2		(0x0020)
#define KEY_CODE_KEY7		(0x0040)
#define KEY_CODE_KEY4		(0x0080)
#define KEY_CODE_KEY8		(0x0100)
#define KEY_CODE_KEY10      (0x8000) //WT.EDIT 2021.04.23 PA15

#endif /* INC_KEYDEF_H_ */
