/********************************************************************************************************
 * @file	btp_funcs.h
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
#ifndef BTP_FUNCS_H
#define BTP_FUNCS_H

#if (TLK_STK_BTP_ENABLE)


enum BTP_FUNCID_SET_ENUM {
    BTP_FUNCID_NONE = 0x0000,
    // Common
    BTP_FUNCID_COMMON_START     = 0x0000,
    BTP_FUNCID_SET_CONN_TIMEOUT = 0x01 + BTP_FUNCID_COMMON_START,
    // SDP Start
    BTP_FUNCID_SDP_START       = 0x0100,
    BTP_FUNCID_SDP_SRV_CONNECT = 0x01 + BTP_FUNCID_SDP_START,
    BTP_FUNCID_SDP_SRV_DISCONN = 0x02 + BTP_FUNCID_SDP_START,
    BTP_FUNCID_SDP_CLT_CONNECT = 0x03 + BTP_FUNCID_SDP_START,
    BTP_FUNCID_SDP_CLT_DISCONN = 0x04 + BTP_FUNCID_SDP_START,
    // RFC Start
    BTP_FUNCID_RFC_START   = 0x0200,
    BTP_FUNCID_RFC_CONNECT = 0x01 + BTP_FUNCID_RFC_START,
    BTP_FUNCID_RFC_DISCONN = 0x02 + BTP_FUNCID_RFC_START,
    // IAP Start
    BTP_FUNCID_IAP_START = 0x0300,
    // SPP Start
    BTP_FUNCID_SPP_START     = 0x0400,
    BTP_FUNCID_SPP_CONNECT   = 0x01 + BTP_FUNCID_SPP_START,
    BTP_FUNCID_SPP_DISCONN   = 0x02 + BTP_FUNCID_SPP_START,
    BTP_FUNCID_SPP_SEND_DATA = 0x03 + BTP_FUNCID_SPP_START,
    BTP_FUNCID_SPP_SEND_RLS  = 0x04 + BTP_FUNCID_SPP_START,
    // ATT Start
    BTP_FUNCID_ATT_START = 0x0500,
    // A2DP Start
    BTP_FUNCID_A2DP_START                  = 0x0600,
    BTP_FUNCID_A2DP_SET_MODE               = 0x01 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SRC_CONNECT            = 0x11 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SRC_DISCONN            = 0x12 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SNK_CONNECT            = 0x13 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SNK_DISCONN            = 0x14 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SNK_DELAY_RPTORT       = 0x15 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SNK_SEND_REJECT_CFG    = 0x24 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SRC_SEND_MEDIA_DATA    = 0x16 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SRC_SEND_A2DP_START    = 0x17 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SEND_ABORT             = 0x18 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SRC_SEND_MEDIA_DATA01  = 0x19 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SRC_SEND_MEDIA_DATA02  = 0x20 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SRC_SEND_A2DP_CLOSE    = 0x21 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SRC_SET_CAPACITY_PARAM = 0x22 + BTP_FUNCID_A2DP_START,
    BTP_FUNCID_A2DP_SRC_SEND_MEDIA_DATA03  = 0x23 + BTP_FUNCID_A2DP_START,

    // HID Start
    BTP_FUNCID_HID_START         = 0x0700,
    BTP_FUNCID_HID_CONNECT       = 0x01 + BTP_FUNCID_HID_START,
    BTP_FUNCID_HID_DISCONN       = 0x02 + BTP_FUNCID_HID_START,
    BTP_FUNCID_HID_SEND_DATA     = 0x03 + BTP_FUNCID_HID_START,
    BTP_FUNCID_HID_SEND_CTR_DATA = 0x04 + BTP_FUNCID_HID_START,
    BTP_FUNCID_HID_SEND_IRQ_DATA = 0x05 + BTP_FUNCID_HID_START,
    BTP_FUNCID_HID_ENABLE_RTN    = 0x08 + BTP_FUNCID_HID_START,
    BTP_FUNCID_HID_ENABLE_QOS    = 0x09 + BTP_FUNCID_HID_START,
    BTP_FUNCID_HID_SET_COD       = 0x0A + BTP_FUNCID_HID_START,
    // HFP Start
    BTP_FUNCID_HFP_START                                 = 0x0800,
    BTP_FUNCID_HFP_HF_CONNECT                            = 0x01 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_DISCONN                            = 0x02 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_SET_FEATURE                        = 0x03 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_SET_VOLUME                         = 0x04 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_SET_BATTERY                        = 0x05 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_SET_SIGNAL                         = 0x06 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_ANSWER                             = 0x10 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_REDIAL                             = 0x11 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_DIAL                               = 0x12 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_HUNGUP                             = 0x13 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_REJECT                             = 0x14 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_REJECT_WAIT_AND_KEEP_ACTIVE        = 0x15 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_ACCEPT_WAIT_AND_HOLD_ACTIVE        = 0x16 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_HUNGUP_ACTIVE_AND_RESUME_HOLD      = 0x17 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_ACTIVATE_VOICE_RECOG               = 0x18 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_QUERY_CALL_LIST                    = 0x19 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_RLS_SPECIFIC_CALL                  = 0x20 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_PRIVATE_CONSULT_WITH_SPECIFIC_CALL = 0x21 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_UPDATE_INDICATOR                   = 0x22 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_HF_CODEC_CONNECT                      = 0x23 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_AG_CONNECT                            = 0x51 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_AG_DISCONN                            = 0x52 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_AG_SET_FEATURE                        = 0x53 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_AG_PLACE_CALL                         = 0x54 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_AG_REMOVE_CALL                        = 0x55 + BTP_FUNCID_HFP_START,
    BTP_FUNCID_HFP_AG_ACTIVE_CALL                        = 0x56 + BTP_FUNCID_HFP_START,
    // AVRCP Start
    BTP_FUNCID_AVRCP_START                 = 0x0900,
    BTP_FUNCID_AVRCP_CONNECT               = 0x01 + BTP_FUNCID_AVRCP_START,
    BTP_FUNCID_AVRCP_DISCONN               = 0x02 + BTP_FUNCID_AVRCP_START,
    BTP_FUNCID_AVRCP_SEND_KEY_PRESSED      = 0x03 + BTP_FUNCID_AVRCP_START,
    BTP_FUNCID_AVRCP_SEND_KEY_RELEASED     = 0x04 + BTP_FUNCID_AVRCP_START,
    BTP_FUNCID_AVRCP_SEND_REG_EVT_NOTY_CMD = 0x05 + BTP_FUNCID_AVRCP_START,
    BTP_FUNCID_AVRCP_SEND_EVENT_NOTY       = 0x06 + BTP_FUNCID_AVRCP_START,
    BTP_FUNCID_AVRCP_SET_TRACK_VALUE       = 0x07 + BTP_FUNCID_AVRCP_START,
    BTP_FUNCID_AVRCP_SET_PLAY_STATUS       = 0x08 + BTP_FUNCID_AVRCP_START,
    BTP_FUNCID_BROWSING_CONNECT            = 0x09 + BTP_FUNCID_AVRCP_START,
    BTP_FUNCID_BROWSING_DISCONN            = 0x0A + BTP_FUNCID_AVRCP_START,
    // OBEX Start
    BTP_FUNCID_OBEX_START = 0x0A00,
    // PBAP Start
    BTP_FUNCID_PBAP_START              = 0x0B00,
    BTP_FUNCID_PBAP_CONNECT            = 0x01 + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_DISCONN            = 0x02 + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_ENABLE_RTN         = 0x03 + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_SEND_GET_REQ       = 0x04 + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_SEND_GET_CONTINUE  = 0x05 + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_SEND_GET_REQ1      = 0x06 + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_SEND_SYNC_BOOK     = 0x07 + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_CONNECT_WITH_AUTH  = 0x08 + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_SET_FOLDER_PATH    = 0x09 + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_SEND_GET_REQ2      = 0x0A + BTP_FUNCID_PBAP_START,
    BTP_FUNCID_PBAP_SEND_GET_CONTINUE2 = 0x0B + BTP_FUNCID_PBAP_START,
    // PTS-L2CAP Start
    BTP_FUNCID_PTSL2C_START      = 0x0C00,
    BTP_FUNCID_PTSL2C_CONNECT    = 0x01 + BTP_FUNCID_PTSL2C_START,
    BTP_FUNCID_PTSL2C_DISCONN    = 0x02 + BTP_FUNCID_PTSL2C_START,
    BTP_FUNCID_PTSL2C_ENABLE_RTN = 0x03 + BTP_FUNCID_PTSL2C_START,
    BTP_FUNCID_PTSL2C_ENABLE_FCS = 0x04 + BTP_FUNCID_PTSL2C_START,
    BTP_FUNCID_PTSL2C_SEND_DATA  = 0x05 + BTP_FUNCID_PTSL2C_START,
    BTP_FUNCID_PTSL2C_ENABLE_EFS = 0x06 + BTP_FUNCID_PTSL2C_START,

    //AVDTP
	BTP_FUNCID_AVDTP_START 					= 0x0D00,
	BTP_FUNCID_AVDTP_SEND_MEDIA_DATA 		= 0x01 + BTP_FUNCID_AVDTP_START,
	BTP_FUNCID_AVDTP_SEND_MEDIA_DATA01 		= 0x02 + BTP_FUNCID_AVDTP_START,
	BTP_FUNCID_AVDTP_SEND_ClOSE_REQ 		= 0x03 + BTP_FUNCID_AVDTP_START,
	BTP_FUNCID_AVDTP_SEND_ABORT_REQ 		= 0x04 + BTP_FUNCID_AVDTP_START,
	BTP_FUNCID_AVDTP_SEND_GET_ALL_CAP 		= 0x05 + BTP_FUNCID_AVDTP_START,
	BTP_FUNCID_AVDTP_SEND_GET_SET_CFG 		= 0x06 + BTP_FUNCID_AVDTP_START,
};


typedef struct {
    uint16 funcID;
    int (*Enter)(uint08 *pData, uint16 dataLen);
} btp_func_item_t;


int btp_func_call(uint16 funcID, uint08 *pData, uint16 dataLen);

void btp_func_setAclHandle(uint16 aclHandle);
void btp_func_setSppRfcChn(uint08 rfcChannel);
void btp_func_setSppHfpHfChn(uint08 rfcChannel);
void btp_func_setSppHfpAgChn(uint08 rfcChannel);


static int btp_func_setConnTimeout(uint08 *pData, uint16 dataLen);

static int btp_func_sdpSrvConnect(uint08 *pData, uint16 dataLen);
static int btp_func_sdpSrvDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_sdpCltConnect(uint08 *pData, uint16 dataLen);
static int btp_func_sdpCltDisconn(uint08 *pData, uint16 dataLen);

static int btp_func_rfcConnect(uint08 *pData, uint16 dataLen);
static int btp_func_rfcDisconn(uint08 *pData, uint16 dataLen);

static int btp_func_sppConnect(uint08 *pData, uint16 dataLen);
static int btp_func_sppDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_sppSendData(uint08 *pData, uint16 dataLen);
static int btp_func_sppSendRLS(uint08 *pData, uint16 dataLen);

static int btp_func_a2dpSetMode(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcConnect(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSnkConnect(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSnkDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSnkDelayReport(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSnkSendRejectCfg(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcSendMediaData(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcSendMediaData01(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcSendMediaData02(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcSendMediaData03(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcSendStart(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcSendClose(uint08 *pData, uint16 dataLen);
static int btp_func_a2dpSrcSetCapacityParam(uint08 *pData, uint16 dataLen);

static int btp_func_avrcpConnect(uint08 *pData, uint16 dataLen);
static int btp_func_avrcpDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_avrcpSendKeyPressed(uint08 *pData, uint16 dataLen);
static int btp_func_avrcpSendKeyReleased(uint08 *pData, uint16 dataLen);
static int btp_func_avrcpSendRegEvtNotyCmd(uint08 *pData, uint16 dataLen);
static int btp_func_avrcpSendEventNoty(uint08 *pData, uint16 dataLen);
static int btp_func_avrcpSetTrackValue(uint08 *pData, uint16 dataLen);
static int btp_func_avrcpSetPlayStatus(uint08 *pData, uint16 dataLen);
static int btp_func_browsingConnect(uint08 *pData, uint16 dataLen);
static int btp_func_browsingDisconn(uint08 *pData, uint16 dataLen);

static int btp_func_hfpHfConnect(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfSetFeature(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfSetVolume(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfSetBattery(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfSetSignal(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfAnswer(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfRedial(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfDial(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfHUngup(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfReject(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfqueryCallList(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfActivateVoiceRecog(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfRejectWaitAndKeepActive(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfAcceptWaitAndHoldActive(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfHungupActiveAndResumeHold(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfReleaseSpecificCall(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfPrivateConsultWithSpecifcCall(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfIndicatorUpdate(uint08 *pData, uint16 dataLen);
static int btp_func_hfpHfCodecConnect(uint08 *pData, uint16 dataLen);
static int btp_func_hfpAgConnect(uint08 *pData, uint16 dataLen);
static int btp_func_hfpAgDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_hfpAgSetFeature(uint08 *pData, uint16 dataLen);
static int btp_func_hfpAgPlaceCall(uint08 *pData, uint16 dataLen);
static int btp_func_hfpAgRemoveCall(uint08 *pData, uint16 dataLen);
static int btp_func_hfpAgActiveCall(uint08 *pData, uint16 dataLen);

static int btp_func_pbapConnect(uint08 *pData, uint16 dataLen);
static int btp_func_pbapDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_pbapEnableRtn(uint08 *pData, uint16 dataLen);
static int btp_func_pbapSendGetReqTest(uint08 *pData, uint16 dataLen);
static int btp_func_pbapSendGetContinueTest(uint08 *pData, uint16 dataLen);
static int btp_func_pbapSendGetReqTest1(uint08 *pData, uint16 dataLen);
static int btp_func_pbapSyncBook(uint08 *pData, uint16 dataLen);
static int btp_func_pbapConnectWithAuth(uint08 *pData, uint16 dataLen);
static int btp_func_pbapSetFolderPath(uint08 *pData, uint16 dataLen);
static int btp_func_pbapSendGetReqTest2(uint08 *pData, uint16 dataLen);
static int btp_func_pbapSendGetContinueTest2(uint08 *pData, uint16 dataLen);

static int btp_func_avdtpSrcSendMediaData(uint08 *pData, uint16 dataLen);
static int btp_func_avdtpSrcSendMediaData01(uint08 *pData, uint16 dataLen);
static int btp_func_avdtpSrcSendCloseReq(uint08 *pData, uint16 dataLen);
static int btp_func_avdtpSrcSendAbortReq(uint08 *pData, uint16 dataLen);
static int btp_func_avdtpSrcSendGetAllCap(uint08 *pData, uint16 dataLen);
static int btp_func_avdtpSrcSendSetCfg(uint08 *pData, uint16 dataLen);

static int btp_func_ptsl2cConnect(uint08 *pData, uint16 dataLen);
static int btp_func_ptsl2cDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_ptsl2cEnableRtn(uint08 *pData, uint16 dataLen);
static int btp_func_ptsl2cEnableFcs(uint08 *pData, uint16 dataLen);
static int btp_func_ptsl2cSendData(uint08 *pData, uint16 dataLen);
static int btp_func_ptsl2cEnableEfs(uint08 *pData, uint16 dataLen);

static int btp_func_hidConnect(uint08 *pData, uint16 dataLen);
static int btp_func_hidDisconn(uint08 *pData, uint16 dataLen);
static int btp_func_hidSendData(uint08 *pData, uint16 dataLen);
static int btp_func_hidSendCtrData(uint08 *pData, uint16 dataLen);
static int btp_func_hidSendIrqData(uint08 *pData, uint16 dataLen);
static int btp_func_hidEnableRtn(uint08 *pData, uint16 dataLen);
static int btp_func_hidEnableQos(uint08 *pData, uint16 dataLen);
static int btp_func_hidSetCod(uint08 *pData, uint16 dataLen);


#endif // #if (TLK_STK_BTP_ENABLE)

#endif // BTP_FUNCS_H
