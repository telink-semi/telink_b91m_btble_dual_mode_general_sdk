/********************************************************************************************************
 * @file	tlk_debug.c
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
#include "tlk_config.h"
#if (TLK_CFG_DBG_ENABLE)
#include "tlk_debug.h"
#include "compiler.h"
#include "stdbool.h"

#ifndef true
#define true  1
#endif

#ifndef false
#define false  0
#endif

#ifndef nullptr
#define nullptr  0
#endif

typedef struct{
	unsigned char minorID;
	unsigned char logIsEn;
	unsigned char vcdIsEn;
	unsigned char logFlag;
	const char *pItemName;
}tlk_debug_item_t;
typedef struct{
	unsigned char logIsEn;
	unsigned char vcdIsEn;
	unsigned char logFlag;
	unsigned char unitCnt;
	const char *pListName;
	tlk_debug_item_t unit[32];
}tlk_debug_list_t;


static const tlk_debug_list_t scTlkDebugSysInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_SYS_MAX, //unitCnt
	"System", //pListName
	{
		{TLK_MINOR_DBGID_SYS,     false, false, TLK_DEBUG_DBG_FLAG_ALL, "[SYS]"},
		{TLK_MINOR_DBGID_SYS_API, false, false, TLK_DEBUG_DBG_FLAG_ALL, "[API]"},
		{TLK_MINOR_DBGID_SYS_ALG, false, false, TLK_DEBUG_DBG_FLAG_ALL, "[ALG]"},
		{TLK_MINOR_DBGID_SYS_SCH, false, false, TLK_DEBUG_DBG_FLAG_ALL, "[SCH]"},
		{TLK_MINOR_DBGID_SYS_TSK, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[TSK]"},
	}
};
static const tlk_debug_list_t scTlkDebugBtcInfo = {
	false, //logIsEn
	true, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_BTC_MAX, //unitCnt
	"BT-Controller", //pListName
	{
		{TLK_MINOR_DBGID_BTC,        true,  true, TLK_DEBUG_DBG_FLAG_ALL, "[BTC]"},
		{TLK_MINOR_DBGID_BTC_LMP,    true,  true, TLK_DEBUG_DBG_FLAG_ALL, "[LMP]"},
		{TLK_MINOR_DBGID_BTC_ACL,    true,  true, TLK_DEBUG_DBG_FLAG_ALL, "[ACL]"},
		{TLK_MINOR_DBGID_BTC_SCO,    false, true, TLK_DEBUG_DBG_FLAG_ALL, "[SCO]"},
		{TLK_MINOR_DBGID_BTC_HCI,    true, true, TLK_DEBUG_DBG_FLAG_ALL, "[HCI]"},
		{TLK_MINOR_DBGID_BTC_EVT,    true,  true, TLK_DEBUG_DBG_FLAG_ALL, "[EVT]"},
		{TLK_MINOR_DBGID_BTC_ENC,    false, false, TLK_DEBUG_DBG_FLAG_ALL, "[ENC]"},
		{TLK_MINOR_DBGID_BTC_RSW,    false, true, TLK_DEBUG_DBG_FLAG_ALL, "[RSW]"},
		{TLK_MINOR_DBGID_BTC_AFH,    false, false, TLK_DEBUG_DBG_FLAG_ALL, "[AFH]"},
		{TLK_MINOR_DBGID_BTC_CHN,    false,  true, TLK_DEBUG_DBG_FLAG_ALL, "[CHN]"},
		{TLK_MINOR_DBGID_BTC_AUTH,   false, true, TLK_DEBUG_DBG_FLAG_ALL, "[AUTH]"},
		{TLK_MINOR_DBGID_BTC_PAIR,   false, true, TLK_DEBUG_DBG_FLAG_ALL, "[PAIR]"},
		{TLK_MINOR_DBGID_BTC_TASK,   false,  true, TLK_DEBUG_DBG_FLAG_ALL, "[TASK]"},
		{TLK_MINOR_DBGID_BTC_NAME,   false,  true, TLK_DEBUG_DBG_FLAG_ALL, "[NAME]"},
		{TLK_MINOR_DBGID_BTC_PAGE,   true,  true, TLK_DEBUG_DBG_FLAG_ALL, "[PAGE]"},
		{TLK_MINOR_DBGID_BTC_SNIFF,  false, true, TLK_DEBUG_DBG_FLAG_ALL, "[SNIFF]"},
		{TLK_MINOR_DBGID_BTC_TIMER,  false,  true, TLK_DEBUG_DBG_FLAG_ALL, "TIMER"},
		{TLK_MINOR_DBGID_BTC_RESET,  false, true, TLK_DEBUG_DBG_FLAG_ALL, "[RESET]"},
		{TLK_MINOR_DBGID_BTC_DETACH, true,  true, TLK_DEBUG_DBG_FLAG_ALL, "[DETACH]"},
	}
};
static const tlk_debug_list_t scTlkDebugBthInfo = {
	false, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_BTH_MAX, //unitCnt
	"BT-Host", //pListName
	{
		{TLK_MINOR_DBGID_BTH,     true, false, TLK_DEBUG_DBG_FLAG_ALL, "[BTH]"},
		{TLK_MINOR_DBGID_BTH_ACL, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[ACL]"},
		{TLK_MINOR_DBGID_BTH_SCO, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[SCO]"},
		{TLK_MINOR_DBGID_BTH_DEV, false, false, TLK_DEBUG_DBG_FLAG_ALL, "[DEV]"},
		{TLK_MINOR_DBGID_BTH_CMD, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[CMD]"},
		{TLK_MINOR_DBGID_BTH_EVT, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[EVT]"},
		{TLK_MINOR_DBGID_BTH_L2C, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[L2C]"},
		{TLK_MINOR_DBGID_BTH_SIG, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[SIG]"},
		{TLK_MINOR_DBGID_BTH_FUNC, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[FUNC]"},
	}
};
static const tlk_debug_list_t scTlkDebugBtpInfo = {
	false, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_BTP_MAX, //unitCnt
	"BT-Profile", //pListName
	{
		{TLK_MINOR_DBGID_BTP,        true,  false, TLK_DEBUG_DBG_FLAG_ALL, "[BTP]"},
		{TLK_MINOR_DBGID_BTP_SDP,    false, false, TLK_DEBUG_DBG_FLAG_ALL, "[SDP]"},
		{TLK_MINOR_DBGID_BTP_SDPC,   false, false, TLK_DEBUG_DBG_FLAG_ALL, "[SDPC]"},
		{TLK_MINOR_DBGID_BTP_SDPS,   false, false, TLK_DEBUG_DBG_FLAG_ALL, "[SDPS]"},
		{TLK_MINOR_DBGID_BTP_RFCOMM, false, false, TLK_DEBUG_DBG_FLAG_ALL, "[RFC]"},
		{TLK_MINOR_DBGID_BTP_IAP,    false, false, TLK_DEBUG_DBG_FLAG_ALL, "[IAP]"},
		{TLK_MINOR_DBGID_BTP_SPP,    false, false, TLK_DEBUG_DBG_FLAG_ALL, "[SPP]"},
		{TLK_MINOR_DBGID_BTP_ATT,    false, false, TLK_DEBUG_DBG_FLAG_ALL, "[ATT]"},
		{TLK_MINOR_DBGID_BTP_A2DP,   true,  false, TLK_DEBUG_DBG_FLAG_ALL, "[A2DP]"},
		{TLK_MINOR_DBGID_BTP_A2DPS,  true,  false, TLK_DEBUG_DBG_FLAG_ALL, "[A2DPS]"},
		{TLK_MINOR_DBGID_BTP_A2DPC,  true,  false, TLK_DEBUG_DBG_FLAG_ALL, "[A2DPC]"},
		{TLK_MINOR_DBGID_BTP_HID,    false, false, TLK_DEBUG_DBG_FLAG_ALL, "[HID]"},
		{TLK_MINOR_DBGID_BTP_HIDS,   false, false, TLK_DEBUG_DBG_FLAG_ALL, "[HIDS]"},
		{TLK_MINOR_DBGID_BTP_HIDC,   false, false, TLK_DEBUG_DBG_FLAG_ALL, "[HIDC]"},
		{TLK_MINOR_DBGID_BTP_HFP,    true,  false, TLK_DEBUG_DBG_FLAG_ALL, "[HFP]"},
		{TLK_MINOR_DBGID_BTP_HFPAG,  false, false, TLK_DEBUG_DBG_FLAG_ALL, "[HFPAG]"},
		{TLK_MINOR_DBGID_BTP_HFPHF,  true, false,  TLK_DEBUG_DBG_FLAG_ALL, "[HFPHF]"},
		{TLK_MINOR_DBGID_BTP_AVRCP,  true,  false, TLK_DEBUG_DBG_FLAG_ALL, "[AVRCP]"},
		{TLK_MINOR_DBGID_BTP_OBEX,   false, false, TLK_DEBUG_DBG_FLAG_ALL, "[OBEX]"},
		{TLK_MINOR_DBGID_BTP_PBAP,   false, false, TLK_DEBUG_DBG_FLAG_ALL, "[PBAP]"},
		{TLK_MINOR_DBGID_BTP_BROWSE, true, false,  TLK_DEBUG_DBG_FLAG_ALL, "[BROWSE]"},
		{TLK_MINOR_DBGID_BTP_FUNC,   false, false, TLK_DEBUG_DBG_FLAG_ALL, "[FUNC]"},
		{TLK_MINOR_DBGID_BTP_PTS,    true, false,  TLK_DEBUG_DBG_FLAG_ALL, "[PTS]"},
	}
};

static const tlk_debug_list_t scTlkDebugLecInfo = {
	false, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_LEC_MAX, //unitCnt
	"BLE-Controller", //pListName
	{
		{TLK_MINOR_DBGID_LEC_ADV,   true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_ADV]"},
		{TLK_MINOR_DBGID_LEC_CON, 	true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_CON]"},
		{TLK_MINOR_DBGID_LEC_INIT,  true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_INIT]"},
		{TLK_MINOR_DBGID_LEC_S, 	true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_SLAVE]"},
		{TLK_MINOR_DBGID_LEC_M, 	true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_MASTER]"},
		{TLK_MINOR_DBGID_LEC_SCAN,  true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_SCAN]"},
		{TLK_MINOR_DBGID_LEC_SCH,   true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_SCH]"},
		{TLK_MINOR_DBGID_LEC_HCI,   false, false, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_HCI]"},
	}
};
static const tlk_debug_list_t scTlkDebugLehInfo = {
	false, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_LEH_MAX, //unitCnt
	"BLE-Host", //pListName
	{
		{TLK_MINOR_DBGID_LEH_L2CAP, true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_L2CAP]"},
		{TLK_MINOR_DBGID_LEH_SMP,   true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_SMP]"},
		{TLK_MINOR_DBGID_LEH_GAP, 	true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_GAP]"},
		{TLK_MINOR_DBGID_LEH_GATT, 	true, true, TLK_DEBUG_DBG_FLAG_ALL, "[BLE_GATT]"},
		{TLK_MINOR_DBGID_LEH_FUNC, 	true, false, TLK_DEBUG_DBG_FLAG_ALL, "[LEH_FUNC]"},
	}
};
static const tlk_debug_list_t scTlkDebugLepInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_LEP_MAX, //unitCnt
	"BLE-Profile", //pListName
	{
		{TLK_MINOR_DBGID_LEP,       true, true, TLK_DEBUG_DBG_FLAG_ALL, "[LEP]"},
		{TLK_MINOR_DBGID_LEP_FUNC,  true, true, TLK_DEBUG_DBG_FLAG_ALL, "[LEP_FUNC]"},
	}
};

static const tlk_debug_list_t scTlkDebugDrvInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_DRV_MAX, //unitCnt
	"Driver", //pListName
	{
		{TLK_MINOR_DBGID_DRV,     true, false, TLK_DEBUG_DBG_FLAG_ALL, "[DRV]"},
		{TLK_MINOR_DBGID_DRV_EXT, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[DRV]"},
		{TLK_MINOR_DBGID_DRV_SYS, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[DRV]"},
		{TLK_MINOR_DBGID_DRV_USR, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[DRV]"},
	}
};
static const tlk_debug_list_t scTlkDebugDevInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_DEV_MAX, //unitCnt
	"Device", //pListName
	{
		{TLK_MINOR_DBGID_DEV,     true, false, TLK_DEBUG_DBG_FLAG_ALL, "[DEV]"},
		{TLK_MINOR_DBGID_DEV_EXT, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[DEV]"},
		{TLK_MINOR_DBGID_DEV_SYS, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[DEV]"},
		{TLK_MINOR_DBGID_DEV_USR, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[DEV]"},
	}
};
static const tlk_debug_list_t scTlkDebugAppInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_APP_MAX, //unitCnt
	"App", //pListName
	{
		{TLK_MINOR_DBGID_APP,     true, false, TLK_DEBUG_DBG_FLAG_ALL, "[APP]"},
	}
};
static const tlk_debug_list_t scTlkDebugMdiMiscInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_MDI_MAX, //unitCnt
	"MDI-MISC", //pListName
	{
		{TLK_MINOR_DBGID_MDI_MISC, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MISC]"},
		{TLK_MINOR_DBGID_MDI_COMM, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[COMM]"},
		{TLK_MINOR_DBGID_MDI_FILE, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[FILE]"},
		{TLK_MINOR_DBGID_MDI_FS,   true, false, TLK_DEBUG_DBG_FLAG_ALL, "[FS]"},
		{TLK_MINOR_DBGID_MDI_EVT,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[EVT]"},
		{TLK_MINOR_DBGID_MDI_KEY,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[KEY]"},
		{TLK_MINOR_DBGID_MDI_LED,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[LED]"},
		{TLK_MINOR_DBGID_MDI_USB,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[USB]"},
	}
};
static const tlk_debug_list_t scTlkDebugMdiAudInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_MDI_AUD_MAX, //unitCnt
	"MDI-Audio", //pListName
	{
		{TLK_MINOR_DBGID_MDI_AUD,      true, false, TLK_DEBUG_DBG_FLAG_ALL, "[AUD]"},
		{TLK_MINOR_DBGID_MDI_AUD_PLAY, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[PLAY]"},
		{TLK_MINOR_DBGID_MDI_AUD_TONE, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[TONE]"},
		{TLK_MINOR_DBGID_MDI_AUD_MP3,  false, false, TLK_DEBUG_DBG_FLAG_ALL, "[MP3]"},
		{TLK_MINOR_DBGID_MDI_AUD_HFP,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[HFP]"},
		{TLK_MINOR_DBGID_MDI_AUD_SCO,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[SCO]"},
		{TLK_MINOR_DBGID_MDI_AUD_SNK,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[SNK]"},
		{TLK_MINOR_DBGID_MDI_AUD_SRC,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[SRC]"},
		{TLK_MINOR_DBGID_MDI_AUD_UAC,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[UAC]"},
		{TLK_MINOR_DBGID_MDI_AUD_DSCO, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[DSCO]"},
	}
};
static const tlk_debug_list_t scTlkDebugMdiBtInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_MDI_BT_MAX, //unitCnt
	"MDI-BT", //pListName
	{
		{TLK_MINOR_DBGID_MDI_BT,      true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MBT]"},
		{TLK_MINOR_DBGID_MDI_BT_ACL,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MACL]"},
		{TLK_MINOR_DBGID_MDI_BT_ATT,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MATT]"},
		{TLK_MINOR_DBGID_MDI_BT_HFP,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MHFP]"},
		{TLK_MINOR_DBGID_MDI_BT_HID,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MHID]"},
		{TLK_MINOR_DBGID_MDI_BT_INQ,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MINQ]"},
		{TLK_MINOR_DBGID_MDI_BT_REC,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MREC]"},
		{TLK_MINOR_DBGID_MDI_BT_IAP,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MIAP]"},
		{TLK_MINOR_DBGID_MDI_BT_A2DP, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MA2DP]"},
		{TLK_MINOR_DBGID_MDI_BT_SCO,  true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MSCO]" },
		{TLK_MINOR_DBGID_MDI_BT_AG,   true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MHFPAG]"},
	}
};
static const tlk_debug_list_t scTlkDebugMdiLeInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_MDI_BT_MAX, //unitCnt
	"MDI-BLE", //pListName
	{
		{TLK_MINOR_DBGID_MDI_LE,      		true, false, TLK_DEBUG_DBG_FLAG_ALL, "[LE]"},
		{TLK_MINOR_DBGID_MDI_LE_ACL,   		true, false, TLK_DEBUG_DBG_FLAG_ALL, "[LEACL]"},
		{TLK_MINOR_DBGID_MDI_LE_COMMAND,    true, false, TLK_DEBUG_DBG_FLAG_ALL, "[LECMD]"},
		{TLK_MINOR_DBGID_MDI_LE_EVENT,    	true, false, TLK_DEBUG_DBG_FLAG_ALL, "[LEEVENT]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiAudioInfo = {
	false, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_MMI_AUDIO_MAX, //unitCnt
	"MMI-AUDIO", //pListName
	{
		{TLK_MINOR_DBGID_MMI_AUDIO,      true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MMI]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiBtmgrInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL, //logFlag
	TLK_MINOR_DBGID_MMI_BTMGR_MAX, //unitCnt
	"MMI-BTMGR", //pListName
	{
		{TLK_MINOR_DBGID_MMI_BTMGR, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MMI]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiLemgrInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL,
	TLK_MINOR_DBGID_MMI_LEMGR_MAX,
	"MMI-LEMGR", //pListName
	{
		{TLK_MINOR_DBGID_MMI_LEMGR, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MMI]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiPhoneInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL,
	TLK_MINOR_DBGID_MMI_PHONE_MAX,
	"MMI-CALL", //pListName
	{
		{TLK_MINOR_DBGID_MMI_PHONE, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MMI]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiFileInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL,
	TLK_MINOR_DBGID_MMI_FILE_MAX,
	"MMI-FILE", //pListName
	{
		{TLK_MINOR_DBGID_MMI_FILE, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MMI]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiViewInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL,
	TLK_MINOR_DBGID_MMI_VIEW_MAX,
	"MMI-VIEW", //pListName
	{
		{TLK_MINOR_DBGID_MMI_VIEW, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MMI]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiTestInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL,
	TLK_MINOR_DBGID_MMI_TEST_MAX,
	"MMI-TEST", //pListName
	{
		{TLK_MINOR_DBGID_MMI_TEST, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[TST]"},
		{TLK_MINOR_DBGID_MMI_TEST_PTS, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[PTS]"},
		{TLK_MINOR_DBGID_MMI_TEST_FAT, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[FAT]"},
		{TLK_MINOR_DBGID_MMI_TEST_EMI, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[EMI]"},
		{TLK_MINOR_DBGID_MMI_TEST_RDT, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[RDT]"},
		{TLK_MINOR_DBGID_MMI_TEST_ATS, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[ATS]"},
		{TLK_MINOR_DBGID_MMI_TEST_USR, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[USR]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiSystemInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL,
	TLK_MINOR_DBGID_MMI_SYSTEM_MAX,
	"MMI-SYSTEM", //pListName
	{
		{TLK_MINOR_DBGID_MMI_SYSTEM, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MSYS]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiDebugInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL,
	TLK_MINOR_DBGID_MMI_DEBUG_MAX,
	"MMI-DEBUG", //pListName
	{
		{TLK_MINOR_DBGID_MMI_DEBUG, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MDBG]"},
	}
};
static const tlk_debug_list_t scTlkDebugMmiDeviceInfo = {
	true, //logIsEn
	false, //vcdIsEn
	TLK_DEBUG_DBG_FLAG_ALL,
	TLK_MINOR_DBGID_MMI_DEVICE_MAX,
	"MMI-DEVICE", //pListName
	{
		{TLK_MINOR_DBGID_MMI_DEVICE, true, false, TLK_DEBUG_DBG_FLAG_ALL, "[MDEV]"},
	}
};
//static
static const tlk_debug_list_t *scTlkDebugInfo[TLK_MAJOR_DBGID_MAX] = {
	nullptr, //TLK_MAJOR_DBGID_NONE
	&scTlkDebugSysInfo, //TLK_MAJOR_DBGID_SYS
	&scTlkDebugBtcInfo, //TLK_MAJOR_DBGID_BTC
	&scTlkDebugBthInfo, //TLK_MAJOR_DBGID_BTH
	&scTlkDebugBtpInfo, //TLK_MAJOR_DBGID_BTP
	&scTlkDebugLecInfo, //TLK_MAJOR_DBGID_LEC
	&scTlkDebugLehInfo, //TLK_MAJOR_DBGID_LEH
	&scTlkDebugLepInfo, //TLK_MAJOR_DBGID_LEP
	&scTlkDebugDrvInfo, //TLK_MAJOR_DBGID_DRV
	&scTlkDebugDevInfo, //TLK_MAJOR_DBGID_DEV
	&scTlkDebugAppInfo, //TLK_MAJOR_DBGID_APP
	&scTlkDebugMdiMiscInfo, //TLK_MAJOR_DBGID_MDI_MISC
	&scTlkDebugMdiAudInfo, //TLK_MAJOR_DBGID_MDI_AUDIO
	&scTlkDebugMdiBtInfo, //TLK_MAJOR_DBGID_MDI_BT
	&scTlkDebugMdiLeInfo, //TLK_MAJOR_DBGID_MDI_LE
	&scTlkDebugMmiAudioInfo, //TLK_MAJOR_DBGID_MMI_AUDIO
	&scTlkDebugMmiBtmgrInfo, //TLK_MAJOR_DBGID_MMI_BTMGR
	&scTlkDebugMmiLemgrInfo, //TLK_MAJOR_DBGID_MMI_LEMGR
	&scTlkDebugMmiPhoneInfo, //TLK_MAJOR_DBGID_MMI_PHONE
	&scTlkDebugMmiFileInfo, //TLK_MAJOR_DBGID_MMI_FILE
	&scTlkDebugMmiViewInfo, //TLK_MAJOR_DBGID_MMI_VIEW
	&scTlkDebugMmiTestInfo, //TLK_MAJOR_DBGID_MMI_TEST
	&scTlkDebugMmiSystemInfo, //TLK_MAJOR_DBGID_MMI_SYSTEM
	&scTlkDebugMmiDebugInfo, //TLK_MAJOR_DBGID_MMI_DEBUG
	&scTlkDebugMmiDeviceInfo, //TLK_MAJOR_DBGID_MMI_DEVICE
};

static unsigned long sTlkDebugLogMask[TLK_MAJOR_DBGID_MAX];
#if (TLK_DBG_VCD_ENABLE)
static unsigned long sTlkDebugVcdMask[TLK_MAJOR_DBGID_MAX];
#endif





void tlk_debug_init(void)
{
	tlk_debug_logLoad();
	tlk_debug_vcdLoad();
}
void tlk_debug_logLoad(void)
{
	unsigned int idxI;
	unsigned int idxJ;
	for(idxI=0; idxI<TLK_MAJOR_DBGID_MAX; idxI++){
		sTlkDebugLogMask[idxI] = 0;
		for(idxJ=0; idxJ<32; idxJ++){
			if(scTlkDebugInfo[idxI] != nullptr && idxJ < scTlkDebugInfo[idxI]->unitCnt
				&& scTlkDebugInfo[idxI]->logIsEn && scTlkDebugInfo[idxI]->unit[idxJ].logIsEn){
				sTlkDebugLogMask[idxI] |= (1 << idxJ);
			}
		}
	}
}
void tlk_debug_vcdLoad(void)
{
#if (TLK_DBG_VCD_ENABLE)
	unsigned int idxI;
	unsigned int idxJ;
	for(idxI=0; idxI<TLK_MAJOR_DBGID_MAX; idxI++){
		sTlkDebugVcdMask[idxI] = 0;
		for(idxJ=0; idxJ<32; idxJ++){
			if(scTlkDebugInfo[idxI] != nullptr && idxJ < scTlkDebugInfo[idxI]->unitCnt
				&& scTlkDebugInfo[idxI]->vcdIsEn && scTlkDebugInfo[idxI]->unit[idxJ].vcdIsEn){
				sTlkDebugVcdMask[idxI] |= (1 << idxJ);
			}
		}
	}
#endif
}


_attribute_ram_code_sec_noinline_
bool tlk_debug_logIsEnable(unsigned int flags, unsigned int logFlag)
{
	unsigned char majorID = (flags >> 24);
	unsigned char minorID = (flags >> 16) & 0xFF;
	if(flags == 0xFFFFFFFF) return true;
	if((flags & logFlag) == 0 || majorID >= TLK_MAJOR_DBGID_MAX || minorID >= 32
		|| (sTlkDebugLogMask[majorID] & (1 << minorID)) == 0
		|| scTlkDebugInfo[majorID] == nullptr
		|| (scTlkDebugInfo[majorID]->logFlag & logFlag) == 0
		|| (scTlkDebugInfo[majorID]->unit[minorID].logFlag & logFlag) == 0){
		return false;
	}
	return true;
}
_attribute_ram_code_sec_noinline_
bool tlk_debug_logIsEnable1(unsigned int flags)
{
	unsigned char majorID = (flags >> 24);
	unsigned char minorID = (flags >> 16) & 0xFF;
	if(flags == 0xFFFFFFFF) return true;
	if(majorID >= TLK_MAJOR_DBGID_MAX || minorID >= 32 || (sTlkDebugLogMask[majorID] & (1 << minorID)) == 0){
		return false;
	}
	return true;
}

_attribute_ram_code_sec_noinline_
bool tlk_debug_vcdIsEnable(unsigned int flags)
{
	#if (TLK_DBG_VCD_ENABLE)
	unsigned char majorID = (flags >> 24);
	unsigned char minorID = (flags >> 16) & 0xFF;
	if(flags == 0xFFFFFFFF) return true;
	if(/*(flags & 0x01) == 0 || */majorID >= TLK_MAJOR_DBGID_MAX || minorID >= 32
		|| (sTlkDebugVcdMask[majorID] & (1 << minorID)) == 0){
		return false;
	}
	return true;
	#else
	return false;
	#endif
}

_attribute_ram_code_sec_noinline_
char *tlk_debug_getDbgSign(unsigned int flags)
{
	unsigned char majorID = (flags >> 24);
	unsigned char minorID = (flags >> 16) & 0xFF;
	if(majorID >= TLK_MAJOR_DBGID_MAX  || scTlkDebugInfo[majorID] == nullptr
		|| minorID >= 32 || (sTlkDebugLogMask[majorID] & (1 << minorID)) == 0){
		return nullptr;
	}
	return (char*)scTlkDebugInfo[majorID]->unit[minorID].pItemName;
}


bool tlk_debug_setLogMask(TLK_DEBUG_MAJOR_ID_ENUM majorID, unsigned int mask)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX) return false;
	sTlkDebugLogMask[majorID] = mask;
	return true;
}
bool tlk_debug_setVcdMask(TLK_DEBUG_MAJOR_ID_ENUM majorID, unsigned int mask)
{
#if (TLK_DBG_VCD_ENABLE)
	if(majorID >= TLK_MAJOR_DBGID_MAX) return false;
	sTlkDebugVcdMask[majorID] = mask;
	return true;
#else
	return false;
#endif
}

unsigned int tlk_debug_getLogMask(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX) return 0;
	else return sTlkDebugLogMask[majorID];
}
unsigned int tlk_debug_getVcdMask(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
#if (TLK_DBG_VCD_ENABLE)
	if(majorID >= TLK_MAJOR_DBGID_MAX) return 0;
	else return sTlkDebugVcdMask[majorID];
#else
	return 0;
#endif
}

unsigned int tlk_debug_getListNumb(void)
{
	return TLK_MAJOR_DBGID_MAX;
}
unsigned int tlk_debug_getItemNumb(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX || scTlkDebugInfo[majorID] == nullptr) return 0;
	return scTlkDebugInfo[majorID]->unitCnt;
}

unsigned int tlk_debug_getListFlag(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX || scTlkDebugInfo[majorID] == nullptr) return 0;
	return scTlkDebugInfo[majorID]->logFlag;
}
unsigned int tlk_debug_getItemFlag(TLK_DEBUG_MAJOR_ID_ENUM majorID, TLK_DEBUG_MINOR_ID_ENUM minorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX || scTlkDebugInfo[majorID] == nullptr 
		|| minorID >= scTlkDebugInfo[majorID]->unitCnt){
		return 0;
	}
	return scTlkDebugInfo[majorID]->unit[minorID].logFlag;
}


const char *tlk_debug_getListName(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX || scTlkDebugInfo[majorID] == nullptr){
		return nullptr;
	}
	return scTlkDebugInfo[majorID]->pListName;
}
const char *tlk_debug_getItemName(TLK_DEBUG_MAJOR_ID_ENUM majorID, TLK_DEBUG_MINOR_ID_ENUM minorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX || scTlkDebugInfo[majorID] == nullptr
		|| minorID >= scTlkDebugInfo[majorID]->unitCnt){
		return nullptr;
	}
	return scTlkDebugInfo[majorID]->unit[minorID].pItemName;
}

bool tlk_debug_logListIsEnable(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX || sTlkDebugLogMask[majorID] == 0){
		return false;
	}
	return true;
}
bool tlk_debug_vcdListIsEnable(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
#if (TLK_DBG_VCD_ENABLE)
	if(majorID >= TLK_MAJOR_DBGID_MAX || sTlkDebugVcdMask[majorID] == 0){
		return false;
	}
	return true;
#else
	return false;
#endif
}

bool tlk_debug_logItemIsEnable(TLK_DEBUG_MAJOR_ID_ENUM majorID, TLK_DEBUG_MINOR_ID_ENUM minorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX  || scTlkDebugInfo[majorID] == nullptr
		|| minorID >= scTlkDebugInfo[majorID]->unitCnt){
		return false;
	}
	if((sTlkDebugLogMask[majorID] & (1 << minorID)) != 0) return true;
	return false;
}
bool tlk_debug_vcdItemIsEnable(TLK_DEBUG_MAJOR_ID_ENUM majorID, TLK_DEBUG_MINOR_ID_ENUM minorID)
{
#if (TLK_DBG_VCD_ENABLE)
	if(majorID >= TLK_MAJOR_DBGID_MAX  || scTlkDebugInfo[majorID] == nullptr
		|| minorID >= scTlkDebugInfo[majorID]->unitCnt){
		return false;
	}
	if((sTlkDebugVcdMask[majorID] & (1 << minorID)) != 0) return true;
	return false;
#else
	return false;
#endif
}

bool tlk_debug_enableLogList(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX){
		return false;
	}
	sTlkDebugLogMask[majorID] = 0xFFFFFFFF;
	return true;
}
bool tlk_debug_enableVcdList(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
#if (TLK_DBG_VCD_ENABLE)
	if(majorID >= TLK_MAJOR_DBGID_MAX){
		return false;
	}
	sTlkDebugVcdMask[majorID] = 0xFFFFFFFF;
	return true;
#else
	return false;
#endif
}
bool tlk_debug_disableLogList(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX){
		return false;
	}
	sTlkDebugLogMask[majorID] = 0;
	return true;
}
bool tlk_debug_disableVcdList(TLK_DEBUG_MAJOR_ID_ENUM majorID)
{
#if (TLK_DBG_VCD_ENABLE)
	if(majorID >= TLK_MAJOR_DBGID_MAX){
		return false;
	}
	sTlkDebugVcdMask[majorID] = 0;
	return true;
#else
	return false;
#endif
}

bool tlk_debug_enableLogItem(TLK_DEBUG_MAJOR_ID_ENUM majorID, TLK_DEBUG_MINOR_ID_ENUM minorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX  || scTlkDebugInfo[majorID] == nullptr
		|| minorID >= scTlkDebugInfo[majorID]->unitCnt){
		return false;
	}
	sTlkDebugLogMask[majorID] |= (1 << minorID);
	return true;
}
bool tlk_debug_enableVcdItem(TLK_DEBUG_MAJOR_ID_ENUM majorID, TLK_DEBUG_MINOR_ID_ENUM minorID)
{
#if (TLK_DBG_VCD_ENABLE)
	if(majorID >= TLK_MAJOR_DBGID_MAX  || scTlkDebugInfo[majorID] == nullptr
		|| minorID >= scTlkDebugInfo[majorID]->unitCnt){
		return false;
	}
	sTlkDebugVcdMask[majorID] |= (1 << minorID);
	return true;
#else
	return false;
#endif
}
bool tlk_debug_disableLogItem(TLK_DEBUG_MAJOR_ID_ENUM majorID, TLK_DEBUG_MINOR_ID_ENUM minorID)
{
	if(majorID >= TLK_MAJOR_DBGID_MAX  || scTlkDebugInfo[majorID] == nullptr
		|| minorID >= scTlkDebugInfo[majorID]->unitCnt){
		return false;
	}
	sTlkDebugLogMask[majorID] &= ~(1 << minorID);
	return true;
}
bool tlk_debug_disableVcdItem(TLK_DEBUG_MAJOR_ID_ENUM majorID, TLK_DEBUG_MINOR_ID_ENUM minorID)
{
#if (TLK_DBG_VCD_ENABLE)
	if(majorID >= TLK_MAJOR_DBGID_MAX  || scTlkDebugInfo[majorID] == nullptr
		|| minorID >= scTlkDebugInfo[majorID]->unitCnt){
		return false;
	}
	sTlkDebugVcdMask[majorID] &= ~(1 << minorID);
	return true;
#else
	return false;
#endif
}

void tlk_debug_DisableAllLog(void)
{
	unsigned int idxI;
	for (idxI = 0; idxI < TLK_MAJOR_DBGID_MAX; idxI++) {
		sTlkDebugLogMask[idxI] = 0;
	}
}
void tlk_debug_DisableAllVcd(void)
{
#if (TLK_DBG_VCD_ENABLE)
	unsigned int idxI;
	for (idxI = 0; idxI < TLK_MAJOR_DBGID_MAX; idxI++) {
		sTlkDebugVcdMask[idxI] = 0;
	}
#endif
}


#endif //#if (TLK_CFG_DBG_ENABLE)

