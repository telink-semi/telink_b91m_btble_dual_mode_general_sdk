/********************************************************************************************************
 * @file	tlkusb_cdcDesc.h
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
#ifndef TLKUSB_CDC_DESC_H
#define TLKUSB_CDC_DESC_H

#if (TLK_USB_CDC_ENABLE)



#define TLKUSB_CDC_STRING_PRODUCT       L"Telink B91 BTBLE CDC"
#define TLKUSB_CDC_STRING_SERIAL        L"TLSR9218-CDC"


typedef struct{
	uint08 bFunctionLength;	//Length
	uint08 bDescriptorType;	//Type
	uint08 bDescriptorSubType;	//Subtype
	uint16 bcdCDC;	//CDC Specification
}tlkusb_cdcFunctionHead_t;
typedef struct{
	uint08 bFunctionLength;//Length
	uint08 bDescriptorType;//Type
	uint08 bDescriptorSubType;//Subtype
	uint08 bmCapabilities;//Capabilities
}tlkusb_cdcFunctionAcm_t;
typedef struct{
	uint08 bFunctionLength;		//Length
	uint08 bDescriptorType;		//Type
	uint08 bDescriptorSubType;	//Subtype
	uint08 bMasterInterface;	//MasterInterfaceNumber
	uint08 bSlaveInterface0;	//SlaveInterfaceNumber
}tlkusb_cdcFunctionUnion_t;


typedef struct{
	// CDC Control Interface
	tlkusb_cdcFunctionHead_t  cciHead;
	tlkusb_cdcFunctionAcm_t   cdcACM;
	tlkusb_cdcFunctionUnion_t cdcUnion;
	tlkusb_cdcFunctionUnion_t cdcCall;
	tlkusb_stdEndpointDesc_t  cdcNotyEdp;
	// CDC Data Interface
	tlkusb_stdInterfaceDesc_t dciInf;
	tlkusb_stdEndpointDesc_t  dciOutEdp;
	tlkusb_stdEndpointDesc_t  dciInEdp;
}tlkusb_cdcFunctionsDesc_t;


typedef struct {
	tlkusb_stdConfigureDesc_t config;
	tlkusb_stdAssociateDesc_t cdc1IAD;
	tlkusb_stdInterfaceDesc_t cdc1Inf;
	tlkusb_cdcFunctionsDesc_t cdc1Func;
	#if (TLKUSB_CDC_SECOND_ENABLE)
	tlkusb_stdAssociateDesc_t cdc2IAD;
	tlkusb_stdInterfaceDesc_t cdc2Inf;
	tlkusb_cdcFunctionsDesc_t cdc2Func;
	#endif
}tlkusb_cdcConfigDesc_t;





#endif //#if (TLK_USB_CDC_ENABLE)

#endif //TLKUSB_CDC_DESC_H

