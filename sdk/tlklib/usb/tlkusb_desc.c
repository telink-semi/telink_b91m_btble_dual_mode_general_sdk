/********************************************************************************************************
 * @file	tlkusb_desc.c
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
#include "tlklib/usb/tlkusb_desc.h"

#if (TLK_CFG_USB_ENABLE)



/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
static const tlkusb_stdStringDesc_t sMmiUsbLanguageDesc = { 
	2 + 2, //
	TLKUSB_TYPE_STRING , //bDescriptorType
	{ TLKUSB_LANG_ID_ENGLISH } 
};

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
static const tlkusb_stdStringDesc_t sMmiUsbVendorDesc = { 
	2+sizeof(TLKUSB_STRING_VENDOR)-2, //-2 is the end of the string
	TLKUSB_TYPE_STRING , // Header
	TLKUSB_STRING_VENDOR
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
static const tlkusb_stdStringDesc_t sMmiUsbProductDesc = {
	2+sizeof(TLKUSB_STRING_PRODUCT)-2, //-2 is the end of the string
	TLKUSB_TYPE_STRING, // Header
	TLKUSB_STRING_PRODUCT 
};

/** Serial number string. This is a Unicode string containing the device's unique serial number, expressed as a
 *  series of uppercase hexadecimal digits.
 */
static const tlkusb_stdStringDesc_t sMmiUsbSerialDesc = {
	2+sizeof(TLKUSB_STRING_SERIAL)-2,
	TLKUSB_TYPE_STRING,
	TLKUSB_STRING_SERIAL
};



static const tlkusb_stdDeviceDesc_t sMmiUsbDeviceDesc = { 
	sizeof(tlkusb_stdDeviceDesc_t), //
	TLKUSB_TYPE_DEVICE , // Header
	0x0110, // USBSpecification, USB 1.1
	USB_CSCP_NoDeviceClass, // Class
	USB_CSCP_NoDeviceSubclass, // SubClass
	USB_CSCP_NoDeviceProtocol, // Protocol
//	USB_CSCP_IADDeviceClass, // Class
//	USB_CSCP_IADDeviceSubclass, // SubClass
//	USB_CSCP_IADDeviceProtocol, // Protocol
	8, // Endpoint0Size, Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64
	TLKUSB_ID_VENDOR, // VendorID
	TLKUSB_ID_PRODUCT, // ProductID
	TLKUSB_ID_VERSION/*0x0100*/, // .ReleaseNumber
	TLKUSB_STRING_INDEX_VENDOR,  // .ManufacturerStrIndex
	TLKUSB_STRING_INDEX_PRODUCT, // .ProductStrIndex
	TLKUSB_STRING_INDEX_SERIAL,  // .SerialNumStrIndex, iSerialNumber
	1 
};


extern uint08 gTlkUsbCurModType;

/******************************************************************************
 * Function: tlkusb_getLanguageDesc
 * Descript: This function use to get language descriptor.
 * Params: None
 * Return: Language descriptor.
 * Others: None.
*******************************************************************************/
uint08 *tlkusb_getLanguageDesc(void)
{
	uint08 *pDesc;
	pDesc = tlkusb_module_getStringDesc(gTlkUsbCurModType, TLKUSB_STRING_INDEX_LANGUAGE);
	if(pDesc == nullptr) pDesc = (uint08*)(&sMmiUsbLanguageDesc);
	return pDesc;
}
/******************************************************************************
 * Function: tlkusb_getVendorDesc
 * Descript: This function use to get vendor descriptor.
 * Params: None
 * Return: Vendor descriptor.
 * Others: None.
*******************************************************************************/
uint08 *tlkusb_getVendorDesc(void)
{
	uint08 *pDesc;
	pDesc = tlkusb_module_getStringDesc(gTlkUsbCurModType, TLKUSB_STRING_INDEX_VENDOR);
	if(pDesc == nullptr) pDesc = (uint08*)(&sMmiUsbVendorDesc);
	return pDesc;
}
/******************************************************************************
 * Function: tlkusb_getProductDesc
 * Descript: This function use to get product descriptor.
 * Params: None
 * Return: Product descriptor.
 * Others: None.
*******************************************************************************/
uint08 *tlkusb_getProductDesc(void)
{
	uint08 *pDesc;
	pDesc = tlkusb_module_getStringDesc(gTlkUsbCurModType, TLKUSB_STRING_INDEX_PRODUCT);
	if(pDesc == nullptr) pDesc = (uint08*)(&sMmiUsbProductDesc);
	return pDesc;
}
/******************************************************************************
 * Function: tlkusb_module_getStringDesc
 * Descript: This function use to get serial descriptor.
 * Params: None
 * Return: Serial descriptor.
 * Others: None.
*******************************************************************************/
uint08 *tlkusb_getSerialDesc(void)
{
	uint08 *pDesc;
	pDesc = tlkusb_module_getStringDesc(gTlkUsbCurModType, TLKUSB_STRING_INDEX_SERIAL);
	if(pDesc == nullptr) pDesc = (uint08*)(&sMmiUsbSerialDesc);
	return pDesc;
}
/******************************************************************************
 * Function: tlkusb_getLanguageLens
 * Descript: This function use to get language length.
 * Params: None
 * Return: Number of language length.
 * Others: None.
*******************************************************************************/
uint16 tlkusb_getLanguageLens(void)
{
	uint16 descLen;
	descLen = tlkusb_module_getStringLens(gTlkUsbCurModType, TLKUSB_STRING_INDEX_LANGUAGE);
	if(descLen == 0) descLen = sizeof(TLKUSB_LANG_ID_ENGLISH);
	return descLen;
}
/******************************************************************************
 * Function: tlkusb_getProductLens
 * Descript: This function use to get language product length.
 * Params: None
 * Return: Number of product string length.
 * Others: None.
*******************************************************************************/
uint16 tlkusb_getProductLens(void)
{
	uint16 descLen;
	descLen = tlkusb_module_getStringLens(gTlkUsbCurModType, TLKUSB_STRING_INDEX_PRODUCT);
	if(descLen == 0) descLen = sizeof(TLKUSB_STRING_PRODUCT);
	return descLen;
}
/******************************************************************************
 * Function: tlkusb_getVendorLens
 * Descript: This function use to get language vendor length.
 * Params: None
 * Return: Number of vendor string length.
 * Others: None.
*******************************************************************************/
uint16 tlkusb_getVendorLens(void)
{
	uint16 descLen;
	descLen = tlkusb_module_getStringLens(gTlkUsbCurModType, TLKUSB_STRING_INDEX_VENDOR);
	if(descLen == 0) descLen = sizeof(TLKUSB_STRING_VENDOR);
	return descLen;
}
/******************************************************************************
 * Function: tlkusb_getSerialLens
 * Descript: This function use to get serial length.
 * Params: None
 * Return: Number of serial length.
 * Others: None.
*******************************************************************************/
uint16 tlkusb_getSerialLens(void)
{
	uint16 descLen;
	descLen = tlkusb_module_getStringLens(gTlkUsbCurModType, TLKUSB_STRING_INDEX_SERIAL);
	if(descLen == 0) descLen = sizeof(TLKUSB_STRING_SERIAL);
	return descLen;
}

/******************************************************************************
 * Function: tlkusb_getDeviceDesc
 * Descript: This function use to get device descriptor.
 * Params: None
 * Return: Device descriptor.
 * Others: None.
*******************************************************************************/
uint08 *tlkusb_getDeviceDesc(void)
{
	uint08 *pDesc;
	pDesc = tlkusb_module_getDeviceDesc(gTlkUsbCurModType);
	if(pDesc == nullptr) pDesc = (uint08*)(&sMmiUsbDeviceDesc);
	return pDesc;	
}
/******************************************************************************
 * Function: tlkusb_getConfigDesc
 * Descript: This function use to get config descriptor.
 * Params: None
 * Return: Config descriptor.
 * Others: None.
*******************************************************************************/
uint08 *tlkusb_getConfigDesc(void)
{
	uint08 *pDesc;
	pDesc = tlkusb_module_getConfigDesc(gTlkUsbCurModType);
//	if(pDesc == nullptr) pDesc = (uint08*)(&sMmiUsbConfigDesc);
	return pDesc;
}
/******************************************************************************
 * Function: tlkusb_getDeviceLens
 * Descript: This function use to get device descriptor length.
 * Params: None
 * Return: Number of device descriptor length.
 * Others: None.
*******************************************************************************/
uint16 tlkusb_getDeviceLens(void)
{
	uint16 descLen;
	descLen = tlkusb_module_getDeviceLens(gTlkUsbCurModType);
	if(descLen == 0) descLen = sizeof(tlkusb_stdDeviceDesc_t);
	return descLen;
}
/******************************************************************************
 * Function: tlkusb_getConfigLens
 * Descript: This function use to get config descriptor length.
 * Params: None
 * Return: Number of config descriptor length.
 * Others: None.
*******************************************************************************/
uint16 tlkusb_getConfigLens(void)
{
	uint16 descLen;
	descLen = tlkusb_module_getConfigLens(gTlkUsbCurModType);
//	if(descLen == 0) descLen = sizeof(tlkusb_stdConfigDesc_t);
	return descLen;
}



#endif //#if (TLK_CFG_USB_ENABLE)

