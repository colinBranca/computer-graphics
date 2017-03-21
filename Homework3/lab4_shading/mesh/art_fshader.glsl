#version 330


out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform sampler2D tex2D;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;


void main() {
   //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 3.2: Artistic shading.
    /// 3) compute specular term using the texture sampler tex.

    float t1 = dot(normal_mv, light_dir);

    vec3 r = normalize(2.0f * normal_mv * t1 - light_dir);
    float t2 = pow(dot(r, view_dir), alpha);

    vec3 ambient = ka * La;
    vec3 diffuse = kd * t1 * Ld;
    vec3 specular = ks * vec3(texture(tex2D, vec2(t1, t2)).rg, 0.0f) * Ls;

    color = ambient + diffuse + specular;
    //<<<<<<<<<< TODO <<<<<<<<<<<
}
