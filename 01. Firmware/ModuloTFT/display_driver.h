#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdint.h>

// Inicializa el SPI y la pantalla
void display_driver_init(void);

// Inicializa el ST7796 (reset + config)
void st7796_init(void);

// Envia un rectángulo de color al display
// color_p apunta a un buffer RGB565
void st7796_flush(int16_t x1, int16_t y1, int16_t x2, int16_t y2, const uint16_t *color_p);

#endif