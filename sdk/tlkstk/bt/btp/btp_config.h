/********************************************************************************************************
 * @file	btp_config.h
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
#ifndef BTP_CONFIG_H
#define BTP_CONFIG_H

#if (TLK_STK_BTP_ENABLE)


#define BTP_TIMER_TIMEOUT                200000
#define BTP_TIMER_TIMEOUT_MS             200

#ifndef BTP_SDP_SHARE_BUFFER_ENABLE
#define BTP_SDP_SHARE_BUFFER_ENABLE      (1 && TLK_STK_BT_ENABLE)
#endif

#define BTP_MAP_L2CAP_ENABLE             0
#define BTP_PBAP_L2CAP_ENABLE            1

#define TLKBTP_CFG_SDP_ENABLE            (1 && TLK_STK_BTP_ENABLE)
#define TLKBTP_CFG_SDPSRV_ENABLE         (1 && TLK_STK_BTP_ENABLE && TLKBTP_CFG_SDP_ENABLE)
#define TLKBTP_CFG_SDPCLT_ENABLE         (1 && TLK_STK_BTP_ENABLE && TLKBTP_CFG_SDP_ENABLE)
#define TLKBTP_CFG_RFC_ENABLE            (1 && TLK_STK_BTP_ENABLE)
#define TLKBTP_CFG_ATT_ENABLE            (1 && TLK_STK_BTP_ENABLE)
#define TLKBTP_CFG_ATTSRV_ENABLE         (1 && TLK_STK_BTP_ENABLE && TLKBTP_CFG_ATT_ENABLE)
#define TLKBTP_CFG_ATTCLT_ENABLE         (0 && TLK_STK_BTP_ENABLE && TLKBTP_CFG_ATT_ENABLE)
#define TLKBTP_CFG_IAP_ENABLE            (1 && TLK_STK_BTP_ENABLE)
#define TLKBTP_CFG_SPP_ENABLE            (1 && TLK_STK_BTP_ENABLE && TLKBTP_CFG_RFC_ENABLE)
#define TLKBTP_CFG_SPP_USER_ENABLE       (0 && TLKBTP_CFG_SPP_ENABLE)
#define TLKBTP_CFG_HFP_ENABLE            (1 && TLK_STK_BTP_ENABLE && TLKBTP_CFG_RFC_ENABLE)
#define TLKBTP_CFG_HFPHF_ENABLE          (1 && TLKBTP_CFG_HFP_ENABLE)
#define TLKBTP_CFG_HFPAG_ENABLE          (1 && TLKBTP_CFG_HFP_ENABLE)
#define TLKBTP_CFG_AVRCP_ENABLE          (1 && TLK_STK_BTP_ENABLE)
#define TLKBTP_CFG_AVRCP_BROWSING_ENABLE (0 && TLK_STK_BTP_ENABLE)
#define TLKBTP_CFG_A2DP_ENABLE           (1 && TLK_STK_BTP_ENABLE)
#define TLKBTP_CFG_A2DPSRC_ENABLE        (1 && TLKBTP_CFG_A2DP_ENABLE)
#define TLKBTP_CFG_A2DPSNK_ENABLE        (1 && TLKBTP_CFG_A2DP_ENABLE)
#define TLKBTP_CFG_OBEX_ENABLE           (1 && TLK_STK_BTP_ENABLE)
#define TLKBTP_CFG_PBAP_ENABLE           (1 && TLKBTP_CFG_OBEX_ENABLE)
#define TLKBTP_CFG_PBAPCLT_ENABLE        (1 && TLKBTP_CFG_PBAP_ENABLE)
#define TLKBTP_CFG_PBAPSRV_ENABLE        (0 && TLKBTP_CFG_PBAP_ENABLE)
#define TLKBTP_CFG_HID_ENABLE            (1 && TLK_STK_BTP_ENABLE)
#define TLKBTP_CFG_HIDD_ENABLE           (1 && TLKBTP_CFG_HID_ENABLE)
#define TLKBTP_CFG_HIDH_ENABLE           (0 && TLKBTP_CFG_HID_ENABLE)
#define TLKBTP_CFG_PTSL2C_ENABLE         (1 && TLK_CFG_TEST_ENABLE)
#define TLKBTP_CFG_PTSHID_ENABLE         (0 && TLKBTP_CFG_HID_ENABLE && TLK_CFG_TEST_ENABLE)
#define TLKBTP_CFG_PTSSDP_ENABLE         (0 && TLKBTP_CFG_SDP_ENABLE && TLK_CFG_TEST_ENABLE)

#define TLK_BT_SDP_MAX_NUMB              (TLK_STK_BTACl_NUMB * 2 + 1)
#define TLK_BT_IAP_MAX_NUMB              (TLK_STK_BTACl_NUMB)
#define TLK_BT_HID_MAX_NUMB              (TLK_STK_BTACl_NUMB)
#define TLK_BT_SPP_MAX_NUMB              (TLK_STK_BTACl_NUMB)
#define TLK_BT_HFP_MAX_NUMB              (TLK_STK_BTACl_NUMB)
#define TLK_BT_ATT_MAX_NUMB              (1)
#define TLK_BT_A2DP_MAX_NUMB             (TLK_STK_BTACl_NUMB)
#define TLK_BT_AVRCP_MAX_NUMB            (TLK_STK_BTACl_NUMB)
#define TLK_BT_BROWSING_MAX_NUMB         (TLK_STK_BTACl_NUMB)
#define TLK_BT_RFCOMM_SERVICE_MAX_NUMB   6 // spp, pbap, hfp-ag, hfp-hf
#define TLK_BT_RFCOMM_SESSION_MAX_NUMB   (TLK_STK_BTACl_NUMB)
#define TLK_BT_RFCOMM_CHANNEL_MAX_NUMB   (TLK_STK_BTACl_NUMB * 6)
#define TLK_BT_RFCOMM_CHNDICT_MAX_NUMB   (TLK_BT_RFCOMM_CHANNEL_MAX_NUMB + 4)
#define TLK_BT_MAP_MAX_NUMB              (TLK_STK_BTACl_NUMB)
#define TLK_BT_PBAP_MAX_NUMB             (TLK_STK_BTACl_NUMB)


#define TLKBT_CFG_HS_RFC_CHANNEL         0x02
#define TLKBT_CFG_HFPHF_RFC_CHANNEL      0x05
#define TLKBT_CFG_HFPAG_RFC_CHANNEL      0x06
#define TLKBT_CFG_SPP_RFC_CHANNEL        0x04
#define TLKBT_CFG_IAP_RFC_CHANNEL        0x07
#define TLKBT_CFG_PBAP_RFC_CHANNEL       0x09
#define TLKBT_CFG_SPP_USER_RFC_CHANNEL 0x0a


#endif // #if (TLK_STK_BTP_ENABLE)

#endif // BTP_CONFIG_H
