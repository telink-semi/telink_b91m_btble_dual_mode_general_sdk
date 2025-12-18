/**
 * @file lv_libs.h
 *
 */

#ifndef LV_LIBS_H
#define LV_LIBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../tlkgui_config.h"
#if (TLKGUI_LVGL_ENABLE)

/*********************
 *      INCLUDES
 *********************/
#include "bmp/lv_bmp.h"
#include "fsdrv/lv_fsdrv.h"
#include "png/lv_png.h"
#include "gif/lv_gif.h"
#include "qrcode/lv_qrcode.h"
#include "sjpg/lv_sjpg.h"
#include "freetype/lv_freetype.h"
#include "rlottie/lv_rlottie.h"
#include "ffmpeg/lv_ffmpeg.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/


#endif //#if (TLKGUI_LVGL_ENABLE)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LIBS_H*/
