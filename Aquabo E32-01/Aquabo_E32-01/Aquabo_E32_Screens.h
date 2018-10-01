/*
  Aquabo_E32_Screens.h - Library for the use of OLED screen.
  Created by Carlos Hernan Brugnoni C. , October 1, 2018.
  Released into the public domain.
*/

#ifndef Aquabo_E32_Screens
#define Aquabo_E32_Screens

#include "Arduino.h"
class E32_Screens {
  public:
    E32_Screens();
    void titulo_1(String linea);
    void titulo_2(String linea1, String linea2);
    void E32_Screen_TEST();
    void initializeScreens();
  private:
    char      chBufferVariable[128];
    String cadena;
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C       u8g2(U8G2_R0, 16, 15, 4);         // OLED graphics
    static  int   screen;
};
