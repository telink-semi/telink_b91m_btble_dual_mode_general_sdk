/********************************************************************************************************
 * @file	tlkmmi_sensor.c
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
#if (TLKMMI_SENSOR_ENABLE)
#include "tlkmmi/sensor/tlkmmi_sensor.h"
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_sensorAdapt.h"
#include "tlkmmi_sensor.h"

#include "tlkmmi_sensorGsensor.h"
#include "tlkmmi_sensorGeomagnetic.h"
#include "tlkmmi_sensorHeartRate.h"


TLKSYS_MMI_TASK_DEFINE(sensor, Sensor);

static int tlkmmi_sensor_init(uint08 procID, uint16 taskID)
{
	tlkmmi_sensor_adaptInit(procID);

	#if (TLK_DEV_GSENSOR_ENABLE)
	tlkmmi_gsensor_init();
	#endif
	
	#if (TLK_DEV_HEARTRATE_ENABLE)
	tlkmmi_heartRate_init();
	#endif

	#if(TLK_DEV_GSENSOR_ENABLE)
	tlkmmi_geomagnetic_init();
	#endif

	return TLK_ENONE;
}
static int tlkmmi_sensor_start(void)
{
	return TLK_ENONE;
}
static int tlkmmi_sensor_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_sensor_close(void)
{
	return TLK_ENONE;
}
static int tlkmmi_sensor_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{
	
	return -TLK_ENOSUPPORT;
}
static void tlkmmi_sensor_handler(void)
{

}



#endif //#if (TLKMMI_SENSOR_ENABLE)

