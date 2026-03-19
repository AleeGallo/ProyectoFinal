#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "lvgl.h"


// funciones del driver
extern void set_window(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1);
extern void tft_data_buf(uint8_t *b,int n);
extern void st7796_init(void);

#define TFT_WIDTH  480
#define TFT_HEIGHT 320

// buffer LVGL
static lv_color_t buf[TFT_WIDTH * 20];

static lv_display_t *disp;

static lv_obj_t *label_flow;
static lv_obj_t *label_vol;
static lv_obj_t *bar_infusion;

static int flow = 123;
static int volume = 500;

static int infused = 300;
static int volume_total = 500;


// ----------------------------------------------------
// -------------------- CALLBACKS  ------------------------
// ----------------------------------------------------

static void flush_cb(lv_display_t *disp,
                     const lv_area_t *area,
                     uint8_t *px_map)
{
    int w = area->x2 - area->x1 + 1;
    int h = area->y2 - area->y1 + 1;

    set_window(area->x1, area->y1, area->x2, area->y2);
    tft_data_buf(px_map, w * h * 2);

    lv_display_flush_ready(disp);
}

// ----------------------------------------------------
// --------------------- INITS ------------------------
// ----------------------------------------------------


// init display
void display_init(void)
{
    lv_init();

    st7796_init();

    disp = lv_display_create(TFT_WIDTH, TFT_HEIGHT);

    lv_display_set_flush_cb(disp, flush_cb);

    lv_display_set_buffers(
            disp,
            buf,
            NULL,
            sizeof(buf),
            LV_DISPLAY_RENDER_MODE_PARTIAL
    );
}






// actualizar valores
static void update_values()
{
    char txt[32];

    snprintf(txt,sizeof(txt),"%04d ml/h",flow);
    lv_label_set_text(label_flow,txt);

    snprintf(txt,sizeof(txt), "Volumen: %04d ml",volume);
    lv_label_set_text(label_vol,txt);

    lv_bar_set_value(bar_infusion, infused, LV_ANIM_ON);
}


// evento botones
static void btn_event(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    const char *txt = lv_label_get_text(lv_obj_get_child(btn,0));

    if(strcmp(txt,"+")==0)
    {
        flow++;
        infused += 10;
    }

    if(strcmp(txt,"-")==0)
    {
        if(flow > 0) flow--;
        if(infused > 0) infused -= 10;
    }

    if(strcmp(txt,"OK")==0)
    {
        volume += 10;
    }

    if(strcmp(txt,"ATRAS")==0)
    {
        flow = 0;
        volume = 0;
        infused = 0;
    }

    if(infused > volume_total)
        infused = volume_total;

    update_values();
}


// crear boton
static lv_obj_t* create_button(lv_obj_t *parent,const char *txt,int x)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_size(btn,120,60);
    lv_obj_align(btn,LV_ALIGN_BOTTOM_LEFT,x,0);

    lv_obj_add_event_cb(btn,btn_event,LV_EVENT_CLICKED,NULL);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label,txt);
    lv_obj_center(label);

    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1F3B73), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);

    return btn;
}


// UI principal
void ui_create()
{
    lv_obj_t *scr = lv_scr_act();

    // fondo azul oscuro
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0F0A3B), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    // titulo
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title,"BOMBA DE INFUSION");
    lv_obj_align(title,LV_ALIGN_TOP_MID,0,10);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);

    // flujo grande
    label_flow = lv_label_create(scr);
    lv_obj_set_style_text_font(label_flow,&lv_font_montserrat_48,0);
    lv_obj_set_style_text_color(label_flow, lv_color_white(), 0);
    lv_obj_align(label_flow,LV_ALIGN_CENTER,0,-40);

    // volumen
    label_vol = lv_label_create(scr);
    lv_obj_set_style_text_font(label_vol,&lv_font_montserrat_22,0);
    lv_obj_set_style_text_color(label_vol, lv_color_white(), 0);
    lv_obj_align(label_vol,LV_ALIGN_CENTER,0,30);

    // barra de infusion
    bar_infusion = lv_bar_create(scr);

    lv_obj_set_size(bar_infusion,400,30);
    lv_obj_align(bar_infusion,LV_ALIGN_CENTER,0,80);

    lv_bar_set_range(bar_infusion,0,volume_total);
    lv_bar_set_value(bar_infusion,infused,LV_ANIM_OFF);

    // fondo barra
    lv_obj_set_style_bg_color(bar_infusion, lv_color_hex(0x202020), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bar_infusion, LV_OPA_COVER, LV_PART_MAIN);

    // progreso
    lv_obj_set_style_bg_color(bar_infusion, lv_color_hex(0x00FF66), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(bar_infusion, LV_OPA_COVER, LV_PART_INDICATOR);


    update_values();

    // botones
    create_button(scr,"ATRAS",0);
    create_button(scr,"-",120);
    create_button(scr,"+",240);
    create_button(scr,"OK",360);
}