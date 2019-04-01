#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <glm/vec2.hpp>
#include "gl/glfw3_window.hpp"
#include "gles2/mesh_gles2.hpp"
#include "gles2/texture_gles2.hpp"
#include "gles2/application.hpp"
#include "gles2/ui/label_gles2.hpp"
#include "gles2/ui/texture_view.hpp"
#include "gles2/ui/text.hpp"
#include "shadertoy_program.hpp"
#include "clock.hpp"
#include "delayed_value.hpp"
#include "key_press_event.hpp"

using mesh = gles2::mesh;

class shadertoy_app : public ui::application
{
public:
	using base = ui::application;

	shadertoy_app(glm::ivec2 const & size, std::string const & shader_fname);
	void display() override;
	void input(float dt) override;
	void update(float dt) override;
	void reshape(int w, int h) override;
	bool load_program(std::string const & fname);
	void edit_program();
	bool reload_program();

private:
	bool load_shader_or_project(std::string const & fname);
	void show_help();

	std::chrono::system_clock::time_point _t0;
	key_press_event _open_pressed, _edit_pressed, _reload_pressed,
		_help_pressed, _pause_pressed, _next_pressed;

	delayed_bool _fps_label_update, _time_label_update;

	std::string _program_fname;
	mesh _quad;
	shadertoy_program _prog;
	std::shared_ptr<ui::label> _fps_label, _time_label;
	std::shared_ptr<ui::text_view> _help_v;
	std::vector<std::shared_ptr<ui::texture_view>> _texture_panel;
	bool _paused;  // step mode
	universe_clock _t;
	int _step = 60;  // in fps
	glm::vec2 _click_position, _mouse_position;

	// resources
	std::vector<std::shared_ptr<gles2::texture2d>> _textures;
};
