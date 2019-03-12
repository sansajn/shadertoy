#pragma once

#if defined(USE_GLFW3)
	#include <GLFW/glfw3.h>
#elif defined(USE_GLEW)
	#include <GL/glew.h>
#else
	#error Unsupported opengl wrapper library.
#endif
