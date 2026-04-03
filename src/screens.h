#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_SCREEN_MENU = 1,
    SCREEN_ID_SCREEN_OPTION_1 = 2,
    _SCREEN_ID_LAST = 2
};

typedef struct _objects_t {
    lv_obj_t *screen_menu;
    lv_obj_t *screen_option_1;
    lv_obj_t *label_menu;
    lv_obj_t *btn_option_1;
    lv_obj_t *label_option_1;
    lv_obj_t *return_arrow;
    lv_obj_t *panel_temperatures;
    lv_obj_t *label_temperature;
    lv_obj_t *label_temp_date;
} objects_t;

extern objects_t objects;

void create_screen_screen_menu();
void tick_screen_screen_menu();

void create_screen_screen_option_1();
void tick_screen_screen_option_1();

// functions of group for screen.
void group_set_for_screen_menu();
void group_set_for_screen_option_1();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/