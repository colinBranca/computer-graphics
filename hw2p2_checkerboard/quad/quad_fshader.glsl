#version 330

in vec2 uv;
out vec3 color;
uniform sampler1D colormap;

void main() {
    int x = int(floor(uv.x * 10));
    int y = int(floor(uv.y * 10));
    int col = 0;
    if (mod(x, 2) == 0) {
        col = mod(y, 2) == 0 ? 0 : 2;
    } else {
        col = mod(y, 2) == 0 ? 2 : 0;
    }
    //float value = sin(uv.x % 2); // Il faut utiliser sin de qqchose comme indiqué dans slides
    color = texture(colormap, col).rgb;
}
