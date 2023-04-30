#pragma once

#include <glm/glm/glm.hpp>
#include <cstdlib>

#include "../chunk.h"

struct SubSquare {
	glm::vec3 lowerLeftGradient;
	glm::vec3 lowerRightGradient;
	glm::vec3 upperRightGradient;
	glm::vec3 upperLeftGradient;
};


class HeightMapGenerator {
public:
	HeightMapGenerator();
	~HeightMapGenerator();

	SubSquare* getSubSquareFor(glm::vec3 bPos, float unitSize, SubSquare* subSquares[32][32]);

private:
	SubSquare* subSquareStage1[1][1];
	SubSquare* subSquareStage2[2][2];
	SubSquare* subSquareStage3[4][4];
	SubSquare* subSquareStage4[8][8];

	void genStageMatrices(SubSquare subSquares[32][32], float coef, int unitSize, int xSize, int zSize);
	float genHeight(float coef);
};

HeightMapGenerator::HeightMapGenerator() {
	SubSquare workingSubSquares[32][32];
	genStageMatrices(workingSubSquares, 12.5f, 8 * Chunk::CHUNK_SIZE, 1, 1);
	subSquareStage1[0][0] = new SubSquare(workingSubSquares[0][0]);

	genStageMatrices(workingSubSquares, 10.0f, 4 * Chunk::CHUNK_SIZE, 2, 2);
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			subSquareStage2[i][j] = new SubSquare(workingSubSquares[i][j]);
		}
	}

	genStageMatrices(workingSubSquares, 15.0f, 2 * Chunk::CHUNK_SIZE, 4, 4);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			subSquareStage3[i][j] = new SubSquare(workingSubSquares[i][j]);
		}
	}

	genStageMatrices(workingSubSquares, 8.0f, Chunk::CHUNK_SIZE, 8, 8);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			subSquareStage4[i][j] = new SubSquare(workingSubSquares[i][j]);
		}
	}

	SubSquare* workTest[32][32];
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			workTest[i][j] = subSquareStage2[i][j];
		}
	}
	SubSquare test = *getSubSquareFor(glm::vec3(16.0f, 0.0f, 129.0f), 4 * Chunk::CHUNK_SIZE, workTest);

	std::cout << test.lowerLeftGradient.x << ", " << test.lowerLeftGradient.y << ", " << test.lowerLeftGradient.z << std::endl;
	std::cout << test.upperRightGradient.x << ", " << test.upperRightGradient.y << ", " << test.upperRightGradient.z << std::endl;
}

HeightMapGenerator::~HeightMapGenerator() {
	delete subSquareStage1[0][0];

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			delete subSquareStage2[i][j];
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			delete subSquareStage3[i][j];
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			delete subSquareStage4[i][j];
		}
	}
}

inline SubSquare* HeightMapGenerator::getSubSquareFor(glm::vec3 bPos, float unitSize, SubSquare* subSquares[32][32]) {
	int x = (bPos.x - static_cast<int>(bPos.x) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE;
	int z = (bPos.z - static_cast<int>(bPos.z) % Chunk::CHUNK_SIZE) / Chunk::CHUNK_SIZE;

	int indexX = static_cast<int>(x / (Chunk::CHUNK_SIZE * unitSize));
	int indexZ = static_cast<int>(z / (Chunk::CHUNK_SIZE * unitSize));

	std::cout << x << ", " << z << " | " << indexX << ", " << indexZ << std::endl;
	return subSquares[x][z];
}

inline void HeightMapGenerator::genStageMatrices(SubSquare subSquares[32][32], float coef, int unitSize, int xSize, int zSize) {

	subSquares[0][0].lowerLeftGradient = glm::vec3(0.0f, genHeight(coef), 0.0f);
	subSquares[0][0].lowerRightGradient = glm::vec3(unitSize, genHeight(coef), 0.0f);
	subSquares[0][0].upperRightGradient = glm::vec3(unitSize, genHeight(coef), unitSize);
	subSquares[0][0].upperLeftGradient = glm::vec3(0.0f, genHeight(coef), unitSize);

	for (int z = 1; z < zSize; z++) {

		subSquares[0][z].lowerLeftGradient = subSquares[0][z - 1].upperLeftGradient;
		subSquares[0][z].lowerRightGradient = subSquares[0][z - 1].upperRightGradient;
		subSquares[0][z].upperRightGradient = glm::vec3(unitSize, genHeight(coef), (z + 1) * unitSize);
		subSquares[0][z].upperLeftGradient = glm::vec3(0.0f, genHeight(coef), (z + 1) * unitSize);
	}

	for (int z = 0; z < zSize; z++) {
		for (int x = 1; x < xSize; x++) {
			if (z > 0) {
				subSquares[x][z].lowerLeftGradient = subSquares[x - 1][z].lowerRightGradient;
				subSquares[x][z].lowerRightGradient = subSquares[x][z - 1].upperRightGradient;
				subSquares[x][z].upperRightGradient = glm::vec4((x + 1) * unitSize, genHeight(coef), (z + 1) * unitSize, 0.0f);
				subSquares[x][z].upperLeftGradient = subSquares[x - 1][z].upperRightGradient;
			}
			else {
				subSquares[x][z].lowerLeftGradient = subSquares[x - 1][z].lowerRightGradient;
				subSquares[x][z].lowerRightGradient = glm::vec4((x + 1) * unitSize, genHeight(coef), 0.0f, 0.0f);
				subSquares[x][z].upperRightGradient = glm::vec4((x + 1) * unitSize, genHeight(coef), (z + 1) * unitSize, 0.0f);
				subSquares[x][z].upperLeftGradient = subSquares[x - 1][z].upperRightGradient;
			}
		}
	}
}

inline float HeightMapGenerator::genHeight(float coef) {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * coef;
}