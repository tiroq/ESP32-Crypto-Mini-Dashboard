#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

// Display and LVGL configuration
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define LVGL_BUFFER_SIZE (SCREEN_WIDTH * 40)

TFT_eSPI tft = TFT_eSPI();
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[LVGL_BUFFER_SIZE];
static lv_disp_drv_t disp_drv;

// Display flushing callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Crypto Dashboard starting...");

    // Initialize display
    tft.begin();
    tft.setRotation(1); // Landscape
    tft.fillScreen(TFT_BLACK);

    // Turn on backlight
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // Initialize LVGL
    lv_init();

    // Setup display buffer
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_BUFFER_SIZE);

    // Initialize display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Create a simple label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Crypto Dashboard");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0x00FF00), 0);
    lv_obj_center(label);

    Serial.println("Display initialized");
}

void loop() {
    lv_timer_handler(); // LVGL task handler
    delay(5);
}
