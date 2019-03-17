#pragma once
#include <string>
#include <vector>
#include <memory>
#include "gles2/mesh_gles2.hpp"
#include "gles2/program_gles2.hpp"
#include "gles2/property.hpp"

namespace gles2 {

class model
{
public:
	model() {}
	model(model && other);
	virtual ~model();
	void append_mesh(std::shared_ptr<mesh> m);
	void append_mesh(std::shared_ptr<mesh> m, std::vector<property *> const & mesh_props);
	void append_global(property * prop);
	virtual void render(shader::program & p) const;
	void operator=(model && other);
	void attribute_location(std::initializer_list<int> locations);

	model(model const &) = delete;
	void operator=(model const &) = delete;

private:
	using propvect = std::vector<property *>;  // predpoklada alokaciu pomocou new

	std::vector<std::shared_ptr<mesh>> _meshes;
	std::vector<propvect> _props;  //!< vlastnosti pre kazdu mriezku
	propvect _glob_props;  //!< spolocne vlastnosti pre vsetky mriezky
};

}  // gles2
