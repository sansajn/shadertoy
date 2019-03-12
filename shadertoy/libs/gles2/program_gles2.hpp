#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <boost/range/adaptor/filtered.hpp>
#include "gl/opengl.hpp"

namespace gles2 {	namespace shader {

class program;

struct exception : public std::runtime_error
{
	exception(std::string const & msg) : std::runtime_error(msg) {}
};

/*! \note Ak je uniform vytvoreny pomocou prazdneho konstruktora, jeho pouzitie
sposoby 'nedefinovane spravanie'. Vytvor ho volanim program::uniform_variable().
\code
// priradenie pola
vector<int> data{1,2,3,4,5};
u = data;
\endcode */
class uniform
{
public:
	uniform() {}
	uniform(int loc, program * prog) : _loc(loc), _prog(prog) {}

	template <typename T>
	uniform & operator=(T const & v);

	template <typename T>
	operator T();

private:
	int _loc;
	program * _prog;
};

namespace detail {

struct valid_shader_pred
{
	bool operator()(unsigned const & v) const {return v > 0;}
};

}  // detail

/*! modul je glsl program obsahujuci vertex aj shader podprogram
\code
#ifdef _VERTEX_
uniform mat4 local_to_screen;
attribute vec3 position;
void main()	{
	gl_Position = local_to_screen * vec4(position,1);
}
#endif
#ifdef _FRAGMENT_
precision mediump float;
uniform vec3 color;  // vec3(.7);
void main() {
	gl_FragColor = vec4(color, 1);
}
#endif
\endcode */
class module
{
public:
	enum class shader_type
	{
		vertex,
		fragment,
		number_of_types
	};

	module();
	module(std::string const & fname, unsigned version = 100);
	~module();

	void from_file(std::string const & fname, unsigned version = 100);
	void from_memory(std::string const & source, unsigned version = 100);

	boost::filtered_range<detail::valid_shader_pred, const unsigned[int(shader_type::number_of_types)]>
		ids() const;

	module(module &) = delete;
	module & operator=(module &) = delete;

private:
	void compile(unsigned version, std::string const & code, shader_type type);
	void compile_check(unsigned sid, shader_type type);
	void empty_ids();

	unsigned _ids[int(shader_type::number_of_types)];  //!< (vertex, fragment, geometry) shader id

	// debug
	std::string _fname;
};

class program
{
public:
	program();
	program(std::string const & fname, unsigned version = 100);
	program(std::shared_ptr<module> m);
	~program();

	void from_file(std::string const & fname, unsigned version = 100);
	void from_memory(std::string const & source, unsigned version = 100);

	void attach(std::shared_ptr<module> m);
	void attach(std::vector<std::shared_ptr<module>> const & mods);

	int id() const {return _pid;}
	void use();
	bool used() const;

	int attribute_location(char const * name) const;
	std::shared_ptr<uniform> uniform_variable(std::string const & name);

	template <typename T>
	void uniform_variable(std::string const & name, T const & v);

	void free();

	program(program &) = delete;
	void operator=(program &) = delete;

private:
	void create_program_lazy();
	void init_uniforms();
	void append_uniform(std::string const & name, int index);
	void link();
	bool link_check();

	unsigned _pid;  //!< progrm id
	std::vector<std::shared_ptr<module>> _modules;
	std::map<std::string, std::shared_ptr<uniform>> _uniforms;

	static program * _CURRENT;
};

template <typename T>
void set_uniform(int location, T const & v);

template <typename T>
void set_uniform(int location, T const * a, int n);

template <typename T>
void set_uniform(int location, std::vector<T> const & arr)
{
	set_uniform(location, arr.data(), arr.size());
}

template <typename T>
void set_uniform(int location, std::pair<T *, int> const & a)  //!< for array where pair::first is array pointer and pair::second is number of array elements
{
	set_uniform(location, a.first, a.second);
}

template <typename T>
void get_uniform(unsigned program, int location, T & v);

std::string read_file(std::string const & fname);


namespace detail {

void gl_error_check();

}

template <typename T>
uniform & uniform::operator=(T const & v)
{
	assert(_prog->used() && "pokusam sa nastavit uniform neaktivneho programu");
	set_uniform(_loc, v);	
	assert(glGetError() == GL_NO_ERROR && "opengl error");
	return *this;
}

template <typename T>
uniform::operator T()
{
	T v;
	get_uniform(_prog->id(), _loc, v);
	return v;
}

template <typename T>
void program::uniform_variable(std::string const & name, T const & v)
{
	auto u = uniform_variable(name);
	*u = v;
}

	}  // shader
}  // gles2
