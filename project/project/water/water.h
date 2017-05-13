#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Water
{
private:
    GLuint water_vao_;
    GLuint water_vbo_;
    GLuint texture_wave_id_;         // PerlinNoise for waves
    GLuint program_id_;

public:
    void Init(GLuint tex_wave) {
        // compile the shaders
        program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
                                              "water_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        const GLfloat water_vertices[] = {
            -1.0f, 0.0f, -1.0f,
            -1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, -1.0f,
            1.0f, 0.0f, -1.0f,
            -1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
        };

        glGenVertexArrays(1, &water_vao_);
        glBindVertexArray(water_vao_);
        glGenBuffers(1, &water_vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, water_vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(water_vertices), &water_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        // Received texture (waves)
        texture_wave_id_ = tex_wave;
        glBindTexture(GL_TEXTURE_2D, texture_wave_id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glUniform1i(glGetUniformLocation(program_id_, "wave_tex"), 3);
        glBindTexture(GL_TEXTURE_2D, 3);

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &water_vbo_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &water_vao_);
    }

    void Draw(const glm::mat4 &model,
              const glm::mat4 &view,
              const glm::mat4 &projection,
              const glm::vec3 &camera_position,
              GLuint skybox_cubemap_id,
              float water_height = 0.0f,
              float time = 1.0f) {
        glUseProgram(program_id_);

        // pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);

        // setup MVP
        GLuint model_id = glGetUniformLocation(program_id_, "model");
        glUniformMatrix4fv(model_id, 1, GL_FALSE, value_ptr(model));
        GLuint view_id = glGetUniformLocation(program_id_, "view");
        glUniformMatrix4fv(view_id, 1, GL_FALSE, value_ptr(view));
        GLuint projection_id = glGetUniformLocation(program_id_, "projection");
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, value_ptr(projection));

        GLuint position_id = glGetUniformLocation(program_id_, "camera_position");
        glUniform3f(position_id, camera_position.x, camera_position.y, camera_position.z);

        //setup water_height
        GLuint water_height_id = glGetUniformLocation(program_id_, "water_height");
        glUniform1f(water_height_id, water_height);

        glBindVertexArray(water_vao_);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_wave_id_);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap_id);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
