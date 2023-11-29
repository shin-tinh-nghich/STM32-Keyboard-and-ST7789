#include "RotaryEncoder.h"

#define PIN_A   PB6   //ky-040 clk pin
#define PIN_B   PB7   //ky-040 dt  pin

RotaryEncoder encoder(PIN_A, PIN_B);

uint16_t cnt_pre, cnt_now;

void encoderISR()
{
  encoder.readAB();
}

void rotaryEncoderConfig(void)
{
  encoder.begin();                                                    //set encoders pins as input & enable built-in pullup resistors

  attachInterrupt(digitalPinToInterrupt(PIN_A), encoderISR, RISING);  //call encoderISR()    every low->high changes

  cnt_pre = encoder.getPosition();
}

////#############################################################################################
// Rotary task
void Rotary_Scan(void *pvParameters)
{
  for (;;) {
    cnt_now = encoder.getPosition();
    if (cnt_now > cnt_pre)
    {
      Consumer.press(HIDConsumer::VOLUME_UP);
      Consumer.release();
      cnt_pre = cnt_now;
    } else if (cnt_now < cnt_pre)
    {
      Consumer.press(HIDConsumer::VOLUME_DOWN);
      Consumer.release();
      cnt_pre = cnt_now;
    }
    vTaskDelay(1);
  }
}
