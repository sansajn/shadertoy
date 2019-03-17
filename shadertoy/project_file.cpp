#include <fstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "project_file.hpp"

namespace io {

using std::string;
using std::vector;
using std::ifstream;
using boost::algorithm::starts_with;
using boost::algorithm::trim;
using boost::algorithm::trim_left;
using boost::algorithm::trim_right;

project_file::project_file()
{}

bool project_file::load(std::string const & fname)
{
	ifstream fin{fname};
	if (!fin.is_open())
		return false;

	string line;
	while (getline(fin, line))
	{
		trim_left(line);
		if (line.empty() || starts_with(line, "#"))  // ignore line
			continue;

		trim_right(line);

		if (_prog.empty())
			_prog = line;
		else
			_texs.push_back(line);
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
