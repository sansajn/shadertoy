#pragma once
#include <glm/vec3.hpp>
#include "gles2/program_gles2.hpp"
#include "gles2/texture_gles2.hpp"

namespace gles2 {

struct property
{
	virtual ~property() {}
	virtual void bind(shader::program & p) = 0;
};

struct texture_property : public property
{
	texture_property(std::shared_ptr<texture2d> tex, std::string const & uname, unsigned bind_unit = 0);
	void bind(shader::program & p) override;

	std::shared_ptr<texture2d> _tex;
	std::string _uname;
	unsigned _bind_unit;
};

//! material povrchu pre phongov osvetlovaci model
struct material_property : public property
{
	material_property(glm::vec3 const & ambient = glm::vec3{.2}, float intensity = 1.0f, float shininess = 64.0f);
	void bind(shader::program & p) override;

	glm::vec3 ambient;
	float intensity;
	float shininess;
};

}  // gles2
