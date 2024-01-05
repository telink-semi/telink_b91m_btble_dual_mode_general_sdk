/********************************************************************************************************
 * @file	tlkmmi_ptsBt.h
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
#ifndef TLKMMI_PTS_BT_H
#define TLKMMI_PTS_BT_H

#if (TLK_TEST_PTS_ENABLE)


#define TLKMMI_BTPTS_DEVICE_CLASS     0x240404//0x5a020c

#define TLKMMI_BTPTS_BDADDR_DEF       {0x66, 0x66, 0x55, 0x55, 0x11, 0x11}
#define TLKMMI_BTPTS_BTNAME_DEF       "Telink-Pts-Host"


int tlkmmi_pts_btInit(void);



#endif //#if (TLK_TEST_PTS_ENABLE)

#endif //TLKMMI_PTS_BT_H

