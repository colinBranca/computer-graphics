#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in vec4 vpoint_mv;


void main() {
/*
    color = vec3(0.0,0.0,0.0);
    const vec3 COLORS[6] = vec3[](
        vec3(1.0,0.0,0.0),
        vec3(0.0,1.0,0.0),
        vec3(0.0,0.0,1.0),
        vec3(1.0,1.0,0.0),
        vec3(0.0,1.0,1.0),
        vec3(1.0,0.0,1.0));
    int index = int( mod(gl_PrimitiveID,6) );
    color = COLORS[index];
*/

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 4.2: Flat shading.
    /// 1) compute triangle normal using dFdx and dFdy
    vec3 normal_mv = normalize(cross(dFdx(vpoint_mv.xyz), dFdy(vpoint_mv.xyz)));
    /// 1) compute ambient term.
    vec3 ambient = ka * La;
    /// 2) compute diffuse term.
    float nl = dot(normal_mv, light_dir);
    nl = nl < 0 ? 0.0f : nl;
    vec3 diffuse = kd * nl * Ld;
    /// 3) compute specular term.
    vec3 r = normalize(2.0f * normal_mv * dot(normal_mv, light_dir) - light_dir);
    float rv = dot(r, view_dir);
    rv = rv < 0 ? 0.0f : rv;
    vec3 specular = ks * pow(rv, alpha) * Ls;

    color = ambient + diffuse + specular;
    ///<<<<<<<<<< TODO <<<<<<<<<<<
}
