/********************************************************************************************************
 * @file	tlkgui.c
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
#if (TLK_CFG_GUI_ENABLE)
#include "tlklib/gui/tlkgui_stdio.h"
#include "tlklib/gui/tlkgui.h"
#include "tlklib/gui/lvgl/lvgl.h"
#include "tlklib/gui/lv_examples/lv_examples.h"

#include "drivers.h"


/******************************************************************************
 * Function: tlkgui_init
 * Descript: 
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkgui_init(void)
{
#if (TLKGUI_LVGL_ENABLE)
	lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

	lv_gui_examples();
#endif
	
	return TLK_ENONE;
}


void tlkgui_handler(void)
{
#if (TLKGUI_LVGL_ENABLE)
	lv_timer_handler();
#endif	
}





#endif //#if (TLK_CFG_GUI_ENABLE)

