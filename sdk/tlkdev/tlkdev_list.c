/********************************************************************************************************
 * @file	tlkdev_list.c
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
#if (TLK_CFG_DEV_ENABLE)
#include "tlkdev.h"
#include "tlkdev_list.h"



static uint08 sTlkDevEctypeNumb = 0;
static uint08 sTlkDevActiveNumb = 0;
static const tlkdev_node_t *scTlkDevEctypeList[TLKDEV_LIST_ECTYPE_NUMB];
static const tlkdev_node_t *scTlkDevActiveList[TLKDEV_LIST_ACTIVE_NUMB];


int tlkdev_list_init(void)
{
	sTlkDevEctypeNumb = 0;
	sTlkDevActiveNumb = 0;
	tmemset(scTlkDevEctypeList, 0, TLKDEV_LIST_ECTYPE_NUMB*sizeof(tlkdev_node_t*));
	tmemset(scTlkDevActiveList, 0, TLKDEV_LIST_ACTIVE_NUMB*sizeof(tlkdev_node_t*));
	return TLK_ENONE;
}

int tlkdev_getEctypeNodeNumb(void)
{
	return sTlkDevEctypeNumb;
}
int tlkdev_getActiveNodeNumb(void)
{
	return sTlkDevActiveNumb;
}

int tlkdev_appendEctypeNode(const tlkdev_node_t *pNode)
{
	int ret;
	if(pNode == nullptr) return -TLK_EPARAM;
	if(tlkdev_getEctypeNode(pNode->devID, nullptr) != nullptr) return -TLK_EREPEAT;
	tlksys_disable_interrupt();
	if(sTlkDevEctypeNumb >= TLKDEV_LIST_ECTYPE_NUMB){
		ret = -TLK_EQUOTA;
	}else{
		scTlkDevEctypeList[sTlkDevEctypeNumb++] = pNode;
		ret = TLK_ENONE;
	}
	tlksys_restore_interrupt();
	return ret;
}
int tlkdev_removeEctypeNode(uint16 did)
{
	uint16 index;
	const tlkdev_node_t *pNode;
	
	tlksys_disable_interrupt();
	pNode = tlkdev_getEctypeNode(did, &index);
	if(pNode != nullptr){
		sTlkDevEctypeNumb --;
		if(index != sTlkDevEctypeNumb){
			tmemcpy(&scTlkDevEctypeList[index], &scTlkDevEctypeList[index+1], (sTlkDevEctypeNumb-index)*sizeof(tlkdev_node_t*));
		}
		scTlkDevEctypeList[sTlkDevEctypeNumb] = nullptr;
	}
	tlksys_restore_interrupt();

	if(pNode == nullptr) return -TLK_ENOOBJECT;
	return TLK_ENONE;
}

int tlkdev_appendActiveNode(const tlkdev_node_t *pNode)
{
	int ret;
	
	if(pNode == nullptr) return -TLK_EPARAM;
	if(tlkdev_getActiveNode(pNode->devID, nullptr) != nullptr){
		return -TLK_EREPEAT;
	} 
	
	tlksys_disable_interrupt();
	if(sTlkDevActiveNumb >= TLKDEV_LIST_ACTIVE_NUMB){
		ret = -TLK_EQUOTA;
	}else{
		scTlkDevActiveList[sTlkDevActiveNumb++] = pNode;
		ret = TLK_ENONE;
	}
	tlksys_restore_interrupt();
	return ret;
}
int tlkdev_removeActiveNode(uint16 did)
{
	uint16 index;
	const tlkdev_node_t *pNode;
	
	tlksys_disable_interrupt();
	pNode = tlkdev_getActiveNode(did, &index);
	if(pNode != nullptr){
		sTlkDevActiveNumb --;
		if(index != sTlkDevActiveNumb){
			tmemcpy(&scTlkDevActiveList[index], &scTlkDevActiveList[index+1], (sTlkDevActiveNumb-index)*sizeof(tlkdev_node_t*));
		}
		scTlkDevActiveList[sTlkDevActiveNumb] = nullptr;
	}
	tlksys_restore_interrupt();
	if(pNode == nullptr) return -TLK_ENOOBJECT;
	return TLK_ENONE;
}

const tlkdev_node_t *tlkdev_getEctypeNode(uint16 did, uint16 *pIndex)
{
	int index;
	const tlkdev_node_t *pNode;
	for(index=0; index<sTlkDevEctypeNumb; index++){
		pNode = scTlkDevEctypeList[index];
		if(pNode != nullptr && pNode->devID == did) break;
	}
	if(index == sTlkDevEctypeNumb) return nullptr;
	if(pIndex != nullptr) *pIndex = index;
	return scTlkDevEctypeList[index];
}
const tlkdev_node_t *tlkdev_getActiveNode(uint16 did, uint16 *pIndex)
{
	int index;
	const tlkdev_node_t *pNode;
	for(index=0; index<sTlkDevActiveNumb; index++){
		pNode = scTlkDevActiveList[index];
		if(pNode != nullptr && pNode->devID == did) break;
	}
	if(index == sTlkDevActiveNumb) return nullptr;
	if(pIndex != nullptr) *pIndex = index;
	return scTlkDevActiveList[index];
}


const tlkdev_node_t *tlkdev_getEctypeNodeByIndex(int index)
{
	if(index >= sTlkDevEctypeNumb) return nullptr;
	return scTlkDevEctypeList[index];
}
const tlkdev_node_t *tlkdev_getActiveNodeByIndex(int index)
{
	if(index >= sTlkDevActiveNumb) return nullptr;
	return scTlkDevActiveList[index];
}

//volatile uint08 AAAA_dev_ectypeNumb;
//volatile uint08 AAAA_dev_activeNumb;
//volatile uint08 AAAA_dev_ectype[TLKDEV_LIST_ECTYPE_NUMB];
//volatile uint08 AAAA_dev_active[TLKDEV_LIST_ACTIVE_NUMB];
//void tlkdev_test(void)
//{
//	int index;
//	const tlkdev_node_t *pNode;
//	
//	tmemset(AAAA_dev_ectype, 0, TLKDEV_LIST_ECTYPE_NUMB);
//	tmemset(AAAA_dev_active, 0, TLKDEV_LIST_ECTYPE_NUMB);

//	AAAA_dev_ectypeNumb = sTlkDevEctypeNumb;
//	for(index=0; index<sTlkDevEctypeNumb; index++){
//		pNode = scTlkDevEctypeList[index];
//		if(pNode != nullptr) AAAA_dev_ectype[index] = pNode->devID;
//	}
//	
//	AAAA_dev_activeNumb = sTlkDevActiveNumb;
//	for(index=0; index<sTlkDevActiveNumb; index++){
//		pNode = scTlkDevActiveList[index];
//		if(pNode != nullptr) AAAA_dev_active[index] = pNode->devID;
//	}
//}


#endif //#if (TLK_CFG_DEV_ENABLE)

