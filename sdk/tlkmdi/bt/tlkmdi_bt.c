/********************************************************************************************************
 * @file	tlkmdi_bt.c
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
#if (TLK_STK_BT_ENABLE)
#include "tlkmdi_btadapt.h"
#include "tlkmdi_bt.h"
#include "tlkmdi/bt/tlkmdi_btacl.h"
#include "tlkmdi/bt/tlkmdi_btinq.h"
#include "tlkmdi/bt/tlkmdi_btrec.h"
#include "tlkmdi/bt/tlkmdi_bthfp.h"
#if (TLK_MDI_BTATT_ENABLE)
#include "tlkmdi/bt/tlkmdi_btatt.h"
#endif
#if (TLK_MDI_BTHID_ENABLE)
#include "tlkmdi/bt/tlkmdi_bthid.h"
#endif
#if (TLK_MDI_BTIAP_ENABLE)
#include "tlkmdi/bt/tlkmdi_btiap.h"
#endif
#if (TLK_MDI_BTA2DP_ENABLE)
#include "tlkmdi/bt/tlkmdi_bta2dp.h"
#endif
#if (TLK_MDI_BTSCO_ENABLE)
#include "tlkmdi/bt/tlkmdi_btsco.h"
#endif

int tlkmdi_bt_init(uint08 procID)
{
	tlkmdi_btadapt_init(procID);
	
	#if (TLK_MDI_BTACL_ENABLE)
	tlkmdi_btacl_init();
	#endif
	#if (TLK_MDI_BTINQ_ENABLE)
	tlkmdi_btinq_init();
	#endif 
	#if (TLK_MDI_BTREC_ENABLE)
	tlkmdi_btrec_init();
	#endif
	#if (TLK_MDI_BTATT_ENABLE)
	tlkmdi_btatt_init();
	#endif
	#if (TLK_MDI_BTHID_ENABLE)
	tlkmdi_bthid_init();
	#endif
	#if (TLK_MDI_BTHFP_ENABLE)
	tlkmdi_bthfp_init();
	#endif
	#if (TLK_MDI_BTIAP_ENABLE)
	tlkmdi_btiap_init();
	#endif
	#if (TLK_MDI_BTA2DP_ENABLE)
	tlkmdi_bta2dp_init();
	#endif
	#if (TLK_MDI_BTSCO_ENABLE)
	tlkmdi_btsco_init();
	#endif
	
	return TLK_ENONE;
}

void tlkmdi_bt_handler(void)
{
	tlkmdi_btadapt_handler();
}




#endif //#if (TLK_STK_BT_ENABLE)

