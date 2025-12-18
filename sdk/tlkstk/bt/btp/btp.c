/********************************************************************************************************
 * @file	btp.c
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
#if (TLK_STK_BTP_ENABLE)
#include "drivers.h"
#include "btp_stdio.h"
#include "btp_adapt.h"
#include "../bth/bth.h"
#include "../bth/bth_device.h"
#include "../bth/bth_handle.h"
#include "../bth/bth_l2cap.h"
#include "btp.h"
#include "btp_config.h"
#include "btp_define.h"
#include "sdp/btp_sdp.h"
#include "a2dp/btp_a2dp.h"
#include "avrcp/btp_avrcp.h"
#include "rfcomm/btp_rfcomm.h"
#include "hfp/btp_hfp.h"
#include "pbap/btp_pbap.h"
#include "spp/btp_spp.h"
#include "att/btp_att.h"
#include "hid/btp_hid.h"
#include "iap/btp_iap.h"
#include "browsing/btp_browsing.h"
#if (TLKBTP_CFG_PTSL2C_ENABLE)
#include "pts/btp_ptsL2c.h"
#endif
#if (TLKBTP_CFG_PTSHID_ENABLE)
#include "pts/btp_ptsHid.h"
#endif


int btp_init(void)
{
	int ret;

	ret = TLK_ENONE;
    if(ret == TLK_ENONE) ret = btp_event_init();
    if(ret == TLK_ENONE) ret = btp_adapt_init();
#if (TLKBTP_CFG_SDP_ENABLE)
    /* When 'BTP_SDP_SHARE_BUFFER_ENABLE' set to 1, HFP and HID fail to connect in
       a peer-to-peer connectivity scenario; set to 0 HFP and HID can be connected
       successfully, but will increase memory consumption.*/
    if(ret == TLK_ENONE){
		ret = btp_sdp_init(TLK_BT_SDP_MAX_NUMB, BTP_SDPSRV_RSP_ATT_LEN, 
			BTP_SDP_BUFFER_SIZE, BTP_SDP_SHARE_BUFFER_ENABLE);
	}
#endif
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    if(ret == TLK_ENONE) ret = btp_ptsl2c_init(TLK_STK_BTACl_NUMB);
#endif
#if (TLKBTP_CFG_AVRCP_ENABLE)
    if(ret == TLK_ENONE) ret = btp_avrcp_init(TLK_BT_AVRCP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_A2DP_ENABLE)
    if(ret == TLK_ENONE) ret = btp_a2dp_init(TLK_BT_A2DP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_RFC_ENABLE)
    if(ret == TLK_ENONE) {
		ret = btp_rfcomm_init(TLK_BT_RFCOMM_SERVICE_MAX_NUMB, TLK_BT_RFCOMM_SESSION_MAX_NUMB,
			TLK_BT_RFCOMM_CHANNEL_MAX_NUMB, TLK_BT_RFCOMM_CHNDICT_MAX_NUMB);
	} 
#endif
#if (TLKBTP_CFG_HFP_ENABLE)
    if(ret == TLK_ENONE) ret = btp_hfp_init(TLK_BT_HFP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_PBAP_ENABLE)
    if(ret == TLK_ENONE) ret = btp_pbap_init(TLK_BT_PBAP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_SPP_ENABLE)
    if(ret == TLK_ENONE) ret = btp_spp_init(TLK_BT_SPP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_IAP_ENABLE)
    if(ret == TLK_ENONE) ret = btp_iap_init(TLK_BT_IAP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_ATT_ENABLE)
    if(ret == TLK_ENONE) ret = btp_att_init(TLK_BT_ATT_MAX_NUMB);
#endif
#if (TLKBTP_CFG_HID_ENABLE)
    if(ret == TLK_ENONE) ret = btp_hid_init(TLK_BT_HID_MAX_NUMB);
#endif
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
    if(ret == TLK_ENONE) ret = btp_browsing_init(TLK_BT_BROWSING_MAX_NUMB);
    if(ret == TLK_ENONE) bth_l2cap_setExtFeatureBits(BTH_L2CAP_EXT_FEATURE_ENHANCED_RTN_MODE | BTH_L2CAP_EXT_FEATURE_FCS_OPTION);
#endif
#if (TLKBTP_CFG_PTSHID_ENABLE)
    if(ret == TLK_ENONE) ret = btp_ptshid_init();
#endif

	if(ret != TLK_ENONE) btp_deinit();
    return ret;
}
void btp_deinit(void)
{
#if (TLKBTP_CFG_SDP_ENABLE)
    /* When 'BTP_SDP_SHARE_BUFFER_ENABLE' set to 1, HFP and HID fail to connect in
       a peer-to-peer connectivity scenario; set to 0 HFP and HID can be connected
       successfully, but will increase memory consumption.*/
	btp_sdp_deinit();
#endif
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    btp_ptsl2c_deinit();
#endif
#if (TLKBTP_CFG_AVRCP_ENABLE)
    btp_avrcp_deinit();
#endif
#if (TLKBTP_CFG_A2DP_ENABLE)
    btp_a2dp_deinit();
#endif
#if (TLKBTP_CFG_RFC_ENABLE)
	btp_rfcomm_deinit();
#endif
#if (TLKBTP_CFG_HFP_ENABLE)
    btp_hfp_deinit();
#endif
#if (TLKBTP_CFG_PBAP_ENABLE)
    btp_pbap_deinit();
#endif
#if (TLKBTP_CFG_SPP_ENABLE)
    btp_spp_deinit();
#endif
#if (TLKBTP_CFG_IAP_ENABLE)
    btp_iap_deinit();
#endif
#if (TLKBTP_CFG_ATT_ENABLE)
    btp_att_deinit();
#endif
#if (TLKBTP_CFG_HID_ENABLE)
    btp_hid_deinit();
#endif
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
    btp_browsing_deinit();
#endif
#if (TLKBTP_CFG_PTSHID_ENABLE)
//    btp_ptshid_deinit();
#endif
}

void btp_handler(void)
{
    btp_adapt_handler();
}


void btp_destroy(uint16 aclHandle)
{
#if (TLKBTP_CFG_SDP_ENABLE)
    btp_sdp_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_A2DP_ENABLE)
    btp_a2dp_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_AVRCP_ENABLE)
    btp_avrcp_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    btp_ptsl2c_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_PTSHID_ENABLE)
    btp_ptshid_destroy(aclHandle);
#endif

#if (TLKBTP_CFG_PBAP_ENABLE)
    btp_pbap_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_HFP_ENABLE)
    btp_hfp_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_SPP_ENABLE)
    btp_spp_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_IAP_ENABLE)
    btp_iap_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_RFC_ENABLE)
    btp_rfcomm_destroy(aclHandle);
#endif

#if (TLKBTP_CFG_ATT_ENABLE)
    btp_att_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_HID_ENABLE)
    btp_hid_destroy(aclHandle);
#endif
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
    btp_browsing_destroy(aclHandle);
#endif
}

int btp_needMemLen(void)
{
	int needLen = 0;

#if (TLKBTP_CFG_SDP_ENABLE)
    /* When 'BTP_SDP_SHARE_BUFFER_ENABLE' set to 1, HFP and HID fail to connect in
       a peer-to-peer connectivity scenario; set to 0 HFP and HID can be connected
       successfully, but will increase memory consumption.*/
    needLen += btp_sdp_getMemLen(TLK_BT_SDP_MAX_NUMB, BTP_SDPSRV_RSP_ATT_LEN, BTP_SDP_BUFFER_SIZE, BTP_SDP_SHARE_BUFFER_ENABLE);
#endif
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    needLen += btp_ptsl2c_getMemLen(TLK_STK_BTACl_NUMB);
#endif
#if (TLKBTP_CFG_AVRCP_ENABLE)
    needLen += btp_avrcp_getMemLen(TLK_BT_AVRCP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_A2DP_ENABLE)
    needLen += btp_a2dp_getMemLen(TLK_BT_A2DP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_RFC_ENABLE)
    needLen += btp_rfcomm_getMemLen(TLK_BT_RFCOMM_SERVICE_MAX_NUMB, TLK_BT_RFCOMM_SESSION_MAX_NUMB,\
    				TLK_BT_RFCOMM_CHANNEL_MAX_NUMB, TLK_BT_RFCOMM_CHNDICT_MAX_NUMB);
#endif
#if (TLKBTP_CFG_HFP_ENABLE)
    needLen += btp_hfp_getMemLen(TLK_BT_HFP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_PBAP_ENABLE)
    needLen += btp_pbap_getMemLen(TLK_BT_PBAP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_SPP_ENABLE)
    needLen += btp_spp_getMemLen(TLK_BT_SPP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_IAP_ENABLE)
    needLen += btp_iap_getMemLen(TLK_BT_IAP_MAX_NUMB);
#endif
#if (TLKBTP_CFG_ATT_ENABLE)
    needLen += btp_att_getMemLen(TLK_BT_ATT_MAX_NUMB);
#endif
#if (TLKBTP_CFG_HID_ENABLE)
    needLen += btp_hid_getMemLen(TLK_BT_HID_MAX_NUMB);
#endif
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
    needLen += btp_browsing_getMemLen(TLK_BT_BROWSING_MAX_NUMB);
#endif

    return needLen;
}


uint16 gBtpConnTimeout = 5000 / BTP_TIMER_TIMEOUT_MS;
uint16 gBtpDiscTimeout = 5000 / BTP_TIMER_TIMEOUT_MS;

void btp_setConnTimeout(uint16 timeout)
{
    if (timeout < 3000) {
        timeout = 3000;
    } else if (timeout > 15000) {
        timeout = 15000;
    }
    gBtpConnTimeout = timeout / BTP_TIMER_TIMEOUT_MS;
}
void btp_setDiscTimeout(uint16 timeout)
{
    if (timeout < 3000) {
        timeout = 3000;
    } else if (timeout > 15000) {
        timeout = 15000;
    }
    gBtpDiscTimeout = timeout / BTP_TIMER_TIMEOUT_MS;
}


#endif // #if (TLK_STK_BTP_ENABLE)
