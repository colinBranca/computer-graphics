// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//for maths things
#include<math.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "triangle/triangle.h"

Triangle triangle;


void Init() {
  // sets background color
  glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

  triangle.Init();
}

void Display() {
  glClear(GL_COLOR_BUFFER_BIT);

  glm::mat4 model = IDENTITY_MATRIX;
  // compute transformations here
  // scale
  glm::mat4 S = IDENTITY_MATRIX;
  float scale = 1.0;

  // rotation
  glm::mat4 R = IDENTITY_MATRIX;
  float alpha = 0.0;

  // translation
  glm::mat4 T = IDENTITY_MATRIX;
  float translate = 0.0;

  glm::mat4 M = IDENTITY_MATRIX;
  for(int n=0; n<25;n++){
    //ne manque plus que mettre les bons coefficients, mais je ne comprends pas
    //comment les distribuer entre les trois transformations...
    scale = 0.25;
    S[0][0] = scale;
    S[1][1] = scale;

    alpha = 15*(M_PI/180);
    R[0][0] =  cos(alpha);
    R[0][1] =  sin(alpha);
    R[1][0] = -sin(alpha);
    R[1][1] =  cos(alpha);

    translate = 0.0;
    T[3][0] = 0.5;
    T[3][1] = 0.5;

    M = glm::mat4(S)*glm::mat4(R)*glm::mat4(T);
    triangle.Draw(M);
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
