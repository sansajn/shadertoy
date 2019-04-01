#pragma once
#include <string>
#include <vector>
#include <map>
#include <glm/matrix.hpp>
#include <ft2build.h>
#include FT_GLYPH_H
#include FT_FREETYPE_H
#include "gles2/program_gles2.hpp"
#include "gles2/texture_gles2.hpp"
#include "gles2/ui/view.hpp"

namespace ui {

/*! Renderovanie text-u.

Trieda ortho_label adresuje text takymto sposobom

	(0,0)
		+-----+
		|     |
		+-----+
			(800,600)

\code
label lbl;
lbl.init(font_path, 16, vec2{800,600}, vec2{0,0});
lbl.text("hello!");
// ...
lbl.render();
\endcode */
class label : public view
{
public:
	label(unsigned dpi = 96);
	~label();

	void init(std::string const & font_file, unsigned font_size, glm::vec2 screen_size,
		glm::vec2 const & pos);

	void text(std::string const & s);
	void position(glm::vec2 const & pos);
	void font(std::string const & font_file, unsigned size);
	void render();
	unsigned width() const;
	unsigned height() const;
	void reshape(glm::vec2 const & screen_size);

private:
	void build_text_texture();
	std::vector<FT_Glyph> load_glyphs(std::string const & s);

	std::string _text;
	glm::vec2 _pos;
	gles2::texture2d _text_tex;
	gles2::shader::program _text_prog;  // TODO: share program
	glm::vec2 _screen;
	FT_Library _lib;
	FT_Face _face;
	std::map<unsigned, FT_Glyph> _cache;  // TODO: cache should be shared
	unsigned const _dpi;
};

}  // ui
