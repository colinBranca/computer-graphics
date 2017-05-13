#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float water_height;
uniform float time;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 Position;

// uniform sampler2D wave_tex;
// out vec2 uv;

// vec2 getTexCoord(vec2 position) {
//     return (position + vec2(5.0f, 5.0f)) * 0.1f;
// }

void main() {
    // uv = getTexCoord(position);
    // vec2 uvWave = uv;
    // uvWave.x = 0.1f*time+uvWave.x;
    // float wave = texture(wave_tex, uvWave).x / 10.0f ;

    // vec3 pos_3d = vec3(position.x, water_height + wave, position.y);
    gl_Position = projection * view * model * vec4(position, 1.0f);
    Normal = mat3(transpose(inverse(model))) * normal;
    Position = vec3(model * vec4(position, 1.0f));
}
