/********************************************************************************************************
 * @file	tlkdrv_ft3168.h
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
#ifndef TLKDRV_FT3168_H
#define TLKDRV_FT3168_H


#if (TLKDRV_TOUCH_FT3168_ENABLE)



#define TLKDRV_FT3168_INT_PIN           GPIO_PF3
#define TLKDRV_FT3168_RST_PIN           GPIO_PF6

#define TLKDRV_FT3168_I2C_SDA           GPIO_PF4
#define TLKDRV_FT3168_I2C_SCL           GPIO_PF5

#define TLKDRV_FT3168_I2C_SLAVE_ADDR    0x70

#define TLKDRV_FT3168_GESTURE_ENABLE    1

/*
 * TP FT3168 Register Map
 */
#define TLKDRV_FT3168_REG_STATUS                     0x02
#define TLKDRV_FT3168_REG_TOUCH1_XH_ADDR             0x03
#define TLKDRV_FT3168_REG_TOUCH2_XH_ADDR             0x09
#define TLKDRV_FT3168_REG_GESTURE_EN                 0xD0
#define TLKDRV_FT3168_REG_GESTURE_OUTPUT_ADDRESS     0xD3




typedef struct{
	uint08 isInit;
	uint08 isOpen;
	uint08 irqRdy;
	uint08 chipID;
}tlkdrv_ft3168_t;


static bool tlkdrv_ft3168_isOpen(void);
static bool tlkdrv_ft3168_isReady(void);

static int tlkdrv_ft3168_init(void);
static int tlkdrv_ft3168_open(void);
static int tlkdrv_ft3168_close(void);
static int tlkdrv_ft3168_getPoint(tlkdrv_touch_point_t *pPoint);
static int tlkdrv_ft3168_getGesture(tlkdrv_touch_gesture_t *pGesture);
static int tlkdrv_ft3168_handler(uint16 opcode, uint32 param0, uint32 param1);


static void tlkdrv_ft3168_sendByte(uint08 reg, uint08 byte);
static void tlkdrv_ft3168_readData(uint08 reg, uint08 *pBuff, uint08 buffLen);





#endif //#if (TLKDRV_TOUCH_FT3168_ENABLE)

#endif //TLKDRV_FT3168_H

