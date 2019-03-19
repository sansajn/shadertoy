#include <fstream>
#include <sstream>
#include "read_file.hpp"

using std::string;
using std::ifstream;
using std::stringstream;


namespace io {

string read_file(string const & fname)
{
	ifstream in(fname);
	if (!in.is_open())
		throw std::runtime_error("can't open '" + fname + "' file");

	stringstream ss;
	ss << in.rdbuf();
	in.close();
	return ss.str();
}

}  // io