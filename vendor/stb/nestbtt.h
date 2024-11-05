#ifndef NESTBTT_H
#define NESTBTT_H
#include "stb_truetype.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct nestbtt_irect
{
  int x, y, w, h;
} nestbtt_irect;

typedef struct nestbtt_codepoint_box
{
  int codepoint;
  float x, y, w, h, baseline;
} nestbtt_codepoint_box;

typedef struct nestbtt_bitmap
{
  int w, h, comp;
  unsigned char *data;
} nestbtt_bitmap;

typedef struct nestbtt_font
{
  struct nestbtt_bitmap bitmap;
  struct nestbtt_codepoint_box *codepoints_boxes;
  size_t codepoints_boxes_count;

  float space_width, size;
  int is_sizes_normalized;
} nestbtt_font;

#ifndef nestbtt_MAX
#define nestbtt_MAX(_a, _b) ((_a) > (_b) ? (_a) : (_b))
#endif

int *nestbtt_getCodepointsFromString(
    const char *_string,
    const size_t _string_length,
    size_t *_codepoints_count);

int *nestbtt_getCodepointsFromWideString(
    const wchar_t *_string,
    const size_t _string_length,
    size_t *_codepoints_count);

int nestbtt_loadFontBitmapMemory(
    struct nestbtt_bitmap *_out_bitmap,
    struct nestbtt_codepoint_box **_out_boxes,
    const unsigned char *_data,
    const size_t _data_size,
    int *_codepoints,
    const size_t _codepoints_count,
    const size_t _codepoints_in_row,
    const size_t _font_size,
    const int _normalize);

int nestbtt_loadFontBitmap(
    struct nestbtt_bitmap *_out_bitmap,
    struct nestbtt_codepoint_box **_out_boxes,
    const char *_font_path,
    int *_codepoints,
    const size_t _codepoints_count,
    const size_t _codepoints_in_row,
    const size_t _font_size,
    const int _normalize);

struct nestbtt_font nestbtt_loadFont(
    const char *_path,
    int *_codepoints,
    size_t _codepoints_count,
    size_t _font_size,
    int _normalize);

struct nestbtt_font nestbtt_loadFontMemory(
    const unsigned char *_data,
    const size_t _data_size,
    int *_codepoints,
    size_t _codepoints_count,
    size_t _font_size,
    int _normalize);

const struct nestbtt_codepoint_box *nestbtt_getCodepointBox(
    const int _codepoint,
    const struct nestbtt_font *_font);

void nestbtt_freeFont(struct nestbtt_font *_font);

int nestbtt_isFontReady(struct nestbtt_font *_font);

#endif

#ifdef NESTBTT_IMPLEMENTATION
int *nestbtt_getCodepointsFromString(
    const char *_string,
    const size_t _string_length,
    size_t *_codepoints_count)
{
  int *buffer, *result, count_flag, temp_codepoint;
  size_t index_a, index_b, temp_codepoints_count;

  buffer = malloc(sizeof(*buffer) * _string_length);
  if (!buffer)
  {
    return NULL;
  }

  index_a = 0;
  temp_codepoints_count = 0;
  while (index_a < _string_length)
  {
    index_b = 0;
    count_flag = 0;
    temp_codepoint = _string[index_a];

    if (temp_codepoint >= 32)
    {
      while (!count_flag && index_b < temp_codepoints_count)
      {
        if (temp_codepoint == buffer[index_b])
        {
          count_flag = 1;
        }
        index_b++;
      }
      if (!count_flag)
      {
        buffer[temp_codepoints_count] = temp_codepoint;
        temp_codepoints_count++;
      }
    }
    index_a++;
  }

  result = malloc(sizeof(*result) * temp_codepoints_count);

  if (!result)
  {
    free(buffer);
    return NULL;
  }

  index_a = 0;
  while (index_a < temp_codepoints_count)
  {
    result[index_a] = buffer[index_a];
    index_a++;
  }

  *_codepoints_count = temp_codepoints_count;
  free(buffer);

  return result;
}

int *nestbtt_getCodepointsFromWideString(
    const wchar_t *_string,
    const size_t _string_length,
    size_t *_codepoints_count)
{
  int *buffer, *result, count_flag, temp_codepoint;
  size_t index_a, index_b, temp_codepoints_count;

  buffer = malloc(sizeof(*buffer) * _string_length);
  if (!buffer)
  {
    return NULL;
  }

  index_a = 0;
  temp_codepoints_count = 0;
  while (index_a < _string_length)
  {
    index_b = 0;
    count_flag = 0;
    temp_codepoint = _string[index_a];

    if (temp_codepoint >= 32)
    {
      while (!count_flag && index_b < temp_codepoints_count)
      {
        if (temp_codepoint == buffer[index_b])
        {
          count_flag = 1;
        }
        index_b++;
      }
      if (!count_flag)
      {
        buffer[temp_codepoints_count] = temp_codepoint;
        temp_codepoints_count++;
      }
    }
    index_a++;
  }

  result = malloc(sizeof(*result) * temp_codepoints_count);

  if (!result)
  {
    free(buffer);
    return NULL;
  }

  index_a = 0;
  while (index_a < temp_codepoints_count)
  {
    result[index_a] = buffer[index_a];
    index_a++;
  }

  *_codepoints_count = temp_codepoints_count;
  free(buffer);

  return result;
}

int nestbtt_loadFontBitmapMemory(
    struct nestbtt_bitmap *_out_bitmap,
    struct nestbtt_codepoint_box **_out_boxes,
    const unsigned char *_data,
    const size_t _data_size,
    int *_codepoints,
    const size_t _codepoints_count,
    const size_t _codepoints_in_row,
    const size_t _font_size,
    const int _normalize)
{
  float margin; /* in px */
  struct stbtt_fontinfo info;

  float scale;
  size_t index, row_codepoint_count;
  int next_x, next_y, ascent, temp_codepoint;
  struct nestbtt_irect temp_rect;
  struct nestbtt_bitmap temp_bitmap;
  struct nestbtt_codepoint_box *temp_boxes;

  if (!_codepoints_count ||
      !_codepoints_in_row ||
      !_font_size)
  {
    return 1;
  }

  margin = (float)_font_size * 0.05f;

  /* Init font. */
  stbtt_InitFont(
      &info,
      _data,
      stbtt_GetFontOffsetForIndex(_data, 0));
  scale = stbtt_ScaleForPixelHeight(
      &info,
      _font_size);
  stbtt_GetFontVMetrics(&info, &ascent, 0, 0);

  /* Prepare codepoints boxes. */
  temp_boxes = malloc(
      _codepoints_count *
      sizeof(struct nestbtt_codepoint_box));
  if (!temp_boxes)
  {
    return 1;
  }

  /* Measure needed bitmap. */
  index = 0;
  next_x = 0;
  next_y = 0;
  temp_codepoint = _codepoints[0];
  row_codepoint_count = 0;
  temp_bitmap.w = 0;
  temp_bitmap.h = 0;
  while (temp_codepoint && index < _codepoints_count)
  {
    stbtt_GetCodepointBitmapBox(
        &info,
        temp_codepoint,
        scale,
        scale,
        &temp_rect.x,
        &temp_rect.y,
        &temp_rect.w,
        &temp_rect.h);

    if (row_codepoint_count >= _codepoints_in_row)
    {
      temp_bitmap.w = nestbtt_MAX(temp_bitmap.w, next_x);
      next_y += _font_size + margin;
      next_x = 0;
      row_codepoint_count = 0;
    }

    next_x += temp_rect.w - temp_rect.x + margin;

    index++;
    row_codepoint_count++;
    temp_codepoint = _codepoints[index];
  }

  /* Allocate bitmap. */
  if (!temp_bitmap.w)
  {
    temp_bitmap.w = next_x;
  }
  temp_bitmap.h = next_y + _font_size;
  temp_bitmap.comp = 1;
  temp_bitmap.data = malloc(
      sizeof(*temp_bitmap.data) *
      temp_bitmap.h *
      temp_bitmap.w *
      temp_bitmap.comp);
  if (!temp_bitmap.data)
  {
    free(temp_boxes);
    return 1;
  }
  memset(
      temp_bitmap.data,
      0,
      sizeof(*temp_bitmap.data) *
          temp_bitmap.h *
          temp_bitmap.w *
          temp_bitmap.comp);

  /* Fill bitmap and codepoints array. */
  index = 0;
  next_x = 0;
  next_y = 0;
  temp_codepoint = _codepoints[0];
  row_codepoint_count = 0;
  while (temp_codepoint && index < _codepoints_count)
  {
    stbtt_GetCodepointBitmapBox(
        &info,
        temp_codepoint,
        scale,
        scale,
        &temp_rect.x,
        &temp_rect.y,
        &temp_rect.w,
        &temp_rect.h);

    if (row_codepoint_count >= _codepoints_in_row)
    {
      temp_bitmap.w = nestbtt_MAX(temp_bitmap.w, next_x);
      next_y += _font_size + margin;
      next_x = 0;
      row_codepoint_count = 0;
    }

    stbtt_MakeCodepointBitmap(
        &info,
        temp_bitmap.data + next_x + next_y * temp_bitmap.w,
        temp_rect.w - temp_rect.x,
        temp_rect.h - temp_rect.y,
        temp_bitmap.w,
        scale,
        scale,
        temp_codepoint);

    if (_normalize)
    {
      temp_boxes[index] = (struct nestbtt_codepoint_box){
          .codepoint = temp_codepoint,
          .x = (float)(next_x) / temp_bitmap.w,
          .y = (float)(next_y) / temp_bitmap.h,
          .w = (float)(temp_rect.w - temp_rect.x) / temp_bitmap.w,
          .h = (float)(temp_rect.h - temp_rect.y) / temp_bitmap.h,
          .baseline = (((float)ascent * scale) + temp_rect.y) / temp_bitmap.h,
      };
    }
    else
    {
      temp_boxes[index] = (struct nestbtt_codepoint_box){
          .codepoint = temp_codepoint,
          .x = next_x,
          .y = next_y,
          .w = temp_rect.w - temp_rect.x,
          .h = temp_rect.h - temp_rect.y,
          .baseline = (int)((float)ascent * scale) + temp_rect.y,
      };
    }

    next_x += temp_rect.w - temp_rect.x + margin;

    index++;
    row_codepoint_count++;
    temp_codepoint = _codepoints[index];
  }

  *_out_bitmap = temp_bitmap;
  *_out_boxes = temp_boxes;

  return 0;
}

int nestbtt_loadFontBitmap(
    struct nestbtt_bitmap *_out_bitmap,
    struct nestbtt_codepoint_box **_out_boxes,
    const char *_font_path,
    int *_codepoints,
    const size_t _codepoints_count,
    const size_t _codepoints_in_row,
    const size_t _font_size,
    const int _normalize)
{
  FILE *file;
  unsigned char *file_buffer;
  size_t file_buffer_size;
  int result;

  /* Load file here. */
  file = fopen(_font_path, "rb");
  if (!file)
  {
    return 1;
  }
  fseek(file, 0, SEEK_END);
  file_buffer_size = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);
  if (!file_buffer_size)
  {
    fclose(file);
    return 1;
  }
  file_buffer = malloc(sizeof(*file_buffer) * file_buffer_size);
  fread(file_buffer, 1, file_buffer_size, file);
  fclose(file);

  result = nestbtt_loadFontBitmapMemory(
      _out_bitmap,
      _out_boxes,
      file_buffer,
      file_buffer_size,
      _codepoints,
      _codepoints_count,
      _codepoints_in_row,
      _font_size,
      _normalize);

  free(file_buffer);
  return result;
}

struct nestbtt_font nestbtt_loadFont(
    const char *_path,
    int *_codepoints,
    size_t _codepoints_count,
    size_t _font_size,
    int _normalize)
{
  struct nestbtt_font result;
  const size_t codepoints_in_row = 20;

  if (!nestbtt_loadFontBitmap(
          &result.bitmap,
          &result.codepoints_boxes,
          _path,
          _codepoints,
          _codepoints_count,
          codepoints_in_row,
          _font_size,
          _normalize))
  {
    result.is_sizes_normalized = _normalize;
    result.codepoints_boxes_count = _codepoints_count;
    result.space_width =
        ((float)result.bitmap.w /
         (float)codepoints_in_row) /
        (float)result.bitmap.w;
    result.size = (float)_font_size;
  }
  else
  {
    fprintf(stderr, "ERROR: cannot init font!\n");
    fflush(stderr);
    memset(&result, 0, sizeof(result));
  }

  return result;
}

struct nestbtt_font nestbtt_loadFontMemory(
    const unsigned char *_data,
    const size_t _data_size,
    int *_codepoints,
    size_t _codepoints_count,
    size_t _font_size,
    int _normalize)
{
  struct nestbtt_font result;
  const size_t codepoints_in_row = 20;

  if (!nestbtt_loadFontBitmapMemory(
          &result.bitmap,
          &result.codepoints_boxes,
          _data,
          _data_size,
          _codepoints,
          _codepoints_count,
          codepoints_in_row,
          _font_size,
          _normalize))
  {
    result.is_sizes_normalized = _normalize;
    result.codepoints_boxes_count = _codepoints_count;
    result.space_width =
        ((float)result.bitmap.w /
         (float)codepoints_in_row) /
        (float)result.bitmap.w;
    result.size = (float)_font_size;
  }
  else
  {
    fprintf(stderr, "ERROR: cannot init font!\n");
    fflush(stderr);
    memset(&result, 0, sizeof(result));
  }

  return result;
}

const struct nestbtt_codepoint_box *nestbtt_getCodepointBox(
    const int _codepoint,
    const struct nestbtt_font *_font)
{
  size_t index;

  index = 0;
  while (index < _font->codepoints_boxes_count)
  {
    if (_font->codepoints_boxes[index].codepoint ==
        _codepoint)
    {
      return &_font->codepoints_boxes[index];
    }
    index++;
  }
  return NULL;
}

void nestbtt_freeFont(struct nestbtt_font *_font)
{
  free(_font->bitmap.data);
  free(_font->codepoints_boxes);
  memset(_font, 0, sizeof(*_font));
}

int nestbtt_isFontReady(struct nestbtt_font *_font)
{
  if (_font->bitmap.data && _font->codepoints_boxes)
  {
    return 0;
  }

  return 1;
}
#endif