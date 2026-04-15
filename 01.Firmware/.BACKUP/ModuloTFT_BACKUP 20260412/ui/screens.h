#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_MENU_PRINCIPAL = 2,
    SCREEN_ID_MENU_REMOTO = 3,
    SCREEN_ID_MENU_LOCAL_JERINGA = 4,
    SCREEN_ID_MENU_LOCAL_MODO = 5,
    SCREEN_ID_MENU_LOCAL_TIEMPO = 6,
    _SCREEN_ID_LAST = 6
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *menu_principal;
    lv_obj_t *menu_remoto;
    lv_obj_t *menu_local_jeringa;
    lv_obj_t *menu_local_modo;
    lv_obj_t *menu_local_tiempo;
    lv_obj_t *btn_inicio;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *btn_inicio_1;
    lv_obj_t *btn_inicio_2;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_menu_principal();
void tick_screen_menu_principal();

void create_screen_menu_remoto();
void tick_screen_menu_remoto();

void create_screen_menu_local_jeringa();
void tick_screen_menu_local_jeringa();

void create_screen_menu_local_modo();
void tick_screen_menu_local_modo();

void create_screen_menu_local_tiempo();
void tick_screen_menu_local_tiempo();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/