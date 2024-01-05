/********************************************************************************************************
 * @file	tlkusb_module.c
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
#if (TLK_CFG_USB_ENABLE)
#include "tlklib/usb/tlkusb_desc.h"
#include "tlklib/usb/tlkusb_module.h"

#if (TLK_USB_UDB_ENABLE)
extern const tlkusb_module_t gTlkUsbUdbModule;
#endif
#if (TLK_USB_UAC_ENABLE)
extern const tlkusb_module_t gTlkUsbUacModule;
#endif
#if (TLK_USB_MSC_ENABLE)
extern const tlkusb_module_t gTlkUsbMscModule;
#endif
#if (TLK_USB_CDC_ENABLE)
extern const tlkusb_module_t gTlkUsbCdcModule;
#endif
#if (TLK_USB_HID_ENABLE)
extern const tlkusb_module_t gTlkUsbHidModule;
#endif
#if (TLK_USB_USR_ENABLE)
extern const tlkusb_module_t gTlkUsbUsrModule;
#endif
static const tlkusb_module_t *sTlkUsbModule[TLKUSB_MODTYPE_MAX] = 
{
	#if (TLK_USB_UDB_ENABLE)
	&gTlkUsbUdbModule,
	#else
	nullptr,
	#endif
	#if (TLK_USB_UAC_ENABLE)
	&gTlkUsbUacModule,
	#else
	nullptr,
	#endif
	#if (TLK_USB_MSC_ENABLE)
	&gTlkUsbMscModule,
	#else
	nullptr,
	#endif
	#if (TLK_USB_CDC_ENABLE)
	&gTlkUsbCdcModule,
	#else
	nullptr,
	#endif
	#if (TLK_USB_HID_ENABLE)
	&gTlkUsbHidModule,
	#else
	nullptr,
	#endif
	#if (TLK_USB_USR_ENABLE)
	&gTlkUsbUsrModule,
	#else
	nullptr,
	#endif
};

/******************************************************************************
 * Function: tlkusb_module_init
 * Descript: This function use to init usb module.
 * Params:
 * 		@modType[IN]--USB Module Type.
 * Return: TLK_ENONE is SUCCESS.
 * Others: None.
*******************************************************************************/
int tlkusb_module_init(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->Init == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->Init();
}
/******************************************************************************
 * Function: tlkusb_module_reset
 * Descript: This function use to reset usb module.
 * Params:
 * 		@modType[IN]--USB Module Type.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkusb_module_reset(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->Reset == nullptr){
		return;
	}
	sTlkUsbModule[modType]->pCtrl->Reset();
}
/******************************************************************************
 * Function: tlkusb_module_deinit
 * Descript: This function use to deinit usb module.
 * Params:
 * 		@modType[IN]--USB Module Type.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkusb_module_deinit(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->Deinit == nullptr){
		return;
	}
	sTlkUsbModule[modType]->pCtrl->Deinit();
}
/******************************************************************************
 * Function: tlkusb_module_handler
 * Descript: This function use to loop through usb device.
 * Params:
 * 		@modType[IN]--USB Module Type.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkusb_module_handler(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->Handler == nullptr){
		return;
	}
	sTlkUsbModule[modType]->pCtrl->Handler();
}
/******************************************************************************
 * Function: tlkusb_module_getClassInf
 * Descript: This function use to get Interface class.
 * Params:
 * 		@modType[IN]--USB Module Type.
 *		@pSetup[IN]--Request information.
 *		@infNumb[IN]--Interface Number.
 * Return: TLK_ENONE is SUCCESS.
 * Others: None.
*******************************************************************************/
int tlkusb_module_getClassInf(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->GetClassInf == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->GetClassInf(pSetup, infNumb);
}
/******************************************************************************
 * Function: tlkusb_module_setClassInf
 * Descript: This function use to set Interface class.
 * Params:
 * 		@modType[IN]--USB Module Type.
 *		@pSetup[IN]--Request information.
 *		@infNumb[IN]--Interface Number.
 * Return: TLK_ENONE is SUCCESS.
 * Others: None.
*******************************************************************************/
int tlkusb_module_setClassInf(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->SetClassInf == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->SetClassInf(pSetup, infNumb);
}
/******************************************************************************
 * Function: tlkusb_module_getClassEdp
 * Descript: This function use to get endpoint class.
 * Params:
 * 		@modType[IN]--USB Module Type.
 *		@pSetup[IN]--Request information.
 *		@infNumb[IN]--Interface Number.
 * Return: TLK_ENONE is SUCCESS.
 * Others: None.
*******************************************************************************/
int tlkusb_module_getClassEdp(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->GetClassEdp == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->GetClassEdp(pSetup, edpNumb);
}
/******************************************************************************
 * Function: tlkusb_module_setClassEdp
 * Descript: This function use to set endpoint class.
 * Params:
 * 		@modType[IN]--USB Module Type.
 *		@pSetup[IN]--Request information.
 *		@infNumb[IN]--Interface Number.
 * Return: TLK_ENONE is SUCCESS.
 * Others: None.
*******************************************************************************/
int tlkusb_module_setClassEdp(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->SetClassEdp == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->SetClassEdp(pSetup, edpNumb);
}
/******************************************************************************
 * Function: tlkusb_module_getInterface
 * Descript: This function use to get interface descriptor.
 * Params:
 * 		@modType[IN]--USB Module Type.
 *		@pSetup[IN]--Request information.
 *		@infNumb[IN]--Interface Number.
 * Return: TLK_ENONE is SUCCESS.
 * Others: None.
*******************************************************************************/
int tlkusb_module_getInterface(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->GetInterface == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->GetInterface(pSetup, infNumb);
}
/******************************************************************************
 * Function: tlkusb_module_setInterface
 * Descript: This function use to set interface.
 * Params:
 * 		@modType[IN]--USB Module Type.
 *		@pSetup[IN]--Request information.
 *		@infNumb[IN]--Interface Number.
 * Return: TLK_ENONE is SUCCESS.
 * Others: None.
*******************************************************************************/
int tlkusb_module_setInterface(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->SetInterface == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->SetInterface(pSetup, infNumb);
}

uint16 tlkusb_module_getDeviceLens(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return 0;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetDeviceLens == nullptr){
		return 0;
	}
	return sTlkUsbModule[modType]->pDesc->GetDeviceLens();
}

uint16 tlkusb_module_getConfigLens(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return 0;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetConfigLens == nullptr){
		return 0;
	}
	return sTlkUsbModule[modType]->pDesc->GetConfigLens();
}

uint16 tlkusb_module_getStringLens(uint08 modType, uint08 index)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return 0;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetStringLens == nullptr){
		return 0;
	}
	return sTlkUsbModule[modType]->pDesc->GetStringLens(index);
}
/******************************************************************************
 * Function: tlkusb_module_getInfDesLens
 * Descript: This function use to get interface descriptor length.
 * Params:
 * 		@modType[IN]--USB Module Type.
 *		@pSetup[IN]--Request information.
 * Return: Number of interface descriptor length.
 * Others: None.
*******************************************************************************/
uint16 tlkusb_module_getInfDesLens(uint08 modType, tlkusb_setup_req_t *pSetup)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return 0;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetInfDesLens == nullptr){
		return 0;
	}
	return sTlkUsbModule[modType]->pDesc->GetInfDesLens(pSetup);
}

uint08 *tlkusb_module_getDeviceDesc(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return nullptr;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetDeviceDesc == nullptr){
		return nullptr;
	}
	return sTlkUsbModule[modType]->pDesc->GetDeviceDesc();
}

uint08 *tlkusb_module_getConfigDesc(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return nullptr;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetConfigDesc == nullptr){
		return nullptr;
	}
	return sTlkUsbModule[modType]->pDesc->GetConfigDesc();
}
/******************************************************************************
 * Function: tlkusb_module_getStringDesc
 * Descript: This function use to get string descriptor.
 * Params:
 * 		@modType[IN]--USB Module Type.
 * Return: String descriptor.
 * Others: None.
*******************************************************************************/
uint08 *tlkusb_module_getStringDesc(uint08 modType, uint08 index)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return nullptr;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetStringDesc == nullptr){
		return nullptr;
	}
	return sTlkUsbModule[modType]->pDesc->GetStringDesc(index);
}
/******************************************************************************
 * Function: tlkusb_module_getInfDesDesc
 * Descript: This function use to get interface descriptor.
 * Params:
 * 		@modType[IN]--USB Module Type.
 * Return: Interface descriptor.
 * Others: None.
*******************************************************************************/
uint08 *tlkusb_module_getInfDesDesc(uint08 modType, tlkusb_setup_req_t *pSetup)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return nullptr;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetInfDesDesc == nullptr){
		return nullptr;
	}
	return sTlkUsbModule[modType]->pDesc->GetInfDesDesc(pSetup);
}



#endif

