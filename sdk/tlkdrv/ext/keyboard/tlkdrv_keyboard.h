/********************************************************************************************************
 * @file	tlkdrv_keyboard.h
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
#ifndef TLKDRV_KEYBOARD_H
#define TLKDRV_KEYBOARD_H

#if (TLKDEV_KEYBOARD_ENABLE)


#define TLKDRV_KB_DBG_FLAG               0//((TLK_MAJOR_DBGID_DRV << 24) | (TLK_MINOR_DBGID_DRV_EXT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKDRV_KB_DBG_SIGN               "[KB]"

#define TLKDRV_KEYBOARD_LONG_PRESS_OPTIMIZE     1   //lower power when pressing key without release
#define	TLKDRV_KEYBOARD_REMOVE_GHOST_KEY_EN		0

#define TLKDRV_KEYBOARD_KEY_MAX_NUMB        8

#define TLKDRV_KEYBOARD_COLPIN_MAX_NUMB     8
#define TLKDRV_KEYBOARD_ROLPIN_MAX_NUMB     8


typedef struct{
	uint08 isOpen;
	uint08 keyNumb;
	uint08 colNumb; //Colume Pin Number, As driver pin.
	uint08 rowNumb; //Row Pin Number, As scan pin.
	uint08 colWrite; //
	uint08 rowValid; //Row Valid Level: 0 or 1
	uint08 colPullup;
	uint08 rowPullup;
	const uint08 *pKeysMap;
	const uint16 *pColPins;
	const uint16 *pRowPins;
	uint08 keycode[TLKDRV_KEYBOARD_KEY_MAX_NUMB];
}tlkdrv_keyboard_t;


int  tlkdrv_keyboard_init(void);

int tlkdrv_keyboard_open(const uint08 *pKeysMap, const uint16 *pColPins,
	const uint16 *pRowPins, uint08 colNumb, uint08 rowNumb, uint08 colWrite,
	uint08 rowValid, uint08 colPullup, uint08 rowPullup);
int tlkdrv_keyboard_close(void);

bool tlkdrv_keyboard_scanKey(bool readKey);

void tlkdrv_keyboard_getKey(uint08 *pBuffer, uint08 buffLen, uint08 *pKeyNum);




#endif //#if (TLKDEV_KEYBOARD_ENABLE)

#endif //TLKDRV_KEYBOARD_H

