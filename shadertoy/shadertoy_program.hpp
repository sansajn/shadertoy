#pragma once
#include <string>
#include <memory>
#include <glm/vec2.hpp>
#include "gles2/program_gles2.hpp"
#include "gles2/texture_gles2.hpp"
#include "gles2/property.hpp"
#include "uniform_variable.hpp"

class shadertoy_program
{
public:
	shadertoy_program();
	shadertoy_program(std::string const & fname);
	bool load(std::string const & fname);
	bool attach(std::shared_ptr<gles2::texture2d> tex);
	void use();

	void update(
		float t,
		glm::vec2 const & resolution,
		int frame,
		glm::vec4 const & mouse
	);

	void free_textures();

private:
	gles2::shader::program _prog;
	float_uniform _time;
	vec3_uniform _resolution;
	int_uniform _frame;
	vec4_uniform _mouse;
	std::vector<gles2::texture_property> _textures;
};
