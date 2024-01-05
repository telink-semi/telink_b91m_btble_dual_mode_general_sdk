/********************************************************************************************************
 * @file	tlkusb_cdcDefine.h
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
#ifndef TLKUSB_CDC_DEFINE_H
#define TLKUSB_CDC_DEFINE_H

#if (TLK_USB_CDC_ENABLE)



typedef enum{
	TLKUSB_CDC_INF_CCI = 0,
	TLKUSB_CDC_INF_DCI,
	#if (TLKUSB_CDC_SECOND_ENABLE)
	TLKUSB_CDC2_INF_CCI,
	TLKUSB_CDC2_INF_DCI,
	#endif
	TLKUSB_CDC_INF_MAX,
}TLKUSB_CDC_INF_ENUM;

typedef enum{
	TLKUSB_CDC_EDP_NOTY   = 1,
	TLKUSB_CDC_EDP_TX     = 3,
	TLKUSB_CDC_EDP_RX     = 6,
	#if (TLKUSB_CDC_SECOND_ENABLE)
	TLKUSB_CDC2_EDP_NOTY  = 2,
	TLKUSB_CDC2_EDP_TX    = 4,
	TLKUSB_CDC2_EDP_RX    = 5,
	#endif
}TLKUSB_CDC_EDP_ENUM;



typedef enum{
	TLKUSB_CDC_CSCP_NoSpecific_Subclass = 0x00,
	TLKUSB_CDC_CSCP_NoSpecific_Protocol = 0x00,
	TLKUSB_CDC_CSCP_NoData_Subclass     = 0x00,
	TLKUSB_CDC_CSCP_NoData_Protocol     = 0x00,
	TLKUSB_CDC_CSCP_ATCmd_Protocol      = 0x01,
	TLKUSB_CDC_CSCP_SubClass            = 0x02,
	TLKUSB_CDC_CSCP_ACM_SubClass        = 0x02,
	TLKUSB_CDC_CSCP_Data_Class          = 0x0A,
	TLKUSB_CDC_CSCP_VendorSpecific_Protocol = 0xFF,
}TLKUSB_CDC_CSCP_ENUM; //CSCP-ClassSubClass_Protocol

// Communications Class Functional Descriptors
typedef enum{
	TLKUSB_CDC_TYPE_CSInterface_Header,
	TLKUSB_CDC_TYPE_CSInterface_CallManagement,
	TLKUSB_CDC_TYPE_CSInterface_ACM,
	TLKUSB_CDC_TYPE_CSInterface_DirectLine,
	TLKUSB_CDC_TYPE_CSInterface_TelephoneRinger,
	TLKUSB_CDC_TYPE_CSInterface_TelephoneCall,
	TLKUSB_CDC_TYPE_CSInterface_Union,
	TLKUSB_CDC_TYPE_CSInterface_CountrySelection,
	TLKUSB_CDC_TYPE_CSInterface_TelephoneOpModes,
	TLKUSB_CDC_TYPE_CSInterface_USBTerminal,
	TLKUSB_CDC_TYPE_CSInterface_NetworkChannel,
	TLKUSB_CDC_TYPE_CSInterface_ProtocolUnit,
	TLKUSB_CDC_TYPE_CSInterface_ExtensionUnit,
	TLKUSB_CDC_TYPE_CSInterface_MultiChannel,
	TLKUSB_CDC_TYPE_CSInterface_CAPI,
	TLKUSB_CDC_TYPE_CSInterface_Ethernet,
	TLKUSB_CDC_TYPE_CSInterface_ATM,
}TLKUSB_CDC_TYPE_ENUM;

typedef enum{
	TLKUSB_CDC_OPCODE_SendEncapsulated_Cmd,
	TLKUSB_CDC_OPCODE_GetEncapsulated_Rsp,
	TLKUSB_CDC_OPCODE_SetLineEncoding = 0x20,
	TLKUSB_CDC_OPCODE_GetLineEncoding,
	TLKUSB_CDC_OPCODE_SetControlLineState,
	TLKUSB_CDC_OPCODE_SendBreak,
}TLKUSB_CDC_OPCODE_ENUM;




#endif //#if (TLK_USB_CDC_ENABLE)

#endif //TLKUSB_CDC_DEFINE_H

