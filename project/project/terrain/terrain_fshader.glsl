#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform float water_height;

uniform sampler2D grass_tex;
uniform sampler2D ground_tex;
uniform sampler2D snow_tex;
uniform sampler2D sand_tex;
uniform sampler2D height_tex;

in vec2 uv;
flat in int isVisible;

out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in vec4 vpoint_mv;
in vec3 normal;

vec3 texMix(float height) {
    vec3 grass = texture(grass_tex, 100.0f * uv).rgb;
    vec3 ground = texture(ground_tex, 10.0f * uv).rgb;
    vec3 snow = texture(snow_tex, 30.0f * uv).rgb;
    vec3 sand = texture(sand_tex, 60.0f * uv).rgb;


    float alpha = length(normal.xz);
    //float alphaBis = dot(normal, normalize(vec3(normal.x, 0.0f, normal.z)));
    // if(alpha == alphaBis) return vec3(1,0,0);

    float coef = 500.0f;
    alpha = clamp(coef*alpha-coef + 1.0f, 0.0f, 1.0f);

    vec3 tmpColor;
    if(height < water_height - 0.5f) tmpColor = sand;
    else if(height < water_height + 0.5f) tmpColor = mix(sand, grass, height - water_height + 0.5f);
    else if(height < water_height + 1.5f) tmpColor = mix(grass, snow, height - water_height - 0.5f);
    else tmpColor = snow;

    return mix(tmpColor, ground, alpha);
    //return mix( mix(snow, grass, 1.5f - height), mix(sand, grass, 0.2 - height), height - 1);
}

void main() {

    vec3 colorT = texMix(texture(height_tex, uv).r);
    /// 1) compute ambient term.
    vec3 ambient = ka * La;
    /// 2) compute diffuse term.
    float nl = max(0.0f, dot(normal, light_dir));
    vec3 diffuse = colorT * nl * Ld;
    /// 3) compute specular term.
    vec3 r = normalize(2.0f * normal * dot(normal, light_dir) - light_dir);
    float rv = max(0.0f, dot(r, view_dir));

    color = ambient + diffuse ;//- 0.1;
    if (colorT == vec3(0,0,0)) color = vec3(0,1,0);


    //color = colorT;
    if(isVisible == 0) {
      discard;
    }
}
