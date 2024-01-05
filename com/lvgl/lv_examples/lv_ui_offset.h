/*
 * lv_ui_offset.h
 *
 *  Created on: 2022年12月7日
 *      Author: 86135
 */

#ifndef VENDOR_DISPLAY_DEMO_LV_EXAMPLES_LV_UI_OFFSET_H_
#define VENDOR_DISPLAY_DEMO_LV_EXAMPLES_LV_UI_OFFSET_H_

/*Please contact Telink for Telink tool to convert image to *.bin file*/

#define  MY_UI_START				0x200000 //2M
#define  MY_LV_IMG_ADDR(addr)		((void *)(0x20000000 + MY_UI_START + addr))


/*Format of the following pictures: TRUE_COLOR_ALPHA*/
#define  CLOCK_BACKGROUND                              0x0
#define  CLOCK_FOWER                                   0x5EECC
#define  CLOCK_HOUR                                    0x6AED8
#define  CLOCK_MINUTE                                  0x6C0B4
#define  CLOCK_SECOND                                  0x6DC50

/*Format of the following pictures: TRUE_COLOR*/
#define  APPLIST_ALARM                                 0x6F4CE
#define  APPLIST_APP_DOWNLOAD                          0x716E2
#define  APPLIST_CALCULAGRAPH                          0x738F6
#define  APPLIST_COMPASS                               0x75B0A
#define  APPLIST_MUSIC                                 0x77D1E
#define  APPLIST_PHONEOFF                              0x79D2A
#define  APPLIST_PHONEON                               0x7BF3E
#define  APPLIST_POWERON                               0x7E152
#define  APPLIST_QQ                                    0x80366
#define  APPLIST_REBOOTTOFACTORY                       0x82372
#define  APPLIST_SHUTDOWN                              0x84586
#define  APPLIST_STOPWATCH                             0x8679A
#define  APPLIST_THEATERMODE                           0x889AE
#define  APPLIST_VIBRATIONOFF                          0x8ABC2
#define  APPLIST_VIBRATIONON                           0x8CDD6
#define  GREEN_360X360_TEST                            0x8EFEA
#define  RED_360X360_TEST                              0xCE476


#endif /* VENDOR_DISPLAY_DEMO_LV_EXAMPLES_LV_UI_OFFSET_H_ */
