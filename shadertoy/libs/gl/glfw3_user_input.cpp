#include "glfw3_user_input.hpp"

namespace ui { namespace glfw3 {

using glm::vec2;

user_input::user_input()
	: _window{nullptr}
{}

user_input::user_input(GLFWwindow * window, input_mode mode)
	: _mouse{0,0}
	, _window{window}
	, _mode{mode}
	, _center{vec2{0, 0}}
{
	assert(window);

	this->mode(mode);

	for (int i = GLFW_MOUSE_BUTTON_1; i < GLFW_MOUSE_BUTTON_LAST+1; ++i)
	{
		_mouse_button[i] = GLFW_RELEASE;
		_mouse_button_up[i] = false;
	}

	for (int i = 0; i < GLFW_KEY_LAST; ++i)
	{
		_key[i] = GLFW_RELEASE;
		_key_up[i] = false;
	}
}

void user_input::input(double dt)
{
	assert(_window);

	// mouse position
	double x, y;
	glfwGetCursorPos(_window, &x, &y);

	if (_mode == input_mode::camera)
	{
		glfwSetCursorPos(_window, _center.x, _center.y);
		_mouse = vec2(x - _center.x, y - _center.y);
	}
	else
		_mouse = vec2(x, y);

	// mouse buttons
	for (int i = GLFW_MOUSE_BUTTON_1; i < GLFW_MOUSE_BUTTON_LAST+1; ++i)
	{
		int state = glfwGetMouseButton(_window, i);
		if (_mouse_button[i] == GLFW_PRESS && state == GLFW_RELEASE)
			_mouse_button_up[i] = true;
		else
			_mouse_button_up[i] = false;

		_mouse_button[i] = state;
	}

	// keyboard
	for (int i = 0; i < GLFW_KEY_LAST; ++i)
	{
		int state = glfwGetKey(_window, i);
		if (_key[i] == GLFW_PRESS && state == GLFW_RELEASE)
			_key_up[i] = true;
		else
			_key_up[i] = false;

		_key[i] = state;
	}
}

vec2 const & user_input::mouse_position() const
{
	return _mouse;
}

bool user_input::mouse(event_handler::button b) const
{
	// TODO: cast function
	int button_code;
	if (b == event_handler::button::left)
		button_code = GLFW_MOUSE_BUTTON_LEFT;
	else if (b == event_handler::button::right)
		button_code = GLFW_MOUSE_BUTTON_RIGHT;
	else if (b == event_handler::button::middle)
		button_code = GLFW_MOUSE_BUTTON_MIDDLE;
	else
		return false;  // unsupported button

	return _mouse_button[button_code] == GLFW_PRESS;
}

bool user_input::mouse_up(event_handler::button b) const
{
	int button_code;
	if (b == event_handler::button::left)
		button_code = GLFW_MOUSE_BUTTON_LEFT;
	else if (b == event_handler::button::right)
		button_code = GLFW_MOUSE_BUTTON_RIGHT;
	else if (b == event_handler::button::middle)
		button_code = GLFW_MOUSE_BUTTON_MIDDLE;
	else
		return false;  // unsupported button

	return _mouse_button_up[button_code];
}

bool user_input::mouse_wheel(event_handler::wheel w) const
{
	return false;
}

void user_input::mode(input_mode m)
{
	_mode = m;

	if (_mode == input_mode::camera)
	{
		int w = 0, h = 0;
		glfwGetWindowSize(_window, &w, &h);
		_center = vec2{w/2, h/2};

		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(_window, _center.x, _center.y);
	}
	else if (_mode == input_mode::window)
	{
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

user_input::input_mode const & user_input::mode() const
{
	return _mode;
}

bool user_input::key(unsigned char c) const
{
	return _key[c] == GLFW_PRESS;
}

bool user_input::key_up(unsigned char c) const
{
	return _key_up[c];
}

bool user_input::any_of_key(char const * keys) const
{
	while (*keys)
		if (key(*keys++))
			return true;
	return false;
}

bool user_input::any_of_key_up(char const * keys) const
{
	while (*keys)
		if (key_up(*keys++))
			return true;
	return false;
}

}}  // ui::glfw3
