/********************************************************************************************************
 * @file	tlkapi_save.h
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
#ifndef TLKAPI_SAVE_H
#define TLKAPI_SAVE_H


/******************************************************************************
 * Note: tlkapi_save is a method interface provided by SDK for users to quickly
 *       save user information to flash. There are currently four ways to save
 *       parameters to flash.
 *
 * save1: Save a single item in a single sector. The length of Item can change
 *        dynamically. However, in the process of saving, the saved data will 
 *        be lost in case of abnormal power failure.
 *
 * save2: Save a single item in a single sector. The length of Item is fixed,
 *        During the saving process, the saved data will be automatically 
 *        restored in case of abnormal power failure. However, if an abnormal 
 *        power failure occurs during erasure, data will be lost.
 * 
 * save3: Save a single item in double sectors. The length of Item is fixed,
 *        During the saving process, the saved data will be automatically 
 *        restored in case of abnormal power failure.
 * 
 * save4: Save multiple items in double sectors. The length of Item is fixed,
 *        During the saving process, the saved data will be automatically 
 *        restored in case of abnormal power failure.
 *****************************************************************************/


/******************************************************************************
 * Object: tlkapi_save_ctrl_t
 * Brief : Control block that save's method information.
 * Params:
 *     @sign--Save method label, identifier for user information.
 *     @vers--Version number of the current stored format.
 *     @prev--Offset location of the last saved information in sector("addr").
 *     @lens--The length of each item in the information to be saved.
 *     @offs--The offset of the current sector operation.
 *     @addr--The address of the current sector operation.
 *     @addr0--The address of the first backup sector of the system.
 *     @addr1--The second of the first backup sector of the system.
 *****************************************************************************/
typedef struct{
	uint08 sign;
	uint08 vers;
	uint16 prev;
	uint16 lens; //=4+Item Length (4=sign+version+length)
	uint16 offs; //Cur Offset
	uint32 addr; //Flash Start Address
	uint32 addr0;
	uint32 addr1;
}tlkapi_save_ctrl_t;

/******************************************************************************
 * Object: tlkapi_save_item_t
 * Brief : Control block that save's item information.
 * Params:
 *     @address--The offset address of the item in the sector. 
 *     @dataLen--The length of the data of the item. 
 *     @buffLen--The length of the buffer of the item.
 *     @pBuffer--The cache of the item.
 *****************************************************************************/
typedef struct{
	uint32 address;
	uint16 dataLen;
	uint16 buffLen;
	uint08 *pBuffer;
}tlkapi_save_item_t;


/******************************************************************************
 * Function: tlkapi_save1_init
 * Descript: Initializes the save method control parameters.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @sign[IN]--Information identification tag.
 *     @version[IN]--The version of the save method.
 *     @address[IN]--The address of the sector to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This method is used to save the variable length of a single sector. 
 *       If an abnormal power failure occurs during the saving process, the 
 *       corresponding information will be deleted.
 *     2.If the "sign" does not match the loaded item in flash, the sector will
 *       be erased.
 *     3.If the "version" is different from the loaded item in flash, the 
 *       sector will be erased.
 *****************************************************************************/
int tlkapi_save1_init(tlkapi_save_ctrl_t *pCtrl, uint08 sign, uint08 version, uint32 address);

/******************************************************************************
 * Function: tlkapi_save1_load
 * Descript: Load user saved information from flash.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pBuff[OUT]--A buffer that stores load information.
 *     @buffLen[IN]--The length of the buffer where the info is stored.
 * Return: The length of the information loaded. Negative value means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save1_init()".
 *****************************************************************************/
int tlkapi_save1_load(tlkapi_save_ctrl_t *pCtrl, uint08 *pBuff, uint16 buffLen);

/******************************************************************************
 * Function: tlkapi_save1_save
 * Descript: Save user information to the flash.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pData[IN]--The data to be saved.
 *     @dataLen[IN]--The length of the data to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save1_init()".
 *     2.If the saved information exceeds the available space of the sector, 
 *       the saving fails.
 *****************************************************************************/
int tlkapi_save1_save(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_save1_clean
 * Descript: Clean up the saved information.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 * Return: None.
 * Notice:
 *     1.This interface should be called after "tlkapi_save1_init()".
 *     2.This interface erases the corresponding sector and does not verify the
 *       flash address, so it needs to be called after initialization.
 *****************************************************************************/
void tlkapi_save1_clean(tlkapi_save_ctrl_t *pCtrl);

/******************************************************************************
 * Function: tlkapi_save1_migrate
 * Descript: Save user information in migration mode.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pData[IN]--The data to be saved.
 *     @dataLen[IN]--The length of the data to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save1_init()".
 *     2.Different from "tlkapi_save1_save()", the interface first calls the
 *       function that erasing the sector and then saves the information, that
 *       is, the interface always saves the user information at the original 
 *       location of the sector.
 *****************************************************************************/
int tlkapi_save1_migrate(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_save1_smartSave
 * Descript: Save user information in smart mode.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pData[IN]--The data to be saved.
 *     @dataLen[IN]--The length of the data to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save1_init()".
 *     2.This interface acts as a combined control application of 
 *       "tlkapi_save1_save()" and "tlkapi_save1_migrate()", which 
 *       automatically calls tlkapi_save1_migrate() to save data when the 
 *       sector is full.
 *****************************************************************************/
int tlkapi_save1_smartSave(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);


/******************************************************************************
 * Function: tlkapi_save2_init
 * Descript: Initializes the save method control parameters.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @sign[IN]--Information identification tag.
 *     @length[IN]--The length of each item in the save's method.
 *     @version[IN]--The version of the save method.
 *     @address[IN]--The address of the sector to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.Save a single item in a single sector. The length of Item is fixed,
 *       During the saving process, the saved data will be automatically 
 *       restored in case of abnormal power failure. However, if an abnormal 
 *       power failure occurs during erasure, data will be lost.
 *     2.If the "sign" does not match the loaded item in flash, the sector will
 *       be erased.
 *     3.If the "version" is different from the loaded item in flash, the 
 *       sector will be erased.
 *****************************************************************************/
int tlkapi_save2_init(tlkapi_save_ctrl_t *pCtrl, uint08 sign, uint08 version,
	uint16 length, uint32 address);

/******************************************************************************
 * Function: tlkapi_save2_load
 * Descript: Load user saved information from flash.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pBuff[OUT]--A buffer that stores load information.
 *     @buffLen[IN]--The length of the buffer where the info is stored.
 * Return: The length of the information loaded. Negative value means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save2_init()".
 *****************************************************************************/
int tlkapi_save2_load(tlkapi_save_ctrl_t *pCtrl, uint08 *pBuff, uint16 buffLen);

/******************************************************************************
 * Function: tlkapi_save2_save
 * Descript: Save user information to the flash.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pData[IN]--The data to be saved.
 *     @dataLen[IN]--The length of the data to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save2_init()".
 *     2.If the saved information exceeds the available space of the sector, 
 *       the saving fails.
 *****************************************************************************/
int tlkapi_save2_save(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_save2_clean
 * Descript: Clean up the saved information.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 * Return: None.
 * Notice:
 *     1.This interface should be called after "tlkapi_save2_init()".
 *     2.This interface erases the corresponding sector and does not verify the
 *       flash address, so it needs to be called after initialization.
 *****************************************************************************/
void tlkapi_save2_clean(tlkapi_save_ctrl_t *pCtrl);

/******************************************************************************
 * Function: tlkapi_save2_migrate
 * Descript: Save user information in migration mode.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pData[IN]--The data to be saved.
 *     @dataLen[IN]--The length of the data to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save2_init()".
 *     2.Different from "tlkapi_save2_save()", the interface first calls the
 *       function that erasing the sector and then saves the information, that
 *       is, the interface always saves the user information at the original 
 *       location of the sector.
 *****************************************************************************/
int tlkapi_save2_migrate(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_save2_smartSave
 * Descript: Save user information in smart mode.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pData[IN]--The data to be saved.
 *     @dataLen[IN]--The length of the data to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save2_init()".
 *     2.This interface acts as a combined control application of 
 *       "tlkapi_save2_save()" and "tlkapi_save2_migrate()", which 
 *       automatically calls tlkapi_save2_migrate() to save data when the 
 *       sector is full.
 *****************************************************************************/
int tlkapi_save2_smartSave(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);


/******************************************************************************
 * Function: tlkapi_save3_init
 * Descript: Initializes the save method control parameters.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @sign[IN]--Information identification tag.
 *     @length[IN]--The length of each item in the save's method.
 *     @version[IN]--The version of the save method.
 *     @address0[IN]--The address of the first backup sector of the system.
 *     @address1[IN]--The address of the second backup sector of the system.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.Save a single item in double sectors. The length of Item is fixed,
 *       During the saving process, the saved data will be automatically 
 *       restored in case of abnormal power failure.
 *     2.If the "sign" does not match the loaded item in flash, the sector will
 *       be erased.
 *     3.If the "version" is different from the loaded item in flash, the 
 *       sector will be erased.
 *     4.This interface is used to initialize control block parameters, traverse
 *       automatically the two sectors, find the sector containing valid 
 *       information, and record the sector address. If there is no valid 
 *       information, the default "address0" is used as the current save sector.
 *     5.If data migration is terminated abnormally (such as power failure) 
 *       during the last migration, the interface continues to migrate data.
 *****************************************************************************/
int tlkapi_save3_init(tlkapi_save_ctrl_t *pCtrl, uint08 sign, uint08 version,
	uint16 length, uint32 address0, uint32 address1);

/******************************************************************************
 * Function: tlkapi_save3_load
 * Descript: Load user saved information from flash.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pBuff[OUT]--A buffer that stores load information.
 *     @buffLen[IN]--The length of the buffer where the info is stored.
 * Return: The length of the information loaded. Negative value means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save3_init()".
 *****************************************************************************/
int tlkapi_save3_load(tlkapi_save_ctrl_t *pCtrl, uint08 *pBuff, uint16 buffLen);

/******************************************************************************
 * Function: tlkapi_save3_save
 * Descript: Save user information to the flash.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pData[IN]--The data to be saved.
 *     @dataLen[IN]--The length of the data to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save3_init()".
 *     2.If the saved information exceeds the available space of the sector, 
 *       the saving fails.
 *****************************************************************************/
int tlkapi_save3_save(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_save3_clean
 * Descript: Clean up the saved information.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 * Return: None.
 * Notice:
 *     1.This interface should be called after "tlkapi_save3_init()".
 *     2.This interface erases the corresponding sector and does not verify the
 *       flash address, so it needs to be called after initialization.
 *****************************************************************************/
void tlkapi_save3_clean(tlkapi_save_ctrl_t *pCtrl);

/******************************************************************************
 * Function: tlkapi_save3_migrate
 * Descript: Save user information in migration mode.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pData[IN]--The data to be saved.
 *     @dataLen[IN]--The length of the data to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save3_init()".
 *     2.Different from "tlkapi_save3_save()", the interface first calls the
 *       function that erasing the sector and then saves the information, that
 *       is, the interface always saves the user information at the original 
 *       location of the sector.
 *****************************************************************************/
int tlkapi_save3_migrate(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkapi_save3_smartSave
 * Descript: Save user information in smart mode.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pData[IN]--The data to be saved.
 *     @dataLen[IN]--The length of the data to be saved.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save3_init()".
 *     2.This interface acts as a combined control application of 
 *       "tlkapi_save3_save()" and "tlkapi_save3_migrate()", which 
 *       automatically calls tlkapi_save3_migrate() to save data when the 
 *       sector is full.
 *****************************************************************************/
int tlkapi_save3_smartSave(tlkapi_save_ctrl_t *pCtrl, uint08 *pData, uint16 dataLen);


/******************************************************************************
 * Function: tlkapi_save4_init
 * Descript: Initializes the save method control parameters.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @sign[IN]--Information identification tag.
 *     @length[IN]--The length of each item in the save's method.
 *     @version[IN]--The version of the save method.
 *     @address0[IN]--The address of the first backup sector of the system.
 *     @address1[IN]--The address of the second backup sector of the system.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.Save multiple items in double sectors. The length of Item is fixed,
 *       During the saving process, the saved data will be automatically 
 *       restored in case of abnormal power failure.
 *     2.If the "sign" does not match the loaded item in flash, the sector will
 *       be erased.
 *     3.If the "version" is different from the loaded item in flash, the 
 *       sector will be erased.
 *     4.This interface is used to initialize control block parameters, traverse
 *       automatically the two sectors, find the sector containing valid 
 *       information, and record the sector address. If there is no valid 
 *       information, the default "address0" is used as the current save sector.
 *     5.If data migration is terminated abnormally (such as power failure) 
 *       during the last migration, the interface continues to migrate data.
 *****************************************************************************/
int tlkapi_save4_init(tlkapi_save_ctrl_t *pCtrl, uint08 sign, uint08 version, 
	uint16 length, uint32 address0, uint32 address1);

/******************************************************************************
 * Function: tlkapi_save4_load
 * Descript: Load user saved information from flash.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pItems[IN/OUT]--The control block of each item.
 *     @itemCount[IN]--The number of the "pItems".
 *     @indexOffs[IN]--The index offset of the valid items in sector.
 * Return: The number of the item loaded. Negative value means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save4_init()".
 *****************************************************************************/
int tlkapi_save4_load(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems,
	uint16 itemCount, uint16 indexOffs);

/******************************************************************************
 * Function: tlkapi_save4_save
 * Descript: Save user information to the flash.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pItems[IN/OUT]--The control block of each item.
 *     @itemCount[IN]--The number of the "pItems".
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save4_init()".
 *     2.If the saved information exceeds the available space of the sector, 
 *       the saving fails.
 *****************************************************************************/
int tlkapi_save4_save(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems,
	uint16 itemCount);

/******************************************************************************
 * Function: tlkapi_save4_clean
 * Descript: Clean up the saved information.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 * Return: None.
 * Notice:
 *     1.This interface should be called after "tlkapi_save4_init()".
 *     2.This interface erases the corresponding sector and does not verify the
 *       flash address, so it needs to be called after initialization.
 *****************************************************************************/
void tlkapi_save4_clean(tlkapi_save_ctrl_t *pCtrl);

/******************************************************************************
 * Function: tlkapi_save4_remove
 * Descript: Remove the saved information.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pItems[IN]--The control block of each item.
 *     @itemCount[IN]--The number of the "pItems".
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save4_init()".
 *****************************************************************************/
int tlkapi_save4_remove(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems,
	uint16 itemCount);

/******************************************************************************
 * Function: tlkapi_save4_modify
 * Descript: Modify the saved information.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pItems[IN]--The control block of each item.
 *     @offset[IN]--The offset of the "pItem->pBuffer".
 *     @length[IN]--The length of the info to be modified.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save4_init()".
 *     2."length" is an absolute length, so buffLen needs to be greater than 
 *       ("offset"+"length").
 *     3.The information to be modified is valid only if the change from bit1
 *       to bit0.
 *     4.A flash page cannot be written more than 64 times after erasing.
 *****************************************************************************/
int tlkapi_save4_modify(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems,
	uint16 offset, uint16 length);

/******************************************************************************
 * Function: tlkapi_save4_migrate
 * Descript: Save user information in migration mode.
 * Params:
 *     @pCtrl[IN]--The control block of the save method.
 *     @pItems[IN]--The control block of each item.
 *     @itemCount[IN]--The number of the "pItems".
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Notice:
 *     1.This interface should be called after "tlkapi_save4_init()".
 *     2.Different from "tlkapi_save4_save()", the interface first calls the
 *       function that erasing the sector and then saves the information, that
 *       is, the interface always saves the user information at the original 
 *       location of the sector.
 *     3.After this interface is used, the system automatically switches the 
 *       flash sector in which information is saved and erases the current
 *       sector.
 *****************************************************************************/
int tlkapi_save4_migrate(tlkapi_save_ctrl_t *pCtrl, tlkapi_save_item_t *pItems,
	uint16 itemCount);


#endif //TLKAPI_SAVE_H

