#version 330 core
in vec2 size;
in vec2 center_position;
in float radius;
in float rect_border;
in vec4 color;
in vec4 border_color;
in float vertex_flags;
in vec2 texture_coordinates;
in float texture_id;

uniform sampler2D texture_unit_0;
uniform sampler2D texture_unit_1;
uniform sampler2D texture_unit_2;
uniform sampler2D texture_unit_3;
uniform sampler2D texture_unit_4;
uniform sampler2D texture_unit_5;
uniform sampler2D texture_unit_6;
uniform sampler2D texture_unit_7;
uniform sampler2D texture_unit_8;
uniform sampler2D texture_unit_9;

float rounded_box_sdf(vec2 _center_pos, vec2 _size, float _radius) {
    return length(max(abs(_center_pos)-_size+_radius, 0.0f))-_radius;
}

vec4 get_texture_unit_color(int _id, vec2 _texture_coordinates) {
    switch (_id) {
        case 0:
        return texture(texture_unit_0, _texture_coordinates);
        break;
        case 1:
        return texture(texture_unit_1, _texture_coordinates);
        break;
        case 2:
        return texture(texture_unit_2, _texture_coordinates);
        break;
        case 3:
        return texture(texture_unit_3, _texture_coordinates);
        break;
        case 4:
        return texture(texture_unit_4, _texture_coordinates);
        break;
        case 5:
        return texture(texture_unit_5, _texture_coordinates);
        break;
        case 6:
        return texture(texture_unit_6, _texture_coordinates);
        break;
        case 7:
        return texture(texture_unit_7, _texture_coordinates);
        break;
        case 8:
        return texture(texture_unit_8, _texture_coordinates);
        break;
        case 9:
        return texture(texture_unit_9, _texture_coordinates);
        break;
        default:
        return color;
        break;
    };
    return color;
};

void main() {
    vec4 temp_color = color;
    if (int(vertex_flags) == 0 && int(texture_id) >= 0) {
        float temp_alpha;
        temp_color = get_texture_unit_color(int(texture_id), texture_coordinates);
        temp_alpha = temp_color.w;
        temp_color = mix(temp_color, color, color.w);
        temp_color.w = temp_alpha;
    }

    if (int(vertex_flags) == 1 && int(texture_id) >= 0) {
        float temp_alpha = get_texture_unit_color(int(texture_id), texture_coordinates).w;
        temp_color.xyz = color.xyz;
        temp_color.w = temp_alpha * color.w;
    }

    if (radius > 0.0f) {
        float distance = rounded_box_sdf(gl_FragCoord.xy - center_position, size / 2.0f, radius) + 1.0f; 
        float smoothed_alpha = smoothstep(0.0f, 2.0f, distance);

        temp_color.w = 1.0f - smoothed_alpha;
    };

    if (rect_border > 0.0f) {
        vec2 center_rect = size - vec2(rect_border * 2.0f);
        vec2 rects_ratio = center_rect / size;
        float scaled_radius = radius * ((rects_ratio.x + rects_ratio.y) / 2.0f);
        float border_distance = rounded_box_sdf(gl_FragCoord.xy - center_position, center_rect / 2.0f, scaled_radius); 
        float border_smoothed_alpha = smoothstep(0.0f, 2.0f, border_distance);

        if (border_distance >= 0.0f) {
            temp_color = mix(temp_color, border_color, border_smoothed_alpha);
        };
    };

    gl_FragColor = temp_color;
};
