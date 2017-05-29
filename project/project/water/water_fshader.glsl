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

    vec3 view_direction = normalize(Position - camera_position);
    vec3 reflection = reflect(view_direction, normalize(Normal));
    //float ratio = 1.00f / 1.33f;
    //vec3 refraction = refract(view_direction, normalize(Normal), ration);

    vec4 reflection_color = texture(skybox, reflection);
    //vec4 refraction_color = texture(skybox, refraction);

    vec4 water_color = vec4(100.0/255.0, 149.0/255.0, 237.0/255.0, 1.0f);
    vec3 color_from_mirror = texture(tex_mirror, vec2(u, v)).rgb;

    color = mix(water_color, reflection_color, vec4(.8));
}
