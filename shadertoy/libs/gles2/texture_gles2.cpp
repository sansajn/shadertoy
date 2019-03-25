#include <algorithm>
#include <stdexcept>
#include <cassert>
#include "gl/opengl.hpp"
#include "texture_gles2.hpp"

namespace gles2 {

using std::swap;
using std::logic_error;

static GLenum opengl_cast(pixel_type t);
static GLenum opengl_cast(pixel_format f);
static GLenum opengl_cast(texture_wrap w);
static GLenum opengl_cast(texture_filter f);

static unsigned alignment_to(unsigned width, pixel_format pfmt, pixel_type type);
static unsigned pixel_sizeof(pixel_format pfmt, pixel_type type);
static unsigned channel_count(pixel_format pfmt);


texture::parameters::parameters()
	: _min(texture_filter::nearest), _mag(texture_filter::linear)
{
	_wrap[0] = _wrap[1] = texture_wrap::clamp_to_edge;
}

texture::parameters & texture::parameters::min(texture_filter mode)
{
	_min = mode;
	return *this;
}

texture::parameters & texture::parameters::mag(texture_filter mode)
{
	_mag = mode;
	return *this;
}

texture::parameters & texture::parameters::filter(texture_filter minmag_mode)
{
	_min = _mag = minmag_mode;
	return *this;
}

texture::parameters & texture::parameters::filter(texture_filter min_mode, texture_filter mag_mode)
{
	_min = min_mode;
	_mag = mag_mode;
	return *this;
}

texture::parameters & texture::parameters::wrap_s(texture_wrap mode)
{
	_wrap[0] = mode;
	return *this;
}

texture::parameters & texture::parameters::wrap_t(texture_wrap mode)
{
	_wrap[1] = mode;
	return *this;
}

texture::texture(unsigned target, unsigned tid)
	: _tid(tid), _target(target)
{
	assert(glIsTexture(tid) && "tid is not a texture id");
}

texture::~texture()
{
	glDeleteTextures(1, &_tid);
}

texture::texture(texture && lhs)
{
	_tid = lhs._tid;
	_target = lhs._target;
	lhs._tid = 0;
}

void texture::operator=(texture && lhs)
{
	swap(_tid, lhs._tid);
	swap(_target, lhs._target);
}

void texture::bind(unsigned unit)
{
	assert(unit >= 0 && unit < 32 && "not enougth texture units");
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(_target, _tid);
}

void texture::init(parameters const & params)
{
	assert(!_tid && "texture already created");

	glGenTextures(1, &_tid);
	glBindTexture(_target, _tid);

	glTexParameteri(_target, GL_TEXTURE_WRAP_S, opengl_cast(params.wrap_s()));
	glTexParameteri(_target, GL_TEXTURE_WRAP_T, opengl_cast(params.wrap_t()));
	glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, opengl_cast(params.min()));
	glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, opengl_cast(params.mag()));

	assert(glGetError() == GL_NO_ERROR && "opengl error");
}


texture2d::texture2d()
	: _w{0}, _h{0}
{}

texture2d::texture2d(unsigned width, unsigned height, pixel_format pfmt, pixel_type type, void const * pixels, parameters const & params)
	: texture{GL_TEXTURE_2D, params}
{
	read(width, height, pfmt, type, pixels);
}

texture2d::texture2d(unsigned tid, unsigned width, unsigned height, pixel_format pfmt, pixel_type type)
	: texture{GL_TEXTURE_2D, tid}
{
	_w = width;
	_h = height;
}

texture2d::texture2d(texture2d && lhs) : texture(std::move(lhs))
{
	_w = lhs._w;
	_h = lhs._h;
}

void texture2d::operator=(texture2d && lhs)
{
	texture::operator=(std::move(lhs));
	_w = lhs._w;
	_h = lhs._h;
}

void texture2d::read(unsigned width, unsigned height, pixel_format pfmt, pixel_type type, void const * pixels)
{
	_w = width;
	_h = height;
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment_to(_w, pfmt, type));
	glTexImage2D(GL_TEXTURE_2D, 0, opengl_cast(pfmt), _w, _h, 0, opengl_cast(pfmt), opengl_cast(type), pixels);  // internal-texture-format must equal pixel-format
	// TODO: podpora mipmap ?
	assert(glGetError() == GL_NO_ERROR && "opengl error");
}


GLenum opengl_cast(pixel_type t)
{
	switch (t)
	{
		case pixel_type::ub8: return GL_UNSIGNED_BYTE;
		case pixel_type::us565: return GL_UNSIGNED_SHORT_5_6_5;
		case pixel_type::us4444: return GL_UNSIGNED_SHORT_4_4_4_4;
		case pixel_type::us5551: return GL_UNSIGNED_SHORT_5_5_5_1;
		default:
			throw cast_error{"unknown pixel type"};
	}
}

GLenum opengl_cast(pixel_format f)
{
	switch (f)
	{
		case pixel_format::rgb: return GL_RGB;
		case pixel_format::rgba: return GL_RGBA;
		case pixel_format::alpha: return GL_ALPHA;
		case pixel_format::luminance: return GL_LUMINANCE;
		case pixel_format::luminance_alpha: return GL_LUMINANCE_ALPHA;
		default:
			throw cast_error{"unknow pixel format"};
	};
}

GLenum opengl_cast(texture_wrap w)
{
	switch (w)
	{
		case texture_wrap::clamp_to_edge: return GL_CLAMP_TO_EDGE;
		case texture_wrap::mirrored_repeat: return GL_MIRRORED_REPEAT;
		case texture_wrap::repeat: return GL_REPEAT;
		default:
			throw std::exception();
	}
}

GLenum opengl_cast(texture_filter f)
{
	switch (f)
	{
		case texture_filter::nearest: return GL_NEAREST;
		case texture_filter::linear: return GL_LINEAR;
		case texture_filter::nearest_mipmap_nearest: return GL_NEAREST_MIPMAP_NEAREST;
		case texture_filter::linear_mipmap_nearest: return GL_LINEAR_MIPMAP_NEAREST;
		case texture_filter::nearest_mipmap_linear: return GL_NEAREST_MIPMAP_LINEAR;
		case texture_filter::linear_mipmap_linear: return GL_LINEAR_MIPMAP_LINEAR;
		default:
			throw std::exception();
	}
}

unsigned alignment_to(unsigned width, pixel_format pfmt, pixel_type type)
{
	if ((width % 4) == 0)
		return 4;

	unsigned rowbytes = width * pixel_sizeof(pfmt, type);

	if ((rowbytes % 4) == 0)
		return 4;
	else if ((rowbytes % 2) == 0)
		return 2;
	else
		return 1;
}

unsigned pixel_sizeof(pixel_format pfmt, pixel_type type)
{
	switch (type)
	{
		case pixel_type::ub8: return channel_count(pfmt);

		case pixel_type::us565:
		case pixel_type::us4444:
		case pixel_type::us5551: return 2;

		default:
			throw logic_error{"unknown pixel_type"};
	}
}

unsigned channel_count(pixel_format pfmt)
{
	switch (pfmt)
	{
		case pixel_format::alpha:
		case pixel_format::luminance: return 1;
		case pixel_format::luminance_alpha: return 2;
		case pixel_format::rgb: return 3;
		case pixel_format::rgba: return 4;

		default:
			throw logic_error("unknown image pixel-format");
	}
}

}  // gles2
