#pragma once
#include <string>

inline std::string help_keys()
{
	return std::string{
		"{keys}\n"
		"O: open shadertoy program or project file (*.stoy)\n"
		"R: reload shader program\n"
		"E: edit shader program\n"
		"P: pause/play\n"
		".: next step\n"
		"H: show this help"};
}

inline std::string help_use()
{
	return std::string{"shadertoy [options][shader_program]"};
}
