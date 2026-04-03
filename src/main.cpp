#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Adafruit_HTU21DF.h>
#include "ui.h"
#include "screens.h"
#include "images.h"
#include "styles.h"
#include "actions.h"
#include "vars.h"
#include "eez-flow.h"
#include "fonts.h"
#include "images.h"
#include "structs.h"
#include <ezButton.h>

//-------------------Pines----------------//
//--------------Encoder--------------------//
#define CLK_PIN 25
#define DT_PIN 26
#define SW_PIN 27
//------------Led--------------------------//
#define LED 5


ezButton button(SW_PIN); // Botón del encoder

// Variables para el control del encoder
bool encoder_pressed = false; // Estado del botón del encoder
volatile int encoderCounter = 0;
volatile int prevCLKState = 0;
volatile int currentCLKState = 0;

//-------Instanciamiento de Variables---------------//
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

// ==== NUEVO: decodificador por interrupciones ====
// ===== Encoder (decoder por interrupción SOLO para edición del ARC) =====
volatile int32_t enc_isr_count = 0;
volatile uint8_t enc_prev_ab = 0;
// LUT de 16 entradas para transiciones (prev<<2 | curr)
// +1/-1/0 según movimiento válido/ruido
static const int8_t ENC_LUT[16] = {
    0, -1, +1, 0,
    +1, 0, 0, -1,
    -1, 0, 0, +1,
    0, +1, -1, 0};

// helper para leer rápido A/B
inline uint8_t enc_read_ab()
{
  return ((uint8_t)digitalRead(CLK_PIN) << 1) | (uint8_t)digitalRead(DT_PIN);
}

// ISR compartida en ambos flancos de A y B
void IRAM_ATTR encoder_isr()
{
  uint8_t curr = enc_read_ab();
  uint8_t idx = ((enc_prev_ab & 0x03) << 2) | (curr & 0x03);
  enc_prev_ab = curr;
  enc_isr_count += ENC_LUT[idx];
}

///////////////////////////////////////////////////////////PANTALLA
TFT_eSPI tft = TFT_eSPI(); // Instancia de pantalla TFT
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[320 * 10]; // Buffer de dibujo para LVGL

// Objetos de entrada y grupo de enfoque
lv_group_t *group;
lv_indev_t *encoder_indev;
lv_style_t style_focus; // Estilo visual para el enfoque
lv_group_t *encoder_group = NULL;

// -------------------- Display Flush --------------------
// Función obligatoria de LVGL para actualizar la pantalla
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)color_p, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp); // Notifica a LVGL que terminó el refresco
}

// -------------------- Encoder Read --------------------
// Función que LVGL usa para leer el estado del encoder
void my_encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
  (void)drv;
  int16_t diff = 0;

  // === MODO NAVEGACIÓN: usar tu contador por polling (como siempre) ===
  static int last_count = 0;
  extern volatile int encoderCounter; // el que actualizás en loop() con CLK/DT
  int delta = encoderCounter - last_count;
  last_count = encoderCounter;
  diff = (int16_t)delta;
  // Serial.println("holaaaaaaaaa");

  data->enc_diff = diff;

  // Botón del encoder: tu one-shot de siempre
  extern bool encoder_pressed;
  data->state = encoder_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
  if (encoder_pressed)
  {
    encoder_pressed = false; // consumir el “press”
  }
  // Serial.print("LVGL diff = ");
  // Serial.println(diff);
}

// -------------------- Botones al Grupo --------------------
// Agrega botones a un grupo para navegación con el encoder
void add_buttons_to_group()
{
  if (!group)
    return;

  lv_group_remove_all_objs(group); // Limpia el grupo actual

  // Agrega botones relevantes y aplica estilo de enfoque
  auto add_btn = [](lv_obj_t *btn)
  {
    lv_group_add_obj(group, btn);
    lv_obj_add_style(btn, &style_focus, LV_STATE_FOCUSED);
  };

  //---------Buttons of Menu------------------//
  add_btn(objects.btn_option_1);

  //------Button of Option 1-----------------//
  add_btn(objects.return_arrow);
}
//----------------Variables Globales-----------//
float temp = 0, hum = 0;

// -------------------- SETUP --------------------
void setup()
{
  // --- Debug por Serial ---
  Serial.begin(9600);
  delay(200);
  Serial.println("[BOOT] inicio setup");
  //----------Inicializacion del Sensor HTU21D-----------------//
  htu.begin();
  //------------Comprobacion de Falla de sensor-----------------//
  if (!htu.begin())
  {
    Serial.println("Falla Sensor");
  }
  
  //-----------Defition of Pins------------------------//
  pinMode(LED, OUTPUT);
  // --- Encoder (pines + antirrebote del botón con ezButton) ---
  Serial.println("[BOOT] encoder pins");
  // Configura pines del encoder
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  prevCLKState = digitalRead(CLK_PIN);

  button.setDebounceTime(50);

  // --- LVGL & TFT ---
  Serial.println("[BOOT] lv_init");
  lv_init();

  Serial.println("[BOOT] tft.begin");
  tft.begin();
  tft.setRotation(1);

  // --- Buffer de dibujo LVGL ---
  Serial.println("[BOOT] lv_disp_draw_buf_init");
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, 320 * 10);

  // --- Driver de display ---
  Serial.println("[BOOT] lv_disp_drv_init");
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 320;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // --- Driver de input (encoder) ---
  Serial.println("[BOOT] lv_indev_drv_init");
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_ENCODER;
  indev_drv.read_cb = my_encoder_read;
  encoder_indev = lv_indev_drv_register(&indev_drv);

  // --- Grupo de enfoque del encoder ---
  Serial.println("[BOOT] group create");
  encoder_group = lv_group_create();
  lv_indev_set_group(encoder_indev, encoder_group);
  group = encoder_group; // si add_buttons_to_group usa 'group', la apuntamos acá

  // --- Inicializar estilo de foco ANTES de usarlo ---
  Serial.println("[BOOT] style init");
  lv_style_init(&style_focus);
  lv_style_set_outline_width(&style_focus, 3);
  lv_style_set_outline_color(&style_focus, lv_color_hex(0x00FF00));
  lv_style_set_outline_pad(&style_focus, 2);
  lv_style_set_outline_opa(&style_focus, LV_OPA_COVER);

  // --- Crear UI generada por EEZ ---
  Serial.println("[BOOT] ui_init");
  ui_init();

  lv_obj_add_flag(objects.btn_option_1, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_flag(objects.return_arrow, LV_OBJ_FLAG_CLICKABLE);

  // --- Agregar objetos al grupo (aplica style_focus internamente) ---
  Serial.println("[BOOT] add_buttons_to_group");
  add_buttons_to_group();

  Serial.println("[BOOT] SETUP OK");
}

// -------------------- LOOP --------------------
void loop()
{
  // 1. Calcular el tiempo transcurrido para LVGL
  static uint32_t last_tick = 0;
  static uint32_t last_temp_read = 0;
  uint32_t current_time = ::millis();

  // Le avisamos a LVGL cuánto tiempo pasó desde la última vuelta
  lv_tick_inc(current_time - last_tick);
  last_tick = current_time;

  // 2. Manejador de tareas de LVGL
  lv_timer_handler();
  delay(5); // Pequeña espera para no saturar la CPU
  // Lógica para detectar giros del encoder
  currentCLKState = digitalRead(CLK_PIN);
  if (currentCLKState != prevCLKState && currentCLKState == LOW)
  {
    if (digitalRead(DT_PIN) == HIGH)
    {
      encoderCounter++;
      Serial.println(encoderCounter);
    }
    else
    {
      encoderCounter--;
      Serial.println(encoderCounter);
    }
  }
  prevCLKState = currentCLKState;

  // 3) BOTÓN DEL ENCODER (pulso corto para que LVGL detecte el click)
  static bool wasPressed = false;
  static unsigned long pressStartTime = 0;
  const unsigned long pressHoldTime = 50;

  // Lectura del botón con antirrebote
  button.loop();
  if (button.isPressed())
  {
    if (!wasPressed)
    {
      encoder_pressed = true;                     // flanco detectado
      pressStartTime = (unsigned long)::millis(); // inicio de retención
      wasPressed = true;
    }
  }
  else
  {
    if (encoder_pressed && ((unsigned long)::millis() - pressStartTime > pressHoldTime))
    {
      encoder_pressed = false; // soltar después de 50 ms
      Serial.println("APRETE");
    }
    wasPressed = false;
  }
  temp = htu.readTemperature();
  if (current_time - last_temp_read > 200)

  {
    last_temp_read = current_time;

    Serial.println(temp);
    lv_label_set_text(objects.label_temp_date, (String(temp, 2) + " °C").c_str());
  }
}