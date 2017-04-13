#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

in vec2 uv;

out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in vec4 vpoint_mv;
in vec3 normal;

uniform sampler2D tex;
uniform sampler1D colormap;

void main() {
    // color = texture(tex, uv).rgb;
    //color = vec3(0.0, 0.0, 0.0);
    float height = texture(tex, uv).r;
    vec3 colorT = texture(colormap, height).rgb;

    // OLDvec3 normal_mv = normalize(cross(dFdx(vpoint_mv.xyz), dFdy(vpoint_mv.xyz)));
    vec3 normal_mv = normal;
    /// 1) compute ambient term.
    //vec3 ambient = ka * La;
    vec3 ambient = ka * La;
    /// 2) compute diffuse term.
    float nl = max(0.0f, dot(normal_mv, light_dir));
    //vec3 diffuse = kd * nl * Ld;
    vec3 diffuse = colorT * nl * Ld;
    /// 3) compute specular term.
    vec3 r = normalize(2.0f * normal_mv * dot(normal_mv, light_dir) - light_dir);
    float rv = max(0.0f, dot(r, view_dir));
    //vec3 specular = ks * pow(rv, alpha) * Ls;

    color = ambient + diffuse /*+ specular*/ - 0.1;
}

