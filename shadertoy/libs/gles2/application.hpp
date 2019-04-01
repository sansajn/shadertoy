#pragma once
#include <memory>
#include <vector>
#include "gl/glfw3_window.hpp"
#include "gles2/ui/view.hpp"

namespace ui {

class application : public ui::glfw_pool_window
{
public:
	using window_type = ui::glfw_pool_window;

	application(window_type::parameters const & params);
	virtual ~application();
	virtual void display();
	virtual void reshape(int w, int h);
	void add_view(std::shared_ptr<ui::view> v);
	void remove_view(std::shared_ptr<ui::view> v);
	void clear_views();

private:
	std::vector<std::shared_ptr<ui::view>> _views;
};

}  // ui
