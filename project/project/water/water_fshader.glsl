#version 330 core

in vec3 Normal;
in vec3 Position;
out vec4 color;
// in vec2 uv;
//uniform sampler2D tex;
// uniform sampler2D tex_mirror;

uniform vec3 camera_position;
uniform samplerCube skybox;

void main() {
    // ivec2 window_dimensions = textureSize(tex_mirror, 0);
    // float window_width = float(window_dimensions.x);
    // float window_height = float(window_dimensions.y);
    //
    // float u = gl_FragCoord.x / window_width ;
    // float v = 1.0f - gl_FragCoord.y / window_height;

    vec3 I = normalize(Position - camera_position);
    vec3 R = reflect(I, normalize(Normal));
    color = texture(skybox, R);
    // vec3 color_from_mirror = texture(tex_mirror, vec2(u, v)).rgb;
    // color = mix(water_color, color_from_mirror, vec3(.15));
}
