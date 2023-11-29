#include <WS2812B.h>
//#include <SPI.h>

#define NUM_LEDS 8

uint8_t mode_led = 5;
uint8_t brightness = 51;

const uint16_t stripModeAdd = functionKey_AddW;
const uint16_t stripBrightnessAdd = key1_AddW;

WS2812B strip = WS2812B(NUM_LEDS);
// Note. Gamma is not really supported in the library, its only included as some functions used in this example require Gamma
uint8_t LEDGamma[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};
//##########################################################################
void readStripEEPROM();
void writeStripEEPROM (bool modeORbriness, uint16_t val);
void modeStrip(int modeL);
void setup_strip();
void colorWipe(uint32_t c, uint8_t wait);
uint32_t Wheel(byte WheelPos);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
//##########################################################################
void setup_strip()
{
  readStripEEPROM();
  
  afio_remap(AFIO_REMAP_SPI1);
  strip.begin();// Sets up the SPI
  strip.show();// Clears the strip, as by default the strip data is set to all LED's off.
  strip.setBrightness(brightness);
}

// Strip task
static void vStrip_Task(void *pvParameters) {
  for (;;) {
    switch (mode_led) {
      case 0:
        colorWipe(strip.Color(0, 0, 0), 20);
        break;
      case 1:
        colorWipe(strip.Color(0, 255, 0), 20); // Green
        break;
      case 2:
        colorWipe(strip.Color(255, 0, 0), 20); // Red
        break;
      case 3:
        colorWipe(strip.Color(0, 0, 255), 20); // Blue
        break;
      case 4:
        rainbow(5);
        break;
      case 5:
        rainbowCycle(10);
        break;
    }
    vTaskDelay(1);
  }
}

//##########################################################################
void modeStrip(int modeL) {
  if (modeL == 6) {
    brightness -= 17;
    if (brightness <= 17) brightness = 17;
    strip.setBrightness(brightness);
    writeStripEEPROM (1, (uint16_t)brightness);
  } else if (modeL == 7) {
    brightness += 17;
    if (brightness >= 255) brightness = 255;
    strip.setBrightness(brightness);
    writeStripEEPROM (1, (uint16_t)brightness);
  } else {
    mode_led = modeL;
    writeStripEEPROM (0, (uint16_t)mode_led);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    vTaskDelay(wait);
  }
}

uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85)
  {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else
  {
    if (WheelPos < 170)
    {
      WheelPos -= 85;
      return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else
    {
      WheelPos -= 170;
      return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
  }
}

void rainbow(uint8_t wait)
{
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    vTaskDelay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait)
{
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    vTaskDelay(wait);
  }
}

void readStripEEPROM() {
  uint16_t _mode_led, _brightness;
  EEPROM.read(stripModeAdd, &_mode_led);
  mode_led = (uint8_t)_mode_led;
  
  check (stripModeAdd);
  EEPROM.read(stripBrightnessAdd, &_brightness);
  check (stripBrightnessAdd);
  brightness = (uint8_t)_brightness;
}

void writeStripEEPROM (bool modeORbriness, uint16_t val) {
  if (modeORbriness == 0) {
    EEPROM.write(stripModeAdd, val);
  } else {
    EEPROM.write(stripBrightnessAdd, val);
  }
}
