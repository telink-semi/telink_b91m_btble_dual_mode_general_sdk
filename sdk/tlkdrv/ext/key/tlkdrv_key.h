/********************************************************************************************************
 * @file	tlkdrv_key.h
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
#ifndef TLKDRV_KEY_H
#define TLKDRV_KEY_H

#if (TLK_DEV_KEY_ENABLE)



#define TLKDRV_KEY_TIMEOUT0              100000 //100ms
#define TLKDRV_KEY_TIMEOUT1              10000 //10ms


#define TLKDRV_KEY_MAX_NUMB              2

#define TLKDRV_KEY_CLICK_TIME_MIN        50000 //50ms
#define TLKDRV_KEY_CLICK_TIME_MAX        500000
#define TLKDRV_KEY_SHORT_TIME_MIN        500000 //100ms
#define TLKDRV_KEY_SHORT_TIME_MAX        2500000
#define TLKDRV_KEY_LONG_TIME_MIN         2500000 //100ms
#define TLKDRV_KEY_LONG_TIME_MAX         5000000
#define TLKDRV_KEY_DCLICK_INTV_MAX       500000

typedef enum{
	TLKDRV_KEY_EVTID_NONE    = 0x00,
	TLKDRV_KEY_EVTID_PRESS   = 0x01,
	TLKDRV_KEY_EVTID_RELEASE = 0x02,
	TLKDRV_KEY_EVTID_CLICK   = 0x03,
	TLKDRV_KEY_EVTID_SHORT   = 0x04,
	TLKDRV_KEY_EVTID_LONG    = 0x05,
	TLKDRV_KEY_EVTID_DCLICK  = 0x06,
}TLKDRV_KEY_EVTID_ENUM;

typedef enum{
	TLKDRV_KEY_EVTMSK_NONE    = 0x00,
	TLKDRV_KEY_EVTMSK_PRESS   = 0x01,
	TLKDRV_KEY_EVTMSK_RELEASE = 0x02,
	TLKDRV_KEY_EVTMSK_CLICK   = 0x04,
	TLKDRV_KEY_EVTMSK_SHORT   = 0x08,
	TLKDRV_KEY_EVTMSK_LONG    = 0x10,
	TLKDRV_KEY_EVTMSK_DCLICK  = 0x20,
	TLKDRV_KEY_EVTMSK_DEFAULT = TLKDRV_KEY_EVTMSK_CLICK | TLKDRV_KEY_EVTMSK_SHORT 
		| TLKDRV_KEY_EVTMSK_LONG | TLKDRV_KEY_EVTMSK_DCLICK,
	TLKDRV_KEY_EVTMSK_ALL     = TLKDRV_KEY_EVTMSK_CLICK | TLKDRV_KEY_EVTMSK_SHORT 
		| TLKDRV_KEY_EVTMSK_LONG | TLKDRV_KEY_EVTMSK_DCLICK
		| TLKDRV_KEY_EVTMSK_PRESS | TLKDRV_KEY_EVTMSK_RELEASE,
}TLKDRV_KEY_EVTMSK_ENUM;

typedef enum{
	TLKDRV_KEY_FLAG_NONE = 0x00,
	TLKDRV_KEY_FLAG_PRESS = 0x01,
	TLKDRV_KEY_FLAG_RELEASE = 0x02,
}TLKDRV_KEY_FLAGS_ENUM;

typedef void(*TlkDrvKeyEventCB)(uint08 keyID, uint08 evtID, uint08 isPress);

typedef struct{
	uint08 keyID;
	uint08 flags;
	uint08 level;
	uint08 evtMsk;
	uint32 ioPort;
	uint32 preTimer;
	uint32 curTimer;
	TlkDrvKeyEventCB evtCB;
}tlkdrv_key_unit_t;
typedef struct{
	tlkapi_timer_t timer;
	tlkdrv_key_unit_t unit[TLKDRV_KEY_MAX_NUMB];
}tlkdrv_key_ctrl_t;



/******************************************************************************
 * Function: tlkdrv_key_init
 * Descript: Initial the timer of the key.
 * Params: None.
 * Return: TLK_ENONE is success.
*******************************************************************************/
int tlkdrv_key_init(void);

bool tlkdrv_key_isBusy(void);

/******************************************************************************
 * Function: tlkdrv_key_insert
 * Descript: Insert a key,initial its GPIO and make timer working.
 * Params: 
 *     @keyID[IN]--The ketID, for instance 0x01, 0x02.
 *     @evtMsk[IN]--A marker for key events, refer to 'TLKDRV_KEY_EVTMSK_ENUM'.
 *     @ioPort[IN]--ioPort
 *     @level[IN]--Key effective level.
 *     @upDown[IN]--refer to 'gpio_pull_type_e'.
 *     @evtCB[IN]--key callback
 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int tlkdrv_key_insert(uint08 keyID, uint08 evtMsk, uint32 ioPort, uint08 level, uint08 upDown, TlkDrvKeyEventCB evtCB);

/******************************************************************************
 * Function: tlkdrv_key_remove
 * Descript: Disable the gpio for the key and remove a timer.
 * Params: 
 *     @keyID[IN]--The keyID.
 *     @upDown[IN]--refer to 'gpio_pull_type_e'.
 *     @enInput[IN]--true enable input, false disable input.
 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int tlkdrv_key_remove(uint08 keyID, uint08 upDown, bool enInput);

void tlkdrv_key_enterSleep(uint mcuMode);
void tlkdrv_key_leaveSleep(uint wakeSrc);


tlkdrv_key_unit_t *tlkdrv_key_getUsedUnit(uint08 keyID);



#endif //#if (TLK_DEV_KEY_ENABLE)

#endif //#ifndef TLKDRV_KEY_H

