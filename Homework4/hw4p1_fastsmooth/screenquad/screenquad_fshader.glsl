#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float tex_width;
uniform float tex_height;

uniform float sigma;
uniform int do_second_pass;

void main() {

    int size = 1 + 4 * int(ceil(sigma * sigma));
    vec3 color_accumulator = vec3(0.0f, 0.0f, 0.0f);
    float weight = 0.0f;

    for (int i = -size; i <= size; ++i) {
       float tmp_w = exp(-pow(i, 2) / (2.0f * pow(sigma, 4)));
       if (do_second_pass == 0)
          color_accumulator += texture(tex1, uv + vec2(i / tex_width, 0.0f)).rgb * tmp_w;
       else if (do_second_pass == 1)
          color_accumulator += texture(tex2, uv + vec2(0.0f, i / tex_height)).rgb * tmp_w;
       weight += tmp_w;
    }
    color = color_accumulator / weight;


}

