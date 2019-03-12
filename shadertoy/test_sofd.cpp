#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <X11/Xlib.h>
#include "sofd/libsofd.h"

static int _fib_filter_fn = 0;

static int fib_filter_movie_filename (const char *name) {
	if (!_fib_filter_fn) return 1;
	const int l3 = strlen (name) - 3;
	const int l4 = l3 - 1;
	const int l5 = l4 - 1;
	const int l6 = l5 - 1;
	const int l9 = l6 - 3;
	if (
			(l4 > 0 && (
					!strcasecmp (&name[l4], ".avi")
				|| !strcasecmp (&name[l4], ".mov")
				|| !strcasecmp (&name[l4], ".ogg")
				|| !strcasecmp (&name[l4], ".ogv")
				|| !strcasecmp (&name[l4], ".mpg")
				|| !strcasecmp (&name[l4], ".mov")
				|| !strcasecmp (&name[l4], ".mp4")
				|| !strcasecmp (&name[l4], ".mkv")
				|| !strcasecmp (&name[l4], ".vob")
				|| !strcasecmp (&name[l4], ".asf")
				|| !strcasecmp (&name[l4], ".avs")
				|| !strcasecmp (&name[l4], ".dts")
				|| !strcasecmp (&name[l4], ".flv")
				|| !strcasecmp (&name[l4], ".m4v")
				)) ||
			(l5 > 0 && (
					!strcasecmp (&name[l5], ".h264")
				|| !strcasecmp (&name[l5], ".webm")
				)) ||
			(l6 > 0 && (
					!strcasecmp (&name[l6], ".dirac")
				)) ||
			(l9 > 0 && (
					!strcasecmp (&name[l9], ".matroska")
				)) ||
			(l3 > 0 && (
					!strcasecmp (&name[l3], ".dv")
				|| !strcasecmp (&name[l3], ".ts")
				))
			)
			{
				return 1;
			}
	return 0;
}

int main (int argc, char **argv) {
	Display* dpy = XOpenDisplay (0);
	if (!dpy) return -1;

	x_fib_cfg_filter_callback (fib_filter_movie_filename);
	x_fib_configure (1, "Open Movie File");
	x_fib_load_recent ("/tmp/sofd.recent");
	x_fib_show (dpy, 0, 300, 300);

	while (1) {
		XEvent event;
		while (XPending (dpy) > 0) {
			XNextEvent (dpy, &event);
			if (x_fib_handle_events (dpy, &event)) {
				if (x_fib_status () > 0) {
					char *fn = x_fib_filename ();
					printf ("OPEN '%s'\n", fn);
					x_fib_add_recent (fn, time (NULL));
					free (fn);
				}
			}
		}
		if (x_fib_status ()) {
			break;
		}
		usleep (80000);
	}
	x_fib_close (dpy);

	x_fib_save_recent ("/tmp/sofd.recent");

	x_fib_free_recent ();
	XCloseDisplay (dpy);
	return 0;
}
