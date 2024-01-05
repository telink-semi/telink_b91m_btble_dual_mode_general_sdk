/********************************************************************************************************
 * @file	tlkusb_cdcDesc.c
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
#if (TLK_USB_CDC_ENABLE)
#include "tlklib/usb/cdc/tlkusb_cdcConfig.h"
#include "tlklib/usb/cdc/tlkusb_cdcDefine.h"
#include "tlklib/usb/cdc/tlkusb_cdc.h"
#include "tlklib/usb/cdc/tlkusb_cdcDesc.h"
#include "tlklib/usb/cdc/tlkusb_cdcCtrl.h"


static uint16 tlkusb_cdcdesc_getDeviceLens(void);
static uint16 tlkusb_cdcdesc_getConfigLens(void);
static uint16 tlkusb_cdcdesc_getStringLens(uint08 index);
static uint16 tlkusb_cdcdesc_getInfDesLens(tlkusb_setup_req_t *pSetup);
static uint08 *tlkusb_cdcdesc_getDeviceDesc(void);
static uint08 *tlkusb_cdcdesc_getConfigDesc(void);
static uint08 *tlkusb_cdcdesc_getStringDesc(uint08 index);
static uint08 *tlkusb_cdcdesc_getInfDesDesc(tlkusb_setup_req_t *pSetup);


const tlkusb_modDesc_t sTlkUsbCdcModDesc = {
	tlkusb_cdcdesc_getDeviceLens, //GetDeviceLens
	tlkusb_cdcdesc_getConfigLens, //GetConfigLens
	tlkusb_cdcdesc_getStringLens, //GetStringLens
	tlkusb_cdcdesc_getInfDesLens, //GetReportLens
	tlkusb_cdcdesc_getDeviceDesc, //GetDeviceDesc
	tlkusb_cdcdesc_getConfigDesc, //GetConfigDesc
	tlkusb_cdcdesc_getStringDesc, //GetStringDesc
	tlkusb_cdcdesc_getInfDesDesc, //GetReportDesc
};



static const tlkusb_stdStringDesc_t sMmiUsbCdcProductDesc = {
	2+sizeof(TLKUSB_CDC_STRING_PRODUCT)-2, //-2 is the end of the string
	TLKUSB_TYPE_STRING, // Header
	TLKUSB_CDC_STRING_PRODUCT 
};

/** Serial number string. This is a Unicode string containing the device's unique serial number, expressed as a
 *  series of uppercase hexadecimal digits.
 */
static const tlkusb_stdStringDesc_t sMmiUsbCdcSerialDesc = {
	2+sizeof(TLKUSB_CDC_STRING_SERIAL)-2,
	TLKUSB_TYPE_STRING,
	TLKUSB_CDC_STRING_SERIAL
};



#if (TLK_USB_CDC_ENABLE)
static const tlkusb_stdDeviceDesc_t sMmiUsbCdcDeviceDesc = { 
	sizeof(tlkusb_stdDeviceDesc_t), //Length
	TLKUSB_TYPE_DEVICE , // Header
	0x0110,
	USB_CSCP_NoDeviceClass, // Class
	USB_CSCP_NoDeviceSubclass, // SubClass
	USB_CSCP_NoDeviceProtocol, // Protocol
	8, // Endpoint0Size, Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64
	TLKUSB_ID_VENDOR, // VendorID
	0x9204, //TLKUSB_ID_PRODUCT, // ProductID
	TLKUSB_ID_VERSION/*0x0100*/, // .ReleaseNumber
	TLKUSB_STRING_INDEX_VENDOR,  // .ManufacturerStrIndex
	TLKUSB_STRING_INDEX_PRODUCT, // .ProductStrIndex
	TLKUSB_STRING_INDEX_SERIAL,  // .SerialNumStrIndex, iSerialNumber
	1
};
static const tlkusb_cdcConfigDesc_t sMmiUsbCdcConfigDesc = {
	{
		sizeof(tlkusb_stdConfigureDesc_t),//Length
		TLKUSB_TYPE_CONFIGURE, // Type
		sizeof(tlkusb_cdcConfigDesc_t), // TotalLength: variable
		TLKUSB_CDC_INF_MAX, // NumInterfaces
		1, // Configuration index
		TLKUSB_NO_DESCRIPTOR, // Configuration String
		TLKUSB_CFG_ATTR_RESERVED, // Attributes
		TLKUSB_CONFIG_POWER(100) // MaxPower = 100mA
	},
#if (1)
	{
		// iad0
		sizeof(tlkusb_stdAssociateDesc_t),//Length
		TLKUSB_TYPE_ASSOCIATE, // Header
		TLKUSB_CDC_INF_CCI, // FirstInterfaceIndex
		2, // TotalInterface
		TLKUSB_CDC_CSCP_SubClass, // Class
		TLKUSB_CDC_CSCP_ACM_SubClass, // Subclass
		TLKUSB_CDC_CSCP_ATCmd_Protocol, // protocol
		TLKUSB_NO_DESCRIPTOR  // IADStrIndex
	},
	{
		// cdc_interface
		sizeof(tlkusb_stdInterfaceDesc_t), //Length
		TLKUSB_TYPE_INTERFACE, // Header
		TLKUSB_CDC_INF_CCI, // InterfaceNumber
		0, // AlternateSetting
		1, // TotalEndpoints
		TLKUSB_CDC_CSCP_SubClass, // Class
		TLKUSB_CDC_CSCP_ACM_SubClass, // Subclass
		TLKUSB_CDC_CSCP_ATCmd_Protocol, // protocol
		TLKUSB_NO_DESCRIPTOR //InterfaceStrIndex
	},
	{
		// cdc_descriptor
		//CDC_Functional_Header
		{
			sizeof(tlkusb_cdcFunctionHead_t),//Length
			TLKUSB_TYPE_CS_INTERFACE, // Header
			TLKUSB_CDC_TYPE_CSInterface_Header, // Subtype
			0x0110 // CDCSpecification
		},
		// CDC_Functional_ACM =
		{
			sizeof(tlkusb_cdcFunctionAcm_t),	//Length
			TLKUSB_TYPE_CS_INTERFACE, // Header
			TLKUSB_CDC_TYPE_CSInterface_ACM, // Subtype
			0x02 // Capabilities
		},
		// CDC_Functional_Union =
		{
			sizeof(tlkusb_cdcFunctionUnion_t),	//Length
			TLKUSB_TYPE_CS_INTERFACE, // Header
			TLKUSB_CDC_TYPE_CSInterface_Union, // Subtype
			0, // MasterInterfaceNumber
			1, // SlaveInterfaceNumber
		},
		// CDC_CallManagement =
		{
			sizeof(tlkusb_cdcFunctionUnion_t),	//Length
			TLKUSB_TYPE_CS_INTERFACE, // Header
			TLKUSB_CDC_TYPE_CSInterface_CallManagement, // Subtype
			0, // MasterInterfaceNumber
			1, // SlaveInterfaceNumber
		},
		// CDC_NotificationEndpoint =
		{
			sizeof(tlkusb_stdEndpointDesc_t),	//Length
			TLKUSB_TYPE_ENDPOINT, // Header
			(TLKUSB_EDP_DIR_IN | TLKUSB_CDC_EDP_NOTY), // EndpointAddress
			(TLKUSB_EDP_TYPE_INTERRUPT | TLKUSB_EDP_ATTR_NO_SYNC | TLKUSB_EDP_USAGE_DATA), // Attributes
			TLKUSB_CDC_NOTIFY_EDPSIZE, // EndpointSize
			0x40 // PollingIntervalMS
		},
		// CDC_DCI_Interface =
		{
			sizeof(tlkusb_stdInterfaceDesc_t),	//Length
			TLKUSB_TYPE_INTERFACE, // Header
			TLKUSB_CDC_INF_DCI, // InterfaceNumber
			0, // AlternateSetting
			2, // TotalEndpoints
			TLKUSB_CDC_CSCP_SubClass, // Class
			TLKUSB_CDC_CSCP_ACM_SubClass, // Subclass
			TLKUSB_CDC_CSCP_ATCmd_Protocol, // protocol
			TLKUSB_NO_DESCRIPTOR // InterfaceStrIndex
		},
		// CDC_DataOutEndpoint =
		{
			sizeof(tlkusb_stdEndpointDesc_t),	//Length
			TLKUSB_TYPE_ENDPOINT, // Header
			(TLKUSB_EDP_DIR_OUT | TLKUSB_CDC_EDP_RX), // EndpointAddress
			(TLKUSB_EDP_TYPE_BULK | TLKUSB_EDP_ATTR_NO_SYNC | TLKUSB_EDP_USAGE_DATA), // Attributes
			TLKUSB_CDC_TXRX_EDPSIZE, // EndpointSize
			0x00 // PollingIntervalMS
		},
		// CDC_DataInEndpoint =
		{
			sizeof(tlkusb_stdEndpointDesc_t),	//Length
			TLKUSB_TYPE_ENDPOINT, // Header
			(TLKUSB_EDP_DIR_IN | TLKUSB_CDC_EDP_TX), // EndpointAddress
			(TLKUSB_EDP_TYPE_BULK | TLKUSB_EDP_ATTR_NO_SYNC | TLKUSB_EDP_USAGE_DATA), // Attributes
			TLKUSB_CDC_TXRX_EDPSIZE, // EndpointSize
			0x00 // PollingIntervalMS
		},
	},
#endif
#if (TLKUSB_CDC_SECOND_ENABLE)
	{
		// iad0
		sizeof(tlkusb_stdAssociateDesc_t),	//Length
		TLKUSB_TYPE_ASSOCIATE, // Header
		TLKUSB_CDC2_INF_CCI, // FirstInterfaceIndex
		2, // TotalInterface
		TLKUSB_CDC_CSCP_SubClass, // Class
		TLKUSB_CDC_CSCP_ACM_SubClass, // Subclass
		TLKUSB_CDC_CSCP_ATCmd_Protocol, // protocol
		TLKUSB_NO_DESCRIPTOR  // IADStrIndex
	},
	{
		// cdc_interface
		sizeof(tlkusb_stdInterfaceDesc_t),	//Length
		TLKUSB_TYPE_INTERFACE, // Header
		TLKUSB_CDC2_INF_CCI, // InterfaceNumber
		0, // AlternateSetting
		1, // TotalEndpoints
		TLKUSB_CDC_CSCP_SubClass, // Class
		TLKUSB_CDC_CSCP_ACM_SubClass, // Subclass
		TLKUSB_CDC_CSCP_ATCmd_Protocol, // protocol
		TLKUSB_NO_DESCRIPTOR //InterfaceStrIndex
	},
	{
		// cdc_descriptor
		//CDC_Functional_Header
		{
			sizeof(tlkusb_cdcFunctionHead_t),	//Length
			TLKUSB_TYPE_CS_INTERFACE, // Header
			TLKUSB_CDC_TYPE_CSInterface_Header, // Subtype
			0x0110 // CDCSpecification
		},
		// CDC_Functional_ACM =
		{
			sizeof(tlkusb_cdcFunctionAcm_t),	//Length
			TLKUSB_TYPE_CS_INTERFACE, // Header
			TLKUSB_CDC_TYPE_CSInterface_ACM, // Subtype
			0x02 // Capabilities
		},
		// CDC_Functional_Union =
		{
			sizeof(tlkusb_cdcFunctionUnion_t),	//Length
			TLKUSB_TYPE_CS_INTERFACE, // Header
			TLKUSB_CDC_TYPE_CSInterface_Union, // Subtype
			0, // MasterInterfaceNumber
			1, // SlaveInterfaceNumber
		},
		// CDC_CallManagement =
		{
			sizeof(tlkusb_cdcFunctionUnion_t),	//Length
			TLKUSB_TYPE_CS_INTERFACE, // Header
			TLKUSB_CDC_TYPE_CSInterface_CallManagement, // Subtype
			0, // MasterInterfaceNumber
			1, // SlaveInterfaceNumber
		},
		// CDC_NotificationEndpoint =
		{
			sizeof(tlkusb_stdEndpointDesc_t),	//Length
			TLKUSB_TYPE_ENDPOINT, // Header
			(TLKUSB_EDP_DIR_IN | TLKUSB_CDC2_EDP_NOTY), // EndpointAddress
			(TLKUSB_EDP_TYPE_INTERRUPT | TLKUSB_EDP_ATTR_NO_SYNC | TLKUSB_EDP_USAGE_DATA), // Attributes
			TLKUSB_CDC_NOTIFY_EDPSIZE, // EndpointSize
			0x40 // PollingIntervalMS
		},
		// CDC_DCI_Interface =
		{
			sizeof(tlkusb_stdInterfaceDesc_t),	//Length
			TLKUSB_TYPE_INTERFACE, // Header
			TLKUSB_CDC2_INF_DCI, // InterfaceNumber
			0, // AlternateSetting
			2, // TotalEndpoints
			TLKUSB_CDC_CSCP_SubClass, // Class
			TLKUSB_CDC_CSCP_ACM_SubClass, // Subclass
			TLKUSB_CDC_CSCP_ATCmd_Protocol, // protocol
			TLKUSB_NO_DESCRIPTOR // InterfaceStrIndex
		},
		// CDC_DataOutEndpoint =
		{
			sizeof(tlkusb_stdEndpointDesc_t),	//Length
			TLKUSB_TYPE_ENDPOINT, // Header
			(TLKUSB_EDP_DIR_OUT | TLKUSB_CDC2_EDP_RX), // EndpointAddress
			(TLKUSB_EDP_TYPE_BULK | TLKUSB_EDP_ATTR_NO_SYNC | TLKUSB_EDP_USAGE_DATA), // Attributes
			TLKUSB_CDC_TXRX_EDPSIZE, // EndpointSize
			0x00 // PollingIntervalMS
		},
		// CDC_DataInEndpoint =
		{
			sizeof(tlkusb_stdEndpointDesc_t),//Length
			TLKUSB_TYPE_ENDPOINT, // Header
			(TLKUSB_EDP_DIR_IN | TLKUSB_CDC2_EDP_TX), // EndpointAddress
			(TLKUSB_EDP_TYPE_BULK | TLKUSB_EDP_ATTR_NO_SYNC | TLKUSB_EDP_USAGE_DATA), // Attributes
			TLKUSB_CDC_TXRX_EDPSIZE, // EndpointSize
			0x00 // PollingIntervalMS
		},
	},
#endif
};
#endif



static uint16 tlkusb_cdcdesc_getDeviceLens(void)
{
	return sizeof(tlkusb_stdDeviceDesc_t);
}
static uint16 tlkusb_cdcdesc_getConfigLens(void)
{
	return sizeof(tlkusb_cdcConfigDesc_t);
}
static uint16 tlkusb_cdcdesc_getStringLens(uint08 index)
{
	if(index == TLKUSB_STRING_INDEX_PRODUCT){
		return sizeof(TLKUSB_CDC_STRING_PRODUCT);
	}else if(index == TLKUSB_STRING_INDEX_SERIAL){
		return sizeof(TLKUSB_CDC_STRING_SERIAL);
	}else{
		return 0;
	}
}
static uint16 tlkusb_cdcdesc_getInfDesLens(tlkusb_setup_req_t *pSetup)
{
	return 0;
}

static uint08 *tlkusb_cdcdesc_getDeviceDesc(void)
{
	return (uint08*)(&sMmiUsbCdcDeviceDesc);
}
static uint08 *tlkusb_cdcdesc_getConfigDesc(void)
{
	return (uint08*)(&sMmiUsbCdcConfigDesc);
}
static uint08 *tlkusb_cdcdesc_getStringDesc(uint08 index)
{
	if(index == TLKUSB_STRING_INDEX_PRODUCT){
		return (uint08*)(&sMmiUsbCdcProductDesc);
	}else if(index == TLKUSB_STRING_INDEX_SERIAL){
		return (uint08*)(&sMmiUsbCdcSerialDesc);
	}else{
		return 0;
	}
}
static uint08 *tlkusb_cdcdesc_getInfDesDesc(tlkusb_setup_req_t *pSetup)
{
	return nullptr;
}





#endif //#if (TLK_USB_CDC_ENABLE)

