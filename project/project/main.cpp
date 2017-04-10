// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "grid/grid.h"

#include "trackball.h"
#include "framebuffer.h"
#include "perlin/PerlinNoise.h"
#include "screenquad/screenquad.h"

Grid grid;
FrameBuffer noiseContainer;
ScreenQuad screenquad;
PerlinNoise perlin;

int window_width = 600;
int window_height = 600;

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;
mat4 old_trackball_matrix;
mat4 quad_model_matrix;

float old_vertical_mouse_pos;

Trackball trackball;

mat4 OrthographicProjection(float left, float right, float bottom,
                            float top, float near, float far) {
    assert(right > left);
    assert(far > near);
    assert(top > bottom);
    mat4 projection = mat4(1.0f);
    projection[0][0] = 2.0f / (right - left);
    projection[1][1] = 2.0f / (top - bottom);
    projection[2][2] = -2.0f / (far - near);
    projection[3][3] = 1.0f;
    projection[3][0] = -(right + left) / (right - left);
    projection[3][1] = -(top + bottom) / (top - bottom);
    projection[3][2] = -(far + near) / (far - near);
    return projection;
}

mat4 PerspectiveProjection(float fovy, float aspect, float near, float far) {
    // Use matrix from class slides
    mat4 projection = IDENTITY_MATRIX;
    float top = near * tan(radians(fovy));
    float bottom = -top;
    float right = top * aspect;
    float left = -right;
    projection[0][0] = (2 * near) / (right - left);
    projection[2][0] = (right + left) / (right - left);
    projection[1][1] = (2 * near) / (top - bottom);
    projection[2][1] = (top + bottom) / (top - bottom);
    projection[2][2] = -(far + near) / (far - near);
    projection[3][2] = (-2.0f * far * near) / (far - near);
    projection[2][3] = -1.0f;

    return projection;
}

mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    // we need a function that converts from world coordinates into camera coordiantes.
    //
    // cam coords to world coords is given by:
    // X_world = R * X_cam + eye
    //
    // inverting it leads to:
    //
    // X_cam = R^T * X_world - R^T * eye
    //
    // or as a homogeneous matrix:
    // [ r_00 r_10 r_20 -r_0*eye
    //   r_01 r_11 r_21 -r_1*eye
    //   r_02 r_12 r_22 -r_2*eye
    //      0    0    0        1 ]

    vec3 z_cam = normalize(eye - center);
    vec3 x_cam = normalize(cross(up, z_cam));
    vec3 y_cam = cross(z_cam, x_cam);

    mat3 R(x_cam, y_cam, z_cam);
    R = transpose(R);

    mat4 look_at(vec4(R[0], 0.0f),
                 vec4(R[1], 0.0f),
                 vec4(R[2], 0.0f),
                 vec4(-R * (eye), 1.0f));
    return look_at;
}

void Init() {
    // sets background color
    //glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);
    glClearColor(0, 0, 0 /*gray*/, 1.0 /*solid*/);


    // enable depth test.
    glEnable(GL_DEPTH_TEST);

    view_matrix = LookAt(vec3(2.0f, 2.0f, 4.0f),
                        vec3(0.0f, 0.0f, 0.0f),
                        vec3(0.0f, 1.0f, 0.0f));
    view_matrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -2.0f));

    trackball_matrix = IDENTITY_MATRIX;

    quad_model_matrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, 0.0f));

    // Draw Perlin noise on framebuffer for later use
    perlin.Init(window_width, window_height, 8);
    int framebuffer_id = noiseContainer.Init(window_width, window_height);
    screenquad.Init(window_width, window_height, framebuffer_id);
    noiseContainer.Bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            perlin.Draw();
    noiseContainer.Unbind();

    grid.Init(1024, framebuffer_id);

}

// gets called for every frame.
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    grid.Draw(trackball_matrix * quad_model_matrix, view_matrix, projection_matrix);
    //screenquad.Draw();
    //perlin.Draw();
    glViewport(0, 0, window_width, window_height);
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f,
                1.0f - 2.0f * (float)y / height);
}

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        trackball.BeingDrag(p.x, p.y);
        old_trackball_matrix = trackball_matrix;
        // Store the current state of the model matrix.
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        // Store the mouse position to be able to determine the direction
        // of the zoom.
        old_vertical_mouse_pos = TransformScreenCoords(window, x_i, y_i).y;
    }

}

void MousePos(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        vec2 p = TransformScreenCoords(window, x, y);
        // Compute new rotation matrix combining the latest transformation
        // with the previous one.
        trackball_matrix = trackball.Drag(p[0], p[1]) * old_trackball_matrix;
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        // Zoom is a simple translation of the view matrix.
        // Direction chosen comparing current pointer position with its
        // previous one.
        float newPos = TransformScreenCoords(window, x, y).y;
        float cursorY = 0.1f * (y / ((float) window_height)); // 0.1 is for fluidity
        cursorY = newPos > old_vertical_mouse_pos ? -cursorY : cursorY; // decide direction
        view_matrix = translate(view_matrix, vec3(0.0f, 0.0f, cursorY));
        old_vertical_mouse_pos = newPos;
    }
}

// Gets called when the windows/framebuffer is resized.
void SetupProjection(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
         << window_width << "x" << window_height << "." << endl;

    glViewport(0, 0, window_width, window_height);

    projection_matrix = PerspectiveProjection(45.0f,
                                               (GLfloat)window_width / window_height,
                                               0.1f, 100.0f);
    GLfloat top = 1.0f;
    GLfloat right = (GLfloat)window_width / window_height * top;
    //projection_matrix = OrthographicProjection(-right, right, -top, top, -10.0, 10.0f);
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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                          "Project", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, SetupProjection);

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MousePos);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

                check_error_gl();
    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    SetupProjection(window, window_width, window_height);

    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    perlin.Cleanup();
    grid.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
