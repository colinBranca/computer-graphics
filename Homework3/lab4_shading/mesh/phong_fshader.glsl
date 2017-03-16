#version 330


out vec3 color;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

void main() {
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

    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 1.2: Phong shading.
    // 1) compute ambient term.
    // 2) compute diffuse term.
    // 3) compute specular term.
    // To avoid GPU bug, remove
    // the code above after
    // implementing Phong shading.
    vec3 r = normalize(2.0f * normal * dot(normal, light_dir) - light_dir);

    float nl = dot(normal, light_dir);
    nl = nl < 0 ? 0.0f : nl;
    float rv = dot(r, view_dir);
    rv = rv < 0 ? 0.0f : rv;
    vcolor = (ka * La) + (kd * nl * Ld) + (ks * pow(rv, alpha) * Ls);
    //<<<<<<<<<< TODO <<<<<<<<<<<
}
