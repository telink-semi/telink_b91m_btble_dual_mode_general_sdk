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
//LV_IMG_DECLARE(appList_music);
//LV_IMG_DECLARE(appList_QQ);

static lv_obj_t * img1;
static lv_obj_t * img2;
static lv_obj_t * img3;
static lv_obj_t * img4;
static lv_obj_t * img5;
static lv_obj_t * img6;
static lv_obj_t * img7;
static lv_obj_t * img8;

static lv_obj_t * imgMusic;
static lv_obj_t * imgQQ;


lv_obj_t *myAppList2;
lv_obj_t * cont;



static void win_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

	//LV_LOG_USER("win code = %d, ta = %x, music = %x, cont = %x", code, ta, imgMusic,cont);

	if( ta == imgMusic )
	{
		//lv_demo_music();
		//LV_LOG_USER("music is clicked");
		
	}
	else if( ta == imgQQ )
	{
		
	}

    if(code == LV_EVENT_GESTURE) {
		
		lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
		//LV_LOG_USER("gesture dir = %d", dir);


		if( dir == LV_DIR_LEFT )
		{
//			lv_obj_remove_event_cb(myAppList2, win_event_cb);
//			lv_obj_clean(myAppList2);

			//to next page
			//page_remind();
		}
		else if( dir == LV_DIR_RIGHT )
		{
			lv_obj_remove_event_cb(myAppList2, win_event_cb);
			lv_obj_clean(myAppList2);

			//to pre page
			lv_example_wb_appList();

		}
    }
    
}

static void app_event_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

	//LV_LOG_USER("code = %d, ta = %x, music = %x, cont = %x", code, ta, imgMusic,cont);

//	if( ta == imgMusic )
//	{
//		lv_obj_remove_event_cb(myAppList2, app_event_cb);
//		lv_obj_clean(myAppList2);
//
//		//to next
//		lv_demo_music();
//	}
//	else if( ta == imgQQ )
//	{
//		
//	}
}


static void my_scroll_fill_app(lv_obj_t *cont)
{
	imgMusic = lv_imgbtn_create(cont);
    lv_imgbtn_set_src(imgMusic, LV_IMGBTN_STATE_RELEASED,  MY_LV_IMG_ADDR(APPLIST_MUSIC), NULL,NULL);
	lv_obj_set_size(imgMusic, 66, 66);
	lv_obj_add_event_cb(imgMusic, app_event_cb, LV_EVENT_CLICKED, NULL);

	imgQQ = lv_imgbtn_create(cont);
    lv_imgbtn_set_src(imgQQ, LV_IMGBTN_STATE_RELEASED,  MY_LV_IMG_ADDR(APPLIST_QQ), NULL,NULL);
	lv_obj_set_size(imgQQ, 66, 66);
	lv_obj_add_event_cb(imgQQ, app_event_cb, LV_EVENT_CLICKED, NULL);

	//RAW 1
	img1 = lv_img_create(cont);
    lv_img_set_src(img1, MY_LV_IMG_ADDR(APPLIST_ALARM));

	img2 = lv_img_create(cont);	
    lv_img_set_src(img2, MY_LV_IMG_ADDR(APPLIST_APP_DOWNLOAD));
	
	img3 = lv_img_create(cont);
    lv_img_set_src(img3, MY_LV_IMG_ADDR(APPLIST_CALCULAGRAPH));
	
	img4 = lv_img_create(cont);
    lv_img_set_src(img4, MY_LV_IMG_ADDR(APPLIST_COMPASS));

	img5 = lv_img_create(cont);	
    lv_img_set_src(img5, MY_LV_IMG_ADDR(APPLIST_PHONEOFF));

	img6 = lv_img_create(cont);
    lv_img_set_src(img6, MY_LV_IMG_ADDR(APPLIST_PHONEON));

	img7 = lv_img_create(cont);
    lv_img_set_src(img7, MY_LV_IMG_ADDR(APPLIST_POWERON));


	img8 = lv_img_create(cont);
    lv_img_set_src(img8, MY_LV_IMG_ADDR(APPLIST_REBOOTTOFACTORY));
}


static void set_zoom(void* img, int32_t v)
{
//    lv_img_set_zoom(img, v);
	lv_obj_set_style_transform_zoom(img, v, 0);
}

static void my_appList_zoom(lv_obj_t * app, lv_coord_t zoomMix, lv_coord_t zoomMax, lv_coord_t boundrayY)
{
	lv_coord_t y = app->coords.y1;
	lv_coord_t height = lv_obj_get_height(app);
	
	
	//Calculation of univariate linear equation y(zoom)
	if( y < boundrayY )
	{
		set_zoom(app, zoomMax + zoomMix*(y - boundrayY)/(boundrayY + height));
	}
	else if( y < SDL_HOR_RES - height - boundrayY )
	{
		set_zoom(app, zoomMax);
	}
	else
	{
		set_zoom(app, zoomMix + zoomMix*(SDL_HOR_RES - y)/(boundrayY + height));
	}
}


static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        }
        else {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);


		//my_appList_zoom(child, 128, 256, 30);


        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}



void lv_example_appList2(void)
{
#if !MY_LVGL_DEMO_DRAG_EN
    myAppList2 = lv_scr_act();
	lv_obj_add_event_cb(myAppList2, win_event_cb, LV_EVENT_GESTURE, NULL);
#endif

#if 1
    cont = lv_obj_create(myAppList2);
    lv_obj_set_size(cont, SDL_HOR_RES, SDL_VER_RES);
    lv_obj_center(cont);
	lv_obj_set_style_bg_color(cont, lv_color_make(0x00, 0x00, 0x00), 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(cont, true, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

	#if 0
    uint32_t i;
    for(i = 0; i < 20; i++) {
		
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_set_width(btn, lv_pct(100));

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Button %"LV_PRIu32, i);

    }
	#else
	my_scroll_fill_app(cont);
	#endif

    /*Update the buttons position manually for first*/
    lv_event_send(cont, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    lv_obj_scroll_to_view(lv_obj_get_child(cont, 2), LV_ANIM_OFF);
#endif
}

#endif
