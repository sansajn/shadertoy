#include <unistd.h>
#include <X11/Xlib.h>
#include "sofd/libsofd.h"
#include "file_chooser_dialog.hpp"

using std::string;

bool open_file_chooser_dialog(string & result_fname, std::string const & dir)
{
	Display* dpy = XOpenDisplay(0);
	if (!dpy) return false;

	x_fib_configure(1, "Open Shader File");
	if (!dir.empty())
		x_fib_configure(0, dir.c_str());
	x_fib_load_recent("/tmp/sofd.recent");
	x_fib_show(dpy, 0, 300, 300);

	bool choosed = false;

	while (1) {
		XEvent event;
		while (XPending(dpy) > 0) {
			XNextEvent(dpy, &event);
			if (x_fib_handle_events(dpy, &event)) {
				if (x_fib_status() > 0) {
					char *fn = x_fib_filename ();
					result_fname = fn;
					choosed = true;
					x_fib_add_recent(fn, time (NULL));
					free(fn);
				}
			}
		}
		if (x_fib_status()) {
			break;
		}
		usleep(80000);
	}
	x_fib_close(dpy);

	x_fib_save_recent("/tmp/sofd.recent");

	x_fib_free_recent();
	XCloseDisplay(dpy);

	return choosed;
}
