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

#include <vector>
#include <map>

using namespace glm;

vector<Terrain*> terrains;
vector<Water*> waters;
vector<PerlinNoise*> terrain_perlins;
vector<PerlinNoise*> water_perlins;

int current = 0;
int seedX = 1;
int seedY = 1;
pair<int, int> gridCoords = {0, 0};

map<pair<int, int>, int> chunks;

float chunk_size = 20.0f;
int grid_resolution = 512;


ScreenQuad screenquad;

Skybox skybox;
FrameBuffer waterReflexion;
GLuint waterReflexion_id;
GLuint water_wave_tex_id;
Camera camera(vec3(-5.0f, 5.0f, 5.0f));

int window_width = 1200;
int window_height = 800;

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

void Init() {
    // sets background color
    glClearColor(0, 0, 0 /*gray*/, 1.0 /*solid*/);

    skybox.Init();

    // enable depth test.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    water_height = 0.0f;

    quad_model_matrix = translate(mat4(1.0f), vec3(0.0f, 0.25f, 0.0f));

    water_perlins.push_back(new PerlinNoise());
    water_perlins[current]->Init(grid_resolution, grid_resolution, 1, 1.0);
    water_perlins[current]->Compute();

    terrain_perlins.push_back(new PerlinNoise());
    terrain_perlins[current]->Init(grid_resolution, grid_resolution, 7, 3.5f, 1 / 400.0f, 1 / 400.0f);
    terrain_perlins[current]->Compute();

    waterReflexion_id = waterReflexion.Init(window_width, window_height);

    waters.push_back(new Water());
    waters[current]->Init(waterReflexion_id, water_wave_tex_id);

    chunks[{0, 0}] = 0;


    //screenquad.Init(window_width, window_height, height_map_tex_id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    terrains.push_back(new Terrain());
    terrains[0]->Init(512, terrain_perlins[current]->getHeightTexId());

}

enum ChunkRelativePosition {C_LEFT, C_RIGHT, C_UP, C_DOWN, C_STILL};

pair<int, int> getCoefs(ChunkRelativePosition pos) {
    switch (pos) {
        case C_LEFT:
            return {-1, 0};
        case C_RIGHT:
            return {1, 0};
        case C_UP:
            return {0, 1};
        case C_DOWN:
            return {0, -1};
        case C_STILL:
            return {0, 0};
    }
}

int createChunk(ChunkRelativePosition pos) {
    terrain_perlins.push_back(new PerlinNoise());
    terrain_perlins.back()->Init(grid_resolution, grid_resolution, 7, 3.5f, 1 / 400.0f, 1 / 400.0f);
    terrain_perlins.back()->Compute(seedX, seedY);
    water_perlins.push_back(new PerlinNoise());
    water_perlins.back()->Init(grid_resolution, grid_resolution, 1, 1.0);
    water_perlins.back()->Compute();

    terrains.push_back(new Terrain());
    waters.push_back(new Water());

    pair<int, int> coefs = getCoefs(pos);

    terrains.back()->Init(grid_resolution, terrain_perlins.back()->getHeightTexId(), 20.f, terrains[current]->minX_ + coefs.first * chunk_size, terrains[current]->minY_ + coefs.second * chunk_size);
    waters.back()->Init(waterReflexion_id, water_wave_tex_id, grid_resolution, 20.0f, waters[current]->minX_ + coefs.first * chunk_size, waters[current]->minY_ + coefs.second * chunk_size);

    return terrains.size() - 1;
}

ChunkRelativePosition getChunkCoordinates() {
    if (camera.position_.x > terrains[current]->minX_ + chunk_size) {
        return C_RIGHT;
    }
    else if (camera.position_.x < terrains[current]->minX_) {
        return C_LEFT;
    }
    else if (camera.position_.z > terrains[current]->minY_ + chunk_size) {
        return C_UP;
    }
    else if (camera.position_.z < terrains[current]->minY_){
        return C_DOWN;
    } 
    else {
        return C_STILL;
    }
}

pair<int, int> operator+(pair<int, int> a, pair<int, int> b) {
    return {a.first + b.first, a.second + b.second};
}

void checkChunk() {
    ChunkRelativePosition position = getChunkCoordinates();
    gridCoords = getCoefs(position) + gridCoords;
    if (chunks.count(gridCoords) < 1) {
        current = createChunk(position);
        chunks[gridCoords] = current;
    } else {
        current = chunks[gridCoords];
    }
}

void Display() {

    checkChunk();
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

    for (size_t i = 0; i < terrains.size(); ++i) {
        waters[i]->Draw(IDENTITY_MATRIX, view, projection, water_height, time);
        terrains[i]->Draw(IDENTITY_MATRIX, view, projection, water_height);
    }
    //water.Draw(IDENTITY_MATRIX, view, projection, water_height, time);
    //terrain.Draw(IDENTITY_MATRIX, view, projection, water_height);

    // mirror the camera position

    mat4 mirror_view = camera.getReversedViewMatrix(water_height);

    waterReflexion.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.Draw(scale, mirror_view, projection);
        /*
        for (size_t i = 0; i < terrains.size(); ++i) {
            terrains[i]->Draw(IDENTITY_MATRIX, mirror_view, projection, water_height);
        }
        */
 
        //terrain.Draw(IDENTITY_MATRIX, mirror_view, projection, water_height);
    waterReflexion.Unbind();

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
    float velocity = 0.05f;

    // render loop
    while(!glfwWindowShouldClose(window)){

        //GLfloat terrain_height = terrain_perlin.getTerrainHeight(camera.position_.x, camera.position_.z);
        GLfloat terrain_height = terrain_perlins[current]->getTerrainHeight(camera.position_.x, camera.position_.z);

        glfwPollEvents();
        
        if (movement != 0) {
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
            if(camera.keys_[GLFW_KEY_W]) movement = 1;
            else if(camera.keys_[GLFW_KEY_S]) movement = 2;
            else if(camera.keys_[GLFW_KEY_D]) movement = 3;
            else if(camera.keys_[GLFW_KEY_A]) movement = 4;

            camera.update(velocity, terrain_height);
        }

        Display();
        glfwSwapBuffers(window);
    }

    for (size_t i = 0; i < terrains.size(); ++i) {
        waters[i]->Cleanup();
        delete waters[i];
        terrains[i]->Cleanup();
        delete terrains[i];
        terrain_perlins[i]->Cleanup();
        delete terrain_perlins[i];
        water_perlins[i]->Cleanup();
        delete water_perlins[i];
    }
    /*
    terrain_perlin.Cleanup();
    water_perlin.Cleanup();
    terrain.Cleanup();
    skybox.Cleanup();
    water.Cleanup();
    */

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
