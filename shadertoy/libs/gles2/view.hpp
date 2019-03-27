#pragma once
#include <glm/vec2.hpp>

namespace ui {

class view
{
public:
	virtual ~view() {}
	virtual void render() = 0;
	virtual void reshape(glm::vec2 const & screen_size) = 0;
};

}  // ui
