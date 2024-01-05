/**
 * @file lv_gif.h
 *
 */

#ifndef LV_GIF_H
#define LV_GIF_H

#ifdef __cplusplus
extern "C" {
#endif


#include "../../../../tlkgui_config.h"
#if (TLKGUI_LVGL_ENABLE)

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"
#if LV_USE_GIF

#include "gifdec.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_img_t img;
    gd_GIF * gif;
    lv_timer_t * timer;
    lv_img_dsc_t imgdsc;
    uint32_t last_call;
} lv_gif_t;

extern const lv_obj_class_t lv_gif_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_gif_create(lv_obj_t * parent);
void lv_gif_set_src(lv_obj_t * obj, const void * src);
void lv_gif_restart(lv_obj_t * gif);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GIF*/



#endif //#if (TLKGUI_LVGL_ENABLE)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GIF_H*/
