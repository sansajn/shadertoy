#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "shadertoy_program.hpp"

using std::string;
using glm::vec2;
using glm::vec3;
using glm::vec4;

shadertoy_program::shadertoy_program()
{}

shadertoy_program::shadertoy_program(string const & fname)
{
	load(fname);
}

bool shadertoy_program::load(string const & fname)
{
	string prolog = R"(
		#ifdef _VERTEX_
		attribute vec3 position;
		void main() {
			gl_Position = vec4(position, 1);
		}
		#endif  // _VERTEX_
		#ifdef _FRAGMENT_
		precision mediump float;
		uniform float iTime;
		uniform vec3 iResolution;
		uniform int iFrame;
		uniform vec4 iMouse;
	)";

	string mainImage = gles2::shader::read_file(fname);

	string epilog = R"(
		void main() {
			mainImage(gl_FragColor, gl_FragCoord.xy);
		}
		#endif
	)";

	try {
		_prog.free();
		_prog.from_memory(prolog + mainImage + epilog, 100);
	} catch (std::exception e) {
		return false;
	}

	return true;
}

void shadertoy_program::use()
{
	_prog.use();
	_time_u = _prog.uniform_variable("iTime");
	_resolution_u = _prog.uniform_variable("iResolution");
	_frame_u = _prog.uniform_variable("iFrame");
	_mouse_u = _prog.uniform_variable("iMouse");
}

void shadertoy_program::update(float t, glm::vec2 const & resolution, int frame)
{
	assert(_prog.used());

	if (_time_u)
		*_time_u = t;

	if (_resolution_u)
        *_resolution_u = vec3{resolution, resolution.x/resolution.y};

	if (_frame_u)
		*_frame_u = frame;

	if (_mouse_u)
		*_mouse_u = vec4{0, 0, 1.0f, 1.0f};
}
