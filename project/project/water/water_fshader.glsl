#version 330 core

in vec3 Normal;
in vec3 Position;
out vec4 color;

uniform vec3 camera_position;
uniform samplerCube skybox;
uniform sampler2D terrain;

void main() {
    ivec2 window_dimensions = textureSize(terrain, 0);
    float window_width = float(window_dimensions.x);
    float window_height = float(window_dimensions.y);

    float u = gl_FragCoord.x / window_width;
    float v = 1.0f - gl_FragCoord.y / window_height;

    // float u = Position.x;
    // float v = Position.z;

    vec3 view_direction = normalize(Position - camera_position);
    vec3 reflection = reflect(view_direction, normalize(Normal));
    float ratio = 1.00f / 1.33f;
    vec3 refraction = refract(view_direction, normalize(Normal), ratio);

    vec4 reflection_color = texture(skybox, reflection);
    vec4 refraction_color = texture(skybox, refraction);
    vec4 terrain_reflection = texture(terrain,vec2(u,v));
    vec4 water_color = vec4(0.45f, 0.8f, 0.96f, 1.0f);

    vec4 color_reflection = (terrain_reflection.xyz != vec3(1,1,1))? terrain_reflection : reflection_color;
    color = mix(water_color, color_reflection, vec4(0.8f));
}
