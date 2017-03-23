#version 330


out vec3 color;

uniform float alpha;
uniform sampler2D tex2D;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;


void main() {

    vec3 r = normalize(2.0f * normal_mv * dot(normal_mv, light_dir) - light_dir);
    float rv = max(0.0f, dot(r, view_dir));
    float nl = max(0.0f, dot(normal_mv, light_dir));


    color = texture(tex2D, vec2(nl, pow(rv, alpha))).rgb;
}
