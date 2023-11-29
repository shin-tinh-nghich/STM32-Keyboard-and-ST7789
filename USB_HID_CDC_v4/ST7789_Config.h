#define TFT_DC    PB11
#define TFT_RST   PB10
#define SCR_WD   240
#define SCR_HT   240   // 320 - to allow access to full 240x320 frame buffer
#define SCR_SYS  80

#include <SPI.h>
#include <Adafruit_GFX.h>
#include "Arduino_ST7789_STM.h"

//#include "my_bmp.h"

Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);

#include "RREFont.h"
#include "rre_chicago_20x24.h"
#include <string.h>

RREFont font;

// needed for RREFont library initialization, define your fillRect
void customRect(int x, int y, int w, int h, int c) {
  return lcd.fillRect(x, y, w, h, c);
}

#define BUF_SIZE  50
uint8_t newMessage[BUF_SIZE];
bool newMessageAvailable = false;

void setupTFT(void)
{
  lcd.init(SCR_WD, SCR_HT);
  font.init(customRect, SCR_WD, SCR_HT); // custom fillRect function and screen width and height values

  font.setFont(&rre_chicago_20x24);
  font.setScale(1, 1); font.setSpacing(3);
  //font.setDigitMinWd(16);

  font.setColor(WHITE);
  lcd.fillScreen(RGBto565(0, 0, 0));
  font.printStr(0, 200, "camos READ");
//  int i,j;
//  for(j=0;j<190;j++) 
//    for(i=0;i<170;i++)
//      lcd.drawImageF(i,j+51,170,190,img);
  
  newMessage[0] = '\0';

  lcd.drawFastHLine(0, 50, SCR_HT, WHITE);
  lcd.drawFastHLine(SCR_WD - SCR_SYS + 10, 118, 60, WHITE);   //24*2+5*2+10+50+1 = 118
  lcd.drawFastHLine(SCR_WD - SCR_SYS + 10, 188, 60, WHITE);   //24*2+5*2+10+118+1 = 188
  lcd.drawFastVLine(SCR_WD - SCR_SYS, 50, SCR_HT - 50, WHITE);
  font.setColor(YELLOW);
  font.printStr((SCR_WD - SCR_SYS) + (SCR_SYS - font.strWidth("CPU")) / 2 , 50 + 5, "CPU");
  font.printStr((SCR_WD - SCR_SYS) + (SCR_SYS - font.strWidth("RAM")) / 2, 118 + 5, "RAM");
}

void readSerial(void)
{
  static uint8_t  putIndex = 0;
  while (CompositeSerial.available())
  {
    newMessage[putIndex] = (char)CompositeSerial.read();
    if ((newMessage[putIndex] == '\n') || (putIndex >= BUF_SIZE - 3)) // end of message character or full buffer
    {
      newMessage[putIndex] = '\0';
      // restart the index for next filling spree and flag we have a message waiting
      putIndex = 0;
      newMessageAvailable = true;
    } else if (newMessage[putIndex] != '\r')
      // Just save the next char in next location
      putIndex++;
      
    vTaskDelay(1);
  }
}

// messager format: "xxxx value"
// xxxx: time, date, sysI, edit, conf, weth, modL
// value: 12:15:59, 10/11/2023, cpu15%, buttonA-0x16, 5

// TFT task
static void tft_Task(void *pvParameters) {
  for (;;) {
    readSerial();
    if (newMessageAvailable)  // there is a new message waiting
    {
      CompositeSerial.println((char* )newMessage);

      uint8_t keyMess[5], valueMess[BUF_SIZE - 5];
      for (int i = 0; i < 4; i++)
        keyMess[i] = newMessage[i];
      keyMess[4] = '\0';

      int j = 0;
      for (int i = 5; i < sizeof(newMessage); i++)
      {
        valueMess[j] = newMessage[i];
        j++;
      }
      CompositeSerial.println((char* )keyMess);
      CompositeSerial.println((char* )valueMess);

      vTaskDelay(1);
      if (strcmp((char* )keyMess, "time") == 0) {          // if received time
        lcd.fillRect(3, 0, 80, 24, BLACK);
        font.setColor(WHITE);
        font.printStr(3, 0, (char* )valueMess);
        newMessageAvailable = false;
      } else if (strcmp((char* )keyMess, "date") == 0) {   // if received date
        lcd.fillRect(3, 25, font.strWidth((char* )valueMess), 24, BLACK);
        font.setColor(WHITE);
        font.printStr(3, 25, (char* )valueMess);
        newMessageAvailable = false;
      } else if (strcmp((char* )keyMess, "weth") == 0) {   // if received weather
        // data value: city-temp-humi-Precipitation-Description-Wind
        int j = 0;
        uint8_t city[10], temp[5], humi[5];
        uint8_t count = 0, pos[3] = {0};
        for (int i = 0; i < sizeof(valueMess); i++) {
          if (count == 3) break;
          if (valueMess[i] == '-') {
            pos[count] = i;
            count++;
          }
        }
        for (int i = 0; i < pos[0]; i++) {
          city[i] = valueMess[i];
          j++;
        }
        city[j] = '\0';
        j = 0;
        for (int i = pos[0] + 1; i < pos[1]; i++) {
          temp[j] = valueMess[i];
          j++;
        }
        temp[j] = '\0';
        j = 0;
        for (int i = pos[1] + 1; i < pos[2]; i++) {
          humi[j] = valueMess[i];
          j++;
        }
        humi[j] = '\0';

        lcd.fillRect(82, 0, font.strWidth((char* )city), 24, BLACK);
        font.setColor(YELLOW);
        font.printStr(82, 0, (char* )city);
        font.setColor(BLUE);
        lcd.fillRect((240 - 55), 0, 55, 24, BLACK);
        font.printStr(ALIGN_RIGHT , 0, (char* )temp);
        lcd.fillRect((240 - 55), 25, 55, 24, BLACK);
        font.printStr(ALIGN_RIGHT , 25, (char* )humi);
        newMessageAvailable = false;
      } else if (strcmp((char* )keyMess, "sysI") == 0) {   // if received system imfomation
        // data value: cpu-ram-
        int j = 0;
        uint8_t cpu[5], ram[5];
        uint8_t count = 0, pos[2] = {0};
        for (int i = 0; i < sizeof(valueMess); i++) {
          if (count == 2) break;
          if (valueMess[i] == '-') {
            pos[count] = i;
            count++;
          }
        }
        for (int i = 0; i < pos[0]; i++) {
          cpu[i] = valueMess[i];
          j++;
        }
        cpu[j] = '\0';
        j = 0;
        for (int i = pos[0] + 1; i < pos[1]; i++) {
          ram[j] = valueMess[i];
          j++;
        }
        ram[j] = '\0';

        font.setColor(YELLOW);
        lcd.fillRect(240 - SCR_SYS + 1, 50 + 10 + 24 + 5, SCR_SYS - 1, 24, BLACK);
        font.printStr(ALIGN_RIGHT, 50 + 10 + 24 + 5, (char* )cpu);
        lcd.fillRect(240 - SCR_SYS + 1, 118 + 24 + 10 + 5, SCR_SYS - 1, 24, BLACK);
        font.printStr(ALIGN_RIGHT, 118 + 24 + 10 + 5, (char* )ram);
        newMessageAvailable = false;
      } else if (strcmp((char* )keyMess, "modL") == 0) {   // if received mode_led
        modeStrip (atoi((char*) valueMess));
        newMessageAvailable = false;
      } else if (strcmp((char* )keyMess, "edit") == 0) {   // if received edit keyboard
        // data value: numberKey-function-key1-key2-key3-
        int j = 0;
        uint8_t numberKey[2], function[2], key1[3], key2[3], key3[3];
        uint8_t count = 0, pos[5] = {0};
        for (int i = 0; i < sizeof(valueMess); i++) {
          if (count == 5) break;
          if (valueMess[i] == '-') {
            pos[count] = i;
            count++;
          }
        }
        for (int i = 0; i < pos[0]; i++) {
          numberKey[i] = valueMess[i];
          j++;
        }
        numberKey[j] = '\0';
        j = 0;
        for (int i = pos[0] + 1; i < pos[1]; i++) {
          function[j] = valueMess[i];
          j++;
        }
        function[j] = '\0';
        j = 0;
        for (int i = pos[1] + 1; i < pos[2]; i++) {
          key1[j] = valueMess[i];
          j++;
        }
        key1[j] = '\0';
        j = 0;
        for (int i = pos[2] + 1; i < pos[3]; i++) {
          key2[j] = valueMess[i];
          j++;
        }
        key2[j] = '\0';
        j = 0;
        for (int i = pos[3] + 1; i < pos[4]; i++) {
          key3[j] = valueMess[i];
          j++;
        }
        key3[j] = '\0';

        uint8_t _numberKey = atoi((char* )numberKey);
        uint8_t _function = atoi((char* )function);
        uint8_t _key1 = atoi((char* )key1);
        uint8_t _key2 = atoi((char* )key2);
        uint8_t _key3 = atoi((char* )key3);
        writeKeyEEPROM(_numberKey, (uint16_t)_function, (uint16_t)_key1, (uint16_t)_key2, (uint16_t)_key3);
        readKeyEEprom(_numberKey);
        check_arr();
        newMessageAvailable = false;
      } else {
        check_arr();
        newMessageAvailable = false;
      }
    }
    vTaskDelay(1);
  }
}
