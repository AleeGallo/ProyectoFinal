#include "pico/stdlib.h"
#include "lvgl.h"
#include "hardware/timer.h"

#include "display_driver.h"
#include "touch_driver.h"
#include "encoder_driver.h"
#include "ui/ui.h"
//#include "pico/time.h"

/* ------------------- DEFINES -------------------*/


/* ---------- VARIABLES Y ESTRUCTURAS ------------*/

typedef struct {
    float flow_rate;
    float volume;
    float time;
    bool use_kvo;
    uint8_t mode;
    uint8_t method;
} infusion_config_t;

// Timer callback para incrementar el tick de LVGL
bool lv_tick_timer_callback(struct repeating_timer *t) {
    lv_tick_inc(5);
    return true; // Repetir
}

int main()
{
    /* Inicializaciones*/
    stdio_init_all();
    lv_init();
    display_driver_init();
    touch_driver_init();
    //encoder_driver_init();

    ui_init();

    // Configurar timer para incrementar tick cada 5ms
    struct repeating_timer timer;
    add_repeating_timer_ms(5, lv_tick_timer_callback, NULL, &timer);

    while (1)
    {
        lv_timer_handler();
        ui_tick();
        sleep_ms(5);
    }
}
