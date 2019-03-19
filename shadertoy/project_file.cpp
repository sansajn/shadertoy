#include <fstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "file_view/read_lines.hpp"
#include "project_file.hpp"

namespace io {

using std::string;
using std::vector;
using std::ifstream;
using boost::algorithm::starts_with;
using boost::algorithm::trim;
using boost::algorithm::trim_left_copy;
using boost::algorithm::trim_right;

project_file::project_file()
{}

bool project_file::load(std::string const & fname)
{
	for (string const & line : io::read_lines(fname))
	{
		if (line.empty())
			continue;

		string resource = trim_left_copy(line);
		if (resource.empty() || starts_with(resource, "#"))  // ignore line
			continue;

		trim_right(resource);

		if (_prog.empty())
			_prog = line;
		else
			_texs.push_back(resource);
	}

	return !_prog.empty();
}

string const & project_file::shader_program() const
{
	return _prog;
}

vector<string> const & project_file::program_textures() const
{
	return _texs;
}

}  // io
