#include "../lv_examples.h"

#if( TLK_CFG_GUI_ENABLE )

//LV_IMG_DECLARE(myDialBG);
//LV_IMG_DECLARE(myDialHour);
//LV_IMG_DECLARE(myDialMinute);
//LV_IMG_DECLARE(myDialSec);

//LV_IMG_DECLARE(myRed_16);
//LV_IMG_DECLARE(myRed_trueColor_16);


lv_obj_t* central;

static lv_timer_t *lvDialTimer;

lv_obj_t * lvMinute;
lv_obj_t * lvHour;
lv_obj_t * lvSecond ;

uint8_t Minute =0;
uint8_t Hour =8;
uint8_t Second=0 ;


static void update_time(lv_timer_t * timer)
{
    //time_t now_t;
    char strftime_buf[64];

    if (++Second >= 60)
    {
        Second = 0;
        if (++Minute >= 60)
        {
            Minute = 0;
            if (++Hour >= 12) Hour = 0;
        }

    }


    if(lvHour !=NULL)
    {
       // Hour,Minute,Second;

        uint16_t h = Hour * 300 + Minute / 12 % 12 * 60;

        lv_img_set_angle(lvHour, h);
        lv_img_set_angle(lvMinute, Minute * 6 * 10);

        lv_img_set_angle(lvSecond, Second * 6 * 10);
        //printf("h %d m %d s %d\n", Hour, Minute, Second);
    }

}

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
			lv_timer_del(lvDialTimer);
			lv_obj_remove_event_cb(central, win_event_cb);
			lv_obj_clean(central);

			//to next page
			LV_LOG_USER("to next page");
			//lv_example_keyboard_1();
			lv_example_wb_appList();
		}
    }
    
}


static void set_angle(void* img, int32_t v)
{
    lv_img_set_angle(img, v);
}

static void set_zoom(void* img, int32_t v)
{
    lv_img_set_zoom(img, v);
}



void page_onlyBG(void)
{
	lv_obj_t * onlyBG = lv_scr_act();
    lv_obj_t * img = lv_img_create(onlyBG);
    lv_img_set_src(img, MY_LV_IMG_ADDR(GREEN_360X360_TEST));
	lv_obj_align(img, LV_ALIGN_CENTER, 0,0);
    //lv_obj_set_size(img, MY_DISP_HOR_RES, MY_DISP_VER_RES);
}


void page_dialPlate()
{
#if !MY_LVGL_DEMO_DRAG_EN
    central = lv_scr_act();
    
    //add signal
    lv_obj_add_event_cb(central, win_event_cb, LV_EVENT_ALL, NULL);
#endif

    lv_obj_set_size(central, MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_obj_set_pos(central,  0, 0);
 

    lv_obj_t * img = lv_img_create(central);
    lv_img_set_src(img, MY_LV_IMG_ADDR(CLOCK_BACKGROUND));


    lv_obj_set_size(img, 360, 360);
    //lv_obj_set_auto_realign(img, true);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

	/*Now create the actual image*/
	LV_IMG_DECLARE(WeChat);
	lv_obj_t* imgAngle = lv_img_create(central); 
	lv_img_set_src(imgAngle, MY_LV_IMG_ADDR(CLOCK_FOWER)); 
	lv_obj_align(imgAngle, LV_ALIGN_CENTER, 0, 0); 
	lv_img_set_pivot(imgAngle, 64, 64); 
	lv_anim_t a;
	lv_anim_init(&a);  
	lv_anim_set_var(&a, imgAngle); 
	lv_anim_set_exec_cb(&a, set_angle); 
	lv_anim_set_values(&a, 0, 3600); // Set the range of animation, here is the angle 0~360 degrees
	lv_anim_set_time(&a, 10000); 
	lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
	lv_anim_start(&a); 

	//zoom
    lv_anim_set_exec_cb(&a, set_zoom);
	lv_anim_set_values(&a, 128, 400); // Set the range of the animation, here is the scale 128~256
	lv_anim_set_playback_time(&a, 3000); 
	lv_anim_start(&a);  


    lvHour = lv_img_create(central);
    lv_img_set_src( lvHour, MY_LV_IMG_ADDR(CLOCK_HOUR));
    lv_obj_align(  lvHour,LV_ALIGN_CENTER, 0, -44);
    lv_img_set_pivot(lvHour, 8, 92);
    uint16_t h = Hour * 300 + Minute / 12 % 12 * 60;
    lv_img_set_angle(  lvHour, h);

    lvMinute = lv_img_create(central);
    lv_img_set_src( lvMinute, MY_LV_IMG_ADDR(CLOCK_MINUTE));
    lv_obj_align(  lvMinute, LV_ALIGN_CENTER, 0, -71);
    lv_img_set_pivot(lvMinute, 8, 145);
    lv_img_set_angle(  lvMinute, Minute*60);

    lvSecond = lv_img_create(central);
    lv_img_set_src( lvSecond, MY_LV_IMG_ADDR(CLOCK_SECOND));
    lv_obj_align(  lvSecond,LV_ALIGN_CENTER, 0, -71);
    lv_img_set_pivot(lvSecond, 7, 145);
    lv_img_set_angle(  lvSecond, Second*60);

	
    lvDialTimer = lv_timer_create(update_time, 1000 ,NULL);
}


void page_tabview(void)
{
	lv_obj_t *tabview;
	tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 0);

	lv_obj_set_style_bg_color(tabview, lv_color_make(0x00, 0x00, 0x00), 0);
	lv_obj_set_scrollbar_mode(tabview, LV_SCROLLBAR_MODE_OFF);

	//add page
	extern lv_obj_t *myAppList1;
	extern lv_obj_t *myAppList2;
	central = lv_tabview_add_tab(tabview, "dial");
	myAppList1 = lv_tabview_add_tab(tabview, "appList1");
	myAppList2 = lv_tabview_add_tab(tabview, "appList2");


	page_dialPlate();
	lv_example_appList1();
	lv_example_appList2();

	//lv_obj_scroll_to_view_recursive(central, LV_ANIM_ON);
}

#endif
