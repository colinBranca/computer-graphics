// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "skybox/skybox.h"
#include "perlin/PerlinNoise.h"
#include "screenquad/screenquad.h"
#include "terrain/terrain.h"
#include "water/water.h"

#include "camera.h"
#include "framebuffer.h"

using namespace glm;

Terrain terrain;
ScreenQuad screenquad;
PerlinNoise perlin;
Skybox skybox;
Water water;
FrameBuffer waterReflexion;
Camera camera(vec3(0.0f, 0.0f, 3.0f));

int window_width = 800;
int window_height = 600;

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
int mouse_input_mode = GLFW_CURSOR_DISABLED;

GLfloat last_frame = 0.0f;

mat4 quad_model_matrix;

float water_height;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS) {
        camera.keys_[key] = true;

        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_F1:
            water_height += 0.1f;
            break;
        case GLFW_KEY_F2:
            water_height -= 0.1f;
            break;
        case GLFW_KEY_F3:
            camera.switchCameraMode();
            break;
        case GLFW_KEY_0:
        case GLFW_KEY_1:
        case GLFW_KEY_2:
        case GLFW_KEY_3:
        case GLFW_KEY_4:
            skybox.Cleanup();
            skybox.Init(key - 48);
            break;
        }
    } else if (action == GLFW_RELEASE) {
        camera.keys_[key] = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}

void mouse_movement_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mod)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        mouse_input_mode = mouse_input_mode == GLFW_CURSOR_NORMAL ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
        glfwSetInputMode(window, GLFW_CURSOR, mouse_input_mode);
    }
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
    // glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);
    glClearColor(0, 0, 0 /*gray*/, 1.0 /*solid*/);

    skybox.Init();
    water.Init();

    // enable depth test.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    water_height = 0.0f;

    quad_model_matrix = translate(mat4(1.0f), vec3(0.0f, 0.25f, 0.0f));

    // Draw Perlin noise on framebuffer for later use
    int height_map_tex_id = perlin.Init(1024, 1024, 8);
    perlin.Compute();

    int water_wave_tex_id = perlin.Init(1024, 1024, 1, 1.0f);
    perlin.Compute();
    //screenquad.Init(window_width, window_height, height_map_tex_id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint framebuffer_texture_id = waterReflexion.Init(window_width, window_height);
    water.Init(framebuffer_texture_id, water_wave_tex_id);

    terrain.Init(1024, height_map_tex_id);
}

// gets called for every frame.
void Display() {
    const float time = glfwGetTime();

    glViewport(0, 0, window_width, window_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    mat4 view = mat4(mat3(camera.getViewMatrix()));
    mat4 projection = perspective(camera.zoom_, (float) window_width / (float) window_height, 0.1f, 100.0f);
    mat4 scale = mat4(20.0f);
    scale[3][3] = 1.0f;

    skybox.Draw(scale, view, projection);

    view = camera.getViewMatrix();

    water.Draw(IDENTITY_MATRIX, view, projection, water_height, time);
    terrain.Draw(IDENTITY_MATRIX, view, projection);

    // mirror the camera position
    mat4 mirror_view = camera.getReversedViewMatrix(water_height);

    waterReflexion.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.Draw(IDENTITY_MATRIX, mirror_view, projection);
        terrain.Draw(IDENTITY_MATRIX, mirror_view, projection, water_height);
    waterReflexion.Unbind();

    // skybox.Draw(cube_scale, view, projection);
    // water.Draw(trackball_matrix * quad_model_matrix, view, projection, water_height);

    glDisable(GL_DEPTH_TEST);
}

// Gets called when the windows/framebuffer is resized.
void buffer_resize_callback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
         << window_width << "x" << window_height << "." << endl;

    glViewport(0, 0, window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
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

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_movement_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Disable mouse pointer
    glfwSetInputMode(window, GLFW_CURSOR, mouse_input_mode);

    // Set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, buffer_resize_callback);

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

    // render loop
    while(!glfwWindowShouldClose(window)){
        // Set frame time
        GLfloat current_frame = glfwGetTime();
        GLfloat delta_time = current_frame - last_frame;
        last_frame = current_frame;

        glfwPollEvents();
        camera.update(delta_time);
        Display();
        glfwSwapBuffers(window);
    }

    perlin.Cleanup();
    terrain.Cleanup();
    skybox.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
