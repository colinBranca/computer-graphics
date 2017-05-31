#version 330 core
out vec4 color;
in vec2 uv;
//uniform sampler2D tex;
uniform sampler2D tex_mirror;
uniform samplerCube skybox;
uniform vec3 camera_position;

in vec3 Normal;
in vec3 Position;


void main() {
    ivec2 window_dimensions = textureSize(tex_mirror, 0);
    float window_width = float(window_dimensions.x);
    float window_height = float(window_dimensions.y);

    float u = gl_FragCoord.x / window_width ;
    float v = 1.0f - gl_FragCoord.y / window_height;

    vec3 mirror_normal = vec3(0, 1, 0);
    vec3 flat_normal = Normal - dot(Normal, mirror_normal) * mirror_normal;
    vec4 terrain_reflection = texture(tex_mirror, vec2(u, v) + flat_normal.xz * 0.1);

    vec3 view_direction = normalize(Position - camera_position);
    vec3 reflection = reflect(view_direction, normalize(Normal));

    vec4 sky_reflection = texture(skybox, reflection);

    vec4 water_color = vec4(100.0/255.0, 149.0/255.0, 237.0/255.0, 1.0f);

    color = mix(mix(water_color, sky_reflection, vec4(0.8f)), terrain_reflection, vec4(0.5));

    // vec4 reflection_color = terrain_reflection.xyz != vec3(0.0f) ? terrain_reflection : sky_reflection;
    // color = mix(water_color, reflection_color, vec4(0.8f));
    
    // if (terrain_reflection.x <= 0.1f && terrain_reflection.y <= 0.1f && terrain_reflection.z <= 0.1f) {
    //   color = mix(water_color, sky_reflection, vec4(0.8f));
    // } else {
    //   color = mix(water_color, terrain_reflection, vec4(0.4f));
    // }
}
