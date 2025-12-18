/********************************************************************************************************
 * @file	tlkdev_heartrate.c
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
#include "tlkdev/tlkdev_stdio.h"
#include "tlkdev/sys/tlkdev_heartrate.h"
#include "tlkdrv/ext/heartrate/tlkdrv_heartrate.h"



#define TLKDEV_HEARTRATE_DEV        TLKDRV_HEARTRATE_DEV_HX3602



bool tlkdev_heartRate_isOpen(void)
{
	return tlkdrv_heartRate_isOpen(TLKDEV_HEARTRATE_DEV);
}

int tlkdev_heartRate_init(void)
{
	return tlkdrv_heartRate_init(TLKDEV_HEARTRATE_DEV);
}
int tlkdev_heartRate_open(void)
{
	return tlkdrv_heartRate_open(TLKDEV_HEARTRATE_DEV);
}
int tlkdev_heartRate_close(void)
{
	return tlkdrv_heartRate_close(TLKDEV_HEARTRATE_DEV);
}
int tlkdev_heartRate_reset(void)
{
	return tlkdrv_heartRate_reset(TLKDEV_HEARTRATE_DEV);
}

int tlkdev_heartRate_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	return tlkdrv_heartRate_handler(TLKDEV_HEARTRATE_DEV, opcode, param0, param1);
}




#endif //#if (TLK_DEV_HEARTRATE_ENABLE)

