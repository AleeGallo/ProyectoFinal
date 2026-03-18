#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#define TFT_SPI spi0

#define PIN_CS   17
#define PIN_DC   16
#define PIN_RST  20
#define PIN_SCK  18
#define PIN_MOSI 19

#define PIN_BTN 15

#define TFT_W 480
#define TFT_H 320

// ---------- LOW LEVEL ----------

static inline void CS_L() { gpio_put(PIN_CS, 0); }
static inline void CS_H() { gpio_put(PIN_CS, 1); }

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

// ---------- INIT ROBUSTO ST7796 SPI ----------

void st7796_init(){

    tft_reset();

    tft_cmd(0x01);
    sleep_ms(150);

    tft_cmd(0xF0); tft_data(0xC3);
    tft_cmd(0xF0); tft_data(0x96);

    tft_cmd(0x36); tft_data(0x28);   // MADCTL estable

    tft_cmd(0x3A); tft_data(0x55);   // RGB565

    tft_cmd(0xB4); tft_data(0x01);
    tft_cmd(0xB7); tft_data(0xC6);

    tft_cmd(0xC1); tft_data(0x06);
    tft_cmd(0xC2); tft_data(0xA7);
    tft_cmd(0xC5); tft_data(0x18);

    // Ventana completa explícita
    tft_cmd(0x2A);
    uint8_t col[4]={0,0, 0x01,0xDF}; // 0-479
    tft_data_buf(col,4);

    tft_cmd(0x2B);
    uint8_t row[4]={0,0, 0x01,0x3F}; // 0-319
    tft_data_buf(row,4);

    tft_cmd(0xF0); tft_data(0x3C);
    tft_cmd(0xF0); tft_data(0x69);

    tft_cmd(0x11);
    sleep_ms(120);

    tft_cmd(0x29);
}

// ---------- WINDOW ----------

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

// ---------- FILL BURST ----------

void fill_color(uint16_t c){

    set_window(0,0,TFT_W-1,TFT_H-1);

    uint8_t hi=c>>8, lo=c&0xFF;

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


bool boton_presionado(){

    static uint32_t last = 0;

    if(!gpio_get(PIN_BTN)){          // activo en LOW
        if(to_ms_since_boot(get_absolute_time()) - last > 180){
            last = to_ms_since_boot(get_absolute_time());
            return true;
        }
    }
    return false;
}


// =========== FUNCIONES ===============

void draw_pixel(uint16_t x, uint16_t y, uint16_t c){
    set_window(x,y,x,y);
    uint8_t d[2]={c>>8,c};
    tft_data_buf(d,2);
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

void test_barras(){

    uint16_t col[6]={0xF800,0xFFE0,0x07E0,0x07FF,0x001F,0xF81F};

    int w=TFT_W/6;

    for(int i=0;i<6;i++)
        fill_rect(i*w,0,w,TFT_H,col[i]);
}

void test_grilla(){

    fill_color(0x0000);

    for(int x=0;x<TFT_W;x+=20)
        for(int y=0;y<TFT_H;y++)
            draw_pixel(x,y,0xFFFF);

    for(int y=0;y<TFT_H;y+=20)
        for(int x=0;x<TFT_W;x++)
            draw_pixel(x,y,0xFFFF);
}

void test_gradiente(){

    for(int x=0;x<TFT_W;x++){
        uint8_t r = x*31/TFT_W;
        uint16_t c = (r<<11);
        fill_rect(x,0,1,TFT_H,c);
    }
}


// ---------- MAIN ----------

int main(){

    stdio_init_all();

    spi_init(TFT_SPI, 8000000);   // estable

    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS); gpio_set_dir(PIN_CS,GPIO_OUT);
    gpio_init(PIN_DC); gpio_set_dir(PIN_DC,GPIO_OUT);
    gpio_init(PIN_RST);gpio_set_dir(PIN_RST,GPIO_OUT);

    gpio_init(PIN_BTN);
    gpio_set_dir(PIN_BTN, GPIO_IN);
    gpio_pull_up(PIN_BTN);

    CS_H();

    sleep_ms(200);

    st7796_init();

    while(1){
        
        test_barras();
        sleep_ms(1200);

        test_gradiente();
        sleep_ms(1200);

        test_grilla();
        sleep_ms(1200);

        fill_rect(40,40,200,120,0xF800);
        fill_rect(80,80,200,120,0x07E0);
        fill_rect(120,120,200,120,0x001F);
        sleep_ms(1200);

        fill_color(0x0000);
    }
}



