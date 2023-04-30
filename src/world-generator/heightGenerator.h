#pragma once

#include <vector>
#include <iostream>
#include <cstdlib>

#include "../utils/constants.h"

struct Gradient {
	glm::vec3 lowerLeft;
	glm::vec3 lowerRight;
	glm::vec3 upperRight;
	glm::vec3 upperLeft;

	glm::vec3& operator[](std::size_t idx) {
		switch (idx) {
		case 0:
			return lowerLeft;

		case 1:
			return lowerRight;

		case 2:
			return upperRight;

		case 3:
			return upperLeft;

		default:
			return lowerLeft;
		}
	}
	const glm::vec3& operator[](std::size_t idx) const {
		switch (idx) {
		case 0:
			return lowerLeft;

		case 1:
			return lowerRight;

		case 2:
			return upperRight;

		case 3:
			return upperLeft;

		default:
			return lowerLeft;
		}
	}
};

typedef std::vector<std::vector<Gradient>> GradientMap;

class HeightGenerator {
public:
	const static unsigned int NB_STAGE = 6;

	const static unsigned int GRAD_MAT_MAX_SIZE = 32;

	HeightGenerator(unsigned int seed);
	~HeightGenerator();

	void load();

	int getHeightOn(glm::vec3 position);

private:
	unsigned int seed;

	GradientMap gradientStage1;
	GradientMap gradientStage2;
	GradientMap gradientStage3;
	GradientMap gradientStage4;
	GradientMap gradientStage5;
	GradientMap gradientStage6;


	void genStageGradients(GradientMap& gradientMap, float coef, int unitSize, int xSize, int zSize);
	Gradient getStageGradient(int x, int z, float unitSize, const GradientMap& gradientMap) const;

	float genHeight(float coef);

	void fillGradientStage(GradientMap& gradientMap, int xSize, int zSize);

	float bilinearinterHeight(glm::vec3 bPos, Gradient gradients);
};

HeightGenerator::HeightGenerator(unsigned int seed) {
	this->seed = seed;
}

HeightGenerator::~HeightGenerator() {}

inline void HeightGenerator::load() {
	std::cout << "[generator]<load> loading of HeightGenerator (seed: " << seed << ")" << std::endl;
	srand(seed);

	// --- Generate HeightMap ---
	// -- Stage 1 --
	float coef = 12.5f;
	float unitSize = 8 * WorldAttributes::CHUNK_SIZE;
	fillGradientStage(gradientStage1, 2, 2);
	genStageGradients(gradientStage1, coef, unitSize, 2, 2);

	// -- Stage 2 --
	coef = 10.0f;
	unitSize = 4 * WorldAttributes::CHUNK_SIZE;
	fillGradientStage(gradientStage2, 4, 4);
	genStageGradients(gradientStage2, coef, unitSize, 4, 4);

	// -- Stage 3 --
	coef = 15.0f;
	unitSize = 2 * WorldAttributes::CHUNK_SIZE;
	fillGradientStage(gradientStage3, 8, 8);
	genStageGradients(gradientStage3, coef, unitSize, 8, 8);

	// -- Stage 4 --
	coef = 8.0f;
	unitSize = 1 * WorldAttributes::CHUNK_SIZE;
	fillGradientStage(gradientStage4, 16, 16);
	genStageGradients(gradientStage4, coef, unitSize, 16, 16);

	// -- Intra Stage 1 --
	coef = 4.0f;
	unitSize = 0.5f * WorldAttributes::CHUNK_SIZE;
	fillGradientStage(gradientStage5, 32, 32);
	genStageGradients(gradientStage5, coef, unitSize, 32, 32);

	// -- Intra Stage 2 --
	coef = 1.5f;
	unitSize = 0.25f * WorldAttributes::CHUNK_SIZE;
	fillGradientStage(gradientStage6, 64, 64);
	genStageGradients(gradientStage6, coef, unitSize, 64, 64);

	std::cout << "[generator]<load> loading of height stages 6/6" << std::endl;
}

inline int HeightGenerator::getHeightOn(glm::vec3 position) {
	int chunkX = static_cast<int>(position.x) / WorldAttributes::CHUNK_SIZE;
	int chunkZ = static_cast<int>(position.z) / WorldAttributes::CHUNK_SIZE;

	int inChunkX = static_cast<int>(position.x) % WorldAttributes::CHUNK_SIZE;
	int inChunkZ = static_cast<int>(position.z) % WorldAttributes::CHUNK_SIZE;

	Gradient stageGradients[NB_STAGE];

	stageGradients[0] = getStageGradient(position.x, position.z, 8.0f, gradientStage1);
	stageGradients[1] = getStageGradient(position.x, position.z, 4.0f, gradientStage2);
	stageGradients[2] = getStageGradient(position.x, position.z, 2.0f, gradientStage3);
	stageGradients[3] = getStageGradient(position.x, position.z, 1.0f, gradientStage4);

	stageGradients[4] = getStageGradient(position.x, position.z, 0.5f, gradientStage5);
	stageGradients[5] = getStageGradient(position.x, position.z, 0.25f, gradientStage6);


	float height = 0.0f;
	for (int i = 0; i < NB_STAGE; i++) {
		height += bilinearinterHeight(position, stageGradients[i]);
	}

	return std::round(height);
}

inline void HeightGenerator::genStageGradients(GradientMap& gradientMap, float coef, int unitSize, int xSize, int zSize) {
	gradientMap[0][0][0] = glm::vec4(0.0f, genHeight(coef), 0.0f, 0.0f);
	gradientMap[0][0][1] = glm::vec4(unitSize, genHeight(coef), 0.0f, 0.0f);
	gradientMap[0][0][2] = glm::vec4(unitSize, genHeight(coef), unitSize, 0.0f);
	gradientMap[0][0][3] = glm::vec4(0.0f, genHeight(coef), unitSize, 0.0f);

	for (int z = 1; z < zSize; z++) {

		gradientMap[0][z][0] = gradientMap[0][z - 1][3];
		gradientMap[0][z][1] = gradientMap[0][z - 1][2];
		gradientMap[0][z][2] = glm::vec4(unitSize, genHeight(coef), (z + 1) * unitSize, 0.0f);
		gradientMap[0][z][3] = glm::vec4(0.0f, genHeight(coef), (z + 1) * unitSize, 0.0f);
	}

	for (int z = 0; z < zSize; z++) {
		for (int x = 1; x < xSize; x++) {
			if (z > 0) {
				gradientMap[x][z][0] = gradientMap[x - 1][z][1];
				gradientMap[x][z][1] = gradientMap[x][z - 1][2];
				gradientMap[x][z][2] = glm::vec4((x + 1) * unitSize, genHeight(coef), (z + 1) * unitSize, 0.0f);
				gradientMap[x][z][3] = gradientMap[x - 1][z][2];
			}
			else {
				gradientMap[x][z][0] = gradientMap[x - 1][z][1];
				gradientMap[x][z][1] = glm::vec4((x + 1) * unitSize, genHeight(coef), 0.0f, 0.0f);
				gradientMap[x][z][2] = glm::vec4((x + 1) * unitSize, genHeight(coef), (z + 1) * unitSize, 0.0f);
				gradientMap[x][z][3] = gradientMap[x - 1][z][2];
			}
		}
	}
}

inline float HeightGenerator::genHeight(float coef) {
	return static_cast <float> (coef - rand()) / static_cast <float> (RAND_MAX) * coef * 2.0f;;
}

inline void HeightGenerator::fillGradientStage(GradientMap& gradientMap, int xSize, int zSize) {
	for (int i = 0; i < xSize; i++) {
		std::vector<Gradient> temp;
		for (int j = 0; j < zSize; j++) {
			temp.push_back(Gradient());
		}
		gradientMap.push_back(temp);
	}
}

inline Gradient HeightGenerator::getStageGradient(int x, int z, float unitSize, const GradientMap& gradientMap) const {
	float chunkRelativeX = x / static_cast<float>(WorldAttributes::CHUNK_SIZE);
	float chunkRelativeZ = z / static_cast<float>(WorldAttributes::CHUNK_SIZE);
	return gradientMap[static_cast<int>(chunkRelativeX / unitSize)][static_cast<int>(chunkRelativeZ / unitSize)];
}

inline float HeightGenerator::bilinearinterHeight(glm::vec3 bPos, Gradient gradients) {
	glm::vec2 lowerRight = glm::vec2(gradients[1].x, gradients[1].z);
	glm::vec2 lowerLeft = glm::vec2(gradients[0].x, gradients[0].z);
	glm::vec2 upperRight = glm::vec2(gradients[2].x, gradients[2].z);
	glm::vec2 upperLeft = glm::vec2(gradients[3].x, gradients[3].z);

	float lrCoef = gradients[1].y;
	float llCoef = gradients[0].y;
	float urCoef = gradients[2].y;
	float ulCoef = gradients[3].y;

	float deltaX = lowerRight.x - lowerLeft.x;
	float deltaY = upperLeft.y - lowerLeft.y;
	float deltaFx = lrCoef - llCoef;
	float deltaFy = ulCoef - llCoef;
	float deltaFxy = llCoef + urCoef - lrCoef - ulCoef;

	glm::vec2 curPos = glm::vec2(bPos.x, bPos.z);
	float dx = curPos.x - lowerLeft.x;
	float dy = curPos.y - lowerLeft.y;

	return (deltaFx * dx) / deltaX + (deltaFy * dy) / deltaY + (deltaFxy * dx * dy) / (deltaX * deltaY) + llCoef;
}
