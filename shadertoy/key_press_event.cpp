#include "key_press_event.hpp"

key_press_event::key_press_event(int rate)
	: _delay{1.0f/rate}
	, _pressed{false}
	, _prev_down{false}
	, _allow_key_press{true}
{}

key_press_event::operator bool() const
{
	return _pressed;
}

void key_press_event::update(float dt, bool down)
{
	_allow_key_press.update(dt);

	bool pressed = !_prev_down && down;
	if (_allow_key_press.get() && pressed)
	{
		_pressed = true;
		_allow_key_press = delayed_value<bool>{false, true, _delay};
	}
	else
		_pressed = false;

	_prev_down = down;
}
