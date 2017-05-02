#version 330

uniform mat4 MVP;
uniform int reflexion;

layout (location = 0) in vec3 position;
out vec3 TexCoords;
flat out int isVisible; //higher than water or not

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    TexCoords = position;
    //isVisible = (reflexion == 1 && gl_Position.z < 10.0f)? 0 : 1;
}
