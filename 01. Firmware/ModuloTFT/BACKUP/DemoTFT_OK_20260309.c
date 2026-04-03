#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <stdio.h>

// ==========================================================
// =================== CONFIGURACIÓN ========================
// ==========================================================

#define TFT_SPI spi0

// Pines TFT
#define PIN_CS    17
#define PIN_DC    15
#define PIN_RST   20
#define PIN_SCK   18
#define PIN_MOSI  19
#define PIN_MISO  16

// Pines Touch
#define PIN_TCS   21
#define PIN_TIRQ  22

// Resolución
#define TFT_W 480
#define TFT_H 320

// Calibración Touch
#define X_MIN 750
#define X_MAX 3050
#define Y_MIN 630
#define Y_MAX 3085

// Colores RGB565
#define C_NEGRO   0x0000
#define C_BLANCO  0xFFFF
#define C_VERDE   0x07E0

// Barra de tareas
#define TOOLBAR_HEIGHT 60
#define TOOLBAR_COLS 4
#define TOOLBAR_BTN_W (TFT_W / TOOLBAR_COLS)
#define DISPLAY_HEIGHT 80

#define CONTENT_TOP DISPLAY_HEIGHT
#define CONTENT_BOTTOM (TFT_H - TOOLBAR_HEIGHT)
#define CONTENT_HEIGHT (CONTENT_BOTTOM - CONTENT_TOP)

// Layout calculadora
#define DISPLAY_HEIGHT 80
#define BTN_ROWS 3
#define BTN_COLS 4
#define BTN_W (TFT_W / BTN_COLS)
#define BTN_H (CONTENT_HEIGHT / BTN_ROWS)

// ==========================================================
// =================== VARIABLES ============================
// ==========================================================

uint32_t current_value = 0;

typedef enum
{
    SCREEN_HOME,
    SCREEN_CALC,
    SCREEN_CONFIG
} screen_t;

screen_t current_screen = SCREEN_HOME;

// ==========================================================
// =================== LOW LEVEL TFT ========================
// ==========================================================

static inline void CS_L() { gpio_put(PIN_CS, 0); }
static inline void CS_H() { gpio_put(PIN_CS, 1); }
static inline void TCS_L() { gpio_put(PIN_TCS, 0); }
static inline void TCS_H() { gpio_put(PIN_TCS, 1); }

void tft_cmd(uint8_t c){
    gpio_put(PIN_DC,0);
    CS_L();
    spi_write_blocking(TFT_SPI,&c,1);
    CS_H();
}

void tft_data(uint8_t d){
    gpio_put(PIN_DC,1);
    CS_L();
    spi_write_blocking(TFT_SPI,&d,1);
    CS_H();
}

void tft_data_buf(uint8_t *b,int n){
    gpio_put(PIN_DC,1);
    CS_L();
    spi_write_blocking(TFT_SPI,b,n);
    CS_H();
}

void tft_reset(){
    gpio_put(PIN_RST,0);
    sleep_ms(60);
    gpio_put(PIN_RST,1);
    sleep_ms(150);
}



// ==========================================================
// =================== GRÁFICOS =============================
// ==========================================================

void set_window(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){

    uint8_t d[4];

    tft_cmd(0x2A);
    d[0]=x0>>8; d[1]=x0;
    d[2]=x1>>8; d[3]=x1;
    tft_data_buf(d,4);

    tft_cmd(0x2B);
    d[0]=y0>>8; d[1]=y0;
    d[2]=y1>>8; d[3]=y1;
    tft_data_buf(d,4);

    tft_cmd(0x2C);
}

void fill_screen(uint16_t c){

    set_window(0,0,TFT_W-1,TFT_H-1);

    uint8_t hi=c>>8, lo=c;
    static uint8_t buf[512];

    for(int i=0;i<512;i+=2){
        buf[i]=hi;
        buf[i+1]=lo;
    }

    gpio_put(PIN_DC,1);
    CS_L();

    int total = TFT_W*TFT_H*2;
    for(int i=0;i<total;i+=512)
        spi_write_blocking(TFT_SPI,buf,512);

    CS_H();
}

void fill_rect(int x,int y,int w,int h,uint16_t c){

    set_window(x,y,x+w-1,y+h-1);

    uint8_t hi=c>>8, lo=c;
    static uint8_t buf[256];

    for(int i=0;i<256;i+=2){
        buf[i]=hi;
        buf[i+1]=lo;
    }

    gpio_put(PIN_DC,1);
    CS_L();

    int total=w*h*2;
    for(int i=0;i<total;i+=256)
        spi_write_blocking(TFT_SPI,buf,256);

    CS_H();
}

// ==========================================================
// =================== 7 SEGMENTOS ==========================
// ==========================================================

const uint8_t digitos7[10] = {
    0b1111110,0b0110000,0b1101101,0b1111001,0b0110011,
    0b1011011,0b1011111,0b1110000,0b1111111,0b1111011
};


void draw_big_digit(int d,int x,int y,uint16_t c){

    int W=30;
    int H=50;
    int T=8;

    uint8_t s = digitos7[d];

    if(s & 0b1000000) fill_rect(x+T,y,W-2*T,T,c);
    if(s & 0b0100000) fill_rect(x+W-T,y+T,T,H/2-T,c);
    if(s & 0b0010000) fill_rect(x+W-T,y+H/2,T,H/2-T,c);
    if(s & 0b0001000) fill_rect(x+T,y+H-T,W-2*T,T,c);
    if(s & 0b0000100) fill_rect(x,y+H/2,T,H/2-T,c);
    if(s & 0b0000010) fill_rect(x,y+T,T,H/2-T,c);
    if(s & 0b0000001) fill_rect(x+T,y+H/2-T/2,W-2*T,T,c);
}

// ==========================================================
// =================== INTERFAZ =============================
// ==========================================================

void draw_display(){

    fill_rect(0,0,TFT_W,DISPLAY_HEIGHT,C_VERDE);

    int d1 = (current_value/1000)%10;
    int d2 = (current_value/100)%10;
    int d3 = (current_value/10)%10;
    int d4 = current_value%10;

    draw_big_digit(d1,40,10,C_BLANCO);
    draw_big_digit(d2,140,10,C_BLANCO);
    draw_big_digit(d3,240,10,C_BLANCO);
    draw_big_digit(d4,340,10,C_BLANCO);
}

void draw_keypad(){

    int num=1;

    for(int row=0; row<BTN_ROWS; row++){
        for(int col=0; col<BTN_COLS; col++){

            int x = col * BTN_W;
            int y = DISPLAY_HEIGHT + row * BTN_H;

            fill_rect(x,y,BTN_W-4,BTN_H-4,C_BLANCO);

            if(num<=9){
                draw_big_digit(num,x+20,y+10,C_NEGRO);
                num++;
            }
            else if(num==10){
                draw_big_digit(0,x+20,y+10,C_NEGRO);
                num++;
            }
        }
    }
}

void draw_screen_config()
{
    fill_rect(0, CONTENT_TOP, TFT_W, CONTENT_HEIGHT, 0xF800);

    draw_big_digit(3, 200, CONTENT_TOP + 80, C_BLANCO);
}

void draw_screen_calc()
{
    fill_rect(0,CONTENT_TOP,TFT_W,CONTENT_HEIGHT,C_NEGRO);

    int num=1;

    for(int r=0;r<BTN_ROWS;r++)
    {
        for(int c=0;c<BTN_COLS;c++)
        {
            int x=c*BTN_W;
            int y=CONTENT_TOP + r*BTN_H;

            fill_rect(x,y,BTN_W-4,BTN_H-4,C_BLANCO);

            if(num<=9)
            {
                draw_big_digit(num,x+20,y+20,C_NEGRO);
                num++;
            }
            else if(num==10)
            {
                draw_big_digit(0,x+20,y+20,C_NEGRO);
                num++;
            }
        }
    }
}

void draw_screen_home()
{
    fill_rect(0, CONTENT_TOP, TFT_W, CONTENT_HEIGHT, 0x001F);

    // Botón CALC
    fill_rect(80, CONTENT_TOP + 60, 140, 80, C_BLANCO);
    draw_big_digit(1, 120, CONTENT_TOP + 80, C_NEGRO);

    // Botón CONFIG
    fill_rect(260, CONTENT_TOP + 60, 140, 80, C_BLANCO);
    draw_big_digit(2, 300, CONTENT_TOP + 80, C_NEGRO);
}


void draw_current_screen()
{
    switch(current_screen)
    {
        case SCREEN_HOME:
            draw_screen_home();
        break;

        case SCREEN_CALC:
            draw_screen_calc();
        break;

        case SCREEN_CONFIG:
            draw_screen_config();
        break;
    }
}

// ==========================================================
// =================== BARRA DE TAREAS =============================
// ==========================================================

void draw_toolbar()
{
    int y = TFT_H - TOOLBAR_HEIGHT;

    for(int i=0;i<4;i++)
    {
        int x = i * (TFT_W/4);

        fill_rect(x, y, TFT_W/4-4, TOOLBAR_HEIGHT-4, C_BLANCO);

        switch(i)
        {
            case 1: // -
                fill_rect(x+40,y+25,40,10,C_NEGRO);
            break;

            case 2: // +
                fill_rect(x+40,y+25,40,10,C_NEGRO);
                fill_rect(x+55,y+10,10,40,C_NEGRO);
            break;

            case 3: // OK
                fill_rect(x+30,y+15,60,30,C_VERDE);
            break;
        }
    }
}

void toolbar_action(int button)
{
    switch(button)
    {
        case 0: // ATRAS
            current_screen = SCREEN_HOME;
            draw_current_screen();
        break;

        case 1: // -
            if(current_value>0)
                current_value--;
            draw_display();
        break;

        case 2: // +
            current_value++;
            draw_display();
        break;

        case 3: // OK
            printf("OK\n");
        break;
    }
}

bool check_toolbar(uint16_t x,uint16_t y)
{
    int y_toolbar = TFT_H - TOOLBAR_HEIGHT;

    if(y < y_toolbar)
        return false;

    int btn = x / (TFT_W/4);

    toolbar_action(btn);

    while(!gpio_get(PIN_TIRQ))
        sleep_ms(5);

    return true;
}

// ==========================================================
// =================== TOUCH ================================
// ==========================================================

uint16_t touch_read(uint8_t cmd){

    CS_H();

    uint8_t tx[3]={cmd,0,0};
    uint8_t rx[3]={0};

    TCS_L();
    sleep_us(2);
    spi_write_read_blocking(TFT_SPI,tx,rx,3);
    TCS_H();

    return ((rx[1]<<8)|rx[2])>>3;
}

bool touch_get_xy(uint16_t *x,uint16_t *y){

    if(gpio_get(PIN_TIRQ)) return false;

    uint16_t rawx=touch_read(0x90);
    uint16_t rawy=touch_read(0xD0);

    int32_t tx=(rawx-X_MIN)*TFT_W/(X_MAX-X_MIN);
    int32_t ty=(rawy-Y_MIN)*TFT_H/(Y_MAX-Y_MIN);

    if(tx<0) tx=0; if(tx>=TFT_W) tx=TFT_W-1;
    if(ty<0) ty=0; if(ty>=TFT_H) ty=TFT_H-1;

    tx = TFT_W - tx;
    ty = TFT_H - ty;

    *x=tx; *y=ty;

    return true;
}

void add_digit(int digit){
    current_value=((current_value%1000)*10)+digit;
    draw_display();
}

/* void check_touch()
{
    uint16_t x,y;

    if(!touch_get_xy(&x,&y))
        return;

    int toolbar_y = TFT_H - TOOLBAR_HEIGHT;

    // Si tocó toolbar
    if(y >= toolbar_y)
    {
        check_toolbar(x,y);
        return;
    }

    // Si tocó keypad
    if(y >= DISPLAY_HEIGHT)
    {
        int col = x / BTN_W;
        int row = (y - DISPLAY_HEIGHT) / BTN_H;

        if(col >= BTN_COLS || row >= BTN_ROWS)
            return;

        int number = row * BTN_COLS + col + 1;

        if(number == 10)
            number = 0;

        if(number <= 10)
            add_digit(number);
    }

    while(!gpio_get(PIN_TIRQ))
        sleep_ms(5);

    if(check_toolbar(x,y))
    return;

    sleep_ms(80);
} */

void check_content_touch(uint16_t x, uint16_t y)
{
    if(y < CONTENT_TOP || y > CONTENT_BOTTOM)
        return;

    switch(current_screen)
    {
        case SCREEN_HOME:

            // Botón CALC
            if(x>80 && x<220 && y>CONTENT_TOP+60 && y<CONTENT_TOP+140)
            {
                current_screen = SCREEN_CALC;
                draw_current_screen();
            }

            // Botón CONFIG
            if(x>260 && x<400 && y>CONTENT_TOP+60 && y<CONTENT_TOP+140)
            {
                current_screen = SCREEN_CONFIG;
                draw_current_screen();
            }

        break;

        case SCREEN_CALC:
        {
            int col = x / BTN_W;
            int row = (y - CONTENT_TOP) / BTN_H;

            if(col >= BTN_COLS || row >= BTN_ROWS)
                return;

            int number = row * BTN_COLS + col + 1;

            if(number == 10)
                number = 0;

            if(number <= 10)
                add_digit(number);
        }
        break;

        case SCREEN_CONFIG:
        break;
    }
}

void check_touch()
{
    uint16_t x,y;

    if(!touch_get_xy(&x,&y))
        return;

    if(check_toolbar(x,y))
        return;

    check_content_touch(x,y);

    while(!gpio_get(PIN_TIRQ))
        sleep_ms(5);

    sleep_ms(80);
}

// ==========================================================
// =================== INIT TFT =============================
// ==========================================================

void st7796_init(){

    tft_reset();

    tft_cmd(0x01); sleep_ms(150);

    tft_cmd(0xF0); tft_data(0xC3);
    tft_cmd(0xF0); tft_data(0x96);

    tft_cmd(0x36); tft_data(0x28);
    tft_cmd(0x3A); tft_data(0x55);

    tft_cmd(0x11); sleep_ms(120);
    tft_cmd(0x29);
}

// ==========================================================
// =================== MAIN =================================
// ==========================================================

int main(){

    stdio_init_all();

    spi_init(TFT_SPI,4000000);
    spi_set_format(TFT_SPI,8,SPI_CPOL_0,SPI_CPHA_0,SPI_MSB_FIRST);

    gpio_set_function(PIN_SCK,GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI,GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO,GPIO_FUNC_SPI);

    gpio_init(PIN_CS); gpio_set_dir(PIN_CS,GPIO_OUT);
    gpio_init(PIN_DC); gpio_set_dir(PIN_DC,GPIO_OUT);
    gpio_init(PIN_RST); gpio_set_dir(PIN_RST,GPIO_OUT);

    gpio_init(PIN_TCS); gpio_set_dir(PIN_TCS,GPIO_OUT);
    gpio_put(PIN_TCS,1);

    gpio_init(PIN_TIRQ);
    gpio_set_dir(PIN_TIRQ,GPIO_IN);
    gpio_pull_up(PIN_TIRQ);

    CS_H();
    sleep_ms(200);

    st7796_init();

    fill_screen(C_NEGRO);
    
    draw_display();
    draw_toolbar();
    draw_current_screen();

    while(1){
        check_touch();
        sleep_ms(5);
    }
}
