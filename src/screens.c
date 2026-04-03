#include <Arduino.h>
#include <string.h>
#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

//-----------Defition of Pins-------------------//
#define LED 5

objects_t objects;
lv_obj_t *tick_value_change_obj;
extern lv_group_t *encoder_group;
extern lv_indev_t *encoder_indev;
extern lv_style_t style_focus;
extern struct _objects_t objects;

static const char *screen_names[] = {"screen_menu", "screen_option_1"};
static const char *object_names[] = {"screen_menu", "screen_option_1", "label_menu", "btn_option_1", "label_option_1", "return_arrow", "panel_temperatures", "label_temperature", "label_temp_date"};

//--------------Event Handler--------------------//

lv_obj_t *tick_value_change_obj;

static void event_handler_cb_screen_menu_btn_option_1(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;

    if (event == LV_EVENT_PRESSED)
    {
        e->user_data = (void *)0;
        action_ir_screen_option_1(e);
        digitalWrite(LED, HIGH);
    }
}

static void event_handler_cb_screen_option_1_return_arrow(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;

    if (event == LV_EVENT_CLICKED)
    {
        e->user_data = (void *)0;
        action_ir_screen_menu(e);
        digitalWrite(LED, LOW);
    }
}

//-----------------Creation of Screens--------------------//
/**********************Screen Menu******************/
void create_screen_screen_menu()
{
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen_menu = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // label_menu
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_menu = obj;
            lv_obj_set_pos(obj, 138, 11);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "MENU");
        }
        {
            // btn_option_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_option_1 = obj;
            lv_obj_set_pos(obj, 110, 95);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_screen_menu_btn_option_1, LV_EVENT_ALL, flowState);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_option_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_option_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Option 1");
                }
            }
        }
    }

    tick_screen_screen_menu();
}

void tick_screen_screen_menu()
{
    void *flowState = getFlowState(0, 0);
    (void)flowState;
}

void group_set_for_screen_menu()
{
    lv_group_remove_all_objs(encoder_group);

    if (objects.btn_option_1)
    {
        lv_group_add_obj(encoder_group, objects.btn_option_1);
        lv_obj_add_style(objects.btn_option_1, &style_focus, LV_STATE_FOCUSED);
    }

    lv_indev_set_group(lv_indev_get_next(NULL), encoder_group);

    lv_group_focus_obj(objects.btn_option_1);
}
/********************Screen Option 1***********************/
void create_screen_screen_option_1()
{
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen_option_1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
            lv_obj_set_pos(obj, 210, 167);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, 64);
        }
        {
            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
            lv_obj_set_pos(obj, 186, 346);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, 228);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_return_arrow, NULL);
        }
        {
            // return_arrow
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.return_arrow = obj;
            lv_obj_set_pos(obj, 10, 10);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_return_arrow);
            lv_obj_add_event_cb(obj, event_handler_cb_screen_option_1_return_arrow, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        {
            // panel_temperatures
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_temperatures = obj;
            lv_obj_set_pos(obj, 62, 73);
            lv_obj_set_size(obj, 196, 94);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_temperature
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_temperature = obj;
                    lv_obj_set_pos(obj, 21, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Temperature (°C)");
                }
                {
                    // label_temp_date
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_temp_date = obj;
                    lv_obj_set_pos(obj, 65, 32);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "XXXX");
                }
            }
        }
    }

    tick_screen_screen_option_1();
}

void tick_screen_screen_option_1()
{
    void *flowState = getFlowState(0, 1);
    (void)flowState;
}
void group_set_for_screen_option_1()
{
    lv_group_remove_all_objs(encoder_group);

    if (objects.return_arrow)
    {
        lv_group_add_obj(encoder_group, objects.return_arrow);
        lv_obj_add_style(objects.return_arrow, &style_focus, LV_STATE_FOCUSED);
    }

    lv_indev_set_group(lv_indev_get_next(NULL), encoder_group);

    lv_group_focus_obj(objects.panel_temperatures);
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_screen_menu,
    tick_screen_screen_option_1,
};
void tick_screen(int screen_index)
{
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId)
{
    tick_screen_funcs[screenId - 1]();
}

//-------------------Fonts-------------------//

ext_font_desc_t fonts[] = {
#if LV_FONT_MONTSERRAT_8
    {"MONTSERRAT_8", &lv_font_montserrat_8},
#endif
#if LV_FONT_MONTSERRAT_10
    {"MONTSERRAT_10", &lv_font_montserrat_10},
#endif
#if LV_FONT_MONTSERRAT_12
    {"MONTSERRAT_12", &lv_font_montserrat_12},
#endif
#if LV_FONT_MONTSERRAT_14
    {"MONTSERRAT_14", &lv_font_montserrat_14},
#endif
#if LV_FONT_MONTSERRAT_16
    {"MONTSERRAT_16", &lv_font_montserrat_16},
#endif
#if LV_FONT_MONTSERRAT_18
    {"MONTSERRAT_18", &lv_font_montserrat_18},
#endif
#if LV_FONT_MONTSERRAT_20
    {"MONTSERRAT_20", &lv_font_montserrat_20},
#endif
#if LV_FONT_MONTSERRAT_22
    {"MONTSERRAT_22", &lv_font_montserrat_22},
#endif
#if LV_FONT_MONTSERRAT_24
    {"MONTSERRAT_24", &lv_font_montserrat_24},
#endif
#if LV_FONT_MONTSERRAT_26
    {"MONTSERRAT_26", &lv_font_montserrat_26},
#endif
#if LV_FONT_MONTSERRAT_28
    {"MONTSERRAT_28", &lv_font_montserrat_28},
#endif
#if LV_FONT_MONTSERRAT_30
    {"MONTSERRAT_30", &lv_font_montserrat_30},
#endif
#if LV_FONT_MONTSERRAT_32
    {"MONTSERRAT_32", &lv_font_montserrat_32},
#endif
#if LV_FONT_MONTSERRAT_34
    {"MONTSERRAT_34", &lv_font_montserrat_34},
#endif
#if LV_FONT_MONTSERRAT_36
    {"MONTSERRAT_36", &lv_font_montserrat_36},
#endif
#if LV_FONT_MONTSERRAT_38
    {"MONTSERRAT_38", &lv_font_montserrat_38},
#endif
#if LV_FONT_MONTSERRAT_40
    {"MONTSERRAT_40", &lv_font_montserrat_40},
#endif
#if LV_FONT_MONTSERRAT_42
    {"MONTSERRAT_42", &lv_font_montserrat_42},
#endif
#if LV_FONT_MONTSERRAT_44
    {"MONTSERRAT_44", &lv_font_montserrat_44},
#endif
#if LV_FONT_MONTSERRAT_46
    {"MONTSERRAT_46", &lv_font_montserrat_46},
#endif
#if LV_FONT_MONTSERRAT_48
    {"MONTSERRAT_48", &lv_font_montserrat_48},
#endif
};

//--------------------------------------------//

void create_screens()
{

    eez_flow_init_fonts(fonts, sizeof(fonts) / sizeof(ext_font_desc_t));

    // Set default LVGL theme
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    // Initialize screens
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));

    // Create screens
    create_screen_screen_menu();
    create_screen_screen_option_1();
}