#include "glfw3_window.hpp"
#include <GLFW/glfw3.h>

#include <iostream>

namespace ui {

using glm::ivec2;

namespace glfw_detail {

glfw3_layer * active_window();
event_handler::modifier to_modifier(int m);

glfw3_layer * __window = nullptr;
GLFWwindow * __glfw_window = nullptr;

}  // glfw_detail


glfw3_layer::glfw3_layer(parameters const & params)
{
	assert(!glfw_detail::__window && "GLFW3 already initialized");

	if (!glfw_detail::__window)
	{
		glfwInit();
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	glfw_detail::__window = this;

	GLFWwindow * window = glfwCreateWindow(params.width(), params.height(), __FILE__, NULL, NULL);
	assert(window);

	glfwMakeContextCurrent(window);
	glfw_detail::__glfw_window = window;

	_in = user_input{window};
}

glfw3_layer::~glfw3_layer()
{
	glfwTerminate();
	glfw_detail::__glfw_window = nullptr;
}

void glfw3_layer::display()
{
	glfwSwapBuffers(glfw_detail::__glfw_window);
}

void glfw3_layer::reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void glfw3_layer::main_loop_event()
{
	glfwPollEvents();
	if (glfwWindowShouldClose(glfw_detail::__glfw_window))
		close();
}

glfw3_layer::user_input & glfw3_layer::in()
{
	return _in;
}

glfw3_layer::user_input const & glfw3_layer::in() const
{
	return _in;
}

void glfw3_layer::name(std::string const & s)
{
	glfwSetWindowTitle(native_window(), s.c_str());
}


GLFWwindow * glfw3_layer::native_window() const
{
	return glfw_detail::__glfw_window;
}

}  // ui
