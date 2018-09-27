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

#define   FONT_ONE_HEIGHT               8                                   // font one height in pixels
#define   FONT_TWO_HEIGHT               12                                  // font two height in pixels

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Variables.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

char      chBuffer[128];   // general purpose character buffer
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
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main loop.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void  loop()
{
  u8g2.clearBuffer();

  if (screen >= 2) {
    screen = 0;
  } else {
    screen = screen + 1;
  }

  switch (screen) {
    case 0:
      //Main screen
      u8g2.setFont(u8g2_font_6x10_tr);
      cadena = "Mon, 24 Sep 2018";
      cadena.toCharArray(chBuffer, cadena.length() + 1);
      u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), 2, chBuffer);

      cadena = "15:38:58";
      cadena.toCharArray(chBuffer, cadena.length() + 1);
      u8g2.setFont(u8g2_font_fub11_tn);
      u8g2.drawRFrame(2, FONT_TWO_HEIGHT + FONT_ONE_HEIGHT + 15, 123, 3, 0);
      u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), FONT_TWO_HEIGHT + 2, chBuffer);
      u8g2.sendBuffer();
      break;

    case 1:
      cadena = "Servicio #1";
      cadena.toCharArray(chBuffer, cadena.length() + 1);
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.drawRFrame(0, 0, 124, FONT_TWO_HEIGHT, 0);
      u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), 1, chBuffer);
      u8g2.sendBuffer();
      break;

    case 2:
      cadena = "Servicio #2";
      cadena.toCharArray(chBuffer, cadena.length() + 1);
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.drawRFrame(0, 0, 124, FONT_TWO_HEIGHT, 0);
      u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), 1, chBuffer);
      u8g2.sendBuffer();
      break;
  }

  delay(2000);
}



void titulo(String mensaje) {
  mensaje.toCharArray(chBuffer, cadena.length() + 1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawRFrame(0, 0, 124, FONT_TWO_HEIGHT, 0);
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), 1, chBuffer);
  u8g2.sendBuffer();
}

