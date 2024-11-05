#ifndef OBSC_GRAPHIC_H
#define OBSC_GRAPHIC_H
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <obscfs.h>
#include <glad/gl.h>
#include <linmath.h>
#include <stb/stb_image.h>
#include <stb/nestbtt.h>

#define OBSC_RENDERER2D_VERTICES_CAPACITY 1024
#define OBSC_RENDERER2D_INDICES_CAPACITY (OBSC_RENDERER2D_VERTICES_CAPACITY * 2)
#define OBSC_RENDERER2D_MAX_TEXTURE_UNITS 10

#define OBSC_LERP(_a, _b, _f) ((_a) + ((_b) - (_a)) * (_f))

typedef struct obsc_vector2
{
    float x, y;
} obsc_vector2;

typedef struct obsc_vector3
{
    float x, y, z;
} obsc_vector3;

typedef struct obsc_vector4
{
    float x, y, z, w;
} obsc_vector4;

typedef struct obsc_color
{
    unsigned char r, g, b, a;
} obsc_color;

typedef struct obsc_texture obsc_texture;
typedef struct obsc_renderer2d obsc_renderer2d;

typedef struct obsc_font
{
    struct obsc_texture *texture;
    struct nestbtt_font font;
} obsc_font;

int obsc_graphic_api__init();
void obsc_graphic_api__terminate();

struct obsc_texture *obsc_texture__load(
    const char *_path);

struct obsc_texture *obsc_texture__load_from_memory(
    const unsigned char *_data,
    const size_t _size);

void obsc_texture__get_size(
    struct obsc_texture *_texture,
    int *_out_width,
    int *_out_height);

void obsc_texture__free(
    struct obsc_texture **_texture);

struct obsc_font obsc_font__load_from_memory(
    const unsigned char *_data,
    const size_t _data_size,
    size_t _size,
    int *_codepoints,
    size_t _codepoints_count);

struct obsc_font obsc_font__load(
    const char *_path,
    size_t _size,
    int *_codepoints,
    size_t _codepoints_count);

void obsc_font__free(
    struct obsc_font *_font);

int obsc_font__is_ready(
    struct obsc_font *_font);

struct obsc_renderer2d *obsc_renderer2d__init();
void obsc_renderer2d__free(
    struct obsc_renderer2d **_renderer);

void obsc_renderer2d__set_size(
    struct obsc_renderer2d *_renderer,
    int _width,
    int _height);

void obsc_renderer2d__begin(struct obsc_renderer2d *_renderer);
void obsc_renderer2d__flush(struct obsc_renderer2d *_renderer);
void obsc_renderer2d__end(struct obsc_renderer2d *_renderer);

void obsc_renderer2d__draw_quad(
    struct obsc_renderer2d *_renderer,
    const float _x,
    const float _y,
    const float _w,
    const float _h,
    const float _radius,
    const float _border,
    const struct obsc_color _color,
    const struct obsc_color _border_color,
    const struct obsc_texture *_texture,
    const struct obsc_vector4 _texture_coordinates,
    const int _vertex_flags);

void obsc_renderer2d__draw_text(
    struct obsc_renderer2d *_renderer,
    const struct obsc_font *_font,
    const char *_text,
    const size_t _size,
    const struct obsc_color _color,
    const float _x,
    const float _y);

void obsc_renderer2d__draw_text_wide(
    struct obsc_renderer2d *_renderer,
    const struct obsc_font *_font,
    const wchar_t *_text,
    const size_t _size,
    const struct obsc_color _color,
    const float _x,
    const float _y);

void obsc_renderer2d__draw_circle(
    struct obsc_renderer2d *_renderer,
    const struct obsc_color _color,
    const float _x,
    const float _y,
    const float _radius);

void obsc_renderer2d__draw_texture_scale(
    struct obsc_renderer2d *_renderer,
    struct obsc_texture *_texture,
    const float _x,
    const float _y,
    const float _scale);

struct obsc_color obsc_color__lerp(
    const struct obsc_color _a,
    const struct obsc_color _b,
    const float _factor);

#endif