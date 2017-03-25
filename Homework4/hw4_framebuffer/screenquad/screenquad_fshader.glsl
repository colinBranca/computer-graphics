#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

mat3 Gx = mat3(
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1
);

mat3 Gy = mat3(
        -1, 0, 1,
        -2,  0, 2,
        -1, 0, 1
);

float rgb_2_luma(vec3 c) {
    return 0.3*c[0] + 0.59*c[1] + 0.11*c[2];
}

void main() {
    float Ix = 0.0f;
    float Iy = 0.0f;
    for (int row = -1; row <= 1; ++row) {
       for (int col = -1; col <= 1; ++col) {
        float a = rgb_2_luma(texture(tex, uv+vec2(row/tex_width, col/tex_height)).rgb);
        Ix += Gx[row+1][col+1] * a;
        Iy += Gy[row+1][col+1] * a;
       }
    }
    float I = 1.0f - sqrt(Ix*Ix + Iy*Iy);
    color = vec3(I, I, I);
    //color = texture(tex,uv).rgb;


}

