/********************************************************************************************************
 * @file	tlkdev_list.h
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
#ifndef TLKDEV_LIST_H
#define TLKDEV_LIST_H


#define TLKDEV_LIST_ECTYPE_NUMB      16
#define TLKDEV_LIST_ACTIVE_NUMB      16


typedef struct{
	const uint devID;
	const char *pName;
	int(*Init)(uint16 did);
	int(*Open)(uint16 did, uint32 param);
	int(*Close)(uint16 did);
	int(*State)(uint16 did); //TLK_STATE_CLOSED(init-idle), TLK_STATE_OPENED(open-idle), TLK_STATE_BUSYING(busy), TLK_STATE_UNKNOWN(unmount)
	int(*Read)(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
	int(*Write)(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
	int(*Ioctl)(uint16 did, uint16 opcode, uint32 param0, uint32 param1);
}tlkdev_node_t;


int tlkdev_list_init(void);

int tlkdev_getEctypeNodeNumb(void);
int tlkdev_getActiveNodeNumb(void);

int tlkdev_appendEctypeNode(const tlkdev_node_t *pNode);
int tlkdev_removeEctypeNode(uint16 did);

int tlkdev_appendActiveNode(const tlkdev_node_t *pNode);
int tlkdev_removeActiveNode(uint16 did);

const tlkdev_node_t *tlkdev_getEctypeNode(uint16 did, uint16 *pIndex);
const tlkdev_node_t *tlkdev_getActiveNode(uint16 did, uint16 *pIndex);

const tlkdev_node_t *tlkdev_getEctypeNodeByIndex(int index);
const tlkdev_node_t *tlkdev_getActiveNodeByIndex(int index);



#endif //TLKDEV_LIST_H

