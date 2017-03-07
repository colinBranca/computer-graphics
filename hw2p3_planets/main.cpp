// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"

// Quad stuff1;
// ...
Quad Sun;
Quad Earth;
Quad Moon;

float SunFactor = 0.25;
float EarthFactor = 2.0;
float MoonFactor = 3.4;

void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);
    // {stuff}.Init(...);
    Sun.Init("sun.tga");
    Earth.Init("earth.tga");
    Moon.Init("moon.tga");
}

glm::mat4 rotation(float time, float factor) {
    glm::mat4 result = IDENTITY_MATRIX;
    float angle = time * factor;
    result[0][0] = cos(angle);
    result[1][0] = -sin(angle);
    result[0][1] = sin(angle);
    result[1][1] = cos(angle);
    return result;
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float time_s = glfwGetTime();

    float factor = 5;
    float excentricity = 0.016*factor;
    float semiMajorAxis = 0.15*factor;
    float semiMinorAxis = semiMajorAxis*sqrt(1 - excentricity*excentricity);
    //float sunPosition = sqrt(semiMajorAxis*semiMajorAxis - semiMinorAxis*semiMinorAxis); // Doesn't work

    glm::mat4 R_Sun = rotation(time_s, SunFactor);
    glm::mat4 S_Sun = IDENTITY_MATRIX;
    S_Sun[0][0] = 0.15;
    S_Sun[1][1] = 0.15;
    glm::mat4 T_Sun = IDENTITY_MATRIX;
    T_Sun[3][0] = excentricity;
    Sun.Draw(T_Sun * R_Sun * S_Sun);

    glm::mat4 R_Earth = rotation(time_s, EarthFactor);
    glm::mat4 S_Earth = IDENTITY_MATRIX;
    S_Earth[0][0] = 0.06;
    S_Earth[1][1] = 0.06;
    glm::mat4 T_Earth = IDENTITY_MATRIX;
    T_Earth[3][0] = semiMajorAxis * cos(time_s);
    T_Earth[3][1] = semiMinorAxis * sin(time_s);
    Earth.Draw(T_Earth * R_Earth * S_Earth);

    glm::mat4 R_Moon = rotation(time_s, MoonFactor);
    glm::mat4 S_Moon = IDENTITY_MATRIX;
    S_Moon[0][0] = 0.03;
    S_Moon[1][1] = 0.03;
    glm::mat4 T_Moon = IDENTITY_MATRIX;
    T_Moon[3][0] = T_Earth[3][0] + 0.13 * cos(5.0*time_s);
    T_Moon[3][1] = T_Earth[3][1] + 0.13 * sin(5.0*time_s);
    Moon.Draw(T_Moon * R_Moon * S_Moon);
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "planets", NULL, NULL);
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

    // {stuff}.Cleanup()

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
