/********************************************************************************************************
 * @file	tlkmmi_pts.h
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
#ifndef TLKMMI_PTS_H
#define TLKMMI_PTS_H

#if (TLK_TEST_PTS_ENABLE)



#define TLKMMI_PTS_TIMEOUT          50000 //50ms

#define TLKMMI_PTS_DBG_FLAG       ((TLK_MAJOR_DBGID_MMI_TEST << 24) | (TLK_MINOR_DBGID_MMI_TEST_PTS << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMMI_PTS_DBG_SIGN       nullptr


typedef enum{
	TLKMMI_PTS_BUSY_NONE   = 0x0000,
}TLKMMI_PTS_BUSYS_ENUM;


typedef struct{
	uint16 flags;
	uint16 busys;
	tlkapi_timer_t timer;
}tlkmmi_pts_ctrl_t;


void tlkmmi_pts_reboot(uint16 timeout); //Unit: ms



#endif //#if (TLK_TEST_PTS_ENABLE)

#endif //TLKMMI_PTS_H

