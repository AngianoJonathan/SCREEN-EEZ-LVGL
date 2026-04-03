#ifndef EEZ_LVGL_UI_GUI_H
#define EEZ_LVGL_UI_GUI_H

#include <lvgl.h>

#include "eez-flow.h"
#include "screens.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t assets[956];

void ui_init();
void ui_tick();
void loadScreen(enum ScreensEnum screenId);

#ifdef __cplusplus
}
#endif

#endif // EEZ_LVGL_UI_GUI_H