#version 330 core
uniform mat4 MVP;
uniform float water_height;
uniform sampler2D wave_tex;
in vec2 position;
out vec2 uv;

vec2 getTexCoord(vec2 position) {
    return (position + vec2(5.0f, 5.0f)) * 0.1f;
}

void main() {
    uv = getTexCoord(position);
    float wave = texture(wave_tex, uv).x / 5.0f ;

    vec3 pos_3d = vec3(position.x, water_height + wave, position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
}
