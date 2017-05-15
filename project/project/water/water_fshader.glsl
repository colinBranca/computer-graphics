#version 330 core

in vec3 Normal;
in vec3 Position;
out vec4 color;

uniform vec3 camera_position;
uniform samplerCube skybox;

void main() {
    vec3 view_direction = normalize(Position - camera_position);
    vec3 reflection = reflect(view_direction, normalize(Normal));
    float ratio = 1.00f / 1.33f;
    vec3 refraction = refract(view_direction, normalize(Normal), ratio);

    vec4 reflection_color = texture(skybox, reflection);
    vec4 refraction_color = texture(skybox, refraction);
    vec4 water_color = vec4(0.45f, 0.8f, 0.96f, 1.0f);

    color = mix(water_color, reflection_color, vec4(0.8f));
}
