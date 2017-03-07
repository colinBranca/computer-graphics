// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"
float time_s;

Quad topLimit;
glm::mat4 topM;
Quad leftLimit;
glm::mat4 leftM;
Quad rightLimit;
glm::mat4 rightM;

Quad Bar;
glm::mat4 S_bar;
glm::mat4 T_bar;
float barDelta = 0.02;
float halfBarLength;

Quad Ball;
glm::mat4 S_ball;
glm::mat4 T_ball;
GLfloat velocity[] = {0.0, 0.1};
float deltaStart = 0.1;
bool isMoving = false;

void resetBall() {
    T_ball = IDENTITY_MATRIX;
    T_ball[3][0] = T_bar[3][0];
    T_ball[3][1] = T_bar[3][1]+S_bar[1][1];
    velocity[0] = 0.0;
    velocity[1] = 0.1;
    isMoving = false;
}

void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);

    topLimit.Init();
    topM[3][1] = 1;
    topM[1][1] = 0.05;

    rightLimit.Init();
    rightM[3][0] = 1;
    rightM[0][0] = 0.05;

    leftLimit.Init();
    leftM[3][0] = -1;
    leftM[0][0] = 0.05;


    S_bar = IDENTITY_MATRIX;
    T_bar = IDENTITY_MATRIX;
    Bar.Init();
    T_bar[3][1] = -0.9;
    S_bar[0][0] = 0.15;
    S_bar[1][1] = 0.01;
    halfBarLength = S_bar[0][0] / 2;

    S_ball = IDENTITY_MATRIX;
    Ball.Init();
    S_ball[0][0] = 0.01;
    S_ball[1][1] = 0.01;
    resetBall();
}

bool hitBar() {
    return T_ball[3][1] <= T_bar[3][1] &&
           (T_ball[3][0] >= T_bar[3][0] - halfBarLength && T_ball[3][0] <= T_bar[3][0] + halfBarLength);
}

void bounce() {
    if (T_ball[3][0] >= 1 || T_ball[3][0] <= -1) {
        velocity[0] = -velocity[0];
    }
    if (T_ball[3][1] >= 1) {
        velocity[1] = -velocity[1];
    }
    if (hitBar()) {
        velocity[1] = -velocity[1];
        printf("Collision with bar\n");
    }

    if (T_ball[3][1] <= -1) {
        resetBall();
    }

}

void UpdateBall() {
    if (!isMoving) return;

    bounce();

    if (isMoving) {
        T_ball[3][0] += velocity[0];
        T_ball[3][1] += velocity[1];
    }
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    topLimit.Draw(topM);
    rightLimit.Draw(rightM);
    leftLimit.Draw(leftM);

    time_s = glfwGetTime();

    Bar.Draw(T_bar * S_bar);
    Ball.Draw(T_ball * S_ball);

}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_RIGHT) {
        T_bar[3][0] += barDelta;
    }
    if (key == GLFW_KEY_LEFT) {
        T_bar[3][0] -= barDelta;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        isMoving = !isMoving;
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
        UpdateBall();
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
