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
	std::vector<Chunk*> getChunks() const;

private:
	BlockLoader* blockLoader;

	CubeModel cubeMod;
	std::vector<Chunk*> chunks;
};

inline World::World(BlockLoader* blockLoader) {
	std::cout << "[world]<load> loading world..." << std::endl;
	this->blockLoader = blockLoader;
}

inline void World::gen(HeightGenerator& heightGenerator) {
	// --- Generate Chunks ---
	int x = 0;
	int z = 0;
	int y = 0;
	for (int i = 0; i < WorldAttributes::NB_CHUNK * WorldAttributes::WOLRD_HEIGHT_CHUNK; i++) {
		chunks.push_back(new Chunk(glm::vec3(x, y, z), cubeMod));
		chunks[i]->gen(heightGenerator);

		z++;
		if(z == static_cast<int>(sqrt(WorldAttributes::NB_CHUNK))) {
			z = 0;
			y++;
		}

		if (y == WorldAttributes::WOLRD_HEIGHT_CHUNK) {
			y = 0;
			x++;
		}
	}

	for (Chunk* chunkPtr : chunks) {
		chunkPtr->pushMatrices();
	}
}

inline std::vector<Chunk*> World::getChunks() const {
	return chunks;
}

World::~World() {}