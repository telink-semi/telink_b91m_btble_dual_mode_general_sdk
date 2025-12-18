/**
 * @file lvgl.h
 * This file exists only to be compatible with Arduino's library structure
 */

#ifndef LVGL_SRC_H
#define LVGL_SRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../tlkgui_config.h"
#if (TLKGUI_LVGL_ENABLE)

/***************************
 * CURRENT VERSION OF LVGL
 ***************************/
#define LVGL_VERSION_MAJOR 8
#define LVGL_VERSION_MINOR 3
#define LVGL_VERSION_PATCH 0
#define LVGL_VERSION_INFO "8.3.0"

/*********************
 *      INCLUDES
 *********************/

#include "tlklib/gui/lvgl/misc/lv_log.h"
#include "tlklib/gui/lvgl/misc/lv_timer.h"
#include "tlklib/gui/lvgl/misc/lv_math.h"
#include "tlklib/gui/lvgl/misc/lv_mem.h"
#include "tlklib/gui/lvgl/misc/lv_async.h"
#include "tlklib/gui/lvgl/misc/lv_anim_timeline.h"
#include "tlklib/gui/lvgl/misc/lv_printf.h"

#include "tlklib/gui/lvgl/hal/lv_hal.h"

#include "tlklib/gui/lvgl/core/lv_obj.h"
#include "tlklib/gui/lvgl/core/lv_group.h"
#include "tlklib/gui/lvgl/core/lv_indev.h"
#include "tlklib/gui/lvgl/core/lv_refr.h"
#include "tlklib/gui/lvgl/core/lv_disp.h"
#include "tlklib/gui/lvgl/core/lv_theme.h"

#include "tlklib/gui/lvgl/font/lv_font.h"
#include "tlklib/gui/lvgl/font/lv_font_loader.h"
#include "tlklib/gui/lvgl/font/lv_font_fmt_txt.h"

#include "tlklib/gui/lvgl/widgets/lv_arc.h"
#include "tlklib/gui/lvgl/widgets/lv_btn.h"
#include "tlklib/gui/lvgl/widgets/lv_img.h"
#include "tlklib/gui/lvgl/widgets/lv_label.h"
#include "tlklib/gui/lvgl/widgets/lv_line.h"
#include "tlklib/gui/lvgl/widgets/lv_table.h"
#include "tlklib/gui/lvgl/widgets/lv_checkbox.h"
#include "tlklib/gui/lvgl/widgets/lv_bar.h"
#include "tlklib/gui/lvgl/widgets/lv_slider.h"
#include "tlklib/gui/lvgl/widgets/lv_btnmatrix.h"
#include "tlklib/gui/lvgl/widgets/lv_dropdown.h"
#include "tlklib/gui/lvgl/widgets/lv_roller.h"
#include "tlklib/gui/lvgl/widgets/lv_textarea.h"
#include "tlklib/gui/lvgl/widgets/lv_canvas.h"
#include "tlklib/gui/lvgl/widgets/lv_switch.h"

#include "tlklib/gui/lvgl/draw/lv_draw.h"

#include "tlklib/gui/lvgl/lv_api_map.h"

#include "porting/lv_port_disp.h"
#include "porting/lv_port_fs_template.h"
#include "porting/lv_port_indev_template.h"
/*-----------------
 * EXTRAS
 *----------------*/
#include "tlklib/gui/lvgl/extra/lv_extra.h"

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

/** Gives 1 if the x.y.z version is supported in the current version
 * Usage:
 *
 * - Require v6
 * #if LV_VERSION_CHECK(6,0,0)
 *   new_func_in_v6();
 * #endif
 *
 *
 * - Require at least v5.3
 * #if LV_VERSION_CHECK(5,3,0)
 *   new_feature_from_v5_3();
 * #endif
 *
 *
 * - Require v5.3.2 bugfixes
 * #if LV_VERSION_CHECK(5,3,2)
 *   bugfix_in_v5_3_2();
 * #endif
 *
 */
#define LV_VERSION_CHECK(x,y,z) (x == LVGL_VERSION_MAJOR && (y < LVGL_VERSION_MINOR || (y == LVGL_VERSION_MINOR && z <= LVGL_VERSION_PATCH)))

/**
 * Wrapper functions for VERSION macros
 */

static inline int lv_version_major(void)
{
    return LVGL_VERSION_MAJOR;
}

static inline int lv_version_minor(void)
{
    return LVGL_VERSION_MINOR;
}

static inline int lv_version_patch(void)
{
    return LVGL_VERSION_PATCH;
}

static inline const char *lv_version_info(void)
{
    return LVGL_VERSION_INFO;
}


#endif //#if (TLKGUI_LVGL_ENABLE)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_SRC_H*/
