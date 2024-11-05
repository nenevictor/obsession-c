#ifndef OBSCUI_H
#define OBSCUI_H

#include "obscgraphic.h"
#include "obscwindow.h"

#define OBSC_CLAMP(_x, _a, _b) (((_x) < (_a)) ? (_a) : (((_x) > (_b)) ? (_b) : (_x)))
#define OBSC_MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))
#define OBSC_HAS_FLAG(_value, _flag) (((_value) & (_flag)) == (_flag))

typedef enum obsc_ui_text_alignment_type
{
	text_alignment_none = 1 << 0,
	text_alignment_left = text_alignment_none,
	text_alignment_right = 1 << 1,
	text_alignment_top = 1 << 2,
	text_alignment_down = 1 << 3,
	text_alignment_center_x = 1 << 4,
	text_alignment_center_y = 1 << 5,
	text_alignment_center = text_alignment_center_x | text_alignment_center_y,
} obsc_ui_text_alignment_type;

typedef struct obsc_ui_style
{
	struct obsc_font *font;
	struct obsc_color text_color_default,
		text_color_hovered,
		text_color_pressed;
	struct obsc_color background_color_default,
		background_color_hovered,
		background_color_pressed;
	struct obsc_color border_color_default,
		border_color_hovered,
		border_color_pressed;

	float border_thickness,
		text_paragraph_size,
		text_title_size,
		text_margin,
		text_transpose_to_center_x,
		text_transpose_to_center_y;
} obsc_ui_style;

struct obsc_vector2 obsc_vector2__follow(
	const struct obsc_vector2 _object_position,
	const struct obsc_vector2 _following_position,
	const float _speed);

int obsc_ui__is_point_in_rect(
	const float _rect_x,
	const float _rect_y,
	const float _rect_w,
	const float _rect_h,
	const float _point_x,
	const float _point_y);

void obsc_ui__measure_text_wide(
	const struct obsc_font *_font,
	const wchar_t *_text,
	const size_t _size,
	float *_out_w,
	float *_out_h);

int obsc_ui__button_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_event *_event,
	const struct obsc_ui_style *_style,
	const wchar_t *_text,
	const float _x,
	const float _y,
	const float _w,
	const float _h);

void obsc_ui__text_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_font *_font,
	const struct obsc_color _color,
	const size_t _size,
	const wchar_t *_text,
	const enum obsc_ui_text_alignment_type _alignment_flags,
	const float _x,
	const float _y,
	const float _text_transpose_to_center_x,
	const float _text_transpose_to_center_y);

void obsc_ui__title_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_ui_style *_style,
	const wchar_t *_text,
	const enum obsc_ui_text_alignment_type _alignment_flags,
	const float _x,
	const float _y);

void obsc_ui__paragraph_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_ui_style *_style,
	const wchar_t *_text,
	const enum obsc_ui_text_alignment_type _alignment_flags,
	const float _x,
	const float _y);

int obsc_ui__slider_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_event *_event,
	const struct obsc_ui_style *_style,
	const wchar_t *_text,
	const float _x,
	const float _y,
	const float _width,
	const float _height,
	const float _min,
	const float _max,
	float *_value);

int obsc_ui__checker_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_event *_event,
	const struct obsc_ui_style *_style,
	const wchar_t *_text,
	const float _x,
	const float _y,
	const float _size,
	int *_value);

#endif