/*
 * cmd_link.h
 *
 *  Created on: Sep 23, 2020
 *      Author: om
 */

#ifndef CMD_LINK_H_
#define CMD_LINK_H_

#include "usart.h"

#define CMD_LINKER	huart2
#define BLE_USART	huart1


#define MAX_FILTER_NUMBER	10
#define MAX_LIGHT_NUMBER	16 //WT.EDIT 2021.04.22//10
#define MAX_UNION_NUMBER	20

#define MAX_LIGHT_LR_NUMBER    5 //WT.EDIT 
#define MAX_AUXILIARY_NUMBER   5 //WT.EDIT 2021.04.27

//Auxiliary Sub Item number LED 
#define MAX_SPOT_NUMBER     1     //WT.EDIT 2021.04.29
#define MAX_SIDE_NUMBER     4
#define MAX_LEFT_NUMBER     4
#define MAX_RIGHT_NUMBER    4

#define CMD_SUCCESS	0
#define CMD_ERROR	1

#define MOTOR_CMD_RUN	0
#define MOTOR_CMD_STOP	1

#define MOTOR_DIR_CW	0
#define MOTOR_DIR_CCW	1

#define MOTOR_SPEED_NORMAL	0
#define MOTOR_SPEED_HIGH	1

#define BRIGHTNESS_ADJ_UP	1
#define BRIGHTNESS_ADJ_DOWN	0

#define NOW_LIGHT_IS_OFF	0
#define NOW_LIGHT_IS_ON		1

#define MOTION_CW	1
#define MOTION_CCW	0

void cmdInit(void);
void decode(void);
void updateParameter(uint8_t unionIndex,uint8_t lightIndex,uint8_t lightIndex_LR,uint8_t lightIndex_AU,uint8_t filterIndex);
void updateLight(uint8_t lightIndex);
void stopSelectFilter(void);
void turnoffAllLight(void);
uint8_t getLightOnoffState(void);
void setCurrentLightOn(void);
void setCurrentLightOn_LR(void);//WT.EDIT 2021.04.27
void setCurrentLightOn_AU(void); //WT.EDIT 


void brightnessAdj(uint8_t dir);
void brightnessAdj_AuxiliaryLed(uint8_t dir);

void motionCtrl(uint8_t dir);
void reportLightStatusChange(void);
uint8_t retrieveSavedParameter(uint8_t *revealUnion,uint8_t *revealFilter,uint8_t *revealLight,uint8_t *revealLight_LR,uint8_t *revealLight_AU, uint8_t *revealGroup);
uint8_t getItemFromUnion(uint8_t unionIndex,uint8_t *filterIndex,uint8_t *lightIndex);
uint8_t getItemFromUnion_AU(uint8_t unionIndex,uint8_t *filterIndex,uint8_t *lightIndex);


void updateLight_LR(uint8_t lightIndex_LR);
void updateLight_AU(uint8_t lightIndex_AU);
void selectLight_AU(uint8_t index);

#endif /* CMD_LINK_H_ */
