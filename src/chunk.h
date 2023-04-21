#pragma once

#include "blocks/cubeModel.h"
#include "world.h"

#include <vector>

class Chunk {
public:
	const static int NB_BLOCKS_PER_CHUNK = 16 * 16;

	Block blocks[NB_BLOCKS_PER_CHUNK];

	Chunk();
	Chunk(glm::vec3 position, CubeModel& cubeMod);
	~Chunk();

	void gen(glm::mat4 gradientMatrices[], int genStage, glm::mat4 intraGradientMatrices[][32][32], int intraGenStage, float IGMCoef[]);
	void pushMatrices();
	unsigned int getChunkVAO() const;
	glm::vec3 getPosition() const;

	glm::mat4 getIntraStageMatrix(glm::vec3 bPos, float unitSize, glm::mat4 intraGradientMatrices[32][32]);

private:
	glm::vec3 position;

	unsigned int chunkVAO;
	unsigned int chunkModelVBO;

	glm::mat4 chunkModelMatrices[NB_BLOCKS_PER_CHUNK];

	float bilinearinterHeight(glm::vec3 bPos, glm::mat4 gradients);
};

inline Chunk::Chunk() {}

inline Chunk::Chunk(glm::vec3 position, CubeModel& cubeMod) {
	this->position = position;
	glGenBuffers(1, &chunkModelVBO);
	glGenVertexArrays(1, &chunkVAO);
	cubeMod.bindVertexVBOTo(chunkVAO);
}

inline void Chunk::gen(glm::mat4 gradientMatrices[], int genStage, glm::mat4 intraGradientMatrices[][32][32], int intraGenStage, float IGMCoef[]) {
	int x = sqrt(NB_BLOCKS_PER_CHUNK) * position.x;
	int z = sqrt(NB_BLOCKS_PER_CHUNK) * position.z;
	for (int i = 0; i < NB_BLOCKS_PER_CHUNK; i++) {
		blocks[i] = Block(EMaterial::GRASS, glm::vec3(x, 0.0f, z));
		x++;
		if (x == sqrt(NB_BLOCKS_PER_CHUNK) * position.x + sqrt(NB_BLOCKS_PER_CHUNK)) {
			x = sqrt(NB_BLOCKS_PER_CHUNK) * position.x;
			z++;
		}
	}

	// Maintenant ça rigole plus
	// --- Height intra stages ---
	

	// --- Height stages ---
	for (int i = 0; i < NB_BLOCKS_PER_CHUNK; i++) {
		Block& b = blocks[i];
		float height = 0;
		
		for(int i = 0 ; i < genStage; i++) {
			height += bilinearinterHeight(b.position, gradientMatrices[i]);
		}

		for (int i = 0; i < intraGenStage; i++) {
			height += bilinearinterHeight(b.position, getIntraStageMatrix(b.position, IGMCoef[i], intraGradientMatrices[i]));
		}
		b.position.y = std::round(height);
	}
}

inline void Chunk::pushMatrices() {
	// -- Fill modelMatrices --
	int i = 0;
	for (Block b : blocks) {
		if (b.type == EMaterial::EMPTY) continue;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, b.position);
		chunkModelMatrices[i] = model;
		i++;
	}

	// -- Bind new buffer to worldVAO

	glBindVertexArray(chunkVAO);

	glBindBuffer(GL_ARRAY_BUFFER, chunkModelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(chunkModelMatrices), chunkModelMatrices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);

	glBindVertexArray(0);
}

inline unsigned int Chunk::getChunkVAO() const {
	return chunkVAO;
}

inline glm::vec3 Chunk::getPosition() const {
	return position;
}

inline glm::mat4 Chunk::getIntraStageMatrix(glm::vec3 bPos, float unitSize, glm::mat4 intraGradientMatrices[32][32]) {
	float x = bPos.x - position.x * sqrt(NB_BLOCKS_PER_CHUNK);
	float z = bPos.z - position.z * sqrt(NB_BLOCKS_PER_CHUNK);
	
	int indexX = static_cast<int>(x / (sqrt(NB_BLOCKS_PER_CHUNK) * unitSize));
	int indexZ = static_cast<int>(z / (sqrt(NB_BLOCKS_PER_CHUNK) * unitSize));
	glm::mat4 result = intraGradientMatrices[indexX][indexZ];

	return result;
}

inline float Chunk::bilinearinterHeight(glm::vec3 bPos, glm::mat4 gradients) {
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



inline Chunk::~Chunk() {}