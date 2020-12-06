#ifndef PCF8563_h
#define PCF8563_h

#define PCF8563address 0x51

byte bcdToDec(byte value)
{
	return ((value / 16) * 10 + value % 16);
}

byte decToBcd(byte value){
	return (value / 10 * 16 + value % 10);
}

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

void setPCF8563()
// this sets the time and date to the PCF8563
{
	TinyWireM.beginTransmission(PCF8563address);
	TinyWireM.write(0x02);
	TinyWireM.write(decToBcd(second));
	TinyWireM.write(decToBcd(minute));
	TinyWireM.write(decToBcd(hour));
	TinyWireM.write(decToBcd(dayOfMonth));
	TinyWireM.write(decToBcd(dayOfWeek));
	TinyWireM.write(decToBcd(month));
	TinyWireM.write(decToBcd(year));
	TinyWireM.endTransmission();
}

void readPCF8563()
// this gets the time and date from the PCF8563
{
	TinyWireM.beginTransmission(PCF8563address);
	TinyWireM.write(0x02);
	TinyWireM.endTransmission();
	TinyWireM.requestFrom(PCF8563address, 7);
	second     = bcdToDec(TinyWireM.read() & 0b01111111); // remove VL error bit
	minute     = bcdToDec(TinyWireM.read() & 0b01111111); // remove unwanted bits from MSB
	hour       = bcdToDec(TinyWireM.read() & 0b00111111);
	dayOfMonth = bcdToDec(TinyWireM.read() & 0b00111111);
	dayOfWeek  = bcdToDec(TinyWireM.read() & 0b00000111);
	month      = bcdToDec(TinyWireM.read() & 0b00011111);  // remove century bit, 1999 is over
	year       = bcdToDec(TinyWireM.read());
}

#endif //PCF8563_h