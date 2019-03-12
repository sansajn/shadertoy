#pragma once
#include <stdexcept>

namespace gles2 {

using cast_error = std::runtime_error;

enum class pixel_type {  //!< \sa glTexImage2D():type
	ub8,
	us565,
	us4444,
	us5551,
};

enum class pixel_format {  //!< \sa glTexImage2D():format
	alpha,
	rgb,
	rgba,
	luminance,
	luminance_alpha
};

enum class texture_wrap  //! \sa glTexParameter
{
	clamp_to_edge,
	mirrored_repeat,
	repeat
};

enum class texture_filter  //! \sa glTexParameter
{
	nearest,
	linear,
	nearest_mipmap_nearest,
	linear_mipmap_nearest,
	nearest_mipmap_linear,
	linear_mipmap_linear
};


class texture
{
public:
	struct parameters
	{
		parameters();
		parameters & min(texture_filter mode);
		parameters & mag(texture_filter mode);
		parameters & filter(texture_filter minmag_mode);
		parameters & filter(texture_filter min_mode, texture_filter mag_mode);
		parameters & wrap_s(texture_wrap mode);
		parameters & wrap_t(texture_wrap mode);

		texture_filter min() const {return _min;}
		texture_filter mag() const {return _mag;}
		texture_wrap wrap_s() const {return _wrap[0];}
		texture_wrap wrap_t() const {return _wrap[1];}

	private:
		texture_filter _min, _mag;
		texture_wrap _wrap[2];  // [s, t]
	};

	texture(texture && lhs);
	virtual ~texture();

	unsigned id() const {return _tid;}
	unsigned target() const {return _target;}
	void bind(unsigned unit);

	void operator=(texture && lhs);

	texture(texture const &) = delete;
	void operator=(texture const &) = delete;

protected:
	texture() : _tid(0) {}
	texture(unsigned target, unsigned tid);
	texture(unsigned target, parameters const & params) : _tid(0), _target(target) {init(params);}

private:
	void init(parameters const & params);

	unsigned _tid;  //!< texture identifier \sa glGenTextures()
	unsigned _target;  //!< \sa glBindTexture()
};


class texture2d : public texture
{
public:
	texture2d() {}
	texture2d(unsigned width, unsigned height, pixel_format pfmt, pixel_type type, parameters const & params = parameters()) : texture2d(width, height, pfmt, type, nullptr, params) {}
	texture2d(unsigned width, unsigned height, pixel_format pfmt, pixel_type type, void const * pixels, parameters const & params = parameters());
	texture2d(unsigned tid, unsigned width, unsigned height, pixel_format pfmt, pixel_type type);
	texture2d(texture2d && lhs);
	~texture2d() {}

	unsigned width() const {return _w;}
	unsigned height() const {return _h;}

	void operator=(texture2d && lhs);

private:
	void read(unsigned width, unsigned height, pixel_format pfmt, pixel_type type, void const * pixels);

	unsigned _w, _h;
};

}  // gles2
