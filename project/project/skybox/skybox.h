#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"

class Skybox {

    private:
        GLuint skybox_vbo_;
        GLuint skybox_vao_;
        GLuint cubemap_texture_;
        GLuint program_id_;                     // GLSL shader program ID
        GLuint MVP_id_;                         // model, view, proj matrix ID

        GLuint loadCubemap() {
            GLuint textureID;
            glGenTextures(1, &textureID);

            int width, height, nb_component;
            unsigned char* image;

            vector<string> faces;
            faces.push_back("right.jpg");
            faces.push_back("left.jpg");
            faces.push_back("top.jpg");
            faces.push_back("bottom.jpg");
            faces.push_back("back.jpg");
            faces.push_back("front.jpg");

            stbi_set_flip_vertically_on_load(1);

            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            for(GLuint i = 0; i < faces.size(); i++) {
                image = stbi_load(faces[i].c_str(), &width, &height, &nb_component, 0);
                if (image == nullptr) {
                    throw(string(stbi_failure_reason()));
                }
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                stbi_image_free(image);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            return textureID;
        }

    public:
        void Init() {
            // compile the shaders.
            program_id_ = icg_helper::LoadShaders("skybox_vshader.glsl",
                                                  "skybox_fshader.glsl");
            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            const GLfloat skyboxVertices[] = {
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
            };

            glGenVertexArrays(1, &skybox_vao_);
            glGenBuffers(1, &skybox_vbo_);
            glBindVertexArray(skybox_vao_);
            glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

            cubemap_texture_ = loadCubemap();

            // other uniforms
            MVP_id_ = glGetUniformLocation(program_id_, "MVP");

            // to avoid the current object being polluted
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Cleanup() {
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &skybox_vbo_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &skybox_vao_);
        }

        void Draw(const glm::mat4 &model = IDENTITY_MATRIX,
                  const glm::mat4 &view = IDENTITY_MATRIX,
                  const glm::mat4 &projection = IDENTITY_MATRIX,
                  int reflexion = 0) {
            glUseProgram(program_id_);

            //glDepthMask(GL_FALSE);

            // setup MVP
            glm::mat4 MVP = projection*view*model;
            glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

            //setup water_height
            GLuint Reflexion_id = glGetUniformLocation(program_id_, "reflexion");
            glUniform1i(Reflexion_id, reflexion);

            glBindVertexArray(skybox_vao_);
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(program_id_, "skybox"), 0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture_);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            //glDepthMask(GL_TRUE);

            glUseProgram(0);
        }
};
