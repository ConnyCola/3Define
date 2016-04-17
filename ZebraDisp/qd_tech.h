/*
 * qd_tech.h
 *
 *      Author: Matthias Wagner
 */

#ifndef __QD_TECH_H__
#define __QD_TECH_H__

#define BOOL int

#define QDTech_TFTWIDTH  128
#define QDTech_TFTHEIGHT 160

// some flags for initR() :(
#define INITR_GREENTAB 0x0
#define INITR_REDTAB   0x1
#define INITR_BLACKTAB   0x2

#define INITR_18GREENTAB    INITR_GREENTAB
#define INITR_18REDTAB      INITR_REDTAB
#define INITR_18BLACKTAB    INITR_BLACKTAB
#define INITR_144GREENTAB   0x1

#define ST7735_TFTWIDTH  128
// for 1.44" display
#define ST7735_TFTHEIGHT_144 128
// for 1.8" display
#define ST7735_TFTHEIGHT_18  160


#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

// Some used command definitions kept from original
#define QDTech_INVOFF  0x20
#define QDTech_INVON   0x21
#define QDTech_DISPOFF 0x28
#define QDTech_DISPON  0x29
#define QDTech_CASET   0x2A
#define QDTech_RASET   0x2B
#define QDTech_RAMWR   0x2C
#define QDTech_RAMRD   0x2E

#define QDTech_PTLAR   0x30
#define QDTech_COLMOD  0x3A
#define QDTech_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Basic colour definitions
#define QDTech_BLACK   0x0000
#define QDTech_RED     0xF800
#define QDTech_GREEN   0x07E0
#define QDTech_BLUE    0x001F
#define QDTech_YELLOW  0xFFE0
#define QDTech_MAGENTA 0xF81F
#define QDTech_CYAN    0x07FF
#define QDTech_WHITE   0xFFFF
#define QDTech_GREY    0x632C


void qd_tech_init();
void qd_tech_initR(uint8_t options);
void qd_tech_setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void qd_tech_pushColor(uint16_t color);
void qd_tech_fillScreen(uint16_t color);
void qd_tech_drawPixel(int16_t x, int16_t y, uint16_t color);
void qd_tech_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void qd_tech_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void qd_tech_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void qd_tech_setRotation(uint8_t r);
void qd_tech_invertDisplay(BOOL i);

void qd_tech_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void qd_tech_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void qd_tech_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void qd_tech_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void qd_tech_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void qd_tech_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void qd_tech_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void qd_tech_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void qd_tech_drawTriangle(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color);
void qd_tech_fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void qd_tech_drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

void qd_tech_write(uint8_t c);
void qd_tech_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void qd_tech_setCursor(int16_t x, int16_t y);
void qd_tech_setTextSize(uint8_t s);
void qd_tech_setTextColor(uint16_t c);
//void qd_tech_setTextColor(uint16_t c, uint16_t b);
void qd_tech_setTextWrap(BOOL w);
uint8_t qd_tech_getRotation(void);
//void qd_tech_setRotation(uint8_t x);
int16_t qd_tech_width(void);
int16_t qd_tech_height(void);

void qd_tech_print(char c[]);
void qd_tech_println(char c[]);


uint16_t qd_tech_Color565(uint8_t r, uint8_t g, uint8_t b);




#endif
