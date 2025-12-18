/********************************************************************************************************
 * @file	tlkmdi_btacl.c
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
#if (TLK_MDI_BTACL_ENABLE)
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/rfcomm/btp_rfcomm.h"
#include "tlkstk/bt/btp/sdp/btp_sdp.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlkstk/bt/btp/spp/btp_spp.h"
#include "tlkstk/bt/btp/iap/btp_iap.h"
#include "tlkstk/bt/btp/rfcomm/btp_rfcomm.h"
#include "tlkstk/bt/btp/avrcp/btp_avrcp.h"
#include "tlkstk/bt/btp/browsing/btp_browsing.h"
#include "tlkstk/bt/btp/a2dp/btp_a2dp.h"
#include "tlkstk/bt/btp/pbap/btp_pbap.h"
#include "tlkstk/bt/btp/hid/btp_hid.h"
#include "tlkstk/bt/btp/att/btp_att.h"
#include "tlkmdi/bt/tlkmdi_btadapt.h"
#include "tlkmdi/bt/tlkmdi_btacl.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkmdi/bt/tlkmdi_bta2dp.h"
#include "tlkmdi/bt/tlkmdi_bthfp.h"


extern void bth_acl_clearSniffPolicy(uint16 aclHandle);
extern void bth_acl_sendUnSniffReq(uint16 aclHandle);
extern void bth_acl_sendGetNameReq(uint16 handle);

int tlkmdi_btacl_deleteProf(tlkmdi_btacl_item_t *pItem, uint08 ptype, uint08 usrID);

static bool tlkmdi_btacl_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkmdi_btacl_procs(tlkmdi_btacl_item_t *pItem);
static void tlkmdi_btacl_connProfProcs(tlkmdi_btacl_item_t *pItem);
static void tlkmdi_btacl_discProfProcs(tlkmdi_btacl_item_t *pItem);
static bool tlkmdi_btacl_profileConnDeal(tlkmdi_btacl_item_t *pItem, tlkmdi_btacl_prof_t *pProf);

static int tlkmdi_btacl_requestEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btacl_connectEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btacl_disconnEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btacl_encryptEvt(uint08 *pData, uint16 dataLen);

static int tlkmdi_btacl_profileServiceEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btacl_profileChannelEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btacl_profileRequestEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btacl_profileConnectEvt(uint08 *pData, uint16 dataLen);
static int tlkmdi_btacl_profileDisconnEvt(uint08 *pData, uint16 dataLen);


#define TLKMDI_BTACL_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_ACL << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTACL_DBG_SIGN       "[MDI]"


static tlkmdi_btacl_ctrl_t sTlkMdiBtAclCtrl;
static TlkMdiBtAclConnCallback sTlkMdiBtAclConnCB;
static TlkMdiBtAclDiscCallback sTlkMdiBtAclDiscCB;
static TlkMdiBtAclCrypCallback sTlkMdiBtAclCrypCB;
static TlkMdiBtAclProfConnCallback sTlkMdiBtAclProfConnCB;
static TlkMdiBtAclProfDiscCallback sTlkMdiBtAclProfDiscCB;


/******************************************************************************
 * Function: tlkmdi_btacl_init
 * Descript: Initial the ACL control block and register the callback, including
 *           the acl connection callback and some profile connection callback.
 * Params:
 * Return: Return TLK_ENONE is success/other value is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_init(void)
{
	tmemset(&sTlkMdiBtAclCtrl, 0, sizeof(tlkmdi_btacl_ctrl_t));

	bth_event_regCB(BTH_EVTID_ACLCONN_REQUEST,  tlkmdi_btacl_requestEvt);
	bth_event_regCB(BTH_EVTID_ACLCONN_COMPLETE, tlkmdi_btacl_connectEvt);
	bth_event_regCB(BTH_EVTID_ENCRYPT_COMPLETE, tlkmdi_btacl_encryptEvt);
	bth_event_regCB(BTH_EVTID_ACLDISC_COMPLETE, tlkmdi_btacl_disconnEvt);

	btp_event_regCB(BTP_EVTID_PROFILE_SERVICE, tlkmdi_btacl_profileServiceEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CHANNEL, tlkmdi_btacl_profileChannelEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_REQUEST, tlkmdi_btacl_profileRequestEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_CONNECT, tlkmdi_btacl_profileConnectEvt);
	btp_event_regCB(BTP_EVTID_PROFILE_DISCONN, tlkmdi_btacl_profileDisconnEvt);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btacl_isActive
 * Descript: Check the acl status is connected.
 * Params:
 *        @handle[IN]--The ACL handle.
 * Return: Return true is active/false is de-active.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isActive(uint16 handle)
{
	tlkmdi_btacl_item_t *pItem; 

	pItem = tlkmdi_btacl_getConnItem(handle);
	if(pItem == nullptr || pItem->state != TLK_STATE_CONNECT) return false;
	return pItem->active;
}

/******************************************************************************
 * Function: tlkmdi_btacl_isHaveRfc
 * Descript: Check rfcomm whether used or not.
 * Params:
 *        @handle[IN]--The ACL handle.
 * Return: Return true is used/false is unused.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isHaveRfc(uint16 handle)
{
	tlkmdi_btacl_item_t *pItem; 

	pItem = tlkmdi_btacl_getConnItem(handle);
	if(pItem == nullptr || pItem->state != TLK_STATE_CONNECT) return false;
	if(pItem->hfChannel != 0 || pItem->agChannel != 0 || pItem->sppChannel != 0 || pItem->pbapChannel != 0){
		return true;
	}else{
		return false; 
	}
}

/******************************************************************************
 * Function: tlkmdi_btacl_isFindHfp
 * Descript: Check HFP whether used or not.
 * Params:
 *        @handle[IN]--The ACL handle.
 * Return: Return true is used/false is unused.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isFindHfp(uint16 handle)
{
	tlkmdi_btacl_item_t *pItem; 

	pItem = tlkmdi_btacl_getConnItem(handle);
	if(pItem == nullptr || pItem->state != TLK_STATE_CONNECT) return false;
	if(pItem->hfChannel != 0 || pItem->agChannel != 0) return true;
	else return false; 
}

/******************************************************************************
 * Function: tlkmdi_btacl_isFindSpp
 * Descript: Check SPP whether used or not.
 * Params:
 *        @handle[IN]--The ACL handle.
 * Return: Return true is used/false is unused.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isFindSpp(uint16 handle)
{
	tlkmdi_btacl_item_t *pItem; 

	pItem = tlkmdi_btacl_getConnItem(handle);
	if(pItem == nullptr || pItem->state != TLK_STATE_CONNECT) return false;
	if(pItem->sppChannel != 0) return true;
	else return false; 
}

/******************************************************************************
 * Function: tlkmdi_btacl_isFindPbap
 * Descript: Check Pbap whether used or not.
 * Params:
 *        @handle[IN]--The acl handle.
 * Return: Return true is used/false is unused.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btacl_isFindPbap(uint16 handle)
{
	tlkmdi_btacl_item_t *pItem; 

	pItem = tlkmdi_btacl_getConnItem(handle);
	if(pItem == nullptr || pItem->state != TLK_STATE_CONNECT) return false;
	if(pItem->pbapChannel != 0) return true;
	else return false; 
}

/******************************************************************************
 * Function: tlkmdi_btacl_cancel
 * Descript: Cancel the acl link setup procedure.
 * Params:
 *        @pBtAddr[IN]--The bt address.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_cancel(uint08 *pBtAddr)
{
	int ret;
	tlkmdi_btacl_item_t *pItem;

	//Check whether a local device is being connected
	pItem = tlkmdi_btacl_searchConnItem(pBtAddr);
	if(pItem != nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_cancel: failure -- connect");
		return -TLK_EBUSY;
	}
	
	//Check whether there are connecting nodes on the local
	pItem = tlkmdi_btacl_searchUsedItem(pBtAddr);
	if(pItem == nullptr){
		bth_acl_connectCancel(pBtAddr);
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_cancel: failure -- no device");
		return -TLK_EQUOTA;
	}

	//Cancel an ACL connecting
	ret = bth_acl_connectCancel(pBtAddr);
	if(ret != TLK_ENONE){
		if(ret == -TLK_EHANDLE){
			tlkmdi_btacl_resetItem(pItem);
		}
		tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_cancel: failure -- %d", ret);
		return -TLK_EBUSY;
	}

	pItem->busys |= TLKMDI_BTACL_WAIT_CANCEL;
	tlkmdi_btadapt_insertTimer(&pItem->timer);
	tlkapi_array(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_cancel:", pBtAddr, 6);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btacl_connect
 * Descript: Create a connection setup procedure.
 * Params:
 *        @pBtAddr[IN]--The bt address.
 *        @devClass[IN]--The Device type.
 *        @timeout[IN]--The timeout value. Unit:ms, Range:3000~3600000.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_connect(uint08 *pBtAddr, uint32 devClass, uint32 timeout)
{
	int ret;
	tlkmdi_btacl_item_t *pItem;

	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connect: 0x%x 0x%x %d", *(uint32*)pBtAddr, devClass, timeout);
	//Check whether the parameters are valid
	if(pBtAddr == nullptr || devClass == 0){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connect: failure -- param");
		return -TLK_EPARAM;
	}
	
	//Check whether a local device is being connected/connecting
	pItem = tlkmdi_btacl_searchUsedItem(pBtAddr);
	if(pItem != nullptr){
		if(pItem->state == TLK_STATE_CONNECT || pItem->state == TLK_STATE_DISCING){
			tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connect: failure -- connect or discing");
			return -TLK_EEXIST;
		}
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connect: failure -- exist");
		return -TLK_EBUSY;
	}
	
	//Check whether there are any free nodes on the local
	pItem = tlkmdi_btacl_getIdleItem();
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connect: failure -- no quota");
		return -TLK_EBUSY;
	}

	if(timeout < 3000) timeout = 3000;
	else if(timeout > 3600000) timeout = 3600000;
	
	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "p->initRole: initRole-%d", tlkmdi_btacl_getRole(devClass));
	//Initiating an ACL connection
	
	#if (TLK_MDI_AUDDSCO_ENABLE)
		ret = bth_acl_connect(pBtAddr, devClass, BTH_ROLE_MASTER, timeout*TLKMDI_BTACL_TIMEOUT_MS);
	#else 
		ret = bth_acl_connect(pBtAddr, devClass, tlkmdi_btacl_getRole(devClass), timeout);
	#endif
	
	if(ret != TLK_ENONE){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connect: failure -- %d", -ret);
		return -TLK_EBUSY;
	}
	
	//Record the initiation status and start the timeout timer
	pItem->state = TLK_STATE_CONNING;
	pItem->stage = TLKMDI_BTACL_CONNING_STAGE_NONE;
	pItem->timeout = 0;
	pItem->devClass = devClass;
	tmemcpy(pItem->btaddr, pBtAddr, 6);
	tlkmdi_btadapt_initTimer(&pItem->timer, tlkmdi_btacl_timer, (uint32)pItem, TLKMDI_BTACL_TIMEOUT);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btacl_disconn
 * Descript: DisConnect the connection.
 * Params:
 *        @handle[IN]--The ACL handle.
 *        @reaosn[IN]--The disconnect reason.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_disconn(uint16 handle, uint08 reason)
{
	int ret;
	tlkmdi_btacl_item_t *pItem;

	pItem = tlkmdi_btacl_getUsedItem(handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_disconn: failure -- exist  0x%x", handle);
		return -TLK_EHANDLE;
	}
	
	bth_acl_leaveSniff(handle, 60000);
	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_disconn: 0x%x %d", handle, reason);
	if(pItem->state == TLK_STATE_OPENED){
		tlkmdi_btacl_resetItem(pItem);
	}else if(pItem->state == TLK_STATE_CONNING){
		ret = bth_acl_disconn(handle, reason);
		if(ret == TLK_ENONE || ret != -TLK_EBUSY){
			tlkmdi_btacl_resetItem(pItem);
		}else{
			pItem->state = TLK_STATE_DISCING;
			pItem->stage = TLKMDI_BTACL_DISCING_STAGE_WAIT;
		}
	}else if(pItem->state == TLK_STATE_CONNECT){
		btp_sdpclt_disconn(handle);
		btp_sdpsrv_disconn(handle);
		
		pItem->state = TLK_STATE_DISCING;
		pItem->busys = TLKMDI_BTACL_BUSY_DISC_PROF | TLKMDI_BTACL_BUSY_DISC_RFC;
		pItem->timeout = TLKMDI_BTACL_DISC_DEF_TIMEOUT;
		tlkmdi_btadapt_insertTimer(&pItem->timer);
	}else{
		pItem->state = TLK_STATE_DISCING;
	}
	
	if(pItem->state == TLK_STATE_DISCING) return -TLK_EBUSY;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btacl_appendProf
 * Descript: Add a new profile to a acl link.
 * Params:
 *        @handle[IN]--The ACL handle.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id.
 *        @delayMs[IN]--The delay time.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_appendProf(uint16 handle, uint08 ptype, uint08 usrID, uint16 delayMs)
{
	uint08 index;
//	uint32 pflag;
	tlkmdi_btacl_prof_t *pProf;
	tlkmdi_btacl_item_t *pItem; 

	pItem = tlkmdi_btacl_getConnItem(handle);
	if(pItem == nullptr || ptype == 0) return -TLK_EPARAM;
	if(pItem->state == TLK_STATE_DISCING){
		return -TLK_ESTATUS;
	}
	if((pItem->connFlag & (1 << ptype)) != 0){
		return -TLK_EALREADY;
	}
	
	for(index=0; index<TLKMDI_BTACL_PROF_NUMB; index++){
		pProf = &pItem->prof[index];
		if(pProf->state != TLK_STATE_CLOSED && pProf->ptype == ptype && (pProf->usrID == 0 || pProf->usrID == usrID)){
			break;
		}
	}
	if(index != TLKMDI_BTACL_PROF_NUMB) return -TLK_EREPEAT;
	for(index=0; index<TLKMDI_BTACL_PROF_NUMB; index++){
		if(pItem->prof[index].state == TLK_STATE_CLOSED) break;
	}
	if(index == TLKMDI_BTACL_PROF_NUMB) return -TLK_EQUOTA;
	
	pProf = &pItem->prof[index];
	pProf->state = TLK_STATE_OPENED;
	pProf->ptype = ptype;
	pProf->usrID = usrID;
	pProf->cwait = TLKMDI_BTACL_PROFILE_CONN_TIMEOUT;
	pProf->delay = (delayMs+TLKMDI_BTACL_TIMEOUT_MS-1)/TLKMDI_BTACL_TIMEOUT_MS;
		
	pItem->busys |= TLKMDI_BTACL_BUSY_CONN_PROF;
	pItem->timeout = TLKMDI_BTACL_CONN_DEF_TIMEOUT;
	tlkmdi_btadapt_insertTimer(&pItem->timer);
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btacl_removeProf
 * Descript: remove a profile from the acl link.
 * Params:
 *        @handle[IN]--The ACL handle.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_removeProf(uint16 handle, uint08 ptype, uint08 usrID)
{
	tlkmdi_btacl_item_t *pItem; 

	pItem = tlkmdi_btacl_getConnItem(handle);
	if(pItem == nullptr || ptype == 0) return -TLK_EPARAM;
	if(pItem->state == TLK_STATE_DISCING){
		return -TLK_ESTATUS;
	}
	return tlkmdi_btacl_deleteProf(pItem, ptype, usrID);
}

/******************************************************************************
 * Function: tlkmdi_btacl_deleteProf
 * Descript: delete a profile from the acl link.
 * Params:
 *        @handle[IN]--The ACL handle.
 *        @ptype[IN]--The profile type.
 *        @usrID[IN]--The user id.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_deleteProf(tlkmdi_btacl_item_t *pItem, uint08 ptype, uint08 usrID)
{
	uint08 index;
	tlkmdi_btacl_prof_t *pProf;
	for(index=0; index<TLKMDI_BTACL_PROF_NUMB; index++){
		pProf = &pItem->prof[index];
		if(pProf->state != TLK_STATE_CLOSED && pProf->ptype == ptype && (pProf->usrID == 0 || pProf->usrID == usrID)){
			break;
		}
	}
	if(index == TLKMDI_BTACL_PROF_NUMB) return -TLK_ENOOBJECT;
	tmemset(&pItem->prof[index], 0, sizeof(tlkmdi_btacl_prof_t));
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_btacl_getRole
 * Descript: Get the role of acl link.
 * Params:
 *        @devClass[IN]--The device class.
 * Return: Return TLK_ENONE is success/other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btacl_getRole(uint32 devClass)
{
	uint08 devType = bth_devClassToDevType(devClass);
	if(devType == BTH_REMOTE_DTYPE_HEADSET){
		return BTH_ROLE_MASTER;
	}else if((devType == BTH_REMOTE_DTYPE_COMPUTER)||( devType == BTH_REMOTE_DTYPE_PHONE)){
		return BTH_ROLE_SLAVE;
	}else{
		tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_getRole:other - %d", devType);
		return BTH_ROLE_SLAVE;
	}
}

/******************************************************************************
 * Function: tlkmdi_btacl_regConnectCB/tlkmdi_btacl_regDisconnCB
 *           /tlkmdi_btacl_regProfileConnectCB/tlkmdi_btacl_regProfileDisconnCB
 * Descript: Register acl link callback.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btacl_regConnectCB(TlkMdiBtAclConnCallback callback)
{
	sTlkMdiBtAclConnCB = callback;
}
void tlkmdi_btacl_regEncryptCB(TlkMdiBtAclDiscCallback callback)
{
	sTlkMdiBtAclCrypCB = callback;
}
void tlkmdi_btacl_regDisconnCB(TlkMdiBtAclDiscCallback callback)
{
	sTlkMdiBtAclDiscCB = callback;
}
void tlkmdi_btacl_regProfileConnectCB(TlkMdiBtAclProfConnCallback callback)
{
	sTlkMdiBtAclProfConnCB = callback;
}
void tlkmdi_btacl_regProfileDisconnCB(TlkMdiBtAclProfDiscCallback callback)
{
	sTlkMdiBtAclProfDiscCB = callback;
}

static int tlkmdi_btacl_requestEvt(uint08 *pData, uint16 dataLen)
{
	tlkmdi_btacl_item_t *pItem;
	bth_aclConnRequestEvt_t *pEvt;

	pEvt = (bth_aclConnRequestEvt_t*)pData;
	if(tlkmdi_btacl_searchUsedItem(pEvt->peerMac) != nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_requestEvt: failure -- busy");
		return -TLK_EBUSY;
	}

	pItem = tlkmdi_btacl_getIdleItem();
	if(pItem == nullptr){
		tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_requestEvt: failure -- no quota");
		return -TLK_EBUSY;
	}
	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_requestEvt: {devClass - 0x%x}", pEvt->devClass);
	#if (TLK_MDI_AUDDSCO_ENABLE)
		bth_acl_setInitRole(pEvt->peerMac, BTH_ROLE_MASTER);
	#else
		bth_acl_setInitRole(pEvt->peerMac, tlkmdi_btacl_getRole(pEvt->devClass));
	#endif
//	bth_acl_setInitRole(pEvt->peerMac, BTH_ROLE_NOT_SET);
	//Record the initiation status and start the timeout timer
	pItem->state = TLK_STATE_CONNING;
	pItem->stage = TLKMDI_BTACL_CONNING_STAGE_NONE;
	pItem->attrs |= TLKMDI_BTACL_ATTR_REQUEST;
	pItem->timeout = TLKMDI_BTACL_CONN_DEF_TIMEOUT;
	pItem->devClass = pEvt->devClass;
	//
	tmemcpy(pItem->btaddr, pEvt->peerMac, 6);
	tlkmdi_btadapt_initTimer(&pItem->timer, tlkmdi_btacl_timer, (uint32)pItem, TLKMDI_BTACL_TIMEOUT);
	tlkmdi_btadapt_insertTimer(&pItem->timer);
	return TLK_ENONE;
}
static int tlkmdi_btacl_connectEvt(uint08 *pData, uint16 dataLen)
{
	tlkmdi_btacl_item_t *pItem;
	bth_aclConnCompleteEvt_t *pEvt;
	
	pEvt = (bth_aclConnCompleteEvt_t*)pData;
	pItem = tlkmdi_btacl_searchUsedItem(pEvt->peerMac);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connectEvt: failure -- no node");
		if(pEvt->status == TLK_ETIMEOUT){
			tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connectEvt: failure -- connect timeout");
		}
		if(pEvt->status == TLK_ENONE) bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	
	if(pEvt->status != TLK_ENONE){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connectEvt: failure -- %d", pEvt->status);
		tlkmdi_btacl_resetItem(pItem);
		if(sTlkMdiBtAclConnCB){
			sTlkMdiBtAclConnCB(pEvt->handle, pEvt->status, pEvt->peerMac);
		}else{
			tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "sTlkMdiBtAclConnCB: null ");
		}
		return TLK_ENONE;
	}
	
	if(pItem->state == TLK_STATE_DISCING){
		bth_acl_disconn(pItem->handle, 0x00);
		return TLK_ENONE;
	}
	
	tlkapi_array(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_connectEvt", pData, dataLen);
	
	pItem->attrs |= TLKMDI_BTACL_ATTR_CONNECT;
	pItem->handle = pEvt->handle;
	pItem->active = pEvt->active;
	pItem->timeout = TLKMDI_BTACL_CONN_DEF_TIMEOUT;
	tlkmdi_btadapt_insertTimer(&pItem->timer);
	
	pItem->state = TLK_STATE_CONNECT;
	pItem->timeout = TLKMDI_BTACL_CONN_DEF_TIMEOUT;

	pItem->busys &= ~TLKMDI_BTACL_WAIT_CANCEL;
	pItem->busys |= TLKMDI_BTACL_WAIT_DISC_ACL;
	pItem->idleTime = TLKMDI_BTACL_IDLE_DEF_TIMEOUT;
	
	if(sTlkMdiBtAclConnCB){
		sTlkMdiBtAclConnCB(pEvt->handle, TLK_ENONE, pEvt->peerMac);
	}else{
		tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "sTlkMdiBtAclConnCB: null - %d", pEvt->status);
	}
	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "sTlkMdiBtAclConnCB: active %d", pItem->active);
	pEvt->isEncrypt = true; // TODO: There's a problem here in next.
	if(!pEvt->isEncrypt){
		btp_sdpclt_connect(pItem->handle);
	}
		
	return TLK_ENONE;
}

static int tlkmdi_btacl_encryptEvt(uint08 *pData, uint16 dataLen)
{
	tlkmdi_btacl_item_t *pItem;
	bth_encryptCompleteEvt_t *pEvt;
	
	pEvt = (bth_encryptCompleteEvt_t*)pData;
	pItem = tlkmdi_btacl_getUsedItem(pEvt->handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_encryptEvt: failure -- no node");
		bth_acl_disconn(pEvt->handle, 0x00);
		return -TLK_EFAIL;
	}
	if(pEvt->status != TLK_ENONE){
		uint08 btaddr[6];
		tmemcpy(btaddr, pItem->btaddr, 6);
		tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_encryptEvt: failure -- %d", pEvt->status);
		pItem->handle = 0; //Important
		tlkmdi_btacl_resetItem(pItem);
		bth_acl_disconn(pEvt->handle, 0x00);
		if(sTlkMdiBtAclCrypCB){
			sTlkMdiBtAclCrypCB(pEvt->handle, TLK_EENCRYPT, btaddr);
		}
		return TLK_ENONE;
	}
	if(pItem->state == TLK_STATE_DISCING){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_encryptEvt: acl is discing");
		bth_acl_disconn(pItem->handle, 0x00);
		return TLK_ENONE;
	}
	
	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_encryptEvt: status[%d], enable[%d]", 
		pEvt->status, pEvt->enable);

    btp_sdpclt_connect(pItem->handle);
	tlkmdi_btadapt_insertTimer(&pItem->timer);
	
	if(sTlkMdiBtAclCrypCB){
		sTlkMdiBtAclCrypCB(pItem->handle, TLK_ENONE, pItem->btaddr);
	}
	
	return TLK_ENONE;
}
static int tlkmdi_btacl_disconnEvt(uint08 *pData, uint16 dataLen)
{
	tlkmdi_btacl_item_t *pItem;
	bth_aclDiscCompleteEvt_t *pEvt;

	pEvt = (bth_aclDiscCompleteEvt_t*)pData;
	pItem = tlkmdi_btacl_searchUsedItem(pEvt->peerMac);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_disconnEvt: failure -- no node");
		return -TLK_EFAIL;
	}
	
	if(pItem->profIsConn && (pItem->connFlag & (~(BTP_PFLAG_SDP | BTP_PFLAG_RFC))) == 0
		&& pEvt->reason == 0x08){
		pEvt->reason = 0x13;
	}
	
	btp_destroy(pItem->handle);
	bth_destroy(pItem->handle);

	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_disconnEvt: success - 0x%x", pEvt->handle);
	
	pItem->handle = 0; //Important
	tlkmdi_btacl_resetItem(pItem);
	if(sTlkMdiBtAclDiscCB){
		tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_disconnEvt ");
		sTlkMdiBtAclDiscCB(pEvt->handle, pEvt->reason, pEvt->peerMac);
	}else{
		tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "sTlkMdiBtAclDiscCB: null ");
	}

	return TLK_ENONE;
}
static int tlkmdi_btacl_profileServiceEvt(uint08 *pData, uint16 dataLen)
{
	//Not Support.
	return TLK_ENONE;
}
static int tlkmdi_btacl_profileChannelEvt(uint08 *pData, uint16 dataLen)
{
	btp_channelEvt_t *pEvt;
	tlkmdi_btacl_item_t *pItem;

	pEvt = (btp_channelEvt_t*)pData;
	pItem = tlkmdi_btacl_getUsedItem(pEvt->handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileChannelEvt: error - no node");
		return TLK_ENONE;
	}
	
	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileChannelEvt:{handle-0x%x,service-%d,channel-%d}",
		pEvt->handle, pEvt->service, pEvt->channel);
	
	if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE){
		pItem->hfChannel = pEvt->channel;
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_HANDSFREE_AGW){
		pItem->agChannel = pEvt->channel;
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_SERIAL_PORT){
		pItem->sppChannel = pEvt->channel;
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_PBAP_PSE){
		pItem->pbapChannel = pEvt->channel;
	}else if(pEvt->service == BTP_SDP_SRVCLASS_ID_IAP2_TEMP){
		pItem->iapChannel = pEvt->channel;
	}
	
	return TLK_ENONE;
}
static int tlkmdi_btacl_profileRequestEvt(uint08 *pData, uint16 dataLen)
{
	btp_requestEvt_t *pEvt;
	tlkmdi_btacl_item_t *pItem;
	
	pEvt = (btp_requestEvt_t*)pData;
	pItem = tlkmdi_btacl_getUsedItem(pEvt->handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileRequestEvt: error - no node");
		return -TLK_EFAIL;
	}
	if(pItem->state == TLK_STATE_DISCING){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileRequestEvt: reject - discing");
		return -TLK_EFAIL;
	}
	
	return TLK_ENONE;
}
static int tlkmdi_btacl_profileConnectEvt(uint08 *pData, uint16 dataLen)
{
	btp_connectEvt_t *pEvt;
	tlkmdi_btacl_item_t *pItem;
	
	pEvt = (btp_connectEvt_t*)pData;
	pItem = tlkmdi_btacl_getUsedItem(pEvt->handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileConnectEvt: error - no node");
		return TLK_ENONE;
	}
	if(pItem->state == TLK_STATE_DISCING){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileConnectEvt: reject - discing");
		btp_module_disconn(pItem->handle, pEvt->ptype, pEvt->usrID);
		return TLK_ENONE;
	}
	
	tlkmdi_btacl_deleteProf(pItem, pEvt->ptype, pEvt->usrID);
	if(pEvt->status != TLK_ENONE){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileConnectEvt: fail - ptype-%d,usrID-%d,handle-0x%x,status-%d ", 
			pEvt->ptype, pEvt->usrID, pEvt->handle, pEvt->status);
//		if(pEvt->status == TLK_ETIMEOUT){
//			tlkmdi_btacl_appendProf(pItem->handle, pEvt->ptype, pEvt->usrID, 500);
//		}
	}else{
		tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileConnectEvt: {ptype-%d,usrID-%d,handle-0x%x,status-%d} - ", 
			pEvt->ptype, pEvt->usrID, pEvt->handle, pEvt->status);
		pItem->connFlag |= (1 << pEvt->ptype);
		if(pEvt->ptype != BTP_PTYPE_SDP && pEvt->ptype != BTP_PTYPE_RFC){
			pItem->busys &= ~TLKMDI_BTACL_WAIT_DISC_ACL;
			if(!pItem->profIsConn) pItem->profIsConn = true;
		}
		if(pEvt->ptype == BTP_PTYPE_A2DP){
			#if (TLK_MDI_BTA2DP_ENABLE)
			tlkmdi_bta2dp_connectEvt(pEvt->handle, pEvt->usrID);
			#endif
		}
		#if (TLKBTP_CFG_AVRCP_BROWSING_ENABLE)
		if(pEvt->ptype == BTP_PTYPE_AVRCP_BROWSING){
			btp_browsing_sendGetFolderItemsCmd(pEvt->handle, 0x00, 0, 100, 0xFF, nullptr);
		}
		#endif
	}
	if(sTlkMdiBtAclProfConnCB != nullptr){
		sTlkMdiBtAclProfConnCB(pEvt->handle, pEvt->status, pEvt->ptype, pEvt->usrID, pItem->btaddr);
	}
	return TLK_ENONE;
}
static int tlkmdi_btacl_profileDisconnEvt(uint08 *pData, uint16 dataLen)
{
	uint16 connFlag;
	btp_disconnEvt_t *pEvt;
	tlkmdi_btacl_item_t *pItem;

	pEvt = (btp_disconnEvt_t*)pData;
	pItem = tlkmdi_btacl_getUsedItem(pEvt->handle);
	if(pItem == nullptr){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileDisconnEvt: error - no node");
		return TLK_ENONE;
	}
	
	tlkmdi_btacl_deleteProf(pItem, pEvt->ptype, pEvt->usrID);
	pItem->connFlag &= ~(1 << pEvt->ptype);
	if(pItem->state == TLK_STATE_DISCING){
		if((pItem->connFlag & BTP_PFLAG_RFC) != 0 && (pItem->connFlag & BTP_PFLAG_RFCOMM_MASK) == 0){
			btp_rfcomm_disconn(pItem->handle);
		}
	}

	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileDisconnEvt: {ptype-%d,usrID-%d,handle-0x%x,connFlag-0x%x}", 
		pEvt->ptype, pEvt->usrID, pEvt->handle, pItem->connFlag);
	if(sTlkMdiBtAclProfDiscCB != nullptr){
		sTlkMdiBtAclProfDiscCB(pItem->handle, 0, pEvt->ptype, pEvt->usrID, pItem->btaddr);
	}
	
	connFlag = pItem->connFlag;
	connFlag &= ~BTP_PFLAG_RFC;
	connFlag &= ~BTP_PFLAG_SDP;
	if(connFlag == 0){
		if(pItem->state == TLK_STATE_DISCING){
			pItem->busys |= TLKMDI_BTACL_BUSY_DISC_ACL;
		}
		pItem->busys |= TLKMDI_BTACL_WAIT_DISC_ACL;
		if(pEvt->ptype != BTP_PTYPE_SDP && pEvt->ptype != BTP_PTYPE_RFC){
			pItem->idleTime = TLKMDI_BTACL_IDLE_DEF_TIMEOUT1;
		}else{
			pItem->idleTime = TLKMDI_BTACL_IDLE_DEF_TIMEOUT;
		}
		tlkmdi_btadapt_insertTimer(&pItem->timer);
	}
	
	return TLK_ENONE;
}



static bool tlkmdi_btacl_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	tlkmdi_btacl_item_t *pItem;

	pItem = (tlkmdi_btacl_item_t*)userArg;
	tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_timer: %d %x", pItem->timeout, pItem->busys);
	
	if(pItem->busys != 0) tlkmdi_btacl_procs(pItem);
	if(pItem->busys == 0 && pItem->timeout != 0) pItem->timeout = 0;
	if(pItem->timeout != 0 || pItem->busys != 0) return true;
	return false;
}
static void tlkmdi_btacl_procs(tlkmdi_btacl_item_t *pItem)
{
	if((pItem->busys & TLKMDI_BTACL_BUSY_CONN_PROF) != 0){
		pItem->busys &= ~TLKMDI_BTACL_BUSY_CONN_PROF;
		tlkmdi_btacl_connProfProcs(pItem);
	}else if((pItem->busys & TLKMDI_BTACL_BUSY_DISC_PROF) != 0){
		pItem->busys &= ~TLKMDI_BTACL_BUSY_DISC_PROF;
		tlkmdi_btacl_discProfProcs(pItem);
	}else if((pItem->busys & TLKMDI_BTACL_WAIT_DISC_PROF) != 0){
		if(pItem->timeout != 0) pItem->timeout--;
		if(pItem->timeout == 0){
			pItem->busys &= ~TLKMDI_BTACL_WAIT_DISC_PROF;
			if(pItem->state == TLK_STATE_DISCING) pItem->busys |= TLKMDI_BTACL_BUSY_DISC_ACL;
		}
	}else if((pItem->busys & TLKMDI_BTACL_BUSY_DISC_RFC) != 0){
		pItem->busys &= ~TLKMDI_BTACL_BUSY_DISC_RFC;
		if(btp_rfcomm_disconn(pItem->handle) == -TLK_EBUSY){
			pItem->busys |= TLKMDI_BTACL_WAIT_DISC_RFC;
			pItem->timeout = TLKMDI_BTACL_PROFILE_DISC_TIMEOUT;
		}else if(pItem->state == TLK_STATE_DISCING){
			pItem->busys |= TLKMDI_BTACL_BUSY_DISC_ACL;
		}
	}else if((pItem->busys & TLKMDI_BTACL_WAIT_DISC_RFC) != 0){
		if(pItem->timeout != 0) pItem->timeout--;
		if(pItem->timeout == 0){
			pItem->busys &= ~TLKMDI_BTACL_WAIT_DISC_RFC;
			if(pItem->state == TLK_STATE_DISCING) pItem->busys |= TLKMDI_BTACL_BUSY_DISC_ACL;
		}
	}

	if((pItem->busys & TLKMDI_BTACL_WAIT_DISC_ACL) != 0){
		if(pItem->idleTime != 0) pItem->idleTime --;
		if(pItem->idleTime == 0){
			pItem->busys &= ~TLKMDI_BTACL_WAIT_DISC_ACL;
			pItem->busys |= TLKMDI_BTACL_BUSY_DISC_ACL;
			tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_procs: idle timeout");
		}
	}
	if((pItem->busys & TLKMDI_BTACL_BUSY_DISC_ACL) != 0){
		pItem->busys &= ~TLKMDI_BTACL_BUSY_DISC_ACL;
		if(bth_acl_disconn(pItem->handle, 0x00) != -TLK_EBUSY){
			btp_destroy(pItem->handle);
			bth_destroy(pItem->handle);
			tlkmdi_btacl_resetItem(pItem);
		}
	}
	if((pItem->busys & TLKMDI_BTACL_WAIT_CANCEL) != 0){
		if(pItem->timeout != 0) pItem->timeout--;
		if(pItem->timeout == 0){
			pItem->busys &= ~TLKMDI_BTACL_WAIT_CANCEL;
			tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "TLKMDI_BTACL_WAIT_CANCEL: timeout");
			tlkmdi_btacl_resetItem(pItem);
		}
	}
}
static void tlkmdi_btacl_connProfProcs(tlkmdi_btacl_item_t *pItem)
{
	bool busy;
	uint08 index;
	busy = false;
	for(index=0; index<TLKMDI_BTACL_PROF_NUMB; index++){
		if(tlkmdi_btacl_profileConnDeal(pItem, &pItem->prof[index]) && !busy){
			busy = true;
		}
	}
	if(busy) pItem->busys |= TLKMDI_BTACL_BUSY_CONN_PROF;
}
static void tlkmdi_btacl_discProfProcs(tlkmdi_btacl_item_t *pItem)
{
	bool busy;
	uint08 index;
	uint16 connFlag;

	busy = false;
	connFlag = pItem->connFlag;
	connFlag &= ~BTP_PFLAG_SDP;
	connFlag &= ~BTP_PFLAG_RFC;
	if(connFlag == 0) return;
	for(index=0; index<16; index++){
		if((connFlag & (1 << index)) != 0 && btp_module_disconn(pItem->handle, index, 0) != -TLK_EBUSY){
			busy = true;
		}
	}
	if(busy){
		pItem->busys |= TLKMDI_BTACL_WAIT_DISC_PROF;
		pItem->timeout = TLKMDI_BTACL_PROFILE_DISC_TIMEOUT;
	}else if(pItem->state == TLK_STATE_DISCING){
		pItem->busys |= TLKMDI_BTACL_BUSY_DISC_ACL;
	}
}

//Return is or not Busy
static bool tlkmdi_btacl_profileConnDeal(tlkmdi_btacl_item_t *pItem, tlkmdi_btacl_prof_t *pProf)
{
	int ret;
	uint08 dtype;

	if(pProf->state == TLK_STATE_CLOSED) return false;

	if((pItem->connFlag & (1 << pProf->ptype)) != 0){
		tmemset(pProf, 0, sizeof(tlkmdi_btacl_prof_t));
		return false;
	}
	
	if(pProf->delay != 0) pProf->delay --;
	if(pProf->delay != 0) return true;

	if(pProf->cwait != 0) pProf->cwait --;
	if(pProf->cwait == 0){
		tlkapi_warn(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileConnDeal: wait timeout ptype-%d usrID-%d", pProf->ptype, pProf->usrID);
		tmemset(pProf, 0, sizeof(tlkmdi_btacl_prof_t));
		return false;
	}
	if(pProf->state == TLK_STATE_CONNING) return true;

	ret = -TLK_EFAIL;
	if(pProf->ptype == BTP_PTYPE_RFC){
		#if (TLKBTP_CFG_RFC_ENABLE)
		ret = btp_rfcomm_connect(pItem->handle);
		#endif
	}else if(pProf->ptype == BTP_PTYPE_A2DP){
		#if (TLKBTP_CFG_A2DPSRC_ENABLE)
		ret = btp_a2dp_connect(pItem->handle, pProf->usrID);
		#else
		ret = -TLK_ENOSUPPORT;
		#endif
	}else if(pProf->ptype == BTP_PTYPE_HFP){
		#if (TLKBTP_CFG_HFP_ENABLE)
//		tlkapi_trace(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileConnDeal: hfp Channel-%d connFlag-%d usrId-%d handle-%d",
//			pItem->hfpChannel, pItem->connFlag, pProf->usrID, pItem->handle);
		if((pItem->connFlag & BTP_PFLAG_RFC) == 0) return true;
		#if (!TLK_MDI_AUDAG_ENABLE)
		if(pItem->agChannel != 0) ret = btp_hfp_connect(pItem->handle, BTP_USRID_CLIENT, pItem->agChannel);
		else if(pItem->hfChannel != 0) ret = btp_hfp_connect(pItem->handle, BTP_USRID_SERVER, pItem->hfChannel);
		#else
		dtype = bth_devClassToDevType(pItem->devClass);	// get peer devClass
		if(dtype == BTH_REMOTE_DTYPE_HEADSET){
			if(pItem->hfChannel != 0) ret = btp_hfp_connect(pItem->handle, BTP_USRID_SERVER, pItem->hfChannel);
		}else if(dtype == BTH_REMOTE_DTYPE_COMPUTER || dtype == BTH_REMOTE_DTYPE_PHONE){
			if(pItem->agChannel != 0) ret = btp_hfp_connect(pItem->handle, BTP_USRID_CLIENT, pItem->agChannel);
		}else{
			if(pItem->agChannel != 0) ret = btp_hfp_connect(pItem->handle, BTP_USRID_CLIENT, pItem->agChannel);
			else if(pItem->hfChannel != 0) ret = btp_hfp_connect(pItem->handle, BTP_USRID_SERVER, pItem->hfChannel);
		}
		#endif // TLK_MDI_AUDAG_ENABLE

		#endif // TLKBTP_CFG_HFP_ENABLE
	}else if(pProf->ptype == BTP_PTYPE_IAP){
		#if (TLKBTP_CFG_IAP_ENABLE)
		if(pItem->iapChannel == 0 || (pItem->connFlag & BTP_PFLAG_RFC) == 0) return true;
		ret = btp_iap_connect(pItem->handle, pItem->iapChannel);
		#endif
	}else if(pProf->ptype == BTP_PTYPE_SPP){
		#if (TLKBTP_CFG_SPP_ENABLE)
		if(pItem->sppChannel == 0 || (pItem->connFlag & BTP_PFLAG_RFC) == 0) return true;
		ret = btp_spp_connect(pItem->handle, pItem->sppChannel);
		#endif
	}else if(pProf->ptype == BTP_PTYPE_HID && (pItem->devClass != BTH_REMOTE_DTYPE_HEADSET)){
		#if (TLKBTP_CFG_HID_ENABLE)
		ret = btp_hid_connect(pItem->handle, pProf->usrID);
		#endif
	}else if(pProf->ptype == BTP_PTYPE_ATT){
		#if (TLKBTP_CFG_ATT_ENABLE)
		ret = btp_att_connect(pItem->handle, pProf->usrID);
		#endif
	}else if(pProf->ptype == BTP_PTYPE_PBAP){
		#if (TLKBTP_CFG_PBAP_ENABLE)
		if(pItem->pbapChannel == 0 || (pItem->connFlag & BTP_PFLAG_RFC) == 0) return true;
		ret = btp_pbap_connect(pItem->handle, pProf->usrID, pItem->pbapChannel, false);
		#endif
	}else if(pProf->ptype == BTP_PTYPE_AVRCP){
		#if (TLKBTP_CFG_AVRCP_ENABLE)
		ret = btp_avrcp_connect(pItem->handle, pProf->usrID);
		#endif
	}
	
	if(ret != TLK_ENONE && ret != -TLK_EBUSY && ret != -TLK_EEXIST){
		tlkapi_error(TLKMDI_BTACL_DBG_FLAG, TLKMDI_BTACL_DBG_SIGN, "tlkmdi_btacl_profileConnDeal: connect fail ptype-%d usrID-%d", pProf->ptype, pProf->usrID);
		tmemset(pProf, 0, sizeof(tlkmdi_btacl_prof_t));
		return false;
	}
	pProf->state = TLK_STATE_CONNING;
	return true;
}


/******************************************************************************
 * Function: tlkmdi_btacl_getIdleCount
 * Descript: Get the idle acl link count.
 * Params: None.
 * Return: The count of idle acl link.
 * Others: None.
*******************************************************************************/
uint08 tlkmdi_btacl_getIdleCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state == TLK_STATE_CLOSED){
			count ++;
		}
	}
	return count;
}

/******************************************************************************
 * Function: tlkmdi_btacl_getUsedCount
 * Descript: Get the used acl link count.
 * Params: None.
 * Return: The count of used acl link.
 * Others: None.
*******************************************************************************/
uint08 tlkmdi_btacl_getUsedCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state != TLK_STATE_CLOSED){
			count ++;
		}
	}
	return count;
}

/******************************************************************************
 * Function: tlkmdi_btacl_getConnCount
 * Descript: Get the connected acl link count.
 * Params: None.
 * Return: The count of connected acl link.
 * Others: None.
*******************************************************************************/
uint08 tlkmdi_btacl_getConnCount(void)
{
	uint08 index;
	uint08 count = 0;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state == TLK_STATE_CONNECT){
			count ++;
		}
	}
	return count;
}

/******************************************************************************
 * Function: tlkmdi_btacl_resetItem
 * Descript: Reset the acl control block.
 * Params: @pItem[IN]--The acl control block.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btacl_resetItem(tlkmdi_btacl_item_t *pItem)
{
	//uint08 state;
	uint16 handle;
	uint08 btaddr[6];
	
	if(pItem == nullptr) return;

	//state  = pItem->state;
	handle = pItem->handle;
	tmemcpy(btaddr, pItem->btaddr, 6);
	tlkmdi_btadapt_removeTimer(&pItem->timer);
	tmemset(pItem, 0, sizeof(tlkmdi_btacl_item_t));
	if(handle != 0 && sTlkMdiBtAclDiscCB != nullptr){
		sTlkMdiBtAclDiscCB(handle, 0, btaddr);
	}
}

/******************************************************************************
 * Function: tlkmdi_btacl_getIdleItem
 * Descript: Get a idle acl control block.
 * Params: None.
 * Return: The idle acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getIdleItem(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state == TLK_STATE_CLOSED) break;
	}
	if(index == TLKMDI_BTACL_ITEM_NUMB) return nullptr;
	return &sTlkMdiBtAclCtrl.item[index];
}

/******************************************************************************
 * Function: tlkmdi_btacl_getInitItem
 * Descript: Get a init acl control block.
 * Params: None.
 * Return: The init acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getInitItem(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state == TLK_STATE_OPENED) break;
	}
	if(index == TLKMDI_BTACL_ITEM_NUMB) return nullptr;
	return &sTlkMdiBtAclCtrl.item[index];
}

/******************************************************************************
 * Function: tlkmdi_btacl_getBusyItem
 * Descript: Get a Connecting acl control block.
 * Params: None.
 * Return: The connecting acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getBusyItem(void)
{
	uint08 index;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state != TLK_STATE_CLOSED
			&& sTlkMdiBtAclCtrl.item[index].state != TLK_STATE_CONNECT
			&& sTlkMdiBtAclCtrl.item[index].state != TLK_STATE_DISCING) break;
	}
	if(index == TLKMDI_BTACL_ITEM_NUMB) return nullptr;
	return &sTlkMdiBtAclCtrl.item[index];
}

/******************************************************************************
 * Function: tlkmdi_btacl_getUsedItem
 * Descript: Get a Used acl control block.
 * Params: None.
 * Return: The used acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getUsedItem(uint16 handle)
{
	uint08 index;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state != TLK_STATE_CLOSED
			&& sTlkMdiBtAclCtrl.item[index].handle == handle) break;
	}
	if(index == TLKMDI_BTACL_ITEM_NUMB) return nullptr;
	return &sTlkMdiBtAclCtrl.item[index];
}

/******************************************************************************
 * Function: tlkmdi_btacl_getConnItem
 * Descript: Get a Connected acl control block.
 * Params: None.
 * Return: The Connected acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getConnItem(uint16 handle)
{
	uint08 index;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state == TLK_STATE_CONNECT
			&& sTlkMdiBtAclCtrl.item[index].handle == handle) break;
	}
	if(index == TLKMDI_BTACL_ITEM_NUMB) return nullptr;
	return &sTlkMdiBtAclCtrl.item[index];
}

/******************************************************************************
 * Function: tlkmdi_btacl_getConnItemByIndex
 * Descript: Get a Connected acl control block.
 * Params: @Index[IN]--The index of connected acl control block.
 * Return: The Connected acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_getConnItemByIndex(uint08 index)
{
	uint08 i;
	uint08 count = 0;
	for(i=0; i<TLKMDI_BTACL_ITEM_NUMB; i++){
		if(sTlkMdiBtAclCtrl.item[i].state == TLK_STATE_CONNECT){
			if(count == index) break;
			count ++;
		}
	}
	if(i == TLKMDI_BTACL_ITEM_NUMB) return nullptr;
	return &sTlkMdiBtAclCtrl.item[i];
}

/******************************************************************************
 * Function: tlkmdi_btacl_searchUsedItem
 * Descript: Find a Used acl control block.
 * Params: @pBtAddr[IN]--The Bt address of acl.
 * Return: The Used acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_searchUsedItem(uint08 *pBtAddr)
{
	uint08 index;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state != TLK_STATE_CLOSED
			&& tmemcmp(sTlkMdiBtAclCtrl.item[index].btaddr, pBtAddr, 6) == 0) break;
	}
	if(index == TLKMDI_BTACL_ITEM_NUMB) return nullptr;
	return &sTlkMdiBtAclCtrl.item[index];
}

/******************************************************************************
 * Function: tlkmdi_btacl_searchUsedItem
 * Descript: Find a Connected acl control block.
 * Params: @pBtAddr[IN]--The Bt address of acl.
 * Return: The Connected acl control block or NULL.
 * Others: None.
*******************************************************************************/
tlkmdi_btacl_item_t *tlkmdi_btacl_searchConnItem(uint08 *pBtAddr)
{
	uint08 index;
	for(index=0; index<TLKMDI_BTACL_ITEM_NUMB; index++){
		if(sTlkMdiBtAclCtrl.item[index].state == TLK_STATE_CONNECT
			&& tmemcmp(sTlkMdiBtAclCtrl.item[index].btaddr, pBtAddr, 6) == 0) break;
	}
	if(index == TLKMDI_BTACL_ITEM_NUMB) return nullptr;
	return &sTlkMdiBtAclCtrl.item[index];
}



#endif //#if (TLK_MDI_BTACL_ENABLE)

