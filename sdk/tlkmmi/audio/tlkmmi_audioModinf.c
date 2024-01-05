/********************************************************************************************************
 * @file	tlkmmi_audioModinf.c
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

#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkmdi/aud/tlkmdi_audhfp.h"
#include "tlkmdi/aud/tlkmdi_audsnk.h"
#include "tlkmdi/aud/tlkmdi_audsrc.h"
#include "tlkmdi/aud/tlkmdi_audsco.h"
#include "tlkmdi/aud/tlkmdi_audmp3.h"
#include "tlkmdi/aud/tlkmdi_auduac.h"
#include "tlkmdi/aud/tlkmdi_audplay.h"
#include "tlkmdi/aud/tlkmdi_audtone.h"
#include "tlkmdi/aud/tlkmdi_auddsco.h"


static const tlkmmi_audio_modinf_t *tlkmmi_audio_getModinf(TLKPTI_AUD_OPTYPE_ENUM optype);


/******************************************************************************
 * Function: tlkmmi_audio_modinfStart
 * Descript: Invoke the opening interface of the corresponding audio module to
 *     enable audio transmission.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 *     @handle[IN]--The connection handle of the current audio initiator.
 *     @param[IN]--Control parameters of the current audio initiator.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_modinfStart(TLKPTI_AUD_OPTYPE_ENUM optype, uint16 handle, uint32 param)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->Start == nullptr) return -TLK_ENOSUPPORT;
	else return pModinf->Start(handle, param);
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfClose
 * Descript: Invoke the closing interface of the corresponding audio module to
 *     disable audio transmission.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 *     @handle[IN]--The connection handle of the current audio initiator.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_modinfClose(TLKPTI_AUD_OPTYPE_ENUM optype, uint16 handle)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->Close == nullptr) return -TLK_ENOSUPPORT;
	else return pModinf->Close(handle);
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfTimer
 * Descript: Invoke the timing callback interface of the corresponding audio 
 *     module to input the timing control source for the audio module.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmmi_audio_modinfTimer(TLKPTI_AUD_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf != nullptr && pModinf->Timer != nullptr){
		pModinf->Timer();
	}
}

bool tlkmmi_audio_modinfFPlay(TLKPTI_AUD_OPTYPE_ENUM optype, bool isRewind, bool isStart)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf != nullptr && pModinf->FPlay != nullptr){
		return pModinf->FPlay(isRewind, isStart);
	}
	return false;
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfSwitch
 * Descript: Call the switching interface of the corresponding audio module to
 *     officially start or turn off the audio input or output.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 *     @handle[IN]--The connection handle of the current audio initiator.
 *     @status[IN]--The state of the audio control.
 *                  TLK_STATE_OPENED -- The audio is on.
 *                  TLK_STATE_CLOSED -- The audio is off.
 *                  TLK_STATE_PAUSED -- The audio is paused.
 * Return: Operating results. True means success, others means failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfSwitch(TLKPTI_AUD_OPTYPE_ENUM optype, uint16 handle, uint08 status)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->Switch == nullptr) return false;
	else return pModinf->Switch(handle, status);
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfToNext
 * Descript: Invoke the next track interface of the corresponding audio module
 *     to cause the audio to play the next track.
 * Params: 
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: Operating results. true means success, others means failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfToNext(TLKPTI_AUD_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->ToNext == nullptr) return false;
	else return pModinf->ToNext();
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfToPrev
 * Descript: Invoke the previous track interface of the corresponding audio 
 *     module to cause the audio to play the previous track.
 * Params: 
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: Operating results. True means success, others means failure.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfToPrev(TLKPTI_AUD_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->ToPrev == nullptr) return false;
	else return pModinf->ToPrev();
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfIsBusy
 * Descript: Check the status of the corresponding audio module.
 * Params: 
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: Operating results. True means audio is busy, others means audio is
 *         idle.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_modinfIsBusy(TLKPTI_AUD_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->IsBusy == nullptr) return false;
	else return pModinf->IsBusy();
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfIntval
 * Descript: Gets the interval time at which the next state of the current audio 
 *     arrives.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: The interval time at which the next state of the current audio 
 *     arrives.
 * Others: None.
*******************************************************************************/
uint tlkmmi_audio_modinfIntval(TLKPTI_AUD_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->Intval == nullptr) return 0;
	else return pModinf->Intval();
}

/******************************************************************************
 * Function: tlkmmi_audio_modinfIrqProc
 * Descript: Invoke the timing callback interface of the corresponding audio 
 *     module to input the timing control source for the audio module.
 * Params:
 *     @optype[IN]--Audio type to be operated. Refer TLKPTI_AUD_OPTYPE_ENUM.
 * Return: Operating results. True means success, others means failure.
 * Others: 
 *     tlkmmi_audio_modinfTimer -- Software timer. There's a big time error.
 *     tlkmmi_audio_modinfIrqProc -- Hardware timer, time accuracy is guaranteed.
*******************************************************************************/
bool tlkmmi_audio_modinfIrqProc(TLKPTI_AUD_OPTYPE_ENUM optype)
{
	const tlkmmi_audio_modinf_t *pModinf;
	pModinf = tlkmmi_audio_getModinf(optype);
	if(pModinf == nullptr || pModinf->IrqProc == nullptr) return false;
	else return pModinf->IrqProc();
}



#if (TLK_MDI_AUDTONE_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioToneModinf = {
	tlkmdi_audtone_start, //Start
	tlkmdi_audtone_close, //Close
	nullptr, //Timer
	nullptr, //FPlay
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_audtone_switch, //Switch
	tlkmdi_audtone_isBusy, //IsBusy
	tlkmdi_audtone_intval, //Intval
	tlkmdi_audtone_irqProc, //IrqProc
};
#endif //TLK_MDI_AUDTONE_ENABLE
#if (TLK_MDI_AUDPLAY_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioPlayModinf = {
	tlkmdi_audplay_start, //Start
	tlkmdi_audplay_close, //Close
	nullptr, //Timer
	tlkmdi_audplay_fPlay, //FPlay
	tlkmdi_audplay_toNext, //ToNext
	tlkmdi_audplay_toPrev, //ToNext
	tlkmdi_audplay_switch, //Switch
	tlkmdi_audplay_isBusy, //IsBusy
	tlkmdi_audplay_intval, //Intval
	tlkmdi_audplay_irqProc, //IrqProc
};
#endif //#if (TLK_MDI_AUDPLAY_ENABLE)
#if (TLK_MDI_AUDHFP_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioHfModinf = {
	nullptr, //Start
	nullptr, //Close
	nullptr, //Timer
	nullptr, //FPlay
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_audhfp_switch, //Switch
	tlkmdi_audhfp_isBusy, //IsBusy
	tlkmdi_audhfp_intval, //Intval
	tlkmdi_audhfp_irqProc, //IrqProc
};
static const tlkmmi_audio_modinf_t sTlkMMidAudioAgModinf = {
	nullptr, //Start
	nullptr, //Close
	nullptr, //Timer
	nullptr, //FPlay
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_audhfp_switch, //Switch
	tlkmdi_audhfp_isBusy, //IsBusy
	tlkmdi_audhfp_intval, //Intval
	tlkmdi_audhfp_irqProc, //IrqProc
};
#endif //#if (TLK_MDI_AUDHFP_ENABLE)
#if (TLK_MDI_AUDSCO_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioScoModinf = {
	nullptr, //Start
	nullptr, //Close
	nullptr, //Timer
	nullptr, //FPlay
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_audsco_switch, //Switch
	tlkmdi_audsco_isBusy, //IsBusy
	tlkmdi_audsco_intval, //Intval
	tlkmdi_audsco_irqProc, //IrqProc
};
#endif //#if (TLK_MDI_AUDSCO_ENABLE)
#if (TLK_MDI_AUDSRC_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioSrcModinf = {
	tlkmdi_audsrc_start, //Start
	tlkmdi_audsrc_close, //Close
	tlkmdi_audsrc_timer, //Timer
	tlkmdi_audsrc_fPlay, //FPlay
	tlkmdi_audsrc_toNext, //ToNext
	tlkmdi_audsrc_toPrev, //ToNext
	tlkmdi_audsrc_switch, //Switch
	tlkmdi_audsrc_isBusy, //IsBusy
	tlkmdi_audsrc_intval, //Intval
	tlkmdi_audsrc_irqProc, //IrqProc
};
#endif //#if (TLK_MDI_AUDSRC_ENABLE)
#if (TLK_MDI_AUDSNK_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioSnkModinf = {
	tlkmdi_audsnk_start, //Start
	tlkmdi_audsnk_close, //Close
	tlkmdi_audsnk_timer, //Timer
	tlkmdi_audsnk_fPlay, //FPlay
	tlkmdi_audsnk_toNext, //ToNext
	tlkmdi_audsnk_toPrev, //ToNext
	tlkmdi_audsnk_switch, //Switch
	tlkmdi_audsnk_isBusy, //IsBusy
	tlkmdi_audsnk_intval, //Intval
	tlkmdi_audsnk_irqProc, //IrqProc
};
#endif //#if (TLK_MDI_AUDSNK_ENABLE)
#if (TLK_MDI_AUDUAC_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMMidAudioUacModinf = {
	tlkmdi_auduac_start, //Start
	tlkmdi_auduac_close, //Close
	tlkmdi_auduac_timer, //Timer
	nullptr, //FPlay
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_auduac_switch, //Switch
	tlkmdi_auduac_isBusy, //IsBusy
	tlkmdi_auduac_intval, //Intval
	tlkmdi_auduac_irqProc, //IrqProc
};
#endif //#if (TLK_MDI_AUDUAC_ENABLE)
#if (TLK_MDI_AUDDSCO_ENABLE)
static const tlkmmi_audio_modinf_t sTlkMmiAudioDscoModinf = {
	nullptr, //Start
	nullptr, //Close
	nullptr, //Timer
	nullptr, //FPlay
	nullptr, //ToNext
	nullptr, //ToNext
	tlkmdi_auddsco_switch, //Switch
	tlkmdi_auddsco_isBusy, //IsBusy
	tlkmdi_auddsco_intval, //Intval
	tlkmdi_auddsco_irqProc, //IrqProc
};
#endif
static const tlkmmi_audio_modinf_t *spTlkMmiAudioModinfs[TLKPTI_AUD_OPTYPE_MAX] = {
	nullptr,
#if TLK_MDI_AUDTONE_ENABLE
	&sTlkMMidAudioToneModinf,
#else
	nullptr,
#endif
#if TLK_MDI_AUDPLAY_ENABLE
	&sTlkMMidAudioPlayModinf,
#else
	nullptr,
#endif
#if TLK_MDI_AUDHFP_ENABLE
	&sTlkMMidAudioHfModinf,
	&sTlkMMidAudioAgModinf,
#else
	nullptr,
	nullptr,
#endif
#if TLK_MDI_AUDSCO_ENABLE
	&sTlkMMidAudioScoModinf,
#else
	nullptr,
#endif
#if TLK_MDI_AUDSRC_ENABLE
	&sTlkMMidAudioSrcModinf,
#else
	nullptr,
#endif
#if TLK_MDI_AUDSNK_ENABLE
	&sTlkMMidAudioSnkModinf,
#else
	nullptr,
#endif
#if TLK_MDI_AUDUAC_ENABLE
	&sTlkMMidAudioUacModinf,
#else
	nullptr,
#endif
#if (TLK_MDI_AUDDSCO_ENABLE)
	&sTlkMmiAudioDscoModinf,
#else
	nullptr,
#endif
};

/******************************************************************************
 * Function: tlkmmi_audio_getModinf
 * Descript: Get the interface of Tone module 
 * Params:@optype[IN]--The optype of audio.
 * Return: The interface function means success, others means failure.
 * Others: None.
*******************************************************************************/
static const tlkmmi_audio_modinf_t *tlkmmi_audio_getModinf(TLKPTI_AUD_OPTYPE_ENUM optype)
{
	if(optype >= TLKPTI_AUD_OPTYPE_MAX) return nullptr;
	return spTlkMmiAudioModinfs[optype];
}

#endif //#if (TLKMMI_AUDIO_ENABLE)
