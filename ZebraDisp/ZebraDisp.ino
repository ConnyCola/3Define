
#include <SPI.h>
#include <msp432.h>
#include "qd_tech.h"


int chipSelectPin = 32;

void testlines(uint16_t color);


int delay_ms(int ms){
  long k=0;
  for(long i=0;i<40000*ms;i++){
    k++;
  }
  return k;
}

void setup()
{
  // put your setup code here, to run once:
  //SPI.begin();
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  P1DIR |= 0x01;
  P2DIR |= 0x01;
  P2DIR |= 0x02; 

    CSKEY = CSKEY_VAL;                         // Unlock CS module for register access
    CSCTL0 = 0;                            // Reset tuning parameters
    CSCTL0 = DCORSEL_5;           // Set DCO to 48MHz
    /* Select MCLK = DCO, no divider */
    CSCTL1 = CSCTL1 & ~(SELM_M | DIVM_M) | SELM_3;
    CSKEY = 0;                             // Lock CS module from unintended accesses

  qd_tech_initR(INITR_BLACKTAB);
  qd_tech_setRotation(0);

  qd_tech_fillScreen(QDTech_WHITE);
}

void loop()
{
  static int i=0;
  // put your main code here, to run repeatedly:

  //qd_tech_fillScreen(QDTech_GREEN);
  P1OUT |= 0x01;
  delay(500);
  
  for(int i=0; i+=2; i<160){
    qd_tech_drawLine(0,i , 128, i, QDTech_BLACK); 
  }
  



}

void testlines(uint16_t color) {
  qd_tech_fillScreen(QDTech_BLACK);
  int16_t x;
  int16_t y;
  for (x=0; x < 128; x+=6) {
    qd_tech_drawLine(0, 0, x, 160-1, color);
  }
  for (y=0; y < 160; y+=6) {
    qd_tech_drawLine(0, 0, 128-1, y, color);
  }

  qd_tech_fillScreen(QDTech_BLACK);
  for (x=0; x < 128; x+=6) {
    qd_tech_drawLine(128-1, 0, x, 160-1, color);
  }
  for (y=0; y < 160; y+=6) {
    qd_tech_drawLine(128-1, 0, 0, y, color);
  }

  qd_tech_fillScreen(QDTech_BLACK);
  for (x=0; x < 128; x+=6) {
    qd_tech_drawLine(0, 160-1, x, 0, color);
  }
  for (y=0; y < 160; y+=6) {
    qd_tech_drawLine(0, 160-1, 128-1, y, color);
  }

  qd_tech_fillScreen(QDTech_BLACK);
  for (x=0; x < 128; x+=6) {
    qd_tech_drawLine(128-1, 160-1, x, 0, color);
  }
  for (y=0; y < 160; y+=6) {
    qd_tech_drawLine(128-1, 160-1, 0, y, color);
  }
}


void os_delay_ms(uint16_t ms){
  delay(ms);
}


