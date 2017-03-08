// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "triangle/triangle.h"

#include <cmath>

char FERMAT_SPIRAL = 0;

Triangle triangle;

float PI = 3.14;
float FERMAT_ANGLE = 137.508;
float ANGLE = PI / 10.0;

glm::mat4 R;
glm::mat4 S;
glm::mat4 T;

float r;
float theta;
float num_triangles;

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    r = 0;
    theta = 0;
    num_triangles = FERMAT_SPIRAL == 0 ? 60 : 400;

    // Initialize triangle
    triangle.Init();
    // Initialize R(otation), S(cale) and T(raslation) matrices
    R = IDENTITY_MATRIX;
    R[0][0] = 0.0f;
    R[1][0] = 1.0f;
    R[0][1] = -1.0f;
    R[1][1] = 0.0f;

    T = IDENTITY_MATRIX;
    S = IDENTITY_MATRIX;
    S[0][0] = FERMAT_SPIRAL == 0 ? 0.005f : 0.020f;
    S[1][1] = FERMAT_SPIRAL == 0 ? 0.005f : 0.020f;
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 1; i < num_triangles; ++i) {
        triangle.Draw(T * R * S);
        if (FERMAT_SPIRAL) {
            r = 0.0045 * sqrt(theta);
            theta = i * FERMAT_ANGLE;
        } else {
            r = 0.04 * theta;
            S[0][0] += 0.0012;
            S[1][1] += 0.0012;
            theta = theta + ANGLE;
        }
        R[0][0] = cos(-theta);
        R[0][1] = -sin(-theta);
        R[1][0] = sin(-theta);
        R[1][1] = cos(-theta);

        T[3][0] = 0.9 * r * cos(theta);
        T[3][1] = 0.9 * r * sin(theta);
    }

    S[0][0] = FERMAT_SPIRAL == 0 ? 0.005f : 0.020f;
    S[1][1] = FERMAT_SPIRAL == 0 ? 0.005f : 0.020f;
    r = 0;
    theta = 0;
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(512, 512, "spiral", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    triangle.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
