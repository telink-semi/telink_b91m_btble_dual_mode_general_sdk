/********************************************************************************************************
 * @file	tlkmmi_sensorHeartRate.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_DEV_HEARTRATE_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_sensorAdapt.h"
#include "tlkmmi_sensor.h"
#include "tlkmmi_sensorHeartRate.h"
#include "tlkdev/sys/tlkdev_heartRate.h"

#define TLKMMI_HEARTRATE_TIMER_INTVAL			(40*1000) //us

static tlkapi_timer_t gTlkMmiHeartRateTimer;

static bool tlkmmi_heartRate_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkdev_heartRate_handler(0,0,0);
	//tlkapi_trace(TLKMMI_SENSOR_DBG_FLAG, TLKMMI_SENSOR_DBG_SIGN,"heart rate timer");
	return true;
}

int tlkmmi_heartRate_init(void)
{
	tlkdev_heartRate_init();
	tlkmmi_sensor_adaptInitTimer(&gTlkMmiHeartRateTimer, tlkmmi_heartRate_timer, NULL, TLKMMI_HEARTRATE_TIMER_INTVAL);
	return TLK_ENONE;
}

int tlkmmi_heartRate_start(void)
{
	tlkdev_heartRate_open();
	tlkmmi_sensor_adaptInsertTimer(&gTlkMmiHeartRateTimer);
	return TLK_ENONE;
}

int tlkmmi_heartRate_close(void)
{
	tlkdev_heartRate_close();
	tlkmmi_sensor_adaptRemoveTimer(&gTlkMmiHeartRateTimer);
	return TLK_ENONE;
}

int tlkmmi_heartRate_reset(void)
{
	tlkdev_heartRate_reset();
	return TLK_ENONE;
}

#endif //#if (TLK_DEV_SENSOR_ENABLE)

