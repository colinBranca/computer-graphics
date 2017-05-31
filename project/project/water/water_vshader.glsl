#version 330 core
uniform mat4 MVP;
uniform float water_height;
uniform float time;
uniform sampler2D wave_tex;
uniform mat4 model;

uniform float cell_size;
uniform float size;
uniform float minX;
uniform float minY;

in vec2 position;

out vec2 uv;
out vec3 Normal;
out vec3 Position;

vec2 getTexCoord(vec2 position) {
    return (position + vec2(size + minX, size + minY)) / size;
}

float computeHeight(float x, float z) {
    x = 0.05f * time + x;
    return texture(wave_tex, vec2(2*x, 2*z)).x / 10.0f;
}

vec3 computeNormal(float x, float z) {
    float height_east = computeHeight(x + cell_size, z) + water_height;
    float height_west = computeHeight(x - cell_size, z) + water_height;
    float height_north = computeHeight(x, z - cell_size) + water_height;
    float height_south = computeHeight(x, z + cell_size) + water_height;
    return normalize(vec3(height_west - height_east, 1.0f, height_south - height_north));
}

void main() {
    uv = getTexCoord(position);
    
    float wave = computeHeight(uv.x, uv.y);

    vec3 pos_3d = vec3(position.x, water_height + wave, position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0f);

    Normal = mat3(transpose(inverse(model))) * computeNormal(uv.x, uv.y);
    Position = vec3(model * vec4(pos_3d, 1.0f));
}
