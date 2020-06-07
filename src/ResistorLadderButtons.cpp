#include <Arduino.h>

#include "ResistorLadderButtons.hpp"

const uint8 BIN_MARGIN_PERCENT = 10; // % of bin ohm value
const uint8 BIN_MARGIN_MIN = 50;	 // in ohms
const uint16 BIN_DEBOUNCE_SAMPLES = 1000;
const uint16 HELD_CALLBACK_INTERVAL = 100; // ms

ResistorLadderButtons::ResistorLadderButtons(const uint16 adcMax, const uint16 rBase, const uint8 pin, const uint16 *bins, const uint8 binCount) : adcMax(adcMax), rBase(rBase), pin(pin), bins(bins), binCount(binCount)
{
}

void ResistorLadderButtons::setup()
{
	pinMode(pin, INPUT); // assume external pullup is used

	// pinMode(pin, INPUT_PULLUP); // use internal pullup
}

// the loop function runs over and over again forever
void ResistorLadderButtons::loop()
{
	uint16 v = analogRead(pin);
	uint16 r = adcToResistance(rBase, v);
	int8 bin = rToBin(r);

	if (bin != lastBin)
	{
		if (debugStream != NULL) {
			debugStream->print(v);
			debugStream->print(" - ");
			debugStream->print(r);
			debugStream->print(" - ");
			debugStream->print(lastBin);
			debugStream->print(" - ");
			debugStream->print(bin);
			debugStream->print(" - ");
		}

		if (lastBin > 0)
		{ // if we are leaving a bin that is a button
			if (samplesInLastBin > BIN_DEBOUNCE_SAMPLES)
			{ // if we are leaving it and we were stable in it
				if (debugStream != NULL) {
					debugStream->print("Stable release: ");
					debugStream->print(lastBin);
				}
				// measure duration
				uint32 releaseTime = millis();
				uint32 duration = 0;
				if (releaseTime > lastStablePressTime)
				{ // check for overflow in millis
					duration = releaseTime - lastStablePressTime;
					if (debugStream != NULL) {
						debugStream->print(", Dur: ");
						debugStream->print(duration);
					}
				}
				// debugStream->print(", ");
				if (buttonUpCallback)
				{
					buttonUpCallback(lastBin, duration);
				}
			}
			// debugStream->print("Releasing: ");
			// debugStream->print(lastBin);
		}
		if (bin > 0)
		{	// if we are entering a new bin that is a button
			// debugStream->print("Pressing: ");
			// debugStream->print(bin);
		}
		if (debugStream != NULL) {
			debugStream->println();
		}

		lastBin = bin;
		samplesInLastBin = 0;
	}
	else
	{ // same bin
		if (samplesInLastBin == BIN_DEBOUNCE_SAMPLES && bin > 0)
		{ // if we are exactly BIN_DEBOUNCE_SAMPLES there
			// stable press
			lastStablePressTime = millis();
			nextHeldCallbackDuration = 0;
			// log
			if (debugStream != NULL) {
				debugStream->print(v);
				debugStream->print(" - ");
				debugStream->print(r);
				debugStream->print(" - ");
				debugStream->print("Stable press: ");
				debugStream->print(bin);
				debugStream->println();
			}
			// callback
			if (buttonDownCallback)
			{
				buttonDownCallback(bin);
			}
		}
		if (samplesInLastBin > BIN_DEBOUNCE_SAMPLES && bin > 0) {
			// if stable press, see if we need to notify about "hold"
			// measure duration
			uint32 releaseTime = millis();
			uint32 duration = 0;
			if (releaseTime > lastStablePressTime)
			{ // check for overflow in millis
				duration = releaseTime - lastStablePressTime;
				// check if need to notify
				if (duration >= nextHeldCallbackDuration) {
					// log
					/*
					debugStream->print(v);
					debugStream->print(" - ");
					debugStream->print(r);
					debugStream->print(" - ");
					debugStream->print("Hold: ");
					debugStream->print(bin);
					debugStream->print(", Dur: ");
					debugStream->print(duration);
					debugStream->print(", Next: ");
					debugStream->print(nextHeldCallbackDuration);
					debugStream->println();
					*/
					// update
					if (buttonHeldCallback)
					{
						buttonHeldCallback(bin, nextHeldCallbackDuration);
					}
					nextHeldCallbackDuration += HELD_CALLBACK_INTERVAL;
				}
			}
		}
		if (samplesInLastBin <= BIN_DEBOUNCE_SAMPLES)
		{ // inc counter till we are BIN_DEBOUNCE_SAMPLES + 1
			samplesInLastBin++;
		}
	}
}

int8 ResistorLadderButtons::rToBin(uint16 r)
{
	for (uint8 i = 0; i < binCount; i++)
	{
		uint16 margin = bins[i] * BIN_MARGIN_PERCENT / 100;
		margin = max(margin, BIN_MARGIN_MIN);
		if (r >= bins[i] - margin && r <= bins[i] + margin)
		{
			return i;
		}
	}
	return -1;
}

uint16 ResistorLadderButtons::adcToResistance(uint16 rBase, uint16 adc)
{
	return (int)adc * rBase / (adcMax - adc);
}
