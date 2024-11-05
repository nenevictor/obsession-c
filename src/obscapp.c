#include "obscapp.h"

#if !defined(_DEBUG)
#include "logo.png.h"
#include "icon.png.h"
#include "Iosevka-Heavy.ttf.h"
#endif

#if defined(__linux__) || defined(__MINGW64__)
int usleep(unsigned int);
#endif

int obsc_app__random_int(
	const int _min,
	const int _max)
{
	return _min + ((float)rand() / (float)RAND_MAX * (float)_max);
}

float obsc_app__random_float(
	const float _min,
	const float _max)
{
	return _min + (float)rand() / (float)RAND_MAX * _max;
}

int obsc_app__circles_intersect(
	const int _x0,
	const int _y0,
	const int _r0,
	const int _x1,
	const int _y1,
	const int _r1)
{
	if (_x0 + _r0 >= _x1 - _r1 && _x0 - _r0 <= _x1 + _r1 &&
		_y0 + _r0 >= _y1 - _r1 && _y0 - _y0 <= _y1 + _y1)
	{
		return sqrtf(powf(_x0 - _x1, 2) + powf(_y0 - _y1, 2)) < (_r0 + _r1);
	}
	return 0;
}

void obsc_app__begin_frame(
	struct obsc_app *_app,
	const struct obsc_color _clear_color)
{
	_app->frame_being_time = obsc_window__get_time();
	_app->frame_delta = _app->frame_being_time - _app->frame_end_time;

	/* Cull frames. */
	if (!_app->settings.try_vsync)
	{
		while (_app->frame_delta < 1.0f / (float)_app->settings.target_fps)
		{
#if defined(__linux__) || defined(__MINGW64__)
			usleep((10000) / (_app->settings.target_fps / 2));
#endif
			_app->frame_being_time = obsc_window__get_time();
			_app->frame_delta = _app->frame_being_time - _app->frame_end_time;
		}
	}

	_app->cursor_position = obsc_vector2__follow(
		(struct obsc_vector2){_app->event->cursor_x, _app->event->cursor_y},
		_app->cursor_position,
		1.0f - _app->settings.cursor_smoothing);

	obsc_window__get_size(
		_app->window,
		&_app->window_width,
		&_app->window_height);
	obsc_renderer2d__set_size(
		_app->renderer,
		_app->window_width,
		_app->window_height);

	_app->size_unit = (float)OBSC_MIN(
						  _app->window_width,
						  _app->window_height) /
					  100.0f;

	_app->ui_style.text_paragraph_size = _app->settings.text_paragraph_size * _app->size_unit;
	_app->ui_style.text_title_size = _app->settings.text_title_size * _app->size_unit;
	_app->ui_style.border_thickness = OBSC_APP_UI_BORDER_THICKNESS * _app->size_unit;
	_app->ui_style.text_margin = OBSC_APP_UI_TEXT_MARGIN * _app->size_unit;

	glClearColor(
		(float)_clear_color.r / 255.0f,
		(float)_clear_color.g / 255.0f,
		(float)_clear_color.b / 255.0f,
		(float)_clear_color.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	obsc_renderer2d__begin(_app->renderer);
}

void obsc_app__end_frame(
	struct obsc_app *_app)
{
	_app->frame_end_time = obsc_window__get_time();
	obsc_renderer2d__end(_app->renderer);
	obsc_window__swap_buffers(_app->window);
}

int obsc_app__reload_font(
	struct obsc_app *_app,
	const wchar_t *_required_codepoints)
{
	int *codepoints;
	size_t codepoints_count;
	wchar_t font_chars[1024];

	memset(font_chars, 0, sizeof(font_chars));
	wcscat(font_chars, L"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM");
	wcscat(font_chars, L"1234567890,.!:;@#$%^&*()-_+{}[]<>");

	if (_required_codepoints && *_required_codepoints)
	{
		wcscat(font_chars, _required_codepoints);
	}

	codepoints = nestbtt_getCodepointsFromWideString(
		font_chars,
		wcslen(font_chars),
		&codepoints_count);

	if (obsc_font__is_ready(&_app->font))
	{
		obsc_font__free(&_app->font);
	}

#if !defined(_DEBUG)
	_app->font = obsc_font__load_from_memory(
		RESOURCES_IOSEVKA_HEAVY_TTF,
		RESOURCES_IOSEVKA_HEAVY_TTF_SIZE,
		_app->window_height / 10,
		codepoints,
		codepoints_count);
#else
	_app->font = obsc_font__load(
		OBSC_APP_FONT_PATH,
		_app->window_height / 10,
		codepoints,
		codepoints_count);
#endif

	if (!obsc_font__is_ready(&_app->font))
	{
		return 1;
	}

	free(codepoints);
	return 0;
}

void obsc_app__prepare_game(
	struct obsc_app *_app)
{
	size_t i = 0;
	while (i < _app->settings.circles_count)
	{
		_app->circles[i].z = 1.0f;
		_app->circles[i].x = obsc_app__random_float(0.0f, 1.0f);
		_app->circles[i].y = obsc_app__random_float(0.0f, 1.0f);
		i++;
	}
	_app->game_stats.hit_count = 0;
	_app->game_stats.mis_count = 0;
	_app->game_stats.despair_count = 0;
	_app->game_status = game_status_begin;
	_app->current_scene = scene_game;
}

int obsc_app__load_settings(
	struct obsc_app *_app)
{
	struct nejson json;
	nejson__init(&json);

	if (nejson__load_from_file(
			&json,
			OBSC_APP_SETTINGS_PATH))
	{
		nejson__free(&json);
		return 1;
	}

	_app->settings.window_width = nejson__get_integer_tree(&json, "s", "window_width");
	_app->settings.window_height = nejson__get_integer_tree(&json, "s", "window_height");
	_app->settings.circles_count = nejson__get_integer_tree(&json, "s", "circles_count");
	_app->settings.target_fps = nejson__get_integer_tree(&json, "s", "target_fps");
	_app->settings.cursor_smoothing = nejson__get_float_tree(&json, "s", "cursor_smoothing");
	_app->settings.text_paragraph_size = nejson__get_float_tree(&json, "s", "text_paragraph_size");
	_app->settings.text_title_size = nejson__get_float_tree(&json, "s", "text_title_size");
	_app->settings.circles_narrowing_speed = nejson__get_float_tree(&json, "s", "circles_narrowing_speed");
	_app->settings.try_vsync = nejson__get_boolean_tree(&json, "s", "try_vsync");
	_app->settings.game_mode = nejson__get_integer_tree(&json, "s", "game_mode");

	strcpy(_app->settings.language_name,
		   nejson__get_string_tree(
			   &json,
			   "s",
			   "language_name"));

	nejson__free(&json);
	return 0;
}

int obsc_app__save_settings(
	struct obsc_app *_app)
{
	int result = 0;
	struct nejson json;
	nejson__init(&json);

	result |= nejson__set_integer_tree(&json, _app->settings.window_width, "s", "window_width");
	result |= nejson__set_integer_tree(&json, _app->settings.window_height, "s", "window_height");
	result |= nejson__set_integer_tree(&json, _app->settings.circles_count, "s", "circles_count");
	result |= nejson__set_integer_tree(&json, _app->settings.target_fps, "s", "target_fps");
	result |= nejson__set_float_tree(&json, _app->settings.cursor_smoothing, "s", "cursor_smoothing");
	result |= nejson__set_float_tree(&json, _app->settings.text_paragraph_size, "s", "text_paragraph_size");
	result |= nejson__set_float_tree(&json, _app->settings.text_title_size, "s", "text_title_size");
	result |= nejson__set_float_tree(&json, _app->settings.circles_narrowing_speed, "s", "circles_narrowing_speed");
	result |= nejson__set_boolean_tree(&json, _app->settings.try_vsync, "s", "try_vsync");
	result |= nejson__set_integer_tree(&json, _app->settings.game_mode, "s", "game_mode");
	result |= nejson__set_string_tree(&json, _app->settings.language_name, "s", "language_name");
	result |= nejson__save_to_file(
		&json,
		OBSC_APP_SETTINGS_PATH,
		1);

	nejson__free(&json);
	return result;
}

int obsc_app__init(
	struct obsc_app *_app)
{
	char window_title[127];
	memset(_app, 0, sizeof(*_app));

	_app->quit = 0;

	if (obsc_app__load_settings(_app))
	{
		_app->settings.window_width = 800;
		_app->settings.window_height = 600;
		_app->settings.circles_count = 3;
		_app->settings.target_fps = 60;
		_app->settings.cursor_smoothing = 0.0f;
		_app->settings.text_paragraph_size = 4.0f;
		_app->settings.text_title_size = 9.0f;
		_app->settings.circles_narrowing_speed = 0.001f;
		_app->settings.try_vsync = 1;
		_app->settings.game_mode = game_mode_hard;
		strcpy(
			_app->settings.language_name,
			"english");

		fprintf(stderr, "[ WARNING ]: cannot load settings file.\n");
		fflush(stderr);
	}

	_app->temp_settings = _app->settings;

	_app->current_scene = scene_intro;
	_app->window_width = _app->settings.window_width;
	_app->window_height = _app->settings.window_height;
	_app->cursor_position.x = 0.0f;
	_app->cursor_position.y = 0.0f;

	_app->ui_style.background_color_default = OBSC_COLOR_BLACK;
	_app->ui_style.background_color_hovered = OBSC_COLOR_BLACK;
	_app->ui_style.background_color_pressed = OBSC_COLOR_BLACK;
	_app->ui_style.text_color_default = OBSC_COLOR_WHITE;
	_app->ui_style.text_color_hovered = OBSC_COLOR_YELLOW;
	_app->ui_style.text_color_pressed = OBSC_COLOR_RED;
	_app->ui_style.border_color_default = OBSC_COLOR_WHITE;
	_app->ui_style.border_color_hovered = OBSC_COLOR_YELLOW;
	_app->ui_style.border_color_pressed = OBSC_COLOR_RED;
	_app->ui_style.font = &_app->font;
	_app->ui_style.border_thickness = 0;
	_app->ui_style.text_paragraph_size = 0;
	_app->ui_style.text_title_size = 0;
	_app->ui_style.text_margin = 0;
	_app->ui_style.text_transpose_to_center_x = 1.0f;
	_app->ui_style.text_transpose_to_center_y = 0.75f;

	obsc_language__init(&_app->language);
	if (obsc_language__load(
			&_app->language,
			OBSC_APP_LANGUAGES_PATH))
	{
		strcpy(
			_app->settings.language_name,
			"english");
		fprintf(stderr, "[ WARNING ]: cannot load language file.\n");
		fflush(stderr);
	}
	else
	{
		obsc_language__set_language(
			&_app->language,
			_app->settings.language_name);
	}

	if (obsc_window_api__init())
	{
		fprintf(stderr, "[ ERROR ]: cannot initialize window API.\n");
		fflush(stderr);
		return 1;
	}

	sprintf(
		window_title,
		"%s %i.%i.%i",
		obsc_language__get(
			&_app->language,
			"title"),
		OBSC_APP_VERSION_MAJOR,
		OBSC_APP_VERSION_MINOR,
		OBSC_APP_VERSION_PATCH);

	_app->window = obsc_window__create(
		_app->settings.window_width,
		_app->settings.window_height,
		window_title,
		_app->settings.try_vsync);

	if (!_app->window)
	{
		fprintf(stderr, "[ ERROR ]: cannot open window.\n");
		fflush(stderr);
		obsc_window_api__terminate();
		return 1;
	}

	if (obsc_graphic_api__init())
	{
		fprintf(stderr, "[ ERROR ]: cannot initialize graphic API.\n");
		fflush(stderr);
		obsc_window_api__terminate();
		return 1;
	}

	_app->renderer = obsc_renderer2d__init();
	if (!_app->renderer)
	{
		fprintf(stderr, "[ ERROR ]: cannot create 2D renderer.\n");
		fflush(stderr);
		obsc_graphic_api__terminate();
		obsc_window_api__terminate();
		return 1;
	}

	if (obsc_app__reload_font(
			_app,
			obsc_language__get_codepoints_string(
				&_app->language)))
	{
		fprintf(stderr, "[ ERROR ]: cannot load font.\n");
		fflush(stderr);
		obsc_graphic_api__terminate();
		obsc_window_api__terminate();
		return 1;
	}

#if !defined(_DEBUG)
	_app->texture_logo = obsc_texture__load_from_memory(
		RESOURCES_LOGO_PNG,
		RESOURCES_LOGO_PNG_SIZE);
#else
	_app->texture_logo = obsc_texture__load(
		OBSC_APP_LOGO_PATH);
#endif

	if (!_app->texture_logo)
	{
		fprintf(stderr, "[ ERROR ]: cannot load logo texture.\n");
		fflush(stderr);
		obsc_graphic_api__terminate();
		obsc_window_api__terminate();
		return 1;
	}

	unsigned char *icon_pixels;
	int icon_width, icon_height;

#if !defined(_DEBUG)
	icon_pixels = stbi_load_from_memory(
		RESOURCES_ICON_PNG,
		RESOURCES_ICON_PNG_SIZE,
		&icon_width,
		&icon_height,
		NULL,
		4);
#else
	icon_pixels = stbi_load(
		OBSC_APP_ICON_PATH,
		&icon_width,
		&icon_height,
		NULL,
		4);
#endif

	if (!icon_pixels)
	{
		fprintf(stderr, "[ WARNING ]: cannot load window icon.\n");
	}
	else
	{
		obsc_window__set_icon(
			_app->window,
			icon_width,
			icon_height,
			icon_pixels);
		free(icon_pixels);
	}

	obsc_window__hide_cursor(_app->window);

	return 0;
}

int obsc_app__run(
	struct obsc_app *_app)
{
#ifdef _DEBUG
	char debug_text[1024];
#endif

	while (!_app->quit)
	{
		obsc_window__poll_events();
		_app->event = obsc_window__get_event(_app->window);
		if (!_app->event)
		{
			fprintf(stderr, "[ ERROR ]: cannot get event.\n");
			fflush(stderr);
			return 1;
		}

		_app->quit |= obsc_window__is_should_close(_app->window);

		obsc_app__begin_frame(
			_app,
			OBSC_COLOR_BLACK);

		switch (_app->current_scene)
		{
		case scene_intro:
			obsc_app__draw_intro_scene(_app);
			break;
		case scene_main_menu:
			obsc_app__draw_menu_scene(_app);
			break;
		case scene_settings:
			obsc_app__draw_settings_scene(_app);
			break;
		case scene_game:
			obsc_app__draw_game_scene(_app);
			break;
		default:
			obsc_app__draw_intro_scene(_app);
			break;
		}

		obsc_app__draw_cursor(_app);

#ifdef _DEBUG
		sprintf(
			debug_text,
			"FPS: %.0f\n"
			"cursor x: %u\n"
			"cursor y: %u\n"
			"cursor keys: %u\n",
			nearbyintf(1.0f / _app->frame_delta),
			(unsigned int)_app->cursor_position.x,
			(unsigned int)_app->cursor_position.y,
			obsc_event__is_cursor_button_down(_app->event, 0));

		obsc_renderer2d__draw_text(
			_app->renderer,
			&_app->font,
			debug_text,
			30,
			OBSC_COLOR_RED,
			0.0f,
			0.0f);
#endif
		obsc_app__end_frame(_app);
	}

	return 0;
}

void obsc_app__close(
	struct obsc_app *_app)
{
	obsc_app__save_settings(_app);
	obsc_language__free(&_app->language);
	obsc_renderer2d__free(&_app->renderer);
	obsc_font__free(&_app->font);
	obsc_texture__free(&_app->texture_logo);
	obsc_graphic_api__terminate();
	obsc_window__destroy(&_app->window);
	obsc_window_api__terminate();
}

void obsc_app__draw_cursor(
	struct obsc_app *_app)
{
	struct obsc_color color = OBSC_COLOR_YELLOW;

	if (obsc_event__is_cursor_button_down(
			_app->event, OBSC_CURSOR_BUTTON_LEFT) ||
		obsc_event__is_key_down(_app->event, OBSC_KEY_SPACE))
	{
		color = OBSC_COLOR_RED;
	}

	obsc_renderer2d__draw_circle(
		_app->renderer,
		color,
		_app->cursor_position.x,
		_app->cursor_position.y,
		_app->size_unit * OBSC_APP_CURSOR_RADIUS);
}

void obsc_app__draw_intro_scene(
	struct obsc_app *_app)
{
	int texture_width, texture_height;
	float texture_logo_scale;

	obsc_texture__get_size(
		_app->texture_logo,
		&texture_width,
		&texture_height);

	texture_logo_scale =
		((float)_app->window_height / 7.0f) /
		texture_height;

	obsc_renderer2d__draw_texture_scale(
		_app->renderer,
		_app->texture_logo,
		_app->window_width / 2 - ((float)texture_width * texture_logo_scale) / 2,
		_app->window_height / 2 - ((float)texture_height * texture_logo_scale) / 2,
		texture_logo_scale);

	obsc_ui__text_wide(
		_app->renderer,
		_app->ui_style.font,
		(size_t)obsc_window__get_time() % 2 ? OBSC_COLOR_RED : OBSC_COLOR_WHITE,
		_app->ui_style.text_paragraph_size,
		obsc_language__get_wide(
			&_app->language,
			"intro_blinking_text"),
		text_alignment_center_x | text_alignment_down,
		_app->window_width / 2,
		_app->window_height - 30,
		_app->ui_style.text_transpose_to_center_x,
		_app->ui_style.text_transpose_to_center_y);

	if (obsc_event__is_key_pressed(_app->event, OBSC_KEY_SPACE) ||
		obsc_event__is_cursor_button_down(_app->event, OBSC_CURSOR_BUTTON_LEFT))
	{
		_app->current_scene = scene_main_menu;
	}

	_app->quit |= obsc_event__is_key_pressed(_app->event, OBSC_KEY_ESCAPE);
}

void obsc_app__draw_menu_scene(
	struct obsc_app *_app)
{
	struct obsc_vector2 margin, element_position;
	float button_width, button_height;
	margin.x = _app->size_unit * 10.0f;
	margin.y = _app->size_unit * 10.0f;
	button_width = _app->size_unit * 50.0f;
	button_height = _app->ui_style.text_paragraph_size +
					_app->ui_style.text_margin * 2.0f +
					_app->ui_style.border_thickness * 2.0f +
					_app->size_unit /* a little margin */;

	element_position = margin;

	obsc_ui__title_wide(
		_app->renderer,
		&_app->ui_style,
		obsc_language__get_wide(
			&_app->language,
			"title"),
		text_alignment_none,
		element_position.x,
		element_position.y);

	element_position.y += _app->size_unit * 30.0f;
	if (obsc_ui__button_wide(
			_app->renderer,
			_app->event,
			&_app->ui_style,
			obsc_language__get_wide(
				&_app->language,
				"button_begin"),
			element_position.x,
			element_position.y,
			button_width,
			_app->size_unit * 5.0f))
	{
		obsc_app__prepare_game(_app);
	}

	element_position.y += button_height;
	if (obsc_ui__button_wide(
			_app->renderer,
			_app->event,
			&_app->ui_style,
			obsc_language__get_wide(
				&_app->language,
				"button_settings"),
			element_position.x,
			element_position.y,
			button_width,
			_app->size_unit * 5.0f))
	{
		_app->current_scene = scene_settings;
	}

	element_position.y += button_height;
	_app->quit |= obsc_ui__button_wide(
		_app->renderer,
		_app->event,
		&_app->ui_style,
		obsc_language__get_wide(
			&_app->language,
			"button_exit"),
		element_position.x,
		element_position.y,
		button_width,
		_app->size_unit * 5.0f);

	_app->quit |= obsc_event__is_key_pressed(_app->event, OBSC_KEY_ESCAPE);
}

void obsc_app__draw_settings_scene(
	struct obsc_app *_app)
{
	static wchar_t temp_text[512];
	const char *temp_key;

	struct obsc_vector2 margin, element_position;
	float button_width, button_height, slider_height;
	margin.x = _app->size_unit * 10.0f;
	margin.y = _app->size_unit * 5.0f;
	button_width = _app->size_unit * 30.0f;
	button_height = _app->ui_style.text_paragraph_size +
					_app->ui_style.text_margin * 2.0f +
					_app->ui_style.border_thickness * 2.0f +
					_app->size_unit /* a little margin */;
	slider_height = _app->size_unit * 4.0f;

	element_position = margin;

	obsc_ui__title_wide(
		_app->renderer,
		&_app->ui_style,
		obsc_language__get_wide(
			&_app->language,
			"title_settings"),
		text_alignment_none,
		element_position.x,
		element_position.y);

	element_position.y += button_height * 2.0f;
	swprintf(
		temp_text,
		sizeof(temp_text) / sizeof(*temp_text),
		L"%ls: %.2f",
		obsc_language__get_wide(
			&_app->language,
			"slider_cursor_smoothing"),
		_app->temp_settings.cursor_smoothing);

	obsc_ui__slider_wide(
		_app->renderer,
		_app->event,
		&_app->ui_style,
		temp_text,
		element_position.x,
		element_position.y,
		button_width,
		slider_height,
		0,
		1,
		&_app->temp_settings.cursor_smoothing);

	element_position.y += button_height;
	swprintf(
		temp_text,
		sizeof(temp_text) / sizeof(*temp_text),
		L"%ls: %u",
		obsc_language__get_wide(
			&_app->language,
			"slider_circles_count"),
		_app->temp_settings.circles_count);
	float circles_count = _app->temp_settings.circles_count;
	obsc_ui__slider_wide(
		_app->renderer,
		_app->event,
		&_app->ui_style,
		temp_text,
		element_position.x,
		element_position.y,
		button_width,
		slider_height,
		1,
		OBSC_APP_MAX_CIRCLES_COUNT,
		&circles_count);
	_app->temp_settings.circles_count = circles_count;

	element_position.y += button_height;
	swprintf(
		temp_text,
		sizeof(temp_text) / sizeof(*temp_text),
		L"%ls: %.2f",
		obsc_language__get_wide(
			&_app->language,
			"slider_circles_narrowing_speed"),
		10000.0f * _app->temp_settings.circles_narrowing_speed);
	obsc_ui__slider_wide(
		_app->renderer,
		_app->event,
		&_app->ui_style,
		temp_text,
		element_position.x,
		element_position.y,
		button_width,
		slider_height,
		0.00001f,
		0.01f,
		&_app->temp_settings.circles_narrowing_speed);

	element_position.y += button_height;
	swprintf(
		temp_text,
		sizeof(temp_text) / sizeof(*temp_text),
		L"%ls: %u",
		obsc_language__get_wide(
			&_app->language,
			"slider_target_fps"),
		_app->temp_settings.target_fps);
	float target_fps = _app->temp_settings.target_fps;
	obsc_ui__slider_wide(
		_app->renderer,
		_app->event,
		&_app->ui_style,
		temp_text,
		element_position.x,
		element_position.y,
		button_width,
		slider_height,
		20,
		244,
		&target_fps);
	_app->temp_settings.target_fps = target_fps;

	element_position.y += button_height;
	swprintf(
		temp_text,
		sizeof(temp_text) / sizeof(*temp_text),
		L"%ls: %s",
		obsc_language__get_wide(
			&_app->language,
			"checker_try_vsync"),
		_app->temp_settings.try_vsync ? "on" : "off");
	obsc_ui__checker_wide(
		_app->renderer,
		_app->event,
		&_app->ui_style,
		temp_text,
		element_position.x,
		element_position.y,
		slider_height,
		&_app->temp_settings.try_vsync);

	element_position.y += button_height;
	switch (_app->temp_settings.game_mode)
	{
	case game_mode_easy:
		temp_key = "label_easy_mode";
		break;
	case game_mode_normal:
		temp_key = "label_normal_mode";
		break;
	case game_mode_hard:
		temp_key = "label_hard_mode";
		break;
	default:
		OBSC_ASSERT("Unreachable game mode.");
		break;
	}

	swprintf(
		temp_text,
		sizeof(temp_text) / sizeof(*temp_text),
		L"%ls: ",
		obsc_language__get_wide(
			&_app->language,
			"label_mode"));

	wcscat(temp_text,
		   obsc_language__get_wide(
			   &_app->language,
			   temp_key));

	if (obsc_ui__button_wide(
			_app->renderer,
			_app->event,
			&_app->ui_style,
			temp_text,
			element_position.x,
			element_position.y,
			button_width,
			_app->size_unit * 5.0f))
	{
		_app->temp_settings.game_mode++;
		if (_app->temp_settings.game_mode > game_mode_hard)
			_app->temp_settings.game_mode = game_mode_easy;
	}

	element_position.y += button_height;
	if (obsc_ui__button_wide(
			_app->renderer,
			_app->event,
			&_app->ui_style,
			obsc_language__get_title(
				&_app->language),
			element_position.x,
			element_position.y,
			button_width,
			_app->size_unit * 5.0f))
	{
		_app->language.current_language_index++;
		if (_app->language.current_language_index >=
			_app->language.languages_list_size)
			_app->language.current_language_index = 0;

		obsc_app__reload_font(
			_app,
			obsc_language__get_codepoints_string(
				&_app->language));

		strcpy(_app->temp_settings.language_name,
			   _app->language.languages_list[_app->language.current_language_index]);
	}

	element_position.y += button_height * 2.0f;
	if (obsc_ui__button_wide(
			_app->renderer,
			_app->event,
			&_app->ui_style,
			obsc_language__get_wide(
				&_app->language,
				"button_use"),
			element_position.x,
			element_position.y,
			button_width,
			_app->size_unit * 5.0f))
	{
		if (_app->settings.try_vsync != _app->temp_settings.try_vsync)
			obsc_window__set_vsync(_app->temp_settings.try_vsync);
		_app->settings = _app->temp_settings;
		obsc_app__save_settings(_app);
	}

	element_position.x += button_width + _app->size_unit * 2.0f;
	if (obsc_ui__button_wide(
			_app->renderer,
			_app->event,
			&_app->ui_style,
			obsc_language__get_wide(
				&_app->language,
				"button_close"),
			element_position.x,
			element_position.y,
			button_width,
			_app->size_unit * 5.0f))
	{
		_app->current_scene = scene_main_menu;
	}
}

void obsc_app__draw_game_scene(
	struct obsc_app *_app)
{
	const float circle_full_r = OBSC_APP_CIRCLE_RADIUS * _app->size_unit;
	const float frame_delta_difference = _app->frame_delta / OBSC_APP_DEFAULT_FRAME_TIME;

	int is_circle_hit = 0;
	int is_hit_pressed = (obsc_event__is_cursor_button_pressed(_app->event, 0) ||
						  obsc_event__is_key_pressed(_app->event, OBSC_KEY_SPACE));

	size_t i = 0;

	while (i < _app->settings.circles_count)
	{
		float circle_x, circle_y, circle_r;
		struct obsc_color temp_color;
		if (_app->game_status == game_status_process)
		{
			_app->circles[i].z -= _app->settings.circles_narrowing_speed *
								  frame_delta_difference;

			if (_app->circles[i].z <= 0.0)
			{
				struct obsc_vector3 temp;

				_app->circles[i].z = 1.0f;

				temp = _app->circles[i];
				_app->circles[i] = _app->circles[_app->settings.circles_count - 1];
				_app->circles[_app->settings.circles_count - 1] = temp;

				_app->game_status = game_status_failure;
			}
		}

		circle_x = _app->circles[i].x * _app->window_width;
		circle_y = _app->circles[i].y * _app->window_height;
		circle_r = _app->circles[i].z * circle_full_r;

		circle_x = OBSC_CLAMP(
			circle_x, circle_full_r,
			_app->window_width - circle_full_r);
		circle_y = OBSC_CLAMP(
			circle_y, circle_full_r,
			_app->window_height - circle_full_r);

		if (is_hit_pressed)
		{
			if (obsc_app__circles_intersect(
					circle_x,
					circle_y,
					circle_r,
					_app->cursor_position.x,
					_app->cursor_position.y,
					_app->size_unit * OBSC_APP_CURSOR_RADIUS))
			{
				_app->circles[i].z = 1.0f;
				_app->circles[i].x = obsc_app__random_float(0.0f, 1.0f);
				_app->circles[i].y = obsc_app__random_float(0.0f, 1.0f);
				circle_x = _app->circles[i].x * _app->window_width;
				circle_y = _app->circles[i].y * _app->window_height;
				circle_r = _app->circles[i].z * circle_full_r;
				if (_app->game_status == game_status_process)
				{
					_app->game_stats.hit_count++;
					is_circle_hit = 1;
				}
				if (_app->game_status == game_status_failure)
				{
					_app->game_stats.despair_count++;
					is_circle_hit = 1;
				}
			}
		}

		if (_app->game_status == game_status_failure &&
			_app->settings.circles_count - 1 == i &&
			!_app->game_stats.despair_count)
		{
			temp_color = OBSC_COLOR_YELLOW;
		}
		else
		{
			temp_color = obsc_color__lerp(
				OBSC_COLOR_RED,
				OBSC_COLOR_WHITE,
				_app->circles[i].z);
		}

		obsc_renderer2d__draw_circle(
			_app->renderer,
			temp_color,
			circle_x,
			circle_y,
			circle_r);

		i++;
	}

	if (!is_circle_hit && is_hit_pressed && _app->game_status == game_status_process)
	{
		_app->game_stats.mis_count++;
		if (_app->settings.game_mode == game_mode_hard)
		{
		}
		switch (_app->settings.game_mode)
		{
		case game_mode_easy:
			_app->game_stats.mis_count++;
			break;
		case game_mode_normal:
			_app->game_stats.mis_count++;
			if (_app->game_stats.hit_count > 0)
				_app->game_stats.hit_count--;
			break;
		case game_mode_hard:
			_app->game_status = game_status_failure;
			break;
		default:
			OBSC_ASSERT("Unreachable game mode.");
			break;
		}
	}

	if (_app->game_stats.hit_count >= OBSC_APP_GAME_SUCCESS_COUNT)
	{
		_app->game_status = game_status_success;
	}

	if (_app->game_status == game_status_begin ||
		_app->game_status == game_status_failure ||
		_app->game_status == game_status_success)
	{
		const char *temp_key_paragraph, *temp_key_title;
		switch (_app->game_status)
		{
		case game_status_begin:
			temp_key_title = "title_game_press_space";
			temp_key_paragraph = "text_game_press_esc";
			break;
		case game_status_failure:
			temp_key_title = "title_game_failure";
			temp_key_paragraph = "text_game_result_helper";
			break;
		case game_status_success:
			temp_key_title = "title_game_success";
			temp_key_paragraph = "text_game_result_helper";
			break;
		case game_status_process:
			temp_key_title = NULL;
			temp_key_paragraph = NULL;
			break;
		default:
			OBSC_ASSERT("Unreachable game status.");
			break;
		}

		if (temp_key_title && temp_key_paragraph)
		{
			obsc_ui__text_wide(
				_app->renderer,
				_app->ui_style.font,
				OBSC_COLOR_RED,
				_app->ui_style.text_title_size,
				obsc_language__get_wide(
					&_app->language,
					temp_key_title),
				text_alignment_center,
				_app->window_width / 2,
				_app->window_height / 2,
				_app->ui_style.text_transpose_to_center_x,
				_app->ui_style.text_transpose_to_center_y);

			obsc_ui__text_wide(
				_app->renderer,
				_app->ui_style.font,
				OBSC_COLOR_RED,
				_app->ui_style.text_paragraph_size,
				obsc_language__get_wide(
					&_app->language,
					temp_key_paragraph),
				text_alignment_center_x | text_alignment_down,
				_app->window_width / 2,
				_app->window_height - _app->size_unit * 2.0f,
				_app->ui_style.text_transpose_to_center_x,
				_app->ui_style.text_transpose_to_center_y);
		}
	}

	if (_app->game_status == game_status_process ||
		_app->game_status == game_status_failure ||
		_app->game_status == game_status_success)
	{
		static wchar_t game_stats_text[127];
		static wchar_t game_mode_text[127],
			hit_count_text[127],
			mis_count_text[127],
			mode_text[127];

		float game_stats_position_y = 0.0f;
		const char *temp_key_mode;
		switch (_app->settings.game_mode)
		{
		case game_mode_easy:
			temp_key_mode = "label_easy_mode";
			break;
		case game_mode_normal:
			temp_key_mode = "label_normal_mode";
			break;
		case game_mode_hard:
			temp_key_mode = "label_hard_mode";
			break;
		default:
			OBSC_ASSERT("Unreachable game mode.");
			break;
		}
		wcscpy(
			game_mode_text,
			obsc_language__get_wide(
				&_app->language,
				temp_key_mode));

		wcscpy(
			hit_count_text,
			obsc_language__get_wide(
				&_app->language,
				"label_hit_count"));
		wcscpy(
			mis_count_text,
			obsc_language__get_wide(
				&_app->language,
				"label_mis_count"));
		wcscpy(
			mode_text,
			obsc_language__get_wide(
				&_app->language,
				"label_mode"));

		memset(game_stats_text, 0, sizeof(game_stats_text));
		swprintf(
			game_stats_text,
			sizeof(game_stats_text) / sizeof(*game_stats_text),
			L"%ls: %u; %ls: %u; %ls: %ls",
			hit_count_text,
			_app->game_stats.hit_count,
			mis_count_text,
			_app->game_stats.mis_count,
			mode_text,
			game_mode_text);

		if (_app->game_status == game_status_process)
		{
			game_stats_position_y = _app->size_unit * 2.0f;
		}
		else
		{
			game_stats_position_y = _app->window_height / 2 + _app->ui_style.text_title_size;
		}

		obsc_ui__text_wide(
			_app->renderer,
			_app->ui_style.font,
			OBSC_COLOR_RED,
			_app->ui_style.text_paragraph_size,
			game_stats_text,
			text_alignment_center_x | text_alignment_top,
			_app->window_width / 2,
			game_stats_position_y,
			_app->ui_style.text_transpose_to_center_x,
			_app->ui_style.text_transpose_to_center_y);
	}

	if (_app->game_status == game_status_begin && is_hit_pressed)
	{
		_app->game_status = game_status_process;
	}

	if (obsc_event__is_key_pressed(_app->event, OBSC_KEY_R))
	{
		obsc_app__prepare_game(_app);
	}

	if (obsc_event__is_key_pressed(_app->event, OBSC_KEY_ESCAPE))
	{
		if (_app->game_status != game_status_process)
		{
			_app->current_scene = scene_main_menu;
		}
		else
		{
			_app->game_status = game_status_failure;
		}
	}
}
