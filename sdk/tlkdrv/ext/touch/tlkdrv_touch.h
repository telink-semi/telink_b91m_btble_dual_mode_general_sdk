/********************************************************************************************************
 * @file	tlkdrv_touch.h
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
#ifndef TLKDRV_TOUCH_H
#define TLKDRV_TOUCH_H


#define TLKDRV_TOUCH_FT3168_ENABLE        0
#define TLKDRV_TOUCH_POINT_MAX            5


typedef enum{
	TLKDRV_TOUCH_DEV_FT3168 = 0,
	TLKDRV_TOUCH_DEV_MAX,
}TLKDRV_TOUCH_DEV_ENUM;

typedef enum{
	TLKDRV_TOUCH_OPCODE_NONE = 0,
}TLKDRV_TOUCH_OPCODE_ENUM;

typedef struct{
	uint16 evtID;
	uint16 axisX;
	uint16 axisY;
}tlkdrv_touch_point_t;
typedef struct{
	uint08 gestID;
	uint08 number;
	uint16 axisX[TLKDRV_TOUCH_POINT_MAX];
	uint16 axisY[TLKDRV_TOUCH_POINT_MAX];
}tlkdrv_touch_gesture_t;




typedef struct{
	bool(*IsOpen)(void);
	bool(*IsReady)(void);
	int(*Init)(void);
	int(*Open)(void);
	int(*Close)(void);
	int(*GetPoint)(tlkdrv_touch_point_t *pPoint);
	int(*GetGesture)(tlkdrv_touch_gesture_t *pGesture);
	int(*Handler)(uint16 opcode, uint32 param0, uint32 param1);
}tlkdrv_touch_modinf_t;


bool tlkdrv_touch_isOpen(TLKDRV_TOUCH_DEV_ENUM dev);
bool tlkdrv_touch_isReady(TLKDRV_TOUCH_DEV_ENUM dev);

int tlkdrv_touch_init(TLKDRV_TOUCH_DEV_ENUM dev);
int tlkdrv_touch_open(TLKDRV_TOUCH_DEV_ENUM dev);
int tlkdrv_touch_close(TLKDRV_TOUCH_DEV_ENUM dev);
int tlkdrv_touch_getPoint(TLKDRV_TOUCH_DEV_ENUM dev, tlkdrv_touch_point_t *pPoint);
int tlkdrv_touch_getGesture(TLKDRV_TOUCH_DEV_ENUM dev, tlkdrv_touch_gesture_t *pGesture);
int tlkdrv_touch_handler(TLKDRV_TOUCH_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1);






#endif //TLKDRV_TOUCH_H

