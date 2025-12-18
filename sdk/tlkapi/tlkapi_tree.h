/********************************************************************************************************
 * @file	tlkapi_tree.h
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
#ifndef TLKAPI_TREE_H
#define TLKAPI_TREE_H


// TODO: wait for verification

/******************************************************************************
 * The node structure of the tree.
 *****************************************************************************/
typedef struct tlkapi_tree_s{
	struct tlkapi_tree_s *pNNode; //Next Node, Parallel grade tree
	struct tlkapi_tree_s *pCNode; //Child Node
	struct tlkapi_tree_s *pPNode; //Parent Node
}tlkapi_tree_t;


/******************************************************************************
 * Function: tlkapi_tree_init
 * Descript: Initialize a tree node to defaults values.
 * Params:
 *     @pNode[IN]--Pointer to the tree node structure.
 * Return: Return the initialization result, TLK ENONE means success.
 *****************************************************************************/
int tlkapi_tree_init(tlkapi_tree_t *pNode);

/******************************************************************************
 * Function: tlkapi_tree_print
 * Descript: Prints all nodes contained in the input node.
 * Params:
 *     @pNode[IN]--The node to be printed.
 * Return: None.
 *****************************************************************************/
void tlkapi_tree_print(tlkapi_tree_t *pNode);

/******************************************************************************
 * Function: tlkapi_tree_printNext
 * Descript: Prints all the parallel nodes contained in the input node.
 * Params:
 *     @pRNode[IN]--The reference node to be printed.
 * Return: None.
 *****************************************************************************/
void tlkapi_tree_printNext(tlkapi_tree_t *pRNode);

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
void tlkapi_tree_printChild(tlkapi_tree_t *pPNode, int deep);

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
void tlkapi_tree_printAllChild(tlkapi_tree_t *pPNode, int deep);

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
int tlkapi_tree_insertNext(tlkapi_tree_t *pRNode, tlkapi_tree_t *pNode);

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
int tlkapi_tree_insertNextAt(tlkapi_tree_t *pRNode, tlkapi_tree_t *pNode, int index);

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
int tlkapi_tree_insertNextFirst(tlkapi_tree_t *pRNode, tlkapi_tree_t *pNode);

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
int tlkapi_tree_insertNextAfter(tlkapi_tree_t *pRNode, tlkapi_tree_t *pNode);

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
int tlkapi_tree_insertChild(tlkapi_tree_t *pPNode, tlkapi_tree_t *pNode);

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
int tlkapi_tree_insertChildAt(tlkapi_tree_t *pPNode, tlkapi_tree_t *pNode, int index);

/******************************************************************************
 * Function: tlkapi_tree_insertChildFirst
 * Descript: Inserts the input node into the first location under the parent
 *           node.
 * Params:
 *     @pPNode[IN]--The parent node.
 *     @pNode[IN]--The node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_tree_insertChildFirst(tlkapi_tree_t *pPNode, tlkapi_tree_t *pNode);

/******************************************************************************
 * Function: tlkapi_tree_insertChildAfter
 * Descript: Inserts the input node into the last location under the parent
 *           node.
 * Params:
 *     @pPNode[IN]--The parent node.
 *     @pNode[IN]--The node to be inserted.
 * Return: The operation result, TLK_ENONE means success.
 *****************************************************************************/
int tlkapi_tree_insertChildAfter(tlkapi_tree_t *pPNode, tlkapi_tree_t *pNode);

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
int tlkapi_tree_removeNode(tlkapi_tree_t *pNode);

/******************************************************************************
 * Function: tlkapi_tree_removeNextAt
 * Descript: Remove a parallel node at a specified position under the reference
 *           node.
 * Params:
 *     @pRNode[IN]--The reference node.
 *     @index[IN]--The position of the "pRNode->pNNode" to be removed.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_removeNextAt(tlkapi_tree_t *pRNode, int index);

/******************************************************************************
 * Function: tlkapi_tree_removeNextFirst
 * Descript: Remove a parallel node at a first position under the reference
 *           node.
 * Params:
 *     @pRNode[IN]--The reference node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_removeNextFirst(tlkapi_tree_t *pRNode);

/******************************************************************************
 * Function: tlkapi_tree_removeNextAfter
 * Descript: Remove a parallel node at a last position under the reference node.
 * Params:
 *     @pRNode[IN]--The reference node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *     2.The reference node must be the same level as the input node.
 *****************************************************************************/
int tlkapi_tree_removeNextAfter(tlkapi_tree_t *pRNode);

/******************************************************************************
 * Function: tlkapi_tree_removeChild
 * Descript: Remove the child node of the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *****************************************************************************/
int tlkapi_tree_removeChild(tlkapi_tree_t *pPNode);

/******************************************************************************
 * Function: tlkapi_tree_removeChildAt
 * Descript: Remove a child node from a specified position under the reference
 *           node.
 * Params:
 *     @pPNode[IN]--The parent node.
 *     @index[IN]--The position of the "pPNode->pCNode" to be removed.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *****************************************************************************/
int tlkapi_tree_removeChildAt(tlkapi_tree_t *pPNode, int index);

/******************************************************************************
 * Function: tlkapi_tree_removeChildFirst
 * Descript: Remove a child node from a first position under the reference node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *****************************************************************************/
int tlkapi_tree_removeChildFirst(tlkapi_tree_t *pPNode);

/******************************************************************************
 * Function: tlkapi_tree_removeChildAfter
 * Descript: Remove a child node from a last position under the reference node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The operation result, TLK_ENONE means success.
 * Notice:
 *     1.This interface will remove all child nodes under the specified node.
 *****************************************************************************/
int tlkapi_tree_removeChildAfter(tlkapi_tree_t *pPNode);

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
int tlkapi_tree_getCount(tlkapi_tree_t *pNode);

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
int tlkapi_tree_getNextCount(tlkapi_tree_t *pNode);

/******************************************************************************
 * Function: tlkapi_tree_getChildCount
 * Descript: Gets the number of child nodes under the input node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The number of obtained nodes.
 * Notice:
 *     1.This interface obtains only the number of child nodes of a layer.
 *****************************************************************************/
int tlkapi_tree_getChildCount(tlkapi_tree_t *pNode);

/******************************************************************************
 * Function: tlkapi_tree_getAllChildCount
 * Descript: Gets the number of all child nodes under the input node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The number of obtained nodes.
 * Notice:
 *     1.This interface obtains the number of child nodes of all layer.
 *****************************************************************************/
int tlkapi_tree_getAllChildCount(tlkapi_tree_t *pNode);

/******************************************************************************
 * Function: tlkapi_tree_getNNode
 * Descript: Gets the next parallel node of the specified node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The obtained node, nullptr means failure or no object.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_getNNode(tlkapi_tree_t *pNode);

/******************************************************************************
 * Function: tlkapi_tree_getCNode
 * Descript: Gets the first chile node of the specified node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The obtained node, nullptr means failure or no object.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_getCNode(tlkapi_tree_t *pNode);

/******************************************************************************
 * Function: tlkapi_tree_getPNode
 * Descript: Gets the parent node of the specified node.
 * Params:
 *     @pNode[IN]--The specified node to be operated.
 * Return: The obtained node, nullptr means failure or no object.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_getPNode(tlkapi_tree_t *pNode);

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
tlkapi_tree_t *tlkapi_tree_pickNextAt(tlkapi_tree_t *pRNode, int index);

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
tlkapi_tree_t *tlkapi_tree_pickNextFirst(tlkapi_tree_t *pRNode);

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
tlkapi_tree_t *tlkapi_tree_pickNextAfter(tlkapi_tree_t *pRNode);

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
tlkapi_tree_t *tlkapi_tree_pickChildAt(tlkapi_tree_t *pPNode, int index);

/******************************************************************************
 * Function: tlkapi_tree_pickChildFirst
 * Descript: Gets the child node at the first location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface does not delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_pickChildFirst(tlkapi_tree_t *pPNode);

/******************************************************************************
 * Function: tlkapi_tree_pickChildAfter
 * Descript: Gets the child node at the last location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface does not delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_pickChildAfter(tlkapi_tree_t *pPNode);

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
tlkapi_tree_t *tlkapi_tree_takeNextAt(tlkapi_tree_t *pRNode, int index);

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
tlkapi_tree_t *tlkapi_tree_takeNextFirst(tlkapi_tree_t *pRNode);

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
tlkapi_tree_t *tlkapi_tree_takeNextAfter(tlkapi_tree_t *pRNode);

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
tlkapi_tree_t *tlkapi_tree_takeChildAt(tlkapi_tree_t *pPNode, int index);

/******************************************************************************
 * Function: tlkapi_tree_takeChildFirst
 * Descript: Gets the child node at the first location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface will delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_takeChildFirst(tlkapi_tree_t *pPNode);

/******************************************************************************
 * Function: tlkapi_tree_takeChildAfter
 * Descript: Gets the child node at the last location under the input node.
 * Params:
 *     @pPNode[IN]--The parent node.
 * Return: The obtained node, nullptr means failure or no object.
 * Notice:
 *     1.This interface will delete the obtained node from the tree.
 *****************************************************************************/
tlkapi_tree_t *tlkapi_tree_takeChildAfter(tlkapi_tree_t *pPNode);



#endif //TLKAPI_TREE_H

