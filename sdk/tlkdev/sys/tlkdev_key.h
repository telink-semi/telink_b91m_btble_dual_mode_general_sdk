/********************************************************************************************************
 * @file	tlkdev_key.h
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
#ifndef TLKDEV_KEY_H
#define TLKDEV_KEY_H

#if (TLK_DEV_KEY_ENABLE)



#define TLKDEV_KEY_TIMEOUT0              100000 //100ms
#define TLKDEV_KEY_TIMEOUT1              10000 //100ms


#define TLKDEV_KEY_MAX_NUMB              2

#define TLKDEV_KEY_CLICK_TIME_MIN        50000 //50ms
#define TLKDEV_KEY_CLICK_TIME_MAX        500000
#define TLKDEV_KEY_SHORT_TIME_MIN        500000 //100ms
#define TLKDEV_KEY_SHORT_TIME_MAX        2500000
#define TLKDEV_KEY_LONG_TIME_MIN         2500000 //100ms
#define TLKDEV_KEY_LONG_TIME_MAX         5000000
#define TLKDEV_KEY_DCLICK_INTV_MAX       500000

typedef enum{
	TLKDEV_KEY_EVTID_NONE    = 0x00,
	TLKDEV_KEY_EVTID_PRESS   = 0x01,
	TLKDEV_KEY_EVTID_RELEASE = 0x02,
	TLKDEV_KEY_EVTID_CLICK   = 0x03,
	TLKDEV_KEY_EVTID_SHORT   = 0x04,
	TLKDEV_KEY_EVTID_LONG    = 0x05,
	TLKDEV_KEY_EVTID_DCLICK  = 0x06,
}TLKDEV_KEY_EVTID_ENUM;

typedef enum{
	TLKDEV_KEY_EVTMSK_NONE    = 0x00,
	TLKDEV_KEY_EVTMSK_PRESS   = 0x01,
	TLKDEV_KEY_EVTMSK_RELEASE = 0x02,
	TLKDEV_KEY_EVTMSK_CLICK   = 0x04,
	TLKDEV_KEY_EVTMSK_SHORT   = 0x08,
	TLKDEV_KEY_EVTMSK_LONG    = 0x10,
	TLKDEV_KEY_EVTMSK_DCLICK  = 0x20,
	TLKDEV_KEY_EVTMSK_DEFAULT = TLKDEV_KEY_EVTMSK_CLICK | TLKDEV_KEY_EVTMSK_SHORT 
		| TLKDEV_KEY_EVTMSK_LONG | TLKDEV_KEY_EVTMSK_DCLICK,
	TLKDEV_KEY_EVTMSK_ALL     = TLKDEV_KEY_EVTMSK_CLICK | TLKDEV_KEY_EVTMSK_SHORT 
		| TLKDEV_KEY_EVTMSK_LONG | TLKDEV_KEY_EVTMSK_DCLICK
		| TLKDEV_KEY_EVTMSK_PRESS | TLKDEV_KEY_EVTMSK_RELEASE,
}TLKDEV_KEY_EVTMSK_ENUM;

typedef enum{
	TLKDEV_KEY_FLAG_NONE = 0x00,
	TLKDEV_KEY_FLAG_PRESS = 0x01,
	TLKDEV_KEY_FLAG_RELEASE = 0x02,
}TLKDEV_KEY_FLAGS_ENUM;


int  tlkdev_key_init(void);
void tlkdev_key_deinit(void);

bool tlkdev_key_isBusy(void);
void tlkdev_key_enterSleep(uint mcuMode);
void tlkdev_key_leaveSleep(uint wakeSrc);




#endif //#if (TLK_DEV_KEY_ENABLE)

#endif //#ifndef TLKDEV_KEY_H

