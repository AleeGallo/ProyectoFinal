#ifndef ENCODER_DRIVER_H
#define ENCODER_DRIVER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void encoder_driver_init(void);
void encoder_set_rotate_handler(void (*handler)(int diff));
void encoder_set_button_handler(void (*handler)(bool pressed));

#ifdef __cplusplus
}
#endif

#endif // ENCODER_DRIVER_H
