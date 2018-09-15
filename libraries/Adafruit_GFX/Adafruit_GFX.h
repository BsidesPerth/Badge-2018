/*
	Adafruit_GFX_Library
	Original:https://github.com/adafruit/Adafruit-GFX-Library
	This version:https://github.com/sumotoy/Adafruit-GFX-Library
	Like with original GFX library but has some bug fix
	plus several useful stuff like user fonts, faster geometric routines,
	Teensy 3 specific optimized drawing routines.
	Even with optimized routines it's still 100% compatible with original librarary
	for all processors.
	Subversion: 1.02
*/

#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif
#include "fonts.h"

//#define swap(a, b) { int16_t t = a; a = b; b = t; }
//#define swap(a, b) { typeof(a) t = a; a = b; b = t; }
inline void swap(int16_t &a, int16_t &b) { int16_t t = a; a = b; b = t; }

#ifndef _ADAFRUIT_GFX_VARIANT
#define _ADAFRUIT_GFX_VARIANT
#endif

class Adafruit_GFX : public Print {

 public:

  Adafruit_GFX(int16_t w, int16_t h); // Constructor

  // This MUST be defined by the subclass:
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void
    drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color),
    drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
    drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
    drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
    fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
    fillScreen(uint16_t color),
    invertDisplay(boolean i),
    drawFastChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

  // These exist only with Adafruit_GFX (no subclass overrides)
  void
    drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color),
	drawArc(int16_t x, int16_t y, int16_t r, int16_t rs, int16_t re, uint16_t color),//new
	ellipse(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color),//new
	drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color),//new
    drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,uint16_t color),
    fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color),
    fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta, uint16_t color),
    drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color),
    fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color),
	drawQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color),
	fillQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color),
	drawPolygon(int16_t cx, int16_t cy, uint8_t sides, int16_t diameter, float rot, uint16_t color),
    drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,int16_t radius, uint16_t color),
    fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,int16_t radius, uint16_t color),
    drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,int16_t w, int16_t h, uint16_t color),
	drawBitmap(int16_t x, int16_t y,const uint8_t *bitmap, int16_t w, int16_t h,uint16_t color, uint16_t bg),//new
	drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap,int16_t w, int16_t h, uint16_t color),//new
    drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,uint16_t bg, uint8_t size),
    setCursor(int16_t x, int16_t y),
    setTextColor(uint16_t c),
    setTextColor(uint16_t c, uint16_t bg),
    setTextSize(uint8_t s),
    setTextWrap(boolean w),
    setRotation(uint8_t r),
    setFont(uint8_t f);

#if ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void   write(uint8_t);
#endif
	int16_t height(void) const;
	int16_t width(void) const;

	
	uint8_t getRotation(void);

 protected:
  	const int16_t WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
  	int16_t _width, _height; // Display w/h as modified by current rotation
  	int16_t cursor_x, cursor_y;
  	uint16_t textcolor, textbgcolor;
  	uint8_t textsize, rotation, font, fontWidth, fontHeight, fontStart, fontLength;
  	int8_t  fontKern;
  	const unsigned char *fontData;
  	boolean wrap; // If set, 'wrap' text at right edge of display
	void plot8points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color);
	void plot4points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color);
	int i_sin(int x);
	int i_cos(int x);
};

class Adafruit_GFX_Button {

 public:
	Adafruit_GFX_Button(void);
	void initButton(Adafruit_GFX *gfx, int16_t x, int16_t y, 
		      uint8_t w, uint8_t h, 
		      uint16_t outline, uint16_t fill, uint16_t textcolor,
		      char *label, uint8_t textsize);
	void drawButton(boolean inverted = false);
	boolean contains(int16_t x, int16_t y);

	void press(boolean p);
	boolean isPressed();
	boolean justPressed();
	boolean justReleased();

 private:
	Adafruit_GFX *_gfx;
	int16_t _x, _y;
	uint16_t _w, _h;
	uint8_t _textsize;
	uint16_t _outlinecolor, _fillcolor, _textcolor;
	char _label[10];

	boolean currstate, laststate;
};

#endif // _ADAFRUIT_GFX_H
