#include <lvgl.h>
#include <arduino.h>
#include "actions.h"
#include "screens.h"
#include "ui.h"
#include "vars.h"

// Objetos LVGL generados (EEZ)
extern struct _objects_t objects;
// Objetos externos globales de la aplicación
extern lv_group_t *encoder_group;
extern lv_indev_t *encoder_indev;
extern lv_style_t style_focus;
extern lv_group_t *group;

// -------------------- Función auxiliar para agregar objetos al grupo con estilo --------------------
static void add_to_group_with_style(lv_obj_t *obj) {
  if (obj && encoder_group) {
    lv_group_add_obj(encoder_group, obj);
    lv_obj_add_style(obj, &style_focus, LV_STATE_FOCUSED);
  }
}

//----------actions to go to screens differents------------------//
void action_ir_screen_menu(lv_event_t * e){
  loadScreen(SCREEN_ID_SCREEN_MENU);
  group_set_for_screen_menu();


  lv_group_focus_obj(objects.btn_option_1);

  //lv_obj_clear_state(objects.btn_back_set_zero, LV_STATE_DISABLED);
  //lv_obj_clear_state(objects.btn_back_vary_speed, LV_STATE_DISABLED);
}
void action_ir_screen_option_1(lv_event_t * e){
  loadScreen(SCREEN_ID_SCREEN_OPTION_1);
  group_set_for_screen_option_1();
  lv_group_focus_obj(objects.return_arrow);

  //lv_obj_clear_state(objects.btn_back_vary_speed, LV_STATE_DISABLED);
  //lv_obj_clear_state(objects.btn_new_zero, LV_STATE_DISABLED);
  //lv_obj_clear_state(objects.btn_vary_speed, LV_STATE_DISABLED);
}


// -------------------- Carga de pantallas --------------------
#ifdef __cplusplus
extern "C" {
#endif

  // Función que carga una pantalla por ID
  void loadScreen(enum ScreensEnum screenId) {
    static int currentScreen = -1;
    currentScreen = screenId - 1;
    lv_obj_t *screen = ((lv_obj_t **)&objects)[currentScreen];
    lv_scr_load(screen);  // Cambia la pantalla actual
  }

#ifdef __cplusplus
}
#endif