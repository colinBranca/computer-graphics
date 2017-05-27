// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "skybox/skybox.h"
#include "screenquad/screenquad.h"

#include "camera.h"
#include "framebuffer.h"

#include "InfiniteTerrain.h"
#include "bezier.h"

using namespace glm;

ScreenQuad screenquad;

Skybox skybox;

Camera camera(vec3(90.0f, 10.0f, 90.0f));

int window_width = 1200;
int window_height = 800;

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
int mouse_input_mode = GLFW_CURSOR_DISABLED;

GLfloat last_frame = 0.0f;

mat4 quad_model_matrix;

float water_height;
InfiniteTerrain infiniteTerrain;
BezierCurve b;

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
        case GLFW_KEY_C:
            //camera.printCameraPosition();
            //cout << "texture height: " << infiniteTerrain.getCurrentPerlin().getTerrainHeight(camera.position_.x, camera.position_.z, infiniteTerrain.getCurrentChunkCoordinates()) << endl;
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

void Init() {
    vector<vec3> ps = {
        vec3(80.0, 6, 80.0f),
        vec3(80.0, 6, 100.0),
        vec3(100.0, 6, 100.0),
    };
    b.Init(1000, ps);

    // sets background color
    glClearColor(0, 0, 0 /*gray*/, 1.0 /*solid*/);

    skybox.Init();

    // enable depth test.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    water_height = 0.0f;

    quad_model_matrix = translate(mat4(1.0f), vec3(0.0f, 0.25f, 0.0f));


    infiniteTerrain.Init(window_width, window_height);

    //screenquad.Init(window_width, window_height, height_map_tex_id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display() {

    const float time = glfwGetTime();
    //cout << "CURRENT " << current << endl;
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
    infiniteTerrain.Draw(IDENTITY_MATRIX, view, projection, water_height);

    mat4 mirror_view = camera.getReversedViewMatrix(water_height);

    //waterReflexion.Bind();
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    skybox.Draw(scale, mirror_view, projection);
        /*
        for (size_t i = 0; i < terrains.size(); ++i) {
            terrains[i]->Draw(IDENTITY_MATRIX, mirror_view, projection, water_height);
        }
        */

        //terrain.Draw(IDENTITY_MATRIX, mirror_view, projection, water_height);
    //waterReflexion.Unbind();

    //skybox.Draw(cube_scale, view, projection);
    // water.Draw(trackball_matrix * quad_model_matrix, view, projection, water_height);

    glDisable(GL_DEPTH_TEST);
}

// Gets called when the windows/framebuffer is resized.
void buffer_resize_callback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
         << window_width << "x" << window_height << "." << endl;

    //waterReflexion_id = waterReflexion.Init(window_width, window_height);
    //water.Init(waterReflexion_id, water_wave_tex_id);

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
    int movement = 0;
    int lastMovement = 0;
    float velocity = 1.00f;

    vector<vec3> path = b.getPath();
    size_t cu = 0;
    // render loop
    while(!glfwWindowShouldClose(window)){

        //GLfloat terrain_height = infiniteTerrain.getCurrentPerlin().getTerrainHeight(camera.position_.x, camera.position_.z, infiniteTerrain.getCurrentChunkCoordinates());
        GLfloat terrain_height = 5.0f;

        glfwPollEvents();

        if(camera.keys_[GLFW_KEY_W]) movement = 1;
        else if(camera.keys_[GLFW_KEY_S]) movement = 2;
        else if(camera.keys_[GLFW_KEY_D]) movement = 3;
        else if(camera.keys_[GLFW_KEY_A]) movement = 4;

        if (movement != 0 && movement == lastMovement) {
            if (camera.keys_[GLFW_KEY_W] || camera.keys_[GLFW_KEY_S] || camera.keys_[GLFW_KEY_D] || camera.keys_[GLFW_KEY_A]) {
              //std::cout << "accelerate   " << velocity <<'\n';
              velocity = std::min(velocity + 0.002f, 0.05f);
              camera.accelerate(movement, velocity, terrain_height);
            }
            else {
              //std::cout << "decelerate   " << velocity <<'\n';
              velocity -= 0.001f;
              camera.accelerate(movement, velocity, terrain_height);
              movement = (velocity <= 0.0f)? 0 : movement;
            }
        }
        else {
            velocity = 0.005f;
            lastMovement = movement;
            // if(camera.keys_[GLFW_KEY_W]) movement = 1;
            // else if(camera.keys_[GLFW_KEY_S]) movement = 2;
            // else if(camera.keys_[GLFW_KEY_D]) movement = 3;
            // else if(camera.keys_[GLFW_KEY_A]) movement = 4;

            camera.update(velocity, terrain_height);
        }

        camera.position_ = path[cu++];
        if (cu == path.size()) cu = 0;

        infiniteTerrain.checkChunk(camera.get2dCoords());
        //camera.printCameraPosition();
        Display();
        glfwSwapBuffers(window);
    }

    infiniteTerrain.Cleanup();
    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
