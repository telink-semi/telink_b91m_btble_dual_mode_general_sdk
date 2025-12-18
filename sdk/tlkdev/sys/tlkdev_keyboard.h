/********************************************************************************************************
 * @file	tlkdev_keyboard.h
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
#ifndef TLKDEV_KEYBOARD_H
#define TLKDEV_KEYBOARD_H

#if (TLKDEV_KEYBOARD_ENABLE)


#define	TLKDEV_KEYBOARD_VALUE_MAP	{ \
		2,  3, \
		4,  5, }
#define TLKDEV_KEYBOARD_COL_NUMBER          2
#define TLKDEV_KEYBOARD_ROW_NUMBER          2
#define TLKDEV_KEYBOARD_COL_PULLUP          GPIO_PIN_PULLDOWN_100K//GPIO_PIN_PULLUP_10K
#define TLKDEV_KEYBOARD_ROW_PULLUP          GPIO_PIN_PULLUP_10K//GPIO_PIN_PULLDOWN_100K
#define TLKDEV_KEYBOARD_COL_WRITE_LEVEL     0
#define TLKDEV_KEYBOARD_ROW_VALID_LEVEL     0
#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
#define TLKDEV_KEYBOARD_COL_PINS            {GPIO_PD6, GPIO_PF6}
#define TLKDEV_KEYBOARD_ROW_PINS            {GPIO_PD2, GPIO_PD7}
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3)
#define TLKDEV_KEYBOARD_COL_PINS            {GPIO_PC2, GPIO_PC0}
#define TLKDEV_KEYBOARD_ROW_PINS            {GPIO_PC3, GPIO_PC1}
#else
#define TLKDEV_KEYBOARD_COL_PINS            {GPIO_NONE_PIN, GPIO_NONE_PIN}
#define TLKDEV_KEYBOARD_ROW_PINS            {GPIO_NONE_PIN, GPIO_NONE_PIN}
#endif

#define TLKDEV_KEYBORAD_TIMEOUT0            100000 //100ms
#define TLKDEV_KEYBORAD_TIMEOUT1            10000 //100ms

#define TLKDEV_KEYBOARD_KEY_MAX_NUMB        8

#define TLKDEV_KEYBOARD_CLICK_TIME_MIN      50000 //50ms
#define TLKDEV_KEYBOARD_CLICK_TIME_MAX      500000
#define TLKDEV_KEYBOARD_SHORT_TIME_MIN      500000 //100ms
#define TLKDEV_KEYBOARD_SHORT_TIME_MAX      2500000
#define TLKDEV_KEYBOARD_LONG_TIME_MIN       2500000 //100ms
#define TLKDEV_KEYBOARD_LONG_TIME_MAX       5000000

typedef enum{
	TLKDEV_KEYBOARD_EVTID_NONE    = 0x00,
	TLKDEV_KEYBOARD_EVTID_PRESS   = 0x01,
	TLKDEV_KEYBOARD_EVTID_RELEASE = 0x02,
	TLKDEV_KEYBOARD_EVTID_CLICK   = 0x03,
	TLKDEV_KEYBOARD_EVTID_SHORT   = 0x04,
	TLKDEV_KEYBOARD_EVTID_LONG    = 0x05,
}TLKDEV_KEYBOARD_EVTID_ENUM;

typedef enum{
	TLKDEV_KEYBOARD_EVTMSK_NONE    = 0x00,
	TLKDEV_KEYBOARD_EVTMSK_PRESS   = 0x01,
	TLKDEV_KEYBOARD_EVTMSK_RELEASE = 0x02,
	TLKDEV_KEYBOARD_EVTMSK_CLICK   = 0x04,
	TLKDEV_KEYBOARD_EVTMSK_SHORT   = 0x08,
	TLKDEV_KEYBOARD_EVTMSK_LONG    = 0x10,
	TLKDEV_KEYBOARD_EVTMSK_DEFAULT = TLKDEV_KEYBOARD_EVTMSK_CLICK | TLKDEV_KEYBOARD_EVTMSK_SHORT 
		| TLKDEV_KEYBOARD_EVTMSK_LONG,
	TLKDEV_KEYBOARD_EVTMSK_ALL     = TLKDEV_KEYBOARD_EVTMSK_CLICK | TLKDEV_KEYBOARD_EVTMSK_SHORT 
		| TLKDEV_KEYBOARD_EVTMSK_LONG | TLKDEV_KEYBOARD_EVTMSK_PRESS | TLKDEV_KEYBOARD_EVTMSK_RELEASE,
}TLKDEV_KEYBOARD_EVTMSK_ENUM;


typedef void(*TlkDevKeyboardEventCB)(uint08 evtID, uint08 *pKey, uint08 keyNumb, bool isPress);


typedef struct{
	uint08 isOpen;
	uint08 evtMask;
	uint08 keyNumb;
	tlkapi_timer_t timer;
	TlkDevKeyboardEventCB evtCB;
	uint08 keycode[TLKDEV_KEYBOARD_KEY_MAX_NUMB];
	uint32 keyTimer[TLKDEV_KEYBOARD_KEY_MAX_NUMB];
}tlkdev_keyboard_t;


int tlkdev_keyboard_init(void);

bool tlkdev_keyboard_isBusy(void);
void tlkdev_keyboard_enterSleep(uint mcuMode);
void tlkdev_keyboard_leaveSleep(uint wakeSrc);


void tlkdev_keyboard_regEventCB(TlkDevKeyboardEventCB evtCB);







#endif //#if (TLKDEV_KEYBOARD_ENABLE)

#endif //TLKDEV_KEYBOARD_H


