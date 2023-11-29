#include "KeyPadConfig.h"
#include <EEPROM.h>

KeyPad_t  KeyPad;

uint16_t functionKey[10] = {0};
uint16_t key1[10] = {0};
uint16_t key2[10] = {0};
uint16_t key3[10] = {0};
const uint16_t functionKey_AddW = 0x10;
const uint16_t key1_AddW = 0x20;
const uint16_t key2_AddW = 0x30;
const uint16_t key3_AddW = 0x40;

void check(uint16_t AddressWrite);
void check_arr();
void keyLayoutP (uint8_t numberKey);
void keyLayoutR (uint8_t numberKey);
void keyPress(uint16_t key);
void keyRelease(uint16_t key);
void readKeyEEprom(uint8_t numberKey);
void writeKeyEEPROM(uint8_t numberKey, uint16_t functionKey, uint16_t key1, uint16_t key2, uint16_t key3);
//#############################################################################################
void  KeyPad_Init(void)
{
  KeyPad.ColumnSize = sizeof(_KEYPAD_COLUMN_GPIO_PIN) / 2;
  KeyPad.RowSize = sizeof(_KEYPAD_ROW_GPIO_PIN) / 2;
  
  for (uint8_t  i = 0 ; i < KeyPad.ColumnSize ; i++)
  {
    pinMode(_KEYPAD_COLUMN_GPIO_PIN[i], OUTPUT);
  }
  for (uint8_t  i = 0 ; i < KeyPad.RowSize ; i++)
  {
    pinMode(_KEYPAD_ROW_GPIO_PIN[i], INPUT_PULLDOWN);
  }

  for (uint8_t i = 1; i <= 9; i++) {
    readKeyEEprom(i);
  }
}
//#############################################################################################
// Keboard task
void  KeyPad_Scan(void *pvParameters)
{
  for (;;) {
    uint16_t  key = 0;
    for (uint8_t c = 0 ; c < KeyPad.ColumnSize ; c++)
    {
      for (uint8_t i = 0 ; i < KeyPad.ColumnSize ; i++)
        digitalWrite(_KEYPAD_COLUMN_GPIO_PIN[i], LOW);
      digitalWrite(_KEYPAD_COLUMN_GPIO_PIN[c], HIGH);
      vTaskDelay(5);
      for (uint8_t r = 0 ; r < KeyPad.RowSize ; r++)
      {
        if (digitalRead(_KEYPAD_ROW_GPIO_PIN[r]) == HIGH)
        {
          key |= 1 << c;
          key |= 1 << (r + 8);
          status_LED = 4;
          vTaskDelay(1);
          // CompositeSerial.println(key, HEX);
          keyPress(key);
          while (digitalRead(_KEYPAD_ROW_GPIO_PIN[r]) == HIGH) vTaskDelay(1);
          status_LED = 3;
          vTaskDelay(1);
          keyRelease(key);
        }
      }
    }
    vTaskDelay(1);
  }
}
////#############################################################################################
void keyPress(uint16_t key)
{
  switch (key)
  {
    case 0x0101:
      keyLayoutP (1);
      break;
    case 0x0201:
      keyLayoutP (2);
      break;
    case 0x0401:
      keyLayoutP (3);
      break;
    case 0x0801:
      keyLayoutP (4);
      break;
    case 0x1001:
      keyLayoutP (5);
      break;
    case 0x0102:      // rotary button default
      Consumer.press(HIDConsumer::MUTE);
      break;
    case 0x0202:
      keyLayoutP (6);
      break;
    case 0x0402:
      keyLayoutP (7);
      break;
    case 0x0802:
      keyLayoutP (8);
      break;
    case 0x1002:
      keyLayoutP (9);
      break;
  }
}

void keyRelease(uint16_t key)
{
  switch (key)
  {
    case 0x0101:
      keyLayoutR (1);
      break;
    case 0x0201:
      keyLayoutR (2);
      break;
    case 0x0401:
      keyLayoutR (3);
      break;
    case 0x0801:
      keyLayoutR (4);
      break;
    case 0x1001:
      keyLayoutR (5);
      break;
    case 0x0102:      // rotary button default
      Consumer.release();
      break;
    case 0x0202:
      keyLayoutR (6);
      break;
    case 0x0402:
      keyLayoutR (7);
      break;
    case 0x0802:
      keyLayoutR (8);
      break;
    case 0x1002:
      keyLayoutR (9);
      break;
  }
}

void keyLayoutP (uint8_t numberKey) {
  switch (functionKey[numberKey]) {
    case 0:
      Keyboard.press((uint8_t)key1[numberKey]);
      break;
    case 1:
      Consumer.press((uint8_t)key1[numberKey]);
      break;
    case 2:
      Keyboard.press((uint8_t)key1[numberKey]);
      Keyboard.press((uint8_t)key2[numberKey]);
      break;
    case 3:
      Keyboard.press((uint8_t)key1[numberKey]);
      Keyboard.press((uint8_t)key2[numberKey]);
      Keyboard.press((uint8_t)key3[numberKey]);
      break;
  }
}

void keyLayoutR (uint8_t numberKey) {
  switch (functionKey[numberKey]) {
    case 0:
      Keyboard.release((uint8_t)key1[numberKey]);
      break;
    case 1:
      Consumer.release();
      break;
    case 2:
      Keyboard.release((uint8_t)key1[numberKey]);
      Keyboard.release((uint8_t)key2[numberKey]);
      break;
    case 3:
      Keyboard.release((uint8_t)key1[numberKey]);
      Keyboard.release((uint8_t)key2[numberKey]);
      Keyboard.release((uint8_t)key3[numberKey]);
      break;
  }
}

void readKeyEEprom(uint8_t numberKey) {
  EEPROM.read(functionKey_AddW + numberKey, &functionKey[numberKey]);
  check (functionKey_AddW + numberKey);
  EEPROM.read(key1_AddW + numberKey, &key1[numberKey]);
  check (key1_AddW + numberKey);
  switch (functionKey[numberKey]) {
    case 2:
      EEPROM.read(key2_AddW + numberKey, &key2[numberKey]);
      check (key2_AddW + numberKey);
      break;
    case 3:
      EEPROM.read(key2_AddW + numberKey, &key2[numberKey]);
      check (key2_AddW + numberKey);
      EEPROM.read(key3_AddW + numberKey, &key3[numberKey]);
      check (key3_AddW + numberKey);
      break;
  }
}

/* 4 array: functionKey[max_num_key], key1[max_num_key], key2[max_num_key], key3[max_num_key]
 * if use function called "Keyboard" like: key a, b, c, etc... : functionKey[numberKey] = 0, and key code use key1; key2, key3 not use
 * if use function called "Consumer" like: key volume up, down, brighness, etc... : functionKey[numberKey] = 1, and key code use key1; key2, key3 not use
 * Keystrokes with 2 key: functionKey[numberKey] = 2, key code use key1, key2; key3 not use
 * Keystrokes with 3 key: functionKey[numberKey] = 3, key code use key1, key2 and key3
 */

void writeKeyEEPROM(uint8_t numberKey, uint16_t functionKey, uint16_t key1, uint16_t key2, uint16_t key3) {
  EEPROM.write(functionKey_AddW + numberKey, functionKey);
  EEPROM.write(key1_AddW + numberKey, key1);
  switch (functionKey) {
    case 2:
      EEPROM.write(key2_AddW + numberKey, key2);
      break;
    case 3:
      EEPROM.write(key2_AddW + numberKey, key2);
      EEPROM.write(key3_AddW + numberKey, key3);
      break;
  }
}

void check(uint16_t AddressWrite) {
  uint16_t Data, Status;
  Status = EEPROM.read(AddressWrite, &Data);
  CompositeSerial.print("EEPROM.read(0x");
  CompositeSerial.print(AddressWrite, HEX);
  CompositeSerial.print(", &..) = 0x");
  CompositeSerial.print(Data, HEX);
  CompositeSerial.print(" : Status : ");
  CompositeSerial.println(Status, HEX);
}

void check_arr() {
  for (uint8_t i = 1; i <= 9;i++) {
    CompositeSerial.print("key: ");
    CompositeSerial.print(i);
    CompositeSerial.print("\tfunc: ");
    CompositeSerial.print(functionKey[i]);
    CompositeSerial.print("\tkey1: ");
    CompositeSerial.print(key1[i]);
    if (functionKey[i] == 2) {
      CompositeSerial.print("\tkey2: ");
      CompositeSerial.print(key2[i]);
    } else if (functionKey[i] == 3) {
      CompositeSerial.print("\tkey2: ");
      CompositeSerial.print(key2[i]);
      CompositeSerial.print("\tkey3: ");
      CompositeSerial.print(key3[i]);
    }
    CompositeSerial.println();
  }
}
