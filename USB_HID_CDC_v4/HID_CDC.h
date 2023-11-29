#include <USBComposite.h>

// Class init
USBHID HID;

const uint8_t reportDescription[] = {
  HID_CONSUMER_REPORT_DESCRIPTOR(),
  HID_KEYBOARD_REPORT_DESCRIPTOR()
};

HIDKeyboard Keyboard(HID);
HIDConsumer Consumer(HID);
USBCompositeSerial CompositeSerial;

void setupUSB() {
  USBComposite.setProductString("USB camos Cmoposite Keyboard");     //Device's name
  HID.begin(CompositeSerial, reportDescription, sizeof(reportDescription));
  while (!USBComposite);
}
