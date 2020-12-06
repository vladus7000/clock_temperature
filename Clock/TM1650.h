#ifndef TM1650_h
#define TM1650_h

#define TM1650_DISPLAY_BASE 0x34 // Address of the left-most digit 0x34
#define TM1650_DCTRL_BASE   0x24  // Address of the control register of the left-most digit 0x24
#define TM1650_NUM_DIGITS   16 // max number of digits
#define TM1650_MAX_STRING   128 // number of digits

#define TM1650_BIT_ONOFF	0b00000001
#define TM1650_MSK_ONOFF	0b11111110
#define TM1650_BIT_DOT		0b00000001
#define TM1650_MSK_DOT		0b11110111
#define TM1650_BRIGHT_SHIFT	4
#define TM1650_MSK_BRIGHT	0b10001111
#define TM1650_MIN_BRIGHT	0
#define TM1650_MAX_BRIGHT	7

typedef unsigned char byte;

const unsigned char TM1650_CDigits[128] {

//0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x00
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10
  0x00, 0x82, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00, // 0x20
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53, // 0x30
  0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x06, 0x1E, 0x00, 0x38, 0x00, 0x54, 0x3F, // 0x40
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x00, 0x0F, 0x00, 0x08, // 0x50 
  0x63, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x02, 0x1E, 0x00, 0x06, 0x00, 0x54, 0x5C, // 0x60
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x30, 0x0F, 0x00, 0x00  // 0x70
};

class TM1650 {
    public:
        TM1650(unsigned int aNumDigits = 2);
        
	void	init();
	void	clear();
	void	displayOn();
	void	displayOff();
	void	displayState(bool aState);
	void	displayString(char *aString);
	int 	displayRunning(char *aString);
	int 	displayRunningShift();
	void	setBrightness(unsigned int aValue = TM1650_MAX_BRIGHT);
	void	setBrightnessGradually(unsigned int aValue = TM1650_MAX_BRIGHT);
	inline unsigned int getBrightness() { return iBrightness; };

	void	controlPosition(unsigned int aPos, byte aValue);
	void	setPosition(unsigned int aPos, byte aValue);
	void	setDot(unsigned int aPos, bool aState);
	byte	getPosition(unsigned int aPos) { return iBuffer[aPos]; };
	inline unsigned int	getNumPositions() { return iNumDigits; };

    private:
	char 	*iPosition;
	bool	iActive;
	unsigned int	iNumDigits;
	unsigned int	iBrightness;
    char	iString[TM1650_MAX_STRING+1];
    byte 	iBuffer[TM1650_NUM_DIGITS+1];
    byte 	iCtrl[TM1650_NUM_DIGITS];
};

//  ----  Implementation ----

/** Constructor, uses default values for the parameters
 * so could be called with no parameters.
 * aNumDigits - number of display digits (default = 4)
 */
TM1650::TM1650(unsigned int aNumDigits) {
	iNumDigits =  (aNumDigits > TM1650_NUM_DIGITS) ? TM1650_NUM_DIGITS : aNumDigits;
}

/** Initialization
 * initializes the driver. Turns display on, but clears all digits.
 */
void TM1650::init() {
	iPosition = NULL;
	for (int i=0; i<iNumDigits; i++) {
		iBuffer[i] = 0;
		iCtrl[i] = 0;
	}
    TinyWireM.beginTransmission(TM1650_DISPLAY_BASE);
    iActive = TinyWireM.endTransmission() == 0;
	clear();
}

/** Set brightness of all digits equally
 * aValue - brightness value with 1 being the lowest, and 7 being the brightest
 */
void TM1650::setBrightness(unsigned int aValue) {
	if (!iActive) return;

	iBrightness = (aValue > TM1650_MAX_BRIGHT) ? TM1650_MAX_BRIGHT : aValue;

	for (int i=0; i<1; i+=1) {
		TinyWireM.beginTransmission(0x24);
		iCtrl[0] = (iCtrl[0] & TM1650_MSK_BRIGHT) | ( iBrightness << TM1650_BRIGHT_SHIFT );
		TinyWireM.write((unsigned char) iCtrl[0]);
		TinyWireM.endTransmission();
	}
}

/** Set brightness of all digits equally
 * aValue - brightness value with 1 being the lowest, and 7 being the brightest
 */
void TM1650::setBrightnessGradually(unsigned int aValue) {
	if (!iActive || aValue == iBrightness) return;

	if (aValue > TM1650_MAX_BRIGHT) aValue = TM1650_MAX_BRIGHT;
	int step = (aValue < iBrightness) ? -1 : 1;
	unsigned int i = iBrightness;
	do {
		setBrightness(i);
		_delay_ms(50);
		i += step;
	} while (i!=aValue);
}

/** Turns display on or off according to aState
 */
void TM1650::displayState (bool aState)
{
  if (aState) displayOn ();
  else displayOff();
}

/** Turns the display on
 */
void TM1650::displayOn ()
// turn all digits on
{
  if (!iActive) return;
  for (int i=0; i<4; i++) {
    TinyWireM.beginTransmission(TM1650_DCTRL_BASE+i);
	iCtrl[i] = (iCtrl[i] & TM1650_MSK_ONOFF) | TM1650_BIT_DOT;
    TinyWireM.write((unsigned char) iCtrl[i]);
    TinyWireM.endTransmission();
  }
}
/** Turns the display off
 */
void TM1650::displayOff ()
// turn all digits off
{
  for (int i=0; i<4; i++) {
    TinyWireM.beginTransmission(TM1650_DCTRL_BASE+i);
	iCtrl[i] = (iCtrl[i] & TM1650_MSK_ONOFF);
    TinyWireM.write((byte) iCtrl[i]);
    TinyWireM.endTransmission();
  }
}

/** Directly write to the CONTROL register of the digital position
 * aPos = position to set the control register for
 * aValue = value to write to the position
 *
 * Internal control buffer is updated as well
 */
void TM1650::controlPosition(unsigned int aPos, byte aValue) {
	if (!iActive) return;
	if (aPos < iNumDigits) {
	    TinyWireM.beginTransmission(TM1650_DCTRL_BASE + (int) aPos);
	    iCtrl[aPos] = aValue;
		TinyWireM.write(aValue);
	    TinyWireM.endTransmission();
	}
}

/** Directly write to the digit register of the digital position
 * aPos = position to set the digit register for
 * aValue = value to write to the position
 *
 * Internal position buffer is updated as well
 */
void TM1650::setPosition(unsigned int aPos, byte aValue) {
	if (!iActive) return;
	if (aPos < iNumDigits) {
	    TinyWireM.beginTransmission(TM1650_DISPLAY_BASE + (int) aPos);
	    iBuffer[aPos] = aValue;
	    TinyWireM.write(aValue);
	    TinyWireM.endTransmission();
	}
}

/** Directly set/clear a 'dot' next to a specific position
 * aPos = position to set/clear the dot for
 * aState = display the dot if true, clear if false
 *
 * Internal buffer is updated as well
 */
void	TM1650::setDot(unsigned int aPos, bool aState) {
if (!iActive) return;
	iBuffer[aPos] = iBuffer[aPos] & 0x7F |(aState ? 0b10000000 : 0);
	setPosition(aPos, iBuffer[aPos]);
}

/** Clear all digits. Keep the display on.
 */
void TM1650::clear()
// clears all digits
{
  if (!iActive) return;
  for (int i=0; i<4; i++) {
    TinyWireM.beginTransmission(TM1650_DISPLAY_BASE+i);
 	iBuffer[i] = 0;
	TinyWireM.write((byte) 0);
   TinyWireM.endTransmission();
  }
}

/** Display string on the display 
 * aString = character array to be displayed
 *
 * Internal buffer is updated as well
 * Only first N positions of the string are displayed if
 *  the string is longer than the number of digits
 */
void TM1650::displayString(char *aString)
{
	//if (!iActive) return;
	for (int i=0; i<4; i++) {
	  byte a = ((byte) aString[i]) & 0b01111111;
	  byte dot = ((byte) aString[i]) & 0b10000000;  
	  iBuffer[i] = TM1650_CDigits[a];

	  if (a) {
	    TinyWireM.beginTransmission(TM1650_DISPLAY_BASE+i);
	    TinyWireM.write((iBuffer[i] | dot));
	    TinyWireM.endTransmission();
	  }
	  else
	    break;
	    
	}
}

/** Display string on the display in a running fashion
 * aString = character array to be displayed
 *
 * Starts with first N positions of the string.
 * Subsequent characters are displayed with 1 char shift each time displayRunningShift() is called
 *
 * returns: number of iterations remaining to display the whole string
 */
int TM1650::displayRunning(char *aString) {

	strncpy(iString, aString, TM1650_MAX_STRING+1);
	iPosition = iString;
	iString[TM1650_MAX_STRING] = '\0'; //just in case.
    	displayString(iPosition);

	int l = strlen(iPosition);
	if (l <= iNumDigits) return 0;
	return (l - iNumDigits);
}

/** Display next segment (shifting to the left) of the string set by displayRunning()
 * Starts with first N positions of the string.
 * Subsequent characters are displayed with 1 char shift each time displayRunningShift is called
 *
 * returns: number of iterations remaining to display the whole string
 */
int TM1650::displayRunningShift() {
    	if (strlen(iPosition) <= iNumDigits) return 0;
    	displayString(++iPosition);
	return (strlen(iPosition) - iNumDigits);
}

#endif //PCF8563_h