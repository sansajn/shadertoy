#pragma once
#include <string>
#include <memory>
#include <glm/vec2.hpp>
#include "gles2/program_gles2.hpp"

class shadertoy_program
{
public:
	shadertoy_program();
	shadertoy_program(std::string const & fname);
	bool load(std::string const & fname);
	void use();

	void update(
		float t,
		glm::vec2 const & resolution,
		int frame
	);

private:
	std::shared_ptr<gles2::shader::uniform> _time_u, _resolution_u, _frame_u,
		_mouse_u;
	gles2::shader::program _prog;
};
