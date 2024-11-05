#ifndef OBSC_WINDOW_H
#define OBSC_WINDOW_H
#include <stdio.h>
#include <malloc.h>
#include <string.h>


#define OBSC_EVENT_KEYS_MAX_COUNT 512
#define OBSC_EVENT_CURSOR_BUTTONS_MAX_COUNT 32
#define OBSC_MAX_WINDOW_COUNT 512

#define OBSC_KEY_ESCAPE 256
#define OBSC_KEY_SPACE 32
#define OBSC_KEY_R 82
#define OBSC_CURSOR_BUTTON_LEFT 0

#ifndef OBSC_WINDOW_TRY_VERTICAL_SYNCHRONIZATION
#define OBSC_WINDOW_TRY_VERTICAL_SYNCHRONIZATION 1
#endif

typedef struct obsc_window obsc_window;

typedef struct obsc_event
{
	void *event_key;
	int keys_down[OBSC_EVENT_KEYS_MAX_COUNT];
	int keys_pressed[OBSC_EVENT_KEYS_MAX_COUNT];
	int keys_up[OBSC_EVENT_KEYS_MAX_COUNT];

	unsigned int cursor_x, cursor_y;
	int cursor_keys_down[OBSC_EVENT_CURSOR_BUTTONS_MAX_COUNT];
	int cursor_keys_pressed[OBSC_EVENT_CURSOR_BUTTONS_MAX_COUNT];
} obsc_event;

int obsc_event__is_key_down(
	const struct obsc_event *_event,
	const int _key);

int obsc_event__is_key_pressed(
	const struct obsc_event *_event,
	const int _key);

int obsc_event__is_key_up(
	const struct obsc_event *_event,
	const int _key);

int obsc_event__is_cursor_button_down(
	const struct obsc_event *_event,
	const int _button);

int obsc_event__is_cursor_button_pressed(
	const struct obsc_event *_event,
	const int _button);

/* Init API. */
int obsc_window_api__init();

/* Create window. */
struct obsc_window *obsc_window__create(
	const int _width,
	const int _height,
	const char *_title,
	const int _try_vsync);

/* Destroy window. */
void obsc_window__destroy(
	struct obsc_window **_window);

/* Check if window should close. */
int obsc_window__is_should_close(
	struct obsc_window *_window);

/* Get window size. */
void obsc_window__get_size(
	struct obsc_window *_window,
	int *_width,
	int *_height);

/* Swap window buffers. */
void obsc_window__swap_buffers(
	struct obsc_window *_window);

/* Update events. */
void obsc_window__poll_events();

/* Get events for window. */
struct obsc_event *obsc_window__get_event(
	struct obsc_window *_window);

/* Hide cursor. */
void obsc_window__hide_cursor(
	struct obsc_window *_window);

/* Show cursor. */
void obsc_window__show_cursor(
	struct obsc_window *_window);

/* Get time. */
double obsc_window__get_time();

/* Try vsync. */
void obsc_window__set_vsync(
	const int _value);

/* Set icon. */
void obsc_window__set_icon(
	struct obsc_window *_window,
	const int _width,
	const int _height,
	unsigned char *_pixels);
	
/* Terminate API. */
void obsc_window_api__terminate();

#endif