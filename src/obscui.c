#include "obscui.h"

struct obsc_vector2 obsc_vector2__follow(
	const struct obsc_vector2 _object_position,
	const struct obsc_vector2 _following_position,
	const float _speed)
{
	struct obsc_vector2 result;
	const float clamped_speed = OBSC_CLAMP(_speed, 0.0f, 1.0f);

	result.x = _following_position.x + (_object_position.x - _following_position.x) * clamped_speed;
	result.y = _following_position.y + (_object_position.y - _following_position.y) * clamped_speed;

	return result;
}

int obsc_ui__is_point_in_rect(
	const float _rect_x,
	const float _rect_y,
	const float _rect_w,
	const float _rect_h,
	const float _point_x,
	const float _point_y)
{
	return (_point_x >= _rect_x && _point_x <= _rect_x + _rect_w &&
			_point_y >= _rect_y && _point_y <= _rect_y + _rect_h);
}

void obsc_ui__measure_text_wide(
	const struct obsc_font *_font,
	const wchar_t *_text,
	const size_t _size,
	float *_out_w,
	float *_out_h)
{
	const wchar_t *temp_char;
	const struct nestbtt_codepoint_box *codepoint_box;
	struct obsc_vector4 position;
	float scale, lines;
	int texture_width, texture_height;

	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;
	position.w = 0.0f;
	temp_char = _text;
	scale = (float)_size / (float)_font->font.size;
	lines = 0.0f;

	obsc_texture__get_size(
		_font->texture,
		&texture_width,
		&texture_height);

	while (*temp_char)
	{
		codepoint_box = nestbtt_getCodepointBox(
			*temp_char,
			&_font->font);

		if (!codepoint_box || *temp_char == ' ')
		{
			position.x += (_font->font.space_width * texture_width) * scale;
		}

		if (*temp_char == '\n')
		{
			position.x = 0;
			lines += _size;
		}

		if (codepoint_box && *temp_char > ' ')
		{
			position = (struct obsc_vector4){
				.x = position.x,
				.y = lines + codepoint_box->baseline * (float)texture_height * scale,
				.z = codepoint_box->w * (float)texture_width * scale,
				.w = codepoint_box->h * (float)texture_height * scale,
			};
			position.x += codepoint_box->w * (float)texture_width * scale;
		}

		temp_char++;
	}

	if (position.w == 0.0f)
	{
		position.w = _size;
	}

	*_out_w = position.x;
	*_out_h = position.y + position.w;
}

int obsc_ui__button_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_event *_event,
	const struct obsc_ui_style *_style,
	const wchar_t *_text,
	const float _x,
	const float _y,
	const float _w,
	const float _h)
{
	int result;
	static int previous = 0;
	float text_x, text_y;
	float rect_w, rect_h;
	struct obsc_color background_color, border_color, text_color;

	/* Calculate sizes. */
	obsc_ui__measure_text_wide(
		_style->font,
		_text,
		_style->text_paragraph_size,
		&text_x,
		&text_y);
	rect_w = text_x + _style->text_margin * 2.0f + _style->border_thickness * 2.0f;
	rect_h = text_y + _style->text_margin * 2.0f + _style->border_thickness * 2.0f;
	rect_w = OBSC_CLAMP(rect_w, _w, rect_w);
	rect_h = OBSC_CLAMP(rect_h, _h, rect_h);
	text_x = _x + (rect_w / 2.0f - text_x / 2.0f) *
					  _style->text_transpose_to_center_x;
	text_y = _y + (rect_h / 2.0f - text_y / 2.0f) *
					  _style->text_transpose_to_center_y;

	result = 0;

	if (obsc_ui__is_point_in_rect(
			_x, _y, rect_w, rect_h,
			_event->cursor_x, _event->cursor_y))
	{
		if (obsc_event__is_cursor_button_down(_event, 0) ||
			obsc_event__is_key_pressed(_event, OBSC_KEY_SPACE))
		{
			background_color = _style->background_color_pressed;
			border_color = _style->border_color_pressed;
			text_color = _style->text_color_pressed;

			if (!previous)
			{
				result = 1;
				previous = 1;
			}
		}
		else
		{
			background_color = _style->background_color_hovered;
			border_color = _style->border_color_hovered;
			text_color = _style->text_color_hovered;
			previous = 0;
		}
	}
	else
	{
		background_color = _style->background_color_default;
		border_color = _style->border_color_default;
		text_color = _style->text_color_default;
	}

	/* Drawing section. */
	obsc_renderer2d__draw_quad(
		_renderer,
		_x,
		_y,
		rect_w,
		rect_h,
		0.0f,
		_style->border_thickness,
		background_color,
		border_color,
		NULL,
		(struct obsc_vector4){0.0f, 0.0f, 0.0f, 0.0f},
		0);

	obsc_renderer2d__draw_text_wide(
		_renderer,
		_style->font,
		_text,
		_style->text_paragraph_size,
		text_color,
		text_x,
		text_y);

	return result;
}

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
	const float _text_transpose_to_center_y)
{
	float text_x, text_y;

	/* Calculate sizes. */
	obsc_ui__measure_text_wide(
		_font,
		_text,
		_size,
		&text_x,
		&text_y);

	text_x *= 1.0f / _text_transpose_to_center_x;
	text_y *= 1.0f / _text_transpose_to_center_y;

	if (OBSC_HAS_FLAG(_alignment_flags, text_alignment_left))
	{
		text_x = _x;
	}
	if (OBSC_HAS_FLAG(_alignment_flags, text_alignment_right))
	{
		text_x = _x - text_x;
	}
	if (OBSC_HAS_FLAG(_alignment_flags, text_alignment_top))
	{
		text_y = _y;
	}
	if (OBSC_HAS_FLAG(_alignment_flags, text_alignment_down))
	{
		text_y = _y - text_y;
	}
	if (OBSC_HAS_FLAG(_alignment_flags, text_alignment_center_x))
	{
		text_x = _x - text_x / 2.0f;
	}
	if (OBSC_HAS_FLAG(_alignment_flags, text_alignment_center_y))
	{
		text_y = _y - text_y / 2.0f;
	}

	obsc_renderer2d__draw_text_wide(
		_renderer,
		_font,
		_text,
		_size,
		_color,
		text_x,
		text_y);
}

void obsc_ui__title_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_ui_style *_style,
	const wchar_t *_text,
	const enum obsc_ui_text_alignment_type _alignment_flags,
	const float _x,
	const float _y)
{
	obsc_ui__text_wide(
		_renderer,
		_style->font,
		_style->text_color_default,
		_style->text_title_size,
		_text,
		_alignment_flags,
		_x,
		_y,
		_style->text_transpose_to_center_x,
		_style->text_transpose_to_center_y);
}

void obsc_ui__paragraph_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_ui_style *_style,
	const wchar_t *_text,
	const enum obsc_ui_text_alignment_type _alignment_flags,
	const float _x,
	const float _y)
{
	obsc_ui__text_wide(
		_renderer,
		_style->font,
		_style->text_color_default,
		_style->text_paragraph_size,
		_text,
		_alignment_flags,
		_x,
		_y,
		_style->text_transpose_to_center_x,
		_style->text_transpose_to_center_y);
}

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
	float *_value)
{
	static int previous = 0;
	int result = 0;

	struct obsc_vector4 slide_bounds;
	struct obsc_color background_color, border_color;

	float new_value = OBSC_CLAMP(*_value, _min, _max);

	slide_bounds.x = ((_x + _height / 2) + (_width - _height) * (new_value - _min) / (_max - _min)) - _height / 2.0f,
	slide_bounds.y = _y;
	slide_bounds.z = _height;
	slide_bounds.w = _height;

	if (obsc_ui__is_point_in_rect(
			slide_bounds.x,
			slide_bounds.y,
			slide_bounds.z,
			slide_bounds.w,
			_event->cursor_x,
			_event->cursor_y))
	{
		if (obsc_event__is_cursor_button_down(_event, 0) ||
			obsc_event__is_key_down(_event, OBSC_KEY_SPACE))
		{
			background_color = _style->background_color_pressed;
			border_color = _style->border_color_pressed;

			new_value = _min + ((_event->cursor_x - (_x + _height / 2)) / (_width - _height)) * (_max - _min);

			if (!previous)
			{
				result = 1;
				previous = 1;
			}
		}
		else
		{
			background_color = _style->background_color_hovered;
			border_color = _style->border_color_hovered;
			previous = 0;
		}
	}
	else
	{
		background_color = _style->background_color_default;
		border_color = _style->border_color_default;
	}

	*_value = OBSC_CLAMP(new_value, _min, _max);

	/* Center line. */
	obsc_renderer2d__draw_quad(
		_renderer,
		_x,
		_y + _height / 2.0f - _style->border_thickness / 2,
		_width,
		_style->border_thickness,
		0,
		0,
		border_color,
		(struct obsc_color){0, 0, 0, 0},
		0,
		(struct obsc_vector4){0, 0, 0, 0},
		0);

	/* Slide button. */
	obsc_renderer2d__draw_quad(
		_renderer,
		slide_bounds.x,
		slide_bounds.y,
		slide_bounds.z,
		slide_bounds.w,
		0,
		_style->border_thickness,
		background_color,
		border_color,
		0,
		(struct obsc_vector4){0, 0, 0, 0},
		0);

	obsc_ui__paragraph_wide(
		_renderer,
		_style,
		_text,
		text_alignment_center_y | text_alignment_left,
		_x + _width + _style->text_paragraph_size,
		_y + _style->text_paragraph_size / 2.0f);

	return result;
}

int obsc_ui__checker_wide(
	struct obsc_renderer2d *_renderer,
	const struct obsc_event *_event,
	const struct obsc_ui_style *_style,
	const wchar_t *_text,
	const float _x,
	const float _y,
	const float _size,
	int *_value)
{
	static int previous = 0;
	int result = 0;

	struct obsc_color background_color, border_color;

	if (obsc_ui__is_point_in_rect(
			_x,
			_y,
			_size,
			_size,
			_event->cursor_x,
			_event->cursor_y))
	{
		if (obsc_event__is_cursor_button_down(_event, 0) ||
			obsc_event__is_key_down(_event, OBSC_KEY_SPACE))
		{
			background_color = _style->background_color_pressed;
			border_color = _style->border_color_pressed;

			if (!previous)
			{
				result = 1;
				previous = 1;

				*_value = !*_value;
			}
		}
		else
		{
			background_color = _style->background_color_hovered;
			border_color = _style->border_color_hovered;
			previous = 0;
		}
	}
	else
	{
		background_color = _style->background_color_default;
		border_color = _style->border_color_default;
	}

	/* Border. */
	obsc_renderer2d__draw_quad(
		_renderer,
		_x,
		_y,
		_size,
		_size,
		0,
		_style->border_thickness,
		background_color,
		border_color,
		0,
		(struct obsc_vector4){0, 0, 0, 0},
		0);

	/* Center. */
	if (*_value)
	{
		obsc_renderer2d__draw_quad(
			_renderer,
			_x + _style->border_thickness * 1.1f,
			_y + _style->border_thickness * 1.1f,
			_size - _style->border_thickness * 2.2f,
			_size - _style->border_thickness * 2.2f,
			0,
			0,
			border_color,
			(struct obsc_color){0, 0, 0, 0},
			0,
			(struct obsc_vector4){0, 0, 0, 0},
			0);
	}

	obsc_ui__paragraph_wide(
		_renderer,
		_style,
		_text,
		text_alignment_center_y | text_alignment_left,
		_x + _size + _style->text_paragraph_size,
		_y + _style->text_paragraph_size / 2.0f);

	return result;
}