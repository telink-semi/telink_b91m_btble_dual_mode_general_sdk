/********************************************************************************************************
 * @file	tlkdev_gsensor.h
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
#ifndef TLKDEV_GSENSOR_H
#define TLKDEV_GSENSOR_H

#if (TLK_DEV_GSENSOR_ENABLE)



typedef enum{
	TLKDEV_GSENSOR_DEV_SC7A20 = 0,
	TLKDEV_GSENSOR_DEV_MMC5603,
	TLKDEV_GSENSOR_DEV_DSDA217,
	TLKDEV_GSENSOR_DEV_MAX,
}TLKDEV_GSENSOR_DEV_ENUM;


bool tlkdev_gsensor_isOpen(TLKDEV_GSENSOR_DEV_ENUM dev);

int tlkdev_gsensor_init(TLKDEV_GSENSOR_DEV_ENUM dev);
int tlkdev_gsensor_open(TLKDEV_GSENSOR_DEV_ENUM dev);
int tlkdev_gsensor_close(TLKDEV_GSENSOR_DEV_ENUM dev);
int tlkdev_gsensor_reset(TLKDEV_GSENSOR_DEV_ENUM dev);

int tlkdev_gsensor_handler(TLKDEV_GSENSOR_DEV_ENUM dev, uint16 opcode, uint32 param0, uint32 param1);



#endif //TLK_DEV_GSENSOR_ENABLE

#endif //TLKDEV_GSENSOR_H

