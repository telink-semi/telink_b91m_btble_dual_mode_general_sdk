/********************************************************************************************************
 * @file	btp_sdpConst.c
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
#include "../btp_stdio.h"
#if (TLKBTP_CFG_SDP_ENABLE)
#include "btp_sdpConst.h"
// #include "btp_sdpInner.h"
#include "btp_sdp.h"
#include "../btp_config.h"
#if (TLKBTP_CFG_HID_ENABLE)
#include "../hid/btp_hid.h"
#include "../pts/btp_ptsHid.h"
#include "../../bth/bth_hcicod.h"
#endif


#define BTP_SDP_VID_TELINK        0X0211
#define BTP_SDP_VID_APPLE_AIRPODS 0x004C


const char gcBthSdpServiceName[]  = "sdp_sv_name";
const char gcBthSdpServiceDesc[]  = "sdp_sv_desc";
const char gcBthSdpProviderName[] = "Telink";


const char gcBthSdpServiceUrl[]      = "www.telink-semi.cn";
const char gcBthPnpInfoServiceDesc[] = "PnP Information";

const char gcBthSppInfoServiceDesc[] = "Telink SPP";
const char gcBthSdpSppServiceName[]  = "Serial Port Service";

// const char gcBthSdpProviderName[] = "telink-semi.com";
const char gcBthSdpServiceAvalibility   = 0xff; // 0x55
const char gcBthSdpPnpInfoServiceDesc[] = "PnP Information";

const char gcBthSdpHfpHfServiceName[] = "HFP HF";
const char gcBthSdpHfpAgServiceName[] = "HFP AG";
const int gcBthSdpHfpHfFeature        = 0x3F;
const int gcBthSdpHfpAgFeature        = 0x3F;

#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
const int gcBthSdpAvrcpCtFeature = 0xC1; // Supports browsing.
const int gcBthSdpAvrcpTgFeature = 0x02;
#else
const int gcBthSdpAvrcpCtFeature = 0x01;
const int gcBthSdpAvrcpTgFeature = 0x02;
#endif
const char gcBthSdpAvrcpCtProviderName[] = "Telink";
const char gcBthSdpAvrcpTgProviderName[] = "Telink";
const char gcBthSdpAvrcpCtServiceName[]  = "AVRCP CT";
const char gcBthSdpAvrcpTgServiceName[]  = "AVRCP TG";


const int gcBthSdpA2dpSrcFeature = 1;
const int gcBthSdpA2dpSnkFeature = 1;

const char gcBthSdpA2dpSrcProviderName[] = "Telink";
const char gcBthSdpA2dpSnkProviderName[] = "Telink";
const char gcBthSdpA2dpSrcServiceName[]  = "Audio/Video Service";
const char gcBthSdpA2dpSnkServiceName[]  = "Audio/Video Service";

#if (TLKBTP_CFG_HID_ENABLE)
static const char gcBtpSdpHidServiceName[]  = "HID device";
static const char gcBtpSdpHidProviderName[] = "Telink";
// SubClass: Refer <HID_v1.1.1.pdf> P66
#define scBtpSdpHidSubClass (BTH_COD_PERIPHERAL_KEYBOARD | BTH_COD_PERIPHERAL_KEYBOARD)
#endif

uint8_t gBthSrvSppUserClassId[16] =
    //{0x65, 0xD6, 0x92, 0x8C, 0x9F, 0x17, 0x4F, 0xCC, 0xAA, 0x55, 0x0B, 0xF0, 0xF4, 0x5B, 0xCE, 0xE1};
    //{ 0x0C, 0xF1, 0x2D, 0x31, 0xFA, 0xC3, 0x45, 0x53, 0xBD, 0x80, 0xD6, 0x83, 0x2E, 0x7B, 0x38, 0x50 };
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
const char headset_spp[] = "SPP";
const btp_sdp_serviceItem_t gcBthSdpSppItem[] = {
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,          BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                          BTP_SDP_ATTR_RECORD_HANDLE,
     (uint08 *)0                                                                                                                }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,          BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                          BTP_SDP_SPP_HANDLE,
     (uint08 *)0                                                                                                                }, // handle

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,      BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                          BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (uint08 *)0                                                                                                                }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)0                                                                                                                }, //  list begin:

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,      BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                          BTP_SDP_SRVCLASS_ID_SERIAL_PORT,
     (uint08 *)0                                                                                                                }, //  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)0                                                                                                                }, // list end

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                          BTP_SDP_ATTR_PROTO_DESC_LIST,
     (uint08 *)0                                                                                                                },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)0                                                                                                                }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)0                                                                                                                }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                          BTP_SDP_PROTOCOL_L2CAP_UUID,
     (uint08 *)0                                                                                                                }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)0                                                                                                                }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)0                                                                                                                }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                          BTP_SDP_PROTOCOL_RFCOMM_UUID,
     (uint08 *)0                                                                                                                }, //  list item[0] UUID = RFCOMM
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_8,                                                           TLKBT_CFG_SPP_RFC_CHANNEL,
     (uint08 *)0                                                                                                                }, //  list item[1]  rfcomm CHANNEL ID
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)0                                                                                                                }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)0                                                                                                                }, // list end

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                          BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (uint08 *)0                                                                                                                }, //  att id =5
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16,                                                      BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (uint08 *)0                                                                                                                }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                          0x656e,
     (uint08 *)0                                                                                                                }, //  language ID 'e' 'en'
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                          106,
     (uint08 *)0                                                                                                                }, // UTF8
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                          BTP_SDP_PRIMARY_LANG_BASE,
     (uint08 *)0                                                                                                                }, //  Attr base
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)0                                                                                                                }, // list end

    { BTP_SDP_FLAG_ATT,        0x0100,    BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0100,                              (uint08 *)0}, // service name
    { BTP_SDP_FLAG_STR,
     0x0100,                              BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)gcBthSdpSppServiceName                                                                                           }, // service name string

    { BTP_SDP_FLAG_ATT,        0x0102,    BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0102,                              (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_STR,
     0x0102,                              BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                        0,
     (uint08 *)gcBthSdpProviderName                                                                                             }, // provider name string
};

const btp_sdp_serviceItem_t gcBthSdpPnpInfoItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                                    BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_ATTR_RECORD_HANDLE,
     (uint08 *)0                                                                                                                                          }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                                    BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                                    BTP_SDP_PNP_INFO_HANDLE,
     (uint08 *)0                                                                                                                                          }, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,                                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (uint08 *)0                                                                                                                                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,                                BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                                  0,
     (uint08 *)0                                                                                                                                          }, //  list begin:
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,                                BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_SRVCLASS_ID_PNP_INFO,
     (uint08 *)0                                                                                                                                          }, //  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,                                BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                                  0,
     (uint08 *)0                                                                                                                                          }, // list end

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_STATE,                                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_ATTR_RECORD_STATE,
     (uint08 *)0                                                                                                                                          }, //  att id = 2
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_STATE,                                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                                    0x00000000,
     (uint08 *)0                                                                                                                                          }, // handle

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,                                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_ATTR_BROWSE_GROUP_LIST,
     (uint08 *)0                                                                                                                                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,                                BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                                  0,
     (uint08 *)0                                                                                                                                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,                                BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_ATTR_PUBLIC_BROWSE_ROOT,
     (uint08 *)0                                                                                                                                          },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,                                BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                                  0,
     (uint08 *)0                                                                                                                                          }, // list end

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (uint08 *)0                                                                                                                                          }, //  att id =5
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16,                                                                                BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (uint08 *)0                                                                                                                                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    0x656e,
     (uint08 *)0                                                                                                                                          }, //  language ID 'e' 'en'
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    106,
     (uint08 *)0                                                                                                                                          }, // UTF8
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_PRIMARY_LANG_BASE,
     (uint08 *)0                                                                                                                                          }, //  Attr base
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    0x6672,
     (uint08 *)0                                                                                                                                          }, //  language ID 'e' 'en'
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    106,
     (uint08 *)0                                                                                                                                          }, // UTF8
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_PRIMARY_LANG_BASE + 0x0010,
     (uint08 *)0                                                                                                                                          }, //  Attr base
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    0x6465,
     (uint08 *)0                                                                                                                                          }, //  language ID 'e' 'en'
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    106,
     (uint08 *)0                                                                                                                                          }, // UTF8
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_PRIMARY_LANG_BASE + 0x0020,
     (uint08 *)0                                                                                                                                          }, //  Attr base
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    0x6A61,
     (uint08 *)0                                                                                                                                          }, //  language ID 'e' 'en'
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    106,
     (uint08 *)0                                                                                                                                          }, // UTF8
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_PRIMARY_LANG_BASE + 0x0030,
     (uint08 *)0                                                                                                                                          }, //  Attr base
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                           BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                                  0,
     (uint08 *)0                                                                                                                                          }, // list end

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SERVICE_AVAILABILITY,                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_ATTR_SERVICE_AVAILABILITY,
     (uint08 *)0                                                                                                                                          }, //  att id = 8
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     BTP_SDP_ATTR_SERVICE_AVAILABILITY,                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_8,                                                                                     0,
     (uint08 *)&gcBthSdpServiceAvalibility                                                                                                                }, // time alive(seconds)

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SERVICE_DESC_LIST,                                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_ATTR_SERVICE_DESC_LIST,
     (uint08 *)0                                                                                                                                          },
    { BTP_SDP_FLAG_STR,
     BTP_SDP_ATTR_SERVICE_DESC_LIST,                                BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                                  0,
     (uint08 *)gcBthSdpPnpInfoServiceDesc                                                                                                                 },

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_GROUP_ID,                                         BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                                    BTP_SDP_ATTR_SPECIFICATION_ID,
     (uint08 *)0                                                                                                                                          },
    { BTP_SDP_FLAG_ATT,                  BTP_SDP_ATTR_GROUP_ID,     BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0102,                              (uint08 *)0},

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_GROUP_ID + 1,
     BTP_SDP_DTYPE_UINT,                                                                BTP_SDP_DSIZE_16,
     BTP_SDP_ATTR_VENDOR_ID,                                                                                                                   (uint08 *)0},
#if BTP_SDP_AIRPODS_ENABLE
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_GROUP_ID + 1,
     BTP_SDP_DTYPE_UINT,                                                                BTP_SDP_DSIZE_16,
     BTP_SDP_VID_APPLE_AIRPODS,                                                                                                                (uint08 *)0},
#else
    { BTP_SDP_FLAG_ATT,
      BTP_SDP_ATTR_GROUP_ID + 1,
      BTP_SDP_DTYPE_UINT,
      BTP_SDP_DSIZE_16,
      BTP_SDP_VID_TELINK,
      (uint08 *)0 },
#endif

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_GROUP_ID + 2,
     BTP_SDP_DTYPE_UINT,                                                                BTP_SDP_DSIZE_16,
     BTP_SDP_ATTR_PRODUCT_ID,                                                                                                                  (uint08 *)0},
#if BTP_SDP_AIRPODS_ENABLE
    { BTP_SDP_FLAG_ATT,                  BTP_SDP_ATTR_GROUP_ID + 2, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x2002,                              (uint08 *)0},
#else
    { BTP_SDP_FLAG_ATT, BTP_SDP_ATTR_GROUP_ID + 2, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x1001, (uint08 *)0 },
#endif

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_GROUP_ID + 3,
     BTP_SDP_DTYPE_UINT,                                                                BTP_SDP_DSIZE_16,
     BTP_SDP_ATTR_VERSION,                                                                                                                     (uint08 *)0},
    { BTP_SDP_FLAG_ATT,                  BTP_SDP_ATTR_GROUP_ID + 3, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0668,                              (uint08 *)0},

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_GROUP_ID + 4,
     BTP_SDP_DTYPE_UINT,                                                                BTP_SDP_DSIZE_16,
     BTP_SDP_ATTR_PRIMARY_RECORD,                                                                                                              (uint08 *)0},
    { BTP_SDP_FLAG_ATT,                  BTP_SDP_ATTR_GROUP_ID + 4, BTP_SDP_DTYPE_BOOL, BTP_SDP_DSIZE_8,  0x01,                                (uint08 *)0},

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_GROUP_ID + 5,
     BTP_SDP_DTYPE_UINT,                                                                BTP_SDP_DSIZE_16,
     BTP_SDP_ATTR_VENDOR_ID_SOURCE,                                                                                                            (uint08 *)0},
    { BTP_SDP_FLAG_ATT,                  BTP_SDP_ATTR_GROUP_ID + 5, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0001,                              (uint08 *)0},

    { BTP_SDP_FLAG_ATT,                  0xa000,                    BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0xa000,                              (uint08 *)0},
    { BTP_SDP_FLAG_ATT,                  0xa000,                    BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_32, 0x080040ff,                          (uint08 *)0},

    { BTP_SDP_FLAG_ATT,                  0xafff,                    BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0xafff,                              (uint08 *)0},
    { BTP_SDP_FLAG_ATT,                  0xafff,                    BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0001,                              (uint08 *)0},
};

const btp_sdp_serviceItem_t gcBthSdpHfpHfItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_RECORD_HANDLE,
     (uint08 *)0                                                                                                                     }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                 BTP_SDP_HFP_HF_HANDLE,
     (uint08 *)0                                                                                                                     }, // handle

  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (uint08 *)0                                                                                                                     }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin:
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_HANDSFREE,
     (uint08 *)0                                                                                                                     }, //  list item[0]
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_GENERIC_AUDIO,
     (uint08 *)0                                                                                                                     }, //  list item[1]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, // list end

  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROTO_DESC_LIST,
     (uint08 *)0                                                                                                                     },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_L2CAP_UUID,
     (uint08 *)0                                                                                                                     }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_RFCOMM_UUID,
     (uint08 *)0                                                                                                                     }, //  list item[0] UUID = RFCOMM
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_8,                                                                  TLKBT_CFG_HFPHF_RFC_CHANNEL,
     (uint08 *)0                                                                                                                     }, //  list item[1]  rfcomm CHANNEL ID
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, // list end

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SERVICE_AVAILABILITY,          BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_SERVICE_AVAILABILITY,
     (uint08 *)0                                                                                                                     }, //  att id = 8
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     BTP_SDP_ATTR_SERVICE_AVAILABILITY,          BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_8,                                                                  0,
     (uint08 *)&gcBthSdpServiceAvalibility                                                                                           }, // time alive(seconds)

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                     },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16,                                                             BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                     }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_HANDSFREE,
     (uint08 *)0                                                                                                                     }, //  list item[0] UUID = HANDSFREE
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0108,
     (uint08 *)0                                                                                                                     }, //  list item[1] VERSION 1.5
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, // list end


    { BTP_SDP_FLAG_ATT,                  0x0100, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0100,                            (uint08 *)0}, // service name
    { BTP_SDP_FLAG_STR,
     0x0100,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpHfpHfServiceName                                                                                              }, // service name string

    { BTP_SDP_FLAG_ATT,                  0x0102, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0102,                            (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_STR,
     0x0102,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpProviderName                                                                                                  }, // provider name string

    { BTP_SDP_FLAG_ATT,                  0x0311, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0311,                            (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     0x0311,                                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0,
     (uint08 *)&gcBthSdpHfpHfFeature                                                                                                 }, // provider name string
};

const btp_sdp_serviceItem_t gcBthSdpHfpAgItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_RECORD_HANDLE,
     (uint08 *)0                                                                                                                     }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                 BTP_SDP_HFP_AG_HANDLE,
     (uint08 *)0                                                                                                                     }, // handle

  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (uint08 *)0                                                                                                                     }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin:
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_HANDSFREE_AGW,
     (uint08 *)0                                                                                                                     }, //  list item[0]
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_GENERIC_AUDIO,
     (uint08 *)0                                                                                                                     }, //  list item[1]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, // list end

  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROTO_DESC_LIST,
     (uint08 *)0                                                                                                                     },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_L2CAP_UUID,
     (uint08 *)0                                                                                                                     }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_RFCOMM_UUID,
     (uint08 *)0                                                                                                                     }, //  list item[0] UUID = RFCOMM
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_8,                                                                  TLKBT_CFG_HFPAG_RFC_CHANNEL,
     (uint08 *)0                                                                                                                     }, //  list item[1]  rfcomm CHANNEL ID
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, // list end

  //
  //	{BTP_SDP_FLAG_ATT, BTP_SDP_ATTR_SERVICE_AVAILABILITY,  BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16,
  //BTP_SDP_ATTR_SERVICE_AVAILABILITY,  (uint08*)0}, //att id = 8
  //		{BTP_SDP_FLAG_ATT|BTP_SDP_FLAG_W, BTP_SDP_ATTR_SERVICE_AVAILABILITY, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_8, 0,
  //(uint08*) &gcBthSdpServiceAvalibility}, // time alive(seconds)

  // Bluetooth Profile Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                     },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16,                                                             BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                     }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_HANDSFREE,
     (uint08 *)0                                                                                                                     }, //  list item[0] UUID = HANDSFREE
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0108,
     (uint08 *)0                                                                                                                     }, //  list item[1] VERSION 1.5
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                     }, // list end

  // Service Name
    { BTP_SDP_FLAG_ATT,                  0x0100, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0100,                            (uint08 *)0}, // service name
    { BTP_SDP_FLAG_STR,
     0x0100,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpHfpAgServiceName                                                                                              }, // service name string

  // Provider Name
  //	{BTP_SDP_FLAG_ATT, 0x0102,  BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16,  0x0102,  (uint08*)0},  // provider name
  //		{BTP_SDP_FLAG_STR, 0x0102, BTP_SDP_DTYPE_NULL, BTP_SDP_DSIZE_NULL,  0,  (uint08*)gcBthSdpProviderName},  //
  //provider name string

  //  SupportedFeatures
    { BTP_SDP_FLAG_ATT,                  0x0311, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0311,                            (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     0x0311,                                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0,
     (uint08 *)&gcBthSdpHfpAgFeature                                                                                                 }, // provider name string
};

const btp_sdp_serviceItem_t gcBthSdpA2dpSrcItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_RECORD_HANDLE,
     (uint08 *)0                                                                                                                      }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                 BTP_SDP_A2DP_SRC_HANDLE,
     (uint08 *)0                                                                                                                      }, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (uint08 *)0                                                                                                                      }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_AUDIO_SOURCE,
     (uint08 *)0                                                                                                                      }, //  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, // list end
  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROTO_DESC_LIST,
     (uint08 *)0                                                                                                                      },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_L2CAP_UUID,
     (uint08 *)0                                                                                                                      }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_PSMID_AVDTP,
     (uint08 *)0                                                                                                                      }, //  list item[1] VALUE = AVDTPddddddddddd
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_AVDTP_UUID,
     (uint08 *)0                                                                                                                      }, //  list item[0] UUID = AVDTP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0103,
     (uint08 *)0                                                                                                                      }, //  list item[1] VERSION = 0x0103
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, // list end
  // Bluetooth Profile Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                      },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16,                                                             BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                      }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_ADVANCED_AUDIO,
     (uint08 *)0                                                                                                                      }, //  list item[0] UUID = A2DP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0103,
     (uint08 *)0                                                                                                                      }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, // list end
  // SupportedFeatures
    { BTP_SDP_FLAG_ATT,                  0x0311, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0311,                             (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     0x0311,                                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0,
     (uint08 *)&gcBthSdpA2dpSrcFeature                                                                                                }, // provider name string
  // Browse Group List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_BROWSE_GROUP_LIST,
     (unsigned char *)0                                                                                                               }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PUBLIC_BROWSE_ROOT,
     (unsigned char *)0                                                                                                               },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list end
  // Provider Name
    { BTP_SDP_FLAG_ATT,                  0x0102, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0102,                             (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_STR,
     0x0102,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpA2dpSrcProviderName                                                                                            }, // provider name string
  // Service Name
    { BTP_SDP_FLAG_ATT,                  0x0100, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0100,                             (uint08 *)0}, // service name
    { BTP_SDP_FLAG_STR,
     0x0100,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpA2dpSrcServiceName                                                                                             }, // service name string
};
const btp_sdp_serviceItem_t gcBthSdpA2dpSnkItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_RECORD_HANDLE,
     (uint08 *)0                                                                                                                      }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                 BTP_SDP_A2DP_SNK_HANDLE,
     (uint08 *)0                                                                                                                      }, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (uint08 *)0                                                                                                                      }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_AUDIO_SINK,
     (uint08 *)0                                                                                                                      }, //  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, // list end
  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROTO_DESC_LIST,
     (uint08 *)0                                                                                                                      },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_L2CAP_UUID,
     (uint08 *)0                                                                                                                      }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_PSMID_AVDTP,
     (uint08 *)0                                                                                                                      }, //  list item[1] VALUE = AVDTPddddddddddd
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_AVDTP_UUID,
     (uint08 *)0                                                                                                                      }, //  list item[0] UUID = AVDTP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0103,
     (uint08 *)0                                                                                                                      }, //  list item[1] VERSION = 0x0103
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, // list end
  // Bluetooth Profile Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                      },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16,                                                             BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                      }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_ADVANCED_AUDIO,
     (uint08 *)0                                                                                                                      }, //  list item[0] UUID = A2DP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0103,
     (uint08 *)0                                                                                                                      }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, // list end
  // SupportedFeatures
    { BTP_SDP_FLAG_ATT,                  0x0311, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0311,                             (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     0x0311,                                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0,
     (uint08 *)&gcBthSdpA2dpSnkFeature                                                                                                }, // provider name string
  // Browse Group List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_BROWSE_GROUP_LIST,
     (unsigned char *)0                                                                                                               }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PUBLIC_BROWSE_ROOT,
     (unsigned char *)0                                                                                                               },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                      }, //  list end
  // Provider Name
    { BTP_SDP_FLAG_ATT,                  0x0102, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0102,                             (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_STR,
     0x0102,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpA2dpSnkProviderName                                                                                            }, // provider name string
  // Service Name
    { BTP_SDP_FLAG_ATT,                  0x0100, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0100,                             (uint08 *)0}, // service name
    { BTP_SDP_FLAG_STR,
     0x0100,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpA2dpSnkServiceName                                                                                             }, // service name string
};

const btp_sdp_serviceItem_t gcBthSdpAvrcpTgItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_RECORD_HANDLE,
     (uint08 *)0                                                                                                                        }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                 BTP_SDP_AVRCP_TG_HANDLE,
     (uint08 *)0                                                                                                                        }, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (uint08 *)0                                                                                                                        }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_AV_REMOTE_TARGET,
     (uint08 *)0                                                                                                                        }, //  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, // list end
  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROTO_DESC_LIST,
     (uint08 *)0                                                                                                                        },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_L2CAP_UUID,
     (uint08 *)0                                                                                                                        }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_PSMID_AVCTP,
     (uint08 *)0                                                                                                                        }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_AVCTP_UUID,
     (uint08 *)0                                                                                                                        }, //  list item[0] UUID = AVDTP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0104,
     (uint08 *)0                                                                                                                        }, //  list item[1] VERSION = 0x0103
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, // list end
  // Bluetooth Profile Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                        }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_AV_REMOTE,
     (uint08 *)0                                                                                                                        }, //  list item[0] UUID = A2DP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0104,
     (uint08 *)0                                                                                                                        }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, // list end
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
  // Additional Protocol Descriptor Lists
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,
     (uint08 *)0                                                                                                                        },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_L2CAP_UUID,
     (uint08 *)0                                                                                                                        },
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_PSMID_AVCTP_BROWSING,
     (uint08 *)0                                                                                                                        },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_AVCTP_UUID,
     (uint08 *)0                                                                                                                        },
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0104,
     (uint08 *)0                                                                                                                        }, //  Version=v1.4
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, // list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, // list end
#endif
  // SupportedFeatures
    { BTP_SDP_FLAG_ATT,                  0x0311, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0311,                               (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     0x0311,                                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0,
     (uint08 *)&gcBthSdpAvrcpTgFeature                                                                                                  }, // provider name string
  // Browse Group List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_BROWSE_GROUP_LIST,
     (unsigned char *)0                                                                                                                 }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PUBLIC_BROWSE_ROOT,
     (unsigned char *)0                                                                                                                 },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                        }, //  list end
  // Provider Name
    { BTP_SDP_FLAG_ATT,                  0x0102, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0102,                               (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_STR,
     0x0102,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpAvrcpTgProviderName                                                                                              }, // provider name string
  // Service Name
    { BTP_SDP_FLAG_ATT,                  0x0100, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0100,                               (uint08 *)0}, // service name
    { BTP_SDP_FLAG_STR,
     0x0100,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpAvrcpTgServiceName                                                                                               }, // service name string
};
const btp_sdp_serviceItem_t gcBthSdpAvrcpCtItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_RECORD_HANDLE,
     (uint08 *)0                                                                                                                    }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                 BTP_SDP_AVRCP_CT_HANDLE,
     (uint08 *)0                                                                                                                    }, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (uint08 *)0                                                                                                                    }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_AV_REMOTE,
     (uint08 *)0                                                                                                                    }, //  list item[0]
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_VIDEO_CONF,
     (uint08 *)0                                                                                                                    }, //  list item[1]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, // list end
  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROTO_DESC_LIST,
     (uint08 *)0                                                                                                                    },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin protocol list
  //  Protocol #0 PSM
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_L2CAP_UUID,
     (uint08 *)0                                                                                                                    }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_PSMID_AVCTP,
     (uint08 *)0                                                                                                                    }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list end
  //  Protocol #1 Version
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_AVCTP_UUID,
     (uint08 *)0                                                                                                                    }, //  list item[0] UUID = AVDTP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0104,
     (uint08 *)0                                                                                                                    }, //  list item[1] VERSION = 0x0104
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, // list end
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
  // Additional Protocol Descriptor Lists
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,
     (uint08 *)0                                                                                                                    },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_L2CAP_UUID,
     (uint08 *)0                                                                                                                    }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_PSMID_AVCTP_BROWSING,
     (uint08 *)0                                                                                                                    }, //  list item[1]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_AVCTP_UUID,
     (uint08 *)0                                                                                                                    },
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0104,
     (uint08 *)0                                                                                                                    }, //  Version=v1.4
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, // list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, // list end
#endif
  // Bluetooth Profile Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (uint08 *)0                                                                                                                    }, //  list begin protocol list
  //  Profile #0
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin protocol list
  //  Parameter #0 for
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_SRVCLASS_ID_AV_REMOTE,
     (uint08 *)0                                                                                                                    }, //  list item[0] UUID = A2DP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x0106,
     (uint08 *)0                                                                                                                    }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, // list end
  // SupportedFeatures
    { BTP_SDP_FLAG_ATT,                  0x0311, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0311,                           (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     0x0311,                                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0,
     (uint08 *)&gcBthSdpAvrcpCtFeature                                                                                              }, // provider name string
  // Browse Group List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_BROWSE_GROUP_LIST,
     (unsigned char *)0                                                                                                             }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PUBLIC_BROWSE_ROOT,
     (unsigned char *)0                                                                                                             },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)0                                                                                                                    }, //  list end
  // Provider Name
    { BTP_SDP_FLAG_ATT,                  0x0102, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0102,                           (uint08 *)0}, // provider name
    { BTP_SDP_FLAG_STR,
     0x0102,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpAvrcpCtProviderName                                                                                          }, // provider name string
  // Service Name
    { BTP_SDP_FLAG_ATT,                  0x0100, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0100,                           (uint08 *)0}, // service name
    { BTP_SDP_FLAG_STR,
     0x0100,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (uint08 *)gcBthSdpAvrcpCtServiceName                                                                                           }, // service name string
};

const uint08 gcBtpSdpHidReportMap[] = {
#if !(TLKBTP_CFG_PTSHID_ENABLE)
    // keyboard report in
    0x05,
    0x01, // Usage Pg (Generic Desktop)
    0x09,
    0x06, // Usage (Keyboard)
    0xA1,
    0x01, // Collection: (Application)
    0x85,
    0x01, // Report Id (keyboard)
    0x05,
    0x07, // Usage Pg (Key Codes)
    0x19,
    0xE0, // Usage Min (224)  VK_CTRL:0xe0
    0x29,
    0xE7, // Usage Max (231)  VK_RWIN:0xe7
    0x15,
    0x00, // Log Min (0)
    0x25,
    0x01, // Log Max (1)
          // Modifier byte
    0x75,
    0x01, // Report Size (1)   1 bit * 8
    0x95,
    0x08, // Report Count (8)
    0x81,
    0x02, // Input: (Data, Variable, Absolute)
          // Reserved byte
    0x95,
    0x01, // Report Count (1)
    0x75,
    0x08, // Report Size (8)
    0x81,
    0x01, // Input: (static constant)
          // keyboard output
          // 5 bit led ctrl: NumLock CapsLock ScrollLock Compose kana
    0x95,
    0x05, // Report Count (5)
    0x75,
    0x01, // Report Size (1)
    0x05,
    0x08, // Usage Pg (LEDs )
    0x19,
    0x01, // Usage Min
    0x29,
    0x05, // Usage Max
    0x91,
    0x02, // Output (Data, Variable, Absolute)
          // 3 bit reserved
    0x95,
    0x01, // Report Count (1)
    0x75,
    0x03, // Report Size (3)
    0x91,
    0x01, // Output (static constant)
          // Key arrays (6 bytes)
    0x95,
    0x06, // Report Count (6)
    0x75,
    0x08, // Report Size (8)
    0x15,
    0x00, // Log Min (0)
    0x25,
    0xF1, // Log Max (241)
    0x05,
    0x07, // Usage Pg (Key Codes)
    0x19,
    0x00, // Usage Min (0)
    0x29,
    0xf1, // Usage Max (241)
    0x81,
    0x00, // Input: (Data, Array)
    0xC0, // End Collection

    // consumer report in
    0x05,
    0x0C, // Usage Page (Consumer)
    0x09,
    0x01, // Usage (Consumer Control)
    0xA1,
    0x01, // Collection (Application)
    0x85,
    0x02, // Report Id
    0x75,
    0x10, // global, report size 16 bits
    0x95,
    0x01, // global, report count 1
    0x15,
    0x01, // global, min  0x01
    0x26,
    0x8c,
    0x02, // global, max  0x28c
    0x19,
    0x01, // local, min   0x01
    0x2a,
    0x8c,
    0x02, // local, max    0x28c
    0x81,
    0x00, // main,  input data variable, absolute
    0xc0, // main, end collection
#else
    BTP_HID_RPT_USAGE_PAGE(8, 0x0C),                        // Usage Page(Consumer)
    BTP_HID_RPT_USAGE(8, 0xA5),                             // Usage(Consumer Control)
    BTP_HID_RPT_COLLECTION(8, 0x01),                        // COLLECTION (Application)
    BTP_HID_RPT_REPORT_ID(8, BTP_PTSHID_INPUT_REPORT_ID),   // Report ID
    BTP_HID_RPT_USAGE_MINIMUM(8, 0x00),                     // local, min   0
    BTP_HID_RPT_USAGE_MAXIMUM(16, 0x0224),                  // local, max    548
    BTP_HID_RPT_LOGICAL_MINIMUM(8, 0x00),                   // Logical Minimum (0)
    BTP_HID_RPT_LOGICAL_MAXIMUM(16, 0x0224),                // Logical Maximum (548)
    BTP_HID_RPT_REPORT_COUNT(8, 0x02),                      // report count (2)
    BTP_HID_RPT_REPORT_SIZE(8, 0x10),                       // Report Size (16)
    BTP_HID_RPT_INPUT(8, BTP_HID_IOF_DATA | BTP_HID_IOF_ARRAY | BTP_HID_IOF_ABSOLUTE),
    BTP_HID_RPT_END_COLLECTION(0),                          // end collection
    BTP_HID_RPT_COLLECTION(8, 0x01),                        // COLLECTION (Application)
    BTP_HID_RPT_REPORT_ID(8, BTP_PTSHID_OUTPUT_REPORT_ID),  // Report ID
    BTP_HID_RPT_LOGICAL_MINIMUM(8, 0x00),                   // Logical Minimum (0)
    BTP_HID_RPT_LOGICAL_MAXIMUM(8, 0xFF),                   // Logical Maximum (255)
    BTP_HID_RPT_REPORT_SIZE(8, 0x08),                       // Report Size (8)
    BTP_HID_RPT_REPORT_COUNT(8, 0x14),                      // report count (20)
    BTP_HID_RPT_USAGE(8, 0x00),                             // Usgae(Vendor defined)
    BTP_HID_RPT_OUTPUT(8, BTP_HID_IOF_DATA | BTP_HID_IOF_ARRAY | BTP_HID_IOF_ABSOLUTE),
    BTP_HID_RPT_END_COLLECTION(0),                          // end collection
    BTP_HID_RPT_COLLECTION(8, 0x01),                        // COLLECTION (Application)
    BTP_HID_RPT_REPORT_ID(8, BTP_PTSHID_FEATURE_REPORT_ID), // Report ID
    BTP_HID_RPT_LOGICAL_MINIMUM(8, 0x00),                   // Logical Minimum (0)
    BTP_HID_RPT_LOGICAL_MAXIMUM(8, 0xFF),                   // Logical Maximum (255)
    BTP_HID_RPT_REPORT_SIZE(8, 0x08),                       // Report Size (8)
    BTP_HID_RPT_REPORT_COUNT(8, 0x14),                      // report count (20)
    BTP_HID_RPT_USAGE(8, 0x00),                             // Usgae(Vendor defined)
    BTP_HID_RPT_FEATURE(8, BTP_HID_IOF_DATA | BTP_HID_IOF_VARIABLE | BTP_HID_IOF_ABSOLUTE),
    BTP_HID_RPT_END_COLLECTION(0)                           // end collection
#endif
};

#if (TLKBTP_CFG_HID_ENABLE)
const btp_sdp_serviceItem_t gcBtpSdpHidItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                         BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_RECORD_HANDLE,
     (unsigned char *)0                                                                                                                                                         }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                         BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                              BTP_SDP_HID_DEV_HANDLE,
     (unsigned char *)0                                                                                                                                                         }, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (unsigned char *)0                                                                                                                                                         }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, //  list begin:
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,                     BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_SRVCLASS_ID_HID,
     (unsigned char *)0                                                                                                                                                         }, //  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, // list end
  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_PROTO_DESC_LIST,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_PROTOCOL_L2CAP_UUID,
     (unsigned char *)0                                                                                                                                                         }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_PSMID_HIDC,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_PROTOCOL_HIDP_UUID,
     (unsigned char *)0                                                                                                                                                         }, //  list item[0] UUID = RFCOMM
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,                       BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, // list end
  // Language Base Attribute ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (unsigned char *)0                                                                                                                                                         }, //  att id =5
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16,                                                                          BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (unsigned char *)0                                                                                                                                                         }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              0x656e,
     (unsigned char *)0                                                                                                                                                         }, //  language ID 'e' 'en'
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              106,
     (unsigned char *)0                                                                                                                                                         }, // UTF8
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_PRIMARY_LANG_BASE,
     (unsigned char *)0                                                                                                                                                         }, //  Attr base
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, // list end
  // Additional Protocol Descriptor Lists
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, //  list begin protocol list
  // Protocol DescriptorList #0
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, // Protocol DescriptorList #0
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_PROTOCOL_L2CAP_UUID,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_PSMID_HIDI,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_PROTOCOL_HIDP_UUID,
     (unsigned char *)0                                                                                                                                                         }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,                   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, // list end
  // Service Name	"HID device"
    { BTP_SDP_FLAG_ATT,        0x0100,                   BTP_SDP_DTYPE_UINT,   BTP_SDP_DSIZE_16,    0x0100,                                (unsigned char *)0                   }, // service name
    { BTP_SDP_FLAG_STR,
     0x0100,                                             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)gcBtpSdpHidServiceName                                                                                                                                    }, // service name string

  // Service Description (O)

  // Provider Name (O)
    { BTP_SDP_FLAG_ATT,        0x0102,                   BTP_SDP_DTYPE_UINT,   BTP_SDP_DSIZE_16,    0x0102,                                (unsigned char *)0                   }, // service name
    { BTP_SDP_FLAG_STR,
     0x0102,                                             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)gcBtpSdpHidProviderName                                                                                                                                   }, // service name string
  // Bluetooth Profile Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (unsigned char *)0                                                                                                                                                         }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,                     BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_SRVCLASS_ID_HID,
     (unsigned char *)0                                                                                                                                                         }, //  list item[0]
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,                     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              0x0101,
     (unsigned char *)0                                                                                                                                                         }, ///  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         },
 // Language Base Attribute ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (unsigned char *)0                                                                                                                                                         }, //  att id =5
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16,                                                                          BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (unsigned char *)0                                                                                                                                                         }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              0x656e,
     (unsigned char *)0                                                                                                                                                         }, //  language ID 'e' 'en'
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              106,
     (unsigned char *)0                                                                                                                                                         }, // UTF8
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_PRIMARY_LANG_BASE,
     (unsigned char *)0                                                                                                                                                         }, //  Attr base
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,                BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                                            0,
     (unsigned char *)0                                                                                                                                                         }, // list end
  // HID Parser Version
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SVCDB_STATE,                           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_SVCDB_STATE,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        BTP_SDP_ATTR_SVCDB_STATE, BTP_SDP_DTYPE_UINT,   BTP_SDP_DSIZE_16,    0x0111,                                (unsigned char *)0                   },
 // HID Device Subclass
    { BTP_SDP_FLAG_ATT,
     0x0202,                                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_HID_DEVICE_SUBCLASS,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        0x0202,                   BTP_SDP_DTYPE_UINT,   BTP_SDP_DSIZE_8,     scBtpSdpHidSubClass,                   (unsigned char *)0                   },
 // HIDCountryCode
    { BTP_SDP_FLAG_ATT,
     0x0203,                                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_HID_COUNTRY_CODE,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        0x0203,                   BTP_SDP_DTYPE_UINT,   BTP_SDP_DSIZE_8,     0x21,                                  (unsigned char *)0                   },
 // HIDVirtualCable
    { BTP_SDP_FLAG_ATT,
     0x0204,                                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_HID_VIRTUAL_CABLE,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        0x0204,                   BTP_SDP_DTYPE_BOOL,   BTP_SDP_DSIZE_8,     0x00,                                  (unsigned char *)0                   },
 // HIDReconnectInitiate
    { BTP_SDP_FLAG_ATT,
     0x0205,                                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_HID_RECONNECT_INITIATE,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        0x0205,                   BTP_SDP_DTYPE_BOOL,   BTP_SDP_DSIZE_8,     0x00,                                  (unsigned char *)0                   },
 // HIDNormallyConnectable
    { BTP_SDP_FLAG_ATT,
     0x0205,                                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_HID_NORMALLY_CONNECTABLE,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        0x0205,                   BTP_SDP_DTYPE_BOOL,   BTP_SDP_DSIZE_8,     TRUE,                                  (unsigned char *)0                   },
 // HIDSDPDISABLE
    { BTP_SDP_FLAG_ATT,
     0x0205,                                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_HID_SDP_DISABLE,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        0x0205,                   BTP_SDP_DTYPE_BOOL,   BTP_SDP_DSIZE_8,     FALSE,                                 (unsigned char *)0                   },
 // HIDDescriptorList
    { BTP_SDP_FLAG_ATT,
     0x0206,                                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_HID_DESCRIPTOR_LIST,
     (unsigned char *)0                                                                                                                                                         },
 // Note: sizeof(gcBtpSdpHidReportMap) <= 249
    { BTP_SDP_FLAG_ATT,
     0x0206,                                             BTP_SDP_DTYPE_DES,
     BTP_SDP_DSIZE_VAR_8,                                                                           sizeof(gcBtpSdpHidReportMap) + 6,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,
     0x0206,                                             BTP_SDP_DTYPE_DES,
     BTP_SDP_DSIZE_VAR_8,                                                                           sizeof(gcBtpSdpHidReportMap) + 4,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        0x0206,                   BTP_SDP_DTYPE_UINT,   BTP_SDP_DSIZE_8,     0x22,                                  (unsigned char *)0                   }, // HID REPORT
  // descriptor
    { BTP_SDP_FLAG_HID_DES,
     0x0206,                                             BTP_SDP_DTYPE_STRING,
     sizeof(gcBtpSdpHidReportMap),
     0x00,                                                                                                                                 (unsigned char *)gcBtpSdpHidReportMap},
 // Note: 249 < sizeof(gcBtpSdpHidReportMap) <= 251
  //       {BTP_SDP_FLAG_ATT, 0x0206 ,  BTP_SDP_DTYPE_DES, BTP_SDP_DSIZE_VAR_16,  sizeof(gcBtpSdpHidReportMap) + 6,
  //       (unsigned char *) 0 },
  //           {BTP_SDP_FLAG_ATT, 0x0206,  BTP_SDP_DTYPE_DES, BTP_SDP_DSIZE_VAR_8,  sizeof(gcBtpSdpHidReportMap) + 4,
  //           (unsigned char *) 0 },
  //    	    {BTP_SDP_FLAG_ATT, 0x0206,  BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_8,  0x22,  (unsigned char *) 0 },   // HID
  //    REPORT descriptor
  //    	    {BTP_SDP_FLAG_HID_DES, 0x0206,  BTP_SDP_DTYPE_STRING, sizeof(gcBtpSdpHidReportMap),  0x00,  (unsigned
  //    char *)gcBtpSdpHidReportMap},
  // Note: 251 < sizeof(gcBtpSdpHidReportMap) <= 255
  //    	{BTP_SDP_FLAG_ATT, 0x0206 ,  BTP_SDP_DTYPE_DES, BTP_SDP_DSIZE_VAR_16,  sizeof(gcBtpSdpHidReportMap) + 7,
  //    (unsigned char *) 0 },
  //          {BTP_SDP_FLAG_ATT, 0x0206,  BTP_SDP_DTYPE_DES, BTP_SDP_DSIZE_VAR_16,  sizeof(gcBtpSdpHidReportMap) + 4,
  //          (unsigned char *) 0 },
  //    	    {BTP_SDP_FLAG_ATT, 0x0206,  BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_8,  0x22,  (unsigned char *) 0 },   // HID
  //    REPORT descriptor
  //    	    {BTP_SDP_FLAG_HID_DES, 0x0206,  BTP_SDP_DTYPE_STRING, sizeof(gcBtpSdpHidReportMap),  0x00,  (unsigned
  //    char *)gcBtpSdpHidReportMap},
  // Note: 255 < sizeof(gcBtpSdpHidReportMap)
  //    	{BTP_SDP_FLAG_ATT, 0x0206 ,  BTP_SDP_DTYPE_DES, BTP_SDP_DSIZE_VAR_16,  sizeof(gcBtpSdpHidReportMap) + 8,
  //    (unsigned char *) 0 },
  //          {BTP_SDP_FLAG_ATT, 0x0206,  BTP_SDP_DTYPE_DES, BTP_SDP_DSIZE_VAR_16,  sizeof(gcBtpSdpHidReportMap) + 5,
  //          (unsigned char *) 0 },
  //    	    {BTP_SDP_FLAG_ATT, 0x0206,  BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_8,  0x22,  (unsigned char *) 0 },   // HID
  //    REPORT descriptor
  //    	    {BTP_SDP_FLAG_HID_DES, 0x0206,  BTP_SDP_DTYPE_STRING, sizeof(gcBtpSdpHidReportMap),  0x00,  (unsigned
  //    char *)gcBtpSdpHidReportMap},

  // HIDLANGIDBaseList
    { BTP_SDP_FLAG_ATT,
     0x0207,                                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_HID_LANG_ID_BASE_LIST,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        0x0207,                   BTP_SDP_DTYPE_DES,    BTP_SDP_DSIZE_VAR_8, 0x08,                                  (unsigned char *)0                   },
    { BTP_SDP_FLAG_ATT,        0x0207,                   BTP_SDP_DTYPE_DES,    BTP_SDP_DSIZE_VAR_8, 0x06,                                  (unsigned char *)0                   },
    { BTP_SDP_FLAG_ATT,        0x0207,                   BTP_SDP_DTYPE_UINT,   BTP_SDP_DSIZE_16,    0x0409,                                (unsigned char *)0                   },
    { BTP_SDP_FLAG_ATT,        0x0207,                   BTP_SDP_DTYPE_UINT,   BTP_SDP_DSIZE_16,    0x0100,                                (unsigned char *)0                   },
 // HIDBootDevice
    { BTP_SDP_FLAG_ATT,
     0x020e,                                             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                              BTP_SDP_ATTR_HID_BOOT_DEVICE,
     (unsigned char *)0                                                                                                                                                         },
    { BTP_SDP_FLAG_ATT,        0x020e,                   BTP_SDP_DTYPE_BOOL,   BTP_SDP_DSIZE_8,     0x00,                                  (unsigned char *)0                   },
};
#endif

const btp_sdp_serviceItem_t gcBtpServiceDiscoverItem[] = {
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,          BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_RECORD_HANDLE,
     (unsigned char *)0                          }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,          BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,     BTP_SDP_SERVICE_HANDLE,
     (unsigned char *)0                          }, // handle

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,      BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (unsigned char *)0                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,      BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_SRVCLASS_ID_SDP_SERVER,
     (unsigned char *)0                          }, //  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          },

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_STATE,           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_RECORD_STATE,
     (unsigned char *)0                          }, //  att id = 2
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_STATE,           BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,     0x12345678,
     (unsigned char *)0                          }, // handle

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SERVICE_ID,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_SERVICE_ID,
     (unsigned char *)0                          }, // att id =3                        // provider name
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SERVICE_ID,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SVC_USER_ID0,
     (unsigned char *)0                          }, // service id


    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_PROTO_DESC_LIST,
     (unsigned char *)0                          },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_PROTOCOL_L2CAP_UUID,
     (unsigned char *)0                          }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_PSMID_SDP,
     (unsigned char *)0                          }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list end						 //list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, // list end


    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,      BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_BROWSE_GROUP_LIST,
     (unsigned char *)0                          }, //  att id =5
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,      BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16, BTP_SDP_ATTR_BROWSE_GROUP_LIST,
     (unsigned char *)0                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,      BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_PUBLIC_BROWSE_ROOT,
     (unsigned char *)0                          },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, // list end

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (unsigned char *)0                          }, //  att id =5
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16, BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (unsigned char *)0                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     0x656e,
     (unsigned char *)0                          }, //  language ID 'e' 'en'
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     106,
     (unsigned char *)0                          }, // UTF8
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_PRIMARY_LANG_BASE,
     (unsigned char *)0                          }, //  Attr base
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST, BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, // list end

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SVCINFO_TTL,            BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_SVCINFO_TTL,
     (unsigned char *)0                          }, //  att id = 7
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SVCINFO_TTL,            BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,     0x55aa7788,
     (unsigned char *)0                          }, // time alive(seconds)

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SERVICE_AVAILABILITY,   BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_SERVICE_AVAILABILITY,
     (unsigned char *)0                          }, //  att id = 8
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     BTP_SDP_ATTR_SERVICE_AVAILABILITY,   BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_8,      0,
     (unsigned char *)&gcBthSdpServiceAvalibility}, // time alive(seconds)


    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,      BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (unsigned char *)0                          },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,      BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16, BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (unsigned char *)0                          }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,      BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_SRVCLASS_ID_SDP_SERVER,
     (unsigned char *)0                          }, //  list item[0] UUID = A2DP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,      BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     0x0102,
     (unsigned char *)0                          }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, // list end


    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_DOC_URL,                BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_DOC_URL,
     (unsigned char *)0                          }, //  att id = 0x0a
    { BTP_SDP_FLAG_URL,
     BTP_SDP_ATTR_DOC_URL,                BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)gcBthSdpServiceUrl         }, //

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_CLIENT_EXEC_URL,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_CLIENT_EXEC_URL,
     (unsigned char *)0                          }, //  att id = 0x0a
    { BTP_SDP_FLAG_URL,
     BTP_SDP_ATTR_CLIENT_EXEC_URL,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)gcBthSdpServiceUrl         }, //

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_ICON_URL,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_ICON_URL,
     (unsigned char *)0                          }, //  att id = 0x0c
    { BTP_SDP_FLAG_URL,
     BTP_SDP_ATTR_ICON_URL,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)gcBthSdpServiceUrl         }, //


    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,    BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,
     (unsigned char *)0                          },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin
  //{BTP_SDP_FLAG_ATT, BTP_SDP_ATTR_ADD_PROTO_DESC_LIST, BTP_SDP_DTYPE_UUID, BTP_SDP_DSIZE_16,
  //BTP_SDP_PROTOCOL_L2CAP_UUID,  (unsigned char *) 0 }, //list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,    BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_PROTOCOL_L2CAP_UUID,
     (unsigned char *)0                          }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,    BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_PROTOCOL_SDP_UUID,
     (unsigned char *)0                          }, //  list item[1] VALUE = AVDTP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          },
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_ADD_PROTO_DESC_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, // list end


    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SVCNAME_PRIMARY,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_SVCNAME_PRIMARY,
     (unsigned char *)0                          }, //  service name
    { BTP_SDP_FLAG_STR,
     BTP_SDP_ATTR_SVCNAME_PRIMARY,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)gcBthSdpServiceName        }, // time alive(seconds)

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SVCDESC_PRIMARY,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_SVCDESC_PRIMARY,
     (unsigned char *)0                          }, //  service name
    { BTP_SDP_FLAG_STR,
     BTP_SDP_ATTR_SVCDESC_PRIMARY,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)gcBthSdpServiceDesc        }, // time alive(seconds)

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROVNAME_PRIMARY,       BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_PROVNAME_PRIMARY,
     (unsigned char *)0                          }, //  provider name
    { BTP_SDP_FLAG_STR,
     BTP_SDP_ATTR_PROVNAME_PRIMARY,       BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)gcBthSdpProviderName       }, // time alive(seconds)


    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_VERSION_NUM_LIST,       BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_VERSION_NUM_LIST,
     (unsigned char *)0                          }, //  version number list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_VERSION_NUM_LIST,       BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_VERSION_NUM_LIST,       BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     0x0102,
     (unsigned char *)0                          }, //  version [0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_VERSION_NUM_LIST,       BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          },

    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SVCDB_STATE,            BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_SVCDB_STATE,
     (unsigned char *)0                          }, //  att id = 7
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SVCDB_STATE,            BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,     0xfaceface,
     (unsigned char *)0                          }, // time alive(seconds)
};


const uint08 scBtpServiceIap2UuidL[16] = { 0x00, 0x00, 0x00, 0x00, 0xde, 0xCA, 0xFA, 0xDE,
                                           0xDE, 0xCA, 0xDE, 0xAF, 0xDE, 0xCA, 0xCA, 0xFF };
//{0xFF, 0xCA, 0xCA, 0xDE, 0xAF, 0xDE, 0xCA, 0xDE, 0xDE, 0xFA, 0xCA, 0xDE, 0x00, 0x00, 0x00, 0x00};
const uint08 scBtpServiceIap2UuidR[16] = { 0x00, 0x00, 0x00, 0x00, 0xde, 0xCA, 0xFA, 0xDE,
                                           0xDE, 0xCA, 0xDE, 0xAF, 0xDE, 0xCA, 0xCA, 0xFE };
//{0xFE, 0xCA, 0xCA, 0xDE, 0xAF, 0xDE, 0xCA, 0xDE, 0xDE, 0xFA, 0xCA, 0xDE, 0x00, 0x00, 0x00, 0x00};
const uint08 scBtpServiceIap2ServiceName[]         = "Telink iAP2 Service";
const btp_sdp_serviceItem_t gcBtpIap2ServiceItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_RECORD_HANDLE,
     (unsigned char *)0                                                                                                                       }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,                 BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                                 BTP_SDP_IAP_HANDLE,
     (unsigned char *)0                                                                                                                       }, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (unsigned char *)0                                                                                                                       }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)0                                                                                                                       }, //  list begin:
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_128,                                                                0,
     (unsigned char *)&scBtpServiceIap2UuidL[0]                                                                                               }, //  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,             BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)0                                                                                                                       }, // list end
  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_PROTO_DESC_LIST,
     (unsigned char *)0                                                                                                                       },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)0                                                                                                                       }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)0                                                                                                                       }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_L2CAP_UUID,
     (unsigned char *)0                                                                                                                       }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)0                                                                                                                       }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)0                                                                                                                       }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PROTOCOL_RFCOMM_UUID,
     (unsigned char *)0                                                                                                                       }, //  list item[0] UUID = RFCOMM
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_8,                                                                  TLKBT_CFG_IAP_RFC_CHANNEL,
     (unsigned char *)0                                                                                                                       }, //  list item[1]  rfcomm CHANNEL ID
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)0                                                                                                                       }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,               BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)0                                                                                                                       }, // list end
  // Bluetooth Profile Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (unsigned char *)0                                                                                                                       }, //  att id =5
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16,                                                             BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
     (unsigned char *)0                                                                                                                       }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 0x656e,
     (unsigned char *)0                                                                                                                       }, //  language ID 'e' 'en'
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 106,
     (unsigned char *)0                                                                                                                       }, // UTF8
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                                 BTP_SDP_PRIMARY_LANG_BASE,
     (unsigned char *)0                                                                                                                       }, //  Attr base
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,        BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)0                                                                                                                       }, // list end
  // Service Name
    { BTP_SDP_FLAG_ATT,                  0x0100, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0100,                              (unsigned char *)0}, // service name
    { BTP_SDP_FLAG_STR,
     0x0100,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)scBtpServiceIap2ServiceName                                                                                             }, // service name string
  // Provider Name
    { BTP_SDP_FLAG_ATT,                  0x0102, BTP_SDP_DTYPE_UINT, BTP_SDP_DSIZE_16, 0x0102,                              (unsigned char *)0}, // provider name
    { BTP_SDP_FLAG_STR,
     0x0102,                                     BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                               0,
     (unsigned char *)gcBthSdpProviderName                                                                                                    }, // provider name string
};

const btp_sdp_serviceItem_t gcBtpBrowseGroupServiceItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,       BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                       BTP_SDP_ATTR_RECORD_HANDLE,
     (unsigned char *)0                                                                                                                 }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,       BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,                                                       BTP_SDP_BROWSE_GROUP_HANDLE,
     (unsigned char *)0                                                                                                                 }, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,   BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,                                                       BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (unsigned char *)0                                                                                                                 }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                     0,
     (unsigned char *)0                                                                                                                 }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,   BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,                                                       BTP_SDP_SRVCLASS_ID_PUBLIC_BROWSE_GROUP,
     (unsigned char *)0                                                                                                                 }, //  list item[0]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,                                                     0,
     (unsigned char *)0                                                                                                                 },
 // Provider Name
    { BTP_SDP_FLAG_ATT,        0x0200, BTP_SDP_DTYPE_UUID, BTP_SDP_DSIZE_16, 0x0200,                                  (unsigned char *)0}, // provider name
    { BTP_SDP_FLAG_ATT,        0x0200, BTP_SDP_DTYPE_UUID, BTP_SDP_DSIZE_16, 0x1234,                                  (unsigned char *)0}, // provider name
  // string
};

const btp_sdp_serviceItem_t gcBtpHeadsetServiceItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_RECORD_HANDLE,
     (unsigned char *)0                          }, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,        BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,     BTP_SDP_HFP_HEADSET_HANDLE,
     (unsigned char *)0                          }, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,    BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (unsigned char *)0                          }, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin:
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,    BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_SRVCLASS_ID_HEADSET,
     (unsigned char *)0                          }, //  list item[0]
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,    BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_SRVCLASS_ID_GENERIC_AUDIO,
     (unsigned char *)0                          }, //  list item[1]
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, // list end
  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_PROTO_DESC_LIST,
     (unsigned char *)0                          },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_PROTOCOL_L2CAP_UUID,
     (unsigned char *)0                          }, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_PROTOCOL_RFCOMM_UUID,
     (unsigned char *)0                          }, //  list item[0] UUID = RFCOMM
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_8,      0x02,
     (unsigned char *)0                          }, //  list item[1]  rfcomm CHANNEL ID
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,      BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, // list end
  // Bluetooth Profile Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,    BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (unsigned char *)0                          },
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,    BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_VAR_16, BTP_SDP_ATTR_PROFILE_DESC_LIST,
     (unsigned char *)0                          }, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,    BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,     BTP_SDP_SRVCLASS_ID_HEADSET,
     (unsigned char *)0                          }, //  list item[0] UUID = HANDSET
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,    BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     0x0100,
     (unsigned char *)0                          }, //  list item[1] VERSION 1.5
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROFILE_DESC_LIST,    BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL,   0,
     (unsigned char *)0                          }, // list end
  //
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SERVICE_AVAILABILITY, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,     BTP_SDP_ATTR_SERVICE_AVAILABILITY,
     (unsigned char *)0                          }, //  att id = 8
    { BTP_SDP_FLAG_ATT | BTP_SDP_FLAG_W,
     BTP_SDP_ATTR_SERVICE_AVAILABILITY, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_8,      255,
     (unsigned char *)&gcBthSdpServiceAvalibility}, // time alive(seconds)
};


const btp_sdp_serviceItem_t gcBtpSdpAttItem[] = {
  // Service Record Handle
    {BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,   BTP_SDP_ATTR_RECORD_HANDLE,
     (unsigned char *)0}, // handle
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_RECORD_HANDLE,     BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_32,   BTP_SDP_ATT_HANDLE,
     (unsigned char *)0}, // handle
  // Service Class ID List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,   BTP_SDP_ATTR_SRV_CLASS_ID_LIST,
     (unsigned char *)0}, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST, BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (uint08 *)0       }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST, BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,   BTP_SDP_SRVCLASS_ID_GATT_SERVICE,
     (unsigned char *)0},
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_SRV_CLASS_ID_LIST, BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (uint08 *)0       }, //  list end
  // Protocol Descriptor List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,   BTP_SDP_ATTR_PROTO_DESC_LIST,
     (unsigned char *)0},
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (unsigned char *)0}, //  list begin protocol list
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (unsigned char *)0}, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,   BTP_SDP_PROTOCOL_L2CAP_UUID,
     (unsigned char *)0}, //  list item[0] UUID = L2CAP
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,   BTP_PSMID_ATT,
     (unsigned char *)0},
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (unsigned char *)0}, //  list end
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (unsigned char *)0}, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,   BTP_SDP_PROTOCOL_ATT_UUID,
     (unsigned char *)0}, //  list item[0] UUID = RFCOMM
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,   0,
     (unsigned char *)0}, // START ATT HANDLE						//
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,   11,
     (unsigned char *)0}, // END ATT HANDLE
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (unsigned char *)0}, //  list end
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_PROTO_DESC_LIST,   BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (unsigned char *)0}, // list end
  // Browse Group List
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST, BTP_SDP_DTYPE_UINT,
     BTP_SDP_DSIZE_16,   BTP_SDP_ATTR_BROWSE_GROUP_LIST,
     (unsigned char *)0}, //  list begin: sv class
    { BTP_SDP_FLAG_ATT_LIST_S,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST, BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (uint08 *)0       }, //  list begin
    { BTP_SDP_FLAG_ATT,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST, BTP_SDP_DTYPE_UUID,
     BTP_SDP_DSIZE_16,   BTP_SDP_ATTR_PUBLIC_BROWSE_ROOT,
     (unsigned char *)0},
    { BTP_SDP_FLAG_ATT_LIST_E,
     BTP_SDP_ATTR_BROWSE_GROUP_LIST, BTP_SDP_DTYPE_NULL,
     BTP_SDP_DSIZE_NULL, 0,
     (uint08 *)0       }, //  list end
};


// Note: BTP_SDP_BUFFER_SIZE.
const btp_sdp_serviceList_t scBthSdpServiceList[BTP_SDP_SRV_MAX_NUMB] = {
#if (TLKBTP_CFG_HIDD_ENABLE)
  //  Note:If the gcBthSdpPnpInfoItem open will cause the snk's audio path is the mobile phone,not the device.
  //{BTP_SDP_PNP_INFO_HANDLE, sizeof(gcBthSdpPnpInfoItem)/sizeof(btp_sdp_serviceItem_t), (btp_sdp_serviceItem_t
  //*)gcBthSdpPnpInfoItem},
    {BTP_SDP_HID_DEV_HANDLE,
     sizeof(gcBtpSdpHidItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBtpSdpHidItem                                                       },
#endif

#if (TLKBTP_CFG_SPP_ENABLE)
    { BTP_SDP_SPP_HANDLE,
     sizeof(gcBthSdpSppItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBthSdpSppItem                                                       },
#if (TLKBTP_CFG_SPP_USER_ENABLE)
    {BTP_SDP_SPP_USER_HANDLE,
     sizeof(gcBthSdpSppUserItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBthSdpSppUserItem                                                   },
#endif
#endif

#if (TLKBTP_CFG_HFPHF_ENABLE)
    { BTP_SDP_HFP_HF_HANDLE,
     sizeof(gcBthSdpHfpHfItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBthSdpHfpHfItem                                                     },
#endif
#if (TLKBTP_CFG_HFPAG_ENABLE)
    { BTP_SDP_HFP_AG_HANDLE,
     sizeof(gcBthSdpHfpAgItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBthSdpHfpAgItem                                                     },
#endif

#if (TLKBTP_CFG_A2DPSRC_ENABLE)
    { BTP_SDP_A2DP_SRC_HANDLE,
     sizeof(gcBthSdpA2dpSrcItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBthSdpA2dpSrcItem                                                   },
#endif
#if (TLKBTP_CFG_A2DPSNK_ENABLE)
    { BTP_SDP_A2DP_SNK_HANDLE,
     sizeof(gcBthSdpA2dpSnkItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBthSdpA2dpSnkItem                                                   },
#endif

#if (TLKBTP_CFG_AVRCP_ENABLE)
    { BTP_SDP_AVRCP_TG_HANDLE,
     sizeof(gcBthSdpAvrcpTgItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBthSdpAvrcpTgItem                                                   },
    { BTP_SDP_AVRCP_CT_HANDLE,
     sizeof(gcBthSdpAvrcpCtItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBthSdpAvrcpCtItem                                                   },
#endif
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
    { BTP_SDP_BROWSE_GROUP_HANDLE,
     sizeof(gcBtpBrowseGroupServiceItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBtpBrowseGroupServiceItem                                           },
#endif

#if TLKBTP_CFG_PTSSDP_ENABLE
    { BTP_SDP_SERVICE_HANDLE,
     sizeof(gcBtpServiceDiscoverItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBtpServiceDiscoverItem                                              },
#endif
#if (TLKBTP_CFG_IAP_ENABLE)
    { BTP_SDP_IAP_HANDLE,
     sizeof(gcBtpIap2ServiceItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBtpIap2ServiceItem                                                  },
#endif

#if (TLKBTP_CFG_ATTSRV_ENABLE)
    { BTP_SDP_ATT_HANDLE,
     sizeof(gcBtpSdpAttItem) / sizeof(btp_sdp_serviceItem_t),
     (btp_sdp_serviceItem_t *)gcBtpSdpAttItem                                                       },
#endif

    { 0xffffffff,                  0,                                     (btp_sdp_serviceItem_t *)0}
};


extern void btp_sdpsrv_setList(const btp_sdp_serviceList_t *cpList);
extern void btp_sdp_setCacheBuffer(uint08 *pBuffer, uint16 buffLen);


void btp_sdp_set_spp_user_srv_class_id(uint8_t spp_srv_user_class_id[16])
{
    tmemcpy(gBthSrvSppUserClassId, spp_srv_user_class_id, 16);
}

void btp_sdp_constInit(void)
{
    btp_sdpsrv_setList(scBthSdpServiceList);
}


#endif

#endif // #if (TLK_STK_BTP_ENABLE)
