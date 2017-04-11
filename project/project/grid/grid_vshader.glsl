#version 330


uniform mat4 MVP;
uniform sampler2D tex;

in vec2 position;

out vec2 uv;


void main() {
    uv =(position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    vec3 pos_3d = vec3(position.x, texture(tex, uv).r, -position.y);
    //vec3 pos_3d = vec3(position.x, 0, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
