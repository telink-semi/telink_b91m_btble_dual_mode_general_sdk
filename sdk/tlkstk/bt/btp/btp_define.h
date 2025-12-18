/********************************************************************************************************
 * @file	btp_define.h
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
#ifndef BTP_DEFINE_H
#define BTP_DEFINE_H

#if (TLK_STK_BTP_ENABLE)


typedef enum {
    BTP_PTYPE_SDP            = 0,
    BTP_PTYPE_RFC            = 1,
    BTP_PTYPE_HFP            = 2,
    BTP_PTYPE_IAP            = 3,
    BTP_PTYPE_SPP            = 4,
    BTP_PTYPE_HID            = 5,
    BTP_PTYPE_ATT            = 6,
    BTP_PTYPE_A2DP           = 7,
    BTP_PTYPE_AVRCP          = 8,
    BTP_PTYPE_PBAP           = 9,
    BTP_PTYPE_AVRCP_BROWSING = 10,
    BTP_PTYPE_PTS_L2C        = 11,
    BTP_PTYPE_MAX,
} BTP_PTYPE_ENUM; // Profile Type
typedef enum {
    BTP_PFLAG_SDP   = (1 << BTP_PTYPE_SDP),
    BTP_PFLAG_RFC   = (1 << BTP_PTYPE_RFC),
    BTP_PFLAG_HFP   = (1 << BTP_PTYPE_HFP),
    BTP_PFLAG_IAP   = (1 << BTP_PTYPE_IAP),
    BTP_PFLAG_SPP   = (1 << BTP_PTYPE_SPP),
    BTP_PFLAG_ATT   = (1 << BTP_PTYPE_ATT),
    BTP_PFLAG_A2DP  = (1 << BTP_PTYPE_A2DP),
    BTP_PFLAG_AVRCP = (1 << BTP_PTYPE_AVRCP),
    BTP_PFLAG_PBAP  = (1 << BTP_PTYPE_PBAP),


    BTP_PFLAG_OBEX_MASK   = BTP_PFLAG_PBAP,
    BTP_PFLAG_L2CAP_MASK  = BTP_PFLAG_ATT | BTP_PFLAG_A2DP | BTP_PFLAG_AVRCP,
    BTP_PFLAG_RFCOMM_MASK = BTP_PFLAG_HFP | BTP_PFLAG_IAP | BTP_PFLAG_SPP | BTP_PFLAG_OBEX_MASK,
} BTP_PFLAG_ENUM;
typedef enum {
    BTP_PSMID_SDP            = 0x0001,
    BTP_PSMID_RFCOMM         = 0x0003,
    BTP_PSMID_HIDC           = 0x0011,
    BTP_PSMID_HIDI           = 0x0013,
    BTP_PSMID_UPNP           = 0x0015,
    BTP_PSMID_AVCTP          = 0x0017,
    BTP_PSMID_AVDTP          = 0x0019,
    BTP_PSMID_ATT            = 0x001F,
    BTP_PSMID_AIRPODS        = 0x1001,
    BTP_PSMID_AVCTP_BROWSING = 0x001B,

    BTP_PSMID_PTS_L2C = 0x0EF1, // temp for PTS
} BTP_PSMID_ENUM;

typedef enum {
    BTP_USRID_NONE = 0,
    BTP_USRID_IGNORE,
    BTP_USRID_SERVER,
    BTP_USRID_CLIENT,
    BTP_USRID_EXTSRV,
    BTP_USRID_EXTCLT,
} BTP_USRID_ENUM;


#endif // #if (TLK_STK_BTP_ENABLE)

#endif // BTP_DEFINE_H
