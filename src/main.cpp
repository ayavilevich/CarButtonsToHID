#include <Arduino.h>

#include "ResistorLadderButtons.hpp"

#define LED_BUILTIN PB12 // redefine for black pill STM32 board
#define USE_HID
#define LAB

#ifdef USE_HID
#include "HID.hpp"
#define DebugSerial Serial1
#else
#define DebugSerial Serial
#endif

const uint16 ADC_MAX = 4096; // STM32 ADC spec
// const uint16 R_BASE = 43000; // value of internal pullup
const uint16 R_BASE = 4700;
const uint32 BUTTON_PIN = PA0;
const uint32 BUZZER_PIN = PA1;
const uint32 LONG_PRESS_DURATION = 1000; // ms

#ifdef LAB
// test bench
const uint16 BINS[] = {4200, 30, 2200}; // first BIN is "nothing pressed"
enum {
	BUT_NOTHING,
	BUT_1,
	BUT_2,
};
#else
// car
const uint16 BINS[] = { 4300 /* no press */, 2100 /*on*/, 10 /*off*/, 1110 /*res*/, 600 /*set+*/, 300 /*set-*/, 120 /*cancel*/ };
enum {
	BUT_NOTHING,
	BUT_ON,
	BUT_OFF,
	BUT_RES,
	BUT_SETP,
	BUT_SETM,
	BUT_CANCEL,
};
#endif

void buttonDownCallback(const uint8 button);
void buttonUpCallback(const uint8 button, uint32 duration);
void buttonHeldCallback(const uint8 button, uint32 duration);

ResistorLadderButtons buttons(ADC_MAX, R_BASE, BUTTON_PIN, BINS, (uint8)(sizeof(BINS) / sizeof(uint16)));

void setup()
{
	// initialize digital pin LED_BUILTIN as an output.
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH); // turn LED off (logic is reverse on my board)

	DebugSerial.begin(115200);
	// DebugSerial.setTimeout(5000); // longer timeout so we have time to send commands with new line from the PC
	delay(100);
	DebugSerial.println("Script started");

#ifdef USE_HID
	DebugSerial.println("Starting USB HID composite");
	HID::setup();
#endif

	DebugSerial.println("Starting Buttons");
	buttons.setup();
	buttons.setDebugStream(&DebugSerial); // optional
	buttons.setButtonDownCallback(buttonDownCallback);
	buttons.setButtonUpCallback(buttonUpCallback);
	buttons.setButtonHeldCallback(buttonHeldCallback);
}

void loop()
{
	buttons.loop();
}

void buttonDownCallback(const uint8 button) {
	digitalWrite(LED_BUILTIN, LOW);
}

void buttonUpCallback(const uint8 button, uint32 duration) {
	digitalWrite(LED_BUILTIN, HIGH);
	if (duration < LONG_PRESS_DURATION) { // if short press
		tone(BUZZER_PIN, 2000, 100);
#ifdef LAB
		switch (button) {
		case BUT_1:
			HID::DoKbUpArrow();
			break;
		case BUT_2:
			HID::DoKbDownArrow();
			break;
		}
#else
		switch (button) {

		}
#endif
	}
}

void buttonHeldCallback(const uint8 button, uint32 duration) {
	if (duration == LONG_PRESS_DURATION) { // if long hold
		tone(BUZZER_PIN, 4000, 200);
#ifdef LAB
		switch (button) {
		case BUT_1:
			HID::DoKbAltTab();
			break;
		case BUT_2:
			HID::DoConsBrightnessUp();
			break;
		}
#else
		switch (button) {
			
		}
#endif
	}
}
