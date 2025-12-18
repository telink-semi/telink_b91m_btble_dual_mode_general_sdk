/********************************************************************************************************
 * @file	tlkapi_list.h
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
#ifndef TLKAPI_LIST_H
#define TLKAPI_LIST_H

// TODO: wait for verification

typedef struct tlkapi_list_node_s{
	struct tlkapi_list_node_s *pNext;
}tlkapi_list_node_t;

typedef struct{
	uint32 count;
	tlkapi_list_node_t *pNode;
}tlkapi_list_t;


/******************************************************************************
 * Function: tlkapi_list_init
 * Descript: Initialize a list to defaults values.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: Return the initialization result, TLK ENONE means success.
 *****************************************************************************/
int tlkapi_list_init(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_clear
 * Descript: Empty all element nodes in the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: None.
 *****************************************************************************/
void tlkapi_list_clear(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_check
 * Descript: Prints all elements of a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: Return true if the check is ok; Otherwise return false.
 * Notice:
 *     1.This interface is just for testing.
 *****************************************************************************/
bool tlkapi_list_check(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_print
 * Descript: Prints all elements of a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: None.
 *****************************************************************************/
void tlkapi_list_print(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_init
 * Descript: Detects whether the linked list is empty.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: Return true if the list is empty; Otherwise return false.
 *****************************************************************************/
bool tlkapi_list_isEmpty(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_count
 * Descript: Gets the number of elements in a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The number of elements in the linked list.
 *****************************************************************************/
int tlkapi_list_count(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_index
 * Descript: Gets the offset position of the input element in the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be detected.
 * Return: The offset position of the input element in the linked list. And 
 *         negative value means failure.
 *****************************************************************************/
int tlkapi_list_index(tlkapi_list_t *pList, tlkapi_list_node_t *pNode);

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
int tlkapi_list_merge(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList);

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
 *****************************************************************************/
int tlkapi_list_mergeAt(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList, int index);

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
int tlkapi_list_mergeFirst(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList);

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
int tlkapi_list_mergeAfter(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList);

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
int tlkapi_list_insert(tlkapi_list_t *pList, tlkapi_list_node_t *pNode);

/******************************************************************************
 * Function: tlkapi_list_insertAt
 * Descript: Inserts the input node into the specified position.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be inserted.
 *     @index[in]--The location of the linked list to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_insertAt(tlkapi_list_t *pList, tlkapi_list_node_t *pNode, int index);

/******************************************************************************
 * Function: tlkapi_list_insertFirst
 * Descript: Insert a node into the first position of the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_insertFirst(tlkapi_list_t *pList, tlkapi_list_node_t *pNode);

/******************************************************************************
 * Function: tlkapi_list_insertAfter
 * Descript: Insert a node to the end of the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_insertAfter(tlkapi_list_t *pList, tlkapi_list_node_t *pNode);

/******************************************************************************
 * Function: tlkapi_list_insertSubList
 * Descript: Inserts another linked list at the specified position in the 
 *           linked list.
 * Params:
 *     @pDstList[IN/OUT]--List of targets to be extracted.
 *     @pSrcList[IN]--List of sources to be extracted.
 *     @index[in]--The location of the linked list to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface is equivalent to "tlkapi_list_mergeAt".
 *****************************************************************************/
int tlkapi_list_insertSubList(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList, int index);

/******************************************************************************
 * Function: tlkapi_list_remove
 * Descript: Removes a node from a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @pNode[IN]--Node to be removed.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_remove(tlkapi_list_t *pList, tlkapi_list_node_t *pNode);

/******************************************************************************
 * Function: tlkapi_list_removeAt
 * Descript: Removes the node at the specified position from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @index[in]--The location of the linked list to be removed.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_removeAt(tlkapi_list_t *pList, int index);

/******************************************************************************
 * Function: tlkapi_list_removeFirst
 * Descript: Removes the first node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_removeFirst(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_removeAfter
 * Descript: Removes the last node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_removeAfter(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_removeSubList
 * Descript: Removes a section nodes from a linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 *     @index[in]--The location of the linked list to be removed.
 *     @count[in]--The number of the node to be removed.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface is equivalent to "tlkapi_list_takeSubList".
 *****************************************************************************/
int tlkapi_list_removeSubList(tlkapi_list_t *pList, int index, int count);

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
tlkapi_list_node_t *tlkapi_list_pickAt(tlkapi_list_t *pList, int index);

/******************************************************************************
 * Function: tlkapi_list_pickFirst
 * Descript: Gets the first node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The pointer to the element extracted, and NULL if the list is empty.
 * Notice:
 *     1.This interface does not remove the element from the linked list.
 *****************************************************************************/
tlkapi_list_node_t *tlkapi_list_pickFirst(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_pickAfter
 * Descript: Gets the last node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The pointer to the element extracted, and NULL if the list is empty.
 * Notice:
 *     1.This interface does not remove the element from the linked list.
 *****************************************************************************/
tlkapi_list_node_t *tlkapi_list_pickAfter(tlkapi_list_t *pList);

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
tlkapi_list_node_t *tlkapi_list_takeAt(tlkapi_list_t *pList, int index);

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
tlkapi_list_node_t *tlkapi_list_takeFirst(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_takeAfter
 * Descript: Gets and removes the last node from the linked list.
 * Params:
 *     @pList[IN]--Pointer to the list structure.
 * Return: The pointer to the element extracted, and NULL if the list is empty.
 * Notice:
 *     1.This interface will remove the element from the linked list.
 *****************************************************************************/
tlkapi_list_node_t *tlkapi_list_takeAfter(tlkapi_list_t *pList);

/******************************************************************************
 * Function: tlkapi_list_takeSubList
 * Descript: Gets and removes a section nodes from the linked list.
 * Params:
 *     @pDstList[IN]--List of targets to be extracted.
 *     @pSrcList[IN]--List of sources to be extracted.
 *     @index[in]--The location of the linked list to be obtained and removed.
 *     @count[in]--The number of the node to be obtained and removed.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_list_takeSubList(tlkapi_list_t *pDstList, tlkapi_list_t *pSrcList, int index, int count);



#endif //TLKAPI_LIST_H

