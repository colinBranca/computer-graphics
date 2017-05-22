#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Water {

    public:
        GLuint vertex_array_id_;        // vertex array object
        static int program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID
        GLuint texture_mirror_id_;      // texture mirror ID
        GLuint texture_wave_id_;         // PerlinNoise for waves
        GLuint vertex_buffer_object_index_;
        int flattenCoord(int i, int j, int dim) {
                return dim * i + j;
        }
        GLuint num_indices_;
        float size_;
        float minX_;
        float minY_;



    public:
        void Init(GLuint tex_mirror = -1, GLuint tex_wave = -1, size_t grid_dim = 1024, float size = 20.0f, float minX = -10.0f, float minY = -10.0f) {
            // compile the shaders
            if (program_id_ < 0) {
                program_id_ = icg_helper::LoadShaders("water_vshader.glsl",
                                                      "water_fshader.glsl");
            }
            size_ = size;
            minX_ = minX;
            minY_ = minY;

            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                std::vector<GLfloat> vertices;

                // Generate vertices coordinates
                float factor = size_ / ((float) grid_dim);
                for (size_t row = 0; row < grid_dim; ++row) {
                        float yCoord = factor * row + minY;
                        for (size_t col = 0; col < grid_dim; ++col) {
                                vertices.push_back(factor * col + minX);
                                vertices.push_back(yCoord);
                        }
                }
                std::vector<GLuint> indices;
                for (size_t row = 0; row < grid_dim -1; ++row) {
                        for (size_t col = 0; col < grid_dim -1; ++col) {
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
                this->num_indices_ = indices.size();

                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                             &vertices[0], GL_STATIC_DRAW);
                //
                // vertex indices
                glGenBuffers(1, &vertex_buffer_object_index_);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                             &indices[0], GL_STATIC_DRAW);

                // attribute
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "position");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 2, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }
            // Received texture (waves)
            {
                    texture_wave_id_ = (tex_wave==-1)? texture_id_ : tex_wave;
                    glBindTexture(GL_TEXTURE_2D, texture_wave_id_);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                    glUniform1i(glGetUniformLocation(program_id_, "wave_tex"), 3);
                    glBindTexture(GL_TEXTURE_2D, 3);
            }

            {
                // // load texture
                // int width;
                // int height;
                // int nb_component;
                // string filename = "floor_texture.tga";
                // // set stb_image to have the same coordinates as OpenGL
                // stbi_set_flip_vertically_on_load(1);
                // unsigned char* image = stbi_load(filename.c_str(), &width,
                //                                  &height, &nb_component, 0);
                //
                // if(image == nullptr) {
                //     throw(string("Failed to load texture"));
                // }
                //
                // glGenTextures(1, &texture_id_);
                // glBindTexture(GL_TEXTURE_2D, texture_id_);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                //
                // if(nb_component == 3) {
                //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                //                  GL_RGB, GL_UNSIGNED_BYTE, image);
                // } else if(nb_component == 4) {
                //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                //                  GL_RGBA, GL_UNSIGNED_BYTE, image);
                // }


                texture_mirror_id_ = (tex_mirror==-1)? texture_id_ : tex_mirror;

                // texture uniforms
                // GLuint tex_id = glGetUniformLocation(program_id_, "tex");
                // glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
                GLuint tex_mirror_id = glGetUniformLocation(program_id_, "tex_mirror");
                glUniform1i(tex_mirror_id, 2 /*GL_TEXTURE2*/);

                // cleanup
                //glBindTexture(GL_TEXTURE_2D, 0);
                //stbi_image_free(image);
            }

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id_);
            // glDeleteTextures(1, &texture_id_);
            glDeleteTextures(1, &texture_mirror_id_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX,
                  float water_height = 0.0f, float time = 1.0f) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            // setup MVP
            glm::mat4 MVP = projection*view*model;

            // // bind textures
            // glActiveTexture(GL_TEXTURE0);
            // glBindTexture(GL_TEXTURE_2D, texture_id_);
            //
            // pass the current time stamp to the shader.
            glUniform1f(glGetUniformLocation(program_id_, "time"), time);
            // bind textures
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture_mirror_id_);

            // bind textures
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, texture_wave_id_);

            // setup MVP
            GLuint MVP_id = glGetUniformLocation(program_id_, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, value_ptr(MVP));

            //setup water_height
            GLuint WaterHeight_id = glGetUniformLocation(program_id_, "water_height");
            glUniform1f(WaterHeight_id, water_height);

            glUniform1f(glGetUniformLocation(program_id_, "size"), size_);
            glUniform1f(glGetUniformLocation(program_id_, "minX"), minX_);
            glUniform1f(glGetUniformLocation(program_id_, "minY"), minY_);


            // draw
            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};

int Water::program_id_ = -1;