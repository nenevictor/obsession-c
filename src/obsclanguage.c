#include "obsclanguage.h"

void obsc_language__init(
	struct obsc_language *_language)
{
	nejson__init(
		&_language->dictionary);

	_language->languages_list_size = 0;
	_language->current_language_index = 0;
}

void obsc_language__add_default(
	struct obsc_language *_language)
{
	const char *default_language = "english";

	nejson__set_string_tree(
		&_language->dictionary,
		"English",
		"ss",
		default_language,
		"title");

	nejson__set_string_tree(
		&_language->dictionary,
		"",
		"ss",
		default_language,
		"codepoints");

	nejson__set_string_tree(
		&_language->dictionary,
		"Obsession-C",
		"sss",
		default_language,
		"dictionary",
		"title");

	nejson__set_string_tree(
		&_language->dictionary,
		"Press SPACE to continue!",
		"sss",
		default_language,
		"dictionary",
		"intro_blinking_text");

	nejson__set_string_tree(
		&_language->dictionary,
		"BEGIN",
		"sss",
		default_language,
		"dictionary",
		"button_begin");

	nejson__set_string_tree(
		&_language->dictionary,
		"SETTINGS",
		"sss",
		default_language,
		"dictionary",
		"button_settings");

	nejson__set_string_tree(
		&_language->dictionary,
		"EXIT",
		"sss",
		default_language,
		"dictionary",
		"button_exit");

	nejson__set_string_tree(
		&_language->dictionary,
		"Settings",
		"sss",
		default_language,
		"dictionary",
		"title_settings");

	nejson__set_string_tree(
		&_language->dictionary,
		"Cursor smoothing",
		"sss",
		default_language,
		"dictionary",
		"slider_cursor_smoothing");

	nejson__set_string_tree(
		&_language->dictionary,
		"Circles count",
		"sss",
		default_language,
		"dictionary",
		"slider_circles_count");

	nejson__set_string_tree(
		&_language->dictionary,
		"Circles narrowing speed",
		"sss",
		default_language,
		"dictionary",
		"slider_circles_narrowing_speed");

	nejson__set_string_tree(
		&_language->dictionary,
		"easy",
		"sss",
		default_language,
		"dictionary",
		"label_easy_mode");

	nejson__set_string_tree(
		&_language->dictionary,
		"normal",
		"sss",
		default_language,
		"dictionary",
		"label_normal_mode");

	nejson__set_string_tree(
		&_language->dictionary,
		"hard",
		"sss",
		default_language,
		"dictionary",
		"label_hard_mode");

	nejson__set_string_tree(
		&_language->dictionary,
		"Hit",
		"sss",
		default_language,
		"dictionary",
		"label_hit_count");

	nejson__set_string_tree(
		&_language->dictionary,
		"Miss",
		"sss",
		default_language,
		"dictionary",
		"label_mis_count");

	nejson__set_string_tree(
		&_language->dictionary,
		"Circles narrowing speed",
		"sss",
		default_language,
		"dictionary",
		"label_mode");

	nejson__set_string_tree(
		&_language->dictionary,
		"Target FPS",
		"sss",
		default_language,
		"dictionary",
		"slider_target_fps");

	nejson__set_string_tree(
		&_language->dictionary,
		"Use vertical synchronization",
		"sss",
		default_language,
		"dictionary",
		"checker_try_vsync");

	nejson__set_string_tree(
		&_language->dictionary,
		"USE",
		"sss",
		default_language,
		"dictionary",
		"button_use");

	nejson__set_string_tree(
		&_language->dictionary,
		"CLOSE",
		"sss",
		default_language,
		"dictionary",
		"button_close");

	nejson__set_string_tree(
		&_language->dictionary,
		"Press SPACE to begin!",
		"sss",
		default_language,
		"dictionary",
		"title_game_press_space");

	nejson__set_string_tree(
		&_language->dictionary,
		"Or press ESC to fail...",
		"sss",
		default_language,
		"dictionary",
		"text_game_press_esc");

	nejson__set_string_tree(
		&_language->dictionary,
		"Kill yourself!",
		"sss",
		default_language,
		"dictionary",
		"title_game_failure");

	nejson__set_string_tree(
		&_language->dictionary,
		"Mode",
		"sss",
		default_language,
		"dictionary",
		"label_mode");

	nejson__set_string_tree(
		&_language->dictionary,
		"Success!",
		"sss",
		default_language,
		"dictionary",
		"title_game_success");

	nejson__set_string_tree(
		&_language->dictionary,
		"ESC - main menu    R - restart",
		"sss",
		default_language,
		"dictionary",
		"text_game_result_helper");
}

int obsc_language__load(
	struct obsc_language *_language,
	const char *_path)
{
	struct nejson_iterator languages_iterator;
	struct nejson_node *temp_node;
	struct nejson_object *root_object;
	size_t index = 0;

	if (nejson__load_from_file(
			&_language->dictionary,
			_path))
	{
		obsc_language__add_default(
			_language);

		strcpy(
			_language->languages_list[0],
			"english");
		_language->languages_list_size = 1;
		_language->current_language_index = 0;

		return 1;
	}

	obsc_language__add_default(
		_language);

	root_object = nejson_variable__get_object(
		_language->dictionary.root);

	if (!root_object)
	{
		return 1;
	}

	languages_iterator = nejson_iterator__object_begin(root_object);
	temp_node = nejson_iterator__object_next_ptr(&languages_iterator);

	while (temp_node)
	{
		strcpy(
			_language->languages_list[index],
			temp_node->key);
		temp_node = nejson_iterator__object_next_ptr(&languages_iterator);
		index++;
		_language->languages_list_size++;
	}

	return 0;
}

void obsc_language__free(
	struct obsc_language *_language)
{
	nejson__free(&_language->dictionary);
}

void obsc_language__set_language(
	struct obsc_language *_language,
	const char *_language_name)
{
	size_t index = 0;

	while (index < _language->languages_list_size)
	{
		if (!strcmp(_language->languages_list[index],
					_language_name))
		{
			_language->current_language_index = index;
			return;
		}
		index++;
	}

	index = 0;
	while (index < _language->languages_list_size)
	{
		if (!strcmp(_language->languages_list[index],
					"english"))
		{
			_language->current_language_index = index;
			return;
		}
		index++;
	}

	_language->current_language_index = 0;
}

const char *obsc_language__get(
	struct obsc_language *_language,
	const char *_key)
{
	return nejson_variable__get_string_or_undefined(
		nejson__get_variable_tree(
			&_language->dictionary,
			"sss",
			_language->languages_list[_language->current_language_index],
			"dictionary",
			_key));
}

const wchar_t *obsc_language__get_wide(
	struct obsc_language *_language,
	const char *_key)
{
	return nejson_variable__get_utf16_string_or_undefined_temporary(
		nejson__get_variable_tree(
			&_language->dictionary,
			"sss",
			_language->languages_list[_language->current_language_index],
			"dictionary",
			_key));
}

const wchar_t *obsc_language__get_codepoints_string(
	struct obsc_language *_language)
{
	return nejson_variable__get_utf16_string_or_undefined_temporary(
		nejson__get_variable_tree(
			&_language->dictionary,
			"ss",
			_language->languages_list[_language->current_language_index],
			"codepoints"));
}

const wchar_t *obsc_language__get_title(
	struct obsc_language *_language)
{
	return nejson_variable__get_utf16_string_or_undefined_temporary(
		nejson__get_variable_tree(
			&_language->dictionary,
			"ss",
			_language->languages_list[_language->current_language_index],
			"title"));
}