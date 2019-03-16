#include <chrono>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <glm/vec2.hpp>
//#include "geometry/glmprint.hpp"
#include "gl/glfw3_window.hpp"
#include "gl/shapes.hpp"
#include "gles2/mesh_gles2.hpp"
#include "gles2/label_gles2.hpp"
#include "shadertoy_program.hpp"
#include "file_chooser_dialog.hpp"
#include "delayed_value.hpp"
#include "clock.hpp"
#include "key_press_event.hpp"

using std::min;
using std::string;
using std::to_string;
using std::shared_ptr;
using std::cout;
using glm::vec2;
using mesh = gles2::mesh;
using gl::make_quad_xy;
using ui::label;
namespace fs = boost::filesystem;

string const default_shader_program = "hello.glsl";

string const default_ubuntu_font_1804 = "/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf";
string const default_ubuntu_font_1604 = "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf";

string program_directory();
string locate_font();

class shadertoy_app : public ui::glfw_pool_window
{
public:
	using base = ui::glfw_pool_window;

	shadertoy_app(string const & shader_fname);
	void display() override;
	void input(float dt) override;
	void update(float dt) override;
	void reshape(int w, int h) override;
	bool load_program(string const & fname);
	void edit_program();
	bool reload_program();

private:
	std::chrono::system_clock::time_point _t0;
	key_press_event _open_pressed, _edit_pressed, _reload_pressed,
		_help_pressed, _pause_pressed, _next_pressed;

	delayed_bool _fps_label_update;

	string _program_fname;
	mesh _quad;
	shadertoy_program _prog;
	label _fps_label;
	bool _paused;  // step mode
	universe_clock _t;
};

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

shadertoy_app::shadertoy_app(string const & shader_fname)
	 : base{parameters{}.geometry(400, 300)}
	, _next_pressed{10}
	, _fps_label_update{true}
	, _paused{false}
{
	// dump help
	cout
		<< "shadertoy [shader_program]\n"
		<< "\n"
		<< "{keys}\n"
		<< "O: open shader program\n"
		<< "R: reload shader program\n"
		<< "E: edit shader program\n"
		<< "P: pause/play\n"
		<< ".: next step\n"
		<< "H: show this help\n"
		<< std::endl;

	_quad = make_quad_xy<mesh>(vec2{-1,-1}, 2);

	load_program(shader_fname);

	_fps_label.init(locate_font(), 12, vec2{width(), height()}, vec2{2,2});

	glClearColor(0,0,0,1);

	cout << with_label("framebuffer-size", framebuffer_size()) << std::endl;
//	print_vector(framebuffer_size());// << std::endl;
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
		string cmd = "kate \"" + _program_fname + "\" &";
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
		float t = _t.next(0.1f);
		cout << "t=" << t_prev << "s -> " << t << "s" << std::endl;
	}

	float const UPDATE_DELAY = 0.25f;

	_fps_label_update.update(dt);

	if (_fps_label_update.get())
	{
		_fps_label.text(string("fps: ") + to_string(fps()));
		_fps_label_update = delayed_bool{false, true, UPDATE_DELAY};
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
	_prog.update(t, vec2(framebuffer_size()), __frame);

	if (!_paused)
		++__frame;

	_quad.render();

	// controls
	glDisable(GL_DEPTH_TEST);
	_fps_label.render();

	base::display();
}

void shadertoy_app::edit_program()
{}

bool shadertoy_app::load_program(string const & fname)
{
	_program_fname = fname;
	if (!_prog.load(_program_fname))
		return false;

	_prog.use();
	auto fn = fs::path{_program_fname}.filename();
	name(fn.native());

	cout << "program '" << _program_fname << "' loaded" << std::endl;

	return true;
}

bool shadertoy_app::reload_program()
{
	return load_program(_program_fname);
}

void shadertoy_app::reshape(int w, int h)
{
	assert(w > 0 && h > 0 && "invalid screen geometry");
	_fps_label.reshape(vec2{w, h});
	base::reshape(w, h);
}

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


int main(int argc, char * argv[])
{
	shadertoy_app app{argc > 1 ? argv[1] : default_shader_program};
	app.start();
	return 0;
}
