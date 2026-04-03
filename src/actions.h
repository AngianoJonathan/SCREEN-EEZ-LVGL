#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>
#include "ui.h"

#ifdef __cplusplus
extern "C" {
#endif
// -------------------- Funciones de Enlace de UI --------------------
// Registra los botones actuales en el grupo de enfoque (para navegación con encoder)
void add_buttons_to_group();

extern void action_ir_screen_menu(lv_event_t * e);
extern void action_ir_screen_option_1(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/