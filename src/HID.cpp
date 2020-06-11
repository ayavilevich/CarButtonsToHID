#include <Arduino.h>
#include <USBComposite.h>

#include "HID.hpp"

#define MAX_DIGITIZER 4095

const uint8_t reportDescription[] = {HID_CONSUMER_REPORT_DESCRIPTOR(), HID_KEYBOARD_REPORT_DESCRIPTOR(),
									 // HID_ABS_MOUSE_REPORT_DESCRIPTOR()
									 // HID_MOUSE_REPORT_ID
									 // HID_MOUSE_REPORT_DESCRIPTOR()
									 HID_DIGITIZER_REPORT_DESCRIPTOR()};

USBHID HIDManager;
USBCompositeSerial CompositeSerial;

HIDConsumer Consumer(HIDManager);
HIDKeyboard Keyboard(HIDManager);
// HIDAbsMouse Mouse(HIDManager);
// HIDMouse Mouse(HIDManager);
HIDDigitizer Mouse(HIDManager); // coordinates are 0 to 16383

void HID::setup() {
	HIDManager.begin(CompositeSerial, reportDescription, sizeof(reportDescription));
	while (!USBComposite)
		;
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

void HID::DoConsHome() {
	Consumer.press(0x223);
	Consumer.release();
}

void HID::DoConsBack() {
	Consumer.press(0x224);
	Consumer.release();
}

void HID::DoConsSelectApp() {
	Consumer.press(0x1A2);
	Consumer.release();
}

void HID::DoKbHome() {
	Keyboard.press(KEY_LEFT_GUI);
	Keyboard.press(KEY_RETURN);
	Keyboard.release(KEY_RETURN);
	Keyboard.release(KEY_LEFT_GUI);
}

void HID::DoKbBack() {
	Keyboard.press(KEY_LEFT_GUI);
	Keyboard.press(KEY_BACKSPACE);
	Keyboard.release(KEY_BACKSPACE);
	Keyboard.release(KEY_LEFT_GUI);
}

void HID::DoTouchSwipeLeft() {
	Mouse.move(MAX_DIGITIZER * 80 / 100, MAX_DIGITIZER / 2);
	Mouse.press();
	delay(50);
	Mouse.move(MAX_DIGITIZER * 20 / 100, MAX_DIGITIZER / 2);
	delay(50);
	Mouse.release();
	delay(1000); // let it finish the swipe animation
}

void HID::DoTouchSwipeRight() {
	// TODO
}

void HID::DoPowerConfigMenu() {
	// enable accessibility for MacroDroid, assume it is disabled, doesn't persist between retarts
	// get home
	DoConsHome();
	delay(50);
	DoConsHome();
	delay(200);
	// press settings icon
	Mouse.move(MAX_DIGITIZER * 155 / 175, MAX_DIGITIZER * 80 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
	delay(150);
	Mouse.move(1, 1);
	// TODO, should I be in range to move this away?
	// get to top
	Keyboard.press(KEY_UP_ARROW);
	delay(2000);
	Keyboard.release(KEY_UP_ARROW);
	delay(100);
	// 9x down
	for (int i = 0; i < 9; i++) {
		DoKbDownArrow();
		delay(100);
	}
	Keyboard.press(KEY_RETURN);
	Keyboard.release(KEY_RETURN);
	delay(100);
	// select MacroDroid
	DoKbDownArrow();
	delay(50);
	Keyboard.press(KEY_RETURN);
	Keyboard.release(KEY_RETURN);
	delay(50);
	/*
	// toggle
	Keyboard.press(' ');
	Keyboard.release(' ');
	delay(50);
	// approve change
	Keyboard.press(KEY_TAB);
	Keyboard.release(KEY_TAB);
	delay(50);
	Keyboard.press(KEY_TAB);
	Keyboard.release(KEY_TAB);
	delay(50);
	Keyboard.press(KEY_RETURN);
	Keyboard.release(KEY_RETURN);
	delay(50);
	// run macrodroid action
	// get home
	DoConsHome();
	delay(50);
	// move to second panel
	Keyboard.press(KEY_RIGHT_ARROW);
	Keyboard.release(KEY_RIGHT_ARROW);
	delay(50);
	// press green icon on macrodroid widget
	Mouse.move(MAX_DIGITIZER * 95 / 175, MAX_DIGITIZER * 15 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
	delay(150);
	*/
}

void HID::DoDvrCleanup() {
	// get home
	DoConsHome();
	delay(50);
	DoConsHome();
	delay(50);
	// change to second panel
	DoTouchSwipeLeft();
	// press old file cleanup icon
	Mouse.move(MAX_DIGITIZER * 75 / 175, MAX_DIGITIZER * 30 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
	delay(1000);
	// press "run now"
	Mouse.move(MAX_DIGITIZER / 2, MAX_DIGITIZER * 88 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
	/*
	Keyboard.press(KEY_TAB);
	Keyboard.release(KEY_TAB);
	delay(200);
	Keyboard.press(KEY_RETURN);
	Keyboard.release(KEY_RETURN);
	delay(50);
	*/
}

void HID::DoDvrToggle() {
	// get home
	DoConsHome();
	delay(50);
	DoConsHome();
	delay(50);
	// press dvr icon
	Mouse.move(MAX_DIGITIZER * 125 / 175, MAX_DIGITIZER * 80 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
	delay(2000);
	// press recorder icon
	Mouse.move(MAX_DIGITIZER * 65 / 175, MAX_DIGITIZER * 90 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
}

void HID::DoBtMusicToggle() {
	// get home
	DoConsHome();
	delay(50);
	DoConsHome();
	delay(50);
	// press bt music icon
	Mouse.move(MAX_DIGITIZER * 75 / 175, MAX_DIGITIZER * 80 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
	delay(1000);
	// press play icon
	Mouse.move(MAX_DIGITIZER / 2, MAX_DIGITIZER * 90 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
}

void HID::DoEmailToSelf() {
	Consumer.press(0x18A); // run email app
	Consumer.release();
	delay(3000);
	// press compose icon
	Mouse.move(MAX_DIGITIZER * 165 / 175, MAX_DIGITIZER * 85 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
	delay(2000);
	// type "to"
	Keyboard.press('a');
	Keyboard.release('a');
	delay(100);
	Keyboard.press('r');
	Keyboard.release('r');
	delay(100);
	Keyboard.press('i');
	Keyboard.release('i');
	delay(100);
	Keyboard.press('k');
	Keyboard.release('k');
	delay(500);
	DoKbDownArrow();
	delay(200);
	Keyboard.press(KEY_RETURN);
	Keyboard.release(KEY_RETURN);
	delay(200);
	// move to "subject"
	Keyboard.press(KEY_TAB);
	Keyboard.release(KEY_TAB);
	delay(100);
	Keyboard.press(KEY_TAB);
	Keyboard.release(KEY_TAB);
	delay(100);
}

void HID::DoRadio() {
	// get home
	DoConsHome();
	delay(50);
	DoConsHome();
	delay(50);
	// press radio icon
	Mouse.move(MAX_DIGITIZER * 105 / 175, MAX_DIGITIZER * 80 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
}

void HID::DoNavigation() {
	// get home
	DoConsHome();
	delay(50);
	DoConsHome();
	delay(50);
	// change to second panel
	DoTouchSwipeLeft();
	// press old file cleanup icon
	Mouse.move(MAX_DIGITIZER * 125 / 175, MAX_DIGITIZER * 30 / 95);
	Mouse.press();
	delay(50);
	Mouse.release();
	delay(100);
}
