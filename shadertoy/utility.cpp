#include <algorithm>
#include <regex>
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <unistd.h>
#include "utility.hpp"

using std::min;
using std::string;
using std::regex;
using std::smatch;
using glm::ivec2;
namespace fs = boost::filesystem;

string const default_ubuntu_font_1804 = "/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf";
string const default_ubuntu_font_1604 = "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf";

string program_directory()
{
	ssize_t len = 1024;
	char szTmp[32];
	char pBuf[len];
	sprintf(szTmp, "/proc/%d/exe", getpid());
	ssize_t bytes = min(readlink(szTmp, pBuf, len), len - 1);
	if(bytes >= 0)
	  pBuf[bytes] = '\0';

	string result;
	result.assign(pBuf);

	result = result.substr(0, result.find_last_of("/"));  // remove the name of the executable from the end

	return result;
}

string locate_font()
{
	if (fs::exists(default_ubuntu_font_1804))
		return default_ubuntu_font_1804;
	else if (fs::exists(default_ubuntu_font_1604))
		return default_ubuntu_font_1604;
	else
		throw std::runtime_error{"unable to locate 'UbuntuMono-R.ttf' font"};
}

ivec2 parse_size(string const & size, ivec2 const & default_value)
{
	try {
		// {width}x{height}
		static regex const pat{R"((\d+)x(\d+))"};

		smatch match;
		if (regex_match(size, match, pat))
		{
			return ivec2{
				stoi(match[1].str()),
				stoi(match[2].str())
			};
		}
		else
			return default_value;
	}
	catch (std::regex_error & e) {
		std::cerr << "error: failed to parse size argument, what: " << e.what() << std::endl;
		return default_value;
	}
}
