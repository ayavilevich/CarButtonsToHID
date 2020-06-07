#include <Arduino.h>
#include <USBComposite.h>

#include "HID.hpp"

const uint8_t reportDescription[] = {
   HID_CONSUMER_REPORT_DESCRIPTOR(),
   HID_KEYBOARD_REPORT_DESCRIPTOR(),
   // HID_ABS_MOUSE_REPORT_DESCRIPTOR() // HID_MOUSE_REPORT_ID
   // HID_MOUSE_REPORT_DESCRIPTOR()
   HID_DIGITIZER_REPORT_DESCRIPTOR()
};

USBHID HIDManager;
USBCompositeSerial CompositeSerial;

HIDConsumer Consumer(HIDManager);
HIDKeyboard Keyboard(HIDManager);
// HIDAbsMouse Mouse(HIDManager);
// HIDMouse Mouse(HIDManager);
HIDDigitizer Mouse(HIDManager); // coordinates are 0 to 16383

void HID::setup() {
  HIDManager.begin(CompositeSerial, reportDescription, sizeof(reportDescription));
  while (!USBComposite);
  delay(1000); // let the system see it
}

void HID::DoKbAltTab() {
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_TAB);
  Keyboard.release(KEY_TAB);
  Keyboard.release(KEY_LEFT_ALT);
}

void HID::DoKbRightArrow() {
  Keyboard.press(KEY_RIGHT_ARROW);
  Keyboard.release(KEY_RIGHT_ARROW);
}

void HID::DoKbLeftArrow() {
  Keyboard.press(KEY_LEFT_ARROW);
  Keyboard.release(KEY_LEFT_ARROW);
}

void HID::DoKbUpArrow() {
  Keyboard.press(KEY_UP_ARROW);
  Keyboard.release(KEY_UP_ARROW);
}

void HID::DoKbDownArrow() {
  Keyboard.press(KEY_DOWN_ARROW);
  Keyboard.release(KEY_DOWN_ARROW);
}

void HID::DoConsBrightnessDown() {
  Consumer.press(HIDConsumer::BRIGHTNESS_DOWN);
  Consumer.release();
}

void HID::DoConsBrightnessUp() {
  Consumer.press(HIDConsumer::BRIGHTNESS_UP);
  Consumer.release();
}
