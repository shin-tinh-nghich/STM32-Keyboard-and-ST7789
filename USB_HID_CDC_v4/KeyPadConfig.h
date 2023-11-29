#ifndef	_KEYPADCONFIG_H
#define	_KEYPADCONFIG_H

/*
 ************************************************
 *          10    08    04    02    01
 * --------------------------------------------
 * A7    |  a  |  b  |  c  |  d  |  e  |     02
 * --------------------------------------------
 * A6    |  f  |  g  |  h  |  i  |  j  |     01
 * --------------------------------------------
 *          A1    A2    A3    A4    A5 
 ************************************************
*/

typedef struct
{
  uint8_t   ColumnSize;
  uint8_t   RowSize;
  uint16_t  LastKey;

} KeyPad_t;

const uint16_t _KEYPAD_COLUMN_GPIO_PIN[] =
{
  PA6,
	PA7
};

const uint16_t _KEYPAD_ROW_GPIO_PIN[] =
{
  PA5,
  PA4,
  PA3,
  PA2,
  PA1
};
#endif
