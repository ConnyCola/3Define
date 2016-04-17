#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <EEPROM.h>

#define TFT_CS     9
#define TFT_RST    7  // you can also connect this to the Arduino reset
#define TFT_DC     8

//#define DEBUG

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


typedef struct {
  byte x;
  byte y;
  byte c1;
  byte c2;
} pix;


void testdrawrects(uint16_t color);
void draw_pix(pix p);
pix read_pix();

void setup(void) {
  Serial.begin(57600);
  //Serial.begin(9600);

  //Serial.println("Hello! ST7735 TFT Test");

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(0);
  tft.fillScreen(ST7735_BLACK);


#define MODE 0
#if MODE == 1
  //mid line
  tft.drawLine(20,64,80,64,ST7735_BLUE);
  tft.drawLine(20,64+1,80,64+1,ST7735_BLUE);

  //left line line
  tft.drawLine(27,30,73,30,ST7735_GREEN);
  tft.drawLine(27,30+5,73,30+5,ST7735_GREEN);

  //right line line
  tft.drawLine(16,90,84,90,ST7735_GREEN);
  tft.drawLine(16,90+9,84,90+9,ST7735_GREEN);
#endif

#if MODE == 2
  tft.fillScreen(ST7735_BLACK);
  for(int i=0; i<128; i+=2){
    //tft.drawLine(0,i, 128, i, ST7735_GREEN);
    //tft.drawLine(0,i+1, 128, i+1, ST7735_BLUE); 
    tft.drawLine(i, 0,  i, 160, ST7735_GREEN);
    tft.drawLine(i+1, 0, i+1, 160, ST7735_BLUE);
  }
#endif

/*
  for(int i=40;i<45;i++){
    pix p;
    p.x = i;
    p.y = i;
    p.c1 = 0xFF & ST7735_BLUE;
    p.c2 = 0xFF & ST7735_BLUE>>8;
    draw_pix(p);
  }
  */
  //  tft.drawPixel(i,i,0x1F);
}

void loop() {
  
  //tft.drawPixel(inByte, int16_t inByte, uint16_t color);

  pix p = read_pix();
  draw_pix(p);

}

pix read_pix(){
  while(Serial.available() < 4);
  pix p;
  byte b[4];

  for(int i=0; i<4; i++){
    byte inByte = Serial.read();
    //Serial.print("DATA in: "); Serial.println(inByte, DEC);
    b[i] = inByte;
  }
  p.x = b[0];
  p.y = b[1];
  p.c1 = b[2];
  p.c2 = b[3];
  #ifdef DEBUG
  Serial.print("\r\nDATA1:");
  Serial.print("  x: "); Serial.print(p.x, DEC);
  Serial.print("  y: "); Serial.print(p.y, DEC);
  Serial.print("  c1: "); Serial.print(p.c1, DEC);
  Serial.print("  c2: "); Serial.print(p.c2, DEC);
  #endif

  Serial.print(0xFF);
  return p;

}

void draw_pix(pix p){
  #ifdef DEBUG
  Serial.println("\r\nDATA:");
  Serial.print("x: "); Serial.println(p.x, DEC);
  Serial.print("y: "); Serial.println(p.y, DEC);
  Serial.print("c1: "); Serial.println(p.c1, DEC);
  Serial.print("c2: "); Serial.println(p.c2, DEC);
  Serial.print("col: 0x"); Serial.print(p.c2<<8 | p.c1, HEX);
  #endif
  tft.drawPixel(p.x, p.y, (p.c2<<8) | p.c1);
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  int16_t x;
  for (x=0; x < 128; x+=6) {
    tft.drawRect(128/2 -x/2, 160/2 -x/2 , x, x, color);
  }
}