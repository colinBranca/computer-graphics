#version 330

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform float water_height;
uniform sampler2D height_tex;

in vec2 position;

uniform vec3 light_pos;

out vec3 light_dir;
out vec3 view_dir;
out vec4 vpoint_mv;
out vec3 normal;
out float height;
out vec2 uv;
flat out int isVisible; //higher than water or not

vec2 getTexCoord(vec2 position) {
    return (position + vec2(5.0f, 5.0f)) * 0.1f;
}

void main() {
    uv = getTexCoord(position);
    float rightEl = textureOffset(height_tex, uv, ivec2(1, 0)).x;
    float downEl = textureOffset(height_tex, uv, ivec2(0, -1)).x;
    float leftEl = textureOffset(height_tex, uv, ivec2(-1, 0)).x;
    float upEl = textureOffset(height_tex, uv, ivec2(0, 1)).x;

    vec3 right = vec3(uv.x + 1.0f / 512.0f, rightEl, uv.y);
    vec3 left = vec3(uv.x - 1.0f / 512.0f, leftEl, uv.y);
    vec3 up = vec3(uv.x, upEl, uv.y + 1.0f / 512.0f);
    vec3 down = vec3(uv.x, downEl, uv.y - 1.0f / 512.0f);
    vec3 b1 = right - left;
    vec3 b2 = down - up;

    //normal = normalize((transpose(inverse(M)) * vec4(cross(b1, b2), 1.0f)).xyz);
    normal = normalize(cross(b1, b2));

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    vec3 pos_3d = vec3(position.x, texture(height_tex, uv).r, position.y);
    gl_Position = P * V * M * vec4(pos_3d, 1.0);
    vpoint_mv = V * M * vec4(pos_3d, 1.0f);
    //light_dir = normalize((V * M * vec4(light_pos, 1.0f)).xyz - vpoint_mv.xyz);
    light_dir = normalize((M * vec4(light_pos, 1.0f)).xyz - vpoint_mv.xyz);
    view_dir = normalize(- vpoint_mv.xyz);

    //isVisible = (pos_3d.y >= water_height)? 1 : 0;
    isVisible = 1;
}
