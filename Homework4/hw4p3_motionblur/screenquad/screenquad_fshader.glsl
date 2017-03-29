#version 330 core
in vec2 uv;
uniform sampler2D colorTex;
uniform sampler2D velocityTex;
out vec4 color;

void main() {
    /// TODO: use the velocity vector stored in velocityTex to compute the line integral
    /// TODO: use a constant number of samples for integral (what happens if you take too few?)
    /// HINT: you can scale the velocity vector to make the motion blur effect more prominent
    /// HINT: to debug integration don't use the velocityTex, simply assume velocity is constant, e.g. vec2(1,0)
    color = texture(colorTex, uv);
    float scale = 0.1;
    vec2 velocity = scale * texture(velocityTex, uv).xy;
    vec2 uv_new = uv + velocity;
    int samples = 20;
    for(int i=0; i<samples; i++) {
      color += texture(colorTex, uv_new);
      uv_new += velocity;
    }
    color = color / samples;
}
