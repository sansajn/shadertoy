#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "gles2/program_gles2.hpp"

template <typename T>
struct uniform_variable
{
	using value_type = T;
	using uniform_type = std::shared_ptr<gles2::shader::uniform>;

	uniform_type value_u;

	uniform_variable() {}

	uniform_variable(uniform_type u)
		: value_u{u}
	{}

	uniform_variable & operator=(T const & value) {
		if (value_u)
			*value_u = value;
		return *this;
	}
};

using int_uniform = uniform_variable<int>;
using float_uniform = uniform_variable<float>;
using vec2_uniform = uniform_variable<glm::vec2>;
using vec3_uniform = uniform_variable<glm::vec3>;
using vec4_uniform = uniform_variable<glm::vec4>;
