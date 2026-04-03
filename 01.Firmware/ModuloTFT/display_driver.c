#include "display_driver.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

// ================= CONFIG =================
#define TFT_SPI spi0

#define PIN_CS   17
#define PIN_DC   15
#define PIN_RST  20
#define PIN_SCK  18
#define PIN_MOSI 19

// ================= LOW LEVEL =================
static inline void CS_L(void) { gpio_put(PIN_CS, 0); }
static inline void CS_H(void) { gpio_put(PIN_CS, 1); }

static void tft_cmd(uint8_t cmd)
{
    gpio_put(PIN_DC, 0);
    CS_L();
    spi_write_blocking(TFT_SPI, &cmd, 1);
    CS_H();
}

static void tft_data_buf(const uint8_t *data, int len)
{
    gpio_put(PIN_DC, 1);
    CS_L();
    spi_write_blocking(TFT_SPI, data, len);
    CS_H();
}

// ================= WINDOW =================
static void set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t d[4];

    tft_cmd(0x2A); // Column address
    d[0] = x0 >> 8; d[1] = x0;
    d[2] = x1 >> 8; d[3] = x1;
    tft_data_buf(d, 4);

    tft_cmd(0x2B); // Row address
    d[0] = y0 >> 8; d[1] = y0;
    d[2] = y1 >> 8; d[3] = y1;
    tft_data_buf(d, 4);

    tft_cmd(0x2C); // Memory write
}

// ================= ST7796 =================
static void tft_reset(void)
{
    gpio_put(PIN_RST, 0);
    sleep_ms(60);
    gpio_put(PIN_RST, 1);
    sleep_ms(150);
}

void st7796_init(void)
{
    tft_reset();

    tft_cmd(0x01); // Software reset
    sleep_ms(150);

    tft_cmd(0xF0); // Unlock
    uint8_t d1 = 0xC3;
    tft_data_buf(&d1, 1);

    tft_cmd(0xF0);
    uint8_t d2 = 0x96;
    tft_data_buf(&d2, 1);

    tft_cmd(0x36); // MADCTL
    uint8_t madctl = 0x28;
    tft_data_buf(&madctl, 1);

    tft_cmd(0x3A); // Pixel format
    uint8_t col = 0x55; // RGB565
    tft_data_buf(&col, 1);

    tft_cmd(0x11); // Sleep out
    sleep_ms(120);

    tft_cmd(0x29); // Display on
}

// ================= PUBLIC FLUSH =================
void st7796_flush(int16_t x1, int16_t y1, int16_t x2, int16_t y2, const uint16_t *color_p)
{
    set_window(x1, y1, x2, y2);
    tft_data_buf((const uint8_t *)color_p, (x2 - x1 + 1) * (y2 - y1 + 1) * 2);
}

// ================= PUBLIC INIT =================
void display_driver_init(void)
{
    // SPI
    spi_init(TFT_SPI, 8000000);
    spi_set_format(TFT_SPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);   gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_init(PIN_DC);   gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_init(PIN_RST);  gpio_set_dir(PIN_RST, GPIO_OUT);

    CS_H();

    sleep_ms(200);
}