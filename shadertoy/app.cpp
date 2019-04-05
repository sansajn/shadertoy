#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "gl/shapes.hpp"
#include "gles2/texture_loader_gles2.hpp"
#include "gles2/property.hpp"
#include "utility.hpp"
#include "file_chooser_dialog.hpp"
#include "project_file.hpp"
#include "help.hpp"
#include "app.hpp"

using std::string;
using std::to_string;
using std::cout;
using std::shared_ptr;
using boost::algorithm::ends_with;
using glm::vec2;
using glm::ivec2;
using glm::vec4;
using gl::make_quad_xy;
using gles2::texture2d;
using gles2::texture_property;
using gles2::texture_from_file;
namespace fs = boost::filesystem;

template <typename GlmT>
struct label_holder
{
	label_holder(std::string const & label, GlmT const & value)
		: value{value}, label{label}
	{}

	GlmT const & value;
	std::string const & label;
};

std::ostream & operator<<(std::ostream & out, label_holder<glm::ivec2> const & x)
{
	if (!x.label.empty())
		out << x.label << "=";

	out << "(" << x.value.x << ", " << x.value.y << ")";

	return out;
}

template <typename GlmT>
label_holder<GlmT> with_label(std::string const & label, GlmT const & value)
{
	return label_holder<GlmT>{label, value};
}

shadertoy_app::shadertoy_app(ivec2 const & size, string const & shader_fname)
	: base{parameters{}.geometry(size[0], size[1])}
	, _next_pressed{10}
	, _fps_label_update{true}
	, _time_label_update{true}
	, _paused{false}
{
	_quad = make_quad_xy<mesh>(vec2{-1,-1}, 2);

	load_program(shader_fname);

	glClearColor(0,0,0,1);

	for (auto const & v : _texture_panel)
		add_view(v);
}

void shadertoy_app::update(float dt)
{
	base::update(dt);

	// code there ...
	if (_open_pressed)
	{
		cout << "open dialog ..." << std::endl;
		if (open_file_chooser_dialog(_program_fname, program_directory()))
			load_program(_program_fname);
	}

	if (_edit_pressed)
	{
		cout << "edit '" << _program_fname << "' ..." << std::endl;
		string cmd = "kate -n \"" + _program_fname + "\" &";
		system(cmd.c_str());
	}

	if (_reload_pressed)
	{
		cout << "reloading program ..." << std::endl;
		if (!_program_fname.empty())
			reload_program();
	}

	if (_help_pressed)
	{
		string cmd = "kate \"help.txt\" &";
		system(cmd.c_str());
	}

	if (_pause_pressed)
	{
		_paused = !_paused;
		if (!_paused)
		{
			_t.resume();
			cout << "resumed" << std::endl;
		}
		else
			cout << "paused" << std::endl;
	}

	if (_next_pressed)
	{
		float t_prev = _t.now();
		float t = _t.next(1.0 / _step);
		cout << "t=" << t_prev << "s -> " << t << "s" << std::endl;
	}

	float const UPDATE_DELAY = 0.25f;

	_fps_label_update.update(dt);

	if (_fps_label_update.get())
	{
		if (_fps_label)
		{
			_fps_label->text(string("fps: ") + to_string(fps()));
			_fps_label_update = delayed_bool{false, true, UPDATE_DELAY};
		}
	}

	_time_label_update.update(dt);
	if (_time_label_update.get())
	{
		if (_paused)
		{
			if (_time_label)
			{
				_time_label->text("P (1/" + to_string(_step) + "), t=" + to_string(_t.now()) + "s");
				_time_label->position(vec2{width() - 180, 2});
			}
		}
		else
		{
			if (_time_label)
			{
				_time_label->text("t=" + to_string(_t.now()) + "s");
				_time_label->position(vec2{width() - 100, 5});
			}
		}

		_time_label_update = delayed_bool{false, true, UPDATE_DELAY/2.0f};
	}
}

void shadertoy_app::input(float dt)
{
	base::input(dt);

	// code there ...

	_open_pressed.update(dt, in().key('O'));
	_edit_pressed.update(dt, in().key('E'));
	_reload_pressed.update(dt, in().key('R'));
	_help_pressed.update(dt, in().key('H'));
	_pause_pressed.update(dt, in().key('P'));
	_next_pressed.update(dt, in().key('.'));

	if (in().mouse(ui::event_handler::button::left))
	{
		if (_click_position.x + _click_position.y == 0.0)
			_click_position = in().mouse_position();

		_mouse_position = in().mouse_position();
	}
	else
		_click_position = _mouse_position = vec2{0, 0};
}

void shadertoy_app::display()
{
	auto now = std::chrono::system_clock::now();
	if (_t0 == std::chrono::system_clock::time_point{})
		_t0 = now;

	std::chrono::duration<double> dt = now - _t0;  // in s

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	float t = _paused ? _t.now() : _t.next();

	static int __frame = 1;

	_prog.use();

	_prog.update(
		t,
		vec2(framebuffer_size()),
		__frame,
		vec4{_mouse_position, _click_position});

	if (!_paused)
		++__frame;

	_quad.render();

	base::display();
}

void shadertoy_app::edit_program()
{}

bool shadertoy_app::load_program(string const & fname)
{
	remove_view(_fps_label);
	remove_view(_time_label);
	remove_view(_help_v);
	for (auto const & v : _texture_panel)
		remove_view(v);
	_texture_panel.clear();
	_textures.clear();

	if (load_shader_or_project(fname))
	{
		_fps_label.reset(new ui::label);
		_fps_label->init(locate_font(), 12, vec2{width(), height()}, vec2{2,2});

		_time_label.reset(new ui::label);
		_time_label->init(locate_font(), 12, vec2{width(), height()}, vec2{width() - 100, 5});

		add_view(_fps_label);
		add_view(_time_label);

		for (size_t i = 0; i < _textures.size(); ++i)
		{
			vec2 pos = vec2{width() - (i+1)*(64+10), height() - 64 - 10};
			shared_ptr<ui::texture_view> tex{new ui::texture_view{pos, vec2{64, 64}}};
			tex->reshape(vec2{width(), height()});
			tex->load(_textures[i]);
			_texture_panel.push_back(tex);
		}

		_prog.use();

		cout << "program '" << _program_fname << "' loaded" << std::endl;
	}
	else
		show_help();

	auto fn = fs::path{_program_fname}.filename();
	name(fn.native());

	_t.reset();

	return true;
}

bool shadertoy_app::reload_program()
{
	return load_program(_program_fname);
}

bool shadertoy_app::load_shader_or_project(std::string const & fname)
{
	if (ends_with(fname, ".stoy"))  // project file
	{
		io::project_file prj;
		if (!prj.load(fname))
			return false;
		_program_fname = prj.shader_program();

		if (!_prog.load(_program_fname))
			return false;

		// load textures there ...
		for (string const & ftex : prj.program_textures())
		{
			_textures.push_back(
				shared_ptr<texture2d>{new texture2d{texture_from_file(ftex)}});

			_prog.attach(_textures.back());
		}
	}
	else  // shader
	{
		_program_fname = fname;

		_prog.free_textures();
		if (!_prog.load(_program_fname))
			return false;
	}

	return true;
}

void shadertoy_app::show_help()
{
	_help_v.reset(new ui::text_view);
	_help_v->init(locate_font(), 10, vec2{width(), height()}, vec2{2,2});
	_help_v->text(help_use() + "\n\n" + help_keys());
	add_view(_help_v);
}

void shadertoy_app::reshape(int w, int h)
{
	assert(w > 0 && h > 0 && "invalid screen geometry");
	base::reshape(w, h);
}
