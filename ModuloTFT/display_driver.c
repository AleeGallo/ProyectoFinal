#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "hardware/spi.h"
#include "lvgl.h"

#define TFT_WIDTH   480
#define TFT_HEIGHT  320

#define PIN_MOSI 19
#define PIN_SCK  18
#define PIN_CS   17
#define PIN_DC   16
#define PIN_RST  20

#define SPI_PORT spi0

static lv_display_t *disp;

/* LVGL buffer (20 lines) */
static lv_color_t lv_buffer[TFT_WIDTH * 20];


/* ---------------- LOW LEVEL ---------------- */

static inline void lcd_cmd(uint8_t cmd)
{
    gpio_put(PIN_DC,0);
    gpio_put(PIN_CS,0);
    spi_write_blocking(SPI_PORT,&cmd,1);
    gpio_put(PIN_CS,1);
}

static inline void lcd_data(uint8_t *data,int len)
{
    gpio_put(PIN_DC,1);
    gpio_put(PIN_CS,0);
    spi_write_blocking(SPI_PORT,data,len);
    gpio_put(PIN_CS,1);
}


/* ventana de escritura */
static void set_window(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1)
{
    uint8_t data[4];

    lcd_cmd(0x2A);
    data[0]=x0>>8;
    data[1]=x0&0xFF;
    data[2]=x1>>8;
    data[3]=x1&0xFF;
    lcd_data(data,4);

    lcd_cmd(0x2B);
    data[0]=y0>>8;
    data[1]=y0&0xFF;
    data[2]=y1>>8;
    data[3]=y1&0xFF;
    lcd_data(data,4);

    lcd_cmd(0x2C);
}


/* ---------------- LVGL FLUSH ---------------- */

static void flush_cb(lv_display_t *disp,
                     const lv_area_t *area,
                     uint8_t *px_map)
{
    int w = area->x2 - area->x1 + 1;
    int h = area->y2 - area->y1 + 1;

    set_window(area->x1,area->y1,area->x2,area->y2);

    gpio_put(PIN_DC,1);
    gpio_put(PIN_CS,0);

    spi_write_blocking(SPI_PORT,px_map,w*h*2);

    gpio_put(PIN_CS,1);

    lv_display_flush_ready(disp);
}


/* ---------------- TFT INIT ---------------- */

static void tft_reset()
{
    gpio_put(PIN_RST,0);
    sleep_ms(50);
    gpio_put(PIN_RST,1);
    sleep_ms(120);
}

static void st7796_init()
{
    tft_reset();

    lcd_cmd(0x36);
    uint8_t madctl = 0x48;
    lcd_data(&madctl,1);

    lcd_cmd(0x3A);
    uint8_t col = 0x55; // RGB565
    lcd_data(&col,1);

    lcd_cmd(0x11);
    sleep_ms(120);

    lcd_cmd(0x29);
}


/* ---------------- PUBLIC INIT ---------------- */

void display_driver_init()
{
    spi_init(SPI_PORT,40000000);

    gpio_set_function(PIN_MOSI,GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS,GPIO_OUT);
    gpio_put(PIN_CS,1);

    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC,GPIO_OUT);

    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST,GPIO_OUT);

    st7796_init();


    lv_init();

    disp = lv_display_create(TFT_WIDTH,TFT_HEIGHT);

    lv_display_set_flush_cb(disp,flush_cb);

    lv_display_set_buffers(
            disp,
            lv_buffer,
            NULL,
            sizeof(lv_buffer),
            LV_DISPLAY_RENDER_MODE_PARTIAL
    );
}