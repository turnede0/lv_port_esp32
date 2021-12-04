#ifndef TESTPAGE_UI_H
#define TESTPAGE_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

lv_obj_t *Screen2;
lv_obj_t *Screen2bg;
//lv_obj_t *Screen1;
lv_obj_t *Screen1bg;
lv_obj_t *Label3;
lv_obj_t *Label4;
lv_obj_t *Label5;
lv_obj_t * chart;
lv_obj_t * chart2;
lv_chart_series_t * ser1;
lv_chart_series_t * ser2;

extern lv_obj_t * Screen1;
extern lv_obj_t * Image3;
extern lv_obj_t * Image4;


LV_IMG_DECLARE(img_icon_png);   // assets\Icon.png


#ifdef __cplusplus
} /*extern "C"*/
#endif

void number_update_task();
void update_chart();

#endif
