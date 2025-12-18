/**
 * @file lv_theme_basic.h
 *
 */

#ifndef LV_THEME_BASIC_H
#define LV_THEME_BASIC_H

#ifdef __cplusplus
extern "C" {
#endif


#include "../../../../tlkgui_config.h"
#if (TLKGUI_LVGL_ENABLE)

/*********************
 *      INCLUDES
 *********************/
#include "../../../core/lv_obj.h"

#if LV_USE_THEME_BASIC

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
 * Initialize the theme
 * @param disp pointer to display to attach the theme
 * @return a pointer to reference this theme later
 */
lv_theme_t * lv_theme_basic_init(lv_disp_t * disp);

/**
* Check if the theme is initialized
* @return true if default theme is initialized, false otherwise
*/
bool lv_theme_basic_is_inited(void);

/**********************
 *      MACROS
 **********************/

#endif



#endif //#if (TLKGUI_LVGL_ENABLE)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_THEME_BASIC_H*/
