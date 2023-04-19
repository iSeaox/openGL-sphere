#pragma once

#include "blocks/cubeModel.h"
#include "blocks/block.h"
#include "chunk.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include <cstdlib>
#include <ctime>

#include <vector>


class World {
public:
	World();
	~World();
	void gen();
	std::vector<Chunk> getChunks() const;
	bool hasChunk(glm::vec3 chunkPosition) const;
	bool hasChunk(float x, float y, float z) const;

private:

	CubeModel cubeMod;
	std::vector<Chunk> chunks;

	glm::vec4 getChunkHeightValues(glm::vec3 chunkPosition) const;
	glm::vec4 getChunkHeightValues(float x, float y, float z) const;

};

inline World::World() {}

inline void World::gen() {
	srand(static_cast <unsigned> (time(NULL)));

	int x = 0;
	int z = 0;
	for (int i = 0; i < 64; i++) {
		const float coef = 20.0f;

		float lrCoef = static_cast <float> (coef - rand()) / static_cast <float> (RAND_MAX) * coef * 2.0f;
		float llCoef = static_cast <float> (coef - rand()) / static_cast <float> (RAND_MAX) * coef * 2.0f;
		float urCoef = static_cast <float> (coef - rand()) / static_cast <float> (RAND_MAX) * coef * 2.0f;
		float ulCoef = static_cast <float> (coef - rand()) / static_cast <float> (RAND_MAX) * coef * 2.0f;
		// -- getting around coef --
		std::cout << "--------" << std::endl;
		std::cout << "pour :" << x << ", " << 0.0f << ", " << z << std::endl;
		if (hasChunk(x + 1, 0.0f, z)) {
			glm::vec4 rightValues = getChunkHeightValues(x + 1, 0.0f, z);
			urCoef = rightValues[3];
			lrCoef = rightValues[0];
			std::cout << "right" << std::endl;
		}

		if (hasChunk(x - 1, 0.0f, z)) {
			glm::vec4 leftValues = getChunkHeightValues(x - 1, 0.0f, z);
			ulCoef = leftValues[2];
			llCoef = leftValues[1];
			std::cout << "left" << std::endl;
		}

		if (hasChunk(x, 0.0f, z + 1)) {
			glm::vec4 topValues = getChunkHeightValues(x, 0.0f, z + 1);
			urCoef = topValues[1];
			ulCoef = topValues[0];
			std::cout << "top" << std::endl;
		}

		if (hasChunk(x, 0.0f, z - 1)) {
			glm::vec4 bottomValues = getChunkHeightValues(x, 0.0f, z - 1);
			lrCoef = bottomValues[2];
			llCoef = bottomValues[3];
			std::cout << "bottom" << std::endl;
		}
		std::cout << llCoef << ", " << lrCoef << ", " << urCoef << ", " << ulCoef << std::endl;

		Chunk chunk(glm::vec3(x, 0.0f, z), glm::vec4(llCoef, lrCoef, urCoef, ulCoef), cubeMod);
		chunk.gen();
		chunks.push_back(chunk);

		x++;
		if(x == 8) {
			x = 0;
			z++;
		}
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



inline glm::vec4 World::getChunkHeightValues(glm::vec3 chunkPosition) const{
	for (Chunk chunk : chunks) {
		if (chunkPosition == chunk.getPosition()) {
			return chunk.getHeightvalues();
		}
	}
	return glm::vec4();
}

inline glm::vec4 World::getChunkHeightValues(float x, float y, float z) const {
	for (Chunk chunk : chunks) {
		if (glm::vec3(x, y, z) == chunk.getPosition()) {
			return chunk.getHeightvalues();
		}
	}
	return glm::vec4();
}

World::~World() {}