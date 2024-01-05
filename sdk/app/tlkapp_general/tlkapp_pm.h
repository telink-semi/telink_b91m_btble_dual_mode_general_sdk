/********************************************************************************************************
 * @file	tlkapp_pm.h
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
#ifndef TLKAPP_PM_H
#define TLKAPP_PM_H


#if(TLK_CFG_PM_ENABLE)


typedef enum{
	TLKAPP_PM_STATE_IDLE = 0,
	TLKAPP_PM_STATE_SLEEP,
}TLKAPP_PM_STAGE_ENUM;

/******************************************************************************
 * Function: tlkapp_pm_init
 * Descript: This function for initial the pm module.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int  tlkapp_pm_init(void);

/******************************************************************************
 * Function: tlkapp_pm_handler
 * Descript: Callback for sdk.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapp_pm_handler(void);



#endif

#endif //TLKAPP_PM_H

