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
    SCREEN_ID_MENU_LOCAL_PARAMETROS = 6,
    SCREEN_ID_MENU_LOCAL_PARAMETROS_CONFIRM = 7,
    SCREEN_ID_MENU_LOCAL_TIEMPO = 8,
    SCREEN_ID_MENU_LOCAL_PARAMETROS_1 = 9,
    _SCREEN_ID_LAST = 9
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *menu_principal;
    lv_obj_t *menu_remoto;
    lv_obj_t *menu_local_jeringa;
    lv_obj_t *menu_local_modo;
    lv_obj_t *menu_local_parametros;
    lv_obj_t *menu_local_parametros_confirm;
    lv_obj_t *menu_local_tiempo;
    lv_obj_t *menu_local_parametros_1;
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
    lv_obj_t *menu_modo_1;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *menu_modo;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *obj23;
    lv_obj_t *container_caudal;
    lv_obj_t *obj24;
    lv_obj_t *container_tiempo;
    lv_obj_t *obj25;
    lv_obj_t *container_volumen;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *obj29;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *obj34;
    lv_obj_t *obj35;
    lv_obj_t *obj36;
    lv_obj_t *obj37;
    lv_obj_t *obj38;
    lv_obj_t *obj39;
    lv_obj_t *obj40;
    lv_obj_t *obj41;
    lv_obj_t *obj42;
    lv_obj_t *obj43;
    lv_obj_t *obj44;
    lv_obj_t *obj45;
    lv_obj_t *label_modo_1;
    lv_obj_t *modo_dropdown_1;
    lv_obj_t *jeringa_dropdown_1;
    lv_obj_t *label_jeringa_1;
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

void create_screen_menu_local_parametros();
void tick_screen_menu_local_parametros();

void create_screen_menu_local_parametros_confirm();
void tick_screen_menu_local_parametros_confirm();

void create_screen_menu_local_tiempo();
void tick_screen_menu_local_tiempo();

void create_screen_menu_local_parametros_1();
void tick_screen_menu_local_parametros_1();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/