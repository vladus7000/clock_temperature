#include <avr/io.h>
#include <avr/delay.h>

#include "TinyWireM.h"
#include "Button.h"
#include "TM1650.h"
#include "OneWire.h"
#include "PCF8563.h"

const int ReadROM = 0x33;
const int MatchROM = 0x55;
const int SkipROM = 0xCC;
const int ConvertT = 0x44;
const int ReadScratchpad = 0xBE;

int abs (int i)
{
	return i < 0 ? -i : i;
}

TM1650 Display;
char DisplayString[] = "0000";

static int timeToBlink = 2;
static int currentTimeToBlink = timeToBlink;
static byte showDot = true;
void DrawTime()
{
	readPCF8563();
	
	DisplayString[0] = (unsigned char)hour/10 + '0';
	DisplayString[1] = (unsigned char)hour%10 + '0';
	DisplayString[2] = (unsigned char)minute/10 + '0';
	DisplayString[3] = (unsigned char)minute%10 + '0';
			
	currentTimeToBlink--;
	if (currentTimeToBlink <= 0)
	{
		currentTimeToBlink = timeToBlink;
		showDot = !showDot;
	}

	if (showDot)
	{
		DisplayString[1] |= 128;
	}
}

void DrawTemperature()
{
  	if (OneWireReset() != 0)
  	{
	  	DisplayString[0] = 'e'; DisplayString[1] = 'r'; DisplayString[2] = 'r';
  	}
  	else
	{
	  	OneWireWrite(SkipROM);
	  	OneWireWrite(ConvertT);
	  	while (OneWireRead() != 0xFF);
	  	OneWireReset();
	  	OneWireWrite(SkipROM);
	  	OneWireWrite(ReadScratchpad);
	  	OneWireReadBytes(9);

		 int rawTemp =  GlobalBuff.DataWords[0];
		 int temp = rawTemp>>4;

		 if (temp>=100)
		 {
		 	unsigned char d1 = temp/100;
		 	temp = temp - 100;

		 	unsigned char d2 = temp/10;
		 	unsigned char d3 = temp%10;
		 	DisplayString[0] = d1 + '0';
		 	DisplayString[1] = d2 + '0';
		 	DisplayString[2] = d3 + '0';
		 	DisplayString[3] = 'C';
			return;
		 }

		 bool negate = false;
		 if (temp<0) negate = true;

		 temp = abs(temp);

		 unsigned char d1 = (temp/10) % 10;
		 unsigned char d2 = temp % 10;
		 unsigned char d3 = 0;

		 if (temp>=0 && temp<100)
		 {
		 	d3 = ((rawTemp & 0x0F)*10)>>4;
		 }

		Display.clear();
		if (negate)
		{
			DisplayString[0] = '-';
			DisplayString[1] = d1 + '0';
			DisplayString[2] = d2 + '0';
			DisplayString[3] = 'C';
		}
		else
		{
		 		DisplayString[0] = d1+'0';
		 		DisplayString[1] = d2+'0';
		 		DisplayString[1] |= 128;

		 		DisplayString[2] = d3 + '0';
		 		DisplayString[3] = 'C';
		}
  	}
}


int main(void)
{
	TinyWireM.begin();
	OneWireSetup();

	second = 0;
	minute = 25;
	hour = 22;
	dayOfWeek = 7;
	dayOfMonth = 6;
	month = 12;
	year = 20;
	
	setPCF8563();
	_delay_ms(100);

	Display.init();
	_delay_ms(100);
	Display.displayOn();

	bool showTime = true;
	int timeToShow = 16;
	int currentTime = timeToShow;

	while(1)
	{
		currentTime--;
		if (currentTime <= 0)
		{
			currentTime = timeToShow;
			showTime = !showTime;
		}

		if (showTime)
		{
			DrawTime();
		}
		else
		{
			DrawTemperature();
		}

	  Display.displayString(DisplayString);	 
	  _delay_ms(200);
	 }	
}

