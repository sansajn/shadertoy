#pragma once
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>
#include "gl/window.hpp"

namespace ui { namespace glfw3 {

class user_input  //!< mouse, keyboard and touch user input
{
public:
	enum class input_mode
	{
		camera, window
	};

	user_input();
	user_input(GLFWwindow * window, input_mode mode = input_mode::window);

	// mouse
	glm::vec2 const & mouse_position() const;
	bool mouse(event_handler::button b) const;
	bool mouse_up(event_handler::button b) const;
	bool mouse_wheel(event_handler::wheel w) const;
	void mode(input_mode m);
	input_mode const & mode() const;

	// keyboard
	bool key(unsigned char c) const;
	bool key_up(unsigned char c) const;
	bool any_of_key(char const * keys) const;
	bool any_of_key_up(char const * keys) const;

	// touch ...

	void input(double dt);

private:
	glm::vec2 _mouse;
	int _mouse_button[GLFW_MOUSE_BUTTON_LAST+1];  // GLFW_MOUSE_BUTTON_LEFT|RIGHT|MIDDLE
	bool _mouse_button_up[GLFW_MOUSE_BUTTON_LAST+1];
	int _key[GLFW_KEY_LAST];  // GLFW_PRESS|GLFW_RELEASE
	bool _key_up[GLFW_KEY_LAST];
	GLFWwindow * _window;
	input_mode _mode;
	glm::vec2 _center;
};

}}  // ui::glfw3
