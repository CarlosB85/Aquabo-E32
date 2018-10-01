#include "Arduino.h"
#ifndef Aquabo_E32_Screens
#define Aquabo_E32_Screens

class E32_Screens{
public:
E32_Screens();
void titulo_1(String linea);
void titulo_2(String linea1, String linea2);
void E32_Screen_TEST();
void initializeScreens();
}

E32_Screens::E32_Screens(){
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include                              <U8g2lib.h>                           // see https://github.com/olikraus/u8g2/wiki/u8g2reference

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constants.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FONT_ONE_HEIGHT 12 // font one height in pixels
#define FONT_TWO_HEIGHT 18 // font two height in pixels
#define FONT_ONE_PADDING 1 // padding superior para font 1
#define FONT_TWO_PADDING 2 // padding superior para font 2

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Variables.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

char      chBufferVariable[128]; //buffer propio de los metodos titulo_1 y titulo_2
String    cadena = "";
U8G2_SSD1306_128X64_NONAME_F_HW_I2C       u8g2(U8G2_R0, 16, 15, 4);         // OLED graphics
static  int   screen = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Setup.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{

  // OLED graphics.
  initializeScrrens();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main loop.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void  E32_Screen_TEST()
{
  u8g2.clearBuffer();

  if (screen >= 3) {
    screen = 0;
  } else {
    screen = screen + 1;
  }

  switch (screen) {
    case 0:
      //Main screen
      titulo_2("Mon, 24 Sep 2018", "15:38:58");
      u8g2.sendBuffer();
      break;

    case 1:
      cadena = "Servicio #1";
      titulo_1(cadena);
      u8g2.sendBuffer();
      break;

    case 2:
      cadena = "Servicio #2";
      titulo_1(cadena);
      u8g2.sendBuffer();
      break;

    case 3:
      //testing the height of the fonts.
      cadena = "font 1";
      cadena.toCharArray(chBuffer, cadena.length() + 1);
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.drawRFrame(0, 0, 128, FONT_ONE_HEIGHT, 0);
      u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), FONT_ONE_PADDING, chBuffer);

      cadena = "font 2";
      cadena.toCharArray(chBuffer, cadena.length() + 1);
      u8g2.setFont(u8g2_font_fub11_tn);
      u8g2.drawRFrame(0, 25, 124, FONT_TWO_HEIGHT, 0);
      u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), (25 + FONT_TWO_PADDING), chBuffer);
      u8g2.sendBuffer();
      break;
  }

  delay(2000);
}

void initializeScrrens() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void titulo_2(String linea1, String linea2) {
  u8g2.setFont(u8g2_font_6x10_tr);
  linea1.toCharArray(chBufferVariable, cadena.length() + 1);
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBufferVariable) / 2), FONT_ONE_PADDING, chBufferVariable);

  linea2.toCharArray(chBufferVariable, cadena.length() + 1);
  u8g2.setFont(u8g2_font_fub11_tn);
  u8g2.drawRFrame(0, 0, 128, FONT_TWO_HEIGHT + FONT_ONE_HEIGHT, 0);
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBufferVariable) / 2), FONT_ONE_HEIGHT + (FONT_TWO_PADDING), chBufferVariable);
}

void titulo_1(String mensaje) {
  mensaje.toCharArray(chBufferVariable, cadena.length() + 1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawRFrame(0, 0, 128, FONT_ONE_HEIGHT, 0);
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBufferVariable) / 2), FONT_ONE_PADDING, chBufferVariable);
}

#endif
