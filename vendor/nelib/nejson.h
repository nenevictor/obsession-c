/*
 * 1.0.0
 * nejson - JSON parsing library. By nenevictor (shyvikaisinlove)
 * _________________________________________________________________
 * |                                                                 |
 * |   ###   ### #########     #####  #######   #######  ###   ###   |
 * |   ####  ### ###     #       ### ###    ## ###   ### ####  ###   |
 * |   ##### ### ######          ###   ###     ###   ### ##### ###   |
 * |   ### ##### ###    ##       ###     ###   ###   ### ### #####   |
 * |   ###  #### ###     # ###   ### ##    ### ###   ### ###  ####   |
 * |   ###   ### #########  #######   #######   #######  ###   ###   |
 * |_________________________________________________________________|
 *
 * Peace!
 * This is "nejson" - "STB"-styled, small and simple library for
 * working with json documents in C99.
 * I stole some ideas and code from "stb_c_lexer" and other "stb"
 * libraries.
 * Also I added cool ASCII-art, so hopefully this json parser
 * library will work better.
 * This library completely unfinished. By this point, it is really
 * unstable.
 * Also, i didn't finished test for this library.
 *
 * ------------------------------------------------------------------------------
 * This software is available under 2 licenses -- choose whichever you prefer.
 * ------------------------------------------------------------------------------
 * ALTERNATIVE A - MIT License
 * Copyright (c) 2024 nenevictor
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ------------------------------------------------------------------------------
 * ALTERNATIVE B - Public Domain (www.unlicense.org)
 * This is free and unencumbered software released into the public domain.
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ------------------------------------------------------------------------------
 *
 * Libary change dates:
 *   26.10.2024: library working!
 *
 *
 * TODO:
 *      create "nejson_variable__copy" function.
 *      create "nejson_variable__set_null" function.
 *      create "nejson_variable__is_null" function.
 *      create "nejson_array__insert" function.
 *      create "nejson_array__erase" function.
 *      create and add NEJSON_DEF to functions.
 *      create and add NEJSON_APш to functions.
 *      create complete "nejson_test". file.
 */

#ifndef NEJSON_H
#define NEJSON_H

#if !defined(NEJSON_ASSERT)
#include <assert.h>
#define NEJSON_ASSERT(_message) _assert(_message, __FILE__, __LINE__)
#endif

#if !defined(NEJSON_MALLOC) || !defined(NEJSON_REALLOC) || !defined(NEJSON_FREE)
#include <malloc.h>
#endif

#if !defined(NEJSON_MALLOC)
#define NEJSON_MALLOC(_size) malloc(_size)
#endif

#if !defined(NEJSON_REALLOC)
#define NEJSON_REALLOC(_memory, _new_size) realloc(_memory, _new_size)
#endif

#if !defined(NEJSON_FREE)
#define NEJSON_FREE(_memory) free(_memory)
#endif

#define nejson_size_t unsigned long
#define nejson_ssize_t long

#if !defined(NEJSON_OBJECT_CHUNK_SIZE)
#define NEJSON_OBJECT_CHUNK_SIZE 512
#endif
#if !defined(NEJSON_TOKENS_CHUNK_SIZE)
#define NEJSON_TOKENS_CHUNK_SIZE 512
#endif
#if !defined(NEJSON_STRING_CHUNK_SIZE)
#define NEJSON_STRING_CHUNK_SIZE 127
#endif

#if !defined(NEJSON_MAX_TREE_DEPTH)
#define NEJSON_MAX_TREE_DEPTH 1024
#endif

#if !defined(NEJSON_STRING_TAB_SIZE)
#define NEJSON_STRING_TAB_SIZE 3
#endif
#if !defined(NEJSON_STRING_FLOAT_SYMBOLS_AFTER_COMMA)
#define NEJSON_STRING_FLOAT_SYMBOLS_AFTER_COMMA 6
#endif
#if !defined(NEJSON_STRING_TEMPORARY_BUFFER_SIZE)
#define NEJSON_STRING_TEMPORARY_BUFFER_SIZE 2028
#endif

#define NEJSON_UNDEFINED_STRING "undefined"
#define NEJSON_UNDEFINED_STRING_UTF16 L"undefined"

#define NEJSON_NULL ((void *)0)
#define NEJSON_TRUE 1
#define NEJSON_FALSE 0
#define NEJSON_SUCCESS 0
#define NEJSON_FAILURE 1

#define NEJSON_CALCULATE_RESERVE(_chunk_size, _buffer_size) (((nejson_size_t)((float)(_buffer_size) / (float)(_chunk_size)) + 1) * _chunk_size)
#define NEJSON_ABS(_x) ((_x) < 0 ? -(_x) : (_x))

/*
#define NEJSON_NO_FORMAT_TREE
#define NEJSON_NO_STDIO
#define NEJSON_NO_UTF16
*/

/* I stole this code from "stb_image.h". */
#if !defined(NEJSON_NO_UTF16)
#include <stdlib.h>
#if defined(_WIN32)
__declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int cp, unsigned long flags, const char *str, int cbmb, wchar_t *widestr, int cchwide);
__declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int cp, unsigned long flags, const wchar_t *widestr, int cchwide, char *str, int cbmb, const char *defchar, int *used_default);
#endif
#endif

#if !defined(NEJSON_NO_FORMAT_TREE)
#include <stdarg.h>
#endif

#if !defined(NEJSON_NO_STDIO)
#include <stdio.h>
#endif

typedef enum nejson_token_type
{
  nejson_token_type_none,
  nejson_token_type_unknown,
  nejson_token_type_true_literal,
  nejson_token_type_false_literal,
  nejson_token_type_integer,
  nejson_token_type_float,
  nejson_token_type_string,
  nejson_token_type_null_literal,
  nejson_token_type_object_begin,
  nejson_token_type_object_end,
  nejson_token_type_array_begin,
  nejson_token_type_array_end,
  nejson_token_type_assign,
  nejson_token_type_separator,
  nejson_token_type_comment_close,
  nejson_token_type_comment_open
} nejson_token_type;

typedef struct nejson_da_string
{
  char *data;
  nejson_size_t capacity;
  nejson_size_t size;
} nejson_da_string;

typedef struct nejson_token
{
  nejson_token_type type;
  const char *begin;
  nejson_size_t length;
} nejson_token;

typedef struct nejson_tokens
{
  nejson_token *data;
  nejson_size_t capacity;
  nejson_size_t size;
} nejson_tokens;

typedef enum nejson_variable_type
{
  nejson_variable_type_undefined,
  nejson_variable_type_boolean,
  nejson_variable_type_integer,
  nejson_variable_type_float,
  nejson_variable_type_string,
  nejson_variable_type_object,
  nejson_variable_type_array,
  nejson_variable_type_null
} nejson_variable_type;

typedef union nejson_variable_variants
{
  int variable_integer;
  float variable_float;
  void *variable_ptr;
} nejson_variable_variants;

typedef struct nejson_variable
{
  enum nejson_variable_type type;
  union nejson_variable_variants variant;
} nejson_variable;

typedef struct nejson_node
{
  char *key;
  struct nejson_variable variable;
} nejson_node;

typedef struct nejson_object
{
  struct nejson_node *data;
  nejson_size_t capacity;
  nejson_size_t size;
} nejson_object;

typedef struct nejson_array
{
  struct nejson_variable *data;
  nejson_size_t capacity;
  nejson_size_t size;
} nejson_array;

typedef struct nejson_iterator
{
  void *object;
  enum nejson_variable_type type;
  nejson_size_t index;
} nejson_iterator;

typedef struct nejson
{
  struct nejson_variable root;
  char info[NEJSON_STRING_TEMPORARY_BUFFER_SIZE];

  int out_of_memory;
  int rewriting_allowed;
} nejson;

/* Functions right here! */
#ifdef __cplusplus
extern "C"
{
#endif

  /* Memory. */
  /*
   * Fill memory chunk with data (memset alternative).
    @param[in] _data pointer to data.
    @param[in] _value value
    @param[in] _size size of data structure in bytes.
   */
  void nejson_memory__fill(
      void *_data,
      const unsigned char _value,
      const nejson_size_t _size);

  /* String. */

  /*
   * Measure string length.
   * @param[in] _string string.
   * @returns length of a string.
   */
  nejson_size_t nejson_string__length(const char *_string);

#if !defined(NEJSON_NO_STDIO)
  /*
   * Read file as a text.
   * @param[in] _path path to file.
   * @returns dynamicly allocated string or NEJSON_NULL.
   */
  char *nejson_string__load_from_file(const char *_path);

  /*
   * Save string as a file.
   * @param[in] _path path to file.
   * @param[in] _string string data.
   * @param[in] _string_size string size.
   * @returns NEJSON_SUCCESS if file saved successfully.
   */
  int nejson_string__save_to_file(
      const char *_path,
      const char *_string,
      const nejson_size_t _string_size);

#endif

  /*
   * Copy string.
   * @param[in] _string input string.
   * @returns copied dynamicly allocated string.
   */
  char *nejson_string__copy(
      const char *_string);

  /*
   * Copy string of certain _length.
   * @param[in] _string input string.
   * @param[in] _length length of input string.
   * @returns copied dynamicly allocated string.
   */
  char *nejson_string__copy_length(
      const char *_string,
      const nejson_size_t _length);

  /*
   * Dynamically reserve space for the string.
   * @param[in] _string_dst pointer to input string.
   * @param[in] _length length of input string.
   * @param[in/out] _capacity pointer to string capacity
   *                (actual allocated size).
   * @returns NEJSON_SUCCESS if space was reserved successfully.
   */

  int nejson_string__reserve(
      char **_string_dst,
      nejson_size_t _length,
      nejson_size_t *_capacity);

  /*
   * Get column and line count for a cursor of a string
   * @param[in] _string input string.
   * @param[in] _cursor cursor position.
   * @param[out] _lines pointer to lines count.
   * @param[out] _column pointer to columns count.
   */
  void nejson_string__get_cursor_line_and_column(
      const char *_string,
      const nejson_size_t _cursor,
      nejson_size_t *_lines,
      nejson_size_t *_columns);

  /*
   * Append postfix string to main string.
   * @param[in/out] _string destination string.
   * @param[in] postfix string.
   */
  void nejson_string__append(
      char *_string,
      const char *_prefix);

  /*
   * Begin string.
   * @param[in/out] _destination destination string.
   * @param[in] _content new content.
   */
  void nejson_string__begin(
      char *_destination,
      const char *_content);

  /*
   * Compare string a with string b.
   * @param[in] _string_a string a.
   * @param[in] _string_b string b.
   * @returns true, if strings are the same.
   */
  int nejson_string__compare(
      const char *_string_a,
      const char *_string_b);

  /*
   * Check if string begins with prefix.
   * @param[in] _string string a.
   * @param[in] _prefix string b.
   * @returns true, if string begins with prefix.
   */
  int nejson_string__begins_with(
      const char *_string,
      const char *_prefix);

  /*
   * Check if char is float number.
   * @param[in] _char char to check.
   * @returns true, if char is float number.
   */
  int nejson_string__is_char_float_number(const char _char);

  /*
   * Check if char is white space.
   * @param[in] _char char to check.
   * @return true, if char is white space.
   */
  int nejson_string__is_char_white_space(const char _char);

  /*
   * Check if char is specific json symbol.
   * @param[in] _char char to check.
   * @return true, if char is json specific symbol.
   */
  int nejson_string__is_char_json_token(const char _char);

#if !defined(NEJSON_NO_UTF16)

  /*
   * Measure wide-string (utf16) length.
   * @param[in] _string string.
   * @returns length of a string.
   */
  nejson_size_t nejson_string__utf16_length(
      const wchar_t *_string);

  /*
   * Convert utf8 (multibyte) string to utf16 (wide) string.
   * @param[in] _utf8 input utf8 string.
   * @param[out] _utf16 output utf16 string.
   * @param[in] _utf16_buffer_size input utf8 string buffer
   *            size (capacity).
   */
  int nejson_string__utf8_to_utf16(
      const char *_utf8,
      wchar_t *_utf16,
      const nejson_size_t _utf16_buffer_size);

  /*
   * Convert utf16 (wide) string to utf8 (multibyte) string.
   * @param[in] _utf16 input utf16 string.
   * @param[out] _utf8 output utf8 string.
   * @param[in] _utf8_buffer_size input utf8 string buffer
   *            size (capacity).
   */
  int nejson_string__utf16_to_utf8(
      const wchar_t *_utf16,
      char *_utf8,
      const nejson_size_t _utf8_buffer_size);

  /*
   * Create utf8 string from utf16 string.
   * @param[in] _string input utf16 string.
   * @returns copied dynamicly allocated utf8 string.
   */
  char *nejson_string__create_from_utf16(
      const wchar_t *_string);

  /*
   * Create utf16 string using static temporary buffer.
   * @param[in] _string input utf8 string.
   * @returns temporary utf 16 string.
   * @warning this function is not thread save.
   */
  wchar_t *nejson_string__utf8_to_utf16_temporary(
      const char *_string);

#endif

  /*
   * Convert string to integer.
   * @param[in] _string string that will be converted.
   * @param[out] _string_end pointer to symbol, that ended
   * 			   number.
   * @return integer.
   */
  nejson_ssize_t nejson_string__to_integer(
      const char *_string,
      const char **_string_end);

  /*
   * Convert string to double.
   * @param[in] _string string that will be converted.
   * @param[out] _string_end pointer to symbol, that ended
   * 			   number.
   * @return double.
   */
  double nejson_string__to_double(
      const char *_string,
      const char **_string_end);

  /*
   * Convert integer to string.
   * @param[out] _output_string string that will be filled with number.
   * @param[out] _output_string_end pointer to symbol after the number.
   * @param[in] _integer_value integer value that will be written to string.
   */
  void nejson_string__from_integer(
      char *_output_string,
      char **_output_string_end,
      const nejson_ssize_t _integer_value);

  /*
   * Convert double to string.
   * @param[out] _output_string string that will be filled with number.
   * @param[out] _output_string_end pointer to symbol after the number.
   * @param[in] _double_value double value that will be written to string.
   * @param[in] _after_comma number of symbols after comma.
   */
  void nejson_string__from_double(
      char *_output_string,
      char **_output_string_end,
      const double _double_value,
      const nejson_size_t _after_comma);

  /* Dynamic string. */

  /*
   * Initialize dynamic string.
   * @param[in/out] _string dynamic string.
   */
  void nejson_da_string__init(
      struct nejson_da_string *_string);

  /*
   * Free dynamic string.
   * @param[in/out] _string dynamic string.
   */
  void nejson_da_string__free(
      struct nejson_da_string *_string);

  /*
   * Reserve dynamic string space.
   * @param[in/out] _string dynamic string.
   * @returns NEJSON_SUCCESS if space reserved successfully.
   */
  int nejson_da_string__append_reserve(
      struct nejson_da_string *_string,
      const nejson_size_t _size);

  /*
   * Get dynamic string data.
   * @param[in/out] _string dynamic string.
   * @returns string or NEJSON_NULL.
   */
  char *nejson_da_string__data(
      struct nejson_da_string *_string);

  /*
   * Add substring to dynamic string.
   * @param[in/out] _string dynamic string.
   * @param[in] _append_string string that will be appended.
   * @returns NEJSON_SUCCESS if substring appended successfully.
   */
  int nejson_da_string__append_string(
      struct nejson_da_string *_string,
      const char *_append_string);

  /*
   * Add symbol to dynamic string.
   * @param[in/out] _string dynamic string.
   * @param[in] _append_symbol symbol that will be appended.
   * @returns NEJSON_SUCCESS if symbol appended successfully.
   */
  int nejson_da_string__append_symbol(
      struct nejson_da_string *_string,
      const char _append_symbol);

  /*
   * Add certain amount of spaces.
   * @param[in/out] _string dynamic string.
   * @param[in] _spaces_count spaces count.
   * @returns NEJSON_SUCCESS if spaces appended successfully.
   */
  int nejson_da_string__append_spaces(
      struct nejson_da_string *_string,
      const nejson_size_t _spaces_count);

  /*
   * Add double value to dynamic string.
   * @param[in/out] _string dynamic string.
   * @param[in] _double_value double value.
   * @returns NEJSON_SUCCESS if double value appended successfully.
   */
  int nejson_da_string__append_double(
      struct nejson_da_string *_string,
      const double _double_value);

  /*
   * Add integer value to dynamic string.
   * @param[in/out] _string dynamic string.
   * @param[in] _integer_value integer value.
   * @returns NEJSON_SUCCESS if integer value appended successfully.
   */
  int nejson_da_string__append_integer(
      struct nejson_da_string *_string,
      const nejson_ssize_t _integer_value);

  /* Variable. */

  /*
   * Create undefined variable.
   * @returns variable of a type "undefined"
   */
  struct nejson_variable nejson_variable__undefined();

  /*
   * Create integer variable.
   * @param[in] _integer_value integer that will be stored.
   * @returns variable that contains integer.
   */
  struct nejson_variable nejson_variable__set_integer(
      const int _integer_value);

  /*
   * Get integer from a variable.
   * @param[in] _variable that contains integer.
   * @returns integer.
   */
  int nejson_variable__get_integer(
      const struct nejson_variable _variable);

  /*
   * Create float variable.
   * @param[in] _float_value float that will be stored.
   * @returns variable that contains float.
   */
  struct nejson_variable nejson_variable__set_float(
      const float _float_value);

  /*
   * Get float from a variable.
   * @param[in] _variable that contains float.
   * @returns float.
   */
  float nejson_variable__get_float(
      const struct nejson_variable _variable);

  /*
   * Create variable that contains string.
   * @param[in] _string string.
   * @returns variable with copied string.
   */
  struct nejson_variable nejson_variable__set_string(
      const char *_string);

  /*
   * Get string from a variable.
   * @param[in] _variable that stores string.
   * @returns string or NEJSON_NULL.
   */
  const char *nejson_variable__get_string(
      const struct nejson_variable _variable);

  /*
   * Set boolean value to a variable.
   * @param[in] _boolean logical value that will be contained.
   * @returns variable.
   */
  struct nejson_variable nejson_variable__set_boolean(
      const int _boolean);
  /*
   * Get boolean from a variable.
   * @param[in] _variable that stores boolean.
   * @returns boolean value.
   */
  int nejson_variable__get_boolean(
      const struct nejson_variable _variable);

  /*
   * Get string from a variable.
   * @param[in] _variable variable that contains string
   * @returns string or NEJSON_UNDEFINED_STRING
   */
  const char *nejson_variable__get_string_or_undefined(
      const struct nejson_variable _variable);

#if !defined(NEJSON_NO_UTF16)
  /*
   * Create variable that contains wide string.
   * @param[in] _string wide string.
   * @returns variable with copied wide string.
   */
  struct nejson_variable nejson_variable__set_utf16_string(
      const wchar_t *_string);

  /*
   * Get temporary utf16 string from a variable.
   * @param[in] _variable variable that contains utf16 string
   * @returns string or NEJSON_NULL
   */

  wchar_t *nejson_variable__get_utf16_string_temporary(
      const struct nejson_variable _variable);

  /*
   * Get temporary utf16 string from a variable or "undefined".
   * @param[in] _variable variable that contains utf16 string
   * @returns string or NEJSON_UNDEFINED_STRING_UTF16
   */
  wchar_t *nejson_variable__get_utf16_string_or_undefined_temporary(
      const struct nejson_variable _variable);
#endif

  /*
   * Create variable that contains array.
   * @param[in] _array array.
   * @returns variable with array.
   */
  struct nejson_variable nejson_variable__set_array(
      struct nejson_array *_array);

  /*
   * Get array from a variable.
   * @param[in] _variable that stores array.
   * @returns array or NEJSON_NULL.
   */
  void *nejson_variable__get_array(
      const struct nejson_variable _variable);

  /*
   * Set object to a variable.
   * @param[in] _object object that will be contained by
   *            variable.
   */
  struct nejson_variable nejson_variable__set_object(
      struct nejson_object *_object);

  /*
   * Get object from a variable.
   * @param[in] _variable that stores object.
   * @returns object or NEJSON_NULL.
   */
  struct nejson_object *nejson_variable__get_object(
      const struct nejson_variable _variable);

  /*
   * Free variable.
   * @param[in/out] _variable variable that will be released.
   * @note _variable will be setted as undefined.
   */
  void nejson_variable__free(
      struct nejson_variable *_variable);

  /* Tokens. */

  /*
   * Initialize tokens structure fields.
   * @param[in] _tokens pointer to tokens object..
   */
  void nejson_tokens__init(
      struct nejson_tokens *_tokens);

  /*
   * Free tokens.
   * @param[in] _tokens pointer to tokens object.
   */
  void nejson_tokens__free(
      struct nejson_tokens *_tokens);

  /*
   * Add token to tokens struct.
   * @param[in] _tokens pointer to tokens object.
   * @param[in] _token token that will be added.
   * @returns NEJSON_SUCCESS if token added successfully.
   */
  int nejson_tokens__add(
      struct nejson_tokens *_tokens,
      const struct nejson_token _token);

  /*
   * Split string into json tokens.
   * @param[in] _json_string json string.
   * @returns nejson_tokens struct, that has prepared tokens.
   * @warning function do not copy string, so do not free
   *          _json_string during work with
   *          tokens.
   */
  struct nejson_tokens nejson_tokens__tokenize(
      const char *_json_string);

  /*
   * Validate tokens.
   * @param[in]  _tokens tokens object.
   * @param[in]  _json_string json string. Needed only for
   *             validation info, so can be NEJSON_NULL.
   * @param[out] _info pointer to string with validation info
   *             or NEJSON_NULL.
   * @returns false if validation was successful.
   */
  int nejson_tokens__validate(
      const struct nejson_tokens *_tokens,
      const char *_json_string,
      char *_info);

  /* Object. */

  /*
   * Init usual "json" usual object.
   * @param[out] _object pointer to "json_object" pointer.
   * @returns NEJSON_SUCCESS if initialization was successful.
   */
  int nejson_object__init(
      struct nejson_object **_object);

  /*
   * Free object.
   * @param[in/out] _object pointer to object. Will be set as NEJSON_NULL.
   */
  void nejson_object__free(
      struct nejson_object **_object);

  /*
   * Get data from "json_object" by key.
   * @param[in] _object pointer "json_object".
   * @param[in] _key string key.
   * @returns variable.
   */
  struct nejson_variable nejson_object__get(
      struct nejson_object *_object,
      const char *_key);

  /*
   * Get pointer to data from "json_object" by key.
   * @param[in] _object pointer "json_object".
   * @param[in] _key string key.
   * @returns poiter to variable or NEJSON_NULL.
   */
  struct nejson_variable *nejson_object__get_ptr(
      struct nejson_object *_object,
      const char *_key);

  /*
   * Set data to "json_object" using prepared node.
   * @param[in] _object pointer "json_object".
   * @param[in] _node string key.
   * @returns NEJSON_SUCCESS if variable setted successfully.
   * @warning this function do not copy node, so be careful.
   */
  int nejson_object__set_node(
      struct nejson_object *_object,
      const int _is_rewriting_allowed,
      const struct nejson_node _node);

  /*
   * Set data to "json_object" by key.
   * @param[in] _object pointer "json_object".
   * @param[in] _key string key.
   * @param[in] _variable variable that will be added.
   * @returns NEJSON_SUCCESS if variable setted successfully.
   */
  int nejson_object__set(
      struct nejson_object *_object,
      const int _is_rewriting_allowed,
      const char *_key,
      const struct nejson_variable _variable);

  /* Array.*/

  /*
   * Init usual "json" usual array.
   * @param[out] _array pointer to "json_array" pointer.
   * @returns NEJSON_SUCCESS if initialization was successful.
   */
  int nejson_array__init(
      struct nejson_array **_array);

  /*
   * Free array.
   * @param[in/out] _array pointer to array. Will be set as NEJSON_NULL.
   */
  void nejson_array__free(
      struct nejson_array **_array);

  /*
   * Get data from "json_array" by index.
   * @param[in] _array pointer "json_array".
   * @param[in] _index variable index.
   * @returns variable.
   */
  struct nejson_variable nejson_array__at(
      struct nejson_array *_array,
      const nejson_size_t _index);

  /*
   * Get pointer to data from "json_array" by index.
   * @param[in] _array pointer "json_array".
   * @param[in] _index variable index.
   * @returns pointer to variable.
   */
  struct nejson_variable *nejson_array__at_ptr(
      struct nejson_array *_array,
      const nejson_size_t _index);

  /*
   * Push value to "nejson_array" object.
   * @param[in] _array pointer "nejson_array".
   * @param[in] _variable variable.
   * @returns NEJSON_SUCCESS if variable pushed successfully.
   */
  int nejson_array__push_back(
      struct nejson_array *_array,
      const struct nejson_variable _variable);

  /*
   * Get array size.
   * @param[in] _array pointer "nejson_array".
   * @returns size of "nejson_array".
   */
  nejson_size_t nejson_array__size(
      struct nejson_array *_array);

  /*
   * Get pointer to last variable size.
   * @param[in] _array pointer "nejson_array".
   * @returns pointer to last variable of "nejson_array".
   */
  struct nejson_variable *nejson_array__last_ptr(
      struct nejson_array *_array);

  /* Iterator. */

  /*
   * Begin object iterator.
   * @param[in] _object pointer to object.
   * @returns iterator.
   */
  struct nejson_iterator nejson_iterator__object_begin(
      struct nejson_object *_object);

  /*
   * Iterate object.
   * @param[in] _iterator pointer to _iterator.
   * @returns pointer to current node or NEJSON_NULL.
   */
  struct nejson_node *nejson_iterator__object_next_ptr(
      struct nejson_iterator *_iterator);

  /*
   * Get pointer to next object node without moving cursor.
   * @param[in] _iterator pointer to _iterator.
   * @returns pointer to next node or NEJSON_NULL.
   */
  struct nejson_node *nejson_iterator__object_next_look_ptr(
      struct nejson_iterator *_iterator);

  /*
   * Begin array iterator.
   * @param[in] _array pointer to array.
   * @returns iterator.
   */
  struct nejson_iterator nejson_iterator__array_begin(
      struct nejson_array *_array);

  /*
   * Iterate array.
   * @param[in] _iterator pointer to _iterator.
   * @returns pointer to current variable or NEJSON_NULL.
   */
  struct nejson_variable *nejson_iterator__array_next_ptr(
      struct nejson_iterator *_iterator);

  /*
   * Get pointer to next variable without moving cursor.
   * @param[in] _iterator pointer to _iterator.
   * @returns pointer to next variable or NEJSON_NULL.
   */
  struct nejson_variable *nejson_iterator__array_next_look_ptr(
      struct nejson_iterator *_iterator);

  /* JSON. */

  /*
   * Init json.
   * @param[in] pointer to json.
   */
  void nejson__init(
      struct nejson *_json);

  /*
   * Free json.
   * @param[in/out] _json pointer to json. Will be set as NEJSON_NULL.
   */
  void nejson__free(
      struct nejson *_json);

  /*
   * Free object tree.
   * @param[in/out] _variable that will be released.
   */
  void nejson__free_object_tree(
      struct nejson_variable *_variable);

  /*
   * Create json from tokens.
   * @param[in/out] json object that will be filled
   *                with parsed data.
   * @param[in] _tokens tokens object.
   * @returns NEJSON_SUCCESS if json created successfully.
   */
  int nejson__create_from_tokens(
      struct nejson *_json,
      const struct nejson_tokens *_tokens);

#if !defined(NEJSON_NO_STDIO)

  /*
   * Load json from a file.
   * @param[in] _json json that will be filled with data.
   * @param[in] _path path to json file
   * @returns NEJSON_SUCCESS if object loaded successfully
   *          or NEJSON_FAILURE
   */
  int nejson__load_from_file(
      struct nejson *_json,
      const char *_path);

  /*
   * Save json to a json file.
   * @param[in] _json json to save.
   * @param[in] _path path to file.
   * @param[in] _is_formatted flag for object formatting.
   * @returns NEJSON_SUCCESS if object saved successfully
   *          or NEJSON_FAILURE.
   */
  int nejson__save_to_file(
      struct nejson *_json,
      const char *_path,
      const int _is_formatted);
#endif

  /*
   * Convert json to string.
   * @param[in] json object that will be converted
   *            to string.
   * @param[in] _is_formatted object formatting flag.
   *            If true, object will be formatted.
   * @returns dynamically allocated string.
   */
  char *nejson__stringify(struct nejson *_json,
                          const int _is_formatted);

#if !defined(NEJSON_NO_FORMAT_TREE)

  /*
   * Get variable from json using key tree.
   * @param[in] _json json object.
   * @param[in] _keys_format keys format.
   * @param[in] _args other keys.
   * @returns variable.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  struct nejson_variable nejson__get_variable_tree_args(
      const struct nejson *_json,
      const char *_keys_format,
      va_list _args);

  /*
   * Get variable from json using key tree.
   * @param[in] _json json object.
   * @param[in] _keys_format keys format.
   * @param[in] ... other keys.
   * @returns variable.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  struct nejson_variable nejson__get_variable_tree(
      const struct nejson *_json,
      const char *_keys_format,
      ...);

  /*
   * Get string from json using key tree.
   * @param[in] _json json object.
   * @param[in] _keys_format keys format.
   * @param[in] ... other keys.
   * @returns string or NEJSON_NULL.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  const char *nejson__get_string_tree(
      const struct nejson *_json,
      const char *_keys_format,
      ...);

  /*
   * Get string or "undefined" from json using key tree.
   * @param[in] _json json object.
   * @param[in] _keys_format keys format.
   * @param[in] ... other keys.
   * @returns string or NEJSON_UNDEFINED_STRING_UTF16.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  const char *nejson__get_string_or_undefined_tree(
      const struct nejson *_json,
      const char *_keys_format,
      ...);

  /*
   * Get float from json using key tree.
   * @param[in] _json json object.
   * @param[in] _keys_format keys format.
   * @param[in] ... other keys.
   * @returns float.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  float nejson__get_float_tree(
      const struct nejson *_json,
      const char *_keys_format,
      ...);

  /*
   * Get integer from json using key tree.
   * @param[in] _json json object.
   * @param[in] _keys_format keys format.
   * @param[in] ... other keys.
   * @returns integer.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  float nejson__get_integer_tree(
      const struct nejson *_json,
      const char *_keys_format,
      ...);

  /*
   * Get boolean from json using key tree.
   * @param[in] _json json object.
   * @param[in] _keys_format keys format.
   * @param[in] ... other keys.
   * @returns boolean.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  int nejson__get_boolean_tree(
      const struct nejson *_json,
      const char *_keys_format,
      ...);

  /*
   * Set variable to json using key tree.
   * @param[in] _json json object.
   * @param[in] _variable variable that will be placed to json object.
   * @param[in] _keys_format keys format.
   * @param[in] _is_rewriting_allowed allow to rewrite existing objects.
   * @param[in] _args other keys.
   * @returns NEJSON_SUCCESS if variable added successfully.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  int nejson__set_variable_tree_args(
      struct nejson *_json,
      struct nejson_variable _variable,
      const char *_keys_format,
      int _is_rewriting_allowed,
      va_list _args);

  /*
   * Set variable to json using key tree.
   * @param[in] _json json object.
   * @param[in] _variable variable that will be placed to json object.
   * @param[in] _keys_format keys format.
   * @param[in] ... other keys.
   * @returns NEJSON_SUCCESS if variable added successfully.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  int nejson__set_variable_tree(
      struct nejson *_json,
      struct nejson_variable _variable,
      const char *_keys_format,
      ...);

  /*
   * Set string to json using key tree.
   * @param[in] _json json object.
   * @param[in] _string string that will be copied to json object.
   * @param[in] _keys_format keys format.
   * @param[in] _args other keys.
   * @returns NEJSON_SUCCESS if variable added successfully.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  int nejson__set_string_tree(
      struct nejson *_json,
      const char *_string,
      const char *_keys_format,
      ...);

#if !defined(NEJSON_NO_UTF16)
  /*
   * Set utf16 string to json using key tree.
   * @param[in] _json json object.
   * @param[in] _string utf16 string that will be copied to json object.
   * @param[in] _keys_format keys format.
   * @param[in] _args other keys.
   * @returns NEJSON_SUCCESS if variable added successfully.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  int nejson__set_string_utf16_tree(
      struct nejson *_json,
      const wchar_t *_string,
      const char *_keys_format,
      ...);
#endif
  /*
   * Set float to json using key tree.
   * @param[in] _json json object.
   * @param[in] _float float that will be copied to json object.
   * @param[in] _keys_format keys format.
   * @param[in] _args other keys.
   * @returns NEJSON_SUCCESS if variable added successfully.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  int nejson__set_float_tree(
      struct nejson *_json,
      const float _float,
      const char *_keys_format,
      ...);
  /*
   * Set integer to json using key tree.
   * @param[in] _json json object.
   * @param[in] _integer integer that will be copied to json object.
   * @param[in] _keys_format keys format.
   * @param[in] _args other keys.
   * @returns NEJSON_SUCCESS if variable added successfully.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  int nejson__set_integer_tree(
      struct nejson *_json,
      const int _integer,
      const char *_keys_format,
      ...);
  /*
   * Set boolean to json using key tree.
   * @param[in] _json json object.
   * @param[in] _boolean boolean that will be copied to json object.
   * @param[in] _keys_format keys format.
   * @param[in] _args other keys.
   * @returns NEJSON_SUCCESS if variable added successfully.
   * @note use 's' for object key, 'w' for utf16 key and 'i' for array index.
   */
  int nejson__set_boolean_tree(
      struct nejson *_json,
      const int _boolean,
      const char *_keys_format,
      ...);

#ifdef __cplusplus
}
#endif

#endif
#endif

/* Functions bodies. */
#ifdef NEJSON_IMPLEMENTATION

/* Memory. */
void nejson_memory__fill(
    void *_data,
    const unsigned char _value,
    const nejson_size_t _size)
{
  nejson_size_t index = 0;
  while (index < _size)
  {
    ((unsigned char *)_data)[index] = _value;
    index++;
  }
}

/* String. */
nejson_size_t nejson_string__length(const char *_string)
{
  const char *temp = _string;
  while (*temp)
    temp++;
  return (temp - _string);
}

#if !defined(NEJSON_NO_STDIO)
char *nejson_string__load_from_file(const char *_path)
{
  FILE *file;
  char *buffer;
  nejson_size_t buffer_size;

  file = fopen(
      _path,
      "r");

  if (!file)
  {
    return NEJSON_NULL;
  }

  fseek(file, 0, SEEK_END);
  buffer_size = (nejson_size_t)ftell(file);
  buffer = (char *)NEJSON_MALLOC(sizeof(*buffer) * (buffer_size + 1));

  nejson_memory__fill(buffer, 0, buffer_size + 1);

  if (!buffer)
  {
    fclose(file);
    return buffer;
  }

  fseek(file, 0, SEEK_SET);
  fread(buffer, sizeof(*buffer), buffer_size, file);
  fclose(file);

  return buffer;
}

int nejson_string__save_to_file(
    const char *_path,
    const char *_string,
    const nejson_size_t _string_size)
{
  FILE *file;

  file = fopen(
      _path,
      "w");

  if (!file)
  {
    return NEJSON_FAILURE;
  }

  fwrite(_string, sizeof(*_string), _string_size, file);

  fclose(file);

  return NEJSON_SUCCESS;
}
#endif

char *nejson_string__copy(const char *_string)
{
  nejson_size_t index = 0;
  const nejson_size_t length = nejson_string__length(_string);
  char *buffer = NEJSON_NULL;

  buffer = NEJSON_MALLOC(length + 1);

  while (index < length && _string[index])
  {
    buffer[index] = _string[index];
    index++;
  }

  buffer[length] = 0;

  return buffer;
}

char *nejson_string__copy_length(
    const char *_string,
    const nejson_size_t _length)
{
  nejson_size_t index = 0;
  char *buffer = NEJSON_NULL;

  buffer = NEJSON_MALLOC(_length + 1);

  while (index < _length && _string[index])
  {
    buffer[index] = _string[index];
    index++;
  }
  buffer[_length] = 0;

  return buffer;
}

int nejson_string__reserve(
    char **_string_dst,
    nejson_size_t _length,
    nejson_size_t *_capacity)
{
  if (!*_capacity && _length)
  {
    *_string_dst = NEJSON_MALLOC(
        sizeof(*(*_string_dst)) *
        NEJSON_STRING_CHUNK_SIZE);
    *_capacity = NEJSON_STRING_CHUNK_SIZE;
  }

  if (_length >= *_capacity)
  {
    *_capacity = NEJSON_CALCULATE_RESERVE(
        NEJSON_STRING_CHUNK_SIZE,
        _length);

    *_string_dst = NEJSON_REALLOC(
        *_string_dst,
        *_capacity * sizeof(*(*_string_dst)));
  }

  if (!*_string_dst)
  {
    return NEJSON_FAILURE;
  }

  return NEJSON_SUCCESS;
}

void nejson_string__get_line_and_column_count(
    const char *_string,
    const nejson_size_t _position,
    nejson_size_t *_lines,
    nejson_size_t *_columns)
{
  nejson_size_t index = 0, lines = 1, columns = 0;

  while (index < _position)
  {
    if (_string[index] == '\n')
    {
      lines++;
      columns = 0;
    }
    columns++;
    index++;
  }

  *_lines = lines;
  *_columns = columns;
}

void nejson_string__append(
    char *_string,
    const char *_prefix)
{
  const nejson_size_t string_length =
      nejson_string__length(_string);
  const nejson_size_t prefix_length =
      nejson_string__length(_prefix);
  nejson_size_t index = 0;

  while (index < prefix_length)
  {
    *(_string + string_length + index) = *(_prefix + index);
    index++;
  }
  *(_string + string_length + index) = 0;
}

void nejson_string__begin(
    char *_destination,
    const char *_content)
{
  const nejson_size_t content_length =
      nejson_string__length(_content);
  nejson_size_t index = 0;

  while (index < content_length)
  {
    *(_destination + index) = *(_content + index);
    index++;
  }
  *(_destination + index) = 0;
}

int nejson_string__compare(
    const char *_string_a,
    const char *_string_b)
{
  const char *a, *b;
  a = _string_a;
  b = _string_b;
  while (*a && *b)
  {
    if ((*a) != (*b))
    {
      return NEJSON_FALSE;
    }
    a++;
    b++;
  }

  if (*a != *b)
  {
    return NEJSON_FALSE;
  }

  return NEJSON_TRUE;
}

int nejson_string__begins_with(
    const char *_string,
    const char *_prefix)
{
  const char *a, *b;
  a = _string;
  b = _prefix;
  while (*a && *b)
  {
    if ((*a) != (*b))
    {
      return NEJSON_FALSE;
    }
    a++;
    b++;
  }

  if (*b)
    return NEJSON_FALSE;

  return NEJSON_TRUE;
}

int nejson_string__is_char_float_number(const char _char)
{
  return _char == '0' ||
         _char == '1' ||
         _char == '2' ||
         _char == '3' ||
         _char == '4' ||
         _char == '5' ||
         _char == '6' ||
         _char == '7' ||
         _char == '8' ||
         _char == '9' ||
         _char == '.';
}

int nejson_string__is_char_white_space(const char _char)
{
  return _char == ' ' ||
         _char == '\t' ||
         _char == '\r' ||
         _char == '\n' ||
         _char == '\f';
}

int nejson_string__is_char_json_token(const char _char)
{
  return _char == '[' ||
         _char == ']' ||
         _char == '{' ||
         _char == '}' ||
         _char == ':' ||
         _char == ';' ||
         _char == ',' ||
         _char == '\"';
}

#if !defined(NEJSON_NO_UTF16)
nejson_size_t nejson_string__utf16_length(
    const wchar_t *_string)
{
  const wchar_t *temp = _string;
  while (*temp)
    temp++;
  return (temp - _string);
}

int nejson_string__utf8_to_utf16(
    const char *_utf8,
    wchar_t *_utf16,
    const nejson_size_t _utf16_buffer_size)
{
#if defined(__linux__)
  return mbstowcs(_utf16, _utf8, _utf16_buffer_size);
#elif defined(_WIN32)
    return MultiByteToWideChar(65001, 0, _utf8, -1, _utf16, _utf16_buffer_size);
#endif
}

int nejson_string__utf16_to_utf8(
    const wchar_t *_utf16,
    char *_utf8,
    const nejson_size_t _utf8_buffer_size)
{
#if defined(__linux__)
  return wcstombs(_utf8, _utf16, _utf8_buffer_size);
#elif defined(_WIN32)
    return WideCharToMultiByte(65001, 0, _utf16, -1, _utf8, _utf8_buffer_size, 0, 0);
#endif
}

char *nejson_string__create_from_utf16(
    const wchar_t *_string)
{
  const nejson_size_t length =
      nejson_string__utf16_length(_string) * 2;
  char *buffer = NEJSON_MALLOC(length + 1);

  nejson_string__utf16_to_utf8(_string, buffer, length);
  buffer[length] = 0;

  return buffer;
}

wchar_t *nejson_string__utf8_to_utf16_temporary(
    const char *_string)
{
  static wchar_t buffer[NEJSON_STRING_TEMPORARY_BUFFER_SIZE];
  nejson_memory__fill(buffer, 0, sizeof(buffer));

  nejson_string__utf8_to_utf16(
      _string,
      buffer,
      NEJSON_STRING_TEMPORARY_BUFFER_SIZE);

  return buffer;
}

#endif

nejson_ssize_t nejson_string__to_integer(
    const char *_string,
    const char **_string_end)
{
  const int base = 10;
  const char *index = _string;
  nejson_ssize_t value = 0, sign = 1;

  if (*index == '-')
  {
    sign = -1;
    index++;
  }

  while (*index >= '0' && *index <= '9')
  {
    value = value * base + (*index - '0');
    index++;
  }

  if (_string_end)
  {
    *_string_end = index;
  }

  return value * sign;
}

double nejson_string__to_double(
    const char *_string,
    const char **_string_end)
{
  const char *index = _string;
  const int base = 10;
  double pow, addend = 0, value = 0, sign = 1;

  if (*index == '-')
  {
    sign = -1;
    index++;
  }

  while (*index >= '0' && *index <= '9')
  {
    value = value * base + (*index - '0');
    index++;
  }

  if (*index == '.')
  {
    index++;
    pow = 1;
    while (*index >= '0' && *index <= '9')
    {
      addend = addend * base + (*index - '0');
      pow *= base;
      index++;
    }
    value += addend / pow;
  }

  if (_string_end)
  {
    *_string_end = index;
  }

  return value * sign;
}

void nejson_string__from_integer(
    char *_output_string,
    char **_output_string_end,
    const nejson_ssize_t _integer_value)
{
  const int base = 10;
  char *index = _output_string;

  nejson_ssize_t pow = 1, temp = _integer_value;

  while (temp /= base)
  {
    pow *= base;
  }

  if (_integer_value < 0)
    *index++ = '-';

  while (pow)
  {
    *index = (_integer_value / pow) % base;
    *index = NEJSON_ABS(*index) + '0';
    pow /= 10;
    index++;
  }
  *index = 0;

  if (_output_string_end)
  {
    *_output_string_end = index;
  }
}

void nejson_string__from_double(
    char *_output_string,
    char **_output_string_end,
    const double _double_value,
    const nejson_size_t _after_comma)
{
  const int base = 10;
  char *index = _output_string;

  nejson_ssize_t pow = 1;
  nejson_ssize_t temp = (nejson_ssize_t)_double_value;

  while (temp /= base)
  {
    pow *= base;
  }

  if (_double_value < 0)
    *index++ = '-';

  while (pow)
  {
    *index = ((nejson_ssize_t)_double_value / pow) % base;
    *index = NEJSON_ABS(*index) + '0';
    pow /= 10;
    index++;
  }

  if (_after_comma)
  {
    temp = _after_comma;
    pow = 1;

    while (temp)
    {
      pow *= base;
      temp--;
    }

    temp = (double)(_double_value - (double)((nejson_ssize_t)_double_value)) * (double)pow;
    temp = NEJSON_ABS(temp);
    pow /= base;

    *index = '.';
    index++;
    while (pow)
    {
      *index = (temp / pow) % base;
      *index = NEJSON_ABS(*index) + '0';
      pow /= 10;
      index++;
    }
  }

  *index = 0;

  if (_output_string_end)
  {
    *_output_string_end = index;
  }
}

/* Dynamic string. */
void nejson_da_string__init(
    struct nejson_da_string *_string)
{
  _string->data = 0;
  _string->capacity = 0;
  _string->size = 0;
}

void nejson_da_string__free(
    struct nejson_da_string *_string)
{
  if (_string->data)
  {
    NEJSON_FREE(_string->data);
  }
  _string->data = 0;
  _string->size = 0;
  _string->capacity = 0;
}

int nejson_da_string__append_reserve(
    struct nejson_da_string *_string,
    const nejson_size_t _size)
{
  return nejson_string__reserve(
      &_string->data,
      _string->size + _size,
      &_string->capacity);
}

char *nejson_da_string__data(
    struct nejson_da_string *_string)
{
  return _string->data;
}

int nejson_da_string__append_string(
    struct nejson_da_string *_string,
    const char *_append_string)
{
  nejson_size_t index = 0;
  const nejson_size_t append_string_length =
      nejson_string__length(_append_string);

  if (nejson_da_string__append_reserve(
          _string,
          append_string_length + 1))
  {
    return NEJSON_FAILURE;
  }

  while (index < append_string_length)
  {
    _string->data[_string->size] = _append_string[index];
    _string->size++;
    index++;
  }

  _string->data[_string->size] = 0;

  return NEJSON_SUCCESS;
}

int nejson_da_string__append_symbol(
    struct nejson_da_string *_string,
    const char _append_symbol)
{
  if (nejson_da_string__append_reserve(
          _string, 1))
  {
    return NEJSON_FAILURE;
  }

  _string->data[_string->size] = _append_symbol;
  _string->size++;

  return NEJSON_SUCCESS;
}

int nejson_da_string__append_spaces(
    struct nejson_da_string *_string,
    const nejson_size_t _spaces_count)
{
  nejson_size_t index = 0;
  while (index < _spaces_count)
  {
    if (nejson_da_string__append_symbol(_string, ' '))
    {
      return NEJSON_FAILURE;
    }
    index++;
  }
  return NEJSON_SUCCESS;
}

int nejson_da_string__append_double(
    struct nejson_da_string *_string,
    const double _double_value)
{
  char temp_string[512], *end;

  nejson_string__from_double(
      temp_string,
      &end,
      _double_value,
      NEJSON_STRING_FLOAT_SYMBOLS_AFTER_COMMA);
  *end = 0;

  if (nejson_da_string__append_string(_string, temp_string))
  {
    return NEJSON_FAILURE;
  }
  return NEJSON_SUCCESS;
}

int nejson_da_string__append_integer(
    struct nejson_da_string *_string,
    const nejson_ssize_t _integer_value)
{
  char temp_string[512], *end;

  nejson_string__from_integer(
      temp_string,
      &end,
      _integer_value);
  *end = 0;

  if (nejson_da_string__append_string(_string, temp_string))
  {
    return NEJSON_FAILURE;
  }
  return NEJSON_SUCCESS;
}

/* Variable. */
struct nejson_variable nejson_variable__undefined()
{
  struct nejson_variable result;
  result.type = nejson_variable_type_undefined;
  result.variant.variable_ptr = NEJSON_NULL;
  return result;
}

struct nejson_variable nejson_variable__set_integer(
    const int _integer)
{
  struct nejson_variable result;
  result.type = nejson_variable_type_integer;
  result.variant.variable_integer = _integer;
  return result;
}

int nejson_variable__get_integer(
    const struct nejson_variable _variable)
{
  if (_variable.type == nejson_variable_type_integer ||
      _variable.type == nejson_variable_type_boolean)
  {
    return _variable.variant.variable_integer;
  }
  else if (_variable.type == nejson_variable_type_float)
  {
    return (int)_variable.variant.variable_float;
  }
  return 0;
}

struct nejson_variable nejson_variable__set_float(
    const float _float)
{
  struct nejson_variable result;
  result.type = nejson_variable_type_float;
  result.variant.variable_float = _float;
  return result;
}

float nejson_variable__get_float(
    const struct nejson_variable _variable)
{
  if (_variable.type == nejson_variable_type_integer ||
      _variable.type == nejson_variable_type_boolean)
  {
    return (float)_variable.variant.variable_integer;
  }
  else if (_variable.type == nejson_variable_type_float)
  {
    return _variable.variant.variable_float;
  }
  return 0;
}

struct nejson_variable nejson_variable__set_boolean(
    const int _boolean)
{
  struct nejson_variable result;
  result.type = nejson_variable_type_boolean;
  result.variant.variable_integer = _boolean;
  return result;
}

int nejson_variable__get_boolean(
    const struct nejson_variable _variable)
{
  if (_variable.type == nejson_variable_type_integer ||
      _variable.type == nejson_variable_type_boolean)
  {
    if (_variable.variant.variable_integer)
      return NEJSON_TRUE;
    return NEJSON_FALSE;
  }
  else if (_variable.type == nejson_variable_type_float)
  {
    if ((int)_variable.variant.variable_float)
      return NEJSON_TRUE;
    return NEJSON_FALSE;
  }
  return 0;
}

struct nejson_variable nejson_variable__set_string(
    const char *_string)
{
  struct nejson_variable result;
  result.type = nejson_variable_type_string;
  result.variant.variable_ptr = nejson_string__copy(_string);
  return result;
}

const char *nejson_variable__get_string(
    const struct nejson_variable _variable)
{
  if (_variable.type == nejson_variable_type_string)
  {
    return _variable.variant.variable_ptr;
  }

  return NEJSON_NULL;
}

const char *nejson_variable__get_string_or_undefined(
    const struct nejson_variable _variable)
{
  if (_variable.type == nejson_variable_type_string)
  {
    return _variable.variant.variable_ptr;
  }
  return NEJSON_UNDEFINED_STRING;
}

#if !defined(NEJSON_NO_UTF16)
struct nejson_variable nejson_variable__set_utf16_string(
    const wchar_t *_string)
{
  struct nejson_variable result;
  result.type = nejson_variable_type_string;
  result.variant.variable_ptr = nejson_string__create_from_utf16(_string);
  return result;
}
wchar_t *nejson_variable__get_utf16_string_temporary(
    const struct nejson_variable _variable)
{
  static wchar_t buffer[NEJSON_STRING_TEMPORARY_BUFFER_SIZE];

  if (_variable.type != nejson_variable_type_string)
    return NEJSON_NULL;

  nejson_string__utf8_to_utf16(
      _variable.variant.variable_ptr,
      buffer,
      NEJSON_STRING_TEMPORARY_BUFFER_SIZE);

  return buffer;
}
wchar_t *nejson_variable__get_utf16_string_or_undefined_temporary(
    const struct nejson_variable _variable)
{
  static wchar_t buffer[NEJSON_STRING_TEMPORARY_BUFFER_SIZE];

  *buffer = 0;
  
  nejson_string__utf8_to_utf16(
      _variable.variant.variable_ptr,
      buffer,
      NEJSON_STRING_TEMPORARY_BUFFER_SIZE);

  if (!*buffer)
    return NEJSON_UNDEFINED_STRING_UTF16;

  return buffer;
}
#endif

struct nejson_variable nejson_variable__set_array(
    struct nejson_array *_array)
{
  struct nejson_variable result;
  result.type = nejson_variable_type_array;
  result.variant.variable_ptr = _array;
  return result;
}

void *nejson_variable__get_array(
    const struct nejson_variable _variable)
{
  if (_variable.type == nejson_variable_type_array)
  {
    return _variable.variant.variable_ptr;
  }

  return NEJSON_NULL;
}

struct nejson_variable nejson_variable__set_object(
    struct nejson_object *_object)
{
  struct nejson_variable result;
  result.type = nejson_variable_type_object;
  result.variant.variable_ptr = _object;
  return result;
}

struct nejson_object *nejson_variable__get_object(
    const struct nejson_variable _variable)
{
  if (_variable.type == nejson_variable_type_object &&
      _variable.variant.variable_ptr != NEJSON_NULL)
  {
    return (struct nejson_object *)_variable.variant.variable_ptr;
  }

  return NEJSON_NULL;
}

void nejson_variable__free(
    struct nejson_variable *_variable)
{
  if (!_variable->variant.variable_ptr)
    return;

  if (_variable->type == nejson_variable_type_string)
  {
    NEJSON_FREE(_variable->variant.variable_ptr);
  }
  else if (_variable->type == nejson_variable_type_object ||
           _variable->type == nejson_variable_type_array)
  {
    nejson__free_object_tree(_variable);
  }
  *_variable = nejson_variable__undefined();
}

/* Tokens. */

void nejson_tokens__init(
    struct nejson_tokens *_tokens)
{
  _tokens->data = 0;
  _tokens->capacity = 0;
  _tokens->size = 0;
}

void nejson_tokens__free(struct nejson_tokens *_tokens)
{
  NEJSON_FREE(_tokens->data);
  _tokens->data = 0;
  _tokens->capacity = 0;
  _tokens->size = 0;
}

int nejson_tokens__add(
    struct nejson_tokens *_tokens,
    const struct nejson_token _token)
{
  if (!_tokens->capacity)
  {
    _tokens->data = NEJSON_MALLOC(
        sizeof(*_tokens->data) *
        NEJSON_TOKENS_CHUNK_SIZE);
    _tokens->capacity = NEJSON_TOKENS_CHUNK_SIZE;
  }

  if (_tokens->size + 1 >= _tokens->capacity)
  {
    _tokens->capacity = NEJSON_CALCULATE_RESERVE(
        NEJSON_TOKENS_CHUNK_SIZE,
        _tokens->size + 1);
    _tokens->data = NEJSON_REALLOC(
        _tokens->data,
        _tokens->capacity * sizeof(*_tokens->data));
  }

  if (!_tokens->data)
  {
    return NEJSON_FAILURE;
  }

  _tokens->data[_tokens->size] = _token;
  _tokens->size++;
  return NEJSON_SUCCESS;
}

struct nejson_tokens nejson_tokens__tokenize(
    const char *_json_string)
{
  nejson_size_t index = 0;
  const nejson_size_t length =
      nejson_string__length(_json_string);

  char temp_char = 0, add_flag = 0;
  struct nejson_tokens tokens;
  struct nejson_token token;

  nejson_tokens__init(&tokens);
  token.type = nejson_token_type_none;

  while (index < length)
  {
    add_flag = 0;
    temp_char = _json_string[index];

    /* Work with undefined tokens. */
    if (token.type == nejson_token_type_unknown &&
        (nejson_string__is_char_json_token(temp_char) ||
         nejson_string__is_char_white_space(temp_char)))
    {
      token.length = (_json_string + index) - token.begin;
      nejson_tokens__add(
          &tokens,
          token);
      token.type = nejson_token_type_none;
    }

    /* String preparing. */
    if (!add_flag &&
        temp_char == '\"' &&
        (token.type == nejson_token_type_string ||
         token.type == nejson_token_type_none))
    {
      if (token.type == nejson_token_type_string)
      {
        if (index > 0 && _json_string[index - 1] != '\\')
        {
          token.length = (_json_string + index) - token.begin;
          add_flag = 1;
        }
      }
      else
      {
        if (token.type == nejson_token_type_none)
        {
          token.type = nejson_token_type_string;
          token.begin = (_json_string + index) + 1;
        }
        else
        {
          token.type = nejson_token_type_unknown;
        }
      }
    }

    /* String errors. */
    if (token.type == nejson_token_type_string &&
        temp_char == '\n')
    {
      token.type = nejson_token_type_unknown;
    }

    /* Work with json special symbols. */
    if (!add_flag &&
        token.type == nejson_token_type_none &&
        nejson_string__is_char_json_token(temp_char))
    {
      switch (temp_char)
      {
      case '{':
        token.type = nejson_token_type_object_begin;
        break;
      case '}':
        token.type = nejson_token_type_object_end;
        break;
      case '[':
        token.type = nejson_token_type_array_begin;
        break;
      case ']':
        token.type = nejson_token_type_array_end;
        break;
      case ':':
        token.type = nejson_token_type_assign;
        break;
      case ',':
        token.type = nejson_token_type_separator;
        break;
      }
      token.begin = _json_string + index;
      token.length = 1;
      add_flag = 1;
    }

    /* Work with numbers */
    if (!add_flag &&
        (token.type == nejson_token_type_none ||
         token.type == nejson_token_type_integer ||
         token.type == nejson_token_type_float))
    {
      if (temp_char == '-' && token.type == nejson_token_type_none)
      {
        token.type = nejson_token_type_integer;
        token.begin = _json_string + index;
      }

      if (nejson_string__is_char_float_number(temp_char))
      {
        if (temp_char == '.')
        {
          if (token.type == nejson_token_type_none)
          {
            token.type = nejson_token_type_unknown;
            token.begin = _json_string + index;
          }
          else if (token.type == nejson_token_type_float)
          {
            token.type = nejson_token_type_unknown;
          }
          else
          {
            token.type = nejson_token_type_float;
          }
        }

        if (token.type == nejson_token_type_none)
        {
          token.type = nejson_token_type_integer;
          token.begin = _json_string + index;
        }
      }
      if (((index + 1 < length && (_json_string[index + 1] == ',' ||
                                   nejson_string__is_char_white_space(_json_string[index + 1])))) &&
          (token.type == nejson_token_type_integer ||
           token.type == nejson_token_type_float))
      {
        token.length = ((_json_string + index) - token.begin) + 1;
        add_flag = 1;
      }
    }

    /* Logical "true" */
    if (!add_flag &&
        token.type == nejson_token_type_none &&
        nejson_string__begins_with(_json_string + index, "true"))
    {
      token.begin = _json_string + index;

      if (index + 4 < length &&
          (nejson_string__is_char_white_space(_json_string[index + 4]) ||
           _json_string[index + 4] == ','))
      {
        token.length = 4;
        token.type = nejson_token_type_true_literal;
        index += 3;
        add_flag = 1;
      }
      else
      {
        token.type = nejson_token_type_unknown;
      }
    }

    /* Logical "false" */
    if (!add_flag &&
        token.type == nejson_token_type_none &&
        nejson_string__begins_with(_json_string + index, "false"))
    {
      token.begin = _json_string + index;

      if (index + 4 < length &&
          (nejson_string__is_char_white_space(_json_string[index + 5]) ||
           _json_string[index + 5] == ','))
      {
        token.length = 5;
        token.type = nejson_token_type_false_literal;
        index += 4;
        add_flag = 1;
      }
      else
      {
        token.type = nejson_token_type_unknown;
      }
    }

    /* "null" type */
    if (!add_flag &&
        token.type == nejson_token_type_none &&
        nejson_string__begins_with(_json_string + index, "null"))
    {
      token.begin = _json_string + index;

      if (index + 4 < length &&
          (nejson_string__is_char_white_space(_json_string[index + 4]) ||
           _json_string[index + 4] == ','))
      {
        token.length = 4;
        token.type = nejson_token_type_null_literal;
        index += 3;
        add_flag = 1;
      }
      else
      {
        token.type = nejson_token_type_unknown;
      }
    }

    /* Catch unknown tokens. */
    if (!add_flag &&
        token.type == nejson_token_type_none &&
        !nejson_string__is_char_white_space(temp_char))
    {
      token.type = nejson_token_type_unknown;
      token.begin = _json_string + index;
    }

    if (add_flag)
    {
      nejson_tokens__add(
          &tokens,
          token);
      token.type = nejson_token_type_none;
    }
    index++;
  }

  return tokens;
}

int nejson_tokens__validate(
    const struct nejson_tokens *_tokens,
    const char *_json_string,
    char *_info)
{
  nejson_size_t index = 0, line = 0, column = 0;
  int result = 0, tree_index = 0;

  char brackets_tree_curve[NEJSON_MAX_TREE_DEPTH];
  char brackets_tree_square[NEJSON_MAX_TREE_DEPTH];
  enum nejson_token_type previous_token = nejson_token_type_none;

  const char *error_text = NEJSON_NULL;

  nejson_memory__fill(brackets_tree_curve, 0, sizeof(brackets_tree_curve));
  nejson_memory__fill(brackets_tree_square, 0, sizeof(brackets_tree_square));

  while (!result && index < _tokens->size)
  {
    switch (_tokens->data[index].type)
    {
    case nejson_token_type_unknown:
      error_text = "Unknown token";
      result = 1;
      break;
    case nejson_token_type_object_begin:
      brackets_tree_curve[tree_index]++;
      tree_index++;
      if (previous_token == nejson_token_type_object_begin)
      {
        error_text = "Unexpected object after parent object declaration";
        result = 1;
      }
      break;
    case nejson_token_type_object_end:
      tree_index--;
      brackets_tree_curve[tree_index]--;
      if (brackets_tree_curve[tree_index] < 0)
      {
        error_text = "Unexpected \'}\'";
        result = 1;
      }
      break;
    case nejson_token_type_array_begin:
      brackets_tree_square[tree_index]++;
      tree_index++;

      if (previous_token == nejson_token_type_object_begin)
      {
        error_text = "Unexpected array after parent object declaration";
        result = 1;
      }

      break;
    case nejson_token_type_array_end:
      tree_index--;
      brackets_tree_square[tree_index]--;
      if (brackets_tree_square[tree_index] < 0)
      {
        error_text = "Unexpected \']\'";
        result = 1;
      }
      break;
    case nejson_token_type_assign:
      if (previous_token != nejson_token_type_string)
      {
        error_text = "Unexpected \':\'";
        result = 1;
      }
      break;
    case nejson_token_type_separator:
      if (previous_token == nejson_token_type_assign ||
          previous_token == nejson_token_type_object_begin ||
          previous_token == nejson_token_type_array_begin)
      {
        error_text = "Unexpected \',\'";
        result = 1;
      }
      break;
    case nejson_token_type_true_literal:
    case nejson_token_type_false_literal:
    case nejson_token_type_integer:
    case nejson_token_type_float:
    case nejson_token_type_null_literal:
      if (previous_token != nejson_token_type_assign &&
          previous_token != nejson_token_type_separator)
      {
        error_text = "Unexpected variable";
        result = 1;
        return 1;
      }
      break;
    case nejson_token_type_string:
      if (previous_token != nejson_token_type_separator &&
          previous_token != nejson_token_type_assign &&
          previous_token != nejson_token_type_object_begin &&
          previous_token != nejson_token_type_array_begin)
      {
        error_text = "Unexpected string variable";
        result = 1;
      }
      break;
    default:
      break;
    }

    previous_token = _tokens->data[index].type;
    if (!result)
    {
      index++;
    }
  }

  if (brackets_tree_curve[0] > 0)
  {
    error_text = "expected \'}\'";
    result = 1;
  }

  if (brackets_tree_square[0] > 0)
  {
    error_text = "expected \']\'";
    result = 1;
  }

  if (_info && _json_string && result)
  {
    nejson_string__get_line_and_column_count(
        _json_string,
        _tokens->data[index].begin - _json_string,
        &line,
        &column);

    _info[0] = 0;
    nejson_string__begin(_info, error_text);
    nejson_string__append(_info, "; Line: ");
    index = nejson_string__length(_info);
    nejson_string__from_integer(_info + index, 0, line);

    nejson_string__append(_info, "; Column: ");
    index = nejson_string__length(_info);
    nejson_string__from_integer(_info + index, 0, column);
  }

  return result;
}

int nejson_object__init(struct nejson_object **_object)
{
  struct nejson_object *object;

  /* Allocate memory for object. */
  object = NEJSON_MALLOC(
      sizeof(struct nejson_object));

  if (!object)
  {
    (*_object) = NEJSON_NULL;
    return NEJSON_FAILURE;
  }

  /* Set object variables. */
  object->size = 0;
  object->capacity = 0;
  object->data = 0;

  *_object = object;
  return NEJSON_SUCCESS;
}

/* Free object. */
void nejson_object__free(
    struct nejson_object **_object)
{
  struct nejson_object *object = *_object;
  nejson_size_t index = 0;

  if (object->data)
  {
    while (index < object->size)
    {
      if (object->data[index].key)
      {
        NEJSON_FREE(object->data[index].key);
      }

      if (object->data[index].variable.type == nejson_variable_type_string)
      {
        NEJSON_FREE(object->data[index].variable.variant.variable_ptr);
      }
      index++;
    }
    NEJSON_FREE(object->data);
  }

  NEJSON_FREE(object);
  *_object = NEJSON_NULL;
}

struct nejson_variable nejson_object__get(
    struct nejson_object *_object,
    const char *_key)
{
  nejson_size_t index = 0;

  while (index < _object->size)
  {
    if (_object->data[index].key[0] == _key[0] &&
        nejson_string__compare(_object->data[index].key, _key))
    {
      return _object->data[index].variable;
    }
    index++;
  }
  return nejson_variable__undefined();
}

struct nejson_variable *nejson_object__get_ptr(
    struct nejson_object *_object,
    const char *_key)
{
  nejson_size_t index = 0;

  while (index < _object->size)
  {
    if (_object->data[index].key[0] == _key[0] &&
        nejson_string__compare(_object->data[index].key, _key))
    {
      return &_object->data[index].variable;
    }
    index++;
  }
  return NEJSON_NULL;
}

int nejson_object__set_node(
    struct nejson_object *_object,
    const int _is_rewriting_allowed,
    const struct nejson_node _node)
{
  nejson_size_t index = 0;

  if (!_object->capacity)
  {
    _object->data = NEJSON_MALLOC(
        sizeof(*_object->data) *
        NEJSON_OBJECT_CHUNK_SIZE);
    _object->capacity = NEJSON_OBJECT_CHUNK_SIZE;
  }
  if (!_object->data)
  {
    return NEJSON_FAILURE;
  }

  while (index < _object->size)
  {
    if (_object->data[index].key[0] == _node.key[0] &&
        nejson_string__compare(_object->data[index].key, _node.key))
    {
      if (!_is_rewriting_allowed)
        return NEJSON_FAILURE;

      nejson_variable__free(&_object->data[index].variable);
      _object->data[index].variable = _node.variable;
      return NEJSON_SUCCESS;
    }
    index++;
  }

  if (index == _object->size)
  {
    index = 0;
    while (index < _object->size)
    {
      if (_object->data[index].key == NEJSON_NULL)
      {
        _object->data[_object->size].key = _node.key;
        _object->data[_object->size].variable = _node.variable;
        return NEJSON_SUCCESS;
      }
      index++;
    }
  }

  if (_object->size + 1 >= _object->capacity)
  {
    _object->capacity = NEJSON_CALCULATE_RESERVE(
        NEJSON_OBJECT_CHUNK_SIZE,
        _object->size + 1);
    _object->data = NEJSON_REALLOC(
        _object->data,
        _object->capacity * sizeof(*_object->data));
    if (!_object->data)
    {
      return NEJSON_FAILURE;
    }
  }

  _object->data[_object->size].key = _node.key;
  _object->data[_object->size].variable = _node.variable;
  _object->size++;
  return NEJSON_SUCCESS;
}

int nejson_object__set(
    struct nejson_object *_object,
    const int _is_rewriting_allowed,
    const char *_key,
    const struct nejson_variable _variable)
{
  struct nejson_node node;
  node.key = nejson_string__copy(_key);
  node.variable = _variable;

  return nejson_object__set_node(
      _object,
      _is_rewriting_allowed,
      node);
}

int nejson_array__init(
    struct nejson_array **_array)
{
  struct nejson_array *array;
  /* Allocate memory for array. */
  array = NEJSON_MALLOC(
      sizeof(struct nejson_array));

  if (!array)
  {
    (*_array) = NEJSON_NULL;
    return NEJSON_FAILURE;
  }

  /* Set array variables. */
  array->size = 0;
  array->capacity = 0;
  array->data = 0;

  *_array = array;
  return NEJSON_SUCCESS;
}

void nejson_array__free(
    struct nejson_array **_array)
{
  struct nejson_array *array = *_array;
  nejson_size_t index = 0;

  if (array->data)
  {
    while (index < array->size)
    {
      if (array->data[index].type == nejson_variable_type_string)
      {
        NEJSON_FREE(array->data[index].variant.variable_ptr);
      }
      index++;
    }

    NEJSON_FREE(array->data);
  }

  NEJSON_FREE(array);
  *_array = NEJSON_NULL;
}

struct nejson_variable nejson_array__at(
    struct nejson_array *_array,
    const nejson_size_t _index)
{
  if (_index < _array->size)
    return _array->data[_index];

  return nejson_variable__undefined();
}

struct nejson_variable *nejson_array__at_ptr(
    struct nejson_array *_array,
    const nejson_size_t _index)
{
  if (_index < _array->size)
    return &_array->data[_index];

  return NEJSON_NULL;
}

int nejson_array__push_back(
    struct nejson_array *_array,
    const struct nejson_variable _variable)
{
  if (!_array->capacity)
  {
    _array->data = NEJSON_MALLOC(
        sizeof(*_array->data) *
        NEJSON_OBJECT_CHUNK_SIZE);
    _array->capacity = NEJSON_OBJECT_CHUNK_SIZE;
  }
  else if (_array->size + 1 >= _array->capacity)
  {
    _array->capacity = NEJSON_CALCULATE_RESERVE(
        NEJSON_OBJECT_CHUNK_SIZE,
        _array->size + 1);
    _array->data = NEJSON_REALLOC(
        _array->data,
        _array->capacity * sizeof(*_array->data));
  }
  if (!_array->data)
  {
    return NEJSON_FAILURE;
  }

  _array->data[_array->size] = _variable;
  _array->size++;

  return NEJSON_SUCCESS;
}

nejson_size_t nejson_array__size(
    struct nejson_array *_array)
{
  return _array->size;
}

struct nejson_variable *nejson_array__last_ptr(
    struct nejson_array *_array)
{
  if (!_array->size)
    return NEJSON_NULL;

  return &_array->data[_array->size - 1];
}

struct nejson_iterator nejson_iterator__object_begin(
    struct nejson_object *_object)
{
  struct nejson_iterator result;
  result.object = _object;
  result.type = nejson_variable_type_object;
  result.index = 0;
  return result;
}

struct nejson_node *nejson_iterator__object_next_ptr(
    struct nejson_iterator *_iterator)
{
  struct nejson_node *result = NEJSON_NULL;
  struct nejson_object *object = _iterator->object;
  nejson_size_t index = _iterator->index;

  if (_iterator->type != nejson_variable_type_object ||
      index >= object->capacity ||
      _iterator->index >= object->size)
  {
    return result;
  }

  while (index < object->capacity)
  {
    if (object->data[index].key &&
        (object->data[index].variable.type !=
         nejson_variable_type_undefined))
    {
      result = &object->data[index];
      break;
    }
    index++;
  }

  _iterator->index++;
  return result;
}

struct nejson_node *nejson_iterator__object_next_look_ptr(
    struct nejson_iterator *_iterator)
{
  struct nejson_node *result =
      nejson_iterator__object_next_ptr(_iterator);
  if (result)
    _iterator->index--;
  return result;
}

struct nejson_iterator nejson_iterator__array_begin(
    struct nejson_array *_array)
{
  struct nejson_iterator result;
  result.object = _array;
  result.type = nejson_variable_type_array;
  result.index = 0;
  return result;
}

struct nejson_variable *nejson_iterator__array_next_ptr(
    struct nejson_iterator *_iterator)
{
  struct nejson_array *array = _iterator->object;

  if (_iterator->index >= array->size)
  {
    return NEJSON_NULL;
  }

  return &array->data[_iterator->index++];
}

struct nejson_variable *nejson_iterator__array_next_look_ptr(
    struct nejson_iterator *_iterator)
{
  struct nejson_variable *result =
      nejson_iterator__array_next_ptr(_iterator);
  if (result)
    _iterator->index--;
  return result;
}

void nejson__init(
    struct nejson *_json)
{

  _json->root.type = nejson_variable_type_undefined;
  _json->root.variant.variable_ptr = NEJSON_NULL;

  _json->out_of_memory = 0;
  _json->rewriting_allowed = 1;
  nejson_memory__fill(_json->info, 0, sizeof(_json->info));
}

void nejson__free(
    struct nejson *_json)
{
  nejson__free_object_tree(&_json->root);
  nejson_memory__fill(_json, 0, sizeof(*_json));
}

void nejson__free_object_tree(
    struct nejson_variable *_variable)
{
  int object_tree_index = 0;
  struct nejson_iterator object_tree[NEJSON_MAX_TREE_DEPTH];
  struct nejson_node *temp_node = NEJSON_NULL;
  struct nejson_variable *temp_variable = NEJSON_NULL;

  if (_variable->type == nejson_variable_type_object)
  {
    object_tree[object_tree_index] = nejson_iterator__object_begin(
        (_variable->variant.variable_ptr));
  }
  else if (_variable->type == nejson_variable_type_array)
  {
    object_tree[object_tree_index] = nejson_iterator__array_begin(
        (_variable->variant.variable_ptr));
  }
  else
  {
    return;
  }

  while (object_tree_index >= 0)
  {
    if ((object_tree[object_tree_index].type !=
         nejson_variable_type_object) &&
        (object_tree[object_tree_index].type !=
         nejson_variable_type_array))
    {
      break;
    }

    temp_variable = 0;
    if (object_tree[object_tree_index].type ==
        nejson_variable_type_object)
    {
      temp_node = nejson_iterator__object_next_ptr(
          &object_tree[object_tree_index]);

      if (temp_node)
      {
        temp_variable = &temp_node->variable;
      }
    }
    else
    {
      temp_variable = nejson_iterator__array_next_ptr(
          &object_tree[object_tree_index]);
    }

    if (!temp_variable)
    {
      if (object_tree[object_tree_index].type ==
          nejson_variable_type_object)
      {
        nejson_object__free(
            (struct nejson_object **)&object_tree[object_tree_index].object);
      }
      else
      {
        nejson_array__free(
            (struct nejson_array **)&object_tree[object_tree_index].object);
      }
      object_tree[object_tree_index].object = NEJSON_NULL;
      object_tree_index--;
      continue;
    }

    if ((temp_variable->type == nejson_variable_type_object) &&
        temp_variable->variant.variable_ptr)
    {
      object_tree_index++;
      object_tree[object_tree_index] = nejson_iterator__object_begin(
          temp_variable->variant.variable_ptr);
      continue;
    }

    if ((temp_variable->type == nejson_variable_type_array) &&
        temp_variable->variant.variable_ptr)
    {
      object_tree_index++;
      object_tree[object_tree_index] = nejson_iterator__array_begin(
          temp_variable->variant.variable_ptr);
      continue;
    }
  }

  *_variable = nejson_variable__undefined();
}

int nejson__create_from_tokens(
    struct nejson *_json,
    const struct nejson_tokens *_tokens)
{
  nejson_size_t index = 0, object_tree_index = 0;
  struct nejson_node temp_node, object_tree[NEJSON_MAX_TREE_DEPTH];
  struct nejson_variable *temp_variable = NEJSON_NULL;
  enum nejson_token_type previous_token = nejson_token_type_none;

  int add_flag;

  object_tree[0].variable.type = nejson_variable_type_undefined;
  temp_node.key = NEJSON_NULL;

  while (index < _tokens->size)
  {
    if (_tokens->data[index].type == nejson_token_type_object_begin ||
        _tokens->data[index].type == nejson_token_type_array_begin)
    {
      object_tree[object_tree_index].key = temp_node.key;
      temp_variable = &object_tree[object_tree_index].variable;

      if (_tokens->data[index].type == nejson_token_type_object_begin)
      {
        temp_variable->type = nejson_variable_type_object;
        if (nejson_object__init(
                (struct nejson_object **)&temp_variable->variant.variable_ptr))
        {
          return NEJSON_FAILURE;
        }
      }
      else
      {
        temp_variable->type = nejson_variable_type_array;
        if (nejson_array__init(
                (struct nejson_array **)&temp_variable->variant.variable_ptr))
        {
          return NEJSON_FAILURE;
        }
      }

      object_tree_index++;
    }

    if ((_tokens->data[index].type == nejson_token_type_array_end ||
         _tokens->data[index].type == nejson_token_type_object_end) &&
        object_tree_index >= 2)
    {
      if (object_tree[object_tree_index - 2].variable.type ==
          nejson_variable_type_object)
      {
        if (nejson_object__set_node(
                object_tree[object_tree_index - 2].variable.variant.variable_ptr,
                _json->rewriting_allowed,
                object_tree[object_tree_index - 1]))
        {
          return NEJSON_FAILURE;
        }
      }
      if (object_tree[object_tree_index - 2].variable.type ==
          nejson_variable_type_array)
      {
        if (nejson_array__push_back(
                object_tree[object_tree_index - 2].variable.variant.variable_ptr,
                object_tree[object_tree_index - 1].variable))
        {
          return NEJSON_FAILURE;
        }
      }

      object_tree_index--;
      temp_variable = &object_tree[object_tree_index - 1].variable;
    }

    if (temp_variable)
    {
      if ((previous_token == nejson_token_type_object_begin ||
           previous_token == nejson_token_type_separator) &&
          _tokens->data[index].type == nejson_token_type_string)
      {
        if (temp_variable->type == nejson_variable_type_object)
        {
          temp_node.key = nejson_string__copy_length(
              _tokens->data[index].begin,
              _tokens->data[index].length);
        }
      }

      if ((previous_token == nejson_token_type_assign &&
           temp_variable->type == nejson_variable_type_object) ||
          temp_variable->type == nejson_variable_type_array)
      {
        add_flag = 0;
        switch (_tokens->data[index].type)
        {
        case nejson_token_type_string:
          temp_node.variable.type = nejson_variable_type_string;
          temp_node.variable.variant.variable_ptr =
              nejson_string__copy_length(
                  _tokens->data[index].begin,
                  _tokens->data[index].length);
          add_flag = 1;
          break;
        case nejson_token_type_integer:
          temp_node.variable.type = nejson_variable_type_integer;
          temp_node.variable.variant.variable_integer =
              nejson_string__to_integer(
                  _tokens->data[index].begin,
                  0);
          add_flag = 1;
          break;
        case nejson_token_type_float:
          temp_node.variable.type = nejson_variable_type_float;
          temp_node.variable.variant.variable_float =
              nejson_string__to_double(
                  _tokens->data[index].begin,
                  0);
          add_flag = 1;
          break;
        case nejson_token_type_true_literal:
          temp_node.variable.type = nejson_variable_type_boolean;
          temp_node.variable.variant.variable_integer = 1;
          add_flag = 1;
          break;
        case nejson_token_type_false_literal:
          temp_node.variable.type = nejson_variable_type_boolean;
          temp_node.variable.variant.variable_integer = 0;
          add_flag = 1;
          break;
        case nejson_token_type_null_literal:
          temp_node.variable.type = nejson_variable_type_null;
          temp_node.variable.variant.variable_integer = 0;
          add_flag = 1;
          break;
        default:
          break;
        }

        if (add_flag && temp_variable->type == nejson_variable_type_object)
        {
          if (nejson_object__set_node(
                  temp_variable->variant.variable_ptr,
                  _json->rewriting_allowed,
                  temp_node))
          {
            return NEJSON_FAILURE;
          }
        }

        if (add_flag && temp_variable->type == nejson_variable_type_array)
        {
          if (nejson_array__push_back(
                  temp_variable->variant.variable_ptr,
                  temp_node.variable))
          {
            return NEJSON_FAILURE;
          }
        }
      }
    }

    previous_token = _tokens->data[index].type;
    index++;
  }

  _json->root = object_tree[0].variable;
  return NEJSON_SUCCESS;
}

#if !defined(NEJSON_NO_STDIO)
int nejson__load_from_file(
    struct nejson *_json,
    const char *_path)
{
  struct nejson_tokens tokens;
  char *json_string = NEJSON_NULL;
  char info[512];

  json_string = nejson_string__load_from_file(_path);
  if (!json_string)
  {
    return NEJSON_FAILURE;
  }

  tokens = nejson_tokens__tokenize(json_string);
  if (nejson_tokens__validate(&tokens, json_string, info))
  {
    nejson_string__begin(_json->info, "Validation error: ");
    nejson_string__append(_json->info, info);

    _json->root.type = nejson_variable_type_undefined;
    _json->root.variant.variable_ptr = NEJSON_NULL;
    return NEJSON_FAILURE;
  }

  nejson__create_from_tokens(_json, &tokens);

  nejson_tokens__free(&tokens);
  NEJSON_FREE(json_string);

  return NEJSON_SUCCESS;
}

int nejson__save_to_file(
    struct nejson *_json,
    const char *_path,
    const int _is_formatted)
{
  char *json_string = nejson__stringify(_json, _is_formatted);

  if (nejson_string__save_to_file(
          _path,
          json_string,
          nejson_string__length(json_string)))
  {
    nejson_string__begin(_json->info, "Cannot save string.");
    return NEJSON_FAILURE;
  }

  free(json_string);

  return NEJSON_SUCCESS;
}
#endif

char *nejson__stringify(struct nejson *_json,
                        const int _is_formatted)
{
  int object_tree_index = 0;
  struct nejson_iterator object_tree[NEJSON_MAX_TREE_DEPTH];
  struct nejson_node *temp_node = NEJSON_NULL;
  struct nejson_variable *temp_variable = NEJSON_NULL;
  enum nejson_variable_type previous_variable_type;

  struct nejson_da_string json_string;
  nejson_da_string__init(&json_string);

  if (_json->root.type == nejson_variable_type_object)
  {
    object_tree[object_tree_index] = nejson_iterator__object_begin(
        _json->root.variant.variable_ptr);

    nejson_da_string__append_symbol(&json_string, '{');
  }
  else if (_json->root.type == nejson_variable_type_array)
  {
    object_tree[object_tree_index] = nejson_iterator__array_begin(
        _json->root.variant.variable_ptr);

    nejson_da_string__append_symbol(&json_string, '[');
    if (_is_formatted && nejson_iterator__array_next_look_ptr(
                             &object_tree[object_tree_index]))
    {
      nejson_da_string__append_symbol(&json_string, '\n');
      nejson_da_string__append_spaces(&json_string, (object_tree_index + 1) * NEJSON_STRING_TAB_SIZE);
    }
  }
  else
  {
    return NEJSON_NULL;
  }

  previous_variable_type = object_tree[object_tree_index].type;

  while (object_tree_index >= 0)
  {
    if ((object_tree[object_tree_index].type !=
         nejson_variable_type_object) &&
        (object_tree[object_tree_index].type !=
         nejson_variable_type_array))
    {
      break;
    }

    temp_variable = NEJSON_NULL;

    if (object_tree[object_tree_index].type ==
        nejson_variable_type_object)
    {
      temp_node = nejson_iterator__object_next_ptr(
          &object_tree[object_tree_index]);

      if (temp_node)
      {
        temp_variable = &temp_node->variable;
      }
    }
    else
    {
      temp_variable = nejson_iterator__array_next_ptr(
          &object_tree[object_tree_index]);
    }

    if (temp_variable)
    {
      if (object_tree[object_tree_index].type ==
          nejson_variable_type_object)
      {
        if (object_tree[object_tree_index].index > 1)
        {
          nejson_da_string__append_symbol(&json_string, ',');
        }
        if (temp_node)
        {
          if (_is_formatted)
          {
            nejson_da_string__append_symbol(&json_string, '\n');
            nejson_da_string__append_spaces(&json_string, (object_tree_index + 1) * NEJSON_STRING_TAB_SIZE);
          }
          nejson_da_string__append_symbol(&json_string, '\"');
          nejson_da_string__append_string(&json_string, temp_node->key);
          nejson_da_string__append_symbol(&json_string, '\"');
          nejson_da_string__append_symbol(&json_string, ':');

          if (_is_formatted)
          {
            nejson_da_string__append_symbol(&json_string, ' ');
          }
        }
      }
    }
    else
    {
      if ((previous_variable_type != nejson_variable_type_object ||
           previous_variable_type != nejson_variable_type_array) &&
          object_tree[object_tree_index].index)
      {
        if (_is_formatted)
        {
          nejson_da_string__append_symbol(&json_string, '\n');
          nejson_da_string__append_spaces(&json_string, (object_tree_index)*NEJSON_STRING_TAB_SIZE);
        }
      }

      if (object_tree[object_tree_index].type ==
          nejson_variable_type_object)
      {
        nejson_da_string__append_symbol(&json_string, '}');
      }
      else
      {
        nejson_da_string__append_symbol(&json_string, ']');
      }

      object_tree[object_tree_index].object = NEJSON_NULL;
      object_tree_index--;

      if (object_tree_index >= 0)
      {
        previous_variable_type = object_tree[object_tree_index].type;
      }
      continue;
    }

    if ((temp_variable->type == nejson_variable_type_object ||
         temp_variable->type == nejson_variable_type_array) &&
        (object_tree[object_tree_index].type == nejson_variable_type_array &&
         (object_tree[object_tree_index].index > 1 ||
          previous_variable_type != nejson_variable_type_array)))
    {
      nejson_da_string__append_symbol(&json_string, ',');
      if (_is_formatted)
      {
        nejson_da_string__append_symbol(&json_string, '\n');
        nejson_da_string__append_spaces(&json_string, (object_tree_index + 1) * NEJSON_STRING_TAB_SIZE);
      }
    }

    if (temp_variable->type == nejson_variable_type_object)
    {
      object_tree_index++;
      object_tree[object_tree_index] = nejson_iterator__object_begin(
          temp_variable->variant.variable_ptr);
      nejson_da_string__append_symbol(&json_string, '{');

      previous_variable_type = temp_variable->type;
      continue;
    }
    else if (temp_variable->type == nejson_variable_type_array)
    {
      object_tree_index++;
      object_tree[object_tree_index] = nejson_iterator__array_begin(
          temp_variable->variant.variable_ptr);

      nejson_da_string__append_symbol(&json_string, '[');

      if (_is_formatted &&
          nejson_iterator__array_next_look_ptr(
              &object_tree[object_tree_index]))
      {
        nejson_da_string__append_symbol(&json_string, '\n');
        nejson_da_string__append_spaces(&json_string, (object_tree_index + 1) * NEJSON_STRING_TAB_SIZE);
      }

      previous_variable_type = temp_variable->type;
      continue;
    }
    else if (temp_variable->type == nejson_variable_type_string)
    {
      if (object_tree[object_tree_index].type == nejson_variable_type_array &&
          (object_tree[object_tree_index].index > 1 ||
           previous_variable_type != nejson_variable_type_array))
      {
        nejson_da_string__append_symbol(&json_string, ',');
        if (_is_formatted)
        {
          nejson_da_string__append_symbol(&json_string, '\n');
          nejson_da_string__append_spaces(&json_string, (object_tree_index + 1) * NEJSON_STRING_TAB_SIZE);
        }
      }

      nejson_da_string__append_symbol(&json_string, '\"');
      nejson_da_string__append_string(&json_string, (char *)temp_variable->variant.variable_ptr);
      nejson_da_string__append_symbol(&json_string, '\"');
    }
    else
    {
      if (object_tree[object_tree_index].type == nejson_variable_type_array)
      {
        nejson_da_string__append_symbol(&json_string, ',');
        if (_is_formatted)
        {
          nejson_da_string__append_symbol(&json_string, '\n');
          nejson_da_string__append_spaces(&json_string, (object_tree_index + 1) * NEJSON_STRING_TAB_SIZE);
        }
      }

      switch (temp_variable->type)
      {
      case nejson_variable_type_float:
        nejson_da_string__append_double(&json_string, temp_variable->variant.variable_float);
        break;
      case nejson_variable_type_integer:
        nejson_da_string__append_integer(&json_string, temp_variable->variant.variable_integer);
        break;
      case nejson_variable_type_boolean:
        nejson_da_string__append_string(&json_string, temp_variable->variant.variable_integer ? "true" : "false");
        break;
      case nejson_variable_type_null:
      case nejson_variable_type_undefined:
        nejson_da_string__append_string(&json_string, "null");
        break;
      default:
        nejson_da_string__free(&json_string);
        return NEJSON_NULL;
        break;
      }
    }
    previous_variable_type = temp_variable->type;
  }

  json_string.data[json_string.size] = 0;

  return json_string.data;
}

#if !defined(NEJSON_NO_FORMAT_TREE)
struct nejson_variable nejson__get_variable_tree_args(
    const struct nejson *_json,
    const char *_keys_format,
    va_list _args)
{
  const nejson_size_t keys_count =
      nejson_string__length(_keys_format);
  nejson_size_t index = 0;
  nejson_size_t array_index = 0;

  const char *object_key = NEJSON_NULL;
#if !defined(NEJSON_NO_UTF16)
  const wchar_t *object_key_utf16 = NEJSON_NULL;
  static char object_key_buffer[NEJSON_STRING_TEMPORARY_BUFFER_SIZE];
#endif

  struct nejson_variable variable = _json->root;

  while (index < keys_count)
  {
    if (_keys_format[index] == 's' ||
        _keys_format[index] == 'w')
    {
      if (_keys_format[index] == 'w')
      {
#if !defined(NEJSON_NO_UTF16)
        object_key_utf16 = va_arg(_args, const wchar_t *);
        nejson_string__utf16_to_utf8(
            object_key_utf16,
            object_key_buffer,
            sizeof(object_key_buffer));
        object_key = object_key_buffer;
#else
          NEJSON_ASSERT("Wide string not allowed in \"NEJSON_NO_UTF16\" mode");
#endif
      }
      else
      {
        object_key = va_arg(_args, const char *);
      }
      if (variable.type == nejson_variable_type_object)
      {
        variable = nejson_object__get(
            variable.variant.variable_ptr,
            object_key);
      }
      else
      {
        return nejson_variable__undefined();
      }
    }
    else if (_keys_format[index] == 'i')
    {
      if (variable.type == nejson_variable_type_array)
      {
        array_index = va_arg(_args, int);
        variable = nejson_array__at(
            variable.variant.variable_ptr,
            array_index);
      }
      else
      {
        return nejson_variable__undefined();
      }
    }
    else
    {
      return nejson_variable__undefined();
    }

    if (variable.type == nejson_variable_type_undefined)
    {
      return variable;
    }

    index++;
  }
  return variable;
}

struct nejson_variable nejson__get_variable_tree(
    const struct nejson *_json,
    const char *_keys_format,
    ...)
{
  va_list args;
  struct nejson_variable result;

  va_start(args, _keys_format);
  result = nejson__get_variable_tree_args(
      _json,
      _keys_format,
      args);

  va_end(args);

  return result;
}

const char *nejson__get_string_tree(
    const struct nejson *_json,
    const char *_keys_format,
    ...)
{
  va_list args;
  struct nejson_variable result;

  va_start(args, _keys_format);
  result = nejson__get_variable_tree_args(
      _json,
      _keys_format,
      args);

  va_end(args);

  return nejson_variable__get_string(result);
}

const char *nejson__get_string_or_undefined_tree(
    const struct nejson *_json,
    const char *_keys_format,
    ...)
{
  va_list args;
  struct nejson_variable result;

  va_start(args, _keys_format);
  result = nejson__get_variable_tree_args(
      _json,
      _keys_format,
      args);

  va_end(args);

  return nejson_variable__get_string_or_undefined(result);
}

float nejson__get_float_tree(
    const struct nejson *_json,
    const char *_keys_format,
    ...)
{
  va_list args;
  struct nejson_variable result;

  va_start(args, _keys_format);
  result = nejson__get_variable_tree_args(
      _json,
      _keys_format,
      args);

  va_end(args);

  return nejson_variable__get_float(result);
}

float nejson__get_integer_tree(
    const struct nejson *_json,
    const char *_keys_format,
    ...)
{
  va_list args;
  struct nejson_variable result;

  va_start(args, _keys_format);
  result = nejson__get_variable_tree_args(
      _json,
      _keys_format,
      args);

  va_end(args);

  return nejson_variable__get_integer(result);
}

int nejson__get_boolean_tree(
    const struct nejson *_json,
    const char *_keys_format,
    ...)
{
  va_list args;
  struct nejson_variable result;

  va_start(args, _keys_format);
  result = nejson__get_variable_tree_args(
      _json,
      _keys_format,
      args);

  va_end(args);

  return nejson_variable__get_boolean(result);
}

int nejson__set_variable_tree_args(
    struct nejson *_json,
    struct nejson_variable _variable,
    const char *_keys_format,
    int _is_rewriting_allowed,
    va_list _args)
{
  const nejson_ssize_t keys_count =
      nejson_string__length(_keys_format);
  nejson_size_t index = 0;
  nejson_size_t array_index = 0;

  const char *object_key = NEJSON_NULL;

#if !defined(NEJSON_NO_UTF16)
  const wchar_t *object_key_utf16 = NEJSON_NULL;
  static char object_key_buffer[NEJSON_STRING_TEMPORARY_BUFFER_SIZE];
#endif

  void *object = NEJSON_NULL;
  struct nejson_variable *variable = &_json->root,
                         *variable_parent = NEJSON_NULL;

  while ((nejson_ssize_t)index < keys_count)
  {
    if (!variable)
      return NEJSON_FAILURE;

    if (_keys_format[index] == 's' ||
        _keys_format[index] == 'w')
    {

      if (_keys_format[index] == 'w')
      {
#if !defined(NEJSON_NO_UTF16)
        object_key_utf16 = va_arg(_args, const wchar_t *);
        nejson_string__utf16_to_utf8(
            object_key_utf16,
            object_key_buffer,
            sizeof(object_key_buffer));
        object_key = object_key_buffer;
#else
          NEJSON_ASSERT("Wide string not allowed in \"NEJSON_NO_UTF16\" mode");
#endif
      }
      else
      {
        object_key = va_arg(_args, const char *);
      }

      /* TODO: check is object_key is NULL; */

      if (variable->type == nejson_variable_type_undefined)
      {
        nejson_variable__free(variable);
        nejson_object__init(
            (struct nejson_object **)&object);
        *variable = nejson_variable__set_object(
            object);
      }
      else if (variable->type != nejson_variable_type_object)
      {
        if (_is_rewriting_allowed)
        {
          nejson_variable__free(variable);
          nejson_object__init(
              (struct nejson_object **)&object);
          *variable = nejson_variable__set_object(
              object);
        }
        else
        {
          return NEJSON_FAILURE;
        }
      }

      if ((nejson_ssize_t)index == (keys_count - 1))
      {
        nejson_object__set(
            variable->variant.variable_ptr,
            _json->rewriting_allowed,
            object_key,
            _variable);
        return EXIT_SUCCESS;
      }

      variable_parent = variable;
      variable = nejson_object__get_ptr(
          variable_parent->variant.variable_ptr,
          object_key);

      if (!variable)
      {
        nejson_object__set(
            variable_parent->variant.variable_ptr,
            _json->rewriting_allowed,
            object_key,
            nejson_variable__undefined());

        variable = nejson_object__get_ptr(
            variable_parent->variant.variable_ptr,
            object_key);
      }
    }
    else if (_keys_format[index] == 'i')
    {
      array_index = va_arg(_args, int);

      if (variable->type == nejson_variable_type_undefined)
      {
        nejson_variable__free(variable);
        nejson_array__init(
            (struct nejson_array **)&object);

        *variable = nejson_variable__set_array(
            object);
      }
      else if (variable->type != nejson_variable_type_array)
      {
        if (_is_rewriting_allowed)
        {
          nejson_variable__free(variable);
          nejson_array__init(
              (struct nejson_array **)&object);
          *variable = nejson_variable__set_array(
              object);
        }
        else
        {
          return NEJSON_FAILURE;
        }
      }

      variable_parent = variable;
      variable = nejson_array__at_ptr(
          variable_parent->variant.variable_ptr,
          array_index);

      if ((nejson_ssize_t)index == (keys_count - 1))
      {
        if (variable)
        {
          nejson_variable__free(variable);
          *variable = _variable;
        }
        else if (nejson_array__size(variable_parent->variant.variable_ptr) < array_index)
        {
          *nejson_array__at_ptr(
              variable_parent->variant.variable_ptr,
              array_index) = _variable;
        }
        else if (nejson_array__size(variable_parent->variant.variable_ptr) == array_index)
        {
          nejson_array__push_back(
              variable_parent->variant.variable_ptr,
              _variable);
        }
        else
        {
          return NEJSON_FAILURE;
        }
        return EXIT_SUCCESS;
      }

      if (!variable)
      {
        nejson_array__push_back(
            variable_parent->variant.variable_ptr,
            nejson_variable__undefined());

        variable = nejson_array__last_ptr(
            variable_parent->variant.variable_ptr);
      }
      else
      {
        return NEJSON_FAILURE;
      }
    }
    else
    {
      return NEJSON_FAILURE;
    }

    index++;
  }

  return NEJSON_SUCCESS;
}

int nejson__set_variable_tree(
    struct nejson *_json,
    struct nejson_variable _variable,
    const char *_keys_format,
    ...)
{
  va_list args;
  int result;

  va_start(args, _keys_format);
  result = nejson__set_variable_tree_args(
      _json,
      _variable,
      _keys_format,
      _json->rewriting_allowed,
      args);

  va_end(args);

  return result;
}
int nejson__set_string_tree(
    struct nejson *_json,
    const char *_string,
    const char *_keys_format,
    ...)
{
  va_list args;
  int result;
  struct nejson_variable variable =
      nejson_variable__set_string(_string);

  va_start(args, _keys_format);
  result = nejson__set_variable_tree_args(
      _json,
      variable,
      _keys_format,
      _json->rewriting_allowed,
      args);

  va_end(args);

  return result;
}
int nejson__set_string_utf16_tree(
    struct nejson *_json,
    const wchar_t *_string,
    const char *_keys_format,
    ...)
{
  va_list args;
  int result;
  struct nejson_variable variable =
      nejson_variable__set_utf16_string(_string);

  va_start(args, _keys_format);
  result = nejson__set_variable_tree_args(
      _json,
      variable,
      _keys_format,
      _json->rewriting_allowed,
      args);

  va_end(args);

  return result;
}

int nejson__set_float_tree(
    struct nejson *_json,
    const float _float,
    const char *_keys_format,
    ...)
{
  va_list args;
  int result;
  struct nejson_variable variable =
      nejson_variable__set_float(_float);

  va_start(args, _keys_format);
  result = nejson__set_variable_tree_args(
      _json,
      variable,
      _keys_format,
      _json->rewriting_allowed,
      args);

  va_end(args);

  return result;
}
int nejson__set_integer_tree(
    struct nejson *_json,
    const int _integer,
    const char *_keys_format,
    ...)
{
  va_list args;
  int result;
  struct nejson_variable variable =
      nejson_variable__set_integer(_integer);

  va_start(args, _keys_format);
  result = nejson__set_variable_tree_args(
      _json,
      variable,
      _keys_format,
      _json->rewriting_allowed,
      args);

  va_end(args);

  return result;
}

int nejson__set_boolean_tree(
    struct nejson *_json,
    const int _boolean,
    const char *_keys_format,
    ...)
{
  va_list args;
  int result;
  struct nejson_variable variable =
      nejson_variable__set_boolean(_boolean);

  va_start(args, _keys_format);
  result = nejson__set_variable_tree_args(
      _json,
      variable,
      _keys_format,
      _json->rewriting_allowed,
      args);

  va_end(args);

  return result;
}
#endif
#endif
