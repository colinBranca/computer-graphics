#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>
#include "./../light.h"
#include <string>
#include <sstream>
#include <iostream>

struct Material {
        glm::vec3 ka = glm::vec3(0.18f, 0.1f, 0.1f);
        glm::vec3 kd = glm::vec3(0.9f, 0.5f, 0.5f);
        //glm::vec3 ks = glm::vec3(0.8f, 0.8f, 0.8f);
        float alpha = 60.0f;

        // pass material properties to the shaders
        void Setup(GLuint program_id) {
            glUseProgram(program_id);

            GLuint ka_id = glGetUniformLocation(program_id, "ka");
            GLuint kd_id = glGetUniformLocation(program_id, "kd");
            //GLuint ks_id = glGetUniformLocation(program_id, "ks");
            GLuint alpha_id = glGetUniformLocation(program_id, "alpha");

            glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
            glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
            //glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
            glUniform1f(alpha_id, alpha);
        }
};

class Terrain: public Light, Material  {

    public:
        GLuint vertex_array_id_;                // vertex array object
        GLuint vertex_buffer_object_position_;  // memory buffer for positions
        GLuint vertex_buffer_object_index_;     // memory buffer for indices
        static int program_id_;                     // GLSL shader program ID
        GLuint height_texture_id_;              // texture ID
        GLuint colormap_texture_id_;            // texture ID
        GLuint num_indices_;                    // number of vertices to render
        GLuint M_id_;                           // model matrix ID
        GLuint V_id_;                           // proj matrix ID
        GLuint P_id_;                           // view matrix ID
        float size_;
        float minX_;
        float minY_;

        GLuint ground_texture;
        GLuint grass_texture;
        GLuint snow_texture;
        GLuint sand_texture;
        int flattenCoord(int i, int j, int dim) {
                return dim * i + j;
        }
        GLuint loadTex(string filename) {
               int width, height, nb_component;
               unsigned char* image;

               GLuint id;
               // Grass
               glGenTextures(1, &id);
               glBindTexture(GL_TEXTURE_2D, id);
               stbi_set_flip_vertically_on_load(1);
               image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);
                if (image == nullptr) {
                    printf("Error on %s load\n", filename.c_str());
                    throw(string(stbi_failure_reason()));
                }
               glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
               stbi_image_free(image);
               glGenerateMipmap(GL_TEXTURE_2D);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

               glBindTexture(GL_TEXTURE_2D, 0);
               return id;
        }

    public:
        void Init(size_t grid_dim, GLuint elevation_texture_id, float size = 20.0f, float minX = -10.0f, float minY = -10.0f) {
            // compile the shaders.
            if (program_id_ < 0) {
                program_id_ = icg_helper::LoadShaders("terrain_vshader.glsl",
                                                      "terrain_fshader.glsl");
            }
            this->height_texture_id_ = elevation_texture_id;
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            size_ = size;
            minX_ = minX;
            minY_ = minY;

            glUseProgram(program_id_);

            // vertex one vertex array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates and indices
            {

                std::vector<GLfloat> vertices;

                // Generate vertices coordinates
                float factor = size / ((float) grid_dim);
                for (size_t row = 0; row < grid_dim; ++row) {
                        float yCoord = factor * row + minY;
                        for (size_t col = 0; col < grid_dim; ++col) {
                                vertices.push_back(factor * col + minX);
                                vertices.push_back(yCoord);
                        }
                }

                // Associate indices to vertices.
                // Please refer to readme.txt for explication
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

            // Received texture (elevation)
            {
                    glBindTexture(GL_TEXTURE_2D, elevation_texture_id);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                    glUniform1i(glGetUniformLocation(program_id_, "height_tex"), 0);
                    glBindTexture(GL_TEXTURE_2D, 0);
            }
            // other uniforms
            M_id_ = glGetUniformLocation(program_id_, "M");
            V_id_ = glGetUniformLocation(program_id_, "V");
            P_id_ = glGetUniformLocation(program_id_, "P");

           // Import textures
            {
               grass_texture = loadTex("grass1.png");
               glUniform1i(glGetUniformLocation(program_id_, "grass_tex"), 1);

               ground_texture = loadTex("ground.jpg");
               glUniform1i(glGetUniformLocation(program_id_, "ground_tex"), 2);

               snow_texture = loadTex("snow.jpg");
               glUniform1i(glGetUniformLocation(program_id_, "snow_tex"), 3);

               sand_texture = loadTex("sand.jpg");
               glUniform1i(glGetUniformLocation(program_id_, "sand_tex"), 4);
            }

            // to avoid the current object being polluted
            glBindTexture(GL_TEXTURE_2D, 0);
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

            glDeleteTextures(1, &ground_texture);
            glDeleteTextures(1, &grass_texture);
            glDeleteTextures(1, &snow_texture);
            glDeleteTextures(1, &sand_texture);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX,
                  float water_height = -10.0f) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            Material::Setup(program_id_);
            Light::Setup(program_id_);

            // setup MVP
            glUniformMatrix4fv(M_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(model));
            glUniformMatrix4fv(V_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(view));
            glUniformMatrix4fv(P_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(projection));

            //setup water_height
            GLuint WaterHeight_id = glGetUniformLocation(program_id_, "water_height");
            glUniform1f(WaterHeight_id, water_height);

            // Dimensions
            glUniform1f(glGetUniformLocation(program_id_, "size"), size_);
            glUniform1f(glGetUniformLocation(program_id_, "minX"), minX_);
            glUniform1f(glGetUniformLocation(program_id_, "minY"), minY_);

            // height
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, height_texture_id_);

           //grass texture
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, grass_texture);
            //ground texture
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, ground_texture);
            //snow texture
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, snow_texture);
            //sand texture
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, sand_texture);

            // draw
            glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);
            glBindVertexArray(0);
        }
};

int Terrain::program_id_ = -1;