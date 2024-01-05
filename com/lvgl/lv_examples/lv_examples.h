/********************************************************************************************************
 * @file     lv_examples.h
 *
 * @brief    This is the source file for BTBLE SDK
 *
 * @author	 BTBLE GROUP
 * @date         2,2022
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#ifndef VENDOR_DISPLAY_DEMO_LV_EXAMPLES_LV_EXAMPLES_H_
#define VENDOR_DISPLAY_DEMO_LV_EXAMPLES_LV_EXAMPLES_H_

#include "tlkapi/tlkapi_stdio.h"
#if( TLK_CFG_GUI_ENABLE )
#include "../lvgl/lvgl.h"
#include "lv_examples.h"
#include "../lvgl/porting/lv_port_disp.h"
#include "../lvgl/core/lv_event.h"
#include "../lvgl/core/lv_obj.h"
#include "../lvgl/extra/widgets/calendar/lv_calendar.h"
#include "../lvgl/widgets/lv_label.h"
#include "lv_ui_offset.h"

#define SDL_HOR_RES		MY_DISP_HOR_RES
#define SDL_VER_RES		MY_DISP_VER_RES

#define MY_LVGL_DEMO_DRAG_EN	1

void lv_gui_examples();

//from simulator
void page_tabview(void);

void page_dialPlate();
void lv_example_appList1();
void lv_example_appList2();

#endif
#endif /* VENDOR_DISPLAY_DEMO_LV_EXAMPLES_LV_EXAMPLES_H_ */
