#pragma once
#include <GLFW/glfw3.h>
#include "gl/window.hpp"
#include "gl/glfw3_user_input.hpp"

namespace ui {

class glfw3_layer : public window_layer
{
public:
	using parameters = window_layer::parameters;
	using user_input = glfw3::user_input;

	glfw3_layer(parameters const & params);
	~glfw3_layer() override;
	void display() override;
	void reshape(int w, int h) override;
	void main_loop_event() override;
	user_input & in();
	user_input const & in() const;
	void name(std::string const & s) override;
    glm::ivec2 framebuffer_size() const override;
	GLFWwindow * native_window() const;

private:
	user_input _in;
};

using glfw_pool_window = ui::window<ui::pool_behaviour, ui::glfw3_layer>;
using glfw_window = glfw_pool_window;

}  // ui
