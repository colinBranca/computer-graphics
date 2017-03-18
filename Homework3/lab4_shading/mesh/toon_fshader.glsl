#version 330

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

out vec3 color;

uniform sampler1D tex1D;

void main() {
    color = vec3(0.0,0.0,0.0);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 2.2: Toon shading.
    /// 1) compute ambient term.
    vec3 ambient = ka*La;
    /// 2) compute diffuse term using the texture sampler tex.
    vec3 diffuse = kd * texture(tex1D, dot(normal_mv, light_dir)).rgb * Ld;
    /// 3) compute specular term using the texture sampler tex.
    vec3 r = normalize(2.0f * normal_mv * dot(normal_mv, light_dir) - light_dir);
    vec3 specular = ks * texture(tex1D, pow(dot(r, view_dir), alpha)).rgb * Ls;
    ///<<<<<<<<<< TODO <<<<<<<<<<<
    color = ambient + diffuse + specular;
}
