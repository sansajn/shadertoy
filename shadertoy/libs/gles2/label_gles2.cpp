#include <vector>
#include <string>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <Magick++.h>
#include "gl/shapes.hpp"
#include "gles2/mesh_gles2.hpp"
#include "gles2/texture_gles2.hpp"
#include "label_gles2.hpp"

namespace ui {

using std::string;
using std::vector;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ortho;
using glm::translate;
using glm::scale;
using gles2::texture2d;
using gles2::mesh;
using gles2::pixel_format;
using gles2::pixel_type;

	namespace detail {

using std::min;
using std::max;

void copy(FT_BitmapGlyph glyph, FT_Vector const & pen, Magick::Image & result);
void unite(FT_BBox & result, FT_BBox const & bbox);
void adjust(FT_BBox & result, FT_Pos dx);
unsigned width(FT_BBox const & b);
unsigned height(FT_BBox const & b);
FT_BBox measure_glyphs(vector<FT_Glyph> const & glyphs);
Magick::Image render_glyphs(vector<FT_Glyph> const & glyphs);
FT_Glyph load_glyph(unsigned char_code, FT_Face face);

FT_BBox measure_glyphs(vector<FT_Glyph> const & glyphs)
{
	if (glyphs.empty())
		return FT_BBox{0, 0, 0, 0};

	FT_BBox result;
	FT_Glyph_Get_CBox(glyphs[0], FT_GLYPH_BBOX_PIXELS, &result);

	FT_Pos advance = glyphs[0]->advance.x >> 16;  // 16.16

	for (size_t i = 1; i < glyphs.size(); ++i)
	{
		FT_BBox bbox;
		FT_Glyph_Get_CBox(glyphs[i], FT_GLYPH_BBOX_PIXELS, &bbox);
		adjust(bbox, advance);

		unite(result, bbox);

		advance += glyphs[i]->advance.x >> 16;  // 16.6
	}

	return result;
}

Magick::Image render_glyphs(vector<FT_Glyph> const & glyphs)
{
	using namespace Magick;

	FT_BBox glyphs_bbox = measure_glyphs(glyphs);
	Image result{Geometry(width(glyphs_bbox) + glyphs_bbox.xMin, height(glyphs_bbox)), "black"};  // TODO: nech je pozadie transparentne
	result.depth(8);

	FT_Vector pen{0, glyphs_bbox.yMax};

	for (FT_Glyph glyph : glyphs)
	{
		FT_BitmapGlyph glyph_bitmap = (FT_BitmapGlyph)glyph;

		FT_Error err = FT_Glyph_To_Bitmap((FT_Glyph *)&glyph_bitmap, FT_RENDER_MODE_NORMAL, nullptr, 0);
		assert(!err && "unable convert a glyph object to a glyph bitmap");

		copy(glyph_bitmap, pen, result);

		pen.x += glyph_bitmap->root.advance.x >> 16;  // 16.16
	}

	return result;
}

FT_Glyph load_glyph(unsigned char_code, FT_Face face)
{
	FT_UInt glyph_index = FT_Get_Char_Index(face, char_code);
	FT_Error err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);  // now we can access face->glyph
	assert(!err && "can't load a glyph");

	FT_Glyph glyph = nullptr;
	err = FT_Get_Glyph(face->glyph, &glyph);
	assert(!err && "unable to extract a glyph object");

	return glyph;
}

void copy(FT_BitmapGlyph glyph, FT_Vector const & pen, Magick::Image & result)
{
	using namespace Magick;

	FT_Bitmap & bitmap = glyph->bitmap;
	unsigned char * bmbuf = bitmap.buffer;

	if (bmbuf)  // some glyphs has no image data (spaces)
	{
		PixelPacket * pixels = result.getPixels(pen.x + glyph->left, pen.y - glyph->top, bitmap.width, bitmap.rows);
		assert(pixels && "out of image geometry");
		for (size_t i = 0; i < (bitmap.width*bitmap.rows); ++i, ++pixels, ++bmbuf)
			*pixels = ColorGray{*bmbuf/255.0};
		result.syncPixels();
	}
}

void unite(FT_BBox & result, FT_BBox const & bbox)
{
	result.xMin = min(result.xMin, bbox.xMin);
	result.xMax = max(result.xMax, bbox.xMax);
	result.yMin = min(result.yMin, bbox.yMin);
	result.yMax = max(result.yMax, bbox.yMax);
}

void adjust(FT_BBox & result, FT_Pos dx)
{
	result.xMin += dx;
	result.xMax += dx;
}

unsigned width(FT_BBox const & b)
{
	return b.xMax - b.xMin;
}

unsigned height(FT_BBox const & b)
{
	return b.yMax - b.yMin;
}

char const * text_gles2_shader_source = R"(
	// gles2 text rendering shader (expects quad [-1,-1,1,1])
	#ifdef _VERTEX_
	attribute vec3 position;
	uniform vec4 os;  // defined as (position.xy, scale.xy), like vec4(0, 0, 1, 1)
	varying vec2 st;
	void main() {
		st = vec2(position.x, -position.y)/2.0 + 0.5;  // flip
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

	}  // detail


label::label(unsigned dpi)
	: _lib{nullptr}, _face{nullptr}, _dpi{dpi}
{}

label::~label()
{
	for (auto e : _cache)
		FT_Done_Glyph(e.second);

	FT_Done_Face(_face);
	FT_Done_FreeType(_lib);
}

void label::init(std::string const & font_file, unsigned font_size,
	glm::vec2 screen_size, glm::vec2 const & pos)
{
	assert(!_lib && !_face && "already initialized");
	_screen = screen_size;
	_pos = pos;
	_text_prog.from_memory(detail::text_gles2_shader_source);  // TODO: toto je super neefektivne (zdielaj pomedzi vsetky ortho_label)
	FT_Error err = FT_Init_FreeType(&_lib);
	assert(!err && "unable to init a free-type library");
	font(font_file, font_size);
}

void label::font(string const & font_file, unsigned size)
{
	FT_Error err = FT_New_Face(_lib, font_file.c_str(), 0, &_face);
	assert(!err && "unable to load a font face");

	unsigned font_size = size << 6;  // 26.6
	err = FT_Set_Char_Size(_face, font_size, font_size, _dpi, _dpi);
	assert(!err && "freetype set font size failed");

	if (!_text.empty())
		build_text_texture();
}

void label::text(string const & s)
{
	assert(_face && "set font first");

	if (_text == s)
		return;

	_text = s;
	build_text_texture();
}

void label::render()
{
	if (_text.empty())
		return;

	_text_prog.use();

	// text texture
	_text_prog.uniform_variable("s", 0);
	_text_tex.bind(0);

	// position and scale
	float win_w = _screen.x, win_h = _screen.y;
	vec2 offset{-1 + width()/win_w + 2*(_pos.x/win_w), 1 - height()/win_h - 2*(_pos.y/win_h)};
	vec2 scale{_text_tex.width()/win_w, _text_tex.height()/win_h};

	_text_prog.uniform_variable("os", vec4{offset.x, offset.y, scale.x, scale.y});

	static mesh m = gl::make_quad_xy<mesh>();  // TODO: zdielaj pomedzi vsetky ortho_label
	m.render();
}

void label::position(vec2 const & pos)
{
	_pos = pos;
}

void label::reshape(glm::vec2 const & screen_size)
{
	_screen = screen_size;
}

unsigned label::width() const
{
	return _text_tex.width();
}

unsigned label::height() const
{
	return _text_tex.height();
}

void label::build_text_texture()
{
	assert(!_text.empty() && "nothing to build from");

	vector<FT_Glyph> glyphs = load_glyphs(_text);
	Magick::Image im = detail::render_glyphs(glyphs);
	Magick::Blob blob;
	im.write(&blob, "GRAY");

	_text_tex = texture2d(im.columns(), im.rows(), pixel_format::luminance, pixel_type::ub8, blob.data());
}

vector<FT_Glyph> label::load_glyphs(string const & s)
{
	std::vector<FT_Glyph> result;
	result.reserve(s.size());

	for (auto char_code : s)
	{
		auto glyph_it = _cache.find(char_code);
		if (glyph_it != _cache.end())
			result.push_back(glyph_it->second);
		else
		{
			FT_Glyph glyph = detail::load_glyph(char_code, _face);
			_cache[char_code] = glyph;
			result.push_back(glyph);
		}
	}

	return result;
}

}  // ui
