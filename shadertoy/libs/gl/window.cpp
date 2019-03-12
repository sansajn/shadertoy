#include <utility>
#include "gl/window.hpp"

namespace ui {

using std::make_pair;


namespace touch {

bool finger::down() const
{
	return state & (int)event_handler::action::down;
}

bool finger::move() const
{
	return state & (int)event_handler::action::move;
}

bool finger::up() const
{
	return state & (int)event_handler::action::up;
}

bool finger::cancel() const
{
	return state & (int)event_handler::action::cancel;
}

}  // touch


window_layer::parameters & window_layer::parameters::geometry(unsigned w, unsigned h)
{
	_w = w;
	_h = h;
	return *this;
}

window_layer::parameters & window_layer::parameters::name(std::string const & s)
{
	_name = s;
	return *this;
}

window_layer::parameters & window_layer::parameters::debug(bool d)
{
	_debug = d;
	return *this;
}

window_layer::parameters & window_layer::parameters::version(int major, int minor)
{
	_version = make_pair(major, minor);
	return *this;
}


window_layer::parameters::parameters()
{
	_w = 800;
	_h = 600;
	_name = "window";
	_debug = true;
	_version = make_pair(0, 0);
}

}   // ui
