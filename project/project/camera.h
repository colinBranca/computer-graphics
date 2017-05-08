#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    PITCH_UP,
    PITCH_DOWN
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVTY = 0.1f;
const GLfloat ZOOM = 45.0f;

class Camera
{
public:
    // Camera Attributes
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 world_up_;
    // Eular Angles
    GLfloat yaw_;
    GLfloat pitch_;
    // Camera options
    GLfloat movement_speed_;
    GLfloat mouse_sensitivity_;
    GLfloat zoom_;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = YAW, GLfloat pitch = PITCH)
        : front_(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed_(SPEED), mouse_sensitivity_(SENSITIVTY), zoom_(ZOOM)
    {
        this->position_ = position;
        this->world_up_ = up;
        this->yaw_ = yaw;
        this->pitch_ = pitch;
        this->updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->position_, this->position_ + this->front_, this->up_);
    }

    void processKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->movement_speed_ * deltaTime;
        if (direction == FORWARD) {
            this->position_ += this->front_ * velocity;
        }
        if (direction == BACKWARD) {
            this->position_ -= this->front_ * velocity;
        }
        if (direction == LEFT) {
            this->position_ -= this->right_ * velocity;
        }
        if (direction == RIGHT) {
            this->position_ += this->right_ * velocity;
        }
        if (direction == PITCH_UP) {
            processMouseMovement(0.0f, 10.0f);
        }
        if (direction == PITCH_DOWN) {
            processMouseMovement(0.0f, -10.0f);
        }
    }

    void processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->mouse_sensitivity_;
        yoffset *= this->mouse_sensitivity_;

        this->yaw_ += xoffset;
        this->pitch_ += yoffset;

        if (constrainPitch) {
            if (this->pitch_ > 89.0f) {
                this->pitch_ = 89.0f;
            }
            if (this->pitch_ < -89.0f) {
                this->pitch_ = -89.0f;
            }
        }

        this->updateCameraVectors();
    }

    void processMouseScroll(GLfloat yoffset)
    {
        if (this->zoom_ >= 1.0f && this->zoom_ <= 45.0f) {
            this->zoom_ -= yoffset;
        }
        if (this->zoom_ <= 1.0f) {
            this->zoom_ = 1.0f;
        }
        if (this->zoom_ >= 45.0f) {
            this->zoom_ = 45.0f;
        }
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
        front.y = sin(glm::radians(this->pitch_));
        front.z = sin(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
        this->front_ = glm::normalize(front);
        this->right_ = glm::normalize(glm::cross(this->front_, this->world_up_));
        this->up_ = glm::normalize(glm::cross(this->right_, this->front_));
    }
};
