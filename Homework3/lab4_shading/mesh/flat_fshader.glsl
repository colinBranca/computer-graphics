#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in vec4 vpoint_mv;


void main() {
    /// 1) compute triangle normal using dFdx and dFdy
    vec3 normal_mv = normalize(cross(dFdx(vpoint_mv.xyz), dFdy(vpoint_mv.xyz)));
    /// 1) compute ambient term.
    vec3 ambient = ka * La;
    /// 2) compute diffuse term.
    float nl = max(0.0f, dot(normal_mv, light_dir));
    vec3 diffuse = kd * nl * Ld;
    /// 3) compute specular term.
    vec3 r = normalize(2.0f * normal_mv * dot(normal_mv, light_dir) - light_dir);
    float rv = max(0.0f, dot(r, view_dir));
    vec3 specular = ks * pow(rv, alpha) * Ls;

    color = ambient + diffuse + specular;
}
