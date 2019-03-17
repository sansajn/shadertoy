#include <iostream>
#include <boost/program_options.hpp>
#include <glm/vec2.hpp>
#include "utility.hpp"
#include "app.hpp"

using std::cout;
using std::string;
using glm::ivec2;
namespace po = boost::program_options;

string const default_shader_program = "hello.glsl";


int main(int argc, char * argv[])
{
	po::options_description desc{"shadertoy options"};
		desc.add_options()
			("help", "produce help messages")
			("size", po::value<string>(), "set window size")
			("shader", po::value<string>(), "load program shader")
			("compile", "compile program shader only");

	po::positional_options_description pos_desc;
	pos_desc.add("shader", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		cout << desc << std::endl;
		return 1;
	}

	string shader_program = vm.count("shader") ? vm["shader"].as<string>() : default_shader_program;
	ivec2 size = parse_size(vm.count("size") ? vm["size"].as<string>() : "400x300", ivec2{400, 300});
	bool compile_only = vm.count("compile") ? true : false;

	shadertoy_app app{size, shader_program};
	if (!compile_only)
		app.start();

	return 0;
}
