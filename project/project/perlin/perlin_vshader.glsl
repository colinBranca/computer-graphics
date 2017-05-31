#version 330

in vec2 vtexcoord;
in vec3 vpoint;

uniform int width;
uniform int height;

out vec2 uv;

void main() {
    gl_Position = vec4(vpoint, 1.0);
    uv = vtexcoord;
    //uv = vec2(vtexcoord.x, height - vtexcoord.y);
}
