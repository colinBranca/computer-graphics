#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "perlin/PerlinNoise.h"
#include "terrain/terrain.h"
#include "water/water.h"
#include "framebuffer.h"

#include <vector>
#include <map>

enum ChunkRelativePosition {C_LEFT, C_RIGHT, C_UP, C_DOWN, C_STILL};

pair<int, int> operator+(pair<int, int> a, pair<int, int> b) {
    return {a.first + b.first, a.second + b.second};
}

class InfiniteTerrain {

private:
    PerlinNoise terrain_perlin;
    PerlinNoise water_perlin;

    pair<int, int> gridCoords;
    map<pair<int, int>, pair<Terrain*, Water*>> chunks;

    float chunk_size = 20.0f;
    int grid_resolution = 200;

    GLuint skybox;

    float frequency = 400.0f;
    float amplitude = 7.0f;
    int octaves = 7;

    FrameBuffer waterReflexion;
    GLuint waterReflexion_id;
    GLuint water_wave_tex_id;

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

    void createChunk(ChunkRelativePosition pos, pair<int, int> previous) {
        pair<int, int> coefs = getCoefs(pos);

        if (chunks.count(coefs + previous) > 0) {
            return;
        }

        Terrain* t = new Terrain();
        Water* w = new Water();

        t->Init(grid_resolution,
                terrain_perlin.getHeightTexId(),
                chunk_size,
                chunks[previous].first->minX_ + coefs.first * chunk_size,
                chunks[previous].first->minY_ + coefs.second * chunk_size);
        w->Init(skybox,
                waterReflexion_id,
                water_wave_tex_id,
                grid_resolution,
                chunk_size,
                chunks[previous].second->minX_ + coefs.first * chunk_size,
                chunks[previous].second->minY_ + coefs.second * chunk_size);

        chunks[coefs + previous] = {t, w};
    }

    ChunkRelativePosition getChunkRelativePosition(glm::vec2 coords2d, pair<int, int> chunk) {
        if (coords2d.x > chunks[chunk].first->minX_ + chunk_size) {
            return C_RIGHT;
        } else if (coords2d.x < chunks[chunk].first->minX_) {
            return C_LEFT;
        } else if (coords2d.y > chunks[chunk].first->minY_ + chunk_size) {
            return C_UP;
        } else if (coords2d.y < chunks[chunk].first->minY_) {
            return C_DOWN;
        } else {
            return C_STILL;
        }
    }

public:
    void checkChunk(glm::vec2 pos) {
        ChunkRelativePosition relPos = getChunkRelativePosition(pos, gridCoords);
        pair<int, int> newCoords = getCoefs(relPos) + gridCoords;

        if (newCoords == gridCoords) return;

        createChunk(relPos, newCoords);
        switch (relPos) {
        case C_RIGHT:
            createChunk(C_UP, newCoords + getCoefs(C_RIGHT));
            createChunk(C_DOWN, newCoords + getCoefs(C_RIGHT));
        case C_LEFT:
            createChunk(C_UP, newCoords + getCoefs(C_LEFT));
            createChunk(C_DOWN, newCoords + getCoefs(C_LEFT));
        case C_UP:
            createChunk(C_LEFT, newCoords + getCoefs(C_UP));
            createChunk(C_RIGHT, newCoords + getCoefs(C_UP));
        case C_DOWN:
            createChunk(C_LEFT, newCoords + getCoefs(C_DOWN));
            createChunk(C_RIGHT, newCoords + getCoefs(C_DOWN));
        }
        gridCoords = newCoords;
    }

    void changePerlin(int param, int window_width, int window_height, glm::mat4 v, glm::mat4 p, glm::mat4 mir, float water_height) {
        switch (param) {
        case 0:
            amplitude += 1.0f;
            break;
        case 1:
            amplitude -= 1.0f;
            break;
        case 2:
            frequency += 10.0f;
            break;
        case 3:
            frequency -= 10.0f;
            break;
        case 4:
            octaves += 1;
            break;
        case 5:
            octaves -= 1;
            break;
        }
        GLuint newTex = terrain_perlin.Init(grid_resolution * 11, grid_resolution * 11, octaves, amplitude, 1/frequency, 1/frequency);
        terrain_perlin.Compute();

        for (pair<pair<int, int>, pair<Terrain*, Water*>> chunk : chunks) {
            chunk.second.first->height_texture_id_ = newTex;
        }
        //Init(window_width, window_height);
        Draw(IDENTITY_MATRIX, v, p, mir, glm::vec3(0,0,0), water_height);
    }

    void Init(int window_width, int window_height, GLuint skybox_tex) {
        skybox = skybox_tex;
        waterReflexion_id = waterReflexion.Init(window_width, window_height);

        water_wave_tex_id = water_perlin.Init(grid_resolution * 11, grid_resolution * 11, 1, 1.0);
        water_perlin.Compute();

        terrain_perlin.Init(grid_resolution * 11, grid_resolution * 11, octaves, amplitude, 1/frequency, 1/frequency);
        terrain_perlin.Compute();

        Terrain* t = new Terrain();
        Water* w = new Water();
        // FIRST
        w->Init(skybox, waterReflexion_id, water_wave_tex_id, grid_resolution, chunk_size, 100, 100);
        t->Init(grid_resolution, terrain_perlin.getHeightTexId(), chunk_size, 100, 100);//-chunk_size / 2.0, -chunk_size / 2.0);

        chunks[ {5, 5}] = {t, w};
        gridCoords = {5, 5};
        // SURROUNDING
        createChunk(C_RIGHT, {5, 5});
        createChunk(C_DOWN, {6, 5});
        createChunk(C_LEFT, {6, 4});
        createChunk(C_LEFT, {5, 4});
        createChunk(C_UP, {4, 4});
        createChunk(C_UP, {4, 5});
        createChunk(C_RIGHT, {4, 6});
        createChunk(C_RIGHT, {5, 6});
    }

    void resize_callback(int window_width, int window_height) {
        waterReflexion_id = waterReflexion.Init(window_width, window_height);
    }

    void Draw(const glm::mat4 &model,
              const glm::mat4 &view,
              const glm::mat4 &projection,
              const glm::mat4 &mirror_view,
              const glm::vec3 &camera_position,
              float water_height = -10.0f) {
        // Only draw chunks around current one.
        for (int y = -1; y <= 1; ++y) {
            for (int x = -1; x <= 1; ++x) {
                pair<int, int> relativeOffset = {x, y};
                pair<int, int> toDraw = relativeOffset + gridCoords;
                if (chunks.count(toDraw) > 0) {
                    chunks[toDraw].first->Draw(IDENTITY_MATRIX, view, projection, water_height);
                    waterReflexion.Bind();
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        chunks[toDraw].first->Draw(IDENTITY_MATRIX, mirror_view, projection, water_height, 1);
                    waterReflexion.Unbind();
                    chunks[toDraw].second->Draw(camera_position, IDENTITY_MATRIX, view, projection, water_height, glfwGetTime());
                }
            }
        }
    }

    void Cleanup() {
        for (pair<pair<int, int>, pair<Terrain*, Water*>> ctw : chunks) {
            ctw.second.first->Cleanup();
            delete ctw.second.first;
            ctw.second.second->Cleanup();
            delete ctw.second.second;
        }
        terrain_perlin.Cleanup();
        water_perlin.Cleanup();
    }

    PerlinNoise &getCurrentPerlin() {
        return terrain_perlin;
    }

    pair<int, int> &getCurrentChunkCoordinates() {
        return gridCoords;
    }
};
