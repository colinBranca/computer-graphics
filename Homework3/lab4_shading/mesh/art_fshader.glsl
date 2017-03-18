#version 330


out vec3 color;

uniform float alpha;
uniform sampler2D tex2D;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;


void main() {
    color = vec3(0.0, 0.0, 0.0);

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
    // TODO 3.2: Artistic shading.
    /// 3) compute specular term using the texture sampler tex.
    vec3 r = normalize(2.0f * normal_mv * dot(normal_mv, light_dir) - light_dir);

    float t1 = pow(dot(r, view_dir), alpha);
    float t2 = dot(normal_mv, light_dir);

    color = texture(tex2D, vec2(t2, t1)).rgb;
    //<<<<<<<<<< TODO <<<<<<<<<<<
}
