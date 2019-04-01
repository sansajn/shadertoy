#include <algorithm>
#include <sstream>
#include <glm/vec2.hpp>
#include "text.hpp"

namespace ui {

using std::max;
using std::string;
using std::vector;
using std::istringstream;
using glm::vec2;

text_view::text_view()
{}

text_view::~text_view()
{}

void text_view::init(string const & font_file, unsigned font_size,
	vec2 screen_size,	vec2 const & pos)
{
	_font_file = font_file;
	_font_size = font_size;
	_screen_size = screen_size;
	_pos = pos;
}

void text_view::text(string const & s)
{
	// clean-up
	_lines.clear();

	// parse input
	vector<string> lines;

	istringstream in{s};
	string line;
	while (getline(in, line))
		lines.push_back(line);

	// create list of labels
	_width = _height = 0;
	_lines.resize(lines.size());
	for (size_t i = 0; i < lines.size(); ++i)
	{
		_lines[i].reset(new label);
		_lines[i]->init(_font_file, _font_size, _screen_size, _pos);
		_lines[i]->text(lines[i]);
		_height += max(10u, _lines[i]->height());  // empty lines also have height
		_width = max(_lines[i]->width(), _width);
	}

	_height += 2 * max(0ul, _lines.size() - 2);

	// arrange
	position(_pos);
}

void text_view::position(vec2 const & pos)
{
	_pos = pos;

	vec2 line_pos = pos;
	for (auto & line : _lines)
	{
		line->position(line_pos);
		line_pos.y += max(10u, line->height()) + 2;
	}
}

void text_view::font(std::string const & font_file, unsigned size)
{
	_font_file = font_file;
	_font_size = size;

	for (auto & l : _lines)
		l->font(font_file, size);
}

void text_view::render()
{
	for (auto & l : _lines)
		l->render();
}

unsigned text_view::width() const
{
	return _width;
}

unsigned text_view::height() const
{
	return _height;
}

void text_view::reshape(glm::vec2 const & screen_size)
{
	_screen_size = screen_size;
}

}  // ui
