/********************************************************************************************************
 * @file	tlkapi_tree.c
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
#include "tlkapi_tree.h"
#include "tlkapi_debug.h"

// TODO: wait for verification


#define TLKAPI_TREE_DBG_FLAG         0//((TLK_MAJOR_DBGID_SYS << 24) | (TLK_MINOR_DBGID_SYS_API << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKAPI_TREE_DBG_SIGN         "[API]"


/******************************************************************************
 * Function: tlkapi_tree_init
 * Descript: Initialize a tree node to defaults values.
 * Params:
 *     @pNode[IN]--Pointer to the tree node structure.
 * Return: Return the initialization result, TLK ENONE means success.
 *****************************************************************************/
int tlkapi_tree_init(tlkapi_tree_t *pNode)
{
	if(pNode == nullptr) return -TLK_EPARAM;
	pNode->pNNode = nullptr;
	pNode->pCNode = nullptr;
	pNode->pPNode = nullptr;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_tree_print
 * Descript: Prints all nodes contained in the input node.
 * Params:
 *     @pNode[IN]--The node to be printed.
 * Return: None.
 *****************************************************************************/
void tlkapi_tree_print(tlkapi_tree_t *pNode)
{
#if (TLK_DBG_LOG_ENABLE)
	if(pNode == nullptr) return;
	tlkapi_trace(TLKAPI_TREE_DBG_FLAG, TLKAPI_TREE_DBG_SIGN,
			"Node{pNode[0x%x], pNNode[0x%x], pPNode[0x%x]}", 
			pNode, pNode->pNNode, pNode->pPNode);
	tlkapi_tree_printNext(pNode);
	tlkapi_tree_printAllChild(pNode, 0);
#endif
}

/******************************************************************************
 * Function: tlkapi_tree_printNext
 * Descript: Prints all the parallel nodes contained in the input node.
 * Params:
 *     @pRNode[IN]--The reference node to be printed.
 * Return: None.
 *****************************************************************************/
void tlkapi_tree_printNext(tlkapi_tree_t *pRNode)
{
#if (TLK_DBG_LOG_ENABLE)
	tlkapi_tree_t *pTemp;
	if(pRNode == nullptr) return;
	pTemp = pRNode->pNNode;
	while(pTemp != nullptr){
		tlkapi_trace(TLKAPI_TREE_DBG_FLAG, TLKAPI_TREE_DBG_SIGN,
			"Next{pNode[0x%x], pNNode[0x%x], pPNode[0x%x]}", 
			pTemp, pTemp->pNNode, pTemp->pPNode);
		pTemp = pTemp->pNNode;
	}
#endif
}

/******************************************************************************
 * Function: tlkapi_tree_printChild
 * Descript: Prints the child nodes contained in the input node.
 * Params:
 *     @pPNode[IN]--The parent node to be printed.
 *     @deep[IN]--Depth of the current printing child node.
 * Return: None.
 * Notice:
 *     1.This interface prints the child nodes of a layer.
 *****************************************************************************/
void tlkapi_tree_printChild(tlkapi_tree_t *pPNode, int deep)
{
#if (TLK_DBG_LOG_ENABLE)
	tlkapi_tree_t *pTemp;
	if(pPNode == nullptr) return;
	pTemp = pPNode->pCNode;
	while(pTemp != nullptr){
		tlkapi_trace(TLKAPI_TREE_DBG_FLAG, TLKAPI_TREE_DBG_SIGN,
			"Child{deep[%d], pNode[0x%x], pNNode[0x%x], pPNode[0x%x]}", 
			deep, pTemp, pTemp->pNNode, pTemp->pPNode);
		pTemp = pTemp->pNNode;
	}
#endif
}

/******************************************************************************
 * Function: tlkapi_tree_printAllChild
 * Descript: Prints all of the child nodes contained in the input node.
 * Params:
 *     @pPNode[IN]--The parent node to be printed.
 *     @deep[IN]--Depth of the current printing child node.
 * Return: None.
 * Notice:
 *     1.This interface prints the child nodes of all layer.
 *****************************************************************************/
void tlkapi_tree_printAllChild(tlkapi_tree_t *pPNode, int deep)
{
	tlkapi_tree_t *pTemp;
	
	if(pPNode == nullptr) return;
	
	pTemp = pPNode->pCNode;
	while(pTemp != nullptr){
		tlkapi_trace(TLKAPI_TREE_DBG_FLAG, TLKAPI_TREE_DBG_SIGN,
			"Child{deep[%d], pNode[0x%x], pNNode[0x%x], pPNode[0x%x]}", 
			deep, pTemp, pTemp->pNNode, pTemp->pPNode);
		pTemp = pTemp->pNNode;
		tlkapi_tree_printAllChild(pTemp, deep+1);
	}
}

/******************************************************************************
 * Function: tlkapi_tree_insertNext
 * Descript: Adds the input node to the level of the reference node.
 * Params:
 *     @pRNode[IN]--The reference node.
 *     @pNode[IN]--The node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface is equivalent to "tlkapi_tree_insertNextAfter".
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_insertNext(tlkapi_tree_t *pRNode, tlkapi_tree_t *pNode)
{
	return tlkapi_tree_insertNextAfter(pRNode, pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_insertNextAt
 * Descript: Adds the input node to the specified position and level with the 
 *           reference node.
 * Params:
 *     @pRNode[IN]--The reference node.
 *     @pNode[IN]--The node to be inserted.
 *     @index[IN]--The position of "pNode" to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_insertNextAt(tlkapi_tree_t *pRNode, tlkapi_tree_t *pNode, int index)
{
	int offset;
	tlkapi_tree_t *pTemp;
	
	if(pRNode == nullptr || pNode == nullptr){
		return -TLK_EPARAM;
	}
	
	pNode->pPNode = pRNode->pPNode;
	if(pRNode->pNNode == nullptr || index == 0){
		pNode->pNNode = pRNode->pNNode;
		pRNode->pNNode = pNode;
		return TLK_ENONE;
	}
	
	offset = 1;
	pTemp = pRNode->pNNode;
	while(pTemp->pNNode != nullptr){
		if((offset++) == index) break;
		pTemp = pTemp->pNNode;
	}
	if(pTemp->pNNode == nullptr){
		pNode->pNNode = nullptr;
	}else{
		pNode->pNNode = pTemp->pNNode->pNNode;
	}
	pTemp->pNNode = pNode;

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_tree_insertNextFirst
 * Descript: Adds the input node to the first position and level with the 
 *           reference node.
 * Params:
 *     @pRNode[IN]--The reference node.
 *     @pNode[IN]--The node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_insertNextFirst(tlkapi_tree_t *pRNode, tlkapi_tree_t *pNode)
{
	if(pRNode == nullptr || pNode == nullptr){
		return -TLK_EPARAM;
	}
	
	pNode->pPNode = pRNode->pPNode;
	pNode->pNNode = pRNode->pNNode;
	pRNode->pNNode = pNode;
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_tree_insertNextAfter
 * Descript: Adds the input node to the last position and level with the 
 *           reference node.
 * Params:
 *     @pRNode[IN]--The reference node.
 *     @pNode[IN]--The node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_insertNextAfter(tlkapi_tree_t *pRNode, tlkapi_tree_t *pNode)
{	
	tlkapi_tree_t *pTemp;
	
	if(pRNode == nullptr || pNode == nullptr){
		return -TLK_EPARAM;
	}

	pNode->pNNode = nullptr;
	pNode->pPNode = pRNode->pPNode;
	if(pRNode->pNNode == nullptr){
		pRNode->pNNode = pNode;
		return TLK_ENONE;
	}

	pTemp = pRNode->pNNode;
	while(pTemp->pNNode != nullptr){
		pTemp = pTemp->pNNode;
	}
	pTemp->pNNode = pNode;

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_tree_insertChild
 * Descript: Adds the input node to the level of the reference node.
 * Params:
 *     @pPNode[IN]--The parent node.
 *     @pNode[IN]--The node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface is equivalent to "tlkapi_tree_insertChildAfter".
 *****************************************************************************/
int tlkapi_tree_insertChild(tlkapi_tree_t *pPNode, tlkapi_tree_t *pNode)
{
	return tlkapi_tree_insertChildAfter(pPNode->pCNode, pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_insertChildAt
 * Descript: Inserts the input node into the specified location under the 
 *           parent node.
 * Params:
 *     @pPNode[IN]--The parent node.
 *     @pNode[IN]--The node to be inserted.
 *     @index[IN]--The position of "pNode" to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_tree_insertChildAt(tlkapi_tree_t *pPNode, tlkapi_tree_t *pNode, int index)
{
	if(pPNode == nullptr || pNode == nullptr){
		return -TLK_EPARAM;
	}
	
	if(pPNode->pCNode == nullptr){
		pNode->pNNode = nullptr;
		pNode->pPNode = pPNode;
		pPNode->pCNode = pNode;
		return TLK_ENONE;
	}
	
	return tlkapi_tree_insertNextAt(pPNode->pCNode, pNode, index);
}

/******************************************************************************
 * Function: tlkapi_tree_insertChildFirst
 * Descript: Inserts the input node into the first location under the parent
 *           node.
 * Params:
 *     @pPNode[IN]--The parent node.
 *     @pNode[IN]--The node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_tree_insertChildFirst(tlkapi_tree_t *pPNode, tlkapi_tree_t *pNode)
{
	if(pPNode == nullptr || pNode == nullptr){
		return -TLK_EPARAM;
	}
	
	if(pPNode->pCNode == nullptr){
		pNode->pNNode = nullptr;
		pNode->pPNode = pPNode;
		pPNode->pCNode = pNode;
		return TLK_ENONE;
	}
	
	return tlkapi_tree_insertNextFirst(pPNode->pCNode, pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_insertChildAfter
 * Descript: Inserts the input node into the last location under the parent
 *           node.
 * Params:
 *     @pPNode[IN]--The parent node.
 *     @pNode[IN]--The node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_tree_insertChildAfter(tlkapi_tree_t *pPNode, tlkapi_tree_t *pNode)
{
	if(pPNode == nullptr || pNode == nullptr){
		return -TLK_EPARAM;
	}
	
	if(pPNode->pCNode == nullptr){
		pNode->pNNode = nullptr;
		pNode->pPNode = pPNode;
		pPNode->pCNode = pNode;
		return TLK_ENONE;
	}
	
	return tlkapi_tree_insertNextAfter(pPNode->pCNode, pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_removeNode
 * Descript: Remove the input node.
 * Params:
 *     @pNode[IN]--The node to be removed.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *     2.A node without a parent node cannot invoke the interface. You need to
 *       manually delete the node.
 *****************************************************************************/
int tlkapi_tree_removeNode(tlkapi_tree_t *pNode)
{
	tlkapi_tree_t *pTemp;
	
	if(pNode == nullptr) return -TLK_EPARAM;
	if(pNode->pPNode == nullptr) return -TLK_ENOSUPPORT;
	if(pNode->pPNode->pNNode == nullptr) return -TLK_EFAULT;

	pTemp = pNode->pPNode;
	while(pTemp->pNNode != nullptr && pTemp->pNNode != pNode){
		pTemp = pTemp->pNNode;
	}
	if(pTemp->pNNode == pNode){
		pTemp->pNNode = pNode->pNNode;
	}
	pNode->pNNode = nullptr;
	return tlkapi_tree_removeChild(pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_removeNextAt
 * Descript: Remove a parallel node at a specified position under the reference
 *           node.
 * Params:
 *     @pNode[IN]--The reference node.
 *     @index[IN]--The position of the "pRNode->pNNode" to be removed.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_removeNextAt(tlkapi_tree_t *pRNode, int index)
{
	int offset;
	tlkapi_tree_t *pNode;
	tlkapi_tree_t *pTemp;
	
	if(pRNode == nullptr) return -TLK_EPARAM;
	if(pRNode->pNNode == nullptr) return TLK_ENONE;

	offset = 0;
	pTemp = pRNode;
	while(pTemp->pNNode->pNNode != nullptr && offset != index){
		offset ++;
		pTemp = pTemp->pNNode;
	}
	pNode = pTemp->pNNode;
	pTemp->pNNode = pNode->pNNode;
	pNode->pNNode = nullptr;
	return tlkapi_tree_removeChild(pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_removeNextFirst
 * Descript: Remove a parallel node at a first position under the reference
 *           node.
 * Params:
 *     @pNode[IN]--The reference node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_removeNextFirst(tlkapi_tree_t *pRNode)
{
	tlkapi_tree_t *pNode;
	
	if(pRNode == nullptr) return -TLK_EPARAM;
	if(pRNode->pNNode == nullptr) return TLK_ENONE;

	pNode = pRNode->pNNode;
	pRNode->pNNode = pNode->pNNode;
	pNode->pNNode = nullptr;
	return tlkapi_tree_removeChild(pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_removeNextAfter
 * Descript: Remove a parallel node at a last position under the reference node.
 * Params:
 *     @pNode[IN]--The reference node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_removeNextAfter(tlkapi_tree_t *pRNode)
{
	tlkapi_tree_t *pNode;
	tlkapi_tree_t *pTemp;
	
	if(pRNode == nullptr) return -TLK_EPARAM;
	if(pRNode->pNNode == nullptr) return TLK_ENONE;

	pTemp = pRNode;
	while(pTemp->pNNode->pNNode != nullptr){
		pTemp = pTemp->pNNode;
	}
	pNode = pTemp->pNNode;
	pTemp->pNNode = nullptr;
	return tlkapi_tree_removeChild(pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_removeChild
 * Descript: Remove the child node of the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *****************************************************************************/
int tlkapi_tree_removeChild(tlkapi_tree_t *pPNode)
{
	tlkapi_tree_t *pNode;
	
	if(pPNode == nullptr) return -TLK_EPARAM;
	if(pPNode->pCNode == nullptr) return TLK_ENONE;
	
	while(pPNode->pCNode != nullptr){
		pNode = pPNode->pCNode;
		pPNode->pCNode = pPNode->pCNode->pNNode;
		pNode->pNNode = nullptr;
		if(pNode->pCNode != nullptr){
			tlkapi_tree_removeChild(pNode);
		}
	}
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkapi_tree_removeChildAt
 * Descript: Remove a child node from a specified position under the reference
 *           node.
 * Params:
 *     @pNode[IN]--The parent node.
 *     @index[IN]--The position of the "pPNode->pCNode" to be removed.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *****************************************************************************/
int tlkapi_tree_removeChildAt(tlkapi_tree_t *pPNode, int index)
{
	int offset;
	tlkapi_tree_t *pNode;
	tlkapi_tree_t *pTemp;
	
	if(pPNode == nullptr) return -TLK_EPARAM;
	if(pPNode->pCNode == nullptr) return TLK_ENONE;
	
	offset = 0;
	pTemp = pPNode->pCNode;
	while(pTemp->pNNode->pNNode != nullptr && offset != index){
		offset ++;
		pTemp = pTemp->pNNode;
	}
	pNode = pTemp->pNNode;
	pTemp->pNNode = pNode->pNNode;
	pNode->pNNode = nullptr;
	return tlkapi_tree_removeChild(pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_removeChildFirst
 * Descript: Remove a child node from a first position under the reference node.
 * Params:
 *     @pNode[IN]--The parent node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *****************************************************************************/
int tlkapi_tree_removeChildFirst(tlkapi_tree_t *pPNode)
{
	tlkapi_tree_t *pNode;
	
	if(pPNode == nullptr) return -TLK_EPARAM;
	if(pPNode->pCNode == nullptr) return TLK_ENONE;

	pNode = pPNode->pCNode;
	pPNode->pCNode = pNode->pNNode;
	pNode->pNNode = nullptr;
	return tlkapi_tree_removeChild(pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_removeChildAfter
 * Descript: Remove a child node from a last position under the reference node.
 * Params:
 *     @pNode[IN]--The parent node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *****************************************************************************/
int tlkapi_tree_removeChildAfter(tlkapi_tree_t *pPNode)
{
	tlkapi_tree_t *pNode;
	tlkapi_tree_t *pTemp;
	
	if(pPNode == nullptr) return -TLK_EPARAM;
	if(pPNode->pCNode == nullptr) return TLK_ENONE;

	pTemp = pPNode->pCNode;
	while(pTemp->pNNode->pNNode != nullptr){
		pTemp = pTemp->pNNode;
	}
	pNode = pTemp->pNNode;
	pTemp->pNNode = nullptr;
	return tlkapi_tree_removeChild(pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_getCount
 * Descript: Gets the number of all parallel and child nodes under the input 
 *           node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The number of obtained nodes.
 * Notice:
 *     1.The number of parallel nodes refers to the number of nodes from the
 *       current "pNode->pNNode" starts counting instead of all parallel nodes.
 *       You can obtain the number of all parallel nodes by obtaining the 
 *       number of all children of the parent node.
 *     2.This interface is equivalent to "tlkapi_tree_getNextCount()+
 *       tlkapi_tree_getAllChildCount()".
 *****************************************************************************/
int tlkapi_tree_getCount(tlkapi_tree_t *pNode)
{
	if(pNode == nullptr) return -TLK_EPARAM;
	return tlkapi_tree_getNextCount(pNode)+tlkapi_tree_getAllChildCount(pNode);
}

/******************************************************************************
 * Function: tlkapi_tree_getNextCount
 * Descript: Gets the number of all parallel nodes under the input node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The number of obtained nodes.
 * Notice:
 *     1.The number of parallel nodes refers to the number of nodes from the
 *       current "pNode->pNNode" starts counting instead of all parallel nodes.
 *       You can obtain the number of all parallel nodes by obtaining the 
 *       number of all children of the parent node.
 *****************************************************************************/
int tlkapi_tree_getNextCount(tlkapi_tree_t *pNode)
{
	int count;
	tlkapi_tree_t *pTemp;
	
	if(pNode == nullptr) return -TLK_EPARAM;

	count = 0;
	pTemp = pNode->pNNode;
	while(pTemp != nullptr){
		count ++;
		pTemp = pTemp->pNNode;
	}
	return count;
}

/******************************************************************************
 * Function: tlkapi_tree_getChildCount
 * Descript: Gets the number of child nodes under the input node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The number of obtained nodes.
 * Notice:
 *     1.This interface obtains only the number of child nodes of a layer.
 *****************************************************************************/
int tlkapi_tree_getChildCount(tlkapi_tree_t *pNode)
{
	int count;
	tlkapi_tree_t *pTemp;
	
	if(pNode == nullptr) return -TLK_EPARAM;

	count = 0;
	pTemp = pNode->pCNode;
	while(pTemp != nullptr){
		count ++;
		pTemp = pTemp->pNNode;
	}
	return count;
}

/******************************************************************************
 * Function: tlkapi_tree_getAllChildCount
 * Descript: Gets the number of all child nodes under the input node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The number of obtained nodes.
 * Notice:
 *     1.This interface obtains the number of child nodes of all layer.
 *****************************************************************************/
int tlkapi_tree_getAllChildCount(tlkapi_tree_t *pNode)
{
	int count;
	tlkapi_tree_t *pTemp;
	
	if(pNode == nullptr) return -TLK_EPARAM;

	count = 0;
	pTemp = pNode->pCNode;
	while(pTemp != nullptr){
		count ++;
		count += tlkapi_tree_getAllChildCount(pTemp);
		pTemp = pTemp->pNNode;
	}
	return count;
}

/******************************************************************************
 * Function: tlkapi_tree_getNNode
 * Descript: Gets the next parallel node of the specified node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The obtained node, nullptr means failure or no object.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_getNNode(tlkapi_tree_t *pNode)
{
	if(pNode == nullptr) return nullptr;
	return pNode->pNNode;
}

/******************************************************************************
 * Function: tlkapi_tree_getCNode
 * Descript: Gets the first chile node of the specified node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The obtained node, nullptr means failure or no object.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_getCNode(tlkapi_tree_t *pNode)
{
	if(pNode == nullptr) return nullptr;
	return pNode->pCNode;
}

/******************************************************************************
 * Function: tlkapi_tree_getPNode
 * Descript: Gets the parent node of the specified node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The obtained node, nullptr means failure or no object.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_getPNode(tlkapi_tree_t *pNode)
{
	if(pNode == nullptr) return nullptr;
	return pNode->pPNode;
}

/******************************************************************************
 * Function: tlkapi_tree_pickNextAt
 * Descript: Gets the node at the specified location and parallel to the input
 *           node.
 * Params:
 *     @pRNode[IN]--The reference node.
 *     @index[IN]--The position of the "pPNode->pPNode" to be obtained.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface does not delete the obtained node from the tree.
 *     2.The position is relative and is from "pNode->pNNode" Starts counting. 
 *       If you want to get the absolute location, you can do this by getting
 *       "tlkapi_tree_pickChildAt(...)".
 *     3.The reference node must be the same level as the input node.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_pickNextAt(tlkapi_tree_t *pRNode, int index)
{
	int offset;
	tlkapi_tree_t *pTemp;
	
	if(pRNode == nullptr || pRNode->pNNode == nullptr) return nullptr;
	
	offset = 0;
	pTemp = pRNode;
	while(pTemp->pNNode->pNNode != nullptr && offset != index){
		offset ++;
		pTemp = pTemp->pNNode;
	}
	return pTemp->pNNode;
}

/******************************************************************************
 * Function: tlkapi_tree_pickNextFirst
 * Descript: Gets the node at the first location and parallel to the input node.
 * Params:
 *     @pRNode[IN]--The reference node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface does not delete the obtained node from the tree.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_pickNextFirst(tlkapi_tree_t *pRNode)
{
	if(pRNode == nullptr) return nullptr;
	return pRNode->pNNode;
}

/******************************************************************************
 * Function: tlkapi_tree_pickNextAfter
 * Descript: Gets the node at the last location and parallel to the input node.
 * Params:
 *     @pRNode[IN]--The reference node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface does not delete the obtained node from the tree.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_pickNextAfter(tlkapi_tree_t *pRNode)
{
	tlkapi_tree_t *pTemp;
	
	if(pRNode == nullptr || pRNode->pNNode == nullptr) return nullptr;
	
	pTemp = pRNode;
	while(pTemp->pNNode->pNNode != nullptr){
		pTemp = pTemp->pNNode;
	}
	return pTemp->pNNode;
}

/******************************************************************************
 * Function: tlkapi_tree_pickChildAt
 * Descript: Gets the child node at the specified location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 *     @index[IN]--The position of the "pPNode->pCNode" to be obtained.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface does not delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_pickChildAt(tlkapi_tree_t *pPNode, int index)
{
	int offset;
	tlkapi_tree_t *pTemp;
	
	if(pPNode == nullptr || pPNode->pCNode == nullptr) return nullptr;
		
	offset = 0;
	pTemp = pPNode->pCNode;
	while(pTemp->pNNode->pNNode != nullptr && offset != index){
		offset ++;
		pTemp = pTemp->pNNode;
	}
	return pTemp->pNNode;
}

/******************************************************************************
 * Function: tlkapi_tree_pickChildFirst
 * Descript: Gets the child node at the first location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface does not delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_pickChildFirst(tlkapi_tree_t *pPNode)
{
	if(pPNode == nullptr) return nullptr;
	return pPNode->pCNode;
}

/******************************************************************************
 * Function: tlkapi_tree_pickChildAfter
 * Descript: Gets the child node at the last location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface does not delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_pickChildAfter(tlkapi_tree_t *pPNode)
{
	tlkapi_tree_t *pTemp;
	
	if(pPNode == nullptr || pPNode->pCNode == nullptr) return nullptr;
	
	pTemp = pPNode->pCNode;
	while(pTemp->pNNode->pNNode != nullptr){
		pTemp = pTemp->pNNode;
	}
	return pTemp->pNNode;
}

/******************************************************************************
 * Function: tlkapi_tree_takeNextAt
 * Descript: Gets the node at the specified location and parallel to the input
 *           node.
 * Params:
 *     @pRNode[IN]--The reference node.
 *     @index[IN]--The position of the "pPNode->pPNode" to be obtained.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface will delete the obtained node from the tree.
 *     2.The position is relative and is from "pNode->pNNode" Starts counting. 
 *       If you want to get the absolute location, you can do this by getting
 *       "tlkapi_tree_takeChildAt(...)".
 *     3.The reference node must be the same level as the input node.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_takeNextAt(tlkapi_tree_t *pRNode, int index)
{
	int offset;
	tlkapi_tree_t *pNode;
	tlkapi_tree_t *pTemp;
	
	if(pRNode == nullptr || pRNode->pNNode == nullptr) return nullptr;
	
	offset = 0;
	pTemp = pRNode;
	while(pTemp->pNNode->pNNode != nullptr && offset != index){
		offset ++;
		pTemp = pTemp->pNNode;
	}
	pNode = pTemp->pNNode;
	pTemp->pNNode = pNode->pNNode;
	pNode->pNNode = nullptr;
	return pNode;
}

/******************************************************************************
 * Function: tlkapi_tree_takeNextFirst
 * Descript: Gets the node at the first location and parallel to the input node.
 * Params:
 *     @pRNode[IN]--The reference node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface will delete the obtained node from the tree.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_takeNextFirst(tlkapi_tree_t *pRNode)
{
	tlkapi_tree_t *pNode;
	
	if(pRNode == nullptr || pRNode->pNNode == nullptr) return nullptr;
	
	pNode = pRNode->pNNode;
	pRNode->pNNode = pNode->pNNode;
	pNode->pNNode = nullptr;
	return pNode;
}

/******************************************************************************
 * Function: tlkapi_tree_takeNextAfter
 * Descript: Gets the node at the last location and parallel to the input node.
 * Params:
 *     @pRNode[IN]--The reference node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface will delete the obtained node from the tree.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_takeNextAfter(tlkapi_tree_t *pRNode)
{
	tlkapi_tree_t *pNode;
	tlkapi_tree_t *pTemp;
	
	if(pRNode == nullptr || pRNode->pNNode == nullptr) return nullptr;
	
	pTemp = pRNode;
	while(pTemp->pNNode->pNNode != nullptr){
		pTemp = pTemp->pNNode;
	}
	pNode = pTemp->pNNode;
	pTemp->pNNode = nullptr;
	return pNode;
}

/******************************************************************************
 * Function: tlkapi_tree_takeChildAt
 * Descript: Gets the child node at the specified location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 *     @index[IN]--The position of the "pPNode->pCNode" to be obtained.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface will delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_takeChildAt(tlkapi_tree_t *pPNode, int index)
{
	int offset;
	tlkapi_tree_t *pNode;
	tlkapi_tree_t *pTemp;
	
	if(pPNode == nullptr || pPNode->pCNode == nullptr) return nullptr;
		
	offset = 0;
	pTemp = pPNode->pCNode;
	while(pTemp->pNNode->pNNode != nullptr && offset != index){
		offset ++;
		pTemp = pTemp->pNNode;
	}
	pNode = pTemp->pNNode;
	pTemp->pNNode = pNode->pNNode;
	pNode->pNNode = nullptr;
	return pNode;
}

/******************************************************************************
 * Function: tlkapi_tree_takeChildFirst
 * Descript: Gets the child node at the first location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface will delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_takeChildFirst(tlkapi_tree_t *pPNode)
{
	tlkapi_tree_t *pNode;
	
	if(pPNode == nullptr || pPNode->pCNode == nullptr) return nullptr;
	
	pNode = pPNode->pCNode;
	pPNode->pCNode = pNode->pNNode;
	pNode->pNNode = nullptr;
	return pNode;
}

/******************************************************************************
 * Function: tlkapi_tree_takeChildAfter
 * Descript: Gets the child node at the last location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface will delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_takeChildAfter(tlkapi_tree_t *pPNode)
{
	tlkapi_tree_t *pNode;
	tlkapi_tree_t *pTemp;
	
	if(pPNode == nullptr || pPNode->pCNode == nullptr) return nullptr;
	
	pTemp = pPNode->pCNode;
	while(pTemp->pNNode->pNNode != nullptr){
		pTemp = pTemp->pNNode;
	}
	pNode = pTemp->pNNode;
	pTemp->pNNode = nullptr;
	return pNode;
}


