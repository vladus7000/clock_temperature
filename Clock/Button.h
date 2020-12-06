#ifndef Button_h
#define Button_h

//Ex Button b1 = Button(PINB3, DDB3);

struct Button
{
	enum State
	{
		Up,
		FirstPressed,
		Hold,
		FirstUp,
	};

	Button()
	{}

	Button(uint8_t i, uint8_t j)
	: pinNumber(i)
	{
		DDRB &= ~(1 << j);
	}

	void update()
	{
		bool pressed = PINB & (1 << pinNumber);

		switch (state)
		{
			case Up:
			if (pressed) state = FirstPressed;
			break;
			case FirstPressed:
			if (pressed) state = Hold; else state = FirstUp;
			break;
			case Hold:
			if (pressed) state = Hold; else state = FirstUp;
			break;
			case FirstUp:
			if (pressed) state = FirstPressed; else state = Up;
			break;
			default:
			break;
		}
	}

	State state = Up;
	uint8_t pinNumber;
};

#endif //Button_h