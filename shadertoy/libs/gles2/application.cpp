#include <algorithm>
#include "application.hpp"

namespace ui {

using std::shared_ptr;
using std::find;
using std::begin;
using std::end;
using glm::vec2;

application::application(window_type::parameters const & params)
	: window_type{params}
{}

application::~application()
{}

void application::add_view(shared_ptr<ui::view> v)
{
	_views.push_back(v);
}

void application::remove_view(std::shared_ptr<ui::view> v)
{
	_views.erase(find(begin(_views), end(_views), v));
}

void application::clear_views()
{}

void application::display()
{
	// controls
	glDisable(GL_DEPTH_TEST);
	for (auto const & v : _views)
		v->render();

	ui::glfw_pool_window::display();
}

void application::reshape(int w, int h)
{
	vec2 size{w, h};
	for (auto const & v : _views)
		v->reshape(size);

	ui::glfw_pool_window::reshape(w, h);
}

}  // ui
