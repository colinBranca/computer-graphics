#version 330

in vec3 vpoint;
in vec3 vnormal;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos; // camera space

out vec3 vcolor;


void main() {
    mat4 MV = view * model;
    vec4 vpoint_mv = MV * vec4(vpoint, 1.0f);
    gl_Position = projection * vpoint_mv;

    // 1) compute the normal using the model_view matrix.
    vec3 normal = normalize( (transpose(inverse(MV)) * vec4(vnormal, 1.0f)).xyz );
    // 2) compute the light direction light_dir.
    vec3 light_dir = normalize(light_pos - vpoint_mv.xyz);
    // 3) compute the view direction view_dir.
    vec3 view_dir = normalize(- vpoint_mv.xyz);
    // 4) compute per vertex color
    vec3 r = normalize(2.0f * normal * dot(normal, light_dir) - light_dir);

    float nl = max(0.0f, dot(normal, light_dir));
    float rv = max(0.0f, dot(r, view_dir));

    vcolor = (ka * La) + (kd * nl * Ld) + (ks * pow(rv, alpha) * Ls);
}
