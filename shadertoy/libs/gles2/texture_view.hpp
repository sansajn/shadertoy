#pragma once
#include <map>
#include <memory>
#include <string>
#include <glm/vec2.hpp>
#include "gles2/texture_gles2.hpp"
#include "gles2/program_gles2.hpp"
#include "view.hpp"

namespace ui {

class texture_view : public view
{
public:
	texture_view(glm::vec2 const & pos = glm::vec2{0,0}, glm::vec2 const & size = glm::vec2{0,0});
	void load(std::string const & fname);
	void load(std::shared_ptr<gles2::texture2d> texture);
	void render() override;
	void reshape(glm::vec2 const & screen_size) override;
	void position(glm::vec2 const & pos);
	unsigned width() const;
	unsigned height() const;
	// is_ok, valid, bool()

private:
	glm::vec2 _pos, _size, _screen;
	std::shared_ptr<gles2::texture2d> _texture;
	std::shared_ptr<gles2::shader::program> _prog;
};

}  // ui
