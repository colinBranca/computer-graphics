#version 330

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
out vec3 color;


const float spot_cos_cutoff = 0.985; // cos 10 degrees
const float spot_exp = 150;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;
//in vec3 spot_dirv;
uniform vec3 spot_dir;

void main() {
   vec3 r = normalize(2.0f * normal_mv * dot(normal_mv, light_dir) - light_dir);

    float nl = dot(normal_mv, light_dir);
    nl = nl < 0 ? 0.0f : nl;
    float rv = dot(r, view_dir);
    rv = rv < 0 ? 0.0f : rv;
    vec3 I = (ka * La) + (kd * nl * Ld) + (ks * pow(rv, alpha) * Ls);

    float spotEffect = pow(dot(light_dir, normalize(spot_dir)), spot_exp);
    color = I * spotEffect;
}
