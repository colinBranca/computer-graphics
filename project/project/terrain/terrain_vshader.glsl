#version 330


uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform sampler2D tex;

in vec2 position;

uniform vec3 light_pos;

out vec3 light_dir;
out vec3 view_dir;
out vec4 vpoint_mv;

out vec2 uv;


void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    vec3 pos_3d = vec3(position.x, texture(tex, uv).r, -position.y);
    //vec3 pos_3d = vec3(position.x, 0, -position.y);
    gl_Position = P * V * M * vec4(pos_3d, 1.0);
    vpoint_mv = M * V * vec4(pos_3d, 1.0f);
    light_dir = normalize(light_pos - vpoint_mv.xyz);
    view_dir = normalize(- vpoint_mv.xyz);
}

