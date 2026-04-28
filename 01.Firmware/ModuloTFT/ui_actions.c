#include "lvgl.h"
#include "ui.h"   // importante para acceder a variables de EEZ
#include "vars.h"
#include "ui/screens.h"
#include "actions.h"
#include "eez-flow.h"
#include <string.h>

typedef enum {
    MODO_NULL = -1,
    MODO_CONTINUO = 0,
    MODO_BOLO = 1,
    MODO_KVO = 2,
    MODO_PURGA = 3,
    MODO_INTERMITENTE = 4,
    MODO_POR_TIEMPO = 5
} modo_t;

/***************VARIABLES*******************/

modo_t selected_mode = MODO_NULL;
char modo_string[100] = { 0 };
int32_t modo_seleccionado;
int32_t metodo_seleccionado;
int32_t tipo_jeringa;
char str_modo_seleccionado[100] = { 0 };
char str_tipo_jeringa[100] = { 0 };

float set_caudal, set_tiempo, set_volumen;


void actualizar_seteo_por_metodo(int metodo);


/* ------ Funciones para obtener y establecer variables globales    ------ */
int32_t get_var_modo_seleccionado() {
    return modo_seleccionado;
}

void set_var_modo_seleccionado(int32_t value) {
    modo_seleccionado = value;
}

int32_t get_var_tipo_jeringa() {
    return tipo_jeringa;
}

void set_var_tipo_jeringa(int32_t value) {
    tipo_jeringa = value;
}

int32_t get_var_metodo_seleccionado() {
    return metodo_seleccionado;
}

void set_var_metodo_seleccionado(int32_t value) {
    metodo_seleccionado = value;
}

float get_var_set_caudal() {
    return set_caudal;
}

void set_var_set_caudal(float value) {
    set_caudal = value;
}

float get_var_set_tiempo() {
    return set_tiempo;
}

void set_var_set_tiempo(float value) {
    set_tiempo = value;
}

float get_var_set_volumen() {
    return set_volumen;
}

void set_var_set_volumen(float value) {
    set_volumen = value;
}

const char *get_var_str_modo_seleccionado() {
    return str_modo_seleccionado;
}

void set_var_str_modo_seleccionado(const char *value) {
    if (value) {
        strncpy(str_modo_seleccionado, value, sizeof(str_modo_seleccionado) - 1);
        str_modo_seleccionado[sizeof(str_modo_seleccionado) - 1] = '\0';
    }
}

const char *get_var_str_tipo_jeringa() {
    return str_tipo_jeringa;
}

void set_var_str_tipo_jeringa(const char *value) {
    if (value) {
        strncpy(str_tipo_jeringa, value, sizeof(str_tipo_jeringa) - 1);
        str_tipo_jeringa[sizeof(str_tipo_jeringa) - 1] = '\0';
    }
}

/* void tipo_modo_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    selected_mode = lv_btnmatrix_get_selected_btn(obj);
}
 */

 /*************** FUNCIONES *******************/

const char *get_var_modo_string() {
    switch(modo_seleccionado)
    {
        case MODO_CONTINUO: return "Continuo";
        case MODO_BOLO: return "Bolo";
        case MODO_KVO: return "KVO";
        case MODO_PURGA: return "Purga";
        case MODO_INTERMITENTE: return "Intermitente";
        case MODO_POR_TIEMPO: return "Por Tiempo";
        default: return "-";
    }
}


const char *jeringa_index_to_string(int32_t index)
{
    switch(index)
    {
        case 0: return "1 mL";
        case 1: return "2 mL";
        case 2: return "3 mL";
        case 3: return "5mL";
        case 4: return "10 mL";
        case 5: return "20 mL";
        case 6: return "30 mL";
        case 7: return "50 mL";
        case 8: return "60 mL";
        default: return "-";
    }
}


/**************** EVENT HANDLERS *****************/

void action_update_label_modo(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) {
        return;
    }

    lv_obj_t * obj = lv_event_get_target(e);
    void *flowState = lv_event_get_user_data(e);
    int selected_btn = lv_btnmatrix_get_selected_btn(obj);

    if (selected_btn < 0) {
        return;
    }

    selected_mode = (modo_t) selected_btn;
    set_var_modo_seleccionado((int32_t)selected_mode);

    const char *btn_text = lv_btnmatrix_get_btn_text(obj, selected_btn);
    /* if (btn_text == NULL) {
        btn_text = modo_to_string(selected_mode);
    } */

    // Actualizar la variable de cadena con el texto del botón
    set_var_str_modo_seleccionado(btn_text);
}


void action_update_modo_dropdown(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) {
        return;
    }

    lv_obj_t * obj = lv_event_get_target(e);
    void *flowState = lv_event_get_user_data(e);
    uint16_t selected_idx = lv_dropdown_get_selected(obj);

    selected_mode = (modo_t) selected_idx;
    set_var_modo_seleccionado((int32_t)selected_mode);

    //const char *selected_text = modo_to_string(selected_mode);
}

void action_select_metodo(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) {
        return;
    }

    lv_obj_t *obj = lv_event_get_target(e);
    if (obj == NULL) {
        return;
    }

    int selected_idx = lv_dropdown_get_selected(obj);
    if (selected_idx < 0) {
        return;
    }

    set_var_metodo_seleccionado((int32_t)selected_idx);

    actualizar_seteo_por_metodo(selected_idx);
}

void action_update_tipo_jeringa(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) {
        return;
    }

    lv_obj_t * obj = lv_event_get_target(e);
    void *flowState = lv_event_get_user_data(e);
    int selected_btn = lv_btnmatrix_get_selected_btn(obj);

    if (selected_btn < 0) {
        return;
    }

    set_var_tipo_jeringa((int32_t)selected_btn);

    const char *btn_text = lv_btnmatrix_get_btn_text(obj, selected_btn);
    if (btn_text == NULL) {
        btn_text = jeringa_index_to_string((int32_t)selected_btn);
    }

    // Actualizar la variable de cadena con el texto del botón
    set_var_str_tipo_jeringa(btn_text);
}



/**************** FUNCIONES INDEPENDIENTES *****************/

void actualizar_seteo_por_metodo(int metodo)
{
    // Metodo por CAUDAL
    if(metodo == 0)
    {
        lv_obj_clear_flag(objects.container_caudal, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(objects.container_volumen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(objects.container_tiempo, LV_OBJ_FLAG_HIDDEN);
    }
    // Metodo por VOLUMEN y TIEMPO
    else
    {
        lv_obj_add_flag(objects.container_caudal, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(objects.container_volumen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(objects.container_tiempo, LV_OBJ_FLAG_HIDDEN);
    }
}