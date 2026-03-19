#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "lvgl.h"
#include "display_driver.h"

// ================= CONFIG =================

#define TFT_SPI spi0

#define PIN_TCS   21
#define PIN_TIRQ  22

#define TFT_W 480
#define TFT_H 320

 #define X_MIN 750
#define X_MAX 3050
#define Y_MIN 630
#define Y_MAX 3085

/* #define X_MIN 0
#define X_MAX 0
#define Y_MIN 0
#define Y_MAX 0 */

static inline void TCS_L() { gpio_put(PIN_TCS, 0); }
static inline void TCS_H() { gpio_put(PIN_TCS, 1); }

// ================= READ =================

uint16_t touch_read(uint8_t cmd)
{
    display_deselect();     // Deselecciona el Display -> CS_H()
    
    uint8_t tx[3] = {cmd, 0, 0};
    uint8_t rx[3] = {0};
    
    TCS_L();    // Activa el Touch
    sleep_us(2);
    spi_write_read_blocking(TFT_SPI, tx, rx, 3);
    TCS_H();

    return ((rx[1] << 8) | rx[2]) >> 3;
}

static bool touch_get_xy(uint16_t *x, uint16_t *y)
{
    if (gpio_get(PIN_TIRQ)) return false;

    uint16_t rawx = touch_read(0x90);
    uint16_t rawy = touch_read(0xD0);

    int32_t tx = (rawx - X_MIN) * TFT_W / (X_MAX - X_MIN);
    int32_t ty = (rawy - Y_MIN) * TFT_H / (Y_MAX - Y_MIN);

    if(tx < 0) tx = 0;
    if(tx >= TFT_W) tx = TFT_W - 1;
    if(ty < 0) ty = 0;
    if(ty >= TFT_H) ty = TFT_H - 1;

    tx = TFT_W - tx;
    ty = TFT_H - ty;

    *x = tx;
    *y = ty;

    return true;
}

// --------- Callback -------------

static void touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t x, y;

    if(touch_get_xy(&x, &y))
    {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = x;
        data->point.y = y;
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// --------- INIT -------------

void touch_driver_init(void)
{
    gpio_init(PIN_TCS);
    gpio_set_dir(PIN_TCS, GPIO_OUT);
    gpio_put(PIN_TCS, 1);

    gpio_init(PIN_TIRQ);
    gpio_set_dir(PIN_TIRQ, GPIO_IN);
    gpio_pull_up(PIN_TIRQ);

    // LVGL input device
    lv_indev_t *indev = lv_indev_create();

    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read_cb);
} 
