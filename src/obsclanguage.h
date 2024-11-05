#ifndef OBSC_LANGUAGE_H
#define OBSC_LANGUAGE_H
#include <nelib/nejson.h>
#include <string.h>

typedef struct obsc_language
{
	struct nejson dictionary;

	char languages_list[127][127];
	size_t languages_list_size;
	size_t current_language_index;
} obsc_language;

void obsc_language__init(
	struct obsc_language *_language);

void obsc_language__add_default(
	struct obsc_language *_language);

int obsc_language__load(
	struct obsc_language *_language,
	const char *_path);

void obsc_language__free(
	struct obsc_language *_language);

void obsc_language__set_language(
	struct obsc_language *_language,
	const char *_language_name);

const char *obsc_language__get(
	struct obsc_language *_language,
	const char *_key);

const wchar_t *obsc_language__get_wide(
	struct obsc_language *_language,
	const char *_key);

const wchar_t *obsc_language__get_codepoints_string(
	struct obsc_language *_language);

const wchar_t *obsc_language__get_title(
	struct obsc_language *_language);

#endif