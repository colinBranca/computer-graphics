#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Water
{
private:
    GLuint water_vao_;
    GLuint water_vbo_;
    GLuint water_vbo_indices_;
    GLuint num_indices_;
    GLuint texture_wave_id_; // PerlinNoise for waves
    GLuint program_id_;

    size_t flattenCoordinates(size_t row, size_t col, size_t dim) {
        return dim * row + col;
    }

public:
    void Init(GLuint texture_wave_id, size_t grid_dim = 1024) {
        // compile the shaders
        program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
                                              "water_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        std::vector<GLfloat> water_vertices;
        float cell_size = 10.0f / grid_dim;
        for (size_t row = 0; row < grid_dim; ++row) {
            float z_coord = cell_size * row - 5.0f;
            for (size_t col = 0; col < grid_dim; ++col) {
                water_vertices.push_back(cell_size * col - 5.0f);
                water_vertices.push_back(0.0f);
                water_vertices.push_back(z_coord);
            }
        }

        std::vector<GLuint> indices;
        for (size_t row = 0; row < grid_dim - 1; ++row) {
            for (size_t col = 0; col < grid_dim - 1; ++col) {
                indices.push_back(flattenCoordinates(row, col, grid_dim));
                indices.push_back(flattenCoordinates(row, col + 1, grid_dim));
                indices.push_back(flattenCoordinates(row + 1, col, grid_dim));

                indices.push_back(flattenCoordinates(row, col + 1, grid_dim));
                indices.push_back(flattenCoordinates(row + 1, col, grid_dim));
                indices.push_back(flattenCoordinates(row + 1, col + 1, grid_dim));
            }
        }

        num_indices_ = indices.size();

        glUniform1f(glGetUniformLocation(program_id_, "cell_size"), cell_size);

        glGenVertexArrays(1, &water_vao_);
        glBindVertexArray(water_vao_);
        glGenBuffers(1, &water_vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, water_vbo_);
        glBufferData(GL_ARRAY_BUFFER, water_vertices.size() * sizeof(GLfloat), &water_vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &water_vbo_indices_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, water_vbo_indices_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        GLuint vertex_point_id = glGetAttribLocation(program_id_, "position");
        glEnableVertexAttribArray(vertex_point_id);
        glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        {
            glBindTexture(GL_TEXTURE_2D, texture_wave_id_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glUniform1i(glGetUniformLocation(program_id_, "tex_wave"), 3);
            glBindTexture(GL_TEXTURE_2D, 3);
        }

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &water_vbo_);
        glDeleteBuffers(1, &water_vbo_indices_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &water_vao_);
        glDeleteTextures(1, &texture_wave_id_);
    }

    void Draw(const glm::mat4 &model,
              const glm::mat4 &view,
              const glm::mat4 &projection,
              const glm::vec3 &camera_position,
              GLuint skybox_cubemap_id,
              GLuint terrain_texture_id,
              float water_height = 0.0f,
              float time = 1.0f) {
        glUseProgram(program_id_);
        glBindVertexArray(water_vao_);

        // pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);

        // Setup MVP
        GLuint model_id = glGetUniformLocation(program_id_, "model");
        glUniformMatrix4fv(model_id, 1, GL_FALSE, value_ptr(model));
        GLuint view_id = glGetUniformLocation(program_id_, "view");
        glUniformMatrix4fv(view_id, 1, GL_FALSE, value_ptr(view));
        GLuint projection_id = glGetUniformLocation(program_id_, "projection");
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, value_ptr(projection));

        // Setup camera position
        GLuint position_id = glGetUniformLocation(program_id_, "camera_position");
        glUniform3f(position_id, camera_position.x, camera_position.y, camera_position.z);

        // Setup water_height
        glUniform1f(glGetUniformLocation(program_id_, "water_height"), water_height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap_id);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_wave_id_);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, terrain_texture_id);
        //glUniform1i(glGetUniformLocation(program_id_, "terrain"), 5);

        glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
