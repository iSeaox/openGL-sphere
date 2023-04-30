#pragma once

#include "blocks/cubeModel.h"
#include "blocks/block.h"
#include "chunk.h"
#include "blocks/block.h"
#include "utils/matrixUtils.h"

#include "blocks/blockLoader.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include <cstdlib>
#include <ctime>

#include <vector>


class World {
public:

	const static unsigned int GRAD_MAT_MAX_SIZE = 32;

	World(BlockLoader* blockLoader);
	~World();
	void gen();
	std::vector<Chunk> getChunks() const;
	bool hasChunk(glm::vec3 chunkPosition) const;
	bool hasChunk(float x, float y, float z) const;

private:
	const static unsigned int NB_STAGE = 4;
	const static unsigned int NB_INTRA_STAGE = 2;
	const static unsigned int NB_CHUNK = 256;

	BlockLoader* blockLoader;

	Block emptyBlock;

	CubeModel cubeMod;
	std::vector<Chunk> chunks;



	float genHeight(float coef);
	void genStageMatrices(glm::mat4 matrices[][GRAD_MAT_MAX_SIZE], float coef, int unitSize, int xSize, int zSize);
	glm::mat4 getStageMatrix(int x, int z, float unitSize, glm::mat4 matrices[][GRAD_MAT_MAX_SIZE]);
	void getIntraStageMatricesFor(int x, int z, float unitSize, glm::mat4 matricesIn[][GRAD_MAT_MAX_SIZE], glm::mat4 matricesOut[][GRAD_MAT_MAX_SIZE]);

};

inline World::World(BlockLoader* blockLoader) {
	std::cout << "[world]<load> loading world..." << std::endl;
	this->blockLoader = blockLoader;
}

inline void World::gen() {
	srand(static_cast<unsigned int>(time(NULL)));

	// --- Generate HeightMap ---
	

	// -- Stage 1 --
	float coef = 12.5f;
	float unitSize = 8 * sqrt(Chunk::NB_BLOCKS_PER_CHUNK);
	glm::mat4 gradientStage1[1][GRAD_MAT_MAX_SIZE];
	genStageMatrices(gradientStage1, coef, unitSize, 1, 1);


	// -- Stage 2 --
	coef = 10.0f;
	unitSize = 4 * sqrt(Chunk::NB_BLOCKS_PER_CHUNK);
	glm::mat4 gradientStage2[2][GRAD_MAT_MAX_SIZE];
	genStageMatrices(gradientStage2, coef, unitSize, 2, 2);

	// -- Stage 3 --
	coef = 15.0f;
	unitSize = 2 * sqrt(Chunk::NB_BLOCKS_PER_CHUNK);
	glm::mat4 gradientStage3[4][GRAD_MAT_MAX_SIZE];
	genStageMatrices(gradientStage3, coef, unitSize, 4, 4);

	// -- Stage 4 --
	coef = 8.0f;
	unitSize = 1 * sqrt(Chunk::NB_BLOCKS_PER_CHUNK);
	glm::mat4 gradientStage4[8][GRAD_MAT_MAX_SIZE];
	genStageMatrices(gradientStage4, coef, unitSize, 8, 8);

	// -- Intra Stage 1 --
	coef = 4.0f;
	unitSize = 0.5f * sqrt(Chunk::NB_BLOCKS_PER_CHUNK);
	glm::mat4 gradientInraStage1[GRAD_MAT_MAX_SIZE][GRAD_MAT_MAX_SIZE];
	genStageMatrices(gradientInraStage1, coef, unitSize, 16, 16);

	// -- Intra Stage 2 --
	coef = 1.5f;
	unitSize = 0.25f * sqrt(Chunk::NB_BLOCKS_PER_CHUNK);
	glm::mat4 gradientInraStage2[GRAD_MAT_MAX_SIZE][GRAD_MAT_MAX_SIZE];
	genStageMatrices(gradientInraStage2, coef, unitSize, 32, 32);

	// --- Generate Chunks ---
	int x = 0;
	int z = 0;
	for (int i = 0; i < NB_CHUNK; i++) {
		glm::mat4 stageMatrices[NB_STAGE];

		stageMatrices[0] = getStageMatrix(x, z, 8.0f, gradientStage1);
		stageMatrices[1] = getStageMatrix(x, z, 4.0f, gradientStage2);
		stageMatrices[2] = getStageMatrix(x, z, 2.0f, gradientStage3);
		stageMatrices[3] = getStageMatrix(x, z, 1.0f, gradientStage4);

		glm::mat4 intraStagesMatrices[NB_INTRA_STAGE][GRAD_MAT_MAX_SIZE][GRAD_MAT_MAX_SIZE];
		float IGMCoef[NB_INTRA_STAGE] = { 0.5f, 0.25f };
		getIntraStageMatricesFor(x, z, IGMCoef[0], gradientInraStage1, intraStagesMatrices[0]);
		getIntraStageMatricesFor(x, z, IGMCoef[1], gradientInraStage2, intraStagesMatrices[1]);




		Chunk chunk(glm::vec3(x, 0.0f, z), cubeMod);
		chunk.gen(stageMatrices, NB_STAGE, intraStagesMatrices, NB_INTRA_STAGE, IGMCoef);
		chunks.push_back(chunk);
		x++;
		if(x == static_cast<int>(sqrt(NB_CHUNK))) {
			x = 0;
			z++;
		}
	}

	for (Chunk chunk : chunks) {
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


inline float World::genHeight(float coef) {
	return static_cast <float> (coef - rand()) / static_cast <float> (RAND_MAX) * coef * 2.0f;;
}

inline void World::genStageMatrices(glm::mat4 matrices[][GRAD_MAT_MAX_SIZE], float coef, int unitSize, int xSize, int zSize) {
	matrices[0][0][0] = glm::vec4(0.0f, genHeight(coef), 0.0f, 0.0f);
	matrices[0][0][1] = glm::vec4(unitSize, genHeight(coef), 0.0f, 0.0f);
	matrices[0][0][2] = glm::vec4(unitSize, genHeight(coef), unitSize, 0.0f);
	matrices[0][0][3] = glm::vec4(0.0f, genHeight(coef), unitSize, 0.0f);

	for (int z = 1; z < zSize; z++) {
		
		matrices[0][z][0] = matrices[0][z - 1][3];
		matrices[0][z][1] = matrices[0][z - 1][2];
		matrices[0][z][2] = glm::vec4(unitSize, genHeight(coef), (z + 1) * unitSize, 0.0f);
		matrices[0][z][3] = glm::vec4(0.0f, genHeight(coef), (z + 1) * unitSize, 0.0f);
	}

	for (int z = 0; z < zSize; z++) {
		for (int x = 1; x < xSize; x++) {
			if (z > 0) {
				matrices[x][z][0] = matrices[x - 1][z][1];
				matrices[x][z][1] = matrices[x][z - 1][2];
				matrices[x][z][2] = glm::vec4((x + 1) * unitSize, genHeight(coef), (z + 1) * unitSize, 0.0f);
				matrices[x][z][3] = matrices[x - 1][z][2];
			}
			else {
				matrices[x][z][0] = matrices[x - 1][z][1];
				matrices[x][z][1] = glm::vec4((x + 1) * unitSize, genHeight(coef), 0.0f, 0.0f);
				matrices[x][z][2] = glm::vec4((x + 1) * unitSize, genHeight(coef), (z + 1) * unitSize, 0.0f);
				matrices[x][z][3] = matrices[x - 1][z][2];
			}
		}
	}
}

inline glm::mat4 World::getStageMatrix(int x, int z, float unitSize, glm::mat4 matrices[][GRAD_MAT_MAX_SIZE]) {
	return matrices[static_cast<int>(x / unitSize)][static_cast<int>(z / unitSize)];
}

inline void World::getIntraStageMatricesFor(int x, int z, float unitSize, glm::mat4 matricesIn[][GRAD_MAT_MAX_SIZE], glm::mat4 matricesOut[][GRAD_MAT_MAX_SIZE]) {
	int aX = static_cast<int>(x / unitSize);
	int aZ = static_cast<int>(z / unitSize);
	
	int outX = 0;
	int outZ = 0;

	for (int xi = aX; xi < (aX + 1 / unitSize); xi++) {
		for (int zi = aZ; zi < (aZ + 1 / unitSize); zi++) {
			matricesOut[outX][outZ] = matricesIn[xi][zi];
			outZ++;
		}
		outX++;
		outZ = 0;
	}
}


World::~World() {}