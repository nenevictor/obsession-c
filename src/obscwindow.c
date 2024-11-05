#include <obscwindow.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct obsc_window
{
	GLFWwindow *glfw_window;
};

static struct obsc_event global_obsc_events[OBSC_MAX_WINDOW_COUNT];

static struct obsc_event *obsc_event__get_by_key(
	void *_key)
{
	size_t i = 0;
	struct obsc_event *event = NULL;

	while (i < OBSC_MAX_WINDOW_COUNT)
	{
		if (global_obsc_events[i].event_key == _key)
		{
			event = &global_obsc_events[i];
			i = OBSC_MAX_WINDOW_COUNT;
		}
		i++;
	}

	return event;
}

static void glfw_error_callback(
	int _error,
	const char *_description)
{
	fprintf(
		stderr,
		"GLFW error; Code: %i; Description: %s;\n",
		_error,
		_description);
}

static void glfw_key_event_callback(
	GLFWwindow *_window,
	int _key,
	int _scancode,
	int _action,
	int _mods)
{
	struct obsc_event *event = NULL;

	(void)_scancode;
	(void)_mods;

	event = obsc_event__get_by_key(_window);

	if (!event)
	{
		return;
	}

	switch (_action)
	{
	case 0:
		event->keys_down[_key] = 0;
		event->keys_pressed[_key] = 0;
		event->keys_up[_key] = 1;
		break;
	case 1:
		event->keys_down[_key] = 0;
		event->keys_pressed[_key] = 1;
		event->keys_up[_key] = 0;
		break;
	case 2:
		event->keys_down[_key] = 1;
		event->keys_pressed[_key] = 0;
		event->keys_up[_key] = 0;
		break;
	}
}

static void glfw_cursor_event_callback(
	GLFWwindow *_window,
	double _x,
	double _y)
{
	struct obsc_event *event = NULL;

	event = obsc_event__get_by_key(_window);

	if (!event)
	{
		return;
	}

	event->cursor_x = _x;
	event->cursor_y = _y;
}

static void glfw_cursor_button_event_callback(GLFWwindow *_window,
											  int _button,
											  int _action,
											  int _mods)
{
	struct obsc_event *event = NULL;

	(void)_mods;

	event = obsc_event__get_by_key(_window);

	if (!event)
	{
		return;
	}

	if (_action)
	{
		event->cursor_keys_down[_button] = 0;
		event->cursor_keys_pressed[_button] = 1;
	}
	else
	{
		event->cursor_keys_down[_button] = 0;
		event->cursor_keys_pressed[_button] = 0;
	}
}

int obsc_event__is_key_down(
	const struct obsc_event *_event,
	const int _key)
{
	return _event->keys_down[_key];
}

int obsc_event__is_key_pressed(
	const struct obsc_event *_event,
	const int _key)
{
	return _event->keys_pressed[_key];
}

int obsc_event__is_key_up(
	const struct obsc_event *_event,
	const int _key)
{
	return _event->keys_up[_key];
}

int obsc_event__is_cursor_button_down(
	const struct obsc_event *_event,
	const int _button)
{
	return _event->cursor_keys_down[_button];
}

int obsc_event__is_cursor_button_pressed(
	const struct obsc_event *_event,
	const int _button)
{
	return _event->cursor_keys_pressed[_button];
}

int obsc_window_api__init()
{
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
	{
		return 1;
	}

	memset(global_obsc_events, 0, sizeof(global_obsc_events));

	return 0;
}

struct obsc_window *obsc_window__create(
	const int _width,
	const int _height,
	const char *_title,
	const int _try_vsync)
{
	struct obsc_window *result;
	struct obsc_event *free_event;

	result = malloc(sizeof(struct obsc_window));

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	result->glfw_window = glfwCreateWindow(
		_width,
		_height,
		_title,
		NULL,
		NULL);

	if (!result->glfw_window)
	{
		fprintf(
			stderr,
			"[ GLFW error ]: cannot create window.\n");

		free(result);
		return NULL;
	};

	glfwMakeContextCurrent(result->glfw_window);

	if (_try_vsync)
	{
		glfwSwapInterval(1);
	}

	free_event = obsc_event__get_by_key(0);

	if (!free_event)
	{
		fprintf(
			stderr,
			"[ GLFW error ]: cannot find free event cell for window.\n");

		free(result);
		return NULL;
	}

	free_event->event_key = (void *)result->glfw_window;
	glfwSetKeyCallback(result->glfw_window, glfw_key_event_callback);
	glfwSetCursorPosCallback(result->glfw_window, glfw_cursor_event_callback);
	glfwSetMouseButtonCallback(result->glfw_window, glfw_cursor_button_event_callback);

	return result;
}

void obsc_window__destroy(
	struct obsc_window **_window)
{
	struct obsc_event *event = obsc_event__get_by_key((void *)(*_window)->glfw_window);

	if (event)
	{
		event->event_key = 0;
	}

	glfwDestroyWindow((*_window)->glfw_window);
	*_window = 0;
}

int obsc_window__is_should_close(
	struct obsc_window *_window)
{
	return glfwWindowShouldClose(_window->glfw_window);
}

void obsc_window__get_size(
	struct obsc_window *_window,
	int *_width,
	int *_height)
{
	glfwGetFramebufferSize(
		_window->glfw_window,
		_width,
		_height);
}

/* Swap window buffers. */
void obsc_window__swap_buffers(
	struct obsc_window *_window)
{
	glfwSwapBuffers(_window->glfw_window);
}

/* Update events. */
void obsc_window__poll_events()
{
	glfwPollEvents();
	size_t i = 0, i_a = 0;
	struct obsc_event *event = NULL;

	while (i < OBSC_MAX_WINDOW_COUNT)
	{
		event = &global_obsc_events[i];
		if (event->event_key)
		{
			i_a = 0;
			while (i_a < OBSC_EVENT_KEYS_MAX_COUNT)
			{
				if (event->keys_pressed[i_a] && event->keys_down[i_a])
				{
					event->keys_pressed[i_a] = 0;
				}
				else if (event->keys_pressed[i_a])
				{
					event->keys_down[i_a] = 1;
				}
				i_a++;
			}

			i_a = 0;
			while (i_a < OBSC_EVENT_CURSOR_BUTTONS_MAX_COUNT)
			{
				if (event->cursor_keys_pressed[i_a] &&
					event->cursor_keys_down[i_a])
				{
					event->cursor_keys_pressed[i_a] = 0;
				}
				else if (event->cursor_keys_pressed[i_a])
				{
					event->cursor_keys_down[i_a] = 1;
				}
				i_a++;
			}
		}

		i++;
	}
}

/* Get events for window. */
struct obsc_event *obsc_window__get_event(
	struct obsc_window *_window)
{
	return obsc_event__get_by_key((void *)_window->glfw_window);
}

/* Hide cursor. */
void obsc_window__hide_cursor(
	struct obsc_window *_window)
{
	glfwSetInputMode(_window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

/* Show cursor. */
void obsc_window__show_cursor(
	struct obsc_window *_window)
{
	glfwSetInputMode(_window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

/* Get time. */
double obsc_window__get_time()
{
	return glfwGetTime();
}

/* Try vsync. */
void obsc_window__set_vsync(
	const int _value)
{
	if (_value)
	{
		glfwSwapInterval(1);
	}
	else
	{
		glfwSwapInterval(0);
	}
}

/* Set icon. */
void obsc_window__set_icon(
	struct obsc_window *_window,
	const int _width,
	const int _height,
	unsigned char *_pixels)
{
	struct GLFWimage image;
	image.width = _width;
	image.height = _height;
	image.pixels = _pixels;
	glfwSetWindowIcon(_window->glfw_window, 1, &image);
}

/* Terminate API. */
void obsc_window_api__terminate()
{
	glfwTerminate();
}