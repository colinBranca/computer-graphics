#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform sampler1D colormap;

void main() {
    float height = texture(tex, uv).r;
    color = texture(colormap, height).rgb;
    // color = texture(tex, uv).rgb;
    //color = vec3(0.0, 0.0, 0.0);
}
