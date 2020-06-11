#include <Arduino.h>

#include "ResistorLadderButtons.hpp"
#include "StreamPrintf.hpp"

// #define ADC_TEST

const uint16 SAMPLE_ACQUISITION = 10; // us, property of uC
const uint16 SAMPLE_DELAY = 100; // us, a delay we can add to make it all less stressful and more stable
const uint8 BIN_MARGIN_PERCENT = 20; // % of bin ohm value
const uint8 BIN_MARGIN_MIN = 50; // in ohms
const uint16 BIN_DEBOUNCE_SAMPLES = 30000 / (SAMPLE_ACQUISITION + SAMPLE_DELAY); // number of samples in about 30ms
const uint16 HELD_CALLBACK_INTERVAL = 100; // ms
const uint16 ADC_AVERAGING_ALPHA_PERCENT = 10; // parameter of an averaging filter. new value is 10% of previous filtered value

ResistorLadderButtons::ResistorLadderButtons(const uint16 adcMax, const uint16 rBase, const uint8 pin, const uint32* bins, const uint8 binCount)
	: adcMax(adcMax), rBase(rBase), pin(pin), bins(bins), binCount(binCount) {
}

void ResistorLadderButtons::setup() {
	pinMode(pin, INPUT); // assume external pullup is used
}

// the loop function runs over and over again forever
void ResistorLadderButtons::loop() {
#ifdef ADC_TEST
	doAdcTest();
#else
	delay_us(SAMPLE_DELAY);
	uint16 v = analogRead(pin);
	filteredValue = ((uint32)filteredValue * (100 - ADC_AVERAGING_ALPHA_PERCENT) + (uint32)v * ADC_AVERAGING_ALPHA_PERCENT) / 100;
	uint32 r = adcToResistance(rBase, filteredValue);
	int8 bin = rToBin(r);

	// Bench result: 947ms for 100K iterations => 100 iterations per 1ms => 1 iteration is 10us (without delay)
	int static counter = 0;
	counter++;
	if (counter % 100000 == 0) {
		StreamPrintf(debugStream, "Time of 100000 samples: %d\r\n", millis());
	}

	if (bin != lastBin) {
		StreamPrintf(debugStream, "%d - %d - %d - %d - %d - %d - ", millis(), v, filteredValue, r, lastBin, bin);

		if (lastBin > 0) { // if we are leaving a bin that is a button
			if (samplesInLastBin > BIN_DEBOUNCE_SAMPLES) { // if we are leaving it and we were stable in it
				if (debugStream != NULL) {
					debugStream->print("Stable release: ");
					debugStream->print(lastBin);
				}
				// measure duration
				uint32 releaseTime = millis();
				uint32 duration = 0;
				if (releaseTime > lastStablePressTime) { // check for overflow in millis
					duration = releaseTime - lastStablePressTime;
					if (debugStream != NULL) {
						debugStream->print(", Dur: ");
						debugStream->print(duration);
					}
				}
				// debugStream->print(", ");
				if (buttonUpCallback) {
					buttonUpCallback(lastBin, duration);
				}
			}
			// debugStream->print("Releasing: ");
			// debugStream->print(lastBin);
		}
		if (bin > 0) { // if we are entering a new bin that is a button
			// debugStream->print("Pressing: ");
			// debugStream->print(bin);
		}
		if (debugStream != NULL) {
			debugStream->println();
		}

		lastBin = bin;
		samplesInLastBin = 0;
	} else { // same bin
		if (counter % (1000000 / SAMPLE_DELAY) == 0) { // do periodical print of values even if there is no change
			StreamPrintf(debugStream, "%d - %d - %d - %d - %d - Periodic\r\n", millis(), v, filteredValue, r, bin);
		}
		if (samplesInLastBin == BIN_DEBOUNCE_SAMPLES && bin > 0) { // if we are exactly BIN_DEBOUNCE_SAMPLES there
			// stable press
			lastStablePressTime = millis();
			nextHeldCallbackDuration = 0;
			// log
			StreamPrintf(debugStream, "%d - %d - %d - %d, Stable press: %d\r\n", millis(), v, filteredValue, r, bin);
			// callback
			if (buttonDownCallback) {
				buttonDownCallback(bin);
			}
		}
		if (samplesInLastBin > BIN_DEBOUNCE_SAMPLES && bin > 0) {
			// if stable press, see if we need to notify about "hold"
			// measure duration
			uint32 releaseTime = millis();
			uint32 duration = 0;
			if (releaseTime > lastStablePressTime) { // check for overflow in millis
				duration = releaseTime - lastStablePressTime;
				// check if need to notify
				if (duration >= nextHeldCallbackDuration) {
					// update
					if (buttonHeldCallback) {
						buttonHeldCallback(bin, nextHeldCallbackDuration);
					}
					nextHeldCallbackDuration += HELD_CALLBACK_INTERVAL;
				}
			}
		}
		if (samplesInLastBin <= BIN_DEBOUNCE_SAMPLES) { // inc counter till we are BIN_DEBOUNCE_SAMPLES + 1
			samplesInLastBin++;
		}
	}
#endif
}

int8 ResistorLadderButtons::rToBin(uint32 r) {
	for (uint8 i = 0; i < binCount; i++) {
		uint32 margin = bins[i] * BIN_MARGIN_PERCENT / 100;
		margin = max(margin, BIN_MARGIN_MIN);
		if (r + margin >= bins[i] && r <= bins[i] + margin) {
			return i;
		}
	}
	return -1;
}

uint32 ResistorLadderButtons::adcToResistance(uint16 rBase, uint16 adc) {
	return (uint32)adc * rBase / (adcMax - adc);
}

void ResistorLadderButtons::doAdcTest() {
	const int N = 1000;
	uint16 buf[N];
	for (int i = 0; i < N; i++) {
		delay_us(SAMPLE_DELAY);
		buf[i] = analogRead(pin);
	}
	for (int i = 0; i < N; i++) {
		if (debugStream != NULL) {
			debugStream->println(buf[i]);
		}
	}
	delay(5000);
}