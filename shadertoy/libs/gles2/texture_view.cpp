#include <glm/vec4.hpp>
#include "gl/shapes.hpp"
#include "gles2/mesh_gles2.hpp"
#include "gles2/texture_loader_gles2.hpp"
#include "resource_loader.hpp"
#include "texture_view.hpp"

using std::shared_ptr;
using std::string;
using glm::vec2;
using glm::vec4;
using gles2::texture_from_file;
using gles2::mesh;
using gles2::texture2d;
using gles2::shader::program;

namespace ui {

namespace detail {

static char const * ortho_texture_gles2_shader_source = R"(
	// gles2 texture rendering shader (expects quad [-1,-1,1,1])
	#ifdef _VERTEX_
	attribute vec3 position;
	uniform vec4 os;  // defined as (position.xy, scale.xy), like vec4(0, 0, 1, 1)
	varying vec2 st;
	void main() {
		st = position.xy/2.0 + 0.5;
		gl_Position = vec4(os.xy + os.zw*position.xy, 0 , 1);
	}
	#endif  // _VERTEX_
	#ifdef _FRAGMENT_
	precision mediump float;
	uniform sampler2D s;
	varying vec2 st;
	void main() {
		gl_FragColor = texture2D(s, st);
	}
	#endif  // _FRAGMENT_
)";

}  //  detail


resource_loader & ui_resource_loader()
{
	static resource_loader result;
	return result;
}


texture_view::texture_view(vec2 const & pos, vec2 const & size)
	: _pos{pos}, _size{size}
{
	_prog = ui_resource_loader().from_memory<gles2::shader::program>(
		"ortho_texture_gles2_shader",	detail::ortho_texture_gles2_shader_source);
}

void texture_view::load(string const & fname)
{
	_texture = ui_resource_loader().from_file<texture2d>(fname);
}

void texture_view::load(shared_ptr<texture2d> texture)
{
	_texture = texture;
}

void texture_view::render()
{
	if (_prog->id() == 0 || !_texture)
		return;

	// texture
	_prog->use();
	_prog->uniform_variable("s", 0);
	_texture->bind(0);

	// position and scale
	float win_w = _screen.x, win_h = _screen.y;
	vec2 offset{-1 + _size[0]/win_w + 2*(_pos.x/win_w), 1 - _size[1]/win_h - 2*(_pos.y/win_h)};
	vec2 scale{_size / vec2{win_w, win_h}};

	_prog->uniform_variable("os", vec4{offset.x, offset.y, scale.x, scale.y});

	static mesh m = gl::make_quad_xy<mesh>();
	m.render();
}

void texture_view::reshape(glm::vec2 const & screen_size)
{
	_screen = screen_size;
}

void texture_view::position(glm::vec2 const & pos)
{
	_pos = pos;
}

unsigned texture_view::width() const
{
	return _texture->width();
}

unsigned texture_view::height() const
{
	return _texture->height();
}

}  // ui
