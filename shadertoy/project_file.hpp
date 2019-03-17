#pragma once
#include <string>
#include <vector>

namespace io {

//! shadertoy project file
class project_file
{
public:
	project_file();
	bool load(std::string const & fname);
	std::string const & shader_program() const;
	std::vector<std::string> const & program_textures() const;

private:
	std::string _prog;
	std::vector<std::string> _texs;
};

}  // io
