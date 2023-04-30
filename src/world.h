#pragma once

#include "blocks/cubeModel.h"
#include "blocks/block.h"
#include "chunk.h"
#include "blocks/block.h"
#include "utils/matrixUtils.h"

#include "blocks/blockLoader.h"

#include "utils/constants.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include <cstdlib>
#include <ctime>

#include <vector>


class World {
public:

	World(BlockLoader* blockLoader);
	~World();
	void gen(HeightGenerator& heightGenerator);
	std::vector<Chunk> getChunks() const;
	bool hasChunk(glm::vec3 chunkPosition) const;
	bool hasChunk(float x, float y, float z) const;

private:
	BlockLoader* blockLoader;

	Block emptyBlock;

	CubeModel cubeMod;
	std::vector<Chunk> chunks;
};

inline World::World(BlockLoader* blockLoader) {
	std::cout << "[world]<load> loading world..." << std::endl;
	this->blockLoader = blockLoader;
}

inline void World::gen(HeightGenerator& heightGenerator) {
	// --- Generate Chunks ---
	int x = 0;
	int z = 0;
	for (int i = 0; i < WorldAttributes::NB_CHUNK; i++) {
		Chunk chunk(glm::vec3(x, 0.0f, z), cubeMod);
		chunk.gen(heightGenerator);
		chunks.push_back(chunk);
		x++;
		if(x == static_cast<int>(sqrt(WorldAttributes::NB_CHUNK))) {
			x = 0;
			z++;
		}
	}

	for (Chunk& chunk : chunks) {
		chunk.pushMatrices();
	}
}

inline std::vector<Chunk> World::getChunks() const {
	return chunks;
}

inline bool World::hasChunk(glm::vec3 chunkPosition) const
{
	for (Chunk chunk : chunks) {
		if (chunkPosition == chunk.getPosition()) {
			return true;
		}
	}
	return false;
}

inline bool World::hasChunk(float x, float y, float z) const
{
	for (Chunk chunk : chunks) {
		if (glm::vec3(x, y, z) == chunk.getPosition()) {
			return true;
		}
	}
	return false;
}

World::~World() {}