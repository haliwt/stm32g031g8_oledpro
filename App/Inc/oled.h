/*
 * oled.h
 *
 *  Created on: 2021年1月12日
 *      Author: om
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

#include "spi.h"

#include "u8g2.h"

#define ENABLE_BLINK	1
#define DISABLE_BLINK	0

#define BLINK_OFF		0
#define BLINK_UNION		1
#define BLINK_FILTER	2
#define BLINK_LIGHT		3
#define BLINK_ALL		4

#define ECHO_GROUP_A	0
#define ECHO_GROUP_B	1

#define ECHO_FORMAT_MID_X	0
#define ECHO_FORMAT_MID_Y	0

#define ALIGN_MID_ALL		0
#define ALIGN_MID_SECOND    1

uint8_t LedSpotNumber;  //WT.EDIT
uint8_t LedMainNumber;

//uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
//uint8_t u8x8_byte_4wire_sw_spi_stm32(u8x8_t* u8g2, uint8_t msg, uint8_t arg_int,void* arg_ptr);
void oledInit(void);
void printEchoFilter(uint8_t filterIndex);
void printEchoLight(uint8_t lightIndex);
void printEchoUnion(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex);
void printSettingInfo(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex);
void printSettingInfo_MainLed(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex);


void setEchoFilterBlink(uint8_t state);
uint8_t getEchoFilterBlink(void);
void setEchoLightBlink(uint8_t state);
uint8_t getEchoLightBlink(void);
void blinkEchoFilter(uint8_t filterIndex);
void blinkEchoLight(uint8_t lightIndex);
void printSettingInfo_Manual(uint8_t unionIndex,uint8_t filterIndex,uint8_t ledlrIndex,uint8_t blinkIndex);
void printSettingInfo_Auxiliary(uint8_t unionIndex,uint8_t filterIndex,uint8_t ledlrIndex,uint8_t blinkIndex);
void printSettingInfo_filter(uint8_t unionIndex,uint8_t filterIndex,uint8_t lightIndex,uint8_t blinkIndex);
void printSettingInfo_SubItem(void); 


#endif /* INC_OLED_H_ */
