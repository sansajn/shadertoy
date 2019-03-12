#pragma once
#include "delayed_value.hpp"

// io

class key_press_event
{
public:
	key_press_event(int rate = 2);
	explicit operator bool() const;
	void update(float dt, bool down);

private:
	float _delay;
	bool _pressed;
	bool _prev_down;
	delayed_value<bool> _allow_key_press;
};
