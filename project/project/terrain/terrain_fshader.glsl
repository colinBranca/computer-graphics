#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

uniform sampler2D grass_tex;
uniform sampler2D ground_tex;
uniform sampler2D snow_tex;
uniform sampler2D height_tex;

in vec2 uv;
flat in int isVisible;

out vec3 color;

in vec3 light_dir;
in vec3 view_dir;
in vec4 vpoint_mv;
in vec3 normal;

vec3 texMix(float height) {
  vec3 mixed;
  vec2 extCoods = 150.0f * uv;
  vec3 gro = texture(ground_tex, extCoods).rgb;
  vec3 gra = texture(grass_tex, extCoods).rgb;
  vec3 sno = texture(snow_tex, extCoods).rgb;

  if(height < 0.0f) mixed = gro;
  else if(height <= 1.0f) mixed = mix(gro, gra, height);
  else if (height <= 2.0f) mixed = mix(gra, sno, height-1.0f);
  else mixed = sno;

  return mixed;
}

void main() {

    vec3 colorT = texMix(texture(height_tex, uv).r);
    //vec3 colorT = texture(snow_tex, uv).rgb;
    vec3 normal_mv = normal;
    /// 1) compute ambient term.
    vec3 ambient = ka * La;
    /// 2) compute diffuse term.
    float nl = max(0.0f, dot(normal_mv, light_dir));
    vec3 diffuse = colorT * nl * Ld;
    /// 3) compute specular term.
    vec3 r = normalize(2.0f * normal_mv * dot(normal_mv, light_dir) - light_dir);
    float rv = max(0.0f, dot(r, view_dir));
    //vec3 specular = ks * pow(rv, alpha) * Ls;

    color = ambient + diffuse- 0.1;
    if (colorT == vec3(0,0,0)) color = vec3(0,1,0);


    if(isVisible == 0) {
      discard;
    }
}
