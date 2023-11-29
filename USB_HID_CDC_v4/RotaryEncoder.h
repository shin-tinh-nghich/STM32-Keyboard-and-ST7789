/***************************************************************************************************/
/*
   This is an Arduino library for Quadrature Rotary Encoder 

   written by : enjoyneering79
   sourse code: https://github.com/enjoyneering/RotaryEncoder
*/
/***************************************************************************************************/
#include <WProgram.h>
#include <avr/pgmspace.h>                  //PROGMEM  support Arduino STM32

class RotaryEncoder
{
  public:
    RotaryEncoder(uint8_t encoderA, uint8_t encoderB);

    void     begin();
    void     readAB();

    int16_t  getPosition();

    void     setPosition(int16_t position);

  protected:
    volatile int16_t _counter = 0;        //encoder click counter, limit -32768..32767

  private:
    volatile uint8_t _prevValueAB = 0;    //previouse state of "A"+"B"
    volatile uint8_t _currValueAB = 0;    //current   state of "A"+"B"

             uint8_t _encoderA;           //pin "A"
             uint8_t _encoderB;           //pin "B"
};
