/********************************************************************************************************
 * @file	tlkdrv_charge.h
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
#ifndef TLKDRV_CHARGE_H
#define TLKDRV_CHARGE_H


#define TLKDRV_CHARGE_ADDR                      0xD6
#define TLKDRV_CHARGE_REF_VOLTAGE				7100 // unit:mv

#define TLKDRV_CHARGE_SCL_PIN                   GPIO_PC1
#define TLKDRV_CHARGE_SDA_PIN				    GPIO_PC2
#define TLKDRV_CHARGE_EN_PIN					GPIO_PC0 // opuput low active
#define TLKDRV_CHARGE_PG_PIN					GPIO_PB0 // input reserved
#define TLKDRV_CHARGE_INT_PIN					GPIO_PB1 // input reserved
#define TLKDRV_CHARGE_ILIMIT1_PIN			    GPIO_PE5 // output
#define TLKDRV_CHARGE_ILIMIT2_PIN			    GPIO_PE6 // output

#define TLKDRV_CHARGE_STATUS_ADDR               0x0B
#define TLKDRV_CHARGE_VBUS_ADDR_H               0x1B
#define TLKDRV_CHARGE_VBUS_ADDR_L               0x1C
#define TLKDRV_CHARGE_VBAT_ADDR_H               0x1D
#define TLKDRV_CHARGE_VBAT_ADDR_L               0x1E


typedef enum {
	TLKDRV_CHARGE_NOT_CHARGING = 0,
	TLKDRV_CHARGE_TRICKLE_CHARGE,	// VBAT < VBAT_SHORT
	TLKDRV_CHARGE_PRE_CHARGE,		// VBAT_SHORT(4.4v) < VBAT < VBAT_LOWV(6v)
	TLKDRV_CHARGE_FAST_CHARGE,		// CC(Constant Current) mode
	TLKDRV_CHARGE_TAPER_CHARGE,		// CV(Constant Voltage) mode
	TLKDRV_CHARGE_TOP_OFF_TIMER_CHARGING,
	TLKDRV_CHARGE_CHARGE_TERMIATION_DONE,
	TLKDRV_CHARGE_RESERVED
} TLKDRV_CHARGE_STATUS_ENUM;


void tlkdrv_charge_init(void);
uint16 tlkdrv_charge_getVBat(void);
uint16 tlkdrv_charge_getVBus(void);
uint08 tlkdrv_charge_getState(void);


#endif //TLKDRV_CHARGE_H

