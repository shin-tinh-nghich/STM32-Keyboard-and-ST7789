// Fast ST7789 IPS 240x240 SPI display library
// (c) 2019 by Pawel A. Hernik

#ifndef _ST7789_STM_H_
#define _ST7789_STM_H_

// ------------------------------
// remove "define COMPATIBILITY_MODE" for best performance on 16MHz AVR Arduinos
// if defined - the library should work on all Arduino compatible boards
#define COMPATIBILITY_MODE

// define for LCD boards where CS pin is internally connected to the ground
#define CS_ALWAYS_LOW
// ------------------------------

#include "Arduino.h"
#include "SPI.h"
#include "Print.h"
#include <Adafruit_GFX.h>
#include <avr/pgmspace.h>

#define DMA_MIN 250
#define DMA_MAX 65535
#define SPI_FREQ 36000000

#define ST7789_TFTWIDTH   240
#define ST7789_TFTHEIGHT  240

#define ST_CMD_DELAY   0x80

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01

#define ST7789_SLPIN   0x10  // sleep on
#define ST7789_SLPOUT  0x11  // sleep off
#define ST7789_PTLON   0x12  // partial on
#define ST7789_NORON   0x13  // partial off
#define ST7789_INVOFF  0x20  // invert off
#define ST7789_INVON   0x21  // invert on
#define ST7789_DISPOFF 0x28  // display off
#define ST7789_DISPON  0x29  // display on
#define ST7789_IDMOFF  0x38  // idle off
#define ST7789_IDMON   0x39  // idle on

#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define ST7789_PTLAR    0x30   // partial start/end
#define ST7789_VSCRDEF  0x33   // SETSCROLLAREA
#define ST7789_VSCRSADD 0x37

#define ST7789_WRDISBV  0x51
#define ST7789_WRCTRLD  0x53
#define ST7789_WRCACE   0x55
#define ST7789_WRCABCMB 0x5e

#define ST7789_POWSAVE    0xbc
#define ST7789_DLPOFFSAVE 0xbd

// bits in MADCTL
#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x00

// System Function Command Table 1
#define ST7789_CMD_NOP               0x00 // No operation
#define ST7789_CMD_SWRESET           0x01 // Software reset
#define ST7789_CMD_RDDID             0x04 // Read display ID
#define ST7789_CMD_RDDST             0x09 // Read display status
#define ST7789_CMD_RDDPM             0x0a // Read display power
#define ST7789_CMD_RDDMADCTL         0x0b // Read display
#define ST7789_CMD_RDDCOLMOD         0x0c // Read display pixel
#define ST7789_CMD_RDDIM             0x0d // Read display image
#define ST7789_CMD_RDDSM             0x0e // Read display signal
#define ST7789_CMD_RDDSDR            0x0f // Read display self-diagnostic result
#define ST7789_CMD_SLPIN             0x10 // Sleep in
#define ST7789_CMD_SLPOUT            0x11 // Sleep out
#define ST7789_CMD_PTLON             0x12 // Partial mode on
#define ST7789_CMD_NORON             0x13 // Partial off (Normal)
#define ST7789_CMD_INVOFF            0x20 // Display inversion off
#define ST7789_CMD_INVON             0x21 // Display inversion on
#define ST7789_CMD_GAMSET            0x26 // Gamma set
#define ST7789_CMD_DISPOFF           0x28 // Display off
#define ST7789_CMD_DISPON            0x29 // Display on
#define ST7789_CMD_CASET             0x2a // Column address set
#define ST7789_CMD_RASET             0x2b // Row address set
#define ST7789_CMD_RAMWR             0x2c // Memory write
#define ST7789_CMD_RAMRD             0x2e // Memory read
#define ST7789_CMD_PTLAR             0x30 // Partial start/end address set
#define ST7789_CMD_VSCRDEF           0x33 // Vertical scrolling definition
#define ST7789_CMD_TEOFF             0x34 // Tearing line effect off
#define ST7789_CMD_TEON              0x35 // Tearing line effect on
#define ST7789_CMD_MADCTL            0x36 // Memory data access control
#define ST7789_CMD_VSCRSADD          0x37 // Vertical address scrolling
#define ST7789_CMD_IDMOFF            0x38 // Idle mode off
#define ST7789_CMD_IDMON             0x39 // Idle mode on
#define ST7789_CMD_COLMOD            0x3a // Interface pixel format
#define ST7789_CMD_RAMWRC            0x3c // Memory write continue
#define ST7789_CMD_RAMRDC            0x3e // Memory read continue
#define ST7789_CMD_TESCAN            0x44 // Set tear scanline
#define ST7789_CMD_RDTESCAN          0x45 // Get scanline
#define ST7789_CMD_WRDISBV           0x51 // Write display brightness
#define ST7789_CMD_RDDISBV           0x52 // Read display brightness value
#define ST7789_CMD_WRCTRLD           0x53 // Write CTRL display
#define ST7789_CMD_RDCTRLD           0x54 // Read CTRL value display
#define ST7789_CMD_WRCACE            0x55 // Write content adaptive brightness control and Color enhancemnet
#define ST7789_CMD_RDCABC            0x56 // Read content adaptive brightness control
#define ST7789_CMD_WRCABCMB          0x5e // Write CABC minimum brightness
#define ST7789_CMD_RDCABCMB          0x5f // Read CABC minimum brightness
#define ST7789_CMD_RDABCSDR          0x68 // Read Automatic Brightness Control Self-Diagnostic Result
#define ST7789_CMD_RDID1             0xda // Read ID1
#define ST7789_CMD_RDID2             0xdb // Read ID2
#define ST7789_CMD_RDID3             0xdc // Read ID3

// System Function Command Table 2
#define ST7789_CMD_RAMCTRL           0xb0 // RAM Control
#define ST7789_CMD_RGBCTRL           0xb1 // RGB Control
#define ST7789_CMD_PORCTRL           0xb2 // Porch control
#define ST7789_CMD_FRCTRL1           0xb3 // Frame Rate Control 1
#define ST7789_CMD_GCTRL             0xb7 // Gate control
#define ST7789_CMD_DGMEN             0xba // Digital Gamma Enable
#define ST7789_CMD_VCOMS             0xbb // VCOM Setting
#define ST7789_CMD_LCMCTRL           0xc0 // LCM Control
#define ST7789_CMD_IDSET             0xc1 // ID Setting
#define ST7789_CMD_VDVVRHEN          0xc2 // VDV and VRH Command enable
#define ST7789_CMD_VRHS              0xc3 // VRH Set
#define ST7789_CMD_VDVSET            0xc4 // VDV Setting
#define ST7789_CMD_VCMOFSET          0xc5 // VCOM Offset Set
#define ST7789_CMD_FRCTR2            0xc6 // FR Control 2
#define ST7789_CMD_CABCCTRL          0xc7 // CABC Control
#define ST7789_CMD_REGSEL1           0xc8 // Register value selection 1
#define ST7789_CMD_REGSEL2           0xca // Register value selection 2
#define ST7789_CMD_PWMFRSEL          0xcc // PWM Frequency Selection
#define ST7789_CMD_PWCTRL1           0xd0 // Power Control 1
#define ST7789_CMD_VAPVANEN          0xd2 // Enable VAP/VAN signal output
#define ST7789_CMD_CMD2EN            0xdf // Command 2 Enable
#define ST7789_CMD_PVGAMCTRL         0xe0 // Positive Voltage Gamma Control
#define ST7789_CMD_NVGAMCTRL         0xe1 // Negative voltage Gamma Control
#define ST7789_CMD_DGMLUTR           0xe2 // Digital Gamma Look-up Table for Red
#define ST7789_CMD_DGMLUTB           0xe3 // Digital Gamma Look-up Table for Blue
#define ST7789_CMD_GATECTRL          0xe4 // Gate control
#define ST7789_CMD_PWCTRL2           0xe8 // Power Control 2
#define ST7789_CMD_EQCTRL            0xe9 // Equalize Time Control
#define ST7789_CMD_PROMCTRL          0xec // Program Control
#define ST7789_CMD_PROMEN            0xfa // Program Mode Enable
#define ST7789_CMD_NVMSET            0xfc // NVM Setting
#define ST7789_CMD_PROMACT           0xfe // Program Action

// Color definitions
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define RGBto565(r,g,b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

class Arduino_ST7789 : public Adafruit_GFX {

  public:
    Arduino_ST7789(int8_t DC, int8_t RST, int8_t CS = -1);

    void init(uint16_t width, uint16_t height);
    void init() {
      init(ST7789_TFTWIDTH, ST7789_TFTHEIGHT);
    }
    void begin() {
      init(ST7789_TFTWIDTH, ST7789_TFTHEIGHT);
    }
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void pushColor(uint16_t color);
    void fillScreen(uint16_t color = BLACK);
    void clearScreen() {
      fillScreen(BLACK);
    }
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *img);
    void drawImageF(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *img16);
    void setRotation(uint8_t r);
    void invertDisplay(boolean mode);
    void partialDisplay(boolean mode);
    void sleepDisplay(boolean mode);
    void enableDisplay(boolean mode);
    void idleDisplay(boolean mode);
    void resetDisplay();
    void setScrollArea(uint16_t tfa, uint16_t bfa);
    void setScroll(uint16_t vsp);
    void setPartArea(uint16_t sr, uint16_t er);
    void setBrightness(uint8_t br);
    void powerSave(uint8_t mode);

    uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
      return Color565(r, g, b);
    }
    void rgbWheel(int idx, uint8_t *_r, uint8_t *_g, uint8_t *_b);
    uint16_t rgbWheel(int idx);

  protected:
    uint8_t  _colstart, _rowstart, _xstart, _ystart;
    void displayInit(const uint8_t *addr);
    void writeCmd(uint16_t c);
    void writeData(uint16_t d);

  private:
    int8_t  csPin, dcPin, rstPin;
    uint8_t  csMask, dcMask;
    volatile uint8_t  *csPort, *dcPort;
    uint16_t dmaBuf[128];

};

#endif
