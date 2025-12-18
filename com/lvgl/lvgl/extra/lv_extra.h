/**
 * @file lv_extra.h
 *
 */

#ifndef LV_EXTRA_H
#define LV_EXTRA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../tlkgui_config.h"
#if (TLKGUI_LVGL_ENABLE)

/*********************
 *      INCLUDES
 *********************/

#include "layouts/lv_layouts.h"
#include "libs/lv_libs.h"
#include "others/lv_others.h"
#include "themes/lv_themes.h"
#include "widgets/lv_widgets.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the extra components
 */
void lv_extra_init(void);

/**********************
 *      MACROS
 **********************/


#endif //#if (TLKGUI_LVGL_ENABLE)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_EXTRA_H*/
