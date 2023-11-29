/***************************************************************************************************/
/*
   This is an Arduino library for Quadrature Rotary Encoder

   written by : enjoyneering79
   sourse code: https://github.com/enjoyneering/RotaryEncoder
*/
/***************************************************************************************************/

#include "RotaryEncoder.h"

/**************************************************************************/
/*
    Constructor
*/
/**************************************************************************/
RotaryEncoder::RotaryEncoder(uint8_t encoderA, uint8_t encoderB)
{
  _encoderA      = encoderA;
  _encoderB      = encoderB;
}

/**************************************************************************/
/*
    begin

    Sets encoders pins as input & enables built-in pullup resistors

    NOTE:
    - high value of pull-up resistor limits the speed
    - typical value of external pull-up resistor is 10kOhm, acceptable
      range 5kOhm..100kOhm
*/
/**************************************************************************/
void RotaryEncoder::begin()
{
  pinMode(_encoderA,      INPUT_PULLUP); //enable internal pull-up resistors
  pinMode(_encoderB,      INPUT_PULLUP);
}

/**************************************************************************/
/*
    readAB()

    Reads "A" & "B" pins value

    NOTE:
    - always call this function before getPosition()
    - 100nF/0.1Î¼F capacitors between A & B channel pin & ground is a must!!!
    - for fast MCU like Cortex use Interrupt Service Routine with "CHANGE"
      parameter, ISR called when pin "A" changes from "1" to "0"
      or from "0" to "1"
    - the ISR function must take no parameters & return nothing
    - delay() doesn't work during ISR & millis() doesn't increment
    - declare all global variables inside ISR as "volatile", it prevent
      compiler to make any optimization & unnecessary changes in the code
      with the variable
*/
/**************************************************************************/
void RotaryEncoder::readAB()
{
  noInterrupts();                                               //disable interrupts
  //fast MCU
  _currValueAB  = digitalRead(_encoderA) << 1;
  _currValueAB |= digitalRead(_encoderB);

  switch ((_prevValueAB | _currValueAB))
  {
    //fast MCU
    case 0b0001: case 0b1110:                                       //CW states, 1 count  per click
      //case 0b0001: case 0b1110: case 0b1000: case 0b0111:         //CW states, 2 counts per click
      _counter++;
      break;

    //fast MCU
    case 0b0100: case 0b1011:                                       //CCW states, 1 count  per click
      //case 0b0100: case 0b1011: case 0b0010: case 0b1101:         //CCW states, 2 counts per click
      _counter--;
      break;
  }

  _prevValueAB = _currValueAB << 2;                             //update previouse state

  interrupts();                                                 //enable interrupts
}
/**************************************************************************/
/*
    getPosition()

    Return encoder position
*/
/**************************************************************************/
int16_t RotaryEncoder::getPosition()
{
  return _counter;
}
/**************************************************************************/
/*
    setPosition()

    Manualy sets encoder position
*/
/**************************************************************************/
void RotaryEncoder::setPosition(int16_t position)
{
  _counter = position;
}
