#include "property.hpp"

namespace gles2 {

using std::string;
using std::shared_ptr;
using glm::vec3;
using shader::program;

texture_property::texture_property(shared_ptr<texture2d> tex, string const & uname, unsigned bind_unit)
	: _tex{tex}, _uname{uname}, _bind_unit{bind_unit}
{}

void texture_property::bind(program & p)
{
	_tex->bind(_bind_unit);
	p.uniform_variable(_uname, (int)_bind_unit);
}

material_property::material_property(vec3 const & ambient, float intensity, float shininess)
	: ambient{ambient}, intensity{intensity}, shininess{shininess}
{}

void material_property::bind(program & p)
{
	p.uniform_variable("material_ambient", ambient);
	p.uniform_variable("material_intensity", intensity);
	p.uniform_variable("material_shininess", shininess);
}

}  // gles2
