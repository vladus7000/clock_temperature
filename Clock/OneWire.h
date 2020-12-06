#ifndef OneWire_h
#define OneWire_h

#define OneWirePin 3

struct OneWireBuff{
	union {
		uint8_t DataBytes[9];
		unsigned int DataWords[4];
	};
};

OneWireBuff GlobalBuff;

inline void PinLow () {
	DDRB = DDRB | 1<<OneWirePin;
}

inline void PinRelease () {
	DDRB = DDRB & ~(1<<OneWirePin);
}

inline uint8_t PinRead () {
	return PINB>>OneWirePin & 1;
}

uint8_t OneWireSetup () {
	TCCR1 = 0<<CTC1 | 0<<PWM1A | 5<<CS10;  // CTC mode, 500kHz clock
	GTCCR = 0<<PWM1B;
}

uint8_t OneWireReset () {
	uint8_t data = 1;
	PinLow();
	_delay_us(480);
	PinRelease();
	_delay_us(70);
	data = PinRead();
	_delay_us(410);
	return data; // 0 = device present
}

void OneWireWrite (uint8_t data) {
	for (int i = 0; i<8; i++) {
		if ((data & 1) == 1)
		{
			PinLow();
			_delay_us(6);
			PinRelease();
			_delay_us(64);
		}
		else
		{
			PinLow();
			_delay_us(60);
			PinRelease();
			_delay_us(10);
		}
		data = data >> 1;
	}
}

uint8_t OneWireRead () {
	uint8_t data = 0;
	for (int i = 0; i<8; i++) {
		PinLow();
		_delay_us(6);
		PinRelease();
		_delay_us(9);
		data = data | PinRead()<<i;
		_delay_us(55);
	}
	return data;
}

void OneWireReadBytes (int bytes) {
	for (int i=0; i<bytes; i++) {
		GlobalBuff.DataBytes[i] = OneWireRead();
	}
}

uint8_t OneWireCRC (int bytes) {
	uint8_t crc = 0;
	for (int j=0; j<bytes; j++) {
		crc = crc ^ GlobalBuff.DataBytes[j];
		for (int i=0; i<8; i++) crc = crc>>1 ^ ((crc & 1) ? 0x8c : 0);
	}
	return crc;
}

#endif //OneWire_h