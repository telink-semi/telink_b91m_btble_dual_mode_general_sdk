/********************************************************************************************************
 * @file	tlkdev_gsensor.c
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
#include "tlkdev/sys/tlkdev_gsensor.h"
#include "tlkdrv/ext/gsensor/tlkdrv_gsensor.h"



bool tlkdev_gsensor_isOpen(TLKDEV_GSENSOR_DEV_ENUM dev)
{
	return tlkdrv_gsensor_isOpen(dev);
}

int tlkdev_gsensor_init(TLKDEV_GSENSOR_DEV_ENUM dev)
{
	return tlkdrv_gsensor_init(dev);
}
int tlkdev_gsensor_open(TLKDEV_GSENSOR_DEV_ENUM dev)
{
	return tlkdrv_gsensor_open(dev);
}
int tlkdev_gsensor_close(TLKDEV_GSENSOR_DEV_ENUM dev)
{
	return tlkdrv_gsensor_close(dev);
}
int tlkdev_gsensor_reset(TLKDEV_GSENSOR_DEV_ENUM dev)
{
	return tlkdrv_gsensor_reset(dev);
}

int tlkdev_gsensor_handler(TLKDEV_GSENSOR_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1)
{
	return tlkdrv_gsensor_handler(dev, opcode, param0, param1);
}





#endif //#if (TLK_DEV_GSENSOR_ENABLE)

