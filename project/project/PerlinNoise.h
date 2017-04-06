#pragma once
#include "icg_helper.h"

class PerlinNoise {
    private:
        float width_, height_;
        GLuint vertex_buffer_object_;
        GLuint vertex_array_id;
        int program_id_;
    public:
        void Init(float width, float height) {
            // set screenquad size
            this->width_ = width;
            this->height_ = height;

            program_id_ = icg_helper::LoadShaders("perlin_vshader.glsl",
                                                  "perlin_fshader.glsl");

            if (!program_id_) exit(EXIT_FAILURE);
            glUseProgram(program_id_);
            glGenVertexArrays(1, &vertex_array_id);
            glBindVertexArray(vertex_array_id);
            const GLfloat vertex_point[] = {  -1.0f, -1.0f, 0.0f,
                                              +1.0f, -1.0f, 0.0f,
                                              -1.0f, +1.0f, 0.0f,
                                              +1.0f, +1.0f, 0.0f};
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                         vertex_point, GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);


            const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
                                                           /*V2*/ 1.0f, 0.0f,
                                                           /*V3*/ 0.0f, 1.0f,
                                                           /*V4*/ 1.0f, 1.0f};

            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                             vertex_texture_coordinates, GL_STATIC_DRAW);

            // attribute
            GLuint vtexcoord_id  = glGetAttribLocation(program_id_, "vtexcoord");
            glEnableVertexAttribArray(vtexcoord_id);
            glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            glBindVertexArray(0);
            glUseProgram(0);
        }

        void Draw() {
                glUseProgram(program_id_);
                glBindVertexArray(vertex_array_id);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                glBindVertexArray(0);
                glUseProgram(0);
        }

        void Cleanup() {
            //glEnableVertexAttribArray(vertex_point_id);
            glBindVertexArray(0);
            glUseProgram(0);
            glDeleteBuffers(1, &vertex_buffer_object_);
            glDeleteProgram(program_id_);
            glDeleteVertexArrays(1, &vertex_array_id);
        }

};
