/********************************************************************************************************
 * @file	tlkapi_list.c
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
#include "stdbool.h"
#include "string.h"
#include "tlk_config.h"
#include "tlk_debug.h"
#include "tlkapi_type.h"
#include "tlkapi_error.h"
#include "tlkapi_list.h"
#include "tlkapi_debug.h"

// TODO: wait for verification

#define TLKAPI_LIST_DBG_FLAG         0//((TLK_MAJOR_DBGID_SYS << 24) | (TLK_MINOR_DBGID_SYS_API << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKAPI_LIST_DBG_SIGN         "[API]"


/******************************************************************************
 * Function: tlkapi_list_init
 * Descript: Initialize a list to defaults values.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: Return the initialization result, TLK ENONE means success.
 *****************************************************************************/
int tlkapi_list_init(tlkapi_list_t *pList)
{
	if(pList == nullptr) return -TLK_EFAIL;
	pList->count = 0;
	pList->pNode = nullptr;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_clear
 * Descript: Empty all element nodes in the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: None.
 *****************************************************************************/
void tlkapi_list_clear(tlkapi_list_t *pList)
{
	tlkapi_list_node_t *pNode;
	tlkapi_list_node_t *pTemp;
	
	if(tlkapi_list_isEmpty(pList)) return;
	
	pNode = pList->pNode->pNext;
	while(true){
		pTemp = pNode->pNext;
		pNode->pNext = nullptr;
		if(pNode == pList->pNode) break;
		pNode = pTemp;
	}
	pNode->pNext = nullptr;
	pList->pNode = nullptr;
}

/******************************************************************************
 * Function: tlkapi_list_check
 * Descript: Prints all elements of a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: Return true if the check is ok; Otherwise return false.
 * Notice:
 *     1.This interface is just for testing.
 *****************************************************************************/
bool tlkapi_list_check(tlkapi_list_t *pList)
{
	int count;
	tlkapi_list_node_t *pTemp;

	if(pList == nullptr) return false;

	count = 0;
	if(pList->pNode != nullptr){
		count ++;
		pTemp = pList->pNode->pNext;
		while(true){
			count ++;
			if(pTemp == pList->pNode) break;
			pTemp = pTemp->pNext;
		}
	}
	if(count == pList->count) return true;
	else return false;
}

/******************************************************************************
 * Function: tlkapi_list_print
 * Descript: Prints all elements of a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: None.
 *****************************************************************************/
void tlkapi_list_print(tlkapi_list_t *pList)
{
#if (TLK_DBG_LOG_ENABLE)
	int index;
	int count;
	tlkapi_list_node_t *pTemp;
	
	if(tlkapi_list_isEmpty(pList)){
		tlkapi_trace(TLKAPI_LIST_DBG_FLAG, TLKAPI_LIST_DBG_SIGN,
			"tlkapi_list_print:{rcdCount=%d, check=%d}",
			pList->count, tlkapi_list_check(pList));
		return;
	}

	count = 1;
	pTemp = pList->pNode->pNext;
	while(pTemp != pList->pNode){
		count ++;
		pTemp = pTemp->pNext;
	}

	tlkapi_trace(TLKAPI_LIST_DBG_FLAG, TLKAPI_LIST_DBG_SIGN,
		"tlkapi_list_print:{rcdCount=%d, relCount=0, check=%d}",
		pList->count, count, tlkapi_list_check(pList));
	
	index = 0;
	pTemp = pList->pNode->pNext;
	while(true){
		tlkapi_trace(TLKAPI_LIST_DBG_FLAG, TLKAPI_LIST_DBG_SIGN,
			"  index:{index-%d, pNode-0x%x, pNext-0x%x}", 
			index, pTemp, pTemp->pNext);
		index ++;
		if(pTemp == pList->pNode) break;
		pTemp = pTemp->pNext;
	}
#endif
}

/******************************************************************************
 * Function: tlkapi_list_init
 * Descript: Detects whether the linked list is empty.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: Return true if the list is empty; Otherwise return false.
 *****************************************************************************/
bool tlkapi_list_isEmpty(tlkapi_list_t *pList)
{
	if(pList == nullptr || pList->pNode == nullptr) return true;
	return false;
}

/******************************************************************************
 * Function: tlkapi_list_count
 * Descript: Gets the number of elements in a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The number of elements in the linked list.
 *****************************************************************************/
int tlkapi_list_count(tlkapi_list_t *pList)
{
	if(pList == nullptr) return 0;
	return pList->count;
}

/******************************************************************************
 * Function: tlkapi_list_index
 * Descript: Gets the offset position of the input element in the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be detected.
 * Return: The offset position of the input element in the linked list. And 
 *         negative value means failure.
 *****************************************************************************/
int tlkapi_list_index(tlkapi_list_t *pList, tlkapi_list_node_t *pNode)
{
	int index;
	tlkapi_list_node_t *pTemp;

	if(pList == nullptr || pList->pNode == nullptr
		|| pNode == nullptr || pNode->pNext == nullptr){
		return -TLK_ENOOBJECT;
	}

	index = 0;
	pTemp = pList->pNode->pNext;
	while(true){
		if(pTemp == pNode) break;
		index ++;
		if(pTemp == pList->pNode) break;
		pTemp = pTemp->pNext;
	}
	if(pTemp != pNode) return -TLK_ENOOBJECT;
	return index;
}

/******************************************************************************
 * Function: tlkapi_list_merge
 * Descript: Merge two linked lists together.
 * Params:
 *     @pDstList[IN/OUT]--List of targets to be merged.
 *     @pSrcList[IN]--List of sources to be merged.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.After the merge, the source list will be emptied.
 *     2.This interface is equivalent to "tlkapi_list_mergeAfter".
 *****************************************************************************/
int tlkapi_list_merge(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList)
{
	return tlkapi_list_mergeAfter(pDstList, pSrcList);
}

/******************************************************************************
 * Function: tlkapi_list_merge
 * Descript: Merge another linked list at the specified position in the linked 
 *           list.
 * Params:
 *     @pDstList[IN/OUT]--List of targets to be merged.
 *     @pSrcList[IN]--List of sources to be merged.
 *     @index[in]--The location of the linked list to be merged.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.After the merge, the source list will be emptied.
 *     2.This interface is equivalent to "tlkapi_list_mergeAfter".
 *****************************************************************************/
int tlkapi_list_mergeAt(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList, int index)
{
	tlkapi_list_node_t *pNode;
	tlkapi_list_node_t *pTemp;
	
	if(pDstList == nullptr || pSrcList == nullptr){
		return -TLK_EPARAM;
	}
	if(pSrcList->pNode == nullptr) return TLK_ENONE;
	
	if(pDstList->pNode == nullptr || index < 0){
		return tlkapi_list_mergeAfter(pDstList, pSrcList);
	}
	
	pDstList->count += pSrcList->count;
	
	if(index == 0){
		return tlkapi_list_mergeFirst(pDstList, pSrcList);
	}

	pNode = tlkapi_list_pickAt(pDstList, index);
	if(pNode == nullptr || pNode == pDstList->pNode){
		return tlkapi_list_mergeAfter(pDstList, pSrcList);
	}
	
	pTemp = pNode->pNext;
	pNode->pNext = pSrcList->pNode->pNext;
	pSrcList->pNode->pNext = pTemp;
	
	pDstList->count += pSrcList->count;
	pSrcList->count = 0;
	pSrcList->pNode = nullptr;

	return TLK_ENONE;	
}

/******************************************************************************
 * Function: tlkapi_list_mergeFirst
 * Descript: Merges the source list into the first position of the target list.
 * Params:
 *     @pDstList[IN/OUT]--List of targets to be merged.
 *     @pSrcList[IN]--List of sources to be merged.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.After the merge, the source list will be emptied.
 *****************************************************************************/
int tlkapi_list_mergeFirst(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList)
{
	tlkapi_list_node_t *pTemp;
	
	if(pDstList == nullptr || pSrcList == nullptr) return -TLK_EPARAM;
	if(pSrcList->pNode == nullptr) return TLK_ENONE;

	pDstList->count += pSrcList->count;
	pTemp = pSrcList->pNode->pNext;
	pSrcList->pNode->pNext = pDstList->pNode->pNext;
	pDstList->pNode->pNext = pTemp;
	pSrcList->count = 0;
	pSrcList->pNode = nullptr;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_mergeAfter
 * Descript: Merges the source list into the last position of the target list.
 * Params:
 *     @pDstList[IN/OUT]--List of targets to be merged.
 *     @pSrcList[IN]--List of sources to be merged.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.After the merge, the source list will be emptied.
 *****************************************************************************/
int tlkapi_list_mergeAfter(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList)
{
	if(pDstList == nullptr || pSrcList == nullptr) return -TLK_EPARAM;
	if(pSrcList->pNode == nullptr) return TLK_ENONE;
	
	pDstList->count += pSrcList->count;
	if(pDstList->pNode != nullptr){
		tlkapi_list_node_t *pTemp;
		pTemp = pDstList->pNode->pNext;
		pDstList->pNode->pNext = pSrcList->pNode->pNext;
		pSrcList->pNode->pNext = pTemp;
	}
	pDstList->pNode = pSrcList->pNode;
	pSrcList->count = 0;
	pSrcList->pNode = nullptr;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_insert
 * Descript: Insert a node into a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface is equivalent to "tlkapi_list_insertAfter".
 *****************************************************************************/
int tlkapi_list_insert(tlkapi_list_t *pList, tlkapi_list_node_t *pNode)
{
	return tlkapi_list_insertAfter(pList, pNode);
}

/******************************************************************************
 * Function: tlkapi_list_insertAt
 * Descript: Inserts the input node into the specified position.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be inserted.
 *     @index[in]--The location of the linked list to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_insertAt(tlkapi_list_t *pList, tlkapi_list_node_t *pNode, int index)
{
	int offset;
	tlkapi_list_node_t *pTemp;
	
	if(pList == nullptr || pNode == nullptr){
		return -TLK_EPARAM;
	}
	if(index < 0 || pList->pNode == nullptr){
		return tlkapi_list_insertAfter(pList, pNode);
	}
	if(index == 0){
		pNode->pNext = pList->pNode->pNext;
		pList->pNode->pNext = pNode;
		return TLK_ENONE;
	}

	offset = 0;
	pTemp = pList->pNode->pNext;
	while(true){
		if((++offset) == index) break;
		if(pTemp == pList->pNode) break;
		pTemp = pTemp->pNext;
	}
	if(pTemp == pList->pNode){
		pNode->pNext = pList->pNode->pNext;
		pList->pNode->pNext = pNode;
		pList->pNode = pNode;
	}else{
		pNode->pNext = pTemp->pNext;
		pTemp->pNext = pNode;
	}
	pList->count ++;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_insertFirst
 * Descript: Insert a node into the first position of the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_insertFirst(tlkapi_list_t *pList, tlkapi_list_node_t *pNode)
{
	if(pList == nullptr || pNode == nullptr){
		return -TLK_EPARAM;
	}
	if(pList->pNode == nullptr){
		pNode->pNext = pNode;
		pList->pNode = pNode;
	}else{
		pNode->pNext = pList->pNode->pNext;
		pList->pNode->pNext = pNode;
	}
	pList->count ++;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_insertAfter
 * Descript: Insert a node to the end of the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_insertAfter(tlkapi_list_t *pList, tlkapi_list_node_t *pNode)
{
	if(pList == nullptr || pNode == nullptr) return -TLK_EPARAM;
	if(pList->pNode == nullptr){
		pNode->pNext = pNode;
	}else{
		pNode->pNext = pList->pNode->pNext;
		pList->pNode->pNext = pNode;
	}
	pList->pNode = pNode;
	pList->count ++;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_insertSubList
 * Descript: Inserts another linked list at the specified position in the 
 *           linked list.
 * Params:
 *     @pDstList[IN]--List of targets to be extracted.
 *     @pSrcList[IN]--List of sources to be extracted.
 *     @index[in]--The location of the linked list to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_insertSubList(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList, int index)
{
	return tlkapi_list_mergeAt(pDstList, pSrcList, index);
}

/******************************************************************************
 * Function: tlkapi_list_remove
 * Descript: Removes a node from a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be removed.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_remove(tlkapi_list_t *pList, tlkapi_list_node_t *pNode)
{
	tlkapi_list_node_t *pTemp;
	
	if(pList == nullptr || pNode == nullptr || pList->pNode == nullptr
		|| pNode->pNext == nullptr){
		return -TLK_EPARAM;
	}

	if(pList->pNode->pNext == pNode){
		return tlkapi_list_removeFirst(pList);
	}
	if(pList->pNode == pNode){
		return tlkapi_list_removeAfter(pList);
	}
	
	pTemp = pList->pNode->pNext;
	while(true){
		if(pTemp->pNext == pNode) break;
		if(pTemp == pList->pNode) break;
		pTemp = pTemp->pNext;
	}
	if(pTemp->pNext != pNode) return -TLK_ENOOBJECT;
	
	pTemp->pNext = pNode->pNext;
	pNode->pNext = nullptr;
	if(pList->count != 0) pList->count --;
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_removeAt
 * Descript: Removes the node at the specified position from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @index[in]--The location of the linked list to be removed.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_removeAt(tlkapi_list_t *pList, int index)
{
	if(tlkapi_list_takeAt(pList, index) == nullptr){
		return -TLK_ENOOBJECT;
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_removeFirst
 * Descript: Removes the first node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_removeFirst(tlkapi_list_t *pList)
{
	if(tlkapi_list_takeFirst(pList) == nullptr){
		return -TLK_ENOOBJECT;
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_removeAfter
 * Descript: Removes the last node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_removeAfter(tlkapi_list_t *pList)
{
	if(tlkapi_list_takeAfter(pList) == nullptr){
		return -TLK_ENOOBJECT;
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_list_removeSubList
 * Descript: Removes a section nodes from a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @index[in]--The location of the linked list to be removed.
 *     @count[in]--The number of the node to be removed.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_removeSubList(tlkapi_list_t *pList, int index, int count)
{
	tlkapi_list_t dstList;
	tlkapi_list_init(&dstList);
	return tlkapi_list_takeSubList(&dstList, pList, index, count);
}

/******************************************************************************
 * Function: tlkapi_list_pickAt
 * Descript: Gets the node at the specified location from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @index[in]--The location of the linked list to be removed.
 * Return: The pointer to the element extracted, and NULL if the list is empty.
 * Notice:
 *     1.This interface does not remove the element from the linked list.
 *****************************************************************************/
tlkapi_list_node_t *tlkapi_list_pickAt(tlkapi_list_t *pList, int index)
{
	int offset;
	tlkapi_list_node_t *pTemp;
	
	if(index < 0) return tlkapi_list_pickAfter(pList);
	if(index == 0) return tlkapi_list_pickFirst(pList);
	if(pList == nullptr || pList->pNode == nullptr) return nullptr;
	
	offset = 0;
	pTemp = pList->pNode->pNext;
	while(true){
		if(offset == index) break;
		if(pTemp == pList->pNode) break;
		pTemp = pTemp->pNext;
		offset ++;
	}
	return pTemp;
}

/******************************************************************************
 * Function: tlkapi_list_pickFirst
 * Descript: Gets the first node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The pointer to the element extracted, and NULL if the list is empty.
 * Notice:
 *     1.This interface does not remove the element from the linked list.
 *****************************************************************************/
tlkapi_list_node_t *tlkapi_list_pickFirst(tlkapi_list_t *pList)
{
	if(pList == nullptr || pList->pNode == nullptr) return nullptr;
	return pList->pNode->pNext;
}

/******************************************************************************
 * Function: tlkapi_list_pickAfter
 * Descript: Gets the last node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The pointer to the element extracted, and NULL if the list is empty.
 * Notice:
 *     1.This interface does not remove the element from the linked list.
 *****************************************************************************/
tlkapi_list_node_t *tlkapi_list_pickAfter(tlkapi_list_t *pList)
{
	if(pList == nullptr || pList->pNode == nullptr) return nullptr;
	return pList->pNode;
}

/******************************************************************************
 * Function: tlkapi_list_takeAt
 * Descript: Gets and removes the node at the specified position from the 
 *           linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @index[in]--The location of the linked list to be obtained and removed.
 * Return: The pointer to the element extracted, and NULL if the list is empty.
 * Notice:
 *     1.This interface will remove the element from the linked list.
 *****************************************************************************/
tlkapi_list_node_t *tlkapi_list_takeAt(tlkapi_list_t *pList, int index)
{
	int offset;
	tlkapi_list_node_t *pNode;
	tlkapi_list_node_t *pTemp;
	
	if(index < 0) return tlkapi_list_takeAfter(pList);
	if(index == 0) return tlkapi_list_takeFirst(pList);
	if(pList == nullptr || pList->pNode == nullptr) return nullptr;
	
	pNode = pList->pNode->pNext;
	if(pNode == pList->pNode){
		if(pList->count != 0) pList->count --;
		pList->pNode = nullptr;
		pNode->pNext = nullptr;
		return pNode;
	}

	if(pList->count != 0) pList->count --;

	offset = 1;
	while(pNode->pNext != pList->pNode){
		if(offset == index) break;
		if(pNode->pNext == pList->pNode) break;
		pNode = pNode->pNext;
		offset ++;
	}

	pTemp = pNode->pNext;
	pNode->pNext = pTemp->pNext;
	if(pTemp == pList->pNode){
		pList->pNode = pNode;
	}
	pTemp->pNext = nullptr;
	
	return pTemp;
}

/******************************************************************************
 * Function: tlkapi_list_takeFirst
 * Descript: Gets and removes the first node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @index[in]--The location of the linked list to be obtained and removed.
 * Return: The pointer to the element extracted, and NULL if the list is empty.
 * Notice:
 *     1.This interface will remove the element from the linked list.
 *****************************************************************************/
tlkapi_list_node_t *tlkapi_list_takeFirst(tlkapi_list_t *pList)
{
	tlkapi_list_node_t *pNode;
	
	if(pList == nullptr || pList->pNode == nullptr) return nullptr;

	if(pList->count != 0) pList->count --;
	
	pNode = pList->pNode->pNext;
	if(pNode == pList->pNode){
		pList->pNode = nullptr;
	}else{
		pList->pNode->pNext = pNode->pNext;
	}
	pNode->pNext = nullptr;
	
	return pNode;
}

/******************************************************************************
 * Function: tlkapi_list_takeAfter
 * Descript: Gets and removes the last node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The pointer to the element extracted, and NULL if the list is empty.
 * Notice:
 *     1.This interface will remove the element from the linked list.
 *****************************************************************************/
tlkapi_list_node_t *tlkapi_list_takeAfter(tlkapi_list_t *pList)
{
	tlkapi_list_node_t *pNode;
	tlkapi_list_node_t *pTemp;
	
	if(pList == nullptr || pList->pNode == nullptr) return nullptr;

	if(pList->count != 0) pList->count --;
	
	pNode = pList->pNode->pNext;
	if(pNode == pList->pNode){
		pList->pNode = nullptr;
		pNode->pNext = nullptr;
		return pNode;
	}
	
	while(pNode->pNext != pList->pNode){
		pNode = pNode->pNext;
	}
	pTemp = pList->pNode;
	pNode->pNext = pList->pNode->pNext;
	pList->pNode = pNode;
	pTemp->pNext = nullptr;
	
	return pTemp;
}

/******************************************************************************
 * Function: tlkapi_list_takeSubList
 * Descript: Gets and removes a section nodes from the source's list.
 * Params:
 *     @pDstList[OUT]--List of targets to be extracted.
 *     @pSrcList[IN]--List of sources to be extracted.
 *     @index[in]--The location of the linked list to be obtained and removed.
 *     @count[in]--The number of the node to be obtained and removed.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_takeSubList(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList, int index, int count)
{
	int number;
	int offset;
	tlkapi_list_node_t *pHead;
	tlkapi_list_node_t *pNode;
	tlkapi_list_node_t *pHNode;
	tlkapi_list_node_t *pSNode;
	tlkapi_list_node_t *pENode;
	
	if(index < 0 || count <= 0 || pDstList == nullptr || pSrcList == nullptr
		|| pSrcList->pNode == nullptr){
		return -TLK_EPARAM;
	}
	
	offset = 0;
	number = 0;
	pSNode = nullptr;
	pENode = nullptr;
	
	pHead = pSrcList->pNode;
	pNode = pHead->pNext;
	pHNode = pNode;
	while(true){
		if(offset+1 == index) pHNode = pNode;
		if(offset == index) pSNode = pNode;
		if(pSNode != nullptr && (++number) == count){
			pENode = pNode;
			break;
		}
		if(pNode == pHead) break;
		pNode = pNode->pNext;
		offset ++;
	}
	
	if(pSNode == nullptr) return -TLK_EFAIL;
	if(pENode == nullptr) pENode = pSrcList->pNode;
	
	pDstList->count = number;
	if(pSNode == pSrcList->pNode->pNext && pENode == pSrcList->pNode){
		pSrcList->pNode = nullptr;
	}else if(pSNode != pSrcList->pNode->pNext && pENode != pSrcList->pNode){
		pHNode->pNext = pENode->pNext;
	}else if(pSNode == pSrcList->pNode->pNext){
		pSrcList->pNode->pNext = pENode->pNext;
	}else{ //pENode == pSrcList->pNode
		pHNode->pNext = pSrcList->pNode->pNext;
		pSrcList->pNode = pHNode;
	}
	pENode->pNext = pSNode;
	pDstList->pNode = pENode;
	if(pSrcList->count <= number) pSrcList->count = 0;
	else pSrcList->count -= number;
	return TLK_ENONE;
}

