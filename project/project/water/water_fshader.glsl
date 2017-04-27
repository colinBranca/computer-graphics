#version 330 core
out vec3 color;
in vec2 uv;
//uniform sampler2D tex;
uniform sampler2D tex_mirror;

void main() {
    ivec2 window_dimensions = textureSize(tex_mirror, 0);
    float window_width = float(window_dimensions.x);
    float window_height = float(window_dimensions.y);

    float u = gl_FragCoord.x / window_width ;
    float v = 1.0f - gl_FragCoord.y / window_height;

    vec3 water_color = vec3(100.0/255.0, 149.0/255.0, 237.0/255.0);
    vec3 color_from_mirror = texture(tex_mirror, vec2(u, v)).rgb;
    color = mix(water_color, color_from_mirror, vec3(.15));
}
