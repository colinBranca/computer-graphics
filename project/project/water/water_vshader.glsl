#version 330 core
uniform float water_height;
uniform float time;
uniform sampler2D tex_wave;
uniform sampler2D tex_terrain;
uniform sampler2D tex_sky;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
in vec2 position;

out vec2 uv;
out vec3 Normal;
out vec3 Position;

vec2 getTexCoord(vec2 position) {
    return (position + vec2(5.0f, 5.0f)) * 0.1f;
}

void main() {
    uv = getTexCoord(position);

    vec2 uvWave = uv;
    uvWave.x = 0.1f*time+uvWave.x;
    float wave = texture(tex_wave, uvWave).x / 10.0f ;

    vec3 pos_3d = vec3(position.x, water_height + wave, position.y);
    gl_Position = P * V * M * vec4(pos_3d, 1.0);

    float rightEl = textureOffset(tex_terrain, uv, ivec2(1, 0)).x;
    float downEl = textureOffset(tex_terrain, uv, ivec2(0, -1)).x;
    float leftEl = textureOffset(tex_terrain, uv, ivec2(-1, 0)).x;
    float upEl = textureOffset(tex_terrain, uv, ivec2(0, 1)).x;

    vec3 right = vec3(pos_3d.x + 1.0f / 1024.0f, rightEl, pos_3d.y);
    vec3 left = vec3(pos_3d.x - 1.0f / 1024.0f, leftEl, pos_3d.y);
    vec3 up = vec3(pos_3d.x, upEl, pos_3d.y + 1.0f / 1024.0f);
    vec3 down = vec3(pos_3d.x, downEl, pos_3d.y - 1.0f / 1024.0f);
    vec3 b1 = right - left;
    vec3 b2 = down - up;

    vec3 normal = normalize((transpose(inverse(M * V)) * vec4(cross(b1, b2), 1.0f)).xyz);

    Normal = mat3(transpose(inverse(M))) * normal;
    Position = vec3(M * vec4(pos_3d, 1.0f));
}
