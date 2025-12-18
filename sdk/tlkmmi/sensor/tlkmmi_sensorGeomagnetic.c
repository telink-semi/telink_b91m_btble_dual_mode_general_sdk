/********************************************************************************************************
 * @file	tlkmmi_sensorGeomagnetic.c
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
#include "tlkmmi_sensorGeomagnetic.h"
#include "tlkdev/sys/tlkdev_gsensor.h"

#define TLKMMI_GEOMAGNETIC_TIMER_INTVAL			(20*1000) //us

static tlkapi_timer_t gTlkMmiGeomagneticTimer;


static bool tlkmmi_geomagnetic_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkdev_gsensor_handler(TLKDEV_GSENSOR_DEV_MMC5603,0,0,0);
	tlkapi_trace(TLKMMI_SENSOR_DBG_FLAG, TLKMMI_SENSOR_DBG_SIGN,"geo mmc timer");
	return true;
}

int tlkmmi_geomagnetic_init(void)
{
	tlkdev_gsensor_init(TLKDEV_GSENSOR_DEV_MMC5603);
	tlkmmi_sensor_adaptInitTimer(&gTlkMmiGeomagneticTimer, tlkmmi_geomagnetic_timer, NULL, TLKMMI_GEOMAGNETIC_TIMER_INTVAL);
	return TLK_ENONE;
}

int tlkmmi_geomagnetic_start(void)
{
	tlkdev_gsensor_open(TLKDEV_GSENSOR_DEV_MMC5603);
	tlkmmi_sensor_adaptInsertTimer(&gTlkMmiGeomagneticTimer);
	return TLK_ENONE;
}

int tlkmmi_geomagnetic_close(void)
{
	tlkdev_gsensor_close(TLKDEV_GSENSOR_DEV_MMC5603);
	tlkmmi_sensor_adaptRemoveTimer(&gTlkMmiGeomagneticTimer);
	return TLK_ENONE;
}

int tlkmmi_geomagnetic_reset(void)
{
	tlkdev_gsensor_reset(TLKDEV_GSENSOR_DEV_MMC5603);
	return TLK_ENONE;
}



#endif //#if (TLK_DEV_SENSOR_ENABLE)

