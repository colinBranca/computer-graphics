#version 330

layout (location = 0) in vec3 position;
out vec3 texture_coordinates;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    texture_coordinates = position;
}
