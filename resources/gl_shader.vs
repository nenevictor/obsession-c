#version 330
uniform mat4 u_projection;

in vec2 a_position;
in vec2 a_size;
in vec2 a_center_position;
in float a_radius;
in float a_rect_border;
in ivec4 a_color;
in ivec4 a_border_color;
in int a_texture_id;
in vec2 a_texture_coordinates;
in int a_vertex_flags;

out vec2 size;
out vec2 center_position;
out float radius;
out float rect_border;
out vec4 color;
out vec4 border_color;
out float texture_id;
out vec2 texture_coordinates;
out float vertex_flags;

void main() {
    size  = a_size;
    center_position = a_center_position;
    radius = a_radius;
    rect_border = a_rect_border;
    color = a_color / 255.0f;
    border_color = a_border_color / 255.0f;
    texture_id = float(a_texture_id);
    texture_coordinates = a_texture_coordinates;
    vertex_flags = float(a_vertex_flags);

    gl_Position = u_projection * vec4(a_position, 0.0f, 1.0f);
};
