#include "read_lines.hpp"

namespace io {

using std::string;
using std::ifstream;

read_lines_range::read_lines_range()
{}

read_lines_range::read_lines_range(string const & fname)
	: _fin{fname}
{
	if (!_fin.is_open())
		throw std::runtime_error{"unable to open '" + fname + "' file"};

	getline(_fin, _line);
}

bool read_lines_range::next()
{
	if (!_fin)
		return false;

	getline(_fin, _line);
	return true;
}

string const & read_lines_range::current() const
{
	return _line;
}

read_lines_range read_lines(string const & fname)
{
	return read_lines_range{fname};
}

read_lines_range::iterator read_lines_range::begin() const
{
	return iterator{const_cast<read_lines_range *>(this)};
}

read_lines_range::iterator read_lines_range::end() const
{
	return iterator{nullptr};
}

}  // io
