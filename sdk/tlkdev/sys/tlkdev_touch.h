/********************************************************************************************************
 * @file	tlkdev_touch.h
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
#ifndef TLKDEV_TOUCH_H
#define TLKDEV_TOUCH_H

#if (TLK_DEV_TOUCH_ENABLE)


typedef enum{
	TLKDEV_TOUCH_ACTID_NONE = 0,
	TLKDEV_TOUCH_ACTID_UP,
	TLKDEV_TOUCH_ACTID_DOWN,
	TLKDEV_TOUCH_ACTID_MOVE,
}TLKDEV_TOUCH_ACTID_ENUM;
typedef enum{
	TLKDEV_TOUCH_EVTID_NONE = 0,
	TLKDEV_TOUCH_EVTID_PRESS,
	TLKDEV_TOUCH_EVTID_RELEASE,
	TLKDEV_TOUCH_EVTID_CLICK,
	TLKDEV_TOUCH_EVTID_L2R,
	TLKDEV_TOUCH_EVTID_R2L,
	TLKDEV_TOUCH_EVTID_T2B,
	TLKDEV_TOUCH_EVTID_B2T,
}TLKDEV_TOUCH_EVTID_ENUM;
	

typedef struct{
	uint16 axisX;
	uint16 axisY;
}tlkdev_touch_point_t;


int tlkdev_touch_init(void);
void tlkdev_touch_deinit(void);


bool tlkdev_touch_isOpen(void);
bool tlkdev_touch_isReady(void);


int tlkdev_touch_open(void);
int tlkdev_touch_close(void);
int tlkdev_touch_getPoint(uint16 *pEvtID, tlkdev_touch_point_t *pPoint);
int tlkdev_touch_getGesture(uint08 *pGstID, uint08 *pNumb, tlkdev_touch_point_t *pPoints, uint08 pointNumb);
int tlkdev_touch_handler(uint16 opcode, uint32 param0, uint32 param1);


int tlkdev_touch_calcGesture(uint16 x, uint16 y, TLKDEV_TOUCH_ACTID_ENUM actID);


#endif //TLK_DEV_TOUCH_ENABLE

#endif //TLKDEV_TOUCH_H

