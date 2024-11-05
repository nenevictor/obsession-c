#include <obscgraphic.h>

#if !defined(_DEBUG)
#include "gl_shader.vs.h"
#include "gl_shader.fs.h"
#include "Iosevka-Heavy.ttf.h"
#endif

typedef struct obsc_vertex
{
  struct obsc_vector2 position;
  struct obsc_vector2 size;
  struct obsc_vector2 center_position;
  float radius;
  float rect_border;
  struct obsc_color color;
  struct obsc_color border_color;
  int texture_id;
  struct obsc_vector2 texture_coordinates;

  int vertex_flags;
} obsc_vertex;

typedef struct obsc_gl_renderer2d
{
  struct obsc_vertex vertices[OBSC_RENDERER2D_VERTICES_CAPACITY];
  unsigned int indices[OBSC_RENDERER2D_INDICES_CAPACITY];
  size_t vertices_count, indices_count;

  unsigned int program, vao, vbo, ibo;

  int width, height;

  unsigned int texture_units[OBSC_RENDERER2D_MAX_TEXTURE_UNITS];
  size_t texture_units_count;
} obsc_gl_renderer2d;

typedef struct obsc_gl_texture
{
  unsigned int id;
  int width, height;
} obsc_gl_texture;

struct obsc_texture
{
  struct obsc_gl_texture texture;
};

struct obsc_renderer2d
{
  struct obsc_gl_renderer2d renderer;
};

int obsc_graphic_api__init()
{
  if (!gladLoaderLoadGL())
  {
    fprintf(
        stderr,
        "GL Initialization error;\n");
    return 1;
  }

  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);

  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  return 0;
}

void obsc_graphic_api__terminate()
{
  gladLoaderUnloadGL();
}

unsigned int obsc_gl_shader__load_from_code(
    const char *_vertex_code,
    const char *_fragment_code)
{
  unsigned int vertex, fragment, program;
  int success;
  char validation_info[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, (const char **)(&_vertex_code), NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertex, 512, NULL, validation_info);
    fprintf(
        stderr,
        "GL Vertex shader compilation error: %s\n",
        validation_info);
    glDeleteShader(vertex);
    return -1;
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, (const char **)(&_fragment_code), NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragment, 512, NULL, validation_info);
    fprintf(
        stderr,
        "GL Fragment shader compilation error: %s\n",
        validation_info);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return -1;
  }

  program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(program, 512, NULL, validation_info);
    fprintf(
        stderr,
        "GL Program linking error: %s\n",
        validation_info);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return -1;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
  return program;
}

unsigned int obsc_gl_shader__load_from_file(
    const char *_vertex_path,
    const char *_fragment_path)
{
  char *vertex_shader_text, *fragment_shader_text;
  unsigned int program;

  vertex_shader_text = obsc_readTextFile(_vertex_path);
  fragment_shader_text = obsc_readTextFile(_fragment_path);

  program = obsc_gl_shader__load_from_code(
      vertex_shader_text,
      fragment_shader_text);

  free(vertex_shader_text);
  free(fragment_shader_text);

  return program;
}

struct obsc_texture *obsc_texture__load(
    const char *_path)
{
  const size_t required_components = 4;
  struct obsc_texture *texture = malloc(sizeof(*texture));

  unsigned char *data = stbi_load(
      _path,
      &texture->texture.width,
      &texture->texture.height,
      NULL,
      required_components);

  if (data)
  {
    glGenTextures(1, &texture->texture.id);
    glBindTexture(GL_TEXTURE_2D, texture->texture.id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        texture->texture.width,
        texture->texture.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
  }
  else
  {
    memset(&texture, 0, sizeof(texture));
    texture->texture.id = (unsigned int)-1;
  }

  return texture;
}

struct obsc_texture *obsc_texture__load_from_memory(
    const unsigned char *_data,
    const size_t _size)
{
  const size_t required_components = 4;
  struct obsc_texture *texture = malloc(sizeof(*texture));

  unsigned char *data = stbi_load_from_memory(
      _data,
      _size,
      &texture->texture.width,
      &texture->texture.height,
      NULL,
      required_components);

  if (data)
  {
    glGenTextures(1, &texture->texture.id);
    glBindTexture(GL_TEXTURE_2D, texture->texture.id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        texture->texture.width,
        texture->texture.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
  }
  else
  {
    memset(&texture, 0, sizeof(texture));
    texture->texture.id = (unsigned int)-1;
  }

  return texture;
}

void obsc_texture__free(
    struct obsc_texture **_texture)
{

  if ((*_texture)->texture.id != (unsigned int)-1)
  {
    glDeleteTextures(1, &(*_texture)->texture.id);
  }
  free((*_texture));
  (*_texture) = 0;
}

struct obsc_font obsc_font__load_from_memory(
    const unsigned char *_data,
    const size_t _data_size,
    size_t _size,
    int *_codepoints,
    size_t _codepoints_count)
{
  const size_t required_components = 4;
  size_t i;
  struct obsc_font result;
  unsigned char *buffer, temp_color;

  memset(&result, 0, sizeof(result));

  result.font = nestbtt_loadFontMemory(
      _data,
      _data_size,
      _codepoints,
      _codepoints_count,
      _size,
      1);

  result.texture = malloc(sizeof(*result.texture));

  if (!result.font.bitmap.data)
  {
    memset(&result, 0, sizeof(result));
    result.texture->texture.id = (unsigned int)-1;
    return result;
  }

  if (!result.texture)
  {
    free(result.font.bitmap.data);
    memset(&result, 0, sizeof(result));
    result.texture->texture.id = (unsigned int)-1;
    return result;
  }

  result.texture->texture.width = result.font.bitmap.w;
  result.texture->texture.height = result.font.bitmap.h;

  buffer = malloc(
      result.font.bitmap.w *
      result.font.bitmap.h *
      required_components);

  i = 0;
  while ((int)i < result.font.bitmap.w * result.font.bitmap.h)
  {
    temp_color = result.font.bitmap.data[i];
    buffer[i * 4 + 0] = temp_color;
    buffer[i * 4 + 1] = temp_color;
    buffer[i * 4 + 2] = temp_color;
    buffer[i * 4 + 3] = temp_color;
    i++;
  }

  free(result.font.bitmap.data);
  result.font.bitmap.data = 0;

  glGenTextures(1, &result.texture->texture.id);
  glBindTexture(GL_TEXTURE_2D, result.texture->texture.id);
  glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      result.texture->texture.width,
      result.texture->texture.height,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      buffer);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  free(buffer);

  return result;
}

struct obsc_font obsc_font__load(
    const char *_path,
    size_t _size,
    int *_codepoints,
    size_t _codepoints_count)
{

  unsigned char *file_buffer;
  size_t file_buffer_size;
  struct obsc_font result;
  FILE *file;

  memset(&result, 0, sizeof(result));

  file = fopen(_path, "rb");
  if (!file)
  {
    return result;
  }
  fseek(file, 0, SEEK_END);
  file_buffer_size = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);
  if (!file_buffer_size)
  {
    fclose(file);
    return result;
  }
  file_buffer = malloc(sizeof(*file_buffer) * file_buffer_size);
  fread(file_buffer, 1, file_buffer_size, file);
  fclose(file);

  result = obsc_font__load_from_memory(
      file_buffer,
      file_buffer_size,
      _size,
      _codepoints,
      _codepoints_count);

  return result;
}

struct obsc_font obsc_font__load_form_memory(
    const unsigned char *_data,
    const size_t _data_size,
    size_t _size,
    int *_codepoints,
    size_t _codepoints_count);

void obsc_texture__get_size(
    struct obsc_texture *_texture,
    int *_out_width,
    int *_out_height)
{
  *_out_width = _texture->texture.width;
  *_out_height = _texture->texture.height;
}

void obsc_font__free(
    struct obsc_font *_font)
{
  if (_font->font.bitmap.data)
  {
    free(_font->font.bitmap.data);
  }

  obsc_texture__free(&_font->texture);

  memset(_font, 0, sizeof(*_font));
}

int obsc_font__is_ready(
    struct obsc_font *_font)
{
  return (_font->font.size &&
          _font->font.codepoints_boxes &&
          _font->font.codepoints_boxes_count &&
          _font->texture->texture.id != (unsigned int)-1 &&
          _font->texture->texture.id);
}

struct obsc_renderer2d *obsc_renderer2d__init()
{
  struct obsc_renderer2d *renderer;
  unsigned int
      sal_position,
      sal_size,
      sal_center_position,
      sal_radius,
      sal_rect_border,
      sal_color,
      sal_border_color,
      sal_texture_id,
      sal_texture_coordinates,
      sal_vertex_flags;

  renderer = malloc(sizeof(struct obsc_renderer2d));
  memset(renderer->renderer.vertices, 0, sizeof(renderer->renderer.vertices));
  renderer->renderer.vertices_count = 0;
  memset(renderer->renderer.texture_units, 0, sizeof(renderer->renderer.texture_units));
  renderer->renderer.texture_units_count = 0;

#if !defined(_DEBUG)
  renderer->renderer.program = obsc_gl_shader__load_from_code(
      (const char *)RESOURCES_GL_SHADER_VS,
      (const char *)RESOURCES_GL_SHADER_FS);
#else
  renderer->renderer.program = obsc_gl_shader__load_from_file(
      "resources/gl_shader.vs",
      "resources/gl_shader.fs");
#endif

  if (renderer->renderer.program == (unsigned int)(-1))
  {
    free(renderer->renderer.vertices);
    free(renderer);
    return 0;
  }

  sal_position = glGetAttribLocation(renderer->renderer.program, "a_position");
  sal_size = glGetAttribLocation(renderer->renderer.program, "a_size");
  sal_center_position = glGetAttribLocation(renderer->renderer.program, "a_center_position");
  sal_radius = glGetAttribLocation(renderer->renderer.program, "a_radius");
  sal_rect_border = glGetAttribLocation(renderer->renderer.program, "a_rect_border");
  sal_color = glGetAttribLocation(renderer->renderer.program, "a_color");
  sal_border_color = glGetAttribLocation(renderer->renderer.program, "a_border_color");
  sal_texture_id = glGetAttribLocation(renderer->renderer.program, "a_texture_id");
  sal_texture_coordinates = glGetAttribLocation(renderer->renderer.program, "a_texture_coordinates");
  sal_vertex_flags = glGetAttribLocation(renderer->renderer.program, "a_vertex_flags");

  glGenVertexArrays(1, &renderer->renderer.vao);
  glBindVertexArray(renderer->renderer.vao);
  glGenBuffers(1, &renderer->renderer.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->renderer.vbo);
  glGenBuffers(1, &renderer->renderer.ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->renderer.ibo);

  glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(obsc_vertex) * OBSC_RENDERER2D_VERTICES_CAPACITY,
      NULL,
      GL_DYNAMIC_DRAW);

  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      sizeof(unsigned int) * OBSC_RENDERER2D_INDICES_CAPACITY,
      NULL,
      GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(sal_position);
  glVertexAttribPointer(
      sal_position,
      2,
      GL_FLOAT,
      GL_FALSE,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, position));

  glEnableVertexAttribArray(sal_size);
  glVertexAttribPointer(
      sal_size,
      2,
      GL_FLOAT,
      GL_FALSE,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, size));

  glEnableVertexAttribArray(sal_center_position);
  glVertexAttribPointer(
      sal_center_position,
      2,
      GL_FLOAT,
      GL_FALSE,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, center_position));

  glEnableVertexAttribArray(sal_radius);
  glVertexAttribPointer(
      sal_radius,
      1,
      GL_FLOAT,
      GL_FALSE,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, radius));

  glEnableVertexAttribArray(sal_rect_border);
  glVertexAttribPointer(
      sal_rect_border,
      1,
      GL_FLOAT,
      GL_FALSE,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, rect_border));

  glEnableVertexAttribArray(sal_color);
  glVertexAttribIPointer(
      sal_color,
      4,
      GL_UNSIGNED_BYTE,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, color));

  glEnableVertexAttribArray(sal_border_color);
  glVertexAttribIPointer(
      sal_border_color,
      4,
      GL_UNSIGNED_BYTE,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, border_color));

  glEnableVertexAttribArray(sal_texture_id);
  glVertexAttribIPointer(
      sal_texture_id,
      1,
      GL_INT,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, texture_id));

  glEnableVertexAttribArray(sal_texture_coordinates);
  glVertexAttribPointer(
      sal_texture_coordinates,
      4,
      GL_FLOAT,
      GL_FALSE,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, texture_coordinates));

  glEnableVertexAttribArray(sal_vertex_flags);
  glVertexAttribIPointer(
      sal_vertex_flags,
      1,
      GL_INT,
      sizeof(struct obsc_vertex),
      (void *)offsetof(struct obsc_vertex, vertex_flags));

  return renderer;
}

void obsc_renderer2d__free(
    struct obsc_renderer2d **_renderer)
{
  glDeleteVertexArrays(1, &(*_renderer)->renderer.vao);
  glDeleteBuffers(1, &(*_renderer)->renderer.vbo);
  glDeleteBuffers(1, &(*_renderer)->renderer.ibo);
  glDeleteProgram((*_renderer)->renderer.program);

  free(*_renderer);
  *_renderer = 0;
}

void obsc_renderer2d__set_size(
    struct obsc_renderer2d *_renderer,
    int _width,
    int _height)
{
  mat4x4 projection;
  int ul_projection;

  _renderer->renderer.width = _width;
  _renderer->renderer.height = _height;
  glViewport(0, 0, _width, _height);

  ul_projection = glGetUniformLocation(
      _renderer->renderer.program,
      "u_projection");

  mat4x4_ortho(
      projection,
      0.0f,
      _width,
      _height,
      0.0,
      1.0f,
      -1.0f);

  glUniformMatrix4fv(
      ul_projection,
      1,
      GL_FALSE,
      (const float *)&projection);
}

void obsc_renderer2d__begin(struct obsc_renderer2d *_renderer)
{
  _renderer->renderer.vertices_count = 0;
  _renderer->renderer.indices_count = 0;
  _renderer->renderer.texture_units_count = 0;
}

void obsc_renderer2d__flush(struct obsc_renderer2d *_renderer)
{
  size_t i;
  char texture_unit_name[127];

  glUseProgram(_renderer->renderer.program);

  i = 0;
  while (i < _renderer->renderer.texture_units_count)
  {
    sprintf(texture_unit_name, "texture_unit_%u", (unsigned int)i);
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, _renderer->renderer.texture_units[i]);
    glUniform1i(glGetUniformLocation(_renderer->renderer.program, texture_unit_name), i);
    i++;
  }

  glBindVertexArray(_renderer->renderer.vao);
  glBufferSubData(
      GL_ARRAY_BUFFER,
      0,
      sizeof(struct obsc_vertex) * _renderer->renderer.vertices_count,
      _renderer->renderer.vertices);
  glBufferSubData(
      GL_ELEMENT_ARRAY_BUFFER,
      0,
      sizeof(unsigned int) * _renderer->renderer.indices_count,
      _renderer->renderer.indices);

  glDrawElements(
      GL_TRIANGLES,
      _renderer->renderer.indices_count,
      GL_UNSIGNED_INT,
      NULL);

  glBindVertexArray(0);

  _renderer->renderer.vertices_count = 0;
  _renderer->renderer.indices_count = 0;
  _renderer->renderer.texture_units_count = 0;
}

void obsc_renderer2d__end(struct obsc_renderer2d *_renderer)
{
  if (_renderer->renderer.vertices_count)
  {
    obsc_renderer2d__flush(_renderer);
  }
}

void obsc_renderer2d__draw_quad(
    struct obsc_renderer2d *_renderer,
    const float _x,
    const float _y,
    const float _w,
    const float _h,
    const float _radius,
    const float _rect_border,
    const struct obsc_color _color,
    const struct obsc_color _border_color,
    const struct obsc_texture *_texture,
    const struct obsc_vector4 _texture_coordinates,
    const int _vertex_flags)
{
  size_t i, t_i;
  int texture_unit_id = -1;
  struct obsc_vector2 q_s, q_c;
  struct obsc_vector4 q_b, q_t_c;

  if (_renderer->renderer.vertices_count + 6 >=
      OBSC_RENDERER2D_VERTICES_CAPACITY)
  {
    obsc_renderer2d__flush(_renderer);
  }

  if (_texture)
  {
    t_i = 0;

    while (texture_unit_id < 0 && t_i < _renderer->renderer.texture_units_count)
    {
      if (_renderer->renderer.texture_units[t_i] == (unsigned int)_texture->texture.id)
      {
        texture_unit_id = t_i;
      }
      t_i++;
    }
    if (texture_unit_id < 0 &&
        _renderer->renderer.texture_units_count < OBSC_RENDERER2D_MAX_TEXTURE_UNITS)
    {
      _renderer->renderer.texture_units[_renderer->renderer.texture_units_count] = _texture->texture.id;
      texture_unit_id = _renderer->renderer.texture_units_count;
      _renderer->renderer.texture_units_count++;
    }
    if (texture_unit_id < 0 &&
        _renderer->renderer.texture_units_count >= OBSC_RENDERER2D_MAX_TEXTURE_UNITS)
    {
      obsc_renderer2d__flush(_renderer);
      _renderer->renderer.texture_units[_renderer->renderer.texture_units_count] = _texture->texture.id;
      texture_unit_id = _renderer->renderer.texture_units_count;
      _renderer->renderer.texture_units_count++;
    }
  }

  q_s.x = _w;
  q_s.y = _h;

  q_c.x = _x + _w * 0.5f;
  q_c.y = _renderer->renderer.height - (_y + _h * 0.5f);

  q_b.x = _x,
  q_b.y = _y,
  q_b.z = _x + _w,
  q_b.w = _y + _h,

  q_t_c.x = _texture_coordinates.x,
  q_t_c.y = _texture_coordinates.y,
  q_t_c.z = _texture_coordinates.z,
  q_t_c.w = _texture_coordinates.w,

  i = _renderer->renderer.vertices_count;
  _renderer->renderer.vertices[i + 0] = (struct obsc_vertex){{q_b.z, q_b.y}, q_s, q_c, _radius, _rect_border, _color, _border_color, texture_unit_id, {q_t_c.z, q_t_c.y}, _vertex_flags};
  _renderer->renderer.vertices[i + 1] = (struct obsc_vertex){{q_b.x, q_b.y}, q_s, q_c, _radius, _rect_border, _color, _border_color, texture_unit_id, {q_t_c.x, q_t_c.y}, _vertex_flags};
  _renderer->renderer.vertices[i + 2] = (struct obsc_vertex){{q_b.x, q_b.w}, q_s, q_c, _radius, _rect_border, _color, _border_color, texture_unit_id, {q_t_c.x, q_t_c.w}, _vertex_flags};
  _renderer->renderer.vertices[i + 3] = (struct obsc_vertex){{q_b.z, q_b.w}, q_s, q_c, _radius, _rect_border, _color, _border_color, texture_unit_id, {q_t_c.z, q_t_c.w}, _vertex_flags};

  i = _renderer->renderer.indices_count;
  _renderer->renderer.indices[i + 0] = _renderer->renderer.vertices_count + 0;
  _renderer->renderer.indices[i + 1] = _renderer->renderer.vertices_count + 1;
  _renderer->renderer.indices[i + 2] = _renderer->renderer.vertices_count + 2;
  _renderer->renderer.indices[i + 3] = _renderer->renderer.vertices_count + 2;
  _renderer->renderer.indices[i + 4] = _renderer->renderer.vertices_count + 3;
  _renderer->renderer.indices[i + 5] = _renderer->renderer.vertices_count + 0;

  _renderer->renderer.vertices_count += 4;
  _renderer->renderer.indices_count += 6;
}

void obsc_renderer2d__draw_text(
    struct obsc_renderer2d *_renderer,
    const struct obsc_font *_font,
    const char *_text,
    const size_t _size,
    const struct obsc_color _color,
    const float _x,
    const float _y)
{
  const char *temp_char;
  const struct nestbtt_codepoint_box *codepoint_box;
  struct obsc_vector4 texture_coordinates, position;
  float scale, lines;

  position.x = _x;
  position.y = _y;
  temp_char = _text;
  scale = (float)_size / _font->font.size;
  lines = 0.0f;

  while (*temp_char)
  {
    codepoint_box = nestbtt_getCodepointBox(
        *temp_char,
        &_font->font);

    if (!codepoint_box || *temp_char == ' ')
    {
      position.x += (_font->font.space_width * _font->texture->texture.width) * scale;
    }

    if (*temp_char == '\n')
    {
      position.x = _x;
      lines += _size;
    }

    if (codepoint_box && *temp_char > ' ')
    {
      texture_coordinates = (struct obsc_vector4){
          .x = codepoint_box->x,
          .y = codepoint_box->y,
          .z = codepoint_box->x + codepoint_box->w,
          .w = codepoint_box->y + codepoint_box->h};

      position = (struct obsc_vector4){
          .x = position.x,
          .y = _y + lines + codepoint_box->baseline * (float)_font->texture->texture.height * scale,
          .z = codepoint_box->w * (float)_font->texture->texture.width * scale,
          .w = codepoint_box->h * (float)_font->texture->texture.height * scale,
      };

      obsc_renderer2d__draw_quad(
          _renderer,
          position.x,
          position.y,
          position.z,
          position.w,
          0,
          0,
          _color,
          (struct obsc_color){0, 0, 0, 0},
          _font->texture,
          texture_coordinates,
          1 /*1 - flag for font texture*/);

      position.x += codepoint_box->w * (float)_font->texture->texture.width * scale;
    }

    temp_char++;
  }
}

void obsc_renderer2d__draw_text_wide(
    struct obsc_renderer2d *_renderer,
    const struct obsc_font *_font,
    const wchar_t *_text,
    const size_t _size,
    const struct obsc_color _color,
    const float _x,
    const float _y)
{
  const wchar_t *temp_char;
  const struct nestbtt_codepoint_box *codepoint_box;
  struct obsc_vector4 texture_coordinates, position;
  float scale, lines;

  position.x = _x;
  position.y = _y;
  temp_char = _text;
  scale = (float)_size / _font->font.size;
  lines = 0.0f;

  while (*temp_char)
  {
    codepoint_box = nestbtt_getCodepointBox(
        *temp_char,
        &_font->font);

    if (!codepoint_box || *temp_char == ' ')
    {
      position.x += (_font->font.space_width * _font->texture->texture.width) * scale;
    }

    if (*temp_char == '\n')
    {
      position.x = _x;
      lines += _size;
    }

    if (codepoint_box && *temp_char > ' ')
    {
      texture_coordinates = (struct obsc_vector4){
          .x = codepoint_box->x,
          .y = codepoint_box->y,
          .z = codepoint_box->x + codepoint_box->w,
          .w = codepoint_box->y + codepoint_box->h};

      position = (struct obsc_vector4){
          .x = position.x,
          .y = _y + lines + codepoint_box->baseline * (float)_font->texture->texture.height * scale,
          .z = codepoint_box->w * (float)_font->texture->texture.width * scale,
          .w = codepoint_box->h * (float)_font->texture->texture.height * scale,
      };

      obsc_renderer2d__draw_quad(
          _renderer,
          position.x,
          position.y,
          position.z,
          position.w,
          0,
          0,
          _color,
          (struct obsc_color){0, 0, 0, 0},
          _font->texture,
          texture_coordinates,
          1 /*1 - flag for font texture*/);

      position.x += codepoint_box->w * (float)_font->texture->texture.width * scale;
    }

    temp_char++;
  }
}

void obsc_renderer2d__draw_circle(
    struct obsc_renderer2d *_renderer,
    const struct obsc_color _color,
    const float _x,
    const float _y,
    const float _radius)
{
  obsc_renderer2d__draw_quad(
      _renderer,
      _x - _radius,
      _y - _radius,
      _radius * 2.0f,
      _radius * 2.0f,
      _radius,
      0,
      _color,
      (struct obsc_color){0, 0, 0, 0},
      NULL,
      (struct obsc_vector4){0.0f, 0.0f, 0.0f, 0.0f},
      0);
}

void obsc_renderer2d__draw_texture_scale(
    struct obsc_renderer2d *_renderer,
    struct obsc_texture *_texture,
    const float _x,
    const float _y,
    const float _scale)
{
  obsc_renderer2d__draw_quad(
      _renderer,
      _x,
      _y,
      (float)(_texture->texture.width) * _scale,
      (float)(_texture->texture.height) * _scale,
      0,
      0,
      (struct obsc_color){0, 0, 0, 0},
      (struct obsc_color){0, 0, 0, 0},
      _texture,
      (struct obsc_vector4){0.0f, 0.0f, 1.0f, 1.0f},
      0);
}

struct obsc_color obsc_color__lerp(
    const struct obsc_color _a,
    const struct obsc_color _b,
    const float _factor)
{
  struct obsc_color r;

  r.r = OBSC_LERP((int)_a.r, (int)_b.r, _factor);
  r.g = OBSC_LERP((int)_a.g, (int)_b.g, _factor);
  r.b = OBSC_LERP((int)_a.b, (int)_b.b, _factor);
  r.a = OBSC_LERP((int)_a.a, (int)_b.a, _factor);

  return r;
}