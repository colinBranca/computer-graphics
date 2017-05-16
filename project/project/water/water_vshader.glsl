#version 330 core

in vec3 position;
out vec3 Normal;
out vec3 Position;

uniform sampler2D tex_wave;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float cell_size;
uniform float water_height;
uniform float time;

const float weight = 1.0f;
const float amplitude = 0.1f;
const float phase = 1.0f;
vec3 wave_position = vec3(0.0f, water_height, 0.0f);

// float computeHeight(float x, float z, vec2 direction) {
//     return amplitude * sin(dot(direction, vec2(x, mod(z, 2.0f))) * weight + time * phase);
// }
//
// float computeHeight(float x, float z, vec2 direction) {
//     return amplitude * sin(x * weight + time * phase);
// }

float computeHeight(float x, float z) {
  x += 0.01f*time;
  return texture(tex_wave, vec2(x,z)).x /10.0f;
}

vec2 getTexCoord(vec2 position) {
    return (position + vec2(5.0f, 5.0f)) * 0.1f;
}

// vec3 computeNormal(float x, float z, vec2 direction) {
//     float height_east = computeHeight(x + cell_size, z, direction);
//     float height_west = computeHeight(x - cell_size, z, direction);
//     float height_north = computeHeight(x, z - cell_size, direction);
//     float height_south = computeHeight(x, z + cell_size, direction);
//     return normalize(vec3(height_west - height_east, 1.0f, height_south - height_north));
// }

vec3 computeNormal(float x, float z) {
    float height_east = computeHeight(x + cell_size, z);
    float height_west = computeHeight(x - cell_size, z);
    float height_north = computeHeight(x, z - cell_size);
    float height_south = computeHeight(x, z + cell_size);
    return normalize(vec3(height_west - height_east, 1.0f, height_south - height_north));
}

void main() {
    vec2 uv = getTexCoord(position.xz);
    vec2 direction = (wave_position - position).xz;
    vec3 adjusted_position =
        vec3(position.x, water_height + computeHeight(uv.x, uv.y), position.z);

    gl_Position = projection * view * model * vec4(adjusted_position, 1.0f);
    Normal = mat3(transpose(inverse(model))) * computeNormal(uv.x, uv.y);
    Position = vec3(model * vec4(adjusted_position, 1.0f));
}
