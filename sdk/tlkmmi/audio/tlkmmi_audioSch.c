/********************************************************************************************************
 * @file	tlkmmi_audioSch.c
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
#if (TLKMMI_AUDIO_ENABLE)
#include "tlksys/prt/tlkpti_audio.h"
#include "tlkmmi_audio.h"
#include "tlkmmi_audioCtrl.h"
#include "tlkmmi_audioSch.h"
#include "tlkmmi_audioModinf.h"



#if (TLK_MDI_AUDTONE_ENABLE)
static const tlkmmi_audio_schParam_t sTlkMMidAudioToneParam = {
	TLKPTI_AUD_OPTYPE_TONE, TLKMMI_AUDIO_TONE_PRIORITY, true,  true
};
#endif
#if (TLK_MDI_AUDPLAY_ENABLE)
static const tlkmmi_audio_schParam_t sTlkMMidAudioPlayParam = {
	TLKPTI_AUD_OPTYPE_PLAY, TLKMMI_AUDIO_PLAY_PRIORITY, false, false
};
#endif
#if (TLK_MDI_AUDHFP_ENABLE)
static const tlkmmi_audio_schParam_t sTlkMMidAudioHfParam = {
	TLKPTI_AUD_OPTYPE_HF,  TLKMMI_AUDIO_HFP_PRIORITY,   false, false
};
#endif
#if (TLK_MDI_AUDHFP_ENABLE)
static const tlkmmi_audio_schParam_t sTlkMMidAudioAgParam = {
	TLKPTI_AUD_OPTYPE_AG,  TLKMMI_AUDIO_HFP_PRIORITY,   false, false
};
#endif
#if (TLK_MDI_AUDSCO_ENABLE)
static const tlkmmi_audio_schParam_t sTlkMMidAudioScoParam = {
	TLKPTI_AUD_OPTYPE_SCO,  TLKMMI_AUDIO_SCO_PRIORITY,  true,  true
};
#endif
#if (TLK_MDI_AUDDSCO_ENABLE)
static const tlkmmi_audio_schParam_t sTlkmmiAudioDscoParam = {
	TLKPTI_AUD_OPTYPE_DSCO, TLKMMI_AUDIO_DSCO_PRIORITY,true,true
};
#endif
#if (TLK_MDI_AUDSRC_ENABLE)
static const tlkmmi_audio_schParam_t sTlkMMidAudioSrcParam = {
	TLKPTI_AUD_OPTYPE_SRC,  TLKMMI_AUDIO_SRC_PRIORITY,  false, false
};
#endif
#if (TLK_MDI_AUDSNK_ENABLE)
static const tlkmmi_audio_schParam_t sTlkMMidAudioSnkParam = {
	TLKPTI_AUD_OPTYPE_SNK,  TLKMMI_AUDIO_SNK_PRIORITY,  false, false
};
#endif
#if (TLK_MDI_AUDUAC_ENABLE)
static const tlkmmi_audio_schParam_t sTlkMMidAudioUacParam = {
	TLKPTI_AUD_OPTYPE_UAC,  TLKMMI_AUDIO_UAC_PRIORITY,  false, false
};
#endif
static const tlkmmi_audio_schParam_t *spTlkMMidAudioSchList[TLKPTI_AUD_OPTYPE_MAX] = {
	nullptr,
#if (TLK_MDI_AUDTONE_ENABLE)
	&sTlkMMidAudioToneParam,
#else
	nullptr,
#endif
#if (TLK_MDI_AUDPLAY_ENABLE)
	&sTlkMMidAudioPlayParam,
#else
	nullptr,
#endif
#if (TLK_MDI_AUDHFP_ENABLE)
	&sTlkMMidAudioHfParam,
#else
	nullptr,
#endif
#if (TLK_MDI_AUDHFP_ENABLE)
	&sTlkMMidAudioAgParam,
#else
	nullptr,
#endif
#if (TLK_MDI_AUDSCO_ENABLE)
	&sTlkMMidAudioScoParam,
#else
	nullptr,
#endif
#if (TLK_MDI_AUDSRC_ENABLE)
	&sTlkMMidAudioSrcParam,
#else
	nullptr,
#endif
#if (TLK_MDI_AUDSNK_ENABLE)
	&sTlkMMidAudioSnkParam,
#else
	nullptr,
#endif
#if (TLK_MDI_AUDUAC_ENABLE)
	&sTlkMMidAudioUacParam,
#else
	nullptr,
#endif
#if (TLK_MDI_AUDDSCO_ENABLE)
	&sTlkmmiAudioDscoParam,
#else
	nullptr,
#endif
};
static tlkmmi_audio_schCtrl_t sTlkMMidAudioSchCtrl = {0};


/******************************************************************************
 * Function: tlkmmi_audio_schInit
 * Descript: Initial all of audio status control block. 
 * Params: None.
 * Return: Return TLK_ENONE is success and other value is filure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_schInit(void)
{
	tmemset(&sTlkMMidAudioSchCtrl, 0, sizeof(tlkmmi_audio_schCtrl_t));
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_getCurOptype
 * Descript: Get the operate type of the Current of audio status control block 
 *           in it's array. 
 * Params: None.
 * Return: Return the operate type.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_getCurOptype(void)
{
	if(sTlkMMidAudioSchCtrl.nowNumb == 0) return TLKPTI_AUD_OPTYPE_NONE;
	else return sTlkMMidAudioSchCtrl.item[0].optype;
}

/******************************************************************************
 * Function: tlkmmi_audio_isHaveOptype
 * Descript: Check the audio status control block array have match 
 *           the special operate type.
 * Params: None.
 * Return: Return true(there exist a audio control block's 
 *         opertate type equal the optye).
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_isHaveOptype(uint08 optype)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioSchCtrl.nowNumb; index++){
		if(sTlkMMidAudioSchCtrl.item[index].optype == optype) break;
	}
	if(index == sTlkMMidAudioSchCtrl.nowNumb) return false;
	else return true;
}

/******************************************************************************
 * Function: tlkmmi_audio_getCurItem
 * Descript: Get the current audio status control block.
 * Params: None.
 * Return: Return current audio control block.
 * Others: None.
*******************************************************************************/
tlkmmi_audio_schItem_t *tlkmmi_audio_getCurItem(void)
{
	if(sTlkMMidAudioSchCtrl.nowNumb == 0) return nullptr;
	else return &sTlkMMidAudioSchCtrl.item[0];
}

/******************************************************************************
 * Function: tlkmmi_audio_getSndItem
 * Descript: Get the 2nd audio status control block.
 * Params: None.
 * Return: Return 2nd audio control block.
 * Others: None.
*******************************************************************************/
tlkmmi_audio_schItem_t *tlkmmi_audio_getSndItem(void)
{
	if(sTlkMMidAudioSchCtrl.nowNumb < 2) return nullptr;
	else return &sTlkMMidAudioSchCtrl.item[1];
}

/******************************************************************************
 * Function: tlkmmi_audio_itemIndex
 * Descript: Get the current audio status control block index. 
 * Params:
 *         @handle[IN]--The handle
 *         @optype[IN]--The audio operate type.
 * Return: The audio status control block index.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_itemIndex(uint16 handle, uint08 optype)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioSchCtrl.nowNumb; index++){
//		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_itemIndex: %d, %d, %d, %d", 
//			optype, handle, sTlkMMidAudioSchCtrl.item[index].optype, sTlkMMidAudioSchCtrl.item[index].handle);
		if(sTlkMMidAudioSchCtrl.item[index].optype == optype && sTlkMMidAudioSchCtrl.item[index].handle == handle){
			break;
		}
	}
	if(index == sTlkMMidAudioSchCtrl.nowNumb) return -TLK_ENOOBJECT;
	return index;
}
int tlkmmi_audio_itemIndexByOptype(uint08 optype)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioSchCtrl.nowNumb; index++){
		if(sTlkMMidAudioSchCtrl.item[index].optype == optype){
			break;
		}
	}
	if(index == sTlkMMidAudioSchCtrl.nowNumb) return -TLK_ENOOBJECT;
	return index;
}

/******************************************************************************
 * Function: tlkmmi_audio_insertItem
 * Descript: Suspend the current audio task when a new audio task insert 
 *           and start running.
 * Params:
 *         @handle[IN]--The handle
 *         @optype[IN]--The audio operate type.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_insertItem(uint16 handle, uint08 optype)
{
	bool isSucc;
	uint08 iIndex;
	uint08 jIndex;
	uint16 tmpHdl;
	const tlkmmi_audio_schParam_t *pFunc0, *pFunc1;

	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: prev status - ");
	tlkmmi_audio_printfList();
	
	if(optype >= TLKPTI_AUD_OPTYPE_MAX || optype == 0){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: failure - error optype %d", optype);
		return -TLK_EPARAM;
	}
	if(tlkmmi_audio_itemIndex(handle, optype) >= 0){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: failure - repeat insert %d", optype);
		return -TLK_EREPEAT;
	}
	if(sTlkMMidAudioSchCtrl.nowNumb >= TLKMMI_AUDIO_STATUS_ITEM_NUMB){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: failure - status full %d", optype);
		return -TLK_EQUOTA;
	}
	
	pFunc0 = spTlkMMidAudioSchList[optype];
	if(pFunc0 == nullptr){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: failure - not supported %d", optype);
		return -TLK_ENOSUPPORT;
	}
	
	if(sTlkMMidAudioSchCtrl.nowNumb == 0){
		//If there is no record in the status list, insert it to the first position
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem[0]: success - %d", optype);
		sTlkMMidAudioSchCtrl.item[0].handle = handle;
		sTlkMMidAudioSchCtrl.item[0].optype = optype;
		sTlkMMidAudioSchCtrl.item[0].priority = pFunc0->priority;
		isSucc = tlkmmi_audio_modinfSwitch(pFunc0->optype, handle, TLK_STATE_OPENED);
		if(!isSucc){
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: failure -- open 01 - %d", pFunc0->optype);
			return -TLK_EFAIL;
		}else{
			sTlkMMidAudioSchCtrl.nowNumb ++;
			tlkmmi_audio_optypeChanged(pFunc0->optype, handle, 0, 0);
			return TLK_ENONE;
		}
	}
	
	pFunc1 = spTlkMMidAudioSchList[sTlkMMidAudioSchCtrl.item[0].optype];
	if(pFunc1 == nullptr) tmpHdl = 0;
	else tmpHdl = sTlkMMidAudioSchCtrl.item[0].handle;
	if(pFunc0->isFirst && pFunc1 != nullptr && pFunc0->priority <= pFunc1->priority){
		//If the state to be inserted is restricted to the first position, but the first position does not allow it to be inserted, failure is returned
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: failure - low priority - %d", optype);
		return -TLK_EFAIL;
	}
	for(iIndex=0; iIndex<sTlkMMidAudioSchCtrl.nowNumb; iIndex++){
		if(sTlkMMidAudioSchCtrl.item[iIndex].priority < pFunc0->priority) break;
	}
	if(iIndex == 0 && pFunc1->isMutex){
		//If the insertion position is the first, and the state of the first position does not allow the back row, the state is removed directly
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem[1]: success - %d", optype);
		sTlkMMidAudioSchCtrl.item[0].handle = handle;
		sTlkMMidAudioSchCtrl.item[0].optype = optype;
		sTlkMMidAudioSchCtrl.item[0].priority = pFunc0->priority;
		tlkmmi_audio_modinfSwitch(pFunc1->optype, tmpHdl, TLK_STATE_CLOSED);
		isSucc = tlkmmi_audio_modinfSwitch(pFunc0->optype, handle, TLK_STATE_OPENED);
		if(!isSucc){
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: failure -- open 02 - %d", pFunc0->optype);
			tlkmmi_audio_removeItem(handle, optype);
			return -TLK_EFAIL;
		}else{
			tlkmmi_audio_optypeChanged(pFunc0->optype, handle, pFunc1->optype, tmpHdl);
			return TLK_ENONE;
		}
	}
	if(iIndex != 0) pFunc1 = nullptr;
	if(iIndex != sTlkMMidAudioSchCtrl.nowNumb){
		//When inserting in the middle position, we need to move the state after the insertion position.
		for(jIndex=sTlkMMidAudioSchCtrl.nowNumb; jIndex>iIndex; jIndex--){
			tmemcpy(&sTlkMMidAudioSchCtrl.item[jIndex], &sTlkMMidAudioSchCtrl.item[jIndex-1], sizeof(tlkmmi_audio_schItem_t));
		}
	}
	
	sTlkMMidAudioSchCtrl.item[iIndex].handle = handle;
	sTlkMMidAudioSchCtrl.item[iIndex].optype = optype;
	sTlkMMidAudioSchCtrl.item[iIndex].priority = pFunc0->priority;
	sTlkMMidAudioSchCtrl.nowNumb ++;
	if(pFunc1 != nullptr){
		tlkmmi_audio_modinfSwitch(pFunc1->optype, tmpHdl, TLK_STATE_PAUSED);
	}
	if(iIndex == 0){
		isSucc = tlkmmi_audio_modinfSwitch(pFunc0->optype, handle, TLK_STATE_OPENED);
		if(isSucc){
			tlkmmi_audio_optypeChanged(pFunc0->optype, handle, pFunc1->optype, tmpHdl);
		}else{
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: failure -- open 03 - %d", pFunc0->optype);
			tlkmmi_audio_removeItem(handle, pFunc0->optype);
			return -TLK_EFAIL;
		}
	}
	
//	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem[2]: success - %d", optype);
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_insertItem: next status - ");
	tlkmmi_audio_printfList();
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmmi_audio_removeItem
 * Descript: Suspend the current task and delete it.Then resume the task 
 *           which been suspend before.
 * Params:
 *         @handle[IN]--The handle
 *         @optype[IN]--The audio operate type.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_removeItem(uint16 handle, uint08 optype)
{
	int index;
	bool isSucc;
	uint16 tmpHdl;
	const tlkmmi_audio_schParam_t *pFunc0; //To be removed
	const tlkmmi_audio_schParam_t *pFunc1; //reopen

//	tlkmmi_audio_printfList();

	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeItem:{handle-%d,optype-%d}", handle, optype);

	index = tlkmmi_audio_itemIndex(handle, optype);
	if(index < 0){
		tlkapi_error(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeItem: failure -- no node - %d", optype);
		return -TLK_ENOOBJECT;
	}

	tmpHdl = 0;
	pFunc0 = nullptr;
	pFunc1 = nullptr;
	if(index == 0) pFunc0 = spTlkMMidAudioSchList[sTlkMMidAudioSchCtrl.item[index].optype]; 
	if(index+1 != sTlkMMidAudioSchCtrl.nowNumb){
		tmemcpy(&sTlkMMidAudioSchCtrl.item[index], &sTlkMMidAudioSchCtrl.item[index+1], (sTlkMMidAudioSchCtrl.nowNumb-index-1)*sizeof(tlkmmi_audio_schItem_t));
	}
	sTlkMMidAudioSchCtrl.nowNumb --;
	if(index == 0 && sTlkMMidAudioSchCtrl.nowNumb != 0){
		pFunc1 = spTlkMMidAudioSchList[sTlkMMidAudioSchCtrl.item[0].optype];
		tmpHdl = sTlkMMidAudioSchCtrl.item[0].handle;
	}
	sTlkMMidAudioSchCtrl.item[sTlkMMidAudioSchCtrl.nowNumb].handle = 0;
	sTlkMMidAudioSchCtrl.item[sTlkMMidAudioSchCtrl.nowNumb].optype = 0;
	sTlkMMidAudioSchCtrl.item[sTlkMMidAudioSchCtrl.nowNumb].priority = 0;
	
	if(pFunc0 != nullptr) tlkmmi_audio_modinfSwitch(pFunc0->optype, handle, TLK_STATE_CLOSED);
	if(pFunc1 != nullptr){
		isSucc = tlkmmi_audio_modinfSwitch(pFunc1->optype, tmpHdl, TLK_STATE_OPENED);
		if(!isSucc){
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeItem: failure -- reopen %d", pFunc1->optype);
			tlkmmi_audio_removeItem(pFunc1->optype, tmpHdl);
			return -TLK_EFAIL;
		}else{
			tlkmmi_audio_optypeChanged(pFunc1->optype, tmpHdl, optype, handle);
			tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeItem:changed - %d  %d  %d  %d", 
				pFunc1->optype, tmpHdl, optype, handle);
		}
	}
	if(sTlkMMidAudioSchCtrl.nowNumb == 0){
		tlkmmi_audio_optypeChanged(0, 0, optype, handle);
	}
	
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_removeItem: optype-%d", optype);
	tlkmmi_audio_printfList();
	return index;
}

/******************************************************************************
 * Function: tlkmmi_audio_itemIsSwitch
 * Descript: Verify the priority of the audio.
 * Params: 
 *     @dstOptype[IN]--The operate type.
 *     @srcOptype[IN]--The operate type.
 * Return: True-"dstOptype" has a higher priority than "srcOptype".
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_itemIsSwitch(uint08 dstOptype, uint08 srcOptype)
{
	const tlkmmi_audio_schParam_t *pDstFunc;
	const tlkmmi_audio_schParam_t *pSrcFunc;

	if(dstOptype >= TLKPTI_AUD_OPTYPE_MAX) pDstFunc = nullptr;
	else pDstFunc = spTlkMMidAudioSchList[dstOptype];
	if(srcOptype >= TLKPTI_AUD_OPTYPE_MAX) pSrcFunc = nullptr;
	else pSrcFunc = spTlkMMidAudioSchList[srcOptype];
	if(pDstFunc == nullptr) return false;
	if(pSrcFunc == nullptr || pDstFunc->priority > pSrcFunc->priority) return true;
	return false;
}

/******************************************************************************
 * Function: tlkmmi_audio_removeItemByOptype
 * Descript: Remove the special audio status control block by it's opreate type.
 * Params: @optype[IN]--The operate type.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_removeItemByOptype(uint08 optype)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioSchCtrl.nowNumb; index++){
		if(sTlkMMidAudioSchCtrl.item[index].optype == optype){
			tlkmmi_audio_removeItem(sTlkMMidAudioSchCtrl.item[index].handle, optype);
			index = 0;
		}
	}
}

/******************************************************************************
 * Function: tlkmmi_audio_removeItemByHandle
 * Descript: Remove the special audio status control block by it's handle.
 * Params: @handle[IN]--The handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_removeItemByHandle(uint16 handle)
{
	uint08 index;
	for(index=0; index<sTlkMMidAudioSchCtrl.nowNumb; index++){
		if(sTlkMMidAudioSchCtrl.item[index].handle == handle){
			tlkmmi_audio_removeItem(handle, sTlkMMidAudioSchCtrl.item[index].optype);
			index = 0;
		}
	}
}

/******************************************************************************
 * Function: tlkmmi_audio_printfList
 * Descript: Print out the list of currently active audio.
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmmi_audio_printfList(void)
{
	uint08 index;
	tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "tlkmmi_audio_printfList: nowNumb-%d", sTlkMMidAudioSchCtrl.nowNumb);
	for(index=0; index<sTlkMMidAudioSchCtrl.nowNumb; index++){
		tlkapi_trace(TLKMMI_AUDIO_DBG_FLAG, TLKMMI_AUDIO_DBG_SIGN, "      item{index-%d,handle-%d,optype-%d,prio-%d}", index, sTlkMMidAudioSchCtrl.item[index].handle,
			sTlkMMidAudioSchCtrl.item[index].optype, sTlkMMidAudioSchCtrl.item[index].priority);
	}
}


#endif //#if (TLKMMI_AUDIO_ENABLE)

