#include "testpage_ui.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "blink.h"
#include "adc_event.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t *Screen2;
lv_obj_t *Screen2bg;
lv_obj_t *Screen1;
lv_obj_t *Screen1bg;
lv_obj_t *Label3;
lv_obj_t *Label4;
lv_obj_t *Label5;
lv_obj_t * chart;
lv_obj_t * chart2;
lv_chart_series_t * ser1;
lv_chart_series_t * ser2;

lv_obj_t * Image3;
lv_obj_t * Image4;


LV_IMG_DECLARE(img_icon_png);   // assets\Icon.png

#define Btn_GPIO GPIO_NUM_26

///////////////////// IMAGES ////////////////////

///////////////////// FUNCTIONS ////////////////////
#define BAR_PROPERTY_VALUE 0
#define BAR_PROPERTY_VALUE_WITH_ANIM 1

void SetBarProperty(lv_obj_t *target, int id, int val)
{
    if (id == BAR_PROPERTY_VALUE_WITH_ANIM)
        lv_bar_set_value(target, val, LV_ANIM_ON);
    if (id == BAR_PROPERTY_VALUE)
        lv_bar_set_value(target, val, LV_ANIM_OFF);
}

#define BASIC_PROPERTY_POSITION_X 0
#define BASIC_PROPERTY_POSITION_Y 1
#define BASIC_PROPERTY_WIDTH 2
#define BASIC_PROPERTY_HEIGHT 3
#define BASIC_PROPERTY_CLICKABLE 4
#define BASIC_PROPERTY_HIDDEN 5
#define BASIC_PROPERTY_DRAGABLE 6
#define BASIC_PROPERTY_DISABLED 7

void SetBasicProperty(lv_obj_t *target, int id, int val)
{
    if (id == BASIC_PROPERTY_POSITION_X)
        lv_obj_set_x(target, val);
    if (id == BASIC_PROPERTY_POSITION_Y)
        lv_obj_set_y(target, val);
    if (id == BASIC_PROPERTY_WIDTH)
        lv_obj_set_width(target, val);
    if (id == BASIC_PROPERTY_HEIGHT)
        lv_obj_set_height(target, val);
}

void SetBasicPropertyB(lv_obj_t *target, int id, bool val)
{
    if (id == BASIC_PROPERTY_CLICKABLE)
        lv_obj_set_click(target, val);
    if (id == BASIC_PROPERTY_HIDDEN)
        lv_obj_set_hidden(target, val);
    if (id == BASIC_PROPERTY_DRAGABLE)
        lv_obj_set_drag(target, val);
    if (id == BASIC_PROPERTY_DISABLED)
    {
        if (val)
            lv_obj_add_state(target, LV_STATE_DISABLED);
        else
            lv_obj_clear_state(target, LV_STATE_DISABLED);
    }
}

#define BUTTON_PROPERTY_TOGGLE 0
#define BUTTON_PROPERTY_CHECKED 1

void SetButtonProperty(lv_obj_t *target, int id, bool val)
{
    if (id == BUTTON_PROPERTY_TOGGLE)
        lv_btn_toggle(target);
    if (id == BUTTON_PROPERTY_CHECKED)
        lv_btn_set_state(target, val ? LV_BTN_STATE_CHECKED_RELEASED : LV_BTN_STATE_RELEASED);
}

#define DROPDOWN_PROPERTY_SELECTED 0

void SetDropdownProperty(lv_obj_t *target, int id, int val)
{
    if (id == DROPDOWN_PROPERTY_SELECTED)
        lv_dropdown_set_selected(target, val);
}

#define IMAGE_PROPERTY_IMAGE 0

void SetImageProperty(lv_obj_t *target, int id, uint8_t *val)
{
    if (id == IMAGE_PROPERTY_IMAGE)
        lv_img_set_src(target, val);
}

#define LABEL_PROPERTY_TEXT 0

void SetLabelProperty(lv_obj_t *target, int id, char *val)
{
    if (id == LABEL_PROPERTY_TEXT)
        lv_label_set_text(target, val);
}

#define ROLLER_PROPERTY_SELECTED 0
#define ROLLER_PROPERTY_SELECTED_WITH_ANIM 1

void SetRollerProperty(lv_obj_t *target, int id, int val)
{
    if (id == ROLLER_PROPERTY_SELECTED_WITH_ANIM)
        lv_roller_set_selected(target, val, LV_ANIM_ON);
    if (id == ROLLER_PROPERTY_SELECTED)
        lv_roller_set_selected(target, val, LV_ANIM_OFF);
}

#define SLIDER_PROPERTY_VALUE 0
#define SLIDER_PROPERTY_VALUE_WITH_ANIM 1

void SetSliderProperty(lv_obj_t *target, int id, int val)
{
    if (id == SLIDER_PROPERTY_VALUE_WITH_ANIM)
        lv_slider_set_value(target, val, LV_ANIM_ON);
    if (id == SLIDER_PROPERTY_VALUE)
        lv_slider_set_value(target, val, LV_ANIM_OFF);
}

void ChangeScreen(lv_obj_t *target, int fademode, int spd, int delay)
{
    lv_scr_load_anim(target, fademode, spd, delay, false);
}

void SetOpacity(lv_obj_t *target, int val)
{
    lv_obj_set_style_local_opa_scale(target, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, val);
}

void anim_callback_set_x(lv_anim_t *a, lv_anim_value_t v)
{
    lv_obj_set_x(a->user_data, v);
}

void anim_callback_set_y(lv_anim_t *a, lv_anim_value_t v)
{
    lv_obj_set_y(a->user_data, v);
}

void anim_callback_set_width(lv_anim_t *a, lv_anim_value_t v)
{
    lv_obj_set_width(a->user_data, v);
}

void anim_callback_set_height(lv_anim_t *a, lv_anim_value_t v)
{
    lv_obj_set_height(a->user_data, v);
}

///////////////////// ANIMATIONS ////////////////////
void Animation1_Animation(lv_obj_t *TargetObject, int delay)
{
}

///////////////////// FUNCTIONS2 ////////////////////


///////////////////// SCREENS ////////////////////
void BuildPages(void)
{
    gpio_set_direction(Btn_GPIO,GPIO_MODE_INPUT);
    
    //======= Screen1 Setup ============
    
    Screen1 = lv_obj_create(NULL, NULL);

    Screen1bg = lv_obj_create(Screen1, NULL);
    lv_obj_set_click(Screen1bg, true);
    lv_obj_set_hidden(Screen1bg, false);
    lv_obj_clear_state(Screen1bg, LV_STATE_DISABLED);
    lv_obj_set_size(Screen1bg, 285, 242);                    // force: 0
    lv_obj_align(Screen1bg, Screen1, LV_ALIGN_CENTER, 0, 0); // force: 280
    lv_obj_set_drag(Screen1bg, true);
    lv_obj_set_style_local_bg_color(Screen1bg, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_opa(Screen1bg, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 100);
    
    Label4 = lv_label_create(Screen1, NULL);
    lv_label_set_align(Label4, LV_LABEL_ALIGN_CENTER);
//    lv_obj_set_size(Label4, 72, 50); // force: 0
    lv_label_set_long_mode(Label4, LV_LABEL_LONG_DOT);     /*Circular scroll*/
    lv_obj_set_width(Label4, 150);
    lv_label_set_text(Label4, "Counter");
    lv_obj_align(Label4, NULL, LV_ALIGN_CENTER, 0, 10); // force: 72
    

    // Image3 = lv_img_create(Screen1, NULL);
    // lv_img_set_src(Image3, &img_icon_png);
    // lv_obj_set_click(Image3, false);
    // lv_obj_set_hidden(Image3, false);
    // lv_obj_set_size(Image3, 64, 72);
    // lv_obj_align(Image3, Screen1, LV_ALIGN_CENTER, -50, 0);
    // lv_obj_set_drag(Image3, false);

    // lv_obj_clear_state(Image3, LV_STATE_DISABLED);

    //  Image4 = lv_img_create(Screen1, NULL);
    // lv_img_set_src(Image4, &img_icon_png);
    // lv_obj_set_click(Image4, false);
    // lv_obj_set_hidden(Image4, false);
    // lv_obj_set_size(Image4, 64, 72);
    // lv_obj_align(Image4, Screen1, LV_ALIGN_CENTER, 50, 0);
    // lv_obj_set_drag(Image4, false);
    // lv_obj_set_style_local_bg_color(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT,
    //                                 lv_color_hex(128 * 256 * 256 + 254 * 256 + 190));
    // lv_obj_set_style_local_bg_opa(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 255);
    // lv_obj_set_style_local_bg_grad_color(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT,
    //                                      lv_color_hex(254 * 256 * 256 + 128 * 256 + 128));
    // lv_obj_set_style_local_bg_main_stop(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 0);
    // lv_obj_set_style_local_bg_grad_stop(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 255);
    // lv_obj_set_style_local_radius(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 0);
    // lv_obj_set_style_local_bg_grad_dir(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    // lv_obj_set_style_local_shadow_color(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT,
    //                                     lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
    // lv_obj_set_style_local_shadow_opa(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 128);
    // lv_obj_set_style_local_shadow_width(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 5);
    // lv_obj_set_style_local_shadow_spread(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 1);
    // lv_obj_set_style_local_shadow_ofs_x(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 5);
    // lv_obj_set_style_local_shadow_ofs_y(Image4, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, 5);

    // lv_obj_clear_state(Image4, LV_STATE_DISABLED);


    //======= End of Screen1 Setup ============

    //======= Screen2 Setup ============ 

    Screen2 = lv_obj_create(NULL, NULL);

    Screen2bg = lv_obj_create(Screen2, NULL);
    lv_obj_set_click(Screen2bg, true);
    lv_obj_set_hidden(Screen2bg, false);
    lv_obj_clear_state(Screen2bg, LV_STATE_DISABLED);
    lv_obj_set_size(Screen2bg, 285, 242);                    // force: 0
    lv_obj_align(Screen2bg, Screen2, LV_ALIGN_CENTER, 0, 0); // force: 280
    lv_obj_set_drag(Screen2bg, true);
    lv_obj_set_style_local_bg_color(Screen2bg, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_GRAY);
    lv_obj_set_style_local_bg_opa(Screen2bg, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 100);

    Label3 = lv_label_create(Screen2, NULL);
    lv_label_set_long_mode(Label3, LV_LABEL_LONG_EXPAND);
    lv_label_set_align(Label3, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(Label3, "Current Value");
    lv_obj_set_size(Label3, 72, 16); // force: 0
    lv_obj_set_click(Label3, false);
    lv_obj_set_hidden(Label3, false);
    lv_obj_clear_state(Label3, LV_STATE_DISABLED);
    lv_obj_set_drag(Label3, false);

    lv_obj_align(Label3, Screen2, LV_ALIGN_IN_TOP_MID, 0, 10); // force: 72


    Label5 = lv_label_create(Screen2, NULL);
    lv_label_set_long_mode(Label5, LV_LABEL_LONG_EXPAND);
    lv_label_set_align(Label5, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(Label5, "10 second Average");
    lv_obj_set_size(Label5, 65, 16); // force: 0
    lv_obj_set_click(Label5, false);
    lv_obj_set_hidden(Label5, false);
    lv_obj_clear_state(Label5, LV_STATE_DISABLED);
    lv_obj_set_drag(Label5, false);

    lv_obj_align(Label5, Screen2, LV_ALIGN_CENTER, 0, 10); // force: 65

   

  chart = lv_chart_create(Screen2, NULL);
    lv_obj_set_size(chart, 200, 80);
    lv_obj_align(chart, Screen2, LV_ALIGN_IN_TOP_MID, 0, 30);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_obj_set_style_local_size(chart,LV_CHART_PART_SERIES,LV_STATE_DEFAULT,0);
    lv_obj_set_style_local_bg_opa(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);      /*Transparent on the bottom*/

    lv_chart_set_point_count(chart,30);

    /*Add two data series*/
    ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
    
    
    /*Set the next points on 'ser1'*/
    lv_chart_set_next(chart, ser1, 0);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 30);
    lv_chart_set_next(chart, ser1, 70);
    lv_chart_set_next(chart, ser1, 90);
    
    
    chart2 = lv_chart_create(Screen2, NULL);
    lv_obj_set_size(chart2, 200, 80);
    lv_obj_align(chart2, Screen2, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
    //lv_chart_set_type(chart2, LV_CHART_TYPE_COLUMN);   /*Show lines and points too*/
    lv_chart_set_type(chart2, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_chart_set_y_range(chart2,LV_CHART_AXIS_PRIMARY_Y,0,4095);

    lv_obj_set_style_local_size(chart2,LV_CHART_PART_SERIES,LV_STATE_DEFAULT,0);
    lv_obj_set_style_local_bg_opa(chart2, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir(chart2, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(chart2, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop(chart2, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);      /*Transparent on the bottom*/

    lv_chart_set_point_count(chart2,50);

    // /*Add two data series*/
     ser2 = lv_chart_add_series(chart2, LV_COLOR_AQUA);
    // // /*Directly set points on 'ser2'*/
    ser2->points[0] = 90;
    ser2->points[1] = 70;
    ser2->points[2] = 65;
    ser2->points[3] = 65;
    ser2->points[4] = 65;
    ser2->points[5] = 65;
    ser2->points[6] = 65;
    ser2->points[7] = 65;
    ser2->points[8] = 65;
    ser2->points[9] = 65;
   // ser2->points[10] = 90;
    lv_chart_set_update_mode(chart2,LV_CHART_UPDATE_MODE_CIRCULAR);

    //======= End of Screen2 Setup ============ 
    



}
int count=0;



void update_chart()
{

  
    /*Set the next points on 'ser1'*/
    lv_chart_set_next(chart, ser1, (count++)%100);
    //ser2->points[5] = count % 79;
    if(count%7)
    //lv_chart_set_next(chart2, ser2, (count++)%30*2);
    lv_chart_set_next(chart2, ser2, adc_val_1);

    lv_chart_refresh(chart); /*Required after direct set*/    
    if (count>100)count=0;

    lv_label_set_text_fmt(Label3, "Current Value: %d", count);
    lv_label_set_text_fmt(Label4, "Peko Value: %d", count);
    lv_label_set_text_fmt(Label5, "Button Value: %d", adc_val_1);

    if(gpio_get_level(Btn_GPIO)){
        lv_scr_load(Screen1);
    }else{
        lv_scr_load(Screen2);
    }

    // lv_bar_set_value(bar1, 70, LV_ANIM_OFF);
    //  printf("job done boss");
    //   printf("%d", gpio_get_level(Btn_GPIO));
    //  printf("\n");

}


