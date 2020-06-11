#include <Arduino.h>

class ResistorLadderButtons {
  public:
	ResistorLadderButtons(const uint16 adcMax, const uint16 rBase, const uint8 pin, const uint32* bins, const uint8 binCount);
	void setup();
	void loop();

	// property setters
	void setButtonDownCallback(void (*callback)(const uint8 button)) {
		buttonDownCallback = callback;
	}
	void setButtonUpCallback(void (*callback)(const uint8 button, uint32 duration)) {
		buttonUpCallback = callback;
	}
	void setButtonHeldCallback(void (*callback)(const uint8 button, uint32 duration)) {
		buttonHeldCallback = callback;
	}
	void setDebugStream(Stream* stream) {
		debugStream = stream;
	}

  private:
	int8 rToBin(uint32 r);
	uint32 adcToResistance(uint16 rBase, uint16 adc);
	void doAdcTest();

	// fields
	const uint16 adcMax;
	const uint16 rBase;
	const uint8 pin;
	const uint32* bins;
	const uint8 binCount;

	int16 lastBin = -1;
	int16 lastStableBin = -1;
	uint16 samplesInLastBin = 0;
	uint32 lastStablePressTime = -1;
	uint32 nextHeldCallbackDuration = 0; // ms
	uint16 filteredValue = 0;

	// properties
	void (*buttonDownCallback)(const uint8 button);
	void (*buttonUpCallback)(const uint8 button, uint32 duration);
	void (*buttonHeldCallback)(const uint8 button, uint32 duration);
	Stream* debugStream = NULL;
};
