/********************************************************************************************************
 * @file	tlkmdi_bta2dp.c
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
#if (TLK_MDI_BTA2DP_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/bt/tlkmdi_bta2dp.h"

#if (TLK_STK_BTP_ENABLE)
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"
#include "tlkstk/bt/btp/browsing/btp_browsing.h"
#endif


#define TLKMDI_BTA2DP_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_A2DP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTA2DP_DBG_SIGN       "[MA2DP]"

#if (TLK_STK_BTP_ENABLE)
static int tlkmdi_btavrcp_volumeChangeEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btavrcp_peerEvtMaskEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btavrcp_trackChangeEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btavrcp_playbackPosChgEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btavrcp_playerAppSettingChgEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btavrcp_playerAddressChgEvt(uint08 *pData, uint16 dataLen);
static void tlkmdi_btavrcp_insCompleteCB(uint16 aclHandle, uint08 pduID, uint08 status, void *pParam, uint16 paramLen);
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
static void tlkmdi_btBrowsing_itemsReportCB(uint16 aclHandle, uint08 pduID, uint08 status, uint08 itemType, uint08 itemNumb, void *pItemData);
static void tlkmdi_btBrowsing_insCompleteCB(uint16 aclHandle, uint08 pduID, uint08 status, void *pParam, uint16 paramLen);
#endif
#endif


/******************************************************************************
 * Function: tlkmdi_hfphf_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_bta2dp_init(void)
{	
	#if (TLK_STK_BTP_ENABLE)
	btp_event_regCB(BTP_EVTID_AVRCP_VOLUME_CHANGED, tlkmdi_btavrcp_volumeChangeEvt);
	btp_event_regCB(BTP_EVTID_AVRCP_PEER_EVT_MASK,  tlkmdi_btavrcp_peerEvtMaskEvt);
	btp_event_regCB(BTP_EVTID_AVRCP_TRACK_CHANGED,  tlkmdi_btavrcp_trackChangeEvt);
	btp_event_regCB(BTP_EVTID_AVRCP_PLAYBACK_POS_CHANGED,   tlkmdi_btavrcp_playbackPosChgEvt);
	btp_event_regCB(BTP_EVTID_AVRCP_PLAYER_APP_SETTING_CHANGED, tlkmdi_btavrcp_playerAppSettingChgEvt);
	btp_event_regCB(BTP_EVTID_AVRCP_PLAYER_ADDRESSED_CHANGED,   tlkmdi_btavrcp_playerAddressChgEvt);
	btp_avrcp_regInsCompleteCB(tlkmdi_btavrcp_insCompleteCB);
	#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
	btp_browsing_regItemsReportCB(tlkmdi_btBrowsing_itemsReportCB);
	btp_browsing_regInsCompleteCB(tlkmdi_btBrowsing_insCompleteCB);
	#endif //#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
	#endif
	
	return TLK_ENONE;
}

void tlkmdi_bta2dp_connectEvt(uint16 aclHandle, uint08 usrID)
{
#if (TLK_STK_BTP_ENABLE)
	if(usrID == BTP_USRID_SERVER){
		btp_avrcp_sendRegEventNotify(aclHandle, BTP_AVRCP_EVTID_VOLUME_CHANGED);
	}else if(usrID == BTP_USRID_CLIENT){
		#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
		btp_browsing_connect(aclHandle);
		#endif //#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
		btp_avrcp_sendRegEventNotify(aclHandle, BTP_AVRCP_EVTID_PLAYBACK_STATUS_CHANGED);
		btp_avrcp_sendRegEventNotify(aclHandle, BTP_AVRCP_EVTID_TRACK_CHANGED);
		btp_avrcp_sendRegEventNotify(aclHandle, BTP_AVRCP_EVTID_PLAYBACK_POS_CHANGED);
		btp_avrcp_sendRegEventNotify(aclHandle, BTP_AVRCP_EVTID_PLAYER_APP_SETTING_CHANGED);
		btp_avrcp_sendRegEventNotify(aclHandle, BTP_AVRCP_EVTID_ADDRESSED_PLAYER_CHANGED);
	}
#endif
}

#if (TLK_STK_BTP_ENABLE)
static int tlkmdi_btavrcp_volumeChangeEvt(uint08 *pData, uint16 dataLen)
{
	uint08 buffer[6];
	uint08 buffLen = 0;
	btp_avrcpVolumeChangeEvt_t *pEvt;
	
	pEvt = (btp_avrcpVolumeChangeEvt_t*)pData;
	buffer[buffLen++] = (pEvt->handle & 0xFF);
	buffer[buffLen++] = (pEvt->handle & 0xFF00) >> 8;
	buffer[buffLen++] = btp_hfphf_isIosDev(pEvt->handle);
	if(btp_a2dp_isSrc(pEvt->handle)) buffer[buffLen++] = 0x01;
	else buffer[buffLen++] = 0x00;
	buffer[buffLen++] = pEvt->volume;
	tlksys_sendInnerMsg(TLKSYS_TASKID_AUDIO, TLKPTI_AUD_MSGID_AVRCP_CHG_VOLUME_EVT, buffer, buffLen);

	return TLK_ENONE;
}
static int tlkmdi_btavrcp_peerEvtMaskEvt(uint08 *pData, uint16 dataLen)
{
	btp_avrcpPeerEvtMaskEvt_t *pEvt = (btp_avrcpPeerEvtMaskEvt_t*)pData;
	if(btp_a2dp_isSrc(pEvt->handle)){
		btp_avrcp_sendRegEventNotify(pEvt->handle, BTP_AVRCP_EVTID_VOLUME_CHANGED);
	}else if(btp_a2dp_isSnk(pEvt->handle)){
		#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
		btp_browsing_connect(pEvt->handle);
		#endif
		btp_avrcp_sendRegEventNotify(pEvt->handle, BTP_AVRCP_EVTID_PLAYBACK_STATUS_CHANGED);
		btp_avrcp_sendRegEventNotify(pEvt->handle, BTP_AVRCP_EVTID_TRACK_CHANGED);
		btp_avrcp_sendRegEventNotify(pEvt->handle, BTP_AVRCP_EVTID_PLAYBACK_POS_CHANGED);
		btp_avrcp_sendRegEventNotify(pEvt->handle, BTP_AVRCP_EVTID_PLAYER_APP_SETTING_CHANGED);
		btp_avrcp_sendRegEventNotify(pEvt->handle, BTP_AVRCP_EVTID_ADDRESSED_PLAYER_CHANGED);
	}
	return TLK_ENONE;
}
static int tlkmdi_btavrcp_trackChangeEvt(uint08 *pData, uint16 dataLen)
{
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
	uint32 attrList[7];
	uint08 invalidID1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint08 invalidID2[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	btp_avrcpTrackChangeEvt_t *pEvt;
	
	pEvt = (btp_avrcpTrackChangeEvt_t*)pData;
	tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN, "tlkmdi_btavrcp_trackChangeEvt:", pEvt->pTrackID, 8);

	if(tmemcmp(pEvt->pTrackID, invalidID1, 8) == 0){
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN, "tlkmdi_btavrcp_trackChangeEvt: This player does not support browsing");
		return TLK_ENONE;
	}
	if(tmemcmp(pEvt->pTrackID, invalidID2, 8) == 0){
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN, "tlkmdi_btavrcp_trackChangeEvt: No music is playing");
		return TLK_ENONE;
	}
	
	/*Get the information of music by track ID*/
	attrList[0] = BTP_BROWSING_ATTID_TITLE;
	attrList[1] = BTP_BROWSING_ATTID_ARTIST_NAME;
	attrList[2] = BTP_BROWSING_ATTID_ALBUM_NAME;
	attrList[3] = BTP_BROWSING_ATTID_TRACK_NUMBER;
	attrList[4] = BTP_BROWSING_ATTID_TOTAL_TRACKS;
	attrList[5] = BTP_BROWSING_ATTID_GENRE;
	attrList[6] = BTP_BROWSING_ATTID_PLAYING_TIME;
	btp_browsing_sendGetItemAttrCmd(pEvt->handle, BTP_BROWSING_SCOPE_NOW_PLAYGING, pEvt->pTrackID, 0, 7, attrList);
#endif
	return TLK_ENONE;
}
static int tlkmdi_btavrcp_playbackPosChgEvt(uint08 *pData, uint16 dataLen)
{
	btp_avrcpPlaybackPosChangeEvt_t *pEvt;
	
	pEvt = (btp_avrcpPlaybackPosChangeEvt_t*)pData;
	tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN, "tlkmdi_btavrcp_playbackPosChgEvt: %dms", pEvt->playPos);
	
	return TLK_ENONE;
}
static int tlkmdi_btavrcp_playerAppSettingChgEvt(uint08 *pData, uint16 dataLen)
{
	btp_avrcpPlayerAppSettingChangeEvt_t *pEvt;
	
	pEvt = (btp_avrcpPlayerAppSettingChangeEvt_t*)pData;
	tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN, "tlkmdi_btavrcp_playerAppSettingChgEvt:", pEvt->pParam, pEvt->paramLen);
	
	return TLK_ENONE;
}
static int tlkmdi_btavrcp_playerAddressChgEvt(uint08 *pData, uint16 dataLen)
{
	btp_avrcpPlayerAddressedChangeEvt_t *pEvt;
	
	pEvt = (btp_avrcpPlayerAddressedChangeEvt_t*)pData;
	tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN, "tlkmdi_btavrcp_playerAddressChgEvt:", pEvt->pParam, pEvt->paramLen);
	
	return TLK_ENONE;
}

static void tlkmdi_btavrcp_insCompleteCB(uint16 aclHandle, uint08 pduID, uint08 status, void *pParam, uint16 paramLen)
{
	if(status != BTP_AVRCP_STATUS_CODE_OPERATE_WITHOUT_ERROR){
		tlkapi_error(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB: pduID[0x%02x], status[%d]", pduID, status);
		return;
	}
	if(pduID == BTP_AVRCP_PDUID_GET_PLAY_STATUS){
		btp_avrcp_getPlayStatusRsp_t *pRsp = (btp_avrcp_getPlayStatusRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_GET_PLAY_STATUS:");
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"songLength: %dms, songPosition: %dms, playStatus: %d", 
			pRsp->songLength, pRsp->songPosition, pRsp->playStatus);
	}else if(pduID == BTP_AVRCP_PDUID_LIST_PAS_ATTRS){//ListPlayerApplicationSettingAttributes, AV/C STATUS
//		btp_avrcp_listPasAttrRsp_t *pRsp = (btp_avrcp_listPasAttrRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_LIST_PAS_ATTRS:");
	}else if(pduID == BTP_AVRCP_PDUID_LIST_PAS_VALUES){//ListPlayerApplicationSettingValues, AV/C STATUS
//		btp_avrcp_listPasValueRsp_t *pRsp = (btp_avrcp_listPasValueRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_LIST_PAS_VALUES:");
	}else if(pduID == BTP_AVRCP_PDUID_GET_CUR_PAS_VALUR){//GetCurrentPlayerApplicationSettingValue, AV/C STATUS
//		btp_avrcp_getCurlistPasValueRsp_t *pRsp = (btp_avrcp_getCurlistPasValueRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_GET_CUR_PAS_VALUR:");
	}else if(pduID == BTP_AVRCP_PDUID_SET_PAS_VALUE){//AV/C CONTROL, AV/C CONTROL
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_SET_PAS_VALUE:");
	}else if(pduID == BTP_AVRCP_PDUID_GET_PAS_ATTR_TEXT){//GetPlayerApplicationSettingAttributeText , AV/C STATUS
//		btp_avrcp_getPasAttrTextRsp_t *pRsp = (btp_avrcp_getPasAttrTextRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_GET_PAS_ATTR_TEXT:");
	}else if(pduID == BTP_AVRCP_PDUID_GET_PAS_VALUE_TEXT){//GetPlayerApplicationSettingValueText, AV/C STATUS
//		btp_avrcp_getPasValueTextRsp_t *pRsp = (btp_avrcp_getPasValueTextRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_GET_PAS_VALUE_TEXT:");
	}else if(pduID == BTP_AVRCP_PDUID_INFORM_DISPLAY_CHARSET){//GetPlayerApplicationSettingValueText, AV/C CONTROL
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_INFORM_DISPLAY_CHARSET:");
	}else if(pduID == BTP_AVRCP_PDUID_INFORM_BATTERY_STATUS){//InformBatteryStatusOfCT, AV/C CONTROL
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_INFORM_BATTERY_STATUS:");
	}else if(pduID == BTP_AVRCP_PDUID_GET_ELEMENT_ATTR){//Metadata Attributes for Current Media Item
//		btp_avrcp_getElementAttrRsp_t *pRsp = (btp_avrcp_getElementAttrRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_GET_ELEMENT_ATTR:");
	}else if(pduID == BTP_AVRCP_PDUID_SET_ABSOLUTE_VOLUME){//Absolute Volume
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_SET_ABSOLUTE_VOLUME:");
	}else if(pduID == BTP_AVRCP_PDUID_ADD_PLAYING){//SetAddressedPlayer, AV/C CONTROL
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btavrcp_insCompleteCB-BTP_AVRCP_PDUID_ADD_PLAYING:");
	}else{//
		tlkapi_warn(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_insCompleteCB: Unknown pduID[%d]", pduID);
	}
}
#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
static void tlkmdi_btBrowsing_itemsReportCB(uint16 aclHandle, uint08 pduID, uint08 status, uint08 itemType, uint08 itemNumb, void *pItemData)
{
	if(status != TLK_ENONE){
		tlkapi_error(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_itemsReportCB: item parse error [%d]", status);
		return;
	}
	if(itemType == BTP_BROWSING_ITEM_TYPE_MEDIA_PLAYER_ITEM){
		uint08 index;
		btp_browsing_playerItem_t *pItem = (btp_browsing_playerItem_t*)pItemData;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_itemsReportCB: BTP_BROWSING_ITEM_TYPE_MEDIA_PLAYER_ITEM [%d]", itemNumb);
		for(index=0; index<itemNumb; index++){
			tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
				"BTP_BROWSING_ITEM_TYPE_MEDIA_PLAYER_ITEM: playerID[%d],playerType[%d],playStatus[%d]",
				pItem[index].playerID, pItem[index].playerType, pItem[index].playStatus);
		}
		if(itemNumb != 0){
			btp_browsing_setBrowsedPlayerCmd(aclHandle, pItem[0].playerID);
		}
	}else if(itemType == BTP_BROWSING_ITEM_TYPE_FOLDER_ITEM){
		uint08 index;
		btp_browsing_folderItem_t *pItem = (btp_browsing_folderItem_t*)pItemData;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_itemsReportCB: BTP_BROWSING_ITEM_TYPE_FOLDER_ITEM [%d]", itemNumb);
		for(index=0; index<itemNumb; index++){
			tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
				"BTP_BROWSING_ITEM_TYPE_FOLDER_ITEM: folderType[%d],playabe[%d]",
				pItem[index].folderType, pItem[index].playabe);
		}
	}else if(itemType == BTP_BROWSING_ITEM_TYPE_MEDIA_ELEMENT_ITEM){
		uint08 index;
		btp_browsing_elementItem_t *pItem = (btp_browsing_elementItem_t*)pItemData;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_itemsReportCB: BTP_BROWSING_ITEM_TYPE_MEDIA_ELEMENT_ITEM [%d]", itemNumb);
		for(index=0; index<itemNumb; index++){
			tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
				"BTP_BROWSING_ITEM_TYPE_MEDIA_ELEMENT_ITEM: mediaType[%d],charSet[0x%x]",
				pItem[index].mediaType, pItem[index].charSet);
		}
	}else if(itemType == BTP_BROWSING_ITEM_TYPE_ATTR_LIST){
		uint08 index;
		btp_browsing_attrItem_t *pItem = (btp_browsing_attrItem_t*)pItemData;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_itemsReportCB: BTP_BROWSING_ITEM_TYPE_ATTR_LIST [%d]", itemNumb);
		for(index=0; index<itemNumb; index++){
			if(pItem[index].attrID == BTP_BROWSING_ATTID_TITLE){
				tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
					"BTP_BROWSING_ITEM_TYPE_ATTR_LIST[Title]: ", 
					pItem[index].pValue, pItem[index].valLens);
			}else if(pItem[index].attrID == BTP_BROWSING_ATTID_ARTIST_NAME){
				tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
					"BTP_BROWSING_ITEM_TYPE_ATTR_LIST[ArtistName]: ", 
					pItem[index].pValue, pItem[index].valLens);
			}else if(pItem[index].attrID == BTP_BROWSING_ATTID_ALBUM_NAME){
				tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
					"BTP_BROWSING_ITEM_TYPE_ATTR_LIST[AlbumName]: ", 
					pItem[index].pValue, pItem[index].valLens);
			}else if(pItem[index].attrID == BTP_BROWSING_ATTID_TRACK_NUMBER){
				tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
					"BTP_BROWSING_ITEM_TYPE_ATTR_LIST[TrackNumb]: ", 
					pItem[index].pValue, pItem[index].valLens);
			}else if(pItem[index].attrID == BTP_BROWSING_ATTID_TOTAL_TRACKS){
				tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
					"BTP_BROWSING_ITEM_TYPE_ATTR_LIST[TotalTracks]: ", 
					pItem[index].pValue, pItem[index].valLens);
			}else if(pItem[index].attrID == BTP_BROWSING_ATTID_GENRE){
				tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
					"BTP_BROWSING_ITEM_TYPE_ATTR_LIST[Genre]: ", 
					pItem[index].pValue, pItem[index].valLens);
			}else if(pItem[index].attrID == BTP_BROWSING_ATTID_PLAYING_TIME){
				tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
					"BTP_BROWSING_ITEM_TYPE_ATTR_LIST[PlayTime]: ", 
					pItem[index].pValue, pItem[index].valLens);
			}else if(pItem[index].attrID == BTP_BROWSING_ATTID_DEF_COVER_ART){
				tlkapi_array(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
					"BTP_BROWSING_ITEM_TYPE_ATTR_LIST[CoverArt]: ", 
					pItem[index].pValue, pItem[index].valLens);
			}else{
				tlkapi_warn(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
					"BTP_BROWSING_ITEM_TYPE_ATTR_LIST: unknown attrID [%d]", pItem[index].attrID);
			}
		}
	}
	else{
		tlkapi_warn(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_itemsReportCB: Unknown itemType[%d]", itemType);
	}
}
//Status-Refer BTP_AVRCP_STATUS_CODE_ENUM. BTP_AVRCP_STATUS_CODE_OPERATE_WITHOUT_ERROR is success.
static void tlkmdi_btBrowsing_insCompleteCB(uint16 aclHandle, uint08 pduID, uint08 status, void *pParam, uint16 paramLen)
{
	if(status != BTP_AVRCP_STATUS_CODE_OPERATE_WITHOUT_ERROR){
		tlkapi_error(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_insCompleteCB: pduID[0x%02x], status[%d]", pduID, status);
		return;
	}
	if(pduID == BTP_AVRCP_PDUID_SET_BROWSED_PLAYER){
		
	}else if(pduID == BTP_AVRCP_PDUID_CHANGE_PATH){
		btp_browsing_changePathRsp_t *pRsp = (btp_browsing_changePathRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_insCompleteCB-BTP_AVRCP_PDUID_CHANGE_PATH: itemNumb[%d]",
			pRsp->itemNumb);
	}else if(pduID == BTP_AVRCP_PDUID_GET_ITEM_ATTR){
		btp_browsing_getItemAttrRsp_t *pRsp = (btp_browsing_getItemAttrRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_insCompleteCB-BTP_AVRCP_PDUID_GET_ITEM_ATTR: itemNumb[%d]",
			pRsp->itemNumb);
	}else if(pduID == BTP_AVRCP_PDUID_SEARCH){
		btp_browsing_searchRsp_t *pRsp = (btp_browsing_searchRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_insCompleteCB-BTP_AVRCP_PDUID_SEARCH: itemNumb[%d]",
			pRsp->itemNumb);
	}else if(pduID == BTP_AVRCP_PDUID_GET_FOLDER_ITEMS){
		btp_browsing_getFolderItemRsp_t *pRsp = (btp_browsing_getFolderItemRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_insCompleteCB-BTP_AVRCP_PDUID_GET_TOTAL_ITEMS: uidCounter[%d],itemNumb[%d]",
			pRsp->uidCounter, pRsp->itemNumb);
	}else if(pduID == BTP_AVRCP_PDUID_GET_TOTAL_ITEMS){
		btp_browsing_getTotalItemRsp_t *pRsp = (btp_browsing_getTotalItemRsp_t*)pParam;
		tlkapi_trace(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_insCompleteCB-BTP_AVRCP_PDUID_GET_TOTAL_ITEMS: uidCounter[%d],itemNumb[%d]",
			pRsp->uidCounter, pRsp->itemNumb);
	}else if(pduID == BTP_AVRCP_PDUID_GENERAL_REJECT){
		
	}else{
		tlkapi_warn(TLKMDI_BTA2DP_DBG_FLAG, TLKMDI_BTA2DP_DBG_SIGN,
			"tlkmdi_btBrowsing_insCompleteCB: Unknown pduID[%d]", pduID);
	}
}
#endif //TLKBTP_CFG_AVRCP_BROWSING_ENABLE
#endif



#endif //#if (TLK_MDI_BTHFP_ENABLE)

