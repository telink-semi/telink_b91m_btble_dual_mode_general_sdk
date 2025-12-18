/********************************************************************************************************
 * @file	btp_funcs.c
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
#if (TLK_STK_BTP_ENABLE && TLK_CFG_TEST_ENABLE)
#include "btp_stdio.h"
#include "btp.h"
#include "btp_funcs.h"
#include "btp_module.h"
#include "sdp/btp_sdp.h"
#include "a2dp/btp_a2dp.h"
#include "avrcp/btp_avrcp.h"
#include "rfcomm/btp_rfcomm.h"
#include "hfp/btp_hfp.h"
#include "spp/btp_spp.h"
#include "a2dp/btp_a2dp.h"
#include "pbap/btp_pbap.h"
#include "hid/btp_hid.h"
#include "att/btp_att.h"
#include "pts/btp_ptsL2c.h"
#include "pts/btp_ptsHid.h"
#include "browsing/btp_browsing.h"



#define BTP_FUNC_DBG_FLAG ((TLK_MAJOR_DBGID_BTH << 24) | (TLK_MINOR_DBGID_BTH_FUNC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_FUNC_DBG_SIGN nullptr

extern int btp_hfphf_connect(uint16 aclHandle, uint08 channel);
extern int btp_hfphf_disconn(uint16 aclHandle);
extern int btp_hfpag_connect(uint16 aclHandle, uint08 channel);
extern int btp_hfpag_disconn(uint16 aclHandle);
extern int bth_signal_sendRejectRsp(uint16 handle, uint08 identify, uint16 reason, uint08 *pData, uint16 dataLen);
extern int btp_hfphf_releaseSpecificCall(uint16 aclHandle, uint08 index);
extern int btp_hfphf_privateConsultWithSpecificCall(uint16 aclHandle, uint08 index);
extern int btp_hfphf_codecConn(uint16 aclHandle);

static uint16 sBtpFuncAclHandle            = 0;
static uint16 sBtpFuncSppRfcChn            = 0;
static uint16 sBtpFuncHfpAgRfcChn          = 0;
static uint16 sBtpFuncHfpHfRfcChn          = 0;
static const btp_func_item_t scBtpFunSet[] = {
  //  btp_setConnTimeout
    {BTP_FUNCID_SET_CONN_TIMEOUT,                           btp_func_setConnTimeout                    },
 //  SDP
    { BTP_FUNCID_SDP_SRV_CONNECT,                           btp_func_sdpSrvConnect                     },
    { BTP_FUNCID_SDP_SRV_DISCONN,                           btp_func_sdpSrvDisconn                     },
    { BTP_FUNCID_SDP_CLT_CONNECT,                           btp_func_sdpCltConnect                     },
    { BTP_FUNCID_SDP_CLT_DISCONN,                           btp_func_sdpCltDisconn                     },
 //  RFC
    { BTP_FUNCID_RFC_CONNECT,                               btp_func_rfcConnect                        },
    { BTP_FUNCID_RFC_DISCONN,                               btp_func_rfcDisconn                        },
 //  SPP
    { BTP_FUNCID_SPP_CONNECT,                               btp_func_sppConnect                        },
    { BTP_FUNCID_SPP_DISCONN,                               btp_func_sppDisconn                        },
    { BTP_FUNCID_SPP_SEND_DATA,                             btp_func_sppSendData                       },
    { BTP_FUNCID_SPP_SEND_RLS,                              btp_func_sppSendRLS                        },
 //  A2DP
    { BTP_FUNCID_A2DP_SET_MODE,                             btp_func_a2dpSetMode                       },
    { BTP_FUNCID_A2DP_SRC_CONNECT,                          btp_func_a2dpSrcConnect                    },
    { BTP_FUNCID_A2DP_SRC_DISCONN,                          btp_func_a2dpSrcDisconn                    },
    { BTP_FUNCID_A2DP_SNK_CONNECT,                          btp_func_a2dpSnkConnect                    },
    { BTP_FUNCID_A2DP_SNK_DISCONN,                          btp_func_a2dpSnkDisconn                    },
    { BTP_FUNCID_A2DP_SNK_DELAY_RPTORT,                     btp_func_a2dpSnkDelayReport                },
    { BTP_FUNCID_A2DP_SNK_SEND_REJECT_CFG,                  btp_func_a2dpSnkSendRejectCfg              },
    { BTP_FUNCID_A2DP_SRC_SEND_A2DP_START,                  btp_func_a2dpSrcSendStart                  },
    { BTP_FUNCID_A2DP_SRC_SEND_MEDIA_DATA,                  btp_func_a2dpSrcSendMediaData              },
    { BTP_FUNCID_A2DP_SRC_SEND_MEDIA_DATA01,                btp_func_a2dpSrcSendMediaData01            },
    { BTP_FUNCID_A2DP_SRC_SEND_MEDIA_DATA02,                btp_func_a2dpSrcSendMediaData02            },
    { BTP_FUNCID_A2DP_SRC_SEND_MEDIA_DATA03,                btp_func_a2dpSrcSendMediaData03            },

    { BTP_FUNCID_A2DP_SRC_SEND_A2DP_CLOSE,                  btp_func_a2dpSrcSendClose                  },
    { BTP_FUNCID_A2DP_SRC_SET_CAPACITY_PARAM,               btp_func_a2dpSrcSetCapacityParam           },
 //  HID
    { BTP_FUNCID_HID_CONNECT,                               btp_func_hidConnect                        },
    { BTP_FUNCID_HID_DISCONN,                               btp_func_hidDisconn                        },
    { BTP_FUNCID_HID_SEND_DATA,                             btp_func_hidSendData                       },
    { BTP_FUNCID_HID_SEND_CTR_DATA,                         btp_func_hidSendCtrData                    },
    { BTP_FUNCID_HID_SEND_IRQ_DATA,                         btp_func_hidSendIrqData                    },
    { BTP_FUNCID_HID_ENABLE_RTN,                            btp_func_hidEnableRtn                      },
    { BTP_FUNCID_HID_ENABLE_QOS,                            btp_func_hidEnableQos                      },
    { BTP_FUNCID_HID_SET_COD,                               btp_func_hidSetCod                         },
 //  AVRCP
    { BTP_FUNCID_AVRCP_CONNECT,                             btp_func_avrcpConnect                      },
    { BTP_FUNCID_AVRCP_DISCONN,                             btp_func_avrcpDisconn                      },
    { BTP_FUNCID_AVRCP_SEND_KEY_PRESSED,                    btp_func_avrcpSendKeyPressed               },
    { BTP_FUNCID_AVRCP_SEND_KEY_RELEASED,                   btp_func_avrcpSendKeyReleased              },
    { BTP_FUNCID_AVRCP_SEND_REG_EVT_NOTY_CMD,               btp_func_avrcpSendRegEvtNotyCmd            },
    { BTP_FUNCID_AVRCP_SEND_EVENT_NOTY,                     btp_func_avrcpSendEventNoty                },
    { BTP_FUNCID_AVRCP_SET_TRACK_VALUE,                     btp_func_avrcpSetTrackValue                },
    { BTP_FUNCID_AVRCP_SET_PLAY_STATUS,                     btp_func_avrcpSetPlayStatus                },
    { BTP_FUNCID_BROWSING_CONNECT,                          btp_func_browsingConnect                   },
    { BTP_FUNCID_BROWSING_DISCONN,                          btp_func_browsingDisconn                   },
 //  HFP
    { BTP_FUNCID_HFP_HF_CONNECT,                            btp_func_hfpHfConnect                      },
    { BTP_FUNCID_HFP_HF_DISCONN,                            btp_func_hfpHfDisconn                      },
    { BTP_FUNCID_HFP_HF_SET_FEATURE,                        btp_func_hfpHfSetFeature                   },
    { BTP_FUNCID_HFP_HF_SET_VOLUME,                         btp_func_hfpHfSetVolume                    },
    { BTP_FUNCID_HFP_HF_SET_BATTERY,                        btp_func_hfpHfSetBattery                   },
    { BTP_FUNCID_HFP_HF_SET_SIGNAL,                         btp_func_hfpHfSetSignal                    },
    { BTP_FUNCID_HFP_HF_ANSWER,                             btp_func_hfpHfAnswer                       },
    { BTP_FUNCID_HFP_HF_REDIAL,                             btp_func_hfpHfRedial                       },
    { BTP_FUNCID_HFP_HF_DIAL,                               btp_func_hfpHfDial                         },
    { BTP_FUNCID_HFP_HF_HUNGUP,                             btp_func_hfpHfHUngup                       },
    { BTP_FUNCID_HFP_HF_REJECT,                             btp_func_hfpHfReject                       },
    { BTP_FUNCID_HFP_HF_QUERY_CALL_LIST,                    btp_func_hfpHfqueryCallList                },
    { BTP_FUNCID_HFP_HF_ACTIVATE_VOICE_RECOG,               btp_func_hfpHfActivateVoiceRecog           },
    { BTP_FUNCID_HFP_HF_REJECT_WAIT_AND_KEEP_ACTIVE,        btp_func_hfpHfRejectWaitAndKeepActive      },
    { BTP_FUNCID_HFP_HF_ACCEPT_WAIT_AND_HOLD_ACTIVE,        btp_func_hfpHfAcceptWaitAndHoldActive      },
    { BTP_FUNCID_HFP_HF_HUNGUP_ACTIVE_AND_RESUME_HOLD,      btp_func_hfpHfHungupActiveAndResumeHold    },
    { BTP_FUNCID_HFP_HF_RLS_SPECIFIC_CALL,                  btp_func_hfpHfReleaseSpecificCall          },
    { BTP_FUNCID_HFP_HF_PRIVATE_CONSULT_WITH_SPECIFIC_CALL, btp_func_hfpHfPrivateConsultWithSpecifcCall},
    { BTP_FUNCID_HFP_HF_UPDATE_INDICATOR,                   btp_func_hfpHfIndicatorUpdate              },
    { BTP_FUNCID_HFP_HF_CODEC_CONNECT,                      btp_func_hfpHfCodecConnect                 },
    { BTP_FUNCID_HFP_AG_CONNECT,                            btp_func_hfpAgConnect                      },
    { BTP_FUNCID_HFP_AG_DISCONN,                            btp_func_hfpAgDisconn                      },
    { BTP_FUNCID_HFP_AG_SET_FEATURE,                        btp_func_hfpAgSetFeature                   },
    { BTP_FUNCID_HFP_AG_PLACE_CALL,                         btp_func_hfpAgPlaceCall                    },
    { BTP_FUNCID_HFP_AG_REMOVE_CALL,                        btp_func_hfpAgRemoveCall                   },
    { BTP_FUNCID_HFP_AG_ACTIVE_CALL,                        btp_func_hfpAgActiveCall                   },
 //  PBAP
    { BTP_FUNCID_PBAP_CONNECT,                              btp_func_pbapConnect                       },
    { BTP_FUNCID_PBAP_DISCONN,                              btp_func_pbapDisconn                       },
    { BTP_FUNCID_PBAP_ENABLE_RTN,                           btp_func_pbapEnableRtn                     },
    { BTP_FUNCID_PBAP_SEND_GET_REQ,                         btp_func_pbapSendGetReqTest                },
    { BTP_FUNCID_PBAP_SEND_GET_CONTINUE,                    btp_func_pbapSendGetContinueTest           },
    { BTP_FUNCID_PBAP_SEND_GET_REQ1,                        btp_func_pbapSendGetReqTest1               },
    { BTP_FUNCID_PBAP_SEND_SYNC_BOOK,                       btp_func_pbapSyncBook                      },
    { BTP_FUNCID_PBAP_CONNECT_WITH_AUTH,                    btp_func_pbapConnectWithAuth               },
    { BTP_FUNCID_PBAP_SET_FOLDER_PATH,                      btp_func_pbapSetFolderPath                 },
    { BTP_FUNCID_PBAP_SEND_GET_REQ2,                        btp_func_pbapSendGetReqTest2               },
    { BTP_FUNCID_PBAP_SEND_GET_CONTINUE2,                   btp_func_pbapSendGetContinueTest2          },
 //  PTS-L2CAP
    { BTP_FUNCID_PTSL2C_CONNECT,                            btp_func_ptsl2cConnect                     },
    { BTP_FUNCID_PTSL2C_DISCONN,                            btp_func_ptsl2cDisconn                     },
    { BTP_FUNCID_PTSL2C_ENABLE_RTN,                         btp_func_ptsl2cEnableRtn                   },
    { BTP_FUNCID_PTSL2C_ENABLE_FCS,                         btp_func_ptsl2cEnableFcs                   },
    { BTP_FUNCID_PTSL2C_SEND_DATA,                          btp_func_ptsl2cSendData                    },
    { BTP_FUNCID_PTSL2C_ENABLE_EFS,                         btp_func_ptsl2cEnableEfs                   },
 //  AVDTP
	{ BTP_FUNCID_AVDTP_SEND_MEDIA_DATA,						btp_func_avdtpSrcSendMediaData             },
	{ BTP_FUNCID_AVDTP_SEND_MEDIA_DATA01,					btp_func_avdtpSrcSendMediaData01           },
	{ BTP_FUNCID_AVDTP_SEND_ClOSE_REQ,						btp_func_avdtpSrcSendCloseReq          	   },
	{ BTP_FUNCID_AVDTP_SEND_ABORT_REQ, 						btp_func_avdtpSrcSendAbortReq			   },
	{ BTP_FUNCID_AVDTP_SEND_GET_ALL_CAP,				    btp_func_avdtpSrcSendGetAllCap		       },
	{ BTP_FUNCID_AVDTP_SEND_GET_SET_CFG, 					btp_func_avdtpSrcSendSetCfg		           },
};


int btp_func_call(uint16 funcID, uint08 *pData, uint16 dataLen)
{
    int index;
    int count;

    count = sizeof(scBtpFunSet) / sizeof(scBtpFunSet[0]);
    for (index = 0; index < count; index++) {
        if (scBtpFunSet[index].funcID == funcID) {
            break;
        }
    }
    if (index == count || scBtpFunSet[index].Enter == nullptr) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "Function ID is not in BTP !");
        return -TLK_EFAIL;
    }
    return scBtpFunSet[index].Enter(pData, dataLen);
}
void btp_func_setAclHandle(uint16 aclHandle)
{
    sBtpFuncAclHandle = aclHandle;
}
void btp_func_setSppRfcChn(uint08 rfcChannel)
{
    sBtpFuncSppRfcChn = rfcChannel;
}
void btp_func_setSppHfpHfChn(uint08 rfcChannel)
{
    sBtpFuncHfpHfRfcChn = rfcChannel;
}
void btp_func_setSppHfpAgChn(uint08 rfcChannel)
{
    sBtpFuncHfpAgRfcChn = rfcChannel;
}


static int btp_func_setConnTimeout(uint08 *pData, uint16 dataLen)
{
    uint16 timeout;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_setConnTimeout: failure - param error");
        return -TLK_EPARAM;
    }
    timeout = ((uint16)pData[1] << 8 | pData[0]);
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_setConnTimeout: timeout[%dms]", timeout);
    btp_setConnTimeout(timeout);
    return TLK_ENONE;
}
static int btp_func_sdpSrvConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpSrvConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpSrvConnect: handle[0x%x]", handle);
    return btp_sdp_connect(handle, BTP_USRID_SERVER);
}
static int btp_func_sdpSrvDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpSrvDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpSrvDisconn: handle[0x%x]", handle);
    return btp_sdp_connect(handle, BTP_USRID_SERVER);
}
static int btp_func_sdpCltConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpCltConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpCltConnect: handle[0x%x]", handle);
    return btp_sdp_connect(handle, BTP_USRID_CLIENT);
}
static int btp_func_sdpCltDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpCltDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sdpCltDisconn: handle[0x%x]", handle);
    return btp_sdp_connect(handle, BTP_USRID_CLIENT);
}

static int btp_func_rfcConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_rfcConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_rfcomm_connect(handle);
}
static int btp_func_rfcDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_rfcDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_rfcomm_disconn(handle);
}

static int btp_func_sppConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 channel;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    if (pData[2] == 0x00) {
        channel = sBtpFuncSppRfcChn;
    } else {
        channel = pData[2];
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_sppConnect: handle[0x%x] channel[%d]",
                 handle,
                 channel);
    return btp_spp_connect(handle, channel);
}
static int btp_func_sppDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppDisconn: handle[0x%x]", handle);
    return btp_spp_disconn(handle);
}
static int btp_func_sppSendData(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppSendData: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_sppSendData: handle[0x%x],Length[%d]",
                 handle,
                 dataLen - 2);
    return btp_spp_sendData(handle, pData + 2, dataLen - 2, nullptr, 0);
}
static int btp_func_sppSendRLS(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_sppSendRLS: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_sppSendRLS: handle[0x%x],Length[%d]",
                 handle,
                 dataLen - 2);
    return btp_spp_sendRemoteLineStatus(handle, pData[2]);
}

static int btp_func_a2dpSetMode(uint08 *pData, uint16 dataLen)
{
    uint08 mode;
    uint16 handle;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSetMode: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    mode   = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSetMode: handle[0x%x], mode[%d]", handle, mode);
    return btp_a2dp_setMode(handle, mode);
}
static int btp_func_a2dpSrcConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcConnect: handle[0x%x]", handle);
    return btp_a2dp_connect(handle, BTP_USRID_SERVER);
}
static int btp_func_a2dpSrcDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcDisconn: handle[0x%x]", handle);
    return btp_a2dp_disconn(handle);
}
static int btp_func_a2dpSnkConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkConnect: handle[0x%x]", handle);
    return btp_a2dp_connect(handle, BTP_USRID_CLIENT);
}
static int btp_func_a2dpSnkDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkDisconn: handle[0x%x]", handle);
    return btp_a2dp_disconn(handle);
}
static int btp_func_a2dpSnkDelayReport(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint16 delay;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkDelayReport: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    delay  = ((uint16)pData[3] << 8 | pData[2]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    if (delay == 0) {
        delay = 1000;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_a2dpSnkDelayReport: handle[0x%x] delay[0x%x]",
                 handle,
                 delay);
    return btp_a2dpsnk_delayreport(handle, delay);
}
static int btp_func_a2dpSrcSendStart(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendA2DPStart: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendA2DPStart: handle[0x%x]", handle);
    return btp_a2dpsrc_start(handle);
}
static int btp_func_a2dpSrcSendClose(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendClose: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendClose: handle[0x%x]", handle);
    return btp_a2dpsrc_close(handle);
}
static int btp_func_a2dpSrcSendMediaData(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    // subbands:8	block_length:16    sample:44.1    Joint_stereo    bit_pool:53
    uint08 SBCBuffer[] = { 0x05, 0x9c, 0xbd, 0x35, 0xd0, 0x7a, 0x97, 0x54, 0x22, 0x22, 0xb8, 0x53, 0x22, 0x12, 0x6a,
                           0x81, 0x5a, 0xaa, 0x12, 0x29, 0x51, 0x82, 0xcd, 0x93, 0x0e, 0x70, 0xa2, 0x72, 0x7a, 0xb6,
                           0x99, 0x63, 0xd1, 0x1d, 0xb4, 0x15, 0x60, 0xc1, 0xe0, 0x16, 0xc9, 0x07, 0x9b, 0xd6, 0x06,
                           0x98, 0x46, 0x18, 0x3e, 0x72, 0x38, 0xeb, 0x1d, 0x51, 0xc3, 0x2c, 0x8d, 0x81, 0xd4, 0xf2,
                           0x89, 0x77, 0x36, 0xab, 0x39, 0x25, 0x2d, 0x7a, 0x92, 0xa4, 0x99, 0xd7, 0xaa, 0x4f, 0x58,
                           0x56, 0x08, 0xb3, 0x4f, 0x2b, 0x2c, 0xea, 0x92, 0x14, 0x74, 0xb4, 0x5a, 0x31, 0x99, 0x66,
                           0xbe, 0xea, 0x66, 0x96, 0x1a, 0x65, 0xb5, 0xf8, 0xdd, 0x40, 0x3b, 0xbf, 0x2a, 0xa7, 0xe5,
                           0xed, 0x11, 0x30, 0xd9, 0x6e, 0x58, 0xcf, 0x50, 0xbd, 0x87, 0x54, 0x92, 0xe1, 0x68, 0x52,
                           0x9c, 0xbd, 0x35, 0xdc, 0xfe, 0xa8, 0x53, 0x21, 0x11, 0xa7, 0x53, 0x21, 0x02, 0x39, 0x60,
                           0xe9, 0x9e, 0x6d, 0x5c, 0x82, 0x25, 0xc9, 0x55, 0x8d, 0xae, 0xdc, 0x52, 0xce, 0xba, 0xd4,
                           0x85, 0xb2, 0x88, 0x68, 0x5d, 0x86, 0xb5, 0x8f, 0x87, 0x10, 0xab, 0x0e, 0xad, 0xa6, 0xac,
                           0xb6, 0x2d, 0xea, 0x87, 0x0c, 0xa1, 0x6a, 0xa5, 0x77, 0x01, 0xe5, 0x6c, 0x74, 0xe9, 0x2a,
                           0xe6, 0x1d, 0xa6, 0x6e, 0x48, 0x8d, 0x78, 0x36, 0x96, 0x08, 0xd8, 0xcc, 0x74, 0x01, 0xc4,
                           0x75, 0x99, 0xd5, 0xcd, 0x19, 0xd4, 0x3d, 0x2b, 0xfc, 0xd0, 0xab, 0x3e, 0x84, 0x95, 0x1a,
                           0x2e, 0xba, 0x99, 0xfa, 0xd2, 0x1d, 0x22, 0x84, 0x27, 0x4b, 0x1f, 0xa8, 0xa7, 0x5b, 0x1a,
                           0x36, 0x36, 0xfb, 0xb6, 0x93, 0xe0, 0xf2, 0x13, 0xfa, 0xd9, 0xc9, 0xc1, 0x45, 0x4e };
    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendMediaData: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_sendMediaData(handle, 0, 0, SBCBuffer, sizeof(SBCBuffer));
}
static int btp_func_a2dpSrcSendMediaData01(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    // subbands:8	block_length:16    sample:48    Joint_stereo    bit_pool:51
    uint08 SBCBuffer[] = {
        0x05, 0x9c, 0xfd, 0x33, 0xfd, 0x8e, 0xc8, 0x52, 0x32, 0x23, 0xcb, 0x83, 0x21, 0x14, 0xc4, 0x04, 0xa5, 0x44,
        0x8f, 0x74, 0x32, 0x26, 0x42, 0xaf, 0xe5, 0x47, 0x45, 0x63, 0x6b, 0x96, 0x60, 0x79, 0xaa, 0xc5, 0xd4, 0x82,
        0xb8, 0x50, 0xc8, 0x5a, 0xa6, 0x6d, 0x67, 0x76, 0x57, 0xca, 0x6d, 0x2e, 0x31, 0x6c, 0xb7, 0x00, 0x59, 0xd3,
        0x49, 0x5e, 0x65, 0xe6, 0xb6, 0x6b, 0xc5, 0x29, 0x89, 0x4b, 0x96, 0xa3, 0x44, 0x44, 0x62, 0x9b, 0x62, 0xcf,
        0xc4, 0xe4, 0xb0, 0x73, 0x14, 0x8a, 0x5f, 0x7b, 0x52, 0x0d, 0x8a, 0x21, 0x68, 0xcf, 0x4a, 0xa8, 0x0e, 0xb1,
        0x57, 0xc9, 0x8d, 0x07, 0x19, 0xc7, 0x8a, 0x4e, 0xf1, 0xa3, 0x2f, 0x5c, 0xf5, 0x9f, 0x01, 0x36, 0x58, 0xa5,
        0x1a, 0x96, 0xd6, 0xd8, 0x22, 0x8c, 0xe4, 0x9a, 0x9c, 0xfd, 0x33, 0x92, 0x8e, 0xc8, 0x52, 0x22, 0x23, 0xba,
        0x83, 0x21, 0x15, 0xb3, 0x79, 0xaa, 0xb2, 0x3d, 0xab, 0x30, 0x72, 0x0d, 0x09, 0x31, 0x8e, 0x66, 0x66, 0x81,
        0xa2, 0xbb, 0x99, 0xd4, 0xd4, 0x58, 0xc0, 0x44, 0xb1, 0x44, 0x92, 0x78, 0x06, 0x8d, 0x40, 0x87, 0x2a, 0x9d,
        0x62, 0xa1, 0xd3, 0xd4, 0xc6, 0x73, 0x18, 0x9a, 0x27, 0xbb, 0x5f, 0x2e, 0x16, 0x42, 0x48, 0xb0, 0x13, 0x15,
        0x8a, 0xee, 0x04, 0xbd, 0x6d, 0x3a, 0xab, 0x9a, 0xed, 0x99, 0x7c, 0x0f, 0x56, 0x4e, 0xd1, 0xc2, 0x71, 0x86,
        0x2a, 0xd4, 0xea, 0xb1, 0x4f, 0x55, 0x3d, 0x53, 0xf7, 0x08, 0x2f, 0x6d, 0xe8, 0x8a, 0xe1, 0x01, 0x25, 0x9e,
        0x13, 0x39, 0x6f, 0x8e, 0x52, 0xa2, 0xfd, 0x1c, 0xac, 0x50, 0x93, 0x49, 0xa8, 0xe3, 0xd4
    };
    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendMediaData01: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_sendMediaData(handle, 0, 0, SBCBuffer, sizeof(SBCBuffer));
}
static int btp_func_a2dpSrcSendMediaData02(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    uint08 SBCBuffer[] = { 0x0f, 0x9c, 0xf2, 0x12, 0x9A, 0x00, 0x00, 0x7b, 0xdd, 0xde, 0xf7, 0x77, 0xbd, 0xdd, 0xef,
                           0x77, 0x7b, 0xdd, 0xde, 0xf7, 0x77, 0xbd, 0xdd, 0xef, 0x77, 0x7b, 0xdd, 0xde, 0xf7, 0x77,
                           0xbd, 0xdd, 0xef, 0x77, 0x7b, 0xdd, 0xde, 0xf7, 0x77, 0xbd, 0xdd, 0xef, 0x77, 0x9c, 0xf2,
                           0x12, 0x9A, 0x00, 0x00, 0x7b, 0xdd, 0xde, 0xf7, 0x77, 0xbd, 0xdd, 0xef, 0x77, 0x7b, 0xdd,
                           0xde, 0xf7, 0x77, 0xbd, 0xdd, 0xef, 0x77, 0x7b, 0xdd, 0xde, 0xf7, 0x77, 0xbd, 0xdd, 0xef,
                           0x77, 0x7b, 0xdd, 0xde, 0xf7, 0x77, 0xbd, 0xdd, 0xef, 0x77, 0x9c, 0xf2, 0x12, 0x17, 0x31,
                           0x00, 0x7e, 0xf6, 0xdf, 0xbd, 0xb7, 0xef, 0x6d, 0xfb, 0xdb, 0x7e, 0xf6, 0xdf, 0xbd, 0xb7,
                           0xef, 0x6d, 0xfb, 0xdb, 0x78, 0xe6, 0xdd, 0xc5, 0x34, 0x87, 0x75, 0x02, 0xe3, 0x79, 0x35,
                           0x99, 0xf6, 0x5a, 0xe5, 0x47, 0x61, 0xa3 };
    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendMediaData: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_sendMediaData(handle, 0, 0, SBCBuffer, sizeof(SBCBuffer));
}
static int btp_func_a2dpSrcSendMediaData03(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    uint08 SBCBuffer[] = { 0x0f, 0x9c, 0x91, 0x20, 0x34, 0xc9, 0x54, 0x23, 0x22, 0x63, 0x5b, 0x97, 0xb5, 0x5b,
                           0xe0, 0x76, 0x71, 0xb5, 0xaf, 0x48, 0x4d, 0xc4, 0x0c, 0x14, 0x6e, 0x4d, 0x60, 0x79,
                           0x8d, 0x07, 0xaa, 0x73, 0x4c, 0xa9, 0x55, 0x1a, 0x51, 0xc3, 0x51, 0xc5, 0xa9, 0x9c,
                           0x91, 0x20, 0xf2, 0xc9, 0x53, 0x22, 0x22, 0x71, 0x1e, 0x2a, 0x8a, 0x70, 0x2f, 0x0e,
                           0x71, 0x8e, 0x06, 0xba, 0x74, 0xa4, 0x32, 0x52, 0x24, 0x3c, 0x8a, 0xc1, 0x6d, 0x46,
                           0x33, 0xa3, 0x20, 0x9e, 0xb4, 0x30, 0x95, 0x82, 0xa1, 0x03, 0xb1 };
    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSendMediaData03: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_sendMediaData(handle, 0, 0, SBCBuffer, sizeof(SBCBuffer));
}
static int btp_func_a2dpSrcSetCapacityParam(uint08 *pData, uint16 dataLen)
{
    uint08 chnMode;
    uint08 frequency;
    uint08 allocaMethod;
    uint08 subbands;
    uint08 blockLength;
    uint08 minBitpool;
    uint08 maxBitpool;

    if (pData == nullptr || dataLen < 7) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSrcSetCapacityParam: failure - param error");
        return -TLK_EPARAM;
    }

    chnMode = pData[0];
    if (chnMode == 0) {
        chnMode = 0x01;
    }
    frequency = pData[1];
    if (frequency == 0) {
        frequency = 0x01 | 0x02 | 0x04 | 0x08;
    }
    allocaMethod = pData[2];
    if (allocaMethod == 0) {
        allocaMethod = 0x01;
    }
    subbands = pData[3];
    if (subbands == 0) {
        subbands = 0x01;
    }
    blockLength = pData[4];
    if (blockLength == 0) {
        blockLength = 0x01;
    }
    minBitpool = pData[5];
    if (minBitpool == 0) {
        minBitpool = 2;
    }
    maxBitpool = pData[6];
    if (maxBitpool == 0) {
        maxBitpool = 36;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_a2dpSrcSetCapacityParam: chnMode[0x%x] freq[0x%x] method[0x%x] "
                 "subbands[0x%x] blockLen [0x%x] minBitpool[0x%x] maxBitpool[0x%x]",
                 chnMode,
                 frequency,
                 allocaMethod,
                 subbands,
                 blockLength,
                 minBitpool,
                 maxBitpool);
    return btp_a2dpsrc_setCodecCapacity(chnMode,
                                        frequency,
                                        allocaMethod,
                                        subbands,
                                        blockLength,
                                        minBitpool,
                                        maxBitpool);
}
static int btp_func_a2dpSnkSendRejectCfg(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 identify;
    uint16 reason;

    if (pData == nullptr || dataLen < 5) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_a2dpSnkSendRejectCfg: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    identify = pData[2];
    reason   = ((uint16)pData[4] << 8 | pData[3]);

    return bth_signal_sendRejectRsp(handle, identify, reason, nullptr, 0);
}

static int btp_func_hidConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 userID;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    userID = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hidConnect: handle[0x%x] userID[0x%x]",
                 handle,
                 userID);
    btp_hid_connect(handle, userID);
    return TLK_ENONE;
}
static int btp_func_hidDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 userID;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    userID = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hidDisconn: handle[0x%x] userID[0x%x]",
                 handle,
                 userID);
    btp_hid_disconn(handle, userID);
    return TLK_ENONE;
}
static int btp_func_hidSendData(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 reportID;
    uint08 reportType;

    if (pData == nullptr || dataLen < 4) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidSendData: failure - param error");
        return -TLK_EPARAM;
    }

    handle     = ((uint16)pData[1] << 8 | pData[0]);
    reportID   = pData[2];
    reportType = pData[3];
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }

    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hidSendData: handle[0x%x], reportID[%d], reportType[%d]",
                 handle,
                 reportID,
                 reportType);
    btp_hidd_sendData(handle, reportID, reportType, pData + 4, dataLen - 4);
    return TLK_ENONE;
}
static int btp_func_hidSendCtrData(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidSendCtrData: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }

    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hidSendCtrData: handle[0x%x], dataLen[%d]",
                 handle,
                 dataLen);
    btp_hid_sendCtrData(handle, nullptr, 0, pData + 2, dataLen - 2);
    return TLK_ENONE;
}
static int btp_func_hidSendIrqData(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidSendIrqData: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }

    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hidSendIrqData: handle[0x%x], dataLen[%d]",
                 handle,
                 dataLen);
    btp_hid_sendIrqData(handle, nullptr, 0, pData + 2, dataLen - 2);
    return TLK_ENONE;
}
static int btp_func_hidEnableRtn(uint08 *pData, uint16 dataLen)
{
    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidEnableRtn: failure - param error");
        return -TLK_EPARAM;
    }

    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hidEnableRtn: enable[%d], rtnMode[%d]",
                 pData[0],
                 pData[1]);
    btp_hid_enableRtnMode(pData[0], pData[1]);
    return TLK_ENONE;
}
static int btp_func_hidEnableQos(uint08 *pData, uint16 dataLen)
{
    if (pData == nullptr || dataLen < 1) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidEnableQos: failure - param error");
        return -TLK_EPARAM;
    }

    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidEnableQos: enable[%d]", pData[0]);
    btp_hid_enableQos(pData[0]);
    return TLK_ENONE;
}
static int btp_func_hidSetCod(uint08 *pData, uint16 dataLen)
{
    //	if(pData == nullptr || dataLen < 1){
    //		tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidEnableQos: failure - param error");
    //		return -TLK_EPARAM;
    //	}

    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hidSetCod");
#if (TLKBTP_CFG_PTSHID_ENABLE)
    btp_ptshid_setClassOfDevice();
#endif
    return TLK_ENONE;
}


static int btp_func_avrcpConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 userID;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    userID = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_avrcpConnect: handle[0x%x] userID[0x%x]",
                 handle,
                 userID);
    btp_avrcp_connect(handle, userID);
    return TLK_ENONE;
}
static int btp_func_avrcpDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 userID;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    userID = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_avrcpDisconn: handle[0x%x] userID[0x%x]",
                 handle,
                 userID);
    btp_avrcp_disconn(handle, userID);
    return TLK_ENONE;
}
static int btp_func_avrcpSendKeyPressed(uint08 *pData, uint16 dataLen)
{
    uint08 keyID;
    uint16 handle;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendKeyPressed: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    keyID = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_avrcpSendKeyPressed: handle[0x%x] keyID[0x%x]",
                 handle,
                 keyID);
    btp_avrcp_sendKeyPress(handle, keyID);
    return TLK_ENONE;
}
static int btp_func_avrcpSendKeyReleased(uint08 *pData, uint16 dataLen)
{
    uint08 keyID;
    uint16 handle;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendKeyReleased: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    keyID = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_avrcpSendKeyReleased: handle[0x%x] keyID[0x%x]",
                 handle,
                 keyID);
    btp_avrcp_sendKeyRelease(handle, keyID);
    return TLK_ENONE;
}
static int btp_func_avrcpSendRegEvtNotyCmd(uint08 *pData, uint16 dataLen)
{
    uint08 evtID;
    uint16 handle;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendRegEvtNotyCmd: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    evtID = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_avrcpSendRegEvtNotyCmd: handle[0x%x] keyID[0x%x]",
                 handle,
                 evtID);
    btp_avrcp_sendRegEventNotify(handle, evtID);
    return TLK_ENONE;
}
static int btp_func_avrcpSendEventNoty(uint08 *pData, uint16 dataLen)
{
    uint08 evtID;
    uint16 handle;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSendEventNoty: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    evtID = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_avrcpSendEventNoty: handle[0x%x] evtID[0x%x]",
                 handle,
                 evtID);
    btp_avrcp_notifyStatusChange(handle, evtID, pData + 3, dataLen - 3);
    return TLK_ENONE;
}
static int btp_func_avrcpSetTrackValue(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 10) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSetTrackValue: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSetTrackValue: handle[0x%x]", handle);
    btp_avrcp_setTrackValue(handle, pData + 2);
    return TLK_ENONE;
}
static int btp_func_avrcpSetPlayStatus(uint08 *pData, uint16 dataLen)
{
    uint08 status;
    uint16 handle;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avrcpSetPlayStatus: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    status = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_avrcpSetPlayStatus: handle[0x%x] status[0x%x]",
                 handle,
                 status);
    btp_avrcp_setPlayState(handle, status);
    return TLK_ENONE;
}
static int btp_func_browsingConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_browsingConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_browsingConnect: handle[0x%x]", handle);
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
    btp_browsing_connect(handle);
#endif
    return TLK_ENONE;
}
static int btp_func_browsingDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_browsingDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_browsingDisconn: handle[0x%x]", handle);
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
    btp_browsing_disconn(handle);
#endif
    return TLK_ENONE;
}


static int btp_func_hfpHfConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 channel;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    channel = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hfpHfConnect: handle[0x%x] channel[0x%x]",
                 handle,
                 channel);
    return btp_hfphf_connect(handle, channel);
}
static int btp_func_hfpHfDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDisconn: handle[0x%x]", handle);
    return btp_hfphf_disconn(handle);
}
static int btp_func_hfpHfSetFeature(uint08 *pData, uint16 dataLen)
{
    uint32 feature;

    if (pData == nullptr || dataLen < 4) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfSetFeature: failure - param error");
        return -TLK_EPARAM;
    }
    feature = ((uint32)pData[3] << 24 | pData[2] << 16 | pData[1] << 8 | pData[0]);
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfSetFeature: feature[0x%x]", feature);
    btp_hfphf_setFeature(feature);
    return TLK_ENONE;
}
static int btp_func_hfpHfSetVolume(uint08 *pData, uint16 dataLen)
{
    uint08 volume;
    uint08 volumeType;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfSetVolume: failure - param error");
        return -TLK_EPARAM;
    }
    volumeType = pData[0];
    volume     = pData[1];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hfpHfSetVolume: spkVolume[0x%x] volumeType[0x%x]",
                 volume,
                 volumeType);

    if (volumeType > 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfSetVolume: failure - volumeType param error");
        return -TLK_EPARAM;
    }
    if (volumeType == BTP_HFP_VOLUME_TYPE_SPK) {
        return btp_hfphf_setSpkVolume(volume);
    } else {
        return btp_hfphf_setMicVolume(volume);
    }
}
static int btp_func_hfpHfSetBattery(uint08 *pData, uint16 dataLen)
{
    return -TLK_ENOSUPPORT;
}
static int btp_func_hfpHfSetSignal(uint08 *pData, uint16 dataLen)
{
    return -TLK_ENOSUPPORT;
}
static int btp_func_hfpHfAnswer(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfAnswer: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfAnswer: handle[0x%x]", handle);
    return btp_hfphf_answer(handle);
}
static int btp_func_hfpHfRedial(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfRedial: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfRedial: handle[0x%x]", handle);
    return btp_hfphf_redial(handle);
}
static int btp_func_hfpHfDial(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 numbLen;
    uint08 index = 0;

    if (pData == nullptr || dataLen < 4) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDial: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    numbLen = pData[2];
    char pNumber[numbLen];
    if (pData[3] == 0x3E) {
        pNumber[index++] = '>';
        pNumber[index++] = pData[4];
    } else {
        for (int i = 0; i < numbLen; i++) {
            pNumber[index++] = pData[3 + i];
        }
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDial: handle[0x%x]", handle);
    tlkapi_array(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfDial: call number: ", pNumber, numbLen);
    return btp_hfphf_dial(handle, pNumber, numbLen);
}
static int btp_func_hfpHfHUngup(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfHUngup: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfHUngup: handle[0x%x]", handle);
    return btp_hfphf_hungUp(handle);
}
static int btp_func_hfpHfReject(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfReject: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfReject: handle[0x%x]", handle);
    return btp_hfphf_reject(handle);
}
static int btp_func_hfpHfqueryCallList(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfqueryCallList: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfqueryCallList: handle[0x%x]", handle);
    return btp_hfphf_queryCallList(handle);
}
static int btp_func_hfpHfActivateVoiceRecog(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfActivateVoiceRecog: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfActivateVoiceRecog: handle[0x%x]", handle);
    return btp_hfphf_siri_ctrl(handle);
}
static int btp_func_hfpHfRejectWaitAndKeepActive(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG,
                     BTP_FUNC_DBG_SIGN,
                     "btp_func_hfpHfRejectWaitAndKeepActive: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfRejectWaitAndKeepActive: handle[0x%x]", handle);
    return btp_hfphf_rejectWaitAndKeepActive(handle);
}
static int btp_func_hfpHfAcceptWaitAndHoldActive(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG,
                     BTP_FUNC_DBG_SIGN,
                     "btp_func_hfpHfAcceptWaitAndHoldActive: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfAcceptWaitAndHoldActive: handle[0x%x]", handle);
    return btp_hfphf_acceptWaitAndHoldActive(handle);
}
static int btp_func_hfpHfHungupActiveAndResumeHold(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG,
                     BTP_FUNC_DBG_SIGN,
                     "btp_func_hfpHfHungupActiveAndResumeHold: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfHungupActiveAndResumeHold: handle[0x%x]", handle);

    return btp_hfphf_hungUpActiveAndResumeHold(handle);
}
static int btp_func_hfpHfReleaseSpecificCall(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 index;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfReleaseSpecificCall: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    index = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hfpHfReleaseSpecificCall: handle[0x%x] index[%x]",
                 handle,
                 index);

    return btp_hfphf_releaseSpecificCall(handle, index);
}
static int btp_func_hfpHfPrivateConsultWithSpecifcCall(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 index;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG,
                     BTP_FUNC_DBG_SIGN,
                     "btp_func_hfpHfPrivateConsultWithSpecifiedCall: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    index = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hfpHfPrivateConsultWithSpecifiedCall: handle[0x%x] index[%x]",
                 handle,
                 index);

    return btp_hfphf_privateConsultWithSpecificCall(handle, index);
}
static int btp_func_hfpHfCodecConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfCodecConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfCodecConnect: handle[0x%x]", handle);

    return btp_hfphf_codecConn(handle);
}
static int btp_func_hfpHfIndicatorUpdate(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint16 assNumber;
    uint16 value;

    if (pData == nullptr || dataLen < 6) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpHfIndicatorUpdate: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    assNumber = ((uint16)pData[3] << 8 | pData[2]);
    value     = ((uint16)pData[5] << 8 | pData[4]);
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hfpHfIndicatorUpdate: handle[0x%x] assNumber[%x] value[%x]",
                 handle,
                 assNumber,
                 value);

    return btp_hfphf_updateIndicator(handle, assNumber, value);
}

static int btp_func_hfpAgConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 channel;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgConnect: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    channel = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_hfpAgConnect: handle[0x%x] channel[0x%x]",
                 handle,
                 channel);
    return btp_hfpag_connect(handle, channel);
}
static int btp_func_hfpAgDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgDisconn: failure - param error");
        return -TLK_EPARAM;
    }
    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgDisconn: handle[0x%x]", handle);
    return btp_hfpag_disconn(handle);
}
static int btp_func_hfpAgSetFeature(uint08 *pData, uint16 dataLen)
{
    uint32 feature;

    if (pData == nullptr || dataLen < 4) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgSetFeature: failure - param error");
        return -TLK_EPARAM;
    }
    feature = ((uint32)pData[3] << 24 | pData[2] << 16 | pData[1] << 8 | pData[0]);
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgSetFeature: feature[0x%x]", feature);
    btp_hfpag_setFeature(feature);
    return TLK_ENONE;
}
static int btp_func_hfpAgPlaceCall(uint08 *pData, uint16 dataLen)
{
    uint08 numbLen;
    uint08 isIncoming;
    uint08 index = 0;

    if (pData == nullptr || dataLen < 9) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgPlaceCall: failure - param error");
        return -TLK_EPARAM;
    }
    numbLen = pData[7];
    char pNumber[numbLen];
    pNumber[index++] = pData[0];
    pNumber[index++] = pData[1];
    pNumber[index++] = pData[2];
    pNumber[index++] = pData[3];
    pNumber[index++] = pData[4];
    pNumber[index++] = pData[5];
    pNumber[index++] = pData[6];
    isIncoming       = pData[8];
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgPlaceCall: isIncoming[0x%x] ", isIncoming);
    tlkapi_array(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgPlaceCall: call number: ", pNumber, numbLen);
//    return btp_hfpag_insertCall(pNumber, numbLen, isIncoming);
    return -TLK_ENOSUPPORT;
}
static int btp_func_hfpAgRemoveCall(uint08 *pData, uint16 dataLen)
{
    uint08 index;
    uint08 numbLen;

    if (pData == nullptr || dataLen < 8) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgRemoveCall: failure - param error");
        return -TLK_EPARAM;
    }
    numbLen = pData[7];
    char pNumber[numbLen];

    index            = 0;
    pNumber[index++] = pData[0];
    pNumber[index++] = pData[1];
    pNumber[index++] = pData[2];
    pNumber[index++] = pData[3];
    pNumber[index++] = pData[4];
    pNumber[index++] = pData[5];
    pNumber[index++] = pData[6];
    tlkapi_array(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgRemoveCall: call number: ", pNumber, numbLen);
//    return btp_hfpag_removeCall(pNumber, numbLen);
    return -TLK_ENOSUPPORT;
}
static int btp_func_hfpAgActiveCall(uint08 *pData, uint16 dataLen)
{
    uint08 index;
    uint08 numbLen;

    if (pData == nullptr || dataLen < 8) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgActiveCall: failure - param error");
        return -TLK_EPARAM;
    }
    numbLen = pData[7];
    char pNumber[numbLen];

    index            = 0;
    pNumber[index++] = pData[0];
    pNumber[index++] = pData[1];
    pNumber[index++] = pData[2];
    pNumber[index++] = pData[3];
    pNumber[index++] = pData[4];
    pNumber[index++] = pData[5];
    pNumber[index++] = pData[6];
    tlkapi_array(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_hfpAgActiveCall: call number: ", pNumber, numbLen);
//    return btp_hfpag_activeCall(pNumber, numbLen);
    return -TLK_ENOSUPPORT;
}

static int btp_func_pbapConnect(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 usrID;
    uint16 psmOrChn;
    bool isL2cap;

    if (pData == nullptr || dataLen < 6) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapConnect: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    usrID    = pData[2];
    psmOrChn = ((uint16)pData[4] << 8 | pData[3]);
    isL2cap  = pData[5];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_pbapConnect: handle[0x%x] usrID[0x%x] psmOrChn[0x%x] isL2cap[0x%x]",
                 handle,
                 usrID,
                 psmOrChn,
                 isL2cap);
    return btp_pbap_connect(handle, usrID, psmOrChn, isL2cap);
}
static int btp_func_pbapDisconn(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 usrID;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapDisconn: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    usrID = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapDisconn: handle[0x%x] usrID[0x%x]", handle, usrID);
    return btp_pbap_disconn(handle, usrID);
}
static int btp_func_pbapEnableRtn(uint08 *pData, uint16 dataLen)
{
    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapEnableRtn: failure - param error");
        return -TLK_EPARAM;
    }

    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_pbapEnableRtn: enable[%d], rtnMode[%d]",
                 pData[0],
                 pData[1]);
    btp_pbap_enableRtnMode(pData[0], pData[1]);
    return TLK_ENONE;
}
static int btp_func_pbapSendGetReqTest(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 isWait;
    uint08 isEnSram;

    if (pData == nullptr || dataLen < 4) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapSendGetReqTest: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    isWait   = pData[2];
    isEnSram = pData[3];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_pbapSendGetReqTest: handle[0x%x] isWait[%d] isEnSram[%d]",
                 handle,
                 isWait,
                 isEnSram);
    return btp_pbapclt_sendGetReqTest(handle, isWait, isEnSram);
}
static int btp_func_pbapSendGetContinueTest(uint08 *pData, uint16 dataLen)
{
    uint08 isWait;
    uint16 handle;

    if (pData == nullptr || dataLen < 3) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapSendGetContinueTest: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    isWait = pData[2];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_pbapSendGetContinueTest: handle[0x%x] isWait[%d]",
                 handle,
                 isWait);
    return btp_pbapclt_sendGetContinueTest(handle, isWait);
}
static int btp_func_pbapSendGetReqTest1(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 isWait;
    uint08 isEnSram;

    if (pData == nullptr || dataLen < 4) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapSendGetReqTest1: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    isWait   = pData[2];
    isEnSram = pData[3];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_pbapSendGetReqTest1: handle[0x%x] isWait[%d] isEnSram[%d]",
                 handle,
                 isWait,
                 isEnSram);
    return btp_pbapclt_sendGetReqTest1(handle, isWait, isEnSram);
}
static int btp_func_pbapSyncBook(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 bookPosi;
    uint08 bookType;
    uint08 bookSort;

    if (pData == nullptr || dataLen < 5) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapSyncBook: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    bookPosi = pData[2];
    bookType = pData[3];
    bookSort = pData[4];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_pbapSyncBook: handle[0x%x] bookPosi[%d] bookType[%d] bookSort[%d]",
                 handle,
                 bookPosi,
                 bookType,
                 bookSort);
    return btp_pbapclt_startSyncBook(handle, bookPosi, bookType, bookSort, 0, 0xFFFF);
}
static int btp_func_pbapConnectWithAuth(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    //	uint08 usrID;
    uint16 psmOrChn;
    bool isL2cap;

    if (pData == nullptr || dataLen < 6) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapConnectWithAuth: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    //	usrID = pData[2];
    psmOrChn = ((uint16)pData[4] << 8 | pData[3]);
    isL2cap  = pData[5];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_pbapConnectWithAuth: handle[0x%x] psmOrChn[0x%x] isL2cap[0x%x]",
                 handle,
                 psmOrChn,
                 isL2cap);
    return btp_pbapclt_connectWithAuth(handle, psmOrChn, isL2cap);
}
static int btp_func_pbapSetFolderPath(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 path;
    uint08 type;

    if (pData == nullptr || dataLen < 4) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapConnectWithAuth: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    path = pData[2];
    type = pData[3];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_pbapSetFolderPath: handle[0x%x] path[0x%x] type[0x%x]",
                 handle,
                 path,
                 type);
    return btp_pbapclt_sendSetFolderTest(handle, path, type);
}
static int btp_func_pbapSendGetReqTest2(uint08 *pData, uint16 dataLen)
{
    uint16 handle;
    uint08 isWait;
    uint08 isEnSram;

    if (pData == nullptr || dataLen < 4) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapSendGetReqTest2: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    isWait   = pData[2];
    isEnSram = pData[3];
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_pbapSendGetReqTest2: handle[0x%x] isWait[%d] isEnSram[%d]",
                 handle,
                 isWait,
                 isEnSram);
    return btp_pbapclt_sendGetReqTest2(handle, isWait, isEnSram);
}
static int btp_func_pbapSendGetContinueTest2(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 4) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_pbapSendGetContinueTest2: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_pbapclt_sendGetContinueTest2(handle, pData[2], pData[3]);
}


static int btp_func_avdtpSrcSendMediaData(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    // subbands:8	block_length:16    sample:44.1    Joint_stereo    bit_pool:35
    uint08 SBCBuffer[] = { 0x05, 0x9c, 0xbd, 0x23, 0xd0, 0x7a, 0x97, 0x54, 0x22, 0x22, 0xb8, 0x53, 0x22, 0x12, 0x6a,
                           0x81, 0x5a, 0xaa, 0x12, 0x29, 0x51, 0x82, 0xcd, 0x93, 0x0e, 0x70, 0xa2, 0x72, 0x7a, 0xb6,
                           0x99, 0x63, 0xd1, 0x1d, 0xb4, 0x15, 0x60, 0xc1, 0xe0, 0x16, 0xc9, 0x07, 0x9b, 0xd6, 0x06,
                           0x98, 0x46, 0x18, 0x3e, 0x72, 0x38, 0xeb, 0x1d, 0x51, 0xc3, 0x2c, 0x8d, 0x81, 0xd4, 0xf2,
                           0x89, 0x77, 0x36, 0xab, 0x39, 0x25, 0x2d, 0x7a, 0x92, 0xa4, 0x99, 0xd7, 0xaa, 0x4f, 0x58,
                           0x56, 0x08, 0xb3, 0x4f, 0x2b, 0x2c, 0xea, 0x92, 0x14, 0x74, 0xb4, 0x5a, 0x31, 0x99, 0x66,
                           0xbe, 0xea, 0x66, 0x96, 0x1a, 0x65, 0xb5, 0xf8, 0xdd, 0x40, 0x3b, 0xbf, 0x2a, 0xa7, 0xe5,
                           0xed, 0x11, 0x30, 0xd9, 0x6e, 0x58, 0xcf, 0x50, 0xbd, 0x87, 0x54, 0x92, 0xe1, 0x68, 0x52,
                           0x9c, 0xbd, 0x23, 0xdc, 0xfe, 0xa8, 0x53, 0x21, 0x11, 0xa7, 0x53, 0x21, 0x02, 0x39, 0x60,
                           0xe9, 0x9e, 0x6d, 0x5c, 0x82, 0x25, 0xc9, 0x55, 0x8d, 0xae, 0xdc, 0x52, 0xce, 0xba, 0xd4,
                           0x85, 0xb2, 0x88, 0x68, 0x5d, 0x86, 0xb5, 0x8f, 0x87, 0x10, 0xab, 0x0e, 0xad, 0xa6, 0xac,
                           0xb6, 0x2d, 0xea, 0x87, 0x0c, 0xa1, 0x6a, 0xa5, 0x77, 0x01, 0xe5, 0x6c, 0x74, 0xe9, 0x2a,
                           0xe6, 0x1d, 0xa6, 0x6e, 0x48, 0x8d, 0x78, 0x36, 0x96, 0x08, 0xd8, 0xcc, 0x74, 0x01, 0xc4,
                           0x75, 0x99, 0xd5, 0xcd, 0x19, 0xd4, 0x3d, 0x2b, 0xfc, 0xd0, 0xab, 0x3e, 0x84, 0x95, 0x1a,
                           0x2e, 0xba, 0x99, 0xfa, 0xd2, 0x1d, 0x22, 0x84, 0x27, 0x4b, 0x1f, 0xa8, 0xa7, 0x5b, 0x1a,
                           0x36, 0x36, 0xfb, 0xb6, 0x93, 0xe0, 0xf2, 0x13, 0xfa, 0xd9, 0xc9, 0xc1, 0x45, 0x4e };
    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avdtpSrcSendMediaData: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_sendMediaData(handle, 0, 0, SBCBuffer, sizeof(SBCBuffer));
}
static int btp_func_avdtpSrcSendMediaData01(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    // subbands:8	block_length:16    sample:44.1    Joint_stereo    bit_pool:36
    uint08 SBCBuffer[] = { 0x05, 0x9c, 0xbd, 0x24, 0xd0, 0x7a, 0x97, 0x54, 0x22, 0x22, 0xb8, 0x53, 0x22, 0x12, 0x6a,
                           0x81, 0x5a, 0xaa, 0x12, 0x29, 0x51, 0x82, 0xcd, 0x93, 0x0e, 0x70, 0xa2, 0x72, 0x7a, 0xb6,
                           0x99, 0x63, 0xd1, 0x1d, 0xb4, 0x15, 0x60, 0xc1, 0xe0, 0x16, 0xc9, 0x07, 0x9b, 0xd6, 0x06,
                           0x98, 0x46, 0x18, 0x3e, 0x72, 0x38, 0xeb, 0x1d, 0x51, 0xc3, 0x2c, 0x8d, 0x81, 0xd4, 0xf2,
                           0x89, 0x77, 0x36, 0xab, 0x39, 0x25, 0x2d, 0x7a, 0x92, 0xa4, 0x99, 0xd7, 0xaa, 0x4f, 0x58,
                           0x56, 0x08, 0xb3, 0x4f, 0x2b, 0x2c, 0xea, 0x92, 0x14, 0x74, 0xb4, 0x5a, 0x31, 0x99, 0x66,
                           0xbe, 0xea, 0x66, 0x96, 0x1a, 0x65, 0xb5, 0xf8, 0xdd, 0x40, 0x3b, 0xbf, 0x2a, 0xa7, 0xe5,
                           0xed, 0x11, 0x30, 0xd9, 0x6e, 0x58, 0xcf, 0x50, 0xbd, 0x87, 0x54, 0x92, 0xe1, 0x68, 0x52,
                           0x9c, 0xbd, 0x24, 0xdc, 0xfe, 0xa8, 0x53, 0x21, 0x11, 0xa7, 0x53, 0x21, 0x02, 0x39, 0x60,
                           0xe9, 0x9e, 0x6d, 0x5c, 0x82, 0x25, 0xc9, 0x55, 0x8d, 0xae, 0xdc, 0x52, 0xce, 0xba, 0xd4,
                           0x85, 0xb2, 0x88, 0x68, 0x5d, 0x86, 0xb5, 0x8f, 0x87, 0x10, 0xab, 0x0e, 0xad, 0xa6, 0xac,
                           0xb6, 0x2d, 0xea, 0x87, 0x0c, 0xa1, 0x6a, 0xa5, 0x77, 0x01, 0xe5, 0x6c, 0x74, 0xe9, 0x2a,
                           0xe6, 0x1d, 0xa6, 0x6e, 0x48, 0x8d, 0x78, 0x36, 0x96, 0x08, 0xd8, 0xcc, 0x74, 0x01, 0xc4,
                           0x75, 0x99, 0xd5, 0xcd, 0x19, 0xd4, 0x3d, 0x2b, 0xfc, 0xd0, 0xab, 0x3e, 0x84, 0x95, 0x1a,
                           0x2e, 0xba, 0x99, 0xfa, 0xd2, 0x1d, 0x22, 0x84, 0x27, 0x4b, 0x1f, 0xa8, 0xa7, 0x5b, 0x1a,
                           0x36, 0x36, 0xfb, 0xb6, 0x93, 0xe0, 0xf2, 0x13, 0xfa, 0xd9, 0xc9, 0xc1, 0x45, 0x4e };
    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avdtpSrcSendMediaData01: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_sendMediaData(handle, 0, 0, SBCBuffer, sizeof(SBCBuffer));
}
static int btp_func_avdtpSrcSendCloseReq(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avdtpSrcSendCloseReq: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_close(handle);
}
static int btp_func_avdtpSrcSendAbortReq(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avdtpSrcSendAbortReq: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_abort(handle);
}
static int btp_func_avdtpSrcSendGetAllCap(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avdtpSrcSendGetAllCap: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_getAllCapability(handle);
}
static int btp_func_avdtpSrcSendSetCfg(uint08 *pData, uint16 dataLen)
{
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_avdtpSrcSendSetCfg: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    return btp_a2dpsrc_setConfiguration(handle);
}
static int btp_func_ptsl2cConnect(uint08 *pData, uint16 dataLen)
{
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cConnect: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cConnect: handle[0x%x]");
    return btp_ptsl2c_connect(handle);
#else
    return -TLK_ENOSUPPORT;
#endif
}
static int btp_func_ptsl2cDisconn(uint08 *pData, uint16 dataLen)
{
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    uint16 handle;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cDisconn: failure - param error");
        return -TLK_EPARAM;
    }

    handle = ((uint16)pData[1] << 8 | pData[0]);
    if (handle == 0) {
        handle = sBtpFuncAclHandle;
    }
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cDisconn: handle[0x%x]", handle);
    return btp_ptsl2c_disconn(handle);
#else
    return -TLK_ENOSUPPORT;
#endif
}
static int btp_func_ptsl2cEnableRtn(uint08 *pData, uint16 dataLen)
{
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    uint08 isEnable;
    uint08 rtnMode;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cEnableRtn: failure - param error");
        return -TLK_EPARAM;
    }

    isEnable = pData[0];
    rtnMode  = pData[1];
    btp_ptsl2c_enableRtn(isEnable, rtnMode);
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_ptsl2cEnableRtn: isEnable[%d],rtnMode[%d]",
                 isEnable,
                 rtnMode);
    return TLK_ENONE;
#else
    return -TLK_ENOSUPPORT;
#endif
}
static int btp_func_ptsl2cEnableFcs(uint08 *pData, uint16 dataLen)
{
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    uint16 isEnable;
    uint08 fcsType;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cEnableFcs: failure - param error");
        return -TLK_EPARAM;
    }

    isEnable = pData[0];
    fcsType  = pData[1];
    btp_ptsl2c_enableFcs(isEnable, fcsType);
    tlkapi_trace(BTP_FUNC_DBG_FLAG,
                 BTP_FUNC_DBG_SIGN,
                 "btp_func_ptsl2cEnableFcs: isEnable[%d],fcsType[%d]",
                 isEnable,
                 fcsType);
    return TLK_ENONE;
#else
    return -TLK_ENOSUPPORT;
#endif
}
static int btp_func_ptsl2cSendData(uint08 *pData, uint16 dataLen)
{
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    uint16 length;

    if (pData == nullptr || dataLen < 2) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cEnableFcs: failure - param error");
        return -TLK_EPARAM;
    }

    length = ((uint16)pData[1] << 8) | pData[0];
    btp_ptsl2c_sendTestData(length);
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cEnableFcs: length[%d]", length);
    return TLK_ENONE;
#else
    return -TLK_ENOSUPPORT;
#endif
}
static int btp_func_ptsl2cEnableEfs(uint08 *pData, uint16 dataLen)
{
#if (TLKBTP_CFG_PTSL2C_ENABLE)
    uint16 isEnable;

    if (pData == nullptr || dataLen < 1) {
        tlkapi_error(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cEnableEfs: failure - param error");
        return -TLK_EPARAM;
    }

    isEnable = pData[0];
    btp_ptsl2c_enableEfs(isEnable);
    tlkapi_trace(BTP_FUNC_DBG_FLAG, BTP_FUNC_DBG_SIGN, "btp_func_ptsl2cEnableEfs: isEnable[%d]", isEnable);
    return TLK_ENONE;
#else
    return -TLK_ENOSUPPORT;
#endif
}


#endif // #if (TLK_STK_BTP_ENABLE && TLK_CFG_TEST_ENABLE)
