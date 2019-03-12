#pragma once
#include <chrono>
#include <string>
#include <tuple>
#include <cassert>
#include <glm/vec2.hpp>

namespace ui {

template <template<class> class B, typename L>  // B : behaviour, L : layer
class window : public B<L>
{
public:
	using base = B<L>;
	using parameters = typename L::parameters;

	window(parameters const & params = parameters{});
	void reshape(int w, int h) override;
	unsigned width() const {return _w;}
	unsigned height() const {return _h;}
	float aspect_ratio() const {return (float)_w/(float)_h;}
	glm::ivec2 center() const;
	void bind_as_render_target();

protected:
	void geometry(unsigned w, unsigned h);  //!< sets window geometry

private:
	unsigned _w, _h;
};


class event_handler  //!< user input and window events
{
public:
	enum class button  //!< mouse buttons
	{
		left,
		middle,
		right,
		wheel_up,
		wheel_down,
		number_of_buttons
	};

	enum class state  //!< button states
	{
		down, up
	};

	enum class wheel
	{
		up, down
	};

	enum modifier  //!< key modifiers \note can be combination of modifiers
	{
		none = 0,
		shift = 1,
		ctrl = 2,
		alt = 4
	};

	enum class key  //!< special keys
	{
		caps_lock,
		f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
		print_screen,
		scroll_lock,
		pause,
		insert,
		home,
		page_up,
		end,
		page_down,
		right,
		left,
		down,
		up,
		unknown
	};

	enum class action  //!< touch action
	{
		down = 1,
		move = 2,
		up = 4,
		cancel = 8
	};

	virtual void display() {}
	virtual void reshape(int w, int h) {}
	virtual void idle() {}
	virtual void close() {}  //!< volane, pri zavreti okna
	virtual void mouse_click(button b, state s, modifier m, int x, int y) {}
	virtual void mouse_motion(int x, int y) {}
	virtual void mouse_passive_motion(int x, int y) {}
	virtual void mouse_wheel(wheel w, modifier m, int x, int y) {}
	virtual void key_typed(unsigned char c, modifier m, int x, int y) {}
	virtual void key_released(unsigned char c, modifier m, int x, int y) {}
	virtual void special_key(key k, modifier m, int x, int y) {}
	virtual void special_key_released(key k, modifier m, int x, int y) {}
	virtual void touch_performed(int x, int y, int finger_id, action a) {}
};  // event_handler


namespace touch {  //!< user-input-touch support

struct finger
{
	glm::ivec2 position;
	int id;
	int state;  //!< \note kombinacia hodnot s event_handler::action (down, move, up a cancel)

	bool down() const;
	bool move() const;
	bool up() const;
	bool cancel() const;
};  // finger

}  // touch


//! abstrakcia okennej vrstvy \sa glut_layer
class window_layer : public event_handler
{
public:
	class parameters
	{
	public:
		using self = parameters;

		parameters();

		self & geometry(unsigned w, unsigned h);
		self & name(std::string const & s);
		self & debug(bool d);
		self & version(int major, int minor);

		unsigned width() const {return _w;}
		unsigned height() const {return _h;}
		std::string const & name() const {return _name;}
		bool debug() const {return _debug;}
		std::pair<int, int> version() const {return _version;}

	private:
		unsigned _w, _h;
		std::string _name;
		bool _debug;
		std::pair<int, int> _version;  // (major, minor)
	};

	virtual ~window_layer() {}
	virtual void name(std::string const & s) {}
	virtual void install_display_handler() {}
	virtual void main_loop() {}
	virtual void main_loop_event() {}
	virtual void swap_buffers() {}
	virtual int modifiers() {assert(0 && "unimplemented method"); return 0;}
	virtual void bind_as_render_target(int w, int h) {}
};


template <typename L>  // L : window-layer implementation \sa glut_layer
class event_behaviour : public L
{
public:
	using parameters = typename L::parameters;

	event_behaviour(parameters const & params);
	void start() {L::main_loop();}
	void idle() override {/* TODO: uspi vlakno na 1/60s */}
};


template <typename L>
class pool_behaviour : public L
{
public:
	using parameters = typename L::parameters;
	using user_input = typename L::user_input;

	pool_behaviour(parameters const & params);
	void start();
	virtual void update(float dt);
	virtual void input(float dt);
	void close() override;  //!< \sa event_handler::close()
	void loop();
	bool loop_step();
	float fps() const;
	std::tuple<float, float, float> const & fps_stats() const;

private:
	using hres_clock = std::chrono::high_resolution_clock;

	void mouse_motion(int x, int y) override;
	void mouse_passive_motion(int x, int y) override;
	void mouse_click(event_handler::button b, event_handler::state s, event_handler::modifier m, int x, int y) override;
	void mouse_wheel(event_handler::wheel w, event_handler::modifier m, int x, int y) override;
	void key_typed(unsigned char c, event_handler::modifier m, int x, int y) override;
	void key_released(unsigned char c, event_handler::modifier m, int x, int y) override;
	void touch_performed(int x, int y, int finger_id, event_handler::action a) override;

	std::tuple<float, float, float> _fps;  // (current, min, max)
	bool _closed = false;
	hres_clock::time_point _tp;
};


template <template<class> class B, typename L>
window<B, L>::window(parameters const & params)
	: B<L>{params}
{
	_w = params.width();
	_h = params.height();
}

template <template<class> class B, typename L>
void window<B, L>::reshape(int w, int h)
{
	base::reshape(w, h);
	_w = w;
	_h = h;
}

template <template<class> class B, typename L>
glm::ivec2 window<B, L>::center() const
{
	return glm::ivec2{_w/2, _h/2};
}

template <template<class> class B, typename L>
void window<B, L>::bind_as_render_target()
{
	L::bind_as_render_target(_w, _h);
}

template <template<class> class B, typename L>
void window<B, L>::geometry(unsigned w, unsigned h)
{
	_w = w;
	_h = h;
}


template <typename L>
event_behaviour<L>::event_behaviour(parameters const & params)
	: L{params}
{
	L::install_display_handler();
}


template <typename L>
pool_behaviour<L>::pool_behaviour(parameters const & params)
	: L{params}, _fps{std::make_tuple(0.0f, 1e6f, 0.0f)}
{
	_tp = hres_clock::now();
}

template <typename L>
void pool_behaviour<L>::start()
{
	loop();
}

template <typename L>
void pool_behaviour<L>::update(float dt)
{
	// fps statistics
	static float time_count = 0.0f;
	static unsigned frame_count = 0;

	time_count += dt;
	frame_count += 1;

	if (time_count > 1.0f)
	{
		float curr = frame_count/time_count;
		_fps = std::make_tuple(curr, std::min(std::get<1>(_fps), curr), std::max(std::get<2>(_fps), curr));
		frame_count = 0;
		time_count -= 1.0f;
	}
}

template <typename L>
void pool_behaviour<L>::input(float dt)
{
	L::in().input(dt);
}


template <typename L>
void pool_behaviour<L>::close()
{
	_closed = true;
}

template <typename L>
void pool_behaviour<L>::loop()
{
	_tp = hres_clock::now();

	while (loop_step())
		;
}

template <typename L>
bool pool_behaviour<L>::loop_step()
{
	hres_clock::time_point now = hres_clock::now();
	hres_clock::duration d = now - _tp;
	_tp = now;
	float dt = std::chrono::duration_cast<std::chrono::milliseconds>(d).count() / 1000.0f;

	L::main_loop_event();
	if (_closed)
		return false;

	input(dt);
	update(dt);
	this->display();

	return !_closed;
}

template <typename L>
float pool_behaviour<L>::fps() const
{
	return std::get<0>(_fps);
}

template <typename L>
std::tuple<float, float, float> const & pool_behaviour<L>::fps_stats() const
{
	return _fps;
}

template <typename L>
void pool_behaviour<L>::mouse_motion(int x, int y)
{
//	in.mouse_motion(x, y);
}

template <typename L>
void pool_behaviour<L>::mouse_passive_motion(int x, int y)
{
//	in.mouse_passive_motion(x, y);
}

template <typename L>
void pool_behaviour<L>::mouse_click(event_handler::button b, event_handler::state s, event_handler::modifier m, int x, int y)
{
//	in.mouse_click(b, s, m, x, y);
}

template <typename L>
void pool_behaviour<L>::mouse_wheel(event_handler::wheel w, event_handler::modifier m, int x, int y)
{
//	in.mouse_wheel(w, m, x, y);
}

template <typename L>
void pool_behaviour<L>::key_typed(unsigned char c, event_handler::modifier m, int x, int y)
{
//	in.key_typed(c, m, x, y);
}

template <typename L>
void pool_behaviour<L>::key_released(unsigned char c, event_handler::modifier m, int x, int y)
{
//	in.key_released(c, m, x, y);
}

template <typename L>
void pool_behaviour<L>::touch_performed(int x, int y, int finger_id, event_handler::action a)
{
//	in.touch_performed(x, y, finger_id, a);
}

}  // ui
