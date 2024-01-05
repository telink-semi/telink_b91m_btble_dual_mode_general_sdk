/********************************************************************************************************
 * @file	btp_ptsL2c.c
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
#include "../../bth/bth_stdio.h"
#include "../btp_stdio.h"
#if (TLKBTP_CFG_PTSL2C_ENABLE)
#include "../../bth/bth_l2cap.h"
#include "../../bth/bth_signal.h"
#include "../btp_adapt.h"
#include "btp_ptsL2c.h"

static void btp_ptsl2c_resetNode(btp_ptsl2c_item_t *pPtsL2c);
static bool btp_ptsl2c_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void btp_ptsl2c_procs(btp_ptsl2c_item_t *pPtsL2c);

static int btp_ptsl2c_eventCB(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen);
static void btp_ptsl2c_rdataCB(uint16 psmID, uint16 chnID, uint16 handle, uint08 *pData, uint16 dataLen);

static uint08 sBtPtsL2capNumb = 0;
static btp_ptsl2c_ctrl_t *spBtPtsL2capCtrl = nullptr;

/******************************************************************************
 * Function: btp_ptsl2c_init
 * Descript: This interface be used by user to initial the ptsl2c resource
 *           of client/server before create a connection between the entity.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_ptsl2c_init(uint08 count)
{
	int ret;
    uint16 index;
    uint32 mallocSize;

	sBtPtsL2capNumb  = 0;
	spBtPtsL2capCtrl = nullptr;
	if (count == 0) return TLK_ENONE;
	
	mallocSize = btp_ptsl2c_getMemLen(count);
	spBtPtsL2capCtrl = (btp_ptsl2c_ctrl_t *)tlkstk_calloc(mallocSize);
	if (spBtPtsL2capCtrl == nullptr) {
	    tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_init: - malloc failure");
	    return -TLK_ENOMEM;
	}
	
	sBtPtsL2capNumb = count;
	spBtPtsL2capCtrl->item = (btp_ptsl2c_item_t*)((uint08*)(spBtPtsL2capCtrl+1));
	spBtPtsL2capCtrl->user = (btp_ptsl2c_user_t*)(((uint08*)spBtPtsL2capCtrl->item) + TLKSTK_BT_STRUCT_ALIGN_4(btp_ptsl2c_item_t) * sBtPtsL2capNumb);
    for (index = 0; index < BTP_PTSL2C_BUFFER_LENGTH; index++) {
        spBtPtsL2capCtrl->buffer[index] = index + 1;
    }

    ret = bth_l2cap_regServiceCB(BTP_PSMID_PTS_L2C, btp_ptsl2c_eventCB, btp_ptsl2c_rdataCB);
	if (ret != TLK_ENONE) {
		btp_ptsl2c_deinit();
	}

	return ret;
}

/******************************************************************************
 * Function: ptsl2c Deinit interface
 * Descript: This interface be used by User to deinitial ptsl2c resource.
 * Params: pPtsL2c: refer to btp_ptsl2c_item_t
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *******************************************************************************/
int btp_ptsl2c_deinit(void)
{
	uint08 index;

	bth_l2cap_regServiceCB(BTP_PSMID_PTS_L2C, nullptr, nullptr);

	if (spBtPtsL2capCtrl != nullptr) {
		for (index = 0; index < sBtPtsL2capNumb; index++) {
			btp_ptsl2c_resetNode(&spBtPtsL2capCtrl->item[index]);
			tmemset(&spBtPtsL2capCtrl->user[index], 0, sizeof(btp_ptsl2c_user_t));
		}
		tlkstk_free(spBtPtsL2capCtrl);
		spBtPtsL2capCtrl = nullptr;
		sBtPtsL2capNumb = 0;
	}
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: btp_ptsl2c_getMemLen
 * Descript: ptsl2c get the size of the required number of nodes interface.
 * Params:
 * Return: Actual node memory size required(unit: byte).
 *******************************************************************************/
int btp_ptsl2c_getMemLen(uint08 count)
{
	if (count == 0) return 0;
	return TLKSTK_BT_STRUCT_ALIGN_4(btp_ptsl2c_ctrl_t) +\
		   TLKSTK_BT_STRUCT_ALIGN_4(btp_ptsl2c_item_t) * count +\
		   TLKSTK_BT_STRUCT_ALIGN_4(btp_ptsl2c_user_t) * count;
}

void btp_ptsl2c_enableRtn(bool isEnable, uint08 rtnMode)
{
    spBtPtsL2capCtrl->user->enRtn   = isEnable;
    spBtPtsL2capCtrl->user->rtnMode = rtnMode;
    if (isEnable) {
        bth_l2cap_setExtFeatureBits(BTH_L2CAP_EXT_FEATURE_ENHANCED_RTN_MODE | BTH_L2CAP_EXT_FEATURE_FCS_OPTION);
        if (rtnMode == BTH_L2CAP_SIG_RTNMODE_STREAMING) {
            bth_l2cap_setExtFeatureBits(BTH_L2CAP_EXT_FEATURE_STREAMING_MODE);
        }
    }
}
void btp_ptsl2c_enableFcs(bool isEnable, uint08 fcsType)
{
    spBtPtsL2capCtrl->user->enFcs   = isEnable;
    spBtPtsL2capCtrl->user->fcsType = fcsType;
}
void btp_ptsl2c_enableEfs(bool isEnable)
{
    spBtPtsL2capCtrl->user->enEfs = isEnable;
}

/******************************************************************************
 * Function: btp_ptsl2c_connect
 * Descript: Defines trigger the ptsl2c connect cmd to peer ptsl2c entity and setup
 *           a connection with devices.
 * Params:
 *     @handle--Connection handle of the current ACL.
 *     @usrId--The service user id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_ptsl2c_connect(uint16 handle)
{
    int ret;
    btp_ptsl2c_item_t *pPtsL2c;

    pPtsL2c = spBtPtsL2capCtrl->item;
    if (pPtsL2c->state != TLK_STATE_CLOSED) {
        tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_connect: %d failure - exist", handle);
        return -TLK_EREPEAT;
    }
    if (bth_l2cap_getUsedChannelByPsm(handle, BTP_PSMID_PTS_L2C) != nullptr) {
        tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_connect: %d failure - busying", handle);
        return -TLK_EBUSY;
    }

    tlkapi_trace(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_connect: %d", handle);

    ret = bth_signal_createConnect(handle, BTP_PSMID_PTS_L2C, BTP_USRID_IGNORE, nullptr);
    if (ret != TLK_ENONE && ret != -TLK_EBUSY) {
        tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_connect: failure - signal reject [%d]", ret);
    } else {
        bth_l2cap_channel_t *pChannel;
        btp_adapt_initTimer(&pPtsL2c->timer, btp_ptsl2c_timer, (uint32)pPtsL2c, BTP_TIMER_TIMEOUT);
        pPtsL2c->state   = TLK_STATE_CONNING;
        pPtsL2c->timeout = gBtpConnTimeout;
        pPtsL2c->handle  = handle;
        btp_adapt_insertTimer(&pPtsL2c->timer);
        tlkapi_info(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_connect: start");
        pChannel = bth_l2cap_getUsedChannelByUser(handle, BTP_PSMID_PTS_L2C, BTP_USRID_IGNORE);
        if (pChannel != nullptr) {
            if (spBtPtsL2capCtrl->user->enRtn) {
                bth_signal_enableRtnOption(handle, pChannel->scid, true);
                bth_signal_setRtnMode(handle, pChannel->scid, spBtPtsL2capCtrl->user->rtnMode);
            }
            if (spBtPtsL2capCtrl->user->enFcs) {
                bth_signal_enableFcsOption(handle, pChannel->scid, true);
                bth_signal_setFcsType(handle, pChannel->scid, spBtPtsL2capCtrl->user->fcsType);
            }
            if (spBtPtsL2capCtrl->user->enEfs) {
                bth_signal_enableEfsOption(handle, pChannel->scid, true);
            }
        }
    }
    return ret;
}

/******************************************************************************
 * Function: btp_ptsl2c_disconn
 * Descript: Defines trigger the ptsl2c disconnect cmd to peer ptsl2c entity and
 *           tear up a connection which specify by aclhandle and usrid.
 * Params:
 *     @handle--Connection handle of the current ACL.
 *     @usrId--The service user id.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_ptsl2c_disconn(uint16 handle)
{
    int ret;
    btp_ptsl2c_item_t *pPtsL2c;

    pPtsL2c = spBtPtsL2capCtrl->item;
    if (pPtsL2c->state == TLK_STATE_CLOSED) {
        bth_signal_disconnChannelByUsrID(handle, BTP_PSMID_PTS_L2C, 0);
        tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_disconn: pPtsL2c is null");
        return -TLK_EPARAM;
    }

    tlkapi_trace(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_disconn: chnID[0x%x]", pPtsL2c->chnID);

    if (pPtsL2c->chnID == 0) {
        ret = TLK_ENONE;
        bth_signal_disconnChannelByUsrID(handle, BTP_PSMID_PTS_L2C, 0);
        btp_ptsl2c_resetNode(pPtsL2c);
        btp_send_connectEvt(TLK_ETIMEOUT, BTP_PTYPE_PTS_L2C, BTP_USRID_NONE, handle, 0);
    } else {
        uint16 chnID = pPtsL2c->chnID;
        ret          = bth_signal_disconnChannel(handle, chnID);
        if (ret != -TLK_EBUSY) {
            ret = TLK_ENONE;
            btp_ptsl2c_resetNode(pPtsL2c);
            btp_send_disconnEvt(BTP_PTYPE_PTS_L2C, BTP_USRID_NONE, handle);
            tlkapi_info(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_disconn: success");
        } else {
            pPtsL2c->state   = TLK_STATE_DISCING;
            pPtsL2c->timeout = gBtpDiscTimeout;
            btp_adapt_insertTimer(&pPtsL2c->timer);
        }
    }
    return ret;
}

/******************************************************************************
 * Function: btp_ptsl2c_destroy
 * Descript: Defines trigger to release the resource which allocated.
 * Params:
 *     @handle--Connection handle of the current ACL.
 * Return: null
 *******************************************************************************/
void btp_ptsl2c_destroy(uint16 handle)
{
    uint08 state;
    uint16 chnID;
    btp_ptsl2c_item_t *pPtsL2c;

    pPtsL2c = spBtPtsL2capCtrl->item;
    if (handle == 0 || pPtsL2c->handle != handle) {
        bth_signal_destroyConnectByUsrID(handle, BTP_PSMID_PTS_L2C, 0);
        tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_destroy: ptsl2c not exist");
        return;
    }

    tlkapi_trace(BTP_PTSL2C_DBG_FLAG,
                 BTP_PTSL2C_DBG_SIGN,
                 "btp_ptsl2c_destroy: handle[%d] state[%d] chnID[0x%x]",
                 handle,
                 pPtsL2c->state,
                 pPtsL2c->chnID);

    state = pPtsL2c->state;
    chnID = pPtsL2c->chnID;
    btp_ptsl2c_resetNode(pPtsL2c);

    if (state == TLK_STATE_CONNING) {
        btp_send_connectEvt(TLK_ETIMEOUT, BTP_PTYPE_PTS_L2C, BTP_USRID_NONE, handle, chnID);
    } else {
        btp_send_disconnEvt(BTP_PTYPE_PTS_L2C, BTP_USRID_NONE, handle);
    }
}

int btp_ptsl2c_sendTestData(uint16 dataLen)
{
    btp_ptsl2c_item_t *pPtsL2c;

    if (dataLen == 0) {
        tlkapi_error(BTP_PTSL2C_DBG_FLAG,
                     BTP_PTSL2C_DBG_SIGN,
                     "btp_ptsl2c_sendTestData: paramError - dataLen[%d]",
                     dataLen);
        return -TLK_EFAIL;
    }
    if (dataLen > BTP_PTSL2C_BUFFER_LENGTH) {
        tlkapi_warn(BTP_PTSL2C_DBG_FLAG,
                    BTP_PTSL2C_DBG_SIGN,
                    "btp_ptsl2c_sendTestData: send length is too long [%d-%d]",
                    dataLen,
                    BTP_PTSL2C_BUFFER_LENGTH);
        dataLen = BTP_PTSL2C_BUFFER_LENGTH;
    }

    pPtsL2c = spBtPtsL2capCtrl->item;
    if (pPtsL2c->state != TLK_STATE_CONNECT) {
        tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_sendTestData: failure - not connected");
        return -TLK_ESTATUS;
    }
    if (pPtsL2c->rtnMode == BTH_L2CAP_SIG_RTNMODE_BASIC) {
        return bth_l2cap_sendChannelData(pPtsL2c->handle, pPtsL2c->chnID, nullptr, 0, spBtPtsL2capCtrl->buffer, dataLen);
    } else {
        int ret;
        bth_l2cap_iframeEnhCtrlField_t *pICtrl;
        ret = bth_l2cap_sendIFrame(pPtsL2c->handle,
                                   pPtsL2c->chnID,
                                   pPtsL2c->ehnCtrl,
                                   nullptr,
                                   0,
                                   spBtPtsL2capCtrl->buffer,
                                   dataLen,
                                   pPtsL2c->fscIsEn);
        if (ret == TLK_ENONE) {
            pICtrl = (bth_l2cap_iframeEnhCtrlField_t *)&pPtsL2c->ehnCtrl;
            pICtrl->field.TxSeq += 1;
            pPtsL2c->busys |= BTP_PTSL2C_BUSY_WAIT_ACK;
            pPtsL2c->rtnNumb  = pPtsL2c->rtnTime;
            pPtsL2c->monNumb  = pPtsL2c->monTime;
            pPtsL2c->rtnTxNum = pPtsL2c->rtnMaxTx;
            btp_adapt_insertTimer(&pPtsL2c->timer);
        }
        return ret;
    }
}


static void btp_ptsl2c_resetNode(btp_ptsl2c_item_t *pPtsL2c)
{
    if (pPtsL2c == nullptr) {
        return;
    }
    pPtsL2c->state  = TLK_STATE_CLOSED;
    pPtsL2c->chnID  = 0;
    pPtsL2c->handle = 0;
    btp_adapt_removeTimer(&pPtsL2c->timer);
    tmemset(pPtsL2c, 0, sizeof(btp_ptsl2c_item_t));
}


static bool btp_ptsl2c_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
    btp_ptsl2c_item_t *pPtsL2c = (btp_ptsl2c_item_t *)userArg;

    if (pPtsL2c == nullptr) {
        tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_timer: fault usrarg");
        return false;
    }

    if (pPtsL2c->timeout != 0) {
        pPtsL2c->timeout--;
    }
    if (pPtsL2c->timeout == 0 && pPtsL2c->state == TLK_STATE_DISCING) {
        uint16 handle = pPtsL2c->handle;
        tlkapi_trace(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_timer: DISCING - timeout");
        btp_ptsl2c_resetNode(pPtsL2c);
        btp_send_disconnEvt(BTP_PTYPE_PTS_L2C, BTP_USRID_NONE, handle);
    } else if (pPtsL2c->timeout == 0 && pPtsL2c->state == TLK_STATE_CONNING) {
        uint16 handle = pPtsL2c->handle;
        tlkapi_trace(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_timer: CONNING - timeout");
        btp_ptsl2c_resetNode(pPtsL2c);
        bth_signal_destroyConnect(handle, BTP_PSMID_PTS_L2C);
        btp_send_connectEvt(TLK_ETIMEOUT, BTP_PTYPE_PTS_L2C, BTP_USRID_NONE, handle, 0);
    }
    if (pPtsL2c->state == TLK_STATE_CLOSED) {
        return false;
    }
    if (pPtsL2c->busys != 0) {
        btp_ptsl2c_procs(pPtsL2c);
    }

    if (pPtsL2c->busys != 0 || pPtsL2c->timeout != 0) {
        return true; // repeat
    }
    return false;    // stop
}
static void btp_ptsl2c_procs(btp_ptsl2c_item_t *pPtsL2c)
{
    if ((pPtsL2c->busys & BTP_PTSL2C_BUSY_WAIT_ACK) != 0) {
        if (pPtsL2c->rtnNumb != 0) {
            pPtsL2c->rtnNumb--;
        }
        if (pPtsL2c->rtnNumb == 0 && pPtsL2c->rtnTxNum != 0) {
            bth_l2cap_sframeEnhCtrlField_t sfield = { 0 };
            bth_l2cap_iframeEnhCtrlField_t *pICtrl1;
            pPtsL2c->rtnNumb = pPtsL2c->rtnTime;
            if (pPtsL2c->rtnTxNum != 0) {
                pPtsL2c->rtnTxNum--;
            }
            pICtrl1             = (bth_l2cap_iframeEnhCtrlField_t *)&pPtsL2c->ehnCtrl;
            sfield.field.P      = 1;
            sfield.field.F      = 0;
            sfield.field.ReqSeq = pICtrl1->field.ReqSeq;
            tlkapi_trace(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_procs: BTP_PTSL2C_BUSY_WAIT_ACK");
            bth_l2cap_sendSFrame(pPtsL2c->handle,
                                 pPtsL2c->chnID,
                                 sfield.value,
                                 nullptr,
                                 0,
                                 nullptr,
                                 0,
                                 pPtsL2c->fscIsEn);
        }
        if (pPtsL2c->rtnTxNum == 0) {
            if (pPtsL2c->monNumb != 0) {
                pPtsL2c->monNumb--;
            }
            if (pPtsL2c->monNumb == 0) {
                btp_ptsl2c_disconn(pPtsL2c->handle);
            }
        }
    }
}


static int btp_ptsl2c_eventCB(uint08 evtID, uint16 psmID, uint08 *pData, uint16 dataLen)
{
    btp_ptsl2c_item_t *pPtsL2c;

    pPtsL2c = spBtPtsL2capCtrl->item;
    if (evtID == BTH_L2CAP_EVTID_REQUEST) {
        bth_l2cap_requestEvt_t *pEvt = (bth_l2cap_requestEvt_t *)pData;
        tlkapi_trace(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_eventCB: request");
        if (pPtsL2c->handle != 0) {
            return -TLK_EQUOTA;
        }
        if (spBtPtsL2capCtrl->user->enRtn) {
            bth_signal_enableRtnOption(pEvt->handle, pEvt->scid, true);
            bth_signal_setRtnMode(pEvt->handle, pEvt->scid, spBtPtsL2capCtrl->user->rtnMode);
        }
        if (spBtPtsL2capCtrl->user->enFcs) {
            bth_signal_enableFcsOption(pEvt->handle, pEvt->scid, true);
            bth_signal_setFcsType(pEvt->handle, pEvt->scid, spBtPtsL2capCtrl->user->fcsType);
        }
        if (spBtPtsL2capCtrl->user->enEfs) {
            bth_signal_enableEfsOption(pEvt->handle, pEvt->scid, true);
        }
    } else if (evtID == BTH_L2CAP_EVTID_CONNECT) {
        uint08 fcsType               = 0x00;
        bth_l2cap_connectEvt_t *pEvt = (bth_l2cap_connectEvt_t *)pData;

        if (pPtsL2c->handle != 0 && pPtsL2c->handle != pEvt->handle) {
            tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_eventCB: pPtsL2c null err");
            bth_signal_disconnChannel(pEvt->handle, pEvt->scid);
            return -TLK_EFAIL;
        }

        pPtsL2c->state   = TLK_STATE_CONNECT;
        pPtsL2c->chnID   = pEvt->scid;
        pPtsL2c->handle  = pEvt->handle;
        pPtsL2c->timeout = gBtpConnTimeout;
        if (bth_signal_getRtnMode(pEvt->handle, pEvt->scid, &pPtsL2c->rtnMode) != TLK_ENONE) {
            pPtsL2c->rtnMode = BTH_L2CAP_SIG_RTNMODE_BASIC;
        }
        if (pPtsL2c->rtnMode != BTH_L2CAP_SIG_RTNMODE_BASIC) {
            uint16 monTime = 0;
            uint16 rtnTime = 0;
            bth_signal_getRtnMonTime(pEvt->handle, pEvt->scid, &monTime);
            bth_signal_getRtnRtnTime(pEvt->handle, pEvt->scid, &rtnTime);
            pPtsL2c->monTime = (monTime + BTP_TIMER_TIMEOUT_MS - 1) / BTP_TIMER_TIMEOUT_MS;
            if (pPtsL2c->monTime == 0) {
                pPtsL2c->monTime = 5;
            }
            pPtsL2c->rtnTime = rtnTime / BTP_TIMER_TIMEOUT_MS;
            if (pPtsL2c->rtnTime == 0) {
                pPtsL2c->rtnTime = 3;
            }
            bth_signal_getRtnCount(pEvt->handle, pEvt->scid, &pPtsL2c->rtnMaxTx);
            if (pPtsL2c->rtnMaxTx == 0) {
                pPtsL2c->rtnMaxTx = 1;
            }
        }
        /******************************************************************************
         * FCS: It is non-negotiable. The FCS option shall only be used when the mode
         * is being, or is already configured to Enhanced Retransmission mode or
         * Streaming mode.
         *******************************************************************************/
        if (bth_signal_getFcsType(pEvt->handle, pEvt->scid, &fcsType) != TLK_ENONE) {
            if (pPtsL2c->rtnMode == BTH_L2CAP_SIG_RTNMODE_ENHANCE ||
                pPtsL2c->rtnMode == BTH_L2CAP_SIG_RTNMODE_STREAMING) {
                pPtsL2c->fscIsEn = true;
            }
        } else {
            if (fcsType == BTH_L2CAP_SIG_FCSTYPE_16bFCS) {
                pPtsL2c->fscIsEn = true;
            } else {
                pPtsL2c->fscIsEn = false;
            }
        }

        tlkapi_trace(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_eventCB: connect: fcsType[%d]", fcsType);

        btp_adapt_initTimer(&pPtsL2c->timer, btp_ptsl2c_timer, (uint32)pPtsL2c, BTP_TIMER_TIMEOUT);
        btp_adapt_insertTimer(&pPtsL2c->timer);

        btp_send_connectEvt(TLK_ENONE, BTP_PTYPE_PTS_L2C, BTP_USRID_NONE, pPtsL2c->handle, pPtsL2c->chnID);
    } else if (evtID == BTH_L2CAP_EVTID_DISCONN || evtID == BTH_L2CAP_EVTID_CLOSED) {
        uint16 handle = 0;
        if (evtID == BTH_L2CAP_EVTID_DISCONN) {
            tlkapi_trace(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_eventCB: disconn");
        } else {
            tlkapi_trace(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_eventCB: closed");
        }
        if (evtID == BTH_L2CAP_EVTID_CLOSED) {
            bth_l2cap_closedEvt_t *pEvt = (bth_l2cap_closedEvt_t *)pData;
            handle                      = pEvt->handle;
        } else {
            bth_l2cap_disconnEvt_t *pEvt = (bth_l2cap_disconnEvt_t *)pData;
            handle                       = pEvt->handle;
        }
        if (pPtsL2c->handle == handle) {
            uint08 state = pPtsL2c->state;
            btp_ptsl2c_resetNode(pPtsL2c);
            if (state == TLK_STATE_CONNING) {
                btp_send_connectEvt(TLK_ETIMEOUT, BTP_PTYPE_PTS_L2C, BTP_USRID_NONE, handle, 0);
            } else {
                btp_send_disconnEvt(BTP_PTYPE_PTS_L2C, BTP_USRID_NONE, handle);
            }
        }
    }

    return TLK_ENONE;
}
static void btp_ptsl2c_rdataCB(uint16 psmID, uint16 chnID, uint16 handle, uint08 *pData, uint16 dataLen)
{
    uint08 othLen;
    uint16 ehnCtrl;
    btp_ptsl2c_item_t *pPtsL2c;
    bth_l2cap_iframeEnhCtrlField_t *pICtrl0;
    bth_l2cap_iframeEnhCtrlField_t *pICtrl1;

    tlkapi_array(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_rdataCB:", pData, dataLen);

    pPtsL2c = spBtPtsL2capCtrl->item;
    if (pPtsL2c->handle != handle) {
        tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_rdataCB: no node");
        return;
    }

    if (pPtsL2c->rtnMode == BTH_L2CAP_SIG_RTNMODE_ENHANCE) {
        if (pPtsL2c->fscIsEn) {
            othLen = 4;
        } else {
            othLen = 2;
        }
        if (pData == nullptr || dataLen < othLen) {
            tlkapi_error(BTP_PTSL2C_DBG_FLAG, BTP_PTSL2C_DBG_SIGN, "btp_ptsl2c_rdataCB: length[%d] error 0", dataLen);
            return; // No Body
        }

        ehnCtrl = (((uint16)pData[1]) << 8) | pData[0];
        pICtrl0 = (bth_l2cap_iframeEnhCtrlField_t *)&ehnCtrl;
        pICtrl1 = (bth_l2cap_iframeEnhCtrlField_t *)&pPtsL2c->ehnCtrl;
        if (ehnCtrl & 0x0001) { // S-Frame Deal
            bth_l2cap_sframeEnhCtrlField_t *pSfield = (bth_l2cap_sframeEnhCtrlField_t *)&ehnCtrl;
            if (pSfield->field.P == 1) {
                pPtsL2c->busys &= ~BTP_PTSL2C_BUSY_WAIT_ACK;
                bth_l2cap_sframeEnhCtrlField_t sfield = { 0 };
                sfield.field.F                        = 1;
                sfield.field.ReqSeq                   = pICtrl1->field.ReqSeq;
                bth_l2cap_sendSFrame(handle, pPtsL2c->chnID, sfield.value, nullptr, 0, nullptr, 0, pPtsL2c->fscIsEn);
            }
            if (pSfield->field.F == 0) {
                if (pICtrl1->field.TxSeq == pICtrl0->field.TxSeq + 1) {
                    pPtsL2c->busys &= ~BTP_PTSL2C_BUSY_WAIT_ACK;
                } else {
                    btp_ptsl2c_disconn(pPtsL2c->handle);
                }
            } else {
                if (pICtrl1->field.TxSeq == pICtrl0->field.TxSeq + 1 && pPtsL2c->rtnTxNum != 0) {
                    pPtsL2c->busys &= ~BTP_PTSL2C_BUSY_WAIT_ACK;
                    pICtrl1->field.TxSeq -= 1;
                } else {
                    btp_ptsl2c_disconn(pPtsL2c->handle);
                }
            }
            return;
        }

        if (pICtrl1->field.ReqSeq != pICtrl0->field.TxSeq || pICtrl1->field.TxSeq != pICtrl0->field.ReqSeq) {
            tlkapi_error(BTP_PTSL2C_DBG_FLAG,
                         BTP_PTSL2C_DBG_SIGN,
                         "btp_ptsl2c_rdataCB: ExpSeq[%d],RcvSeq[%d]",
                         pICtrl1->field.ReqSeq,
                         pICtrl0->field.ReqSeq);
            {
                bth_l2cap_sframeEnhCtrlField_t sfield = { 0 };
                sfield.field.P                        = 0;
                sfield.field.F                        = 1; // Req Retransmit
                sfield.field.ReqSeq                   = pICtrl1->field.ReqSeq;
                bth_l2cap_sendSFrame(handle, pPtsL2c->chnID, sfield.value, nullptr, 0, nullptr, 0, pPtsL2c->fscIsEn);
            }
            return;
        }
        pICtrl1->field.ReqSeq = pICtrl0->field.TxSeq + 1;
        pPtsL2c->busys &= ~BTP_PTSL2C_BUSY_WAIT_ACK;

        if (pICtrl0->field.F == 0) {
            bth_l2cap_sframeEnhCtrlField_t sfield = { 0 };
            sfield.field.P                        = 0;
            sfield.field.F                        = 0;
            sfield.field.ReqSeq                   = pICtrl1->field.ReqSeq;
            bth_l2cap_sendSFrame(handle, pPtsL2c->chnID, sfield.value, nullptr, 0, nullptr, 0, pPtsL2c->fscIsEn);
        }
    }
}


#endif // #if (TLKBTP_CFG_PTSL2C_ENABLE)
#endif // #if (TLK_STK_BTP_ENABLE)
