#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <boost/format.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "program_gles2.hpp"

using std::string;
using std::ifstream;
using std::stringstream;
using std::shared_ptr;
using std::make_shared;

namespace gles2 {
	namespace shader {

char const * LOG_TAG = "gles2::program";

void dump_compile_log(GLuint shader, std::string const & name);
void dump_link_log(GLuint program, std::string const & name);
string to_string(module::shader_type type);

program * program::_CURRENT = nullptr;

program::program() : _pid(0)
{}

program::program(string const & fname, unsigned version) : _pid(0)
{
	attach(shared_ptr<module>(new module(fname, version)));
}

program::program(shared_ptr<module> m) : _pid(0)
{
	attach(m);
}

void program::from_file(std::string const & fname, unsigned version)
{
	attach(shared_ptr<module>(new module{fname, version}));
}

void program::from_memory(std::string const & source, unsigned version)
{
	shared_ptr<module> m = make_shared<module>();
	m->from_memory(source, version);
	attach(m);
}

void program::attach(std::shared_ptr<module> m)
{
	create_program_lazy();

	for (unsigned sid : m->ids())
		glAttachShader(_pid, sid);

	link();
	init_uniforms();

	_modules.push_back(m);

	assert(glGetError() == GL_NO_ERROR && "opengl error");
}

void program::attach(std::vector<std::shared_ptr<module>> const & mods)
{
	create_program_lazy();

	for (auto m : mods)
	{
		for (unsigned sid : m->ids())
			glAttachShader(_pid, sid);
	}

	link();
	init_uniforms();

	for (auto m : mods)
		_modules.push_back(m);

	assert(glGetError() == GL_NO_ERROR && "opengl error");
}

void program::create_program_lazy()
{
	if (_pid == 0)
		_pid = glCreateProgram();
	assert(_pid > 0 && "unable to create a program");
}

void program::link()
{
	glLinkProgram(_pid);

	bool result = link_check();
	assert(result && "program linking failed");
	if (!result)
		throw exception("unable to link a program");
}

program::~program()
{
	free();
}

void program::use()
{
	if (_CURRENT == this)
		return;

	glUseProgram(_pid);
	_CURRENT = this;
}

bool program::used() const
{
	return _CURRENT == this;
}

int program::attribute_location(char const * name) const
{
	return glGetAttribLocation(_pid, name);
}

shared_ptr<uniform> program::uniform_variable(string const & name)
{
	auto it = _uniforms.find(name);

	if (it == _uniforms.end())
		return shared_ptr<uniform>{};

	assert(it != _uniforms.end() && "unknown uniform variable (or not active)");

	return it->second;
}

void program::free()
{
	if (used())
	{
		glUseProgram(0);
		_CURRENT = nullptr;
	}

	_uniforms.clear();
	_modules.clear();

	glDeleteProgram(_pid);
	_pid = 0;
}

void program::init_uniforms()
{
	GLint max_length = 0;
	glGetProgramiv(_pid, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);

	std::unique_ptr<GLchar[]> buf{new GLchar[max_length]};

	GLint nuniform = 0;
	glGetProgramiv(_pid, GL_ACTIVE_UNIFORMS, &nuniform);
	for (GLuint i = 0; i < (GLuint)nuniform; ++i)
	{
		GLint size;
		GLsizei length;
		GLenum type;
		glGetActiveUniform(_pid, i, max_length, &length, &size, &type, buf.get());

		string uname(buf.get());
		GLint location = glGetUniformLocation(_pid, uname.c_str());
		assert(location != -1 && "unknown uniform");
		if (size > 1 && uname.find_first_of('[') != string::npos)  // if array removes [0]
			uname = uname.substr(0, uname.find_first_of('['));

		append_uniform(uname, location);
	}

	assert(glGetError() == GL_NO_ERROR);
}

void program::append_uniform(std::string const & name, int index)
{
	_uniforms[name] = make_shared<uniform>(index, this);
}

bool program::link_check()
{
	GLint linked;
	glGetProgramiv(_pid, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE)
		dump_link_log(_pid, "<unspecified>");
	return linked != GL_FALSE;
}

namespace detail {

void gl_error_check()
{
#ifndef NDEBUG
	switch (glGetError())
	{
		case GL_NO_ERROR: return;
		case GL_INVALID_ENUM: throw exception{"glGetError(): invalid enum"};
		case GL_INVALID_VALUE: throw exception{"glGetError(): invalid value"};
		case GL_INVALID_OPERATION: throw exception{"glGetError(): invalid operation"};
		case GL_INVALID_FRAMEBUFFER_OPERATION: throw exception{"glGetError(): invalid framebuffer operation"};
		case GL_OUT_OF_MEMORY: throw exception{"glGetError(): out of memory"};
		default: throw exception{"glGetError(): unknown error"};
	}
#endif
}

}

module::module()
{
	empty_ids();
}

module::module(string const & fname, unsigned version)
{
	empty_ids();
	from_file(fname, version);
}

void module::from_file(string const & fname, unsigned version)
{
	_fname = fname;
	string source = read_file(fname);
	from_memory(source, version);
}

void module::from_memory(string const & source, unsigned version)
{
	if (source.find("_VERTEX_") != string::npos)
		compile(version, source, shader_type::vertex);

	if (source.find("_FRAGMENT_") != string::npos)
		compile(version, source, shader_type::fragment);

	if (_ids[(int)shader_type::vertex] == 0 && _ids[(int)shader_type::fragment] == 0)
		throw exception("empty shader module");
}

module::~module()
{
	for (unsigned sid : _ids)
	{
		if (sid > 0)
			glDeleteShader(sid);
	}
}

boost::filtered_range<detail::valid_shader_pred, const unsigned[int(module::shader_type::number_of_types)]> module::ids() const
{
	detail::valid_shader_pred pred;
	return boost::filtered_range<detail::valid_shader_pred, const unsigned[int(shader_type::number_of_types)]>(pred, _ids);
}

void module::compile(unsigned version, std::string const & code, shader_type type)
{
	char const * lines[3];
	string vstr = string("#version ") + std::to_string(version) + string("\n");
	lines[0] = vstr.c_str();

	unsigned sid;
	GLenum stype;

	switch (type)
	{
		case shader_type::vertex:
			sid = 0;
			lines[1] = "#define _VERTEX_\n";
			stype = GL_VERTEX_SHADER;
			break;

		case shader_type::fragment:
			sid = 1;
			lines[1] = "#define _FRAGMENT_\n";
			stype = GL_FRAGMENT_SHADER;
			break;

		default:
			assert(0 && "unsupported shader program type");
	}

	lines[2] = code.c_str();

	_ids[sid] = glCreateShader(stype);
	glShaderSource(_ids[sid], 3, lines, nullptr);
	glCompileShader(_ids[sid]);

	compile_check(_ids[sid], type);
}

void module::compile_check(unsigned sid, shader_type type)
{
	string name = _fname + string(":") + to_string(type);

	GLint compiled;
	glGetShaderiv(sid, GL_COMPILE_STATUS, &compiled);

	if (compiled == GL_FALSE)  // TODO: zobrazit warningi ?
	{
		dump_compile_log(sid, name);
		throw exception("program shader compilation failed");
	}

	assert(glGetError() == GL_NO_ERROR && "opengl error");
}

void module::empty_ids()
{
	for (unsigned & id : _ids)
		id = 0;
}

void dump_compile_log(GLuint shader, std::string const & name)
{
	GLint len;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	std::string log;
	log.resize(len);
	glGetShaderInfoLog(shader, len, nullptr, (GLchar *)log.data());
	std::cerr << "compile output ('" << name << "'):\n" << log << std::endl;
}

void dump_link_log(GLuint program, std::string const & name)
{
	GLint len;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	std::string log;
	log.resize(len);
	glGetProgramInfoLog(program, len, nullptr, (GLchar *)log.data());
	std::cerr << "link output ('" << name << "'):\n" << log << std::endl;
}

string read_file(string const & fname)
{
	ifstream in(fname);
	if (!in.is_open())
		throw std::runtime_error(boost::str(boost::format("can't open '%1%' file") % fname));

	stringstream ss;
	ss << in.rdbuf();
	in.close();
	return ss.str();
}

string to_string(module::shader_type type)
{
	switch (type)
	{
		case module::shader_type::vertex: return "vertex";
		case module::shader_type::fragment: return "fragment";
		default:
			throw exception("unsupported shader program type");
	}
}

template <>
void set_uniform<glm::mat3>(int location, glm::mat3 const & v)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(v));
}

template <>
void set_uniform<glm::mat4>(int location, glm::mat4 const & v)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));
}

template <>
void set_uniform<glm::vec2>(int location, glm::vec2 const & v)
{
	glUniform2fv(location, 1, glm::value_ptr(v));
}

template <>
void set_uniform<glm::vec3>(int location, glm::vec3 const & v)
{
	glUniform3fv(location, 1, glm::value_ptr(v));
}

template <>
void set_uniform<glm::vec4>(int location, glm::vec4 const & v)
{
	glUniform4fv(location, 1, glm::value_ptr(v));
}

template <>
void set_uniform<glm::ivec2>(int location, glm::ivec2 const & v)
{
	glUniform2iv(location, 1, glm::value_ptr(v));
}

template <>
void set_uniform<glm::ivec3>(int location, glm::ivec3 const & v)
{
	glUniform3iv(location, 1, glm::value_ptr(v));
}

template <>
void set_uniform<glm::ivec4>(int location, glm::ivec4 const & v)
{
	glUniform4iv(location, 1, glm::value_ptr(v));
}

template <>
void set_uniform<int>(int location, int const & v)
{
	glUniform1i(location, v);
}

template <>
void set_uniform<unsigned>(int location, unsigned const & v)
{
//	glUniform1ui(location, v);
	throw std::logic_error{"unsigned uniform is not supported in opengl es 2"};
}

template<>
void set_uniform<float>(int location, float const & v)
{
	glUniform1f(location, v);
}

template <>  // pre pole float-ou
void set_uniform<float>(int location, float const * a, int n)
{
	glUniform1fv(location, n, a);
}

template <>  // pre pole vec4 vektorov
void set_uniform<glm::vec4>(int location, glm::vec4 const * a, int n)
{
	glUniform4fv(location, n, glm::value_ptr(*a));
}

template <>  // pre pole matic
void set_uniform<glm::mat4>(int location, glm::mat4 const * a, int n)
{
	glUniformMatrix4fv(location, n, GL_FALSE, glm::value_ptr(*a));
}

template <>
void get_uniform(unsigned program, int location, int & v)
{
	glGetUniformiv(program, location, &v);
}

	}  // shader
}  // gles2
