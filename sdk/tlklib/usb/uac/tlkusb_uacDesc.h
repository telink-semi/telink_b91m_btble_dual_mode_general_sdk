/********************************************************************************************************
 * @file	tlkusb_uacDesc.h
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
#ifndef TLKUSB_AUDIO_DESC_H
#define TLKUSB_AUDIO_DESC_H

#if (TLK_USB_UAC_ENABLE)



#define TLKUSB_AUD_STRING_PRODUCT       L"Telink B91 BTBLE Audio"

#define TLKUSB_AUD_STRING_SERIAL0       L"TLSR-BTBLE-MIC"
#define TLKUSB_AUD_STRING_SERIAL1       L"TLSR-BTBLE-SPK"
#define TLKUSB_AUD_STRING_SERIAL2       L"TLSR-BTBLE-MIC-SPK"


typedef struct {
	tlkusb_stdConfigureDesc_t config;
	tlkusb_stdInterfaceDesc_t audCtrInf;
	//control interface
	#if (TLKUSB_UAC_MIC_ENABLE && TLKUSB_UAC_SPK_ENABLE)
	tlkusb_uacInterfaceAcTLDesc_t audCtrAcInf;
	#else
	tlkusb_uacInterfaceAcDesc_t audCtrAcInf;
	#endif
	#if (TLKUSB_UAC_SPK_ENABLE)
	//Input Terminal
	tlkusb_uacInputDesc_t      spkInputDesc;
	//Feature Unit
	#if (TLKUSB_AUD_SPK_CHANNEL_COUNT == 2)
	tlkusb_uacDoubleFeatureDesc_t spkFeatureDesc;
	#else
	tlkusb_uacSingleFeatureDesc_t spkFeatureDesc;
	#endif
	//Output Terminal
	tlkusb_uacOutputDesc_t     spkOutputDesc;
	#endif
	#if (TLKUSB_UAC_MIC_ENABLE)
	//Input Terminal
	tlkusb_uacInputDesc_t      micInputDesc;
	//Feature Unit
	#if (TLKUSB_AUD_MIC_CHANNEL_COUNT == 2)
	tlkusb_uacDoubleFeatureDesc_t micFeatureDesc;
	#else
	tlkusb_uacSingleFeatureDesc_t micFeatureDesc;
	#endif
	//Output Terminal
	tlkusb_uacOutputDesc_t     micOutputDesc;
	#endif
	//audio streaming interface
	#if (TLKUSB_UAC_SPK_ENABLE)
	tlkusb_stdInterfaceDesc_t   spkSetting0Inf;
	tlkusb_stdInterfaceDesc_t   spkSetting1Inf;
	tlkusb_uacInterfaceAsDesc_t spkDatAsInf;
	//audio format
	tlkusb_uacFormatDesc_t      spkFormatDesc;
	tlkusb_uacStdEndpointDesc_t spkStdEdpDesc;
	tlkusb_uacSpcEndpointDesc_t spkSpcEdpDesc;
	#endif
	#if (TLKUSB_UAC_MIC_ENABLE)
	tlkusb_stdInterfaceDesc_t   micSetting0Inf;
	tlkusb_stdInterfaceDesc_t   micSetting1Inf;
	tlkusb_uacInterfaceAsDesc_t micDatAsInf;
	//audio format
	tlkusb_uacFormatDesc_t      micFormatDesc;
	tlkusb_uacStdEndpointDesc_t micStdEdpDesc;
	tlkusb_uacSpcEndpointDesc_t micSpcEdpDesc;
	#endif
}tlkusb_uacAudConfigDesc_t;







#endif //#if (TLK_USB_UAC_ENABLE)

#endif //TLKUSB_AUDIO_DESC_H

