#pragma once
#include <string>

namespace io {

//! read \c fname file into memory at once
std::string read_file(std::string const & fname);

}  // io
