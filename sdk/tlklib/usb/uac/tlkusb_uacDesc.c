/********************************************************************************************************
 * @file	tlkusb_uacDesc.c
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
#include "tlklib/usb/tlkusb_stdio.h"
#if (TLK_USB_UAC_ENABLE)
#include "tlklib/usb/tlkusb_desc.h"
#include "tlklib/usb/uac/tlkusb_uacDefine.h"
#include "tlklib/usb/uac/tlkusb_uacDesc.h"
#include "tlklib/usb/uac/tlkusb_uac.h"
#include "tlklib/usb/uac/tlkusb_uacSpk.h"
#include "tlklib/usb/uac/tlkusb_uacMic.h"


static uint16 tlkusb_uacdesc_getConfigLens(void);
static uint16 tlkusb_uacdesc_getStringLens(uint08 index);
static uint08 *tlkusb_uacdesc_getConfigDesc(void);
static uint08 *tlkusb_uacdesc_getStringDesc(uint08 index);

extern uint32 sTlkUsbUacSamplRate;

const tlkusb_modDesc_t sTlkUsbUacModDesc = {
	nullptr, //GetDeviceLens
	tlkusb_uacdesc_getConfigLens, //GetConfigLens
	tlkusb_uacdesc_getStringLens, //GetStringLens
	nullptr, //GetReportLens
	nullptr, //GetDeviceDesc
	tlkusb_uacdesc_getConfigDesc, //GetConfigDesc
	tlkusb_uacdesc_getStringDesc, //GetStringDesc
	nullptr, //GetReportDesc
};




/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
static const tlkusb_stdStringDesc_t sMmiUsbAudProductDesc = {
	2+sizeof(TLKUSB_AUD_STRING_PRODUCT)-2, //-2 is the end of the string
	TLKUSB_TYPE_STRING, // Header
	TLKUSB_AUD_STRING_PRODUCT 
};

/** Serial number string. This is a Unicode string containing the device's unique serial number, expressed as a
 *  series of uppercase hexadecimal digits.
 */
#if (TLKUSB_UAC_MIC_ENABLE && TLKUSB_UAC_SPK_ENABLE)
static const tlkusb_stdStringDesc_t sMmiUsbAudMicSpkSerialDesc = {
	2+sizeof(TLKUSB_AUD_STRING_SERIAL2)-2,
	TLKUSB_TYPE_STRING,
	TLKUSB_AUD_STRING_SERIAL2
};
#elif (TLKUSB_UAC_MIC_ENABLE)
static const tlkusb_stdStringDesc_t sMmiUsbAudMicSerialDesc = {
	2+sizeof(TLKUSB_AUD_STRING_SERIAL0)-2,
	TLKUSB_TYPE_STRING,
	TLKUSB_AUD_STRING_SERIAL0
};
#elif (TLKUSB_UAC_SPK_ENABLE)
static const tlkusb_stdStringDesc_t sMmiUsbAudSpkSerialDesc = {
	2+sizeof(TLKUSB_AUD_STRING_SERIAL1)-2,
	TLKUSB_TYPE_STRING,
	TLKUSB_AUD_STRING_SERIAL1
};
#endif



static const tlkusb_uacAudConfigDesc_t sMmiUsbAudConfigDesc = {
	{
		sizeof(tlkusb_stdConfigureDesc_t),// Length
		TLKUSB_TYPE_CONFIGURE,  //Type
		sizeof(tlkusb_uacAudConfigDesc_t), // TotalLength: variable
		TLKUSB_AUD_INF_MAX, // NumInterfaces
		1, // Configuration index
		TLKUSB_NO_DESCRIPTOR, // Configuration String
		TLKUSB_CFG_ATTR_RESERVED, // Attributes
		TLKUSB_CONFIG_POWER(100) // MaxPower = 100mA
	},
	// audio_control_interface
	{
		sizeof(tlkusb_stdInterfaceDesc_t),// Length
		TLKUSB_TYPE_INTERFACE,//Type
		TLKUSB_AUD_INF_CTR,	//InterfaceNumber
		0, // AlternateSetting
		0, // bNumEndpoints
		TLKUSB_AUD_CSCP_AudioClass, // bInterfaceclass ->Printer
		TLKUSB_AUD_CSCP_ControlSubclass, // bInterfaceSubClass -> Control
		TLKUSB_AUD_CSCP_ControlProtocol, // bInterfaceProtocol
		TLKUSB_NO_DESCRIPTOR // iInterface
	},
	// audio_control_interface_ac;
	#if (TLKUSB_UAC_MIC_ENABLE && TLKUSB_UAC_SPK_ENABLE)
	{
		sizeof(tlkusb_uacInterfaceAcTLDesc_t),	//Length
		TLKUSB_TYPE_CS_INTERFACE,	//Type
		AUDIO_DSUBTYPE_CSInterface_Header, // Subtype
		{0x00, 0x01}, // ACSpecification, version == 1.0
		{
			(sizeof(tlkusb_uacInterfaceAcTLDesc_t) + /*9*/
			sizeof(tlkusb_uacInputDesc_t) + /*12*/
			sizeof(tlkusb_uacOutputDesc_t) + /*9*/
			#if (TLKUSB_AUD_SPK_CHANNEL_COUNT == 2)
			sizeof(tlkusb_uacDoubleFeatureDesc_t) + /*10*/
			#else
			sizeof(tlkusb_uacSingleFeatureDesc_t) + /*9*/
			#endif
			sizeof(tlkusb_uacInputDesc_t) + /*12*/
			sizeof(tlkusb_uacOutputDesc_t) + /*9*/
			#if (TLKUSB_AUD_MIC_CHANNEL_COUNT == 2)
			sizeof(tlkusb_uacDoubleFeatureDesc_t) /*10*/
			#else
			sizeof(tlkusb_uacSingleFeatureDesc_t) /*9*/
			#endif
			),
			0
		},
		2,	//InCollection
		TLKUSB_AUD_INF_SPK,	//InterfaceNumber_spk
		TLKUSB_AUD_INF_MIC	//InterfaceNumber_mic
	},
	#elif (TLKUSB_UAC_MIC_ENABLE)
	{
		sizeof(tlkusb_uacInterfaceAcDesc_t),//Length
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_Header, // Subtype
		{0x00, 0x01}, // ACSpecification, version == 1.0
		{
			(sizeof(tlkusb_uacInterfaceAcDesc_t) + /*9*/
			sizeof(tlkusb_uacInputDesc_t) + /*12*/
			sizeof(tlkusb_uacOutputDesc_t) + /*9*/
			#if (TLKUSB_AUD_MIC_CHANNEL_COUNT == 2)
			sizeof(tlkusb_uacDoubleFeatureDesc_t) /*10*/
			#else
			sizeof(tlkusb_uacSingleFeatureDesc_t) /*9*/
			#endif
			),
			0
		},
		1,	//InCollection
		TLKUSB_AUD_INF_MIC	//InterfaceNumber_mic
	},
	#elif (TLKUSB_UAC_SPK_ENABLE)
	{
		sizeof(tlkusb_uacInterfaceAcDesc_t),
		TLKUSB_TYPE_CS_INTERFACE,
		AUDIO_DSUBTYPE_CSInterface_Header, // Subtype
		{0x00, 0x01}, // ACSpecification, version == 1.0
		{(sizeof(tlkusb_uacInterfaceAcDesc_t) + /*9*/
			sizeof(tlkusb_uacInputDesc_t) + /*12*/
			sizeof(tlkusb_uacOutputDesc_t) + /*9*/
			#if (TLKUSB_AUD_SPK_CHANNEL_COUNT == 2)
			sizeof(tlkusb_uacDoubleFeatureDesc_t) /*10*/
			#else
			sizeof(tlkusb_uacSingleFeatureDesc_t) /*9*/
			#endif
			),
			0
		},
		1,	//InCollection
		TLKUSB_AUD_INF_SPK	//InterfaceNumber_spk
	},
	#endif
	#if (TLKUSB_UAC_SPK_ENABLE)
	// spk_input_terminal
	{
		sizeof(tlkusb_uacInputDesc_t),//Length
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_InputTerminal,//Subtype
		TLKUSB_AUDID_SPK_INPUT_TERMINAL_ID,//TerminalID
		AUDIO_TERMINAL_STREAMING,//TerminalType
		0, // AssociatedOutputTerminal
		TLKUSB_AUD_SPK_CHANNEL_COUNT, // TotalChannels
		#if (TLKUSB_AUD_SPK_CHANNEL_COUNT == 2)
		0x0000, //0x0003, // Channel Config bitmap(left and right)
		#else
		0x0000, //0x0001, // ChannelConfig - mono
		#endif
		0, // ChannelStrIndex
		TLKUSB_NO_DESCRIPTOR
	},
	// spk_feature_unit
	{
		#if (TLKUSB_AUD_SPK_CHANNEL_COUNT == 2)
		sizeof(tlkusb_uacDoubleFeatureDesc_t),
		#else
		sizeof(tlkusb_uacSingleFeatureDesc_t),//Length
		#endif
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_Feature,//Subtype
		TLKUSB_AUDID_SPK_FEATURE_UNIT_ID,//UnitID
		TLKUSB_AUD_SRCID_SPK_FEATURE_UNIT,//SourceID
		1, // bControlSize
		#if (TLKUSB_AUD_SPK_CHANNEL_COUNT == 2)
		{0x03, 0x00, 0x00}, // bmaControls
		#else
		{0x03, 0x00}, // bmaControls
		#endif
		TLKUSB_NO_DESCRIPTOR
	},
	// spk_output_terminal
	{	
		sizeof(tlkusb_uacOutputDesc_t),//Length
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_OutputTerminal,//Subtype
		TLKUSB_AUDID_SPK_OUTPUT_TERMINAL_ID,//TerminalID
		AUDIO_TERMINAL_OUT_SPEAKER,//TerminalType
		0, // AssociatedOutputTerminal
		TLKUSB_AUD_SRCID_SPK_OUTPUT_TERMINAL,//SourceID
		TLKUSB_NO_DESCRIPTOR
	},
	#endif //#if (TLKUSB_UAC_SPK_ENABLE)
	#if (TLKUSB_UAC_MIC_ENABLE)
	// mic_input_terminal
	{	
		sizeof(tlkusb_uacInputDesc_t),//Length
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_InputTerminal,//Subtype
		TLKUSB_AUDID_MIC_INPUT_TERMINAL_ID,//TerminalID
		AUDIO_TERMINAL_IN_MIC,//TerminalType
		0, // AssociatedOutputTerminal
		TLKUSB_AUD_MIC_CHANNEL_COUNT, // TotalChannels
		#if (TLKUSB_AUD_MIC_CHANNEL_COUNT == 2)
		0x0000, //0x0003, // ChannelConfig - stero
		#else
		0x0001, //0x0001, // ChannelConfig - mono
		#endif
		0, // ChannelStrIndex
		TLKUSB_NO_DESCRIPTOR
	},
	// mic_feature_unit
	{
		#if (TLKUSB_AUD_MIC_CHANNEL_COUNT == 2)
		sizeof(tlkusb_uacDoubleFeatureDesc_t),
		#else
		sizeof(tlkusb_uacSingleFeatureDesc_t),//Length
		#endif
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_Feature,//Subtype
		TLKUSB_AUDID_MIC_FEATURE_UNIT_ID,//UnitID
		TLKUSB_AUD_SRCID_MIC_FEATURE_UNIT,//SourceID
		1, // bControlSize
		#if (TLKUSB_AUD_MIC_CHANNEL_COUNT == 2)
		{0x03, 0x00, 0x00}, // bmaControls
		#else
		{0x03, 0x00}, // bmaControls
		#endif
		TLKUSB_NO_DESCRIPTOR
	},
	// mic_output_terminal
	{	
		sizeof(tlkusb_uacOutputDesc_t),//Length
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_OutputTerminal,//Subtype
		TLKUSB_AUDID_MIC_OUTPUT_TERMINAL_ID,//TerminalID
		AUDIO_TERMINAL_STREAMING,//TerminalType
		0, // AssociatedOutputTerminal
		TLKUSB_AUD_SRCID_MIC_OUTPUT_TERMINAL,//SourceID
		TLKUSB_NO_DESCRIPTOR
	},
	#endif //#if (TLKUSB_UAC_MIC_ENABLE)
	#if (TLKUSB_UAC_SPK_ENABLE)
	// spk_setting0
	{	
		sizeof(tlkusb_stdInterfaceDesc_t),//Length
		TLKUSB_TYPE_INTERFACE,//Type
		TLKUSB_AUD_INF_SPK,//InterfaceNumber
		0, // AlternateSetting
		0, // bNumEndpoints
		TLKUSB_AUD_CSCP_AudioClass,//Class
		TLKUSB_AUD_CSCP_AudioStreamingSubclass,//Subclass
		TLKUSB_AUD_CSCP_StreamingProtocol,//Protocol
		TLKUSB_NO_DESCRIPTOR
	},
	// spk_setting1
	{
		sizeof(tlkusb_stdInterfaceDesc_t),//Length
		TLKUSB_TYPE_INTERFACE,//Type
		TLKUSB_AUD_INF_SPK,//InterfaceNumber
		1, // AlternateSetting
		1, // bNumEndpoints
		TLKUSB_AUD_CSCP_AudioClass,//Class
		TLKUSB_AUD_CSCP_AudioStreamingSubclass,//Subclass
		TLKUSB_AUD_CSCP_StreamingProtocol,//Protocol
		TLKUSB_NO_DESCRIPTOR
	},
	// spk_audio_stream
	{
		sizeof(tlkusb_uacInterfaceAsDesc_t),//Length
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_General,//Subtype
		1, // bTerminalLink: #1 USB Streaming IN
		0, // bDelay
		{ USB_AUDIO_FORMAT_PCM & 0xff, (USB_AUDIO_FORMAT_PCM >> 8)& 0xff}//StdioFormat
	},
	// spk_audio_format
	{
		sizeof(tlkusb_uacFormatDesc_t),//Length
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_FormatType,//Subtype
		USB_AUDIO_FORMAT_PCM, // FormatType
		TLKUSB_AUD_SPK_CHANNEL_COUNT, // Channels
		2, // SubFrameSize
		TLKUSB_AUD_SPK_RESOLUTION_BIT, // BitsResolution
		1, // TotalDiscreteSampleRates
		{ // spk_sample_rate
			(TLKUSB_AUD_SPK_SAMPLE_RATE & 0xff),
			((TLKUSB_AUD_SPK_SAMPLE_RATE & 0xFF00) >> 8),
			((TLKUSB_AUD_SPK_SAMPLE_RATE & 0xFF0000) >> 16)
		}
	},
	// spk_stream_endpoint
	{	
		sizeof(tlkusb_uacStdEndpointDesc_t), //Length
		TLKUSB_TYPE_ENDPOINT, //Type
		TLKUSB_EDP_DIR_OUT | TLKUSB_UAC_EDP_SPK,//Endpoint
		TLKUSB_EDP_TYPE_ISOCHRONOUS | (TLKUSB_EDP_SYNC_TYPE_SYNC << 2) | (TLKUSB_EDP_USAGE_TYPE_DATA << 4), // Attributes
		TLKUSB_AUD_SPK_CHANNEL_LENGTH,//MaxPacketSize
		1, // PollingIntervalMS
		0, // Refresh
		0 // SyncEndpointNumber
	},
	// spk_stream_endpoint_spc
	{
		sizeof(tlkusb_uacSpcEndpointDesc_t),//Length
		TLKUSB_TYPE_CS_ENDPOINT,	//Type
		AUDIO_DSUBTYPE_CSInterface_General,	//Subtype
		AUDIO_EP_FULL_PACKETS_ONLY | AUDIO_EP_SAMPLE_FREQ_CONTROL,//Attributes
		0, // LockDelayUnits
		{0, 0} // LockDelay
	},
	#endif //#if (TLKUSB_UAC_SPK_ENABLE)
	#if (TLKUSB_UAC_MIC_ENABLE)
	// mic_setting0
	{	
		sizeof(tlkusb_stdInterfaceDesc_t),//Length
		TLKUSB_TYPE_INTERFACE,//Type
		TLKUSB_AUD_INF_MIC,//InterfaceNumber
		0, // AlternateSetting
		0, // bNumEndpoints
		TLKUSB_AUD_CSCP_AudioClass,//Class
		TLKUSB_AUD_CSCP_AudioStreamingSubclass,//Subclass
		TLKUSB_AUD_CSCP_StreamingProtocol,//Protocol
		TLKUSB_NO_DESCRIPTOR
	},
	// mic_setting1
	{
		sizeof(tlkusb_stdInterfaceDesc_t),//Length
		TLKUSB_TYPE_INTERFACE,//Type
		TLKUSB_AUD_INF_MIC,//InterfaceNumber
		1, // AlternateSetting
		1, // bNumEndpoints
		TLKUSB_AUD_CSCP_AudioClass,//Class
		TLKUSB_AUD_CSCP_AudioStreamingSubclass,//Subclass
		TLKUSB_AUD_CSCP_StreamingProtocol,//Protocol
		TLKUSB_NO_DESCRIPTOR
	},
	// mic_audio_stream
	{
		sizeof(tlkusb_uacInterfaceAsDesc_t),//Length
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_General,//Subtype
		6, // bTerminalLink: #6 USB Streaming OUT
		1, // FrameDelay
		{ USB_AUDIO_FORMAT_PCM & 0xff, (USB_AUDIO_FORMAT_PCM >> 8)& 0xff}//StdioFormat
	},
	// mic_audio_format
	{
		sizeof(tlkusb_uacFormatDesc_t),//Length
		TLKUSB_TYPE_CS_INTERFACE,//Type
		AUDIO_DSUBTYPE_CSInterface_FormatType,//Subtype
		USB_AUDIO_FORMAT_PCM, // FormatType
		TLKUSB_AUD_MIC_CHANNEL_COUNT, // Channels
		2, // SubFrameSize
		TLKUSB_AUD_MIC_RESOLUTION_BIT, // BitsResolution
		1, // TotalDiscreteSampleRates
		{ // mic_sample_rate
			(TLKUSB_AUD_MIC_SAMPLE_RATE & 0xff),
			(TLKUSB_AUD_MIC_SAMPLE_RATE >> 8),
			0x00
		}
	},
	// mic_stream_endpoint
	{	
		sizeof(tlkusb_uacStdEndpointDesc_t), //Length
		TLKUSB_TYPE_ENDPOINT, //Type
		TLKUSB_EDP_DIR_IN | TLKUSB_UAC_EDP_MIC,//Endpoint
		TLKUSB_EDP_TYPE_ISOCHRONOUS | (TLKUSB_EDP_SYNC_TYPE_SYNC << 2) | (TLKUSB_EDP_USAGE_TYPE_DATA << 4), // Attributes
		TLKUSB_AUD_MIC_CHANNEL_LENGTH,//MaxPacketSize
		1, // PollingIntervalMS
		0, // Refresh
		0 // SyncEndpointNumber
	},
	// mic_stream_endpoint_spc
	{
		sizeof(tlkusb_uacSpcEndpointDesc_t),//Length
		TLKUSB_TYPE_CS_ENDPOINT,//Type
		AUDIO_DSUBTYPE_CSInterface_General,//Subtype
		AUDIO_EP_SAMPLE_FREQ_CONTROL,//Attributes
		0, // LockDelayUnits
		{0, 0} // LockDelay
	},
	#endif //#if (TLKUSB_UAC_MIC_ENABLE)
};



static uint16 tlkusb_uacdesc_getConfigLens(void)
{
	return sizeof(tlkusb_uacAudConfigDesc_t);
}
static uint16 tlkusb_uacdesc_getStringLens(uint08 index)
{
	if(index == TLKUSB_STRING_INDEX_PRODUCT){
		return sizeof(TLKUSB_AUD_STRING_PRODUCT);
	}else if(index == TLKUSB_STRING_INDEX_SERIAL){
		#if (TLKUSB_UAC_MIC_ENABLE && TLKUSB_UAC_SPK_ENABLE)
			return sizeof(TLKUSB_AUD_STRING_SERIAL2);
		#elif (TLKUSB_UAC_MIC_ENABLE)
			return sizeof(TLKUSB_AUD_STRING_SERIAL0);
		#elif (TLKUSB_UAC_SPK_ENABLE) 
			return sizeof(TLKUSB_AUD_STRING_SERIAL1);
		#else
			return 0;
		#endif
	}else{
		return 0;
	}
}

static uint08 *tlkusb_uacdesc_getConfigDesc(void)
{
	return (uint08*)(&sMmiUsbAudConfigDesc);
}
static uint08 *tlkusb_uacdesc_getStringDesc(uint08 index)
{
	if(index == TLKUSB_STRING_INDEX_PRODUCT){
		return (uint08*)(&sMmiUsbAudProductDesc);
	}else if(index == TLKUSB_STRING_INDEX_SERIAL){
		#if (TLKUSB_UAC_MIC_ENABLE && TLKUSB_UAC_SPK_ENABLE)
			return (uint08*)(&sMmiUsbAudMicSpkSerialDesc);
		#elif (TLKUSB_UAC_MIC_ENABLE)
			return (uint08*)(&sMmiUsbAudMicSerialDesc);
		#elif (TLKUSB_UAC_SPK_ENABLE) 
			return (uint08*)(&sMmiUsbAudSpkSerialDesc);
		#else
			return nullptr;
		#endif
	}else{
		return 0;
	}
}



#endif //#if (TLK_USB_UAC_ENABLE)


