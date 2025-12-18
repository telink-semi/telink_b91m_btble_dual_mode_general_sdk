/********************************************************************************************************
 * @file	tlkdev_led.h
 *
 * @brief	This is the header file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef TLKDEV_LED_H
#define TLKDEV_LED_H

#if (TLK_DEV_LED_ENABLE)



typedef struct{
	uint08 operate; //0x00-off, 0x01-on, 0x02-auto
	uint08 firstOn;
	uint08 isKeepOn;
	uint16 exeCount;
	uint16 onTimerMs;
	uint16 offTimerMs;
}tlkdev_led_write_t;



/******************************************************************************
 * Function: tlkdev_led_init
 * Descript: Initial I/O and PWM timers of the led
 * Params: None.
 * Return: TLK_ENONE is success.
*******************************************************************************/
int tlkdev_led_init(void);

bool tlkdev_led_isBusy(void);
void tlkdev_led_enterSleep(uint mcuMode);
void tlkdev_led_leaveSleep(uint wakeSrc);


/******************************************************************************
 * Function: tlkdev_led_on
 * Descript: Turn on a led.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_led_on(uint08 ledID);

/******************************************************************************
 * Function: tlkdev_led_off
 * Descript: Turn off a led.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_led_off(uint08 ledID);

/******************************************************************************
 * Function: tlkdev_led_auto
 * Descript: Control led flash,the led flash frequency can be controlled by
 * 			 'onTimerMs' and 'offTimerMs', can also choose to keep the led on
 * 			 or off after the flashing is over.
 * Params: @ledID[IN]--The ledID.
 * 		   @firstOn[IN]--The state of the first work, true is on, false is off.
 * 		   @count[IN]--led flash count.
 * 		   @onTimerMs[IN]--on time.
 * 		   @offTimerMs[IN]--off time.
 * 		   @isKeepOn[IN]--The status after the flash is over, true is on, false is off.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_led_auto(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs, bool isKeepOn) ;



#endif //#if (TLK_DEV_LED_ENABLE)

#endif //#ifndef TLKDEV_LED_H

