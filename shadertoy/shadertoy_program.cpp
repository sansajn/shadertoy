#include <regex>
#include <sstream>
#include <vector>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "shadertoy_program.hpp"

using std::string;
using std::to_string;
using std::shared_ptr;
using std::cerr;
using std::vector;
using std::istringstream;
using std::regex;
using std::regex_match;
using std::smatch;
using boost::lexical_cast;
using boost::algorithm::join;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using gles2::texture_property;

static void correct_log_line_numbers(string & log, int line_offset);

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
		uniform sampler2D iChannel0;
		uniform sampler2D iChannel1;
		uniform sampler2D iChannel2;
		uniform sampler2D iChannel3;
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
	}
	catch (gles2::shader::exception & e) {
		correct_log_line_numbers(e.error_log, -7);

		cerr << "error: " << e.what() << ", what:\n"
			<< e.error_log << std::endl;

		return false;
	}

	return true;
}

bool shadertoy_program::attach(shared_ptr<gles2::texture2d> tex)
{
	size_t idx = _textures.size();
	_textures.emplace_back(tex, "iChannel" + to_string(idx), idx);
	return true;
}

void shadertoy_program::use()
{
	_prog.use();

	_time = float_uniform{_prog.uniform_variable("iTime")};
	_resolution = vec3_uniform{_prog.uniform_variable("iResolution")};
	_frame = int_uniform{_prog.uniform_variable("iFrame")};
	_mouse = vec4_uniform{_prog.uniform_variable("iMouse")};

	// iChannelN
	for (auto & prop : _textures)
	{
		if (_prog.uniform_variable(prop._uname))
			prop.bind(_prog);
		else
			std::cerr << "warning: uniform variable '" << prop._uname << "' not used" << std::endl;
	}
}

void shadertoy_program::update(float t, glm::vec2 const & resolution, int frame, glm::vec4 const & mouse)
{
	assert(_prog.used());

	_time = t;
	_resolution = vec3{resolution, resolution.x/resolution.y};
	_frame = frame;
	_mouse = mouse;
}

void shadertoy_program::free_textures()
{
	_textures.clear();
}

void correct_log_line_numbers(string & log, int line_offset)
{
	vector<string> errors;

	// (prefix)(line)(error)
	static regex pat{R"((\d:\d+)\((\d+)\)(:.+))"};

	istringstream in{log};
	string line;
	while (getline(in, line))
	{
		smatch what;
		if (regex_match(line, what, pat))
		{
			int lineno = lexical_cast<int>(what[2]);
			errors.push_back(string{what[1]} + "(" + to_string(lineno + line_offset) + ")" + string{what[3]});
		}
	}

	log = join(errors, "\n");
}
