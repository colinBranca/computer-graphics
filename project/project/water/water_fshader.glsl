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
    float v = 1.0f - gl_FragCoord.y-wave / window_height;

    vec4 terrain_reflection = texture(tex_mirror, vec2(u, v));

    vec3 view_direction = normalize(Position - camera_position);
    vec3 reflection = reflect(view_direction, normalize(Normal));

    vec4 sky_reflection = texture(skybox, reflection);

    vec4 water_color = vec4(100.0/255.0, 149.0/255.0, 237.0/255.0, 1.0f);

    if(terrain_reflection.x <= 0.1f && terrain_reflection.y <= 0.1f && terrain_reflection.z <= 0.1f) {
      color = mix(water_color, sky_reflection, vec4(0.4f));
    }
    else {
      color = mix(water_color, terrain_reflection, vec4(0.4f));
    }
}
