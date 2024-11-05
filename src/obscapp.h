#ifndef OBSC_APP_H
#define OBSC_APP_H

#include <nelib/nejson.h>

#include <obscfs.h>
#include <obscgraphic.h>
#include <obscwindow.h>
#include <obscui.h>
#include <obsclanguage.h>

#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <wchar.h>

#define OBSC_ASSERT(_message)                 \
	{                                         \
		fprintf(                              \
			stderr,                           \
			"[ ERROR ]: Unreachable point.\n" \
			"  Message: %s\n"                 \
			"  File: %s\n"                    \
			"  Line: %i\n",                   \
			_message,                         \
			__FILE__,                         \
			(int)__LINE__);                   \
		fflush(stderr);                       \
		exit(1);                              \
	}

#define OBSC_APP_VERSION_MAJOR 1
#define OBSC_APP_VERSION_MINOR 0
#define OBSC_APP_VERSION_PATCH 0

#define OBSC_APP_FONT_PATH "resources/Iosevka-Heavy.ttf"
#define OBSC_APP_ICON_PATH "resources/icon.png"
#define OBSC_APP_LOGO_PATH "resources/logo.png"
#define OBSC_APP_SETTINGS_PATH  "settings.json"
#define OBSC_APP_LANGUAGES_PATH "languages.json"
#define OBSC_APP_DEFAULT_FPS 60
#define OBSC_APP_DEFAULT_FRAME_TIME (1.0f / (float)OBSC_APP_DEFAULT_FPS)
#define OBSC_APP_MAX_CIRCLES_COUNT 64
#define OBSC_APP_CIRCLE_RADIUS 6.5f
#define OBSC_APP_CURSOR_RADIUS 1.0f

#define OBSC_APP_GAME_SUCCESS_COUNT 1000

#define OBSC_APP_UI_BORDER_THICKNESS 0.75f
#define OBSC_APP_UI_TEXT_MARGIN 0.75f

#define OBSC_COLOR_WHITE \
	(struct obsc_color) { 244, 236, 236, 255 }

#define OBSC_COLOR_RED \
	(struct obsc_color) { 197, 13, 13, 255 }

#define OBSC_COLOR_YELLOW \
	(struct obsc_color) { 229, 200, 37, 255 }

#define OBSC_COLOR_BLACK \
	(struct obsc_color) { 32, 30, 30, 255 }

typedef enum obsc_app_scene_type
{
	scene_intro,
	scene_main_menu,
	scene_settings,
	scene_game
} obsc_app_scene_type;

typedef enum obsc_app_game_status_type
{
	game_status_begin,
	game_status_process,
	game_status_failure,
	game_status_success
} obsc_app_game_status_type;

typedef enum obsc_app_game_mode_type
{
	game_mode_easy = 0,
	game_mode_normal = 1,
	game_mode_hard = 2
} obsc_app_game_mode_type;

typedef struct obsc_app_game_stats
{
	int hit_count, mis_count, despair_count;
} obsc_app_game_stats;

typedef struct obsc_app_settings
{
	unsigned int window_width,
		window_height,
		target_fps,
		circles_count;
	float cursor_smoothing,
		text_paragraph_size,
		text_title_size,
		circles_narrowing_speed;
	int try_vsync;

	enum obsc_app_game_mode_type game_mode;

	char language_name[127];
} obsc_app_settings;

typedef struct obsc_app
{
	int quit;

	struct obsc_app_settings settings, temp_settings;

	struct obsc_font font;
	struct obsc_window *window;
	struct obsc_event *event;
	struct obsc_renderer2d *renderer;
	struct obsc_ui_style ui_style;
	struct obsc_texture *texture_logo;

	struct obsc_vector2 cursor_position;

	enum obsc_app_scene_type current_scene;

	enum obsc_app_game_status_type game_status;
	struct obsc_app_game_stats game_stats;

	struct obsc_vector3 circles[OBSC_APP_MAX_CIRCLES_COUNT];

	struct obsc_language language;

	double frame_being_time, frame_end_time, frame_delta;
	int window_width, window_height;

	float size_unit;
} obsc_app;

int obsc_app__random_int(
	const int _min,
	const int _max);

float obsc_app__random_float(
	const float _min,
	const float _max);

int obsc_app__circles_intersect(
	const int _x0,
	const int _y0,
	const int _r0,
	const int _x1,
	const int _y1,
	const int _r1);

void obsc_app__begin_frame(
	struct obsc_app *_app,
	const struct obsc_color _clear_color);

void obsc_app__end_frame(
	struct obsc_app *_app);

int obsc_app__reload_font(
	struct obsc_app *_app,
	const wchar_t *_required_codepoints);

void obsc_app__prepare_game(
	struct obsc_app *_app);

int obsc_app__load_settings(
	struct obsc_app *_app);

int obsc_app__save_settings(
	struct obsc_app *_app);

int obsc_app__init(
	struct obsc_app *_app);

int obsc_app__run(
	struct obsc_app *_app);

void obsc_app__close(
	struct obsc_app *_app);

void obsc_app__draw_cursor(
	struct obsc_app *_app);

void obsc_app__draw_intro_scene(
	struct obsc_app *_app);

void obsc_app__draw_menu_scene(
	struct obsc_app *_app);

void obsc_app__draw_settings_scene(
	struct obsc_app *_app);

void obsc_app__draw_game_scene(
	struct obsc_app *_app);

#endif