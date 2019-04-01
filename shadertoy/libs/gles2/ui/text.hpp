#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include "gles2/ui/label_gles2.hpp"

namespace ui {

class text_view : public view
{
public:
	text_view();
	~text_view();

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
	std::vector<std::shared_ptr<label>> _lines;
	glm::vec2 _pos;
	std::string _font_file;
	unsigned _font_size;
	glm::vec2 _screen_size;
	unsigned _width, _height;
};

}  // ui
