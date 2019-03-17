#include <algorithm>
#include "model_gles2.hpp"

namespace gles2 {

using std::vector;
using std::swap;
using std::move;
using std::shared_ptr;
using std::string;
using shader::program;


model::model(model && other)
	: _meshes{move(other._meshes)}, _props{move(other._props)}, _glob_props{move(other._glob_props)}
{}

void model::operator=(model && other)
{
	swap(_meshes, other._meshes);
	swap(_props, other._props);
	swap(_glob_props, other._glob_props);
}

model::~model()
{
	for (vector<property *> & prop_list : _props)
		for (property *& prop : prop_list)
			delete prop;

	for (property * prop : _glob_props)
		delete prop;
}

void model::append_mesh(shared_ptr<mesh> m)
{
	append_mesh(m, vector<property *>{});
}

void model::append_mesh(shared_ptr<mesh> m, vector<property *> const & mesh_props)
{
	_meshes.push_back(m);
	_props.push_back(mesh_props);
}

void model::append_global(property * prop)
{
	_glob_props.push_back(prop);
}

void model::render(program & p) const
{
	for (size_t i = 0; i < _meshes.size(); ++i)
	{
		for (property * prop : _glob_props)
			prop->bind(p);

		for (property * prop : _props[i])
			prop->bind(p);

		_meshes[i]->render();
	}
}

void model::attribute_location(std::initializer_list<int> locations)
{
	for (auto m : _meshes)
		m->attribute_location(locations);
}

}  // gles2
