/********************************************************************************************************
 * @file	tlkmmi_btmgrRec.c
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
#include "tlkmmi_btmgr.h"
#if (TLKMMI_BTMGR_BTREC_ENABLE)
#include "tlkmmi_btmgrCtrl.h"
#include "tlkmmi_btmgrAcl.h"
#include "tlkmmi_btmgrInq.h"
#include "tlkmmi_btmgrRec.h"

#include "tlkmdi/bt/tlkmdi_btacl.h"
#include "tlkmdi/bt/tlkmdi_btrec.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/bth/bth_device.h"


static void tlkmmi_btmgr_recOver(void);

/******************************************************************************
 * Function: tlkmmi_btmgr_recInit
 * Descript: Trigger the BT page initial. 
 * Params:
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_recInit(void)
{
	tlkmdi_btrec_init();
	tlkmdi_btrec_setKeepMode(TLKMDI_BTREC_KEEP_MODE_BOTH_SCAN, 120);
	tlkmdi_btrec_setPageParam(10000, 15);
	tlkmdi_btrec_setScanParam(3000, 0, true, true);

	tlkmdi_btrec_regCB(tlkmmi_btmgr_recOver);

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_btmgr_recStart
 * Descript: Trigger start BT page. 
 * Params:
 *         @pDevAddr[IN]--The device bt address.
 *         @devClass[IN]--The device class type.
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_recStart(uint08 *pDevAddr, uint32 devClass, bool enPage, bool enScan)
{	
	#if (TLKMMI_BTMGR_BTACL_ENABLE)
	if(tlkmmi_btmgr_aclIsBusy()){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recStart: failure - mmi acl busy");
		return -TLK_EBUSY;
	}
	#endif
//	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recOver 02");
	#if (TLK_MDI_BTACL_ENABLE)
	if(tlkmdi_btacl_getBusyItem() != nullptr){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recStart: failure - mdi acl busy");
		return -TLK_EBUSY;
	}
	if(enPage && pDevAddr != nullptr && tlkmdi_btacl_searchConnItem(pDevAddr) != nullptr){
		tlkapi_error(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recStart: failure - device exist");
		return -TLK_EEXIST;
	}
	#endif
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recStart: enPage-%d,enScan-%d,devClass-0x%06x", enPage, enScan, devClass);
	return tlkmdi_btrec_start(pDevAddr, devClass, enPage, enScan);
}

/******************************************************************************
 * Function: tlkmmi_btmgr_recClose
 * Descript: Trigger stop BT page. 
 * Params: None.
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_btmgr_recClose(void)
{
	return tlkmdi_btrec_close();
}

/******************************************************************************
 * Function: tlkmmi_btmgr_recIsBusy
 * Descript: Check the BT is busy. 
 * Params: None.
 * Return: Return true is busy, false is idle.
 * Others: None.
*******************************************************************************/
bool tlkmmi_btmgr_recIsBusy(void)
{
	return tlkmdi_btrec_isInBusy();
}

bool tlkmmi_btmgr_recIsPage(void)
{
	return tlkmdi_btrec_isInPage();
}
bool tlkmmi_btmgr_recIsScan(void)
{
	return tlkmdi_btrec_isInScan();
}

uint08 *tlkmmi_btmgr_recPageAddr(void)
{
	return tlkmdi_btrec_getPageAddr();
}


static void tlkmmi_btmgr_recOver(void)
{
	tlkapi_trace(TLKMMI_BTMGR_DBG_FLAG, TLKMMI_BTMGR_DBG_SIGN, "tlkmmi_btmgr_recOver");
}


#endif //#if (TLKMMI_BTMGR_BTREC_ENABLE)

