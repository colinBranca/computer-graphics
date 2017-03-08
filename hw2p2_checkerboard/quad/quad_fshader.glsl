#version 330

in vec2 uv;
out vec3 color;
uniform sampler1D colormap;

void main() {
    float sinx = sin(10 * 3.14 * uv.x);
    float siny = sin(10 * 3.14 * uv.y);
    color = texture(colormap, (sinx * siny + 1) / 2.0).rgb;
}
