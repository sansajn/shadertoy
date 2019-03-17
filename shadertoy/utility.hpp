#pragma once
#include <string>
#include <glm/vec2.hpp>

std::string program_directory();
std::string locate_font();
glm::ivec2 parse_size(std::string const & size, glm::ivec2 const & default_value);
