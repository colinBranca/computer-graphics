#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float tex_width;
uniform float tex_height;
uniform float sigma;

uniform float kernel[7];
uniform int do_second_pass;

void main() {

    vec3 color_accumulator = vec3(0.0f, 0.0f, 0.0f);
    int size = (kernel.length() - 1) / 2;

    for (int i = -size; i <= size; ++i) {
       if (do_second_pass == 0)
          color_accumulator += texture(tex1, uv + vec2(i / tex_width, 0.0f)).rgb * kernel[i+size];
       else if (do_second_pass == 1)
          color_accumulator += texture(tex2, uv + vec2(0.0f, i / tex_height)).rgb * kernel[i+size];
    }
    color = color_accumulator;


}

