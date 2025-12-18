/********************************************************************************************************
 * @file	tlkmmi_sensorGsensor.c
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
#if (TLK_DEV_GSENSOR_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_sensorAdapt.h"
#include "tlkmmi_sensor.h"
#include "tlkmmi_sensorGsensor.h"
#include "tlkdev/sys/tlkdev_gsensor.h"


#if(TLKDRV_SC7A20_INT_EN)
static tlkapi_queue_t gTlkMmiGsensorQueue;

static bool tlkmmi_gsensor_queue(tlkapi_queue_t *pQueue, uint32 userArg)
{
	tlkdev_gsensor_handler(0,0,0);
	return true;
}

int tlkmmi_gsensor_init(void)
{
	tlkdev_gsensor_init(TLKDEV_GSENSOR_DEV_SC7A20);
	tlkdev_gsensor_open(TLKDEV_GSENSOR_DEV_SC7A20);

	tlkmmi_sensor_adaptInitQueue(&gTlkMmiGsensorQueue, tlkmmi_gSensor_queue, NULL);
	tlkmmi_sensor_adaptAppendQueue(&gTlkMmiGsensorQueue);

	return TLK_ENONE;
}

int tlkmmi_gsensor_start(void)
{
	tlkmmi_sensor_adaptAppendQueue(&gTlkMmiGsensorQueue);
	return TLK_ENONE;
}

int tlkmmi_gsensor_close(void)
{
	tlkmmi_sensor_adaptRemoveQueue(&gTlkMmiGsensorQueue);
	return TLK_ENONE;
}

int tlkmmi_gsensor_reset(void)
{
	tlkdev_gsensor_reset(TLKDEV_GSENSOR_DEV_SC7A20);
	return TLK_ENONE;
}
#else
#define TLKMMI_GSENSOR_TIMER_INTVAL			(500*1000) //us
static tlkapi_timer_t gTlkMmiGsensorTimer;

static bool tlkmmi_gsensor_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkdev_gsensor_handler(TLKDEV_GSENSOR_DEV_SC7A20,0,0,0);
	return true;
}

int tlkmmi_gsensor_init(void)
{
	tlkdev_gsensor_init(TLKDEV_GSENSOR_DEV_SC7A20);
	tlkdev_gsensor_open(TLKDEV_GSENSOR_DEV_SC7A20);
	tlkmmi_sensor_adaptInitTimer(&gTlkMmiGsensorTimer, tlkmmi_gsensor_timer, NULL, TLKMMI_GSENSOR_TIMER_INTVAL);
	tlkmmi_sensor_adaptInsertTimer(&gTlkMmiGsensorTimer);

	return TLK_ENONE;
}

int tlkmmi_gsensor_start(void)
{
	tlkmmi_sensor_adaptInsertTimer(&gTlkMmiGsensorTimer);
	return TLK_ENONE;
}

int tlkmmi_gsensor_close(void)
{
	tlkmmi_sensor_adaptRemoveTimer(&gTlkMmiGsensorTimer);
	return TLK_ENONE;
}

int tlkmmi_gsensor_reset(void)
{
	tlkdev_gsensor_reset(TLKDEV_GSENSOR_DEV_SC7A20);
	return TLK_ENONE;
}
#endif


#endif //#if (TLK_DEV_GSENSOR_ENABLE)

