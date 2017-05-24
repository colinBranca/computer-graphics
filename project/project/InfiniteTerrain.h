#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

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
	vector<Terrain*> terrains;
	//vector<Water*> waters;
	//vector<PerlinNoise*> terrain_perlins;
	PerlinNoise terrain_perlin;
	PerlinNoise water_perlin;

	int current = 0;
	int seedX = 1;
	int seedY = 1;
	pair<int, int> gridCoords = {5, 5};

	map<pair<int, int>, int> chunks;
	pair<int, int> current_chunk_coordinates;

	float chunk_size = 20.0f;
	int grid_resolution = 512;

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

	int createChunk(ChunkRelativePosition pos) {
	    cout << "There are now " << terrains.size() << " chunks " << " in " << gridCoords.first << " " << gridCoords.second << endl;
	    //terrain_perlins.push_back(new PerlinNoise());
	    //terrain_perlins.back()->Init(grid_resolution, grid_resolution, 7, 3.5f, 1 / 400.0f, 1 / 400.0f);
	    //terrain_perlins.back()->Compute(seedX, seedY);

	    terrains.push_back(new Terrain());
	    //waters.push_back(new Water());

	    pair<int, int> coefs = getCoefs(pos);

	    terrains.back()->Init(grid_resolution, terrain_perlin.getHeightTexId(), 20.f, terrains[current]->minX_ + coefs.first * chunk_size, terrains[current]->minY_ + coefs.second * chunk_size);
	    //terrains.back()->Init(grid_resolution, terrain_perlins.back()->getHeightTexId(), 20.f, terrains[current]->minX_ + coefs.first * chunk_size, terrains[current]->minY_ + coefs.second * chunk_size);
	    //waters.back()->Init(waterReflexion_id, water_wave_tex_id, grid_resolution, 20.0f, waters[current]->minX_ + coefs.first * chunk_size, waters[current]->minY_ + coefs.second * chunk_size);
	    return terrains.size() - 1;
	}

	ChunkRelativePosition getChunkCoordinates(glm::vec2 coords2d) {
	    if (coords2d.x > terrains[current]->minX_ + chunk_size) {
	        return C_RIGHT;
	    }
	    else if (coords2d.x < terrains[current]->minX_) {
	        return C_LEFT;
	    }
	    else if (coords2d.y > terrains[current]->minY_ + chunk_size) {
	        return C_UP;
	    }
	    else if (coords2d.y < terrains[current]->minY_){
	        return C_DOWN;
	    }
	    else {
	        return C_STILL;
	    }
	}
	
pair<int, int> before = {5,5};
public:
	void checkChunk(glm::vec2 pos) {
	    ChunkRelativePosition position = getChunkCoordinates(pos);
	    gridCoords = getCoefs(position) + gridCoords;
		if (gridCoords != before) {
			cout << "in chunk " << gridCoords.first << " " << gridCoords.second << endl;
			before = gridCoords;
		}
	    if (chunks.count(gridCoords) < 1) {
	        current = createChunk(position);
	        chunks[gridCoords] = current;
	    } else {
	        current = chunks[gridCoords];
	    }
	    current_chunk_coordinates = gridCoords;
	    // cout << "i: " << current_chunk_coordinates.first << ", j: " << current_chunk_coordinates.second << endl;
	}

	void Init(int window_width, int window_height) {
    	waterReflexion_id = waterReflexion.Init(window_width, window_height);

		water_wave_tex_id = water_perlin.Init(grid_resolution, grid_resolution, 1, 1.0);
	    water_perlin.Compute();

	    //terrain_perlins.push_back(new PerlinNoise());
	    //terrain_perlins[current]->Init(grid_resolution, grid_resolution, 7, 3.5f, 1 / 400.0f, 1 / 400.0f);
	    //terrain_perlins[current]->Compute();
	    terrain_perlin.Init(grid_resolution * 11, grid_resolution * 11, 7, 3.5f, 1 / 400.0f, 1 / 400.0f);
	    terrain_perlin.Compute();
	    terrains.push_back(new Terrain());
	    // terrains[0]->Init(grid_resolution, terrain_perlin.getHeightTexId(), chunk_size, -10.0, -10.0);

	    //waters.push_back(new Water());
    	//waters[current]->Init(waterReflexion_id, water_wave_tex_id);
	    // FIRST
	    terrains[0]->Init(grid_resolution, terrain_perlin.getHeightTexId(), chunk_size, 80, 80);//-chunk_size / 2.0, -chunk_size / 2.0);
	    chunks[{5, 5}] = 0;
		gridCoords = {5, 5};
	    // SURROUNDING
		current = createChunk(C_RIGHT);
	    chunks[{6, 5}] = current;
	    current = createChunk(C_DOWN);
	    chunks[{6, 4}] = current;
	    current = createChunk(C_LEFT);
	    chunks[{5, 4}] = current;
	    current = createChunk(C_LEFT);
	    chunks[{4, 4}] = current;
	    current = createChunk(C_UP);
	    chunks[{4, 5}] = current;
	    current = createChunk(C_UP);
	    chunks[{4, 6}] = current;
	    current = createChunk(C_RIGHT);
	    chunks[{5, 6}] = current;
	    current = createChunk(C_RIGHT);
	    chunks[{6, 6}] = current;
     /*
	    current = createChunk(C_RIGHT);
	    chunks[{1, 0}] = current;
	    current = createChunk(C_DOWN);
	    chunks[{1, -1}] = current;
	    current = createChunk(C_LEFT);
	    chunks[{0, -1}] = current;
	    current = createChunk(C_LEFT);
	    chunks[{-1, -1}] = current;
	    current = createChunk(C_UP);
	    chunks[{-1, 0}] = current;
	    current = createChunk(C_UP);
	    chunks[{-1, 1}] = current;
	    current = createChunk(C_RIGHT);
	    chunks[{0, 1}] = current;
	    current = createChunk(C_RIGHT);
	    chunks[{1, 1}] = current;
		*/
	}

	void Draw(const glm::mat4 &model,
        const glm::mat4 &view,
        const glm::mat4 &projection,
        float water_height = -10.0f) {
			for (size_t i = 0; i < terrains.size(); ++i) {
        	//waters[i]->Draw(IDENTITY_MATRIX, view, projection, water_height, time);
       			terrains[i]->Draw(IDENTITY_MATRIX, view, projection, water_height);
    		}
   		//water.Draw(IDENTITY_MATRIX, view, projection, water_height, time);
    	//terrain.Draw(IDENTITY_MATRIX, view, projection, water_height);
    	// mirror the camera position
	}

	void Cleanup() {
		for (size_t i = 0; i < terrains.size(); ++i) {
			terrains[i]->Cleanup();
    	    delete terrains[i];
    	}
        terrain_perlin.Cleanup();
        water_perlin.Cleanup();
        //waters[i]->Cleanup();
        //delete waters[i];

        //terrain_perlins[i]->Cleanup();
        //delete terrain_perlins[i];
	}

	PerlinNoise &getCurrentPerlin() {
		return terrain_perlin;
	}

	pair<int, int> &getCurrentChunkCoordinates() {
		return current_chunk_coordinates;
	}
};
