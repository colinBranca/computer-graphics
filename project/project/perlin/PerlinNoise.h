#pragma once
#include "icg_helper.h"

class PerlinNoise {
    private:
        int width_, height_;
        GLuint vertex_buffer_object_;
        GLuint vertex_array_id;
        int program_id_;
        int p[512];
        int octaves;

        void generateP() {
                int permutation[] = { 151,160,137,91,90,15,
                   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
                   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
                   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
                   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
                   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
                   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
                   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
                   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
                   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
                   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
                   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
                   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
                   };
                for (int i=0; i < 256 ; i++) {
                        int v = permutation[i];
                        p[256+i] = v;
                        p[i] = v;
                }
        }

    public:
        void Init(int width, int height, int octaves=6) {
            // set screenquad size
            this->width_ = width;
            this->height_ = height;
            this->octaves = octaves;
            generateP();

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


            const GLint vertex_texture_coordinates[] = { /*V1*/ 0, 0,
                                                           /*V2*/ width_, 0,
                                                           /*V3*/ 0, height_,
                                                           /*V4*/ width_, height_};

            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                             vertex_texture_coordinates, GL_STATIC_DRAW);

            // attribute
            GLuint vtexcoord_id  = glGetAttribLocation(program_id_, "vtexcoord");
            glEnableVertexAttribArray(vtexcoord_id);
            glVertexAttribPointer(vtexcoord_id, 2, GL_INT, DONT_NORMALIZE,
                                  ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            glUniform1iv(glGetUniformLocation(program_id_, "p"), 512, p);

            glUseProgram(0);
        }

        void Draw() {
                glUseProgram(program_id_);
                glUniform1i(glGetUniformLocation(program_id_, "width"),
                        width_);
                glUniform1i(glGetUniformLocation(program_id_, "height"),
                        height_);
                glUniform1i(glGetUniformLocation(program_id_, "octaves"),
                        octaves);
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
