#include <Arduino.h>
#include "U8glib-HAL.h" 
//Initialize display.
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);
void draw(void);

void setup(void)
{
//Set font.
// u8g.setFont(u8g_font_helvB12);
}

void loop(void)
{
u8g.firstPage();
do {
draw();
} while (u8g.nextPage());
//Delay before repeating the loop.
delay(50);
}

void draw(void)
{
//Write text. (x, y, text)
u8g.setFont(u8g_font_helvB12);
u8g.drawStr(20, 20, "Hello World.");
u8g.drawBox(10,12,20,30);
u8g.drawRBox(100,0,8,15,1);
u8g.setFont(u8g_font_gdb17);
u8g.drawStr(20, 50, "Hello World.");
}