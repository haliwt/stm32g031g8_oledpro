/*
 * user_main.c
 *
 *  Created on: Jan 12, 2021
 *      Author: om
 */

#include "oled.h"
#include "handleKey.h"
#include "cmd_link.h"

void user_main(void)
{
	oledInit();
	cmdInit();
	keyInit();
	while(1)
	{
		handleInput();
		decode();
	}
}
