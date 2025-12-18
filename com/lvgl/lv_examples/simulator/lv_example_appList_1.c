#include "../lv_examples.h"
#if( TLK_CFG_GUI_ENABLE )

/**
 * Open a BMP file from a file
 */
//LV_IMG_DECLARE(appList_alarm);
//LV_IMG_DECLARE(appList_app_download);
//LV_IMG_DECLARE(appList_calculagraph);
//
//LV_IMG_DECLARE(appList_compass);
//LV_IMG_DECLARE(appList_phoneOff);
//LV_IMG_DECLARE(appList_phoneOn);
//
//
//LV_IMG_DECLARE(appList_poweron);
//LV_IMG_DECLARE(appList_rebootToFactory);
//LV_IMG_DECLARE(appList_shutdown);
//
//LV_IMG_DECLARE(appList_stopWatch);
//LV_IMG_DECLARE(appList_theaterMode);
//LV_IMG_DECLARE(appList_vibrationOff);
//
//LV_IMG_DECLARE(appList_vibrationOn);



lv_obj_t *myAppList1;

static void win_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

    
    if(code == LV_EVENT_FOCUSED) {
		LV_LOG_USER("LV_EVENT_FOCUSED = %d", code);

    }

    if(code == LV_EVENT_DEFOCUSED) {
		LV_LOG_USER("LV_EVENT_DEFOCUSED = %d", code);

    }

    if(code == LV_EVENT_GESTURE) {
		LV_LOG_USER("LV_EVENT_GESTURE = %d", code);
		lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
		LV_LOG_USER("gesture dir = %d", dir);


		if( dir == LV_DIR_LEFT )
		{
			lv_obj_remove_event_cb(myAppList1, win_event_cb);
			lv_obj_clean(myAppList1);

			//to next page
			lv_example_wb_appList2();
		}
		else if( dir == LV_DIR_RIGHT )
		{
			lv_obj_remove_event_cb(myAppList1, win_event_cb);
			lv_obj_clean(myAppList1);

			//to pre page
			page_dialPlate();

		}
    }
    
}



void lv_example_appList1(void)
{
#if !MY_LVGL_DEMO_DRAG_EN
    myAppList1 = lv_scr_act();
    lv_obj_add_event_cb(myAppList1, win_event_cb, LV_EVENT_ALL, NULL);
#endif

	//RAW 1
	lv_obj_t * img1 = lv_img_create(myAppList1);
    lv_img_set_src(img1, MY_LV_IMG_ADDR(APPLIST_ALARM)); //use Telink tool to use this addr
	lv_obj_align(img1, LV_ALIGN_TOP_LEFT, 20, 20);


	lv_obj_t * img2 = lv_img_create(myAppList1);	
    lv_img_set_src(img2, MY_LV_IMG_ADDR(APPLIST_APP_DOWNLOAD));
	lv_obj_align(img2, LV_ALIGN_TOP_MID, 0, 20);

	lv_obj_t * img3 = lv_img_create(myAppList1);
    lv_img_set_src(img3, MY_LV_IMG_ADDR(APPLIST_CALCULAGRAPH));
	lv_obj_align(img3, LV_ALIGN_TOP_RIGHT, -20, 20);

	//RAW 2
	lv_obj_t * img4 = lv_img_create(myAppList1);
    lv_img_set_src(img4, MY_LV_IMG_ADDR(APPLIST_COMPASS));
	lv_obj_align(img4, LV_ALIGN_LEFT_MID, 20, 0);

	lv_obj_t * img5 = lv_img_create(myAppList1);	
    lv_img_set_src(img5, MY_LV_IMG_ADDR(APPLIST_PHONEOFF));
	lv_obj_align(img5, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t * img6 = lv_img_create(myAppList1);
    lv_img_set_src(img6, MY_LV_IMG_ADDR(APPLIST_PHONEON));
	lv_obj_align(img6, LV_ALIGN_RIGHT_MID, -20, 0);

	//RAW 3
	lv_obj_t * img7 = lv_img_create(myAppList1);
    lv_img_set_src(img7, MY_LV_IMG_ADDR(APPLIST_POWERON));
	lv_obj_align(img7, LV_ALIGN_BOTTOM_LEFT, 20, -20);


	lv_obj_t * img8 = lv_img_create(myAppList1);	
    lv_img_set_src(img8, MY_LV_IMG_ADDR(APPLIST_REBOOTTOFACTORY));
	lv_obj_align(img8, LV_ALIGN_BOTTOM_MID, 0, -20);

	lv_obj_t * img9 = lv_img_create(myAppList1);
    lv_img_set_src(img9, MY_LV_IMG_ADDR(APPLIST_SHUTDOWN));
	lv_obj_align(img9, LV_ALIGN_BOTTOM_RIGHT, -20, -20);

	//RAW 4
	lv_obj_t * img10 = lv_img_create(myAppList1);
    lv_img_set_src(img10, MY_LV_IMG_ADDR(APPLIST_STOPWATCH));
	lv_obj_align(img10, LV_ALIGN_BOTTOM_LEFT, 20, 90);

	lv_obj_t * img11 = lv_img_create(myAppList1);
    lv_img_set_src(img11, MY_LV_IMG_ADDR(APPLIST_THEATERMODE));
	lv_obj_align(img11, LV_ALIGN_BOTTOM_MID, 0, 90);

	lv_obj_t * img12 = lv_img_create(myAppList1);
    lv_img_set_src(img12, MY_LV_IMG_ADDR(APPLIST_VIBRATIONOFF));
	lv_obj_align(img12, LV_ALIGN_BOTTOM_RIGHT, -20, 90);

	//RAW 5
	lv_obj_t * img13 = lv_img_create(myAppList1);
    lv_img_set_src(img13, MY_LV_IMG_ADDR(APPLIST_VIBRATIONON));
	lv_obj_align(img13, LV_ALIGN_BOTTOM_LEFT, 20, 210);
}


#endif
