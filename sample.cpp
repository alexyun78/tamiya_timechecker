#include <Arduino.h>
#include "U8glib-HAL.h" 

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);        // I2C / TWI
u8g_uint_t offset;			// current offset for the scrolling text
u8g_uint_t width;			// pixel width of the scrolling text (must be lesser than 128 unless u8g_16BIT is defined
const char *text = "Hello World I love you ";	// scroll this text from right to left

void draw(void) {
u8g.setColorIndex(1);
// graphic commands to redraw the complete screen should be placed here
u8g.setFont(u8g_font_unifont);
//u8g.setFont(u8g_font_osb21);
u8g.drawStr( 0, 60, "Hello World!");
u8g.drawRBox(0,0,50,30,1);
u8g.setColorIndex(0);
u8g.drawBox(20,10,10,10);
}

void setup(void) {
  u8g.begin();  
  
  u8g.setFont(u8g_font_unifont);	// set the target font to calculate the pixel width
  width = u8g.getStrWidth(text);		// calculate the pixel width of the text
  
//   u8g.setFontMode(0);		// enable transparent mode, which is faster
}

void loop(void) {
  u8g_uint_t x;
  
  u8g.firstPage();
  do {
  
    // draw the scrolling text at current offset
    x = offset;
    u8g.setFont(u8g_font_unifont);		// set the target font
    do {								// repeated drawing of the scrolling text...
      u8g.drawStr(x, 30, text);			// draw the scolling text
      x += width;						// add the pixel width of the scrolling text
    } while( x < u8g.getWidth() );		// draw again until the complete display is filled
    
    u8g.setFont(u8g_font_unifont);		// draw the current pixel width
    u8g.setCursorPos(0, 58);
    u8g.print(width);					// this value must be lesser than 128 unless u8g_16BIT is set
    
  } while ( u8g.nextPage() );
  
  offset-=1;							// scroll by one pixel
  if ( (u8g_uint_t)offset < (u8g_uint_t)-width )	
    offset = 0;							// start over again
    
  delay(10);
}