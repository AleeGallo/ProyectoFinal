#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_update_label_modo(lv_event_t * e);
extern void action_update_modo_dropdown(lv_event_t * e);
extern void action_update_tipo_jeringa(lv_event_t * e);
extern void action_select_metodo(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/