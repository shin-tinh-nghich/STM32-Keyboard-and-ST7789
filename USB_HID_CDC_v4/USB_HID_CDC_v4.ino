#include <MapleFreeRTOS900.h>
#include "HID_CDC.h"
#include "LEDConfig.h"

#include "KeyPad.h"
#include "RotaryEncoderConfig.h"
#include "ws2812_func.h"
#include "ST7789_Config.h"

void setup() {
  setupUSB();

  setupLED();

  KeyPad_Init();

  rotaryEncoderConfig();

  setup_strip();
  
  setupTFT();
  
  xTaskCreate(vLED_Task,
              "vLED_Task",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 1,
              NULL);
  xTaskCreate(Rotary_Scan,
              "Rotary_Scan",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 2,
              NULL);
  xTaskCreate(KeyPad_Scan,
              "KeyPad_Scan",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 2,
              NULL);
  xTaskCreate(vStrip_Task,
              "vStrip_Task",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 1,
              NULL);            
  xTaskCreate(tft_Task,
              "tft_Task",
              256,
              NULL,
              tskIDLE_PRIORITY + 2,
              NULL);    
  vTaskStartScheduler();
}

void loop() {
  // Don't write anything
}
