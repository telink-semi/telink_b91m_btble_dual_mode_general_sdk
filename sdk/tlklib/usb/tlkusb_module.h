/********************************************************************************************************
 * @file	tlkusb_module.h
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
#ifndef TLKUSB_MODULE_H
#define TLKUSB_MODULE_H

#if (TLK_CFG_USB_ENABLE)


#define TLKUSB_MODULE_UINT08_PTR    uint08*




typedef struct{
	int(*Init)(void);//Init
	void(*Reset)(void);//Reset
	void(*Deinit)(void);//Deinit
	void(*Handler)(void);//Handler
	int(*GetClassInf)(tlkusb_setup_req_t *pSetup, uint08 infNumb);//GetClassInf
	int(*SetClassInf)(tlkusb_setup_req_t *pSetup, uint08 infNumb);//SetClassInf
	int(*GetClassEdp)(tlkusb_setup_req_t *pSetup, uint08 edpNumb);//GetClassEdp
	int(*SetClassEdp)(tlkusb_setup_req_t *pSetup, uint08 edpNumb);//SetClassEdp
	int(*GetInterface)(tlkusb_setup_req_t *pSetup, uint08 infNumb);//GetInterface
	int(*SetInterface)(tlkusb_setup_req_t *pSetup, uint08 infNumb);//SetInterface
}tlkusb_modCtrl_t;

typedef struct{
	uint16(*GetDeviceLens)(void);//GetDeviceLens
	uint16(*GetConfigLens)(void);//GetConfigLens
	uint16(*GetStringLens)(uint08 index);//GetStringLens
	uint16(*GetInfDesLens)(tlkusb_setup_req_t *pSetup);//GetInterfaceLens
	TLKUSB_MODULE_UINT08_PTR(*GetDeviceDesc)(void);//Get device descriptor
	TLKUSB_MODULE_UINT08_PTR(*GetConfigDesc)(void);//Get config descriptor
	TLKUSB_MODULE_UINT08_PTR(*GetStringDesc)(uint08 index); //Get Serial String Descriptor
	TLKUSB_MODULE_UINT08_PTR(*GetInfDesDesc)(tlkusb_setup_req_t *pSetup); //Get interface descriptor
}tlkusb_modDesc_t;

typedef struct{
	uint08 modType;
	const tlkusb_modDesc_t *pDesc;
	const tlkusb_modCtrl_t *pCtrl;
}tlkusb_module_t;



int tlkusb_module_init(uint08 modType);
void tlkusb_module_reset(uint08 modType);
void tlkusb_module_deinit(uint08 modType);
void tlkusb_module_handler(uint08 modType);
int tlkusb_module_getClassInf(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb);
int tlkusb_module_setClassInf(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb);
int tlkusb_module_getClassEdp(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 edpNumb);
int tlkusb_module_setClassEdp(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 edpNumb);
int tlkusb_module_getInterface(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb);
int tlkusb_module_setInterface(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb);

uint16 tlkusb_module_getDeviceLens(uint08 modType);
uint16 tlkusb_module_getConfigLens(uint08 modType);
uint16 tlkusb_module_getStringLens(uint08 modType, uint08 index);
uint16 tlkusb_module_getInfDesLens(uint08 modType, tlkusb_setup_req_t *pSetup);

uint08 *tlkusb_module_getDeviceDesc(uint08 modType);
uint08 *tlkusb_module_getConfigDesc(uint08 modType);
uint08 *tlkusb_module_getStringDesc(uint08 modType, uint08 index);
uint08 *tlkusb_module_getInfDesDesc(uint08 modType, tlkusb_setup_req_t *pSetup);




#endif //#if (TLK_CFG_USB_ENABLE)

#endif //TLKUSB_MODULE_H

