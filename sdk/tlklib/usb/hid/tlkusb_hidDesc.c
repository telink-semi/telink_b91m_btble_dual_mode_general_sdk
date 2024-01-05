/********************************************************************************************************
 * @file	tlkusb_hidDesc.c
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
#if (TLK_USB_HID_ENABLE)
#include "tlklib/usb/hid/tlkusb_hidConfig.h"
#include "tlklib/usb/hid/tlkusb_hidDefine.h"
#include "tlklib/usb/hid/tlkusb_hid.h"
#include "tlklib/usb/hid/tlkusb_hidDesc.h"
#include "tlklib/usb/hid/tlkusb_hidCtrl.h"
#include "tlklib/usb/hid/tlkusb_hidConst.h"


static uint16 tlkusb_hiddesc_getDeviceLens(void);
static uint16 tlkusb_hiddesc_getConfigLens(void);
static uint16 tlkusb_hiddesc_getStringLens(uint08 index);
static uint16 tlkusb_hiddesc_getInfDesLens(tlkusb_setup_req_t *pSetup);
static uint08 *tlkusb_hiddesc_getDeviceDesc(void);
static uint08 *tlkusb_hiddesc_getConfigDesc(void);
static uint08 *tlkusb_hiddesc_getStringDesc(uint08 index);
static uint08 *tlkusb_hiddesc_getInfDesDesc(tlkusb_setup_req_t *pSetup);


const tlkusb_modDesc_t sTlkUsbHidModDesc = {
	tlkusb_hiddesc_getDeviceLens, //GetDeviceLens
	tlkusb_hiddesc_getConfigLens, //GetConfigLens
	tlkusb_hiddesc_getStringLens, //GetStringLens
	tlkusb_hiddesc_getInfDesLens, //GetReportLens
	tlkusb_hiddesc_getDeviceDesc, //GetDeviceDesc
	tlkusb_hiddesc_getConfigDesc, //GetConfigDesc
	tlkusb_hiddesc_getStringDesc, //GetStringDesc
	tlkusb_hiddesc_getInfDesDesc, //GetReportDesc
};



static const tlkusb_stdStringDesc_t sMmiUsbHidProductDesc = {
	2+sizeof(TLKUSB_HID_STRING_PRODUCT)-2, //-2 is the end of the string
	TLKUSB_TYPE_STRING, // Header
	TLKUSB_HID_STRING_PRODUCT 
};

/** Serial number string. This is a Unicode string containing the device's unique serial number, expressed as a
 *  series of uppercase hexadecimal digits.
 */
static const tlkusb_stdStringDesc_t sMmiUsbHidSerialDesc = {
	2+sizeof(TLKUSB_HID_STRING_SERIAL)-2,
	TLKUSB_TYPE_STRING,
	TLKUSB_HID_STRING_SERIAL
};

#if (TLK_USB_HID_ENABLE)
static const tlkusb_stdDeviceDesc_t sMmiUsbHidDeviceDesc = { 
	sizeof(tlkusb_stdDeviceDesc_t), //
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
static const tlkusb_hidConfigDesc_t sMmiUsbHidConfigDesc = {
	{
		sizeof(tlkusb_stdConfigureDesc_t),//Length
		TLKUSB_TYPE_CONFIGURE, // type
		sizeof(tlkusb_hidConfigDesc_t), // TotalLength: variable
		TLKUSB_HID_INF_MAX, // NumInterfaces
		1, // Configuration index
		TLKUSB_NO_DESCRIPTOR, // Configuration String
		TLKUSB_CFG_ATTR_RESERVED, // Attributes
		TLKUSB_CONFIG_POWER(100) // MaxPower = 100mA
	},
#if (TLKUSB_HID_MOUSE_ENABLE)
	// mouse_interface
	{
		sizeof(tlkusb_stdInterfaceDesc_t),//Length
		TLKUSB_TYPE_INTERFACE,	//type
		TLKUSB_HID_INF_MOUSE,	//Interface number
		0, // AlternateSetting
		1, // bNumEndpoints
		TLKUSB_HID_CSCP_HIDClass,	//class
		TLKUSB_HID_CSCP_BootSubclass,	//subclass
		TLKUSB_HID_CSCP_MouseBootProtocol,	//protocol
		TLKUSB_NO_DESCRIPTOR
	},
	// mouse_hid
	{	sizeof(tlkusb_HidEndpointDesc_t),	//Length
		TLKUSB_HID_DESC_HID,	//type:HID
		0x0111, // HIDSpec
		TLKUSB_HID_COUNTRY_US, // CountryCode
		1, // TotalReportDescriptors
		TLKUSB_HID_DESC_REPORT, 	//HIDReportType
		sizeof(cTlkUsbHidMouseDesc), // HIDReportLength
	},
	// mouse_in_endpoint
	{
		sizeof(tlkusb_stdEndpointDesc_t),//length
		TLKUSB_TYPE_ENDPOINT,	//type
		TLKUSB_EDP_DIR_IN | TLKUSB_HID_EDP_MOUSE_IN,	//endppoint
		TLKUSB_EDP_TYPE_INTERRUPT,	//bmAttributes
		0x0008, // EndpointSize
		TLKUSB_HID_MOUSE_POLL_INTERVAL // PollingIntervalMS
	},
#endif
#if (TLKUSB_HID_KEYBOARD_ENABLE)
	// keyboardInterface
	{
		sizeof(tlkusb_stdInterfaceDesc_t),//length
		TLKUSB_TYPE_INTERFACE,	//type
		TLKUSB_HID_INF_KEYBOARD,	//interface number
		0, // AlternateSetting
		1, // bNumEndpoints
		TLKUSB_HID_CSCP_HIDClass,//class
		TLKUSB_HID_CSCP_BootSubclass,//subclass
		TLKUSB_HID_CSCP_KeyboardBootProtocol,//protocol
		TLKUSB_NO_DESCRIPTOR	//string descriptor
	}, 
	// keyboard_hid
	{	
		sizeof(tlkusb_HidEndpointDesc_t),//length
		TLKUSB_HID_DESC_HID,//type:HID
		0x0111, // HIDSpec
		TLKUSB_HID_COUNTRY_US, // CountryCode
		1, // TotalReportDescriptors
		TLKUSB_HID_DESC_REPORT, //HIDReportType
		sizeof(cTlkUsbHidKeyboardDesc), // HIDReportLength
	},
	// keyboard_in_endpoint
	{	
		sizeof(tlkusb_stdEndpointDesc_t),//length
		TLKUSB_TYPE_ENDPOINT,//type
		TLKUSB_EDP_DIR_IN | TLKUSB_HID_EDP_KEYBOARD_IN,//endpoint
		TLKUSB_EDP_TYPE_INTERRUPT,//bmAttributes
		0x0008, // EndpointSize
		TLKUSB_HID_KEYBOARD_POLL_INTERVAL // PollingIntervalMS
	},
#endif
};
#endif



static uint16 tlkusb_hiddesc_getDeviceLens(void)
{
	return sizeof(tlkusb_stdDeviceDesc_t);
}
static uint16 tlkusb_hiddesc_getConfigLens(void)
{
	return sizeof(tlkusb_hidConfigDesc_t);
}
static uint16 tlkusb_hiddesc_getStringLens(uint08 index)
{
	if(index == TLKUSB_STRING_INDEX_PRODUCT){
		return sizeof(TLKUSB_HID_STRING_PRODUCT);
	}else if(index == TLKUSB_STRING_INDEX_SERIAL){
		return sizeof(TLKUSB_HID_STRING_SERIAL);
	}else{
		return 0;
	}
}
static uint16 tlkusb_hiddesc_getInfDesLens(tlkusb_setup_req_t *pSetup)
{
	uint16 length = 0;
	uint08 value_h = (pSetup->wValue >> 8) & 0xff;
	uint08 index_l = (pSetup->wIndex) & 0xff;

	if(value_h == TLKUSB_HID_DESC_HID){

	}
	else if(value_h == TLKUSB_HID_DESC_REPORT){
		#if (TLKUSB_HID_MOUSE_ENABLE)
		if(index_l == TLKUSB_HID_INF_MOUSE){
			length = sizeof(cTlkUsbHidMouseDesc);
		}
		#endif
		#if (TLKUSB_HID_KEYBOARD_ENABLE)
		if(index_l == TLKUSB_HID_INF_KEYBOARD){
			length = sizeof(cTlkUsbHidKeyboardDesc);
		}
		#endif
	}
	else if(value_h == TLKUSB_HID_DESC_PHYSICAL){

	}
	return length;
}

static uint08 *tlkusb_hiddesc_getDeviceDesc(void)
{
	return (uint08*)(&sMmiUsbHidDeviceDesc);
}
static uint08 *tlkusb_hiddesc_getConfigDesc(void)
{
	return (uint08*)(&sMmiUsbHidConfigDesc);
}
static uint08 *tlkusb_hiddesc_getStringDesc(uint08 index)
{
	if(index == TLKUSB_STRING_INDEX_PRODUCT){
		return (uint08*)(&sMmiUsbHidProductDesc);
	}else if(index == TLKUSB_STRING_INDEX_SERIAL){
		return (uint08*)(&sMmiUsbHidSerialDesc);
	}else{
		return 0;
	}
}
static uint08 *tlkusb_hiddesc_getInfDesDesc(tlkusb_setup_req_t *pSetup)
{
	uint08 *pData = nullptr;
	uint08 value_h = (pSetup->wValue >> 8) & 0xff;
	uint08 index_l = (pSetup->wIndex) & 0xff;

	if(value_h == TLKUSB_HID_DESC_HID){

	}
	else if(value_h == TLKUSB_HID_DESC_REPORT){
		#if (TLKUSB_HID_MOUSE_ENABLE)
		if(index_l == TLKUSB_HID_INF_MOUSE){
			pData = (uint08*)cTlkUsbHidMouseDesc;
		}
		#endif
		#if (TLKUSB_HID_KEYBOARD_ENABLE)
		if(index_l == TLKUSB_HID_INF_KEYBOARD){
			pData = (uint08*)cTlkUsbHidKeyboardDesc;
		}
		#endif
	}
	else if(value_h == TLKUSB_HID_DESC_PHYSICAL){

	}
	return pData;
}




#endif //#if (TLK_USB_HID_ENABLE)

