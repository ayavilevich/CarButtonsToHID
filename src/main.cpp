#include <Arduino.h>

#include "ResistorLadderButtons.hpp"

#undef LED_BUILTIN
#define LED_BUILTIN PB12 // redefine for black pill STM32 board
#define USE_HID // might interferer with debugging
#define USE_BUZZER
// #define LAB // our "stage" environment
#define PERFORM_HID_ACTIONS

#ifdef USE_HID
#include "HID.hpp"
#else
#endif
#define DebugSerial Serial1

const uint16 ADC_MAX = 4096; // STM32 ADC spec
// const uint16 R_BASE = 43000; // value of internal pullup
const uint16 R_BASE = 4700;
const uint32 BUTTON_PIN = PA0;
const uint32 BUZZER_PIN = PA1;
const uint32 LONG_PRESS_DURATION = 1000; // ms

#ifdef LAB
// test bench
const uint32 BINS[] = {4200, 30, 2200}; // first BIN is "nothing pressed"
enum {
	BUT_NOTHING,
	BUT_1,
	BUT_2,
};
#else
// car
const uint32 BINS[] = {4300 /* no press */, 2100 /*on*/, 10 /*off*/, 1110 /*res*/, 600 /*set+*/, 300 /*set-*/, 120 /*cancel*/};
enum {
	BUT_NOTHING,
	BUT_ON, // left, top
	BUT_OFF, // left, bottom
	BUT_RES, // right, top
	BUT_SETP, // right, 2nd
	BUT_SETM, // right 3rd
	BUT_CANCEL, // right, bottom
};
#endif

void buttonDownCallback(const uint8 button);
void buttonUpCallback(const uint8 button, uint32 duration);
void buttonHeldCallback(const uint8 button, uint32 duration);

ResistorLadderButtons buttons(ADC_MAX, R_BASE, BUTTON_PIN, BINS, (uint8)(sizeof(BINS) / sizeof(BINS[0])));

void setup() {
	// initialize digital pin LED_BUILTIN as an output.
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN,
				 HIGH); // turn LED off (logic is reverse on my board)

	DebugSerial.begin(115200);
	// DebugSerial.setTimeout(5000); // longer timeout so we have time to send
	// commands with new line from the PC
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

void loop() {
	buttons.loop();
}

void buttonDownCallback(const uint8 button) {
	digitalWrite(LED_BUILTIN, LOW);
}

void buttonUpCallback(const uint8 button, uint32 duration) {
	digitalWrite(LED_BUILTIN, HIGH);
	if (duration < LONG_PRESS_DURATION) { // if short press
#ifdef USE_BUZZER
		DebugSerial.print(" Starting tone: ");
		DebugSerial.print(millis());
		tone(BUZZER_PIN, 2000, 100);
#endif
#ifdef PERFORM_HID_ACTIONS
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
		case BUT_ON:
			HID::DoConsHome();
			break;
		case BUT_OFF:
			HID::DoConsBack();
			break;
		case BUT_RES:
			HID::DoConsPlayOrPause();
			break;
		case BUT_SETP:
			// HID::DoKbUpArrow();
			HID::DoConsPlay();
			delay(500);
			HID::DoBTMusicPlayButton();
			break;
		case BUT_SETM:
			// HID::DoKbDownArrow();
			HID::DoConsNext();
			break;
		case BUT_CANCEL:
			//HID::DoRadio();
			HID::DoConsPrevious();
			break;
		}
#endif
#endif
	}
}

void buttonHeldCallback(const uint8 button, uint32 duration) {
	if (duration == LONG_PRESS_DURATION) { // if long hold
#ifdef USE_BUZZER
		DebugSerial.print("Starting long tone: ");
		DebugSerial.println(millis());
		tone(BUZZER_PIN, 4000, 200);
#endif
#ifdef PERFORM_HID_ACTIONS
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
		case BUT_ON:
			// HID::DoPowerConfigMenu();
			HID::DoConsFF();
			break;
		case BUT_OFF:
			// HID::DoEmailToSelf();
			HID::DoConsRewind();
			break;
		case BUT_RES:
			// HID::DoKbAltTab();
			HID::DoMainHomeButton(0);
			break;
		case BUT_SETP:
			// HID::DoDvrToggle();
			// HID::DoConsPlay();
			HID::DoMainHomeButton(1);
			break;
		case BUT_SETM:
			// HID::DoDvrCleanup();
			HID::DoMainHomeButton(2);
			break;
		case BUT_CANCEL:
			// HID::DoNavigation();
			HID::DoMainHomeButton(3); // might not work due to digitizer coordinates limit
			break;
		}
#endif
#endif
	}
}
