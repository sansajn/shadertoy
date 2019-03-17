#pragma once
#include <string>
#include <chrono>
#include <glm/vec2.hpp>
#include "gl/glfw3_window.hpp"
#include "gles2/mesh_gles2.hpp"
#include "gles2/label_gles2.hpp"
#include "shadertoy_program.hpp"
#include "clock.hpp"
#include "delayed_value.hpp"
#include "key_press_event.hpp"

using mesh = gles2::mesh;

class shadertoy_app : public ui::glfw_pool_window
{
public:
	using base = ui::glfw_pool_window;

	shadertoy_app(glm::ivec2 const & size, std::string const & shader_fname);
	void display() override;
	void input(float dt) override;
	void update(float dt) override;
	void reshape(int w, int h) override;
	bool load_program(std::string const & fname);
	void edit_program();
	bool reload_program();

private:
	std::chrono::system_clock::time_point _t0;
	key_press_event _open_pressed, _edit_pressed, _reload_pressed,
		_help_pressed, _pause_pressed, _next_pressed;

	delayed_bool _fps_label_update;

	std::string _program_fname;
	mesh _quad;
	shadertoy_program _prog;
	ui::label _fps_label;
	bool _paused;  // step mode
	universe_clock _t;
};
