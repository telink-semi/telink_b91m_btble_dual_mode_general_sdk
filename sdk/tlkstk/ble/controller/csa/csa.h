/********************************************************************************************************
 * @file	csa.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef CSA_H_
#define CSA_H_

#if (LL_FEATURE_ENABLE_CHANNEL_SELECTION_ALGORITHM2)


/******************************* Macro & Enumeration variables for User Begin ******************************************/
//See the Core_v5.0(Vol 6/Part B/4.5.8, "Data Channel Index Selection") for more information.
typedef enum {
	CHANNAL_SELECTION_ALGORITHM_1      	=	0x00,
	CHANNAL_SELECTION_ALGORITHM_2      	=	0x01,
} channel_algorithm_t;
/******************************* Macro & Enumeration variables for User End ********************************************/


/******************************* Macro & Enumeration & Structure Definition for Stack Begin, user can not use!!!!  *****/
extern channel_algorithm_t local_chsel;
/******************************* Macro & Enumeration & Structure Definition for Stack End ******************************/


/******************************* Stack Interface Begin, user can not use!!! ********************************************/
typedef u8 (*ll_chn_index_calc_callback_t)(u8*, u16, u16, u8*, u8);
extern ll_chn_index_calc_callback_t	ll_chn_index_calc_cb;

u16		blt_csa2_getPrn(u16 counter, u16 ch_id, bool prn_s);

#if (LL_FEATURE_ENABLE_ISO)
	u8  	blt_csa2_getRemapIdxOfLastUsedChn4SE(u8 chm[5], u16 event_cntr, u16 channel_id, u8 *remap_tbl, u8 channel_used_num);
	void	blt_csa2_getPrnSubEvent(u16 lastUsed_prn, u16 chnId, u16* prnSubEvent_lu, u16* prnSubEvent_se);
	u8		blt_csa2_calcSubEventIdx(u8 chnUsedNums, u8 d, u8 idxOfLastUsedChn, u16 prnSubEvent_se);
#endif

u8 		blt_csa2_calculateRemapping_table(u8 chm[5], u8 *remap_tbl, u8 *channel_used_num);
u8 		blt_csa2_calculateChannel_index(u8 chm[5], u16 event_cntr, u16 channel_id, u8 *remap_tbl, u8 channel_used_num);
/******************************* Stack Interface End *******************************************************************/


/******************************* User Interface  Begin *****************************************************************/
void blc_ll_initChannelSelectionAlgorithm_2_feature(void);
/******************************* User Interface  End  ******************************************************************/
#endif
#endif /* LL_CONN_CSA_H_ */
