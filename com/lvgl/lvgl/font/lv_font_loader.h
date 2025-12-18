/**
 * @file lv_font_loader.h
 *
 */

#ifndef LV_FONT_LOADER_H
#define LV_FONT_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../tlkgui_config.h"
#if (TLKGUI_LVGL_ENABLE)

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_font_t * lv_font_load(const char * fontName);
void lv_font_free(lv_font_t * font);

/**********************
 *      MACROS
 **********************/




#endif //#if (TLKGUI_LVGL_ENABLE)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_LOADER_H*/
