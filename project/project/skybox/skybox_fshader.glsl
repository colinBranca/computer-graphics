#version 330

in vec3 TexCoords;
flat in int isVisible;
out vec4 color;

uniform samplerCube skybox;

void main() {
    color = texture(skybox, TexCoords);

    // if(isVisible == 0) {
    //   discard;
    // }
}
