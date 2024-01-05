/********************************************************************************************************
 * @file	tlkdrv_battery.h
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
#ifndef TLKDRV_BATTERY_H
#define TLKDRV_BATTERY_H
#if (TLK_DEV_BATTERY_ENABLE)


// TLKAPP_CFG_BAT_CHANNEL=0: Internal battery voltage. The battery voltage sample range is 1.8~3.5V 
// TLKAPP_CFG_BAT_CHANNEL=GPIO_Pxx: External battery voltage. if the battery voltage > 3.6V, should take some external voltage divider.
#if(MCU_CORE_TYPE == MCU_CORE_B91)
#define TLKDRV_BATTERY_GPIO_PIN        GPIO_PB7//0//GPIO_PB0
#define TLKDRV_BATTERY_ADC_PIN         ADC_GPIO_PB7
#endif
#if(MCU_CORE_TYPE == MCU_CORE_B92)
#define TLKDRV_BATTERY_GPIO_PIN        GPIO_PB6//0//GPIO_PB0
#define TLKDRV_BATTERY_ADC_PIN         ADC_GPIO_PB6
#endif

#define TLKDEV_BATTERY_CHECK_INTERVAL         3000000 //3s
#define TLKDEV_BATTERY_LOW_PROTECT_ENABLE     1
#define TLKDEV_BATTERY_LOW_PROTECT_VOLTAGE    3500
#define TLKDEV_BATTERY_LOW_RESTORE_VOLTAGE    3700

#define TLKDEV_BATTERY_ANALOG_REG             0x39 //0x39 ~ 0x3f
#define TLKDEV_BATTERY_FLAG_LOW_POWER         0x01

typedef void(*TlkDrvBatteryCB)(uint08 level, uint16 voltage);

typedef struct{
	uint08 isOpen;
	uint08 level;
	uint16 voltage;
	uint16 preValue;
	tlkapi_timer_t timer;
	TlkDrvBatteryCB batCB;
}tlkdrv_battery_t;


int tlkdrv_battery_init(void);

int tlkdrv_battery_open(void);
int tlkdrv_battery_close(void);

int tlkdrv_battery_level(void);
int tlkdrv_battery_voltage(void);

void tlkdrv_battery_regCB(TlkDrvBatteryCB batCB);

bool tlkdrv_battery_isOpen(void);

void tlkdrv_battery_enterSleep(uint mcuMode);
void tlkdrv_battery_leaveSleep(uint wakeSrc);

void tlkdrv_battery_enterDeep(uint08 setFlag);
void tlkdrv_battery_leaveDeep(uint08 clrFlag);






#endif //#if (TLK_DEV_BATTERY_ENABLE)
#endif //TLKDRV_BATTERY_H

