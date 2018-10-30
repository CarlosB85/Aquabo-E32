//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                          //
//                                     Heltec WiFi Kit 32 NTP Clock                                         //
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include                              <time.h>                              // for time calculations
#include                              <WiFi.h>                              // for wifi
#include                              <WiFiUdp.h>                           // for udp via wifi
#include                              <U8g2lib.h>                           // see https://github.com/olikraus/u8g2/wiki/u8g2reference

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constants.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//// WIFI ////
#define   NTP_DELAY_COUNT               20                                  // delay count for ntp update
#define   NTP_PACKET_LENGTH             48                                  // ntp packet length
#define   TIME_ZONE                     (-5)                                // offset from utc
#define   UDP_PORT                      4000                                // UDP listen port
//// SCREENS ////
#define FONT_ONE_HEIGHT 12 // font one height in pixels
#define FONT_TWO_HEIGHT 18 // font two height in pixels
#define FONT_ONE_PADDING 1 // padding superior para font 1
#define FONT_TWO_PADDING 2 // padding superior para font 2
//// INPUTS ////
#define ANALOG_PIN_0 36
#define TOUCH_1 13
#define TOUCH_2 12
#define TOUCH_3 14
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Variables.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// TIMT ////
char chBuffer[128];
char chBuffer2[128];
//// WIFI ////
char      chPassword[] =                  "M0b1u$85";               // your network password
char      chSSID[] =                      "Mobius_Hogar";                   // your network SSID
bool      bTimeReceived =                 false;                            // time has not been received
int       nWifiStatus =                   WL_IDLE_STATUS;                   // wifi status
WiFiUDP   Udp;
//// SCREENS ////
char      chBufferVariable[128]; //buffer propio de los metodos titulo_1 y titulo_2
String    cadena = "";
U8G2_SSD1306_128X64_NONAME_F_HW_I2C       u8g2(U8G2_R0, 16, 15, 4);         // OLED graphics
static  int   screen = 0;
bool menu_avb = true;
//// INPUTS ////
int analog_value = 0;
int mapeado = 0;
int touch_value_1 = 100;
int touch_value_2 = 100;
int touch_value_3 = 100;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Setup.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  // Serial.

  Serial.begin(115200);
  while (!Serial)
  {
    Serial.print('.');
  }

  // Connect to wifi.

  Serial.print("NTP clock: connecting to wifi");
  WiFi.begin(chSSID, chPassword);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  sprintf(chBuffer, "NTP clock: WiFi connected to %s.", chSSID);
  Serial.println(chBuffer);

  //// SCREENS ////
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  // Clean the display buffer.
  u8g2.clearBuffer();

  // Display the title.
  sprintf(chBuffer, "%s", "WiFi Stats:");
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBuffer) / 2), 0, chBuffer);

  // Display the ip address assigned by the wifi router.
  char  chIp[81];
  WiFi.localIP().toString().toCharArray(chIp, sizeof(chIp) - 1);
  sprintf(chBuffer, "IP  : %s", chIp);
  u8g2.drawStr(0, FONT_ONE_HEIGHT * 2, chBuffer);

  // Display the ssid of the wifi router.
  sprintf(chBuffer, "SSID: %s", chSSID);
  u8g2.drawStr(0, FONT_ONE_HEIGHT * 3, chBuffer);

  // Display the rssi.
  sprintf(chBuffer, "RSSI: %d", WiFi.RSSI());
  u8g2.drawStr(0, FONT_ONE_HEIGHT * 4, chBuffer);

  // Display waiting for ntp message.
  u8g2.drawStr(0, FONT_ONE_HEIGHT * 6, "Awaiting NTP time...");

  // Now send the display buffer to the OLED.
  u8g2.sendBuffer();

  // Udp.
  Udp.begin(UDP_PORT);
}

//// SCREENS METHODS ////

void titulo_2(String linea1, String linea2) {
  u8g2.setFont(u8g2_font_6x10_tr);
  linea1.toCharArray(chBufferVariable, linea1.length() + 1);
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBufferVariable) / 2), FONT_ONE_PADDING, chBufferVariable);

  linea2.toCharArray(chBufferVariable, linea2.length() + 1);
  u8g2.setFont(u8g2_font_fub11_tn);
  u8g2.drawRFrame(0, 0, 128, FONT_TWO_HEIGHT + FONT_ONE_HEIGHT, 0);
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBufferVariable) / 2), FONT_ONE_HEIGHT + (FONT_TWO_PADDING), chBufferVariable);
}

void titulo_1(String mensaje) {
  mensaje.toCharArray(chBufferVariable, mensaje.length() + 1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawRFrame(0, 0, 128, FONT_ONE_HEIGHT, 0);
  u8g2.drawStr(64 - (u8g2.getStrWidth(chBufferVariable) / 2), FONT_ONE_PADDING, chBufferVariable);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Main loop.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void  loop()
{

  // Local variables.
  static  int   nNtpDelay = NTP_DELAY_COUNT;
  static  byte  chNtpPacket[NTP_PACKET_LENGTH];

  // Check for time to send ntp request.
  if (bTimeReceived == false)
  {
    // Have yet to receive time from the ntp server, update delay counter.
    nNtpDelay += 1;

    // Check for time to send ntp request.
    if (nNtpDelay >= NTP_DELAY_COUNT)
    {

      // Time to send ntp request, reset delay.
      nNtpDelay = 0;

      // Send ntp time request.
      // Initialize ntp packet.
      // Zero out chNtpPacket.

      memset(chNtpPacket, 0, NTP_PACKET_LENGTH);

      // Set the ll (leap indicator), vvv (version number) and mmm (mode) bits.
      //
      //  These bits are contained in the first byte of chNtpPacker and are in
      // the following format:  llvvvmmm
      //
      // where:
      //
      //    ll  (leap indicator) = 0
      //    vvv (version number) = 3
      //    mmm (mode)           = 3

      chNtpPacket[0]  = 0b00011011;

      // Send the ntp packet.
      IPAddress ipNtpServer(129, 6, 15, 28);
      Udp.beginPacket(ipNtpServer, 123);
      Udp.write(chNtpPacket, NTP_PACKET_LENGTH);
      Udp.endPacket();

      Serial.println("NTP clock: ntp packet sent to ntp server.");
      Serial.print("NTP clock: awaiting response from ntp server");
    }

    Serial.print(".");

    // Check for time to check for server response.
    if (nNtpDelay == (NTP_DELAY_COUNT - 1))
    {

      // Time to check for a server response.
      if (Udp.parsePacket())
      {

        // Server responded, read the packet.
        Udp.read(chNtpPacket, NTP_PACKET_LENGTH);

        // Obtain the time from the packet, convert to Unix time, and adjust for the time zone.
        struct  timeval tvTimeValue = {0, 0};

        tvTimeValue.tv_sec = ((unsigned long)chNtpPacket[40] << 24) +       // bits 24 through 31 of ntp time
                             ((unsigned long)chNtpPacket[41] << 16) +       // bits 16 through 23 of ntp time
                             ((unsigned long)chNtpPacket[42] <<  8) +       // bits  8 through 15 of ntp time
                             ((unsigned long)chNtpPacket[43]) -             // bits  0 through  7 of ntp time
                             (((70UL * 365UL) + 17UL) * 86400UL) +          // ntp to unix conversion
                             (TIME_ZONE * 3600UL) +                        // time zone adjustment
                             (5);                                           // transport delay fudge factor

        // Set the ESP32 rtc.
        settimeofday(& tvTimeValue, NULL);

        // Time has been received.
        bTimeReceived = true;

        // Output date and time to serial.
        struct tm * tmPointer = localtime(& tvTimeValue.tv_sec);
        strftime (chBuffer, sizeof(chBuffer), "%a, %d %b %Y %H:%M:%S",  tmPointer);
        Serial.println();
        Serial.print("NTP clock: response received, time written to ESP32 rtc: ");
        Serial.println(chBuffer);

        // No longer need wifi.
        WiFi.mode(WIFI_OFF);
      }
      else
      {

        // Server did not respond.
        Serial.println("NTP clock: packet not received.");
      }
    }
  }

  // Update OLED.
  if (bTimeReceived)
  {
    analog_value = analogRead(ANALOG_PIN_0);
    mapeado = map(analog_value, 0, 4095, 0, 4);

    u8g2.clearBuffer();

    struct  timeval tvTimeValue;
    gettimeofday(& tvTimeValue, NULL);
    struct tm * tmPointer = localtime(& tvTimeValue.tv_sec);
    strftime(chBuffer, sizeof(chBuffer), "%a, %d %b %Y",  tmPointer);
    strftime(chBuffer2, sizeof(chBuffer2), "%I:%M:%S",  tmPointer);
    titulo_2(String(chBuffer), String(chBuffer2));
    u8g2.drawStr(0, 35, "S");//servicio
    u8g2.drawStr(32, 35, "M");//manual
    u8g2.drawStr(64, 35, "C");//configuracion
    u8g2.drawStr(96, 35, "E");//estado
    
    if (menu_avb) {
      switch (mapeado) {
        case 0://main
          u8g2.drawStr(
          break;

        case 1://Servicio

          break;

        case 2://control manual

          break;
      }
    }

    u8g2.sendBuffer();

    //barra segun posicion del potenciometro en pin 36
    u8g2.drawRFrame(0, 35, 128, 3, 0);

    u8g2.drawLine(0, 36, mapeado, 36);

    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(0, 40, "POT:");
    String(analog_value).toCharArray(chBuffer, sizeof(chBuffer));
    u8g2.drawStr(50, 40, chBuffer);
    u8g2.drawStr(0, 50, "MAP :");
    String(mapeado).toCharArray(chBuffer2, sizeof(chBuffer2));
    u8g2.drawStr(50, 50, chBuffer2);
    // Send the display buffer to the OLED

    u8g2.drawStr(80, 40, "TOUCH:");
    touch_value_1 = touchRead(TOUCH_1);
    touch_value_2 = touchRead(TOUCH_2);
    touch_value_3 = touchRead(TOUCH_3);
    if (touch_value_1 < 50) {
      String("S").toCharArray(chBuffer, sizeof(chBuffer));
    } else {
      String("N").toCharArray(chBuffer, sizeof(chBuffer));
    }
    u8g2.drawStr(80, 50, chBuffer);
    if (touch_value_2 < 50) {
      String("S").toCharArray(chBuffer, sizeof(chBuffer));
    } else {
      String("N").toCharArray(chBuffer, sizeof(chBuffer));
    }
    u8g2.drawStr(90, 50, chBuffer);
    if (touch_value_3 < 50) {
      String("S").toCharArray(chBuffer, sizeof(chBuffer));
    } else {
      String("N").toCharArray(chBuffer, sizeof(chBuffer));
    }
    u8g2.drawStr(100, 50, chBuffer);
    u8g2.sendBuffer();
  }

  // Give up some time.
  delay(200);
}
