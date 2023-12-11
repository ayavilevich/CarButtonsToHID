
class HID {
  public:
	static void setup();

	// actions
	// simple
	static void DoKbAltTab();
	static void DoKbRightArrow();
	static void DoKbLeftArrow();
	static void DoKbUpArrow();
	static void DoKbDownArrow();
	static void DoConsBrightnessDown();
	static void DoConsBrightnessUp();
	static void DoConsHome();
	static void DoConsBack();
	static void DoConsPlayOrPause();
	static void DoConsSelectApp();
	static void DoKbHome();
	static void DoKbBack();
	static void DoTouchSwipeLeft();
	static void DoTouchSwipeRight();
	// flows
	static void DoPowerConfigMenu();
	static void DoDvrCleanup();
	static void DoDvrToggle();
	static void DoBtMusicToggle();
	static void DoEmailToSelf();
	static void DoRadio();
	static void DoNavigation();
};
