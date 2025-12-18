/********************************************************************************************************
 * @file	tlkmdi_bthfp.c
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
#if (TLK_MDI_BTHFP_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/bt/tlkmdi_bthfp.h"
#include "tlkmdi/bt/tlkmdi_bthfphf.h"
#include "tlkmdi/bt/tlkmdi_bthfpag.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/bt/btp/hfp/btp_hfp.h"
#include "tlksys/prt/tlkpto_comm.h"


#define TLKMDI_BTHFP_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_BT << 24) | (TLK_MINOR_DBGID_MDI_BT_HFP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_BTHFP_DBG_SIGN       "[MHFP]"


/******************************************************************************
 * Function: tlkmdi_hfphf_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_bthfp_init(void)
{	
	#if (TLK_MDI_AUDAG_ENABLE)
	tlkmdi_bthfpag_init();
	#endif

	#if (TLK_MDI_AUDHF_ENABLE)
	tlkmdi_bthfphf_init();
	#endif

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmdi_hfp_destroy.
 * Descript: Reset the HF control block and release resource.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_bthfp_destroy(uint16 aclHandle)
{
	tlkmdi_bthfpag_reset();
	tlkmdi_bthfphf_reset(aclHandle);
}

#endif //#if (TLK_MDI_BTHFP_ENABLE)

