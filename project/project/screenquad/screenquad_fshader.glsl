#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex1;
uniform float tex_width;
uniform float tex_height;

void main() {

    color = texture(tex1, uv).rgb;
    //vec3 color_accumulator = vec3(0.0f, 0.0f, 0.0f);
    //color = color_accumulator;


}

