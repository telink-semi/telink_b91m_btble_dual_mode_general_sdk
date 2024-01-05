/********************************************************************************************************
 * @file	tlkmdi_audhfp.c
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
#if (TLK_MDI_AUDHFP_ENABLE)
#include "tlkmdi/aud/tlkmdi_audhfp.h"

#include "tlkdev/sys/tlkdev_codec.h"


#define TLKMDI_AUDHF_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_HFP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDHF_DBG_SIGN       "[MDI]"


static tlkmdi_audhfp_ctrl_t sTlkMdiAudHfpCtrl;



/******************************************************************************
 * Function: tlkmdi_audhfp_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_audhfp_init(void)
{
	
	
	return TLK_ENONE;
}

int tlkmdi_audhfp_start(uint16 handle)
{
	return -TLK_ENOSUPPORT;
}
int tlkmdi_audhfp_close(uint16 handle)
{
	return -TLK_ENOSUPPORT;
}

/******************************************************************************
 * Function: tlkmdi_audhfp_isBusy.
 * Descript: Get the HF control block is in-using.
 * Params: None.
 * Return: Return true or false.
 * Others: None.
*******************************************************************************/
bool tlkmdi_audhfp_isBusy(void)
{
	return (sTlkMdiAudHfpCtrl.status == TLK_STATE_OPENED);
}

/******************************************************************************
 * Function: tlkmdi_audhfp_intval.
 * Descript: Get the HF control block interval.
 * Params: None.
 * Return: Return the interval.
 * Others: None.
*******************************************************************************/
uint tlkmdi_audhfp_intval(void)
{
	return 1000;
}

/******************************************************************************
 * Function: tlkmdi_audhfp_switch.
 * Descript: Trigger to Change the HF control block status.
 * Params: None.
 * Return: Return true or false.
 * Others: None.
*******************************************************************************/
bool tlkmdi_audhfp_switch(uint16 handle, uint08 status)
{
	sTlkMdiAudHfpCtrl.status = status;
	sTlkMdiAudHfpCtrl.handle = handle;
	return true;
}

/******************************************************************************
 * Function: tlkmdi_audhfp_irqProc.
 * Descript: Get the HF status.
 * Params: None.
 * Return: Return HF status.
 * Others: None.
*******************************************************************************/
bool tlkmdi_audhfp_irqProc(void)
{
	if(sTlkMdiAudHfpCtrl.status == TLK_STATE_OPENED) return true;
	else return false;
}





#endif //#if (TLK_MDI_AUDHFP_ENABLE)

