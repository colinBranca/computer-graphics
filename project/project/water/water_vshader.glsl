#version 330 core

layout (location = 0) in vec3 position;
out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float water_height;
uniform float time;

uniform sampler2D wave_tex;

vec2 getTexCoord(vec3 position) {
    return (position.xz + vec2(5.0f, 5.0f)) * 0.1f;
}

void main() {
    vec2 uvWave = getTexCoord(position);
    uvWave.x = 0.1f * time + uvWave.x;
    float wave = texture(wave_tex, uvWave).x / 10.0f;

    vec3 adjusted_position = vec3(position.x, water_height + wave, position.z);

    gl_Position = projection * view * model * vec4(adjusted_position, 1.0f);
    Normal = mat3(transpose(inverse(model))) * vec3(0.0f, 1.0f, 0.0f);
    Position = vec3(model * vec4(adjusted_position, 1.0f));
}
