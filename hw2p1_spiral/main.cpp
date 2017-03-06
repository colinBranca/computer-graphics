// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "triangle/triangle.h"

#include <cmath>

Triangle triangle;

double PI = 3.14;
double FIB_ANGLE = 137.508;
double ANGLE = PI / 10.0;

char FIBONACCI_SPIRAL = 1;

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    triangle.Init();
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model = IDENTITY_MATRIX;
    // compute transformations here

    glm::mat4 R = IDENTITY_MATRIX;
    R[0][0] = 0.0f;
    R[1][0] = 1.0f;
    R[0][1] = -1.0f;
    R[1][1] = 0.0f;

    glm::mat4 S = IDENTITY_MATRIX;
    S[0][0] = FIBONACCI_SPIRAL == 0 ? 0.005f : 0.025f;
    S[1][1] = FIBONACCI_SPIRAL == 0 ? 0.005f : 0.025f;

    glm::mat4 T = IDENTITY_MATRIX;

    double r = 0;
    double theta = 0;
    double lim = FIBONACCI_SPIRAL == 0 ? 60 : 400;
    for (int i = 1; i < lim; ++i) {
        triangle.Draw(T * R * S);
        if (FIBONACCI_SPIRAL) {
                r = 0.05 * sqrt(i);
                theta = i * FIB_ANGLE * PI / 180.0;
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
        T[3][0] = r * cos(theta);
        T[3][1] = r * sin(theta);
    }
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

