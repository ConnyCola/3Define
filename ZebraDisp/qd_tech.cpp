/*
 * qd_tech.c
 *
 *      Author: Matthias Wagner
 */
#include <msp432.h>
#include <Energia.h>
#include <SPI.h>
#include "glcdfont.h"

#include "qd_tech.h"

#define DELAY 0x80

//static const int16_t WIDTH;
//static const int16_t HEIGHT;   // This is the 'raw' display w/h - never changes
static int16_t _width, _height, // Display w/h as modified by current rotation
               cursor_x, cursor_y;
static uint16_t textcolor, textbgcolor;
static uint8_t textsize, rotation;
static BOOL wrap; // If set, 'wrap' text at right edge of display

static uint8_t colstart, rowstart; // some displays need this changed

static uint8_t  tabcolor;

void swap(int16_t *x, int16_t *y)
  {
	int16_t t;
    t=*x;
    *x = *y;
    *y = t;
  }

static void 
  qd_tech_spiwrite(uint8_t c) {
  P2OUT &= ~0x01;                 //CS low
  SPI.transfer(c); //Send register location
  P2OUT |= 0x01; ;                //CS high


}

static void 
 qd_tech_writecommand(uint8_t c) {
    P2OUT &= ~0x02;								//DC low
    qd_tech_spiwrite(c);
}

static void 
 qd_tech_writedata(uint8_t c) {
	P2OUT |= 0x02; 								//DC high
        qd_tech_spiwrite(c);
}

/*
static const uint8_t 	// Multiple LCD init commands removed
  QDTech[] = {                  // QDTech support only now
	29,
	0xf0,	2,	0x5a, 0x5a,				// Excommand2
	0xfc,	2,	0x5a, 0x5a,				// Excommand3
	0x26,	1,	0x01,					// Gamma set
	0xfa,	15,	0x02, 0x1f,	0x00, 0x10,	0x22, 0x30, 0x38, 0x3A, 0x3A, 0x3A,	0x3A, 0x3A,	0x3d, 0x02, 0x01,	// Positive gamma control
	0xfb,	15,	0x21, 0x00,	0x02, 0x04,	0x07, 0x0a, 0x0b, 0x0c, 0x0c, 0x16,	0x1e, 0x30,	0x3f, 0x01, 0x02,	// Negative gamma control
	0xfd,	11,	0x00, 0x00, 0x00, 0x17, 0x10, 0x00, 0x01, 0x01, 0x00, 0x1f, 0x1f,							// Analog parameter control
	0xf4,	15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x07, 0x00, 0x3C, 0x36, 0x00, 0x3C, 0x36, 0x00,	// Power control
	0xf5,	13, 0x00, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x66, 0x06,				// VCOM control
	0xf6,	11, 0x02, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x02, 0x00, 0x06, 0x01, 0x00,							// Source control
	0xf2,	17, 0x00, 0x01, 0x03, 0x08, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x08, 0x08,	//Display control
	0xf8,	1,	0x11,					// Gate control
	0xf7,	4, 0xc8, 0x20, 0x00, 0x00,	// Interface control
	0xf3,	2, 0x00, 0x00,				// Power sequence control
	0x11,	DELAY, 50,					// Wake
	0xf3,	2+DELAY, 0x00, 0x01, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x03, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x07, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x0f, 50,	// Power sequence control
	0xf4,	15+DELAY, 0x00, 0x04, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x07, 0x00, 0x3C, 0x36, 0x00, 0x3C, 0x36, 0x00, 50,	// Power control
	0xf3,	2+DELAY, 0x00, 0x1f, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x7f, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0xff, 50,	// Power sequence control
	0xfd,	11, 0x00, 0x00, 0x00, 0x17, 0x10, 0x00, 0x00, 0x01, 0x00, 0x16, 0x16,							// Analog parameter control
	0xf4,	15, 0x00, 0x09, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x07, 0x00, 0x3C, 0x36, 0x00, 0x3C, 0x36, 0x00,	// Power control
	0x36,	1, 0x08,					// Memory access data control
	0x35,	1, 0x00,					// Tearing effect line on
	0x3a,	1+DELAY, 0x05, 150,			// Interface pixel control
	0x29,	0,							// Display on
	0x2c,	0							// Memory write
  };
*/

static const uint8_t
  QDTech[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 csommands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    QDTech_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    QDTech_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    QDTech_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    QDTech_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    QDTech_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 },                  //     255 = 500 ms delay

  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    QDTech_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    QDTech_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    QDTech_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

  Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    QDTech_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    QDTech_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 },      //     XEND = 159
  Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    QDTech_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    QDTech_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F },           //     XEND = 159

  Rcmd2green144[] = {              // Init for 7735R, part 2 (green 1.44 tab)
    2,                        //  2 commands in list:
    QDTech_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    QDTech_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127

  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    QDTech_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay


// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
static void 
 qd_tech_commandList(const uint8_t *addr) {

  uint8_t  numCommands, numArgs;
  uint16_t ms;

  numCommands = *addr++;   // Number of commands to follow

  while(numCommands--) {                 // For each command...
	  qd_tech_writecommand(*addr++); //   Read, issue command
    numArgs  = *addr++;    //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
    	qd_tech_writedata(*addr++);  //     Read, issue argument
    }
    if(ms) {
      ms = *addr++; // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay(ms);
      //for(long i=0;i<40000*ms;i++);
    }
  }
}

// Initialization code for  QDTech displays
static void  
 qd_tech_commonInit(const uint8_t *cmdList) {
	colstart  = rowstart = 0; // May be overridden in init func
    
    P2DIR |= 0x01;
    P2DIR |= 0x02; 
    
    P2OUT |= 0x01;
    P2OUT |= 0x02;

    

    //gpio_output_set(BIT0, 0, BIT0, 0);					 //set DC HIGH
    
    //SPI.setClockDivider(SPI_CLOCK_DIV16);
    //UCB0BR0 = 0x01;
    //UCB0BR0 = 0x00;
    SPI.begin();

    if(cmdList) qd_tech_commandList(cmdList);
}

void  
 qd_tech_init() {
	_width    = QDTech_TFTWIDTH;
	_height   = QDTech_TFTHEIGHT;
	rotation  = 0;
	qd_tech_commonInit(0);
	qd_tech_commandList(QDTech);
}

void  
 qd_tech_initR(uint8_t options){

    qd_tech_commonInit(Rcmd1);

    if(options == INITR_GREENTAB) {
    	qd_tech_commandList(Rcmd2green);
    	colstart = 2;
    	rowstart = 1;
    } else if(options == INITR_144GREENTAB) {
    	_height = ST7735_TFTHEIGHT_144;
    	qd_tech_commandList(Rcmd2green144);
    	colstart = 2;
    	rowstart = 3;
    } else {
    // colstart, rowstart left at default '0' values
	  qd_tech_commandList(Rcmd2red);
    }

    qd_tech_commandList(Rcmd3);

  // if black, change MADCTL color filter
    if (options == INITR_BLACKTAB) {
    	qd_tech_writecommand(QDTech_MADCTL);
    	qd_tech_writedata(0xC0);
    }

  tabcolor = options;

}


void  
 qd_tech_setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

	qd_tech_writecommand(QDTech_CASET); // Column addr set
	qd_tech_writedata(0x00);
	qd_tech_writedata(x0+colstart);     // XSTART
	qd_tech_writedata(0x00);
	qd_tech_writedata(x1+colstart);     // XEND

	qd_tech_writecommand(QDTech_RASET); // Row addr set
	qd_tech_writedata(0x00);
	qd_tech_writedata(y0+rowstart);     // YSTART
	qd_tech_writedata(0x00);
	qd_tech_writedata(y1+rowstart);     // YEND

	qd_tech_writecommand(QDTech_RAMWR); // write to RAM
}

void  
 qd_tech_pushColor(uint16_t color) {
	P2OUT |= 0x02; 								//DC high

	qd_tech_spiwrite(color >> 8);
	qd_tech_spiwrite(color);
 }

void  
 qd_tech_drawPixel(int16_t x, int16_t y, uint16_t color) {
	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

	qd_tech_setAddrWindow(x,y,x+1,y+1);

	P2OUT |= 0x02; 								//DC high

	qd_tech_spiwrite(color >> 8);
	qd_tech_spiwrite(color);
}

void  
 qd_tech_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {

	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;
	if((y+h-1) >= _height) h = _height-y;
	qd_tech_setAddrWindow(x, y, x, y+h-1);

  	uint8_t hi = color >> 8, lo = color;
	P2OUT |= 0x02; 								//DC high
	while (h--) {
		qd_tech_spiwrite(hi);
		qd_tech_spiwrite(lo);
	}
}

void  
 qd_tech_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {

	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;
	if((x+w-1) >= _width)  w = _width-x;
	qd_tech_setAddrWindow(x, y, x+w-1, y);

	uint8_t hi = color >> 8, lo = color;
	P2OUT |= 0x02; 								//DC high
	while (w--) {
		qd_tech_spiwrite(hi);
		qd_tech_spiwrite(lo);
	}
}



void  
 qd_tech_fillScreen(uint16_t color) {
	qd_tech_fillRect(0, 0,  _width, _height, color);
}

// fill a rectangle
void  
 qd_tech_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

	// rudimentary clipping (qd_tech_drawChar w/big text requires this)
	if((x >= _width) || (y >= _height)) return;
	if((x + w - 1) >= _width)  w = _width  - x;
	if((y + h - 1) >= _height) h = _height - y;

	qd_tech_setAddrWindow(x, y, x+w-1, y+h-1);

	uint8_t hi = color >> 8, lo = color;
	P2OUT |= 0x02; 								//DC high
	for(y=h; y>0; y--) {
		for(x=w; x>0; x--) {
			qd_tech_spiwrite(hi);
			qd_tech_spiwrite(lo);
		}
	}
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t  
 qd_tech_Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void  
 qd_tech_setRotation(uint8_t m) {
// Generally 0 - Portrait 1 - Landscape

	qd_tech_writecommand(QDTech_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
	   qd_tech_writedata(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
     _width  = QDTech_TFTWIDTH;
     _height = QDTech_TFTHEIGHT;
     break;
   case 1:
	   qd_tech_writedata(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
     _width  = QDTech_TFTHEIGHT;
     _height = QDTech_TFTWIDTH;
     break;
  case 2:
	  qd_tech_writedata(MADCTL_BGR);
     _width  = QDTech_TFTWIDTH;
     _height = QDTech_TFTHEIGHT;
    break;
   case 3:
//     writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
	   qd_tech_writedata(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
     _width  = QDTech_TFTHEIGHT;
     _height = QDTech_TFTWIDTH;
     break;
  }
}

void  
 qd_tech_invertDisplay(BOOL i) {
	qd_tech_writecommand(i ? QDTech_INVON : QDTech_INVOFF);
}

/*
 * Adafruit GFX
 */

void  
 qd_tech_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  qd_tech_drawPixel(x0  , y0+r, color);
  qd_tech_drawPixel(x0  , y0-r, color);
  qd_tech_drawPixel(x0+r, y0  , color);
  qd_tech_drawPixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    qd_tech_drawPixel(x0 + x, y0 + y, color);
    qd_tech_drawPixel(x0 - x, y0 + y, color);
    qd_tech_drawPixel(x0 + x, y0 - y, color);
    qd_tech_drawPixel(x0 - x, y0 - y, color);
    qd_tech_drawPixel(x0 + y, y0 + x, color);
    qd_tech_drawPixel(x0 - y, y0 + x, color);
    qd_tech_drawPixel(x0 + y, y0 - x, color);
    qd_tech_drawPixel(x0 - y, y0 - x, color);
  }
}

void  
 qd_tech_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      qd_tech_drawPixel(x0 + x, y0 + y, color);
      qd_tech_drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      qd_tech_drawPixel(x0 + x, y0 - y, color);
      qd_tech_drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      qd_tech_drawPixel(x0 - y, y0 + x, color);
      qd_tech_drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      qd_tech_drawPixel(x0 - y, y0 - x, color);
      qd_tech_drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void  
 qd_tech_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  qd_tech_drawFastVLine(x0, y0-r, 2*r+1, color);
  qd_tech_fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void  
 qd_tech_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      qd_tech_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      qd_tech_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      qd_tech_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      qd_tech_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}
void  
 qd_tech_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
   int16_t ay = (y1-y0) > 0 ? y1-y0 : y0 - y1;
   int16_t ax = (x1-x0) > 0 ? x1-x0 : x0 - x1;
  //int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  int16_t steep = ay > ax;
  if (steep) {
    swap(&x0, &y0);
    swap(&x1, &y1);
  }

  if (x0 > x1) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = (y1-y0) > 0 ? y1-y0 : y0 - y1;
  //dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      qd_tech_drawPixel(y0, x0, color);
    } else {
    	qd_tech_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// Draw a rectangle
void  
 qd_tech_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  qd_tech_drawFastHLine(x, y, w, color);
  qd_tech_drawFastHLine(x, y+h-1, w, color);
  qd_tech_drawFastVLine(x, y, h, color);
  qd_tech_drawFastVLine(x+w-1, y, h, color);
}

void  
 qd_tech_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  qd_tech_drawFastHLine(x+r  , y    , w-2*r, color); // Top
  qd_tech_drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  qd_tech_drawFastVLine(x    , y+r  , h-2*r, color); // Left
  qd_tech_drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  qd_tech_drawCircleHelper(x+r    , y+r    , r, 1, color);
  qd_tech_drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  qd_tech_drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  qd_tech_drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void  
 qd_tech_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  qd_tech_fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  qd_tech_fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  qd_tech_fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}
// Draw a triangle
void  
 qd_tech_drawTriangle(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color) {
  qd_tech_drawLine(x0, y0, x1, y1, color);
  qd_tech_drawLine(x1, y1, x2, y2, color);
  qd_tech_drawLine(x2, y2, x0, y0, color);
}
// Fill a triangle
void  
 qd_tech_fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(&y0, &y1); swap(&x0, &x1);
  }
  if (y1 > y2) {
    swap(&y2, &y1); swap(&x2, &x1);
  }
  if (y0 > y1) {
    swap(&y0, &y1); swap(&x0, &x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    qd_tech_drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(&a,&b);
    qd_tech_drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(&a,&b);
    qd_tech_drawFastHLine(a, y, b-a+1, color);
  }
}

void  
 qd_tech_drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
    	uint8_t *p = (bitmap + j * byteWidth + i / 8);
    	if(*p & (128 >> (i & 7))) {
    		qd_tech_drawPixel(x+i, y+j, color);
     	}
    }
  }
}


void  
 qd_tech_write(uint8_t c) {
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    qd_tech_drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_x > (_width - textsize*6))) {
      cursor_y += textsize*8;
      cursor_x = 0;
    }
  }
}

void  
 qd_tech_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {

  if((x >= _width)            || // Clip right
     (y >= _height)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;
  int8_t i;
  for ( i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5)
      line = 0x0;
    else{
    	uint8_t *p = (font+(c*5)+i);
    	line = *p;}
    int8_t j;
    for ( j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
        	qd_tech_drawPixel(x+i, y+j, color);
        else {  // big size
        	qd_tech_fillRect(x+(i*size), y+(j*size), size, size, color);
        }
      } else if (bg != color) {
        if (size == 1) // default size
        	qd_tech_drawPixel(x+i, y+j, bg);
        else {  // big size
          qd_tech_fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void  
 qd_tech_setCursor(int16_t x, int16_t y) {
  cursor_x = x;
  cursor_y = y;
}

void  
 qd_tech_setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}

void  
 qd_tech_setTextColor(uint16_t c) {
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}
/*
void  
 qd_tech_setTextColor(uint16_t c, uint16_t b) {
  textcolor   = c;
  textbgcolor = b;
}
*/

void  
 qd_tech_setTextWrap(BOOL w) {
	wrap = w;
}

uint8_t  
 qd_tech_getRotation(void) {
	return rotation;
}
/*
void  
 qd_tech_setRotation(uint8_t x) {
  rotation = (x & 3);
  switch(rotation) {
   case 0:
   case 2:
    _width  = WIDTH;
    _height = HEIGHT;
    break;
   case 1:
   case 3:
    _width  = HEIGHT;
    _height = WIDTH;
    break;
  }
}
*/

// Return the size of the display (per current rotation)
int16_t   
 qd_tech_width(void) {
  return _width;
}

int16_t  
 qd_tech_height(void) {
  return _height;
}

void  
 qd_tech_print(char c[]){
	uint8_t i;
	for(i=0;i<sizeof(c);i++)
		qd_tech_write(c[i]);
}

void  
 qd_tech_println(char c[]){
	uint8_t x = cursor_x;
	qd_tech_print(c);
	qd_tech_setCursor(x,cursor_y+textsize*8 + textsize);
}
