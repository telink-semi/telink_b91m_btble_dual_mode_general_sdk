/**
 * @file lv_bmp.h
 *
 */

#ifndef LV_BMP_H
#define LV_BMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../../tlkgui_config.h"
#if (TLKGUI_LVGL_ENABLE)

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"
#if LV_USE_BMP

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_bmp_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BMP*/


#endif //#if (TLKGUI_LVGL_ENABLE)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BMP_H*/
