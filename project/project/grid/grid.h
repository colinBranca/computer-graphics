#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>


class Grid  {

    private:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        GLuint program_id_;                     // GLSL shader program ID
        GLuint height_texture_id_;              // texture ID
        GLuint colormap_texture_id_;            // texture ID
        GLuint num_indices_;                    // number of vertices to render
        GLuint MVP_id_;                         // model, view, proj matrix ID
        int flattenCoord(int i, int j, int dim) {
                return dim * i + j;
        }

    public:
        void Init(size_t grid_dim, GLuint texture_id) {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("grid_vshader.glsl",
                                                  "grid_fshader.glsl");
            this->height_texture_id_ = texture_id;
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {

                std::vector<GLfloat> vertices;

                // Generate vertices coordinates
                float factor = 2.0f / ((float) grid_dim);
                for (int row = 0; row < grid_dim; ++row) {
                        float yCoord = factor * row -1.0f;
                        for (int col = 0; col < grid_dim; ++col) {
                                vertices.push_back(factor * col - 1.0f);
                                vertices.push_back(yCoord);
                        }
                }

                // Associate indices to vertices.
                // Please refer to readme.txt for explication
                std::vector<GLuint> indices;
                for (int row = 0; row < grid_dim -1; ++row) {
                        for (int col = 0; col < grid_dim -1; ++col) {
                                // Upper triangle
                                indices.push_back(flattenCoord(row, col, grid_dim));
                                indices.push_back(flattenCoord(row, col + 1, grid_dim));
                                indices.push_back(flattenCoord(row + 1, col, grid_dim));
                                // Lower triangle
                                indices.push_back(flattenCoord(row, col + 1, grid_dim));
                                indices.push_back(flattenCoord(row + 1, col + 1, grid_dim));
                                indices.push_back(flattenCoord(row + 1, col, grid_dim));
                        }
                }

                num_indices_ = indices.size();

                // position buffer
                glGenBuffers(1, &vertex_buffer_object_position_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                             &vertices[0], GL_STATIC_DRAW);

                // vertex indices
                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                             &indices[0], GL_STATIC_DRAW);

                // position shader attribute
                GLuint loc_position = glGetAttribLocation(program_id_, "position");
                glEnableVertexAttribArray(loc_position);
                glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

           glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            GLuint h_tex_id = glGetUniformLocation(program_id_, "tex");
            glUniform1i(h_tex_id, 0);
            // other uniforms
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");

            // create 1D texture (colormap)
            {
                const int ColormapSize=4;
                GLfloat colors[3*ColormapSize] = {/*dark braun*/ 184.0/255.0, 134.0/255.0, 11.0/255.0,
                                                /*light braun*/ 218.0/255.0, 165.0/255.0, 32.0/255.0,
                                               /*green*/ 34.0/255.0, 139.0/255.0, 34.0/255.0,
                                               /*dark snow*/ 238.0/255.0, 233.0/255.0, 233.0/255.0 };
                glGenTextures(1, &colormap_texture_id_);
                glBindTexture(GL_TEXTURE_1D, colormap_texture_id_);
                glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, ColormapSize, 0, GL_RGB, GL_FLOAT, colors);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                GLuint c_tex_id = glGetUniformLocation(program_id_, "colormap");
                glUniform1i(c_tex_id, 1 /*GL_TEXTURE0*/);
                // check_error_gl();
            }

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_position_);
            glDeleteBuffers(1, &vertex_buffer_object_index_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            glDeleteProgram(program_id_);
            glDeleteTextures(1, &height_texture_id_);
            glDeleteTextures(1, &colormap_texture_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // setup MVP
            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

            // bind textures
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_1D, colormap_texture_id_);

            // draw
            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

        }
};
