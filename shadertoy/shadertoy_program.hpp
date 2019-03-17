#pragma once
#include <string>
#include <memory>
#include <glm/vec2.hpp>
#include "gles2/program_gles2.hpp"
#include "gles2/texture_gles2.hpp"
#include "gles2/property.hpp"

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
		int frame
	);

private:
	gles2::shader::program _prog;
	std::shared_ptr<gles2::shader::uniform> _time_u, _resolution_u, _frame_u,
		_mouse_u;
	std::vector<gles2::texture_property> _textures;
};
