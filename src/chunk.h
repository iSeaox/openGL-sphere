#pragma once

#include "blocks/cubeModel.h"

class Chunk {
public:
	const static int NB_BLOCKS_PER_CHUNK = 32 * 32;

	Chunk(glm::vec3 position, glm::vec4 heightValues, CubeModel& cubeMod);
	~Chunk();
	void gen();
	unsigned int getChunkVAO() const;
	glm::vec3 getPosition() const;
	glm::vec4 getHeightvalues() const;

private:
	Block blocks[NB_BLOCKS_PER_CHUNK];
	glm::vec3 position;

	unsigned int chunkVAO;
	unsigned int chunkModelVBO;

	glm::mat4 chunkModelMatrices[NB_BLOCKS_PER_CHUNK];

	glm::vec4 heightValuesS1;
};

inline Chunk::Chunk(glm::vec3 position, glm::vec4 heightValuesS1, CubeModel& cubeMod) {
	this->position = position;
	this->heightValuesS1 = heightValuesS1;
	glGenBuffers(1, &chunkModelVBO);
	glGenVertexArrays(1, &chunkVAO);
	cubeMod.bindVertexVBOTo(chunkVAO);
}

inline void Chunk::gen() {
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
	float maxX = sqrt(NB_BLOCKS_PER_CHUNK) * position.x + sqrt(NB_BLOCKS_PER_CHUNK);
	float maxZ = sqrt(NB_BLOCKS_PER_CHUNK) * position.z + sqrt(NB_BLOCKS_PER_CHUNK);
	float minX = sqrt(NB_BLOCKS_PER_CHUNK) * position.x;
	float minZ = sqrt(NB_BLOCKS_PER_CHUNK) * position.z;

	glm::vec2 lowerRight = glm::vec2(maxX, minZ);
	glm::vec2 lowerLeft = glm::vec2(minX, minZ);
	glm::vec2 upperRight = glm::vec2(maxX, maxZ);
	glm::vec2 upperLeft = glm::vec2(minX, maxZ);
	
	float lrCoef = heightValuesS1[1];
	float llCoef = heightValuesS1[0];
	float urCoef = heightValuesS1[2];
	float ulCoef = heightValuesS1[3];

	float deltaX = lowerRight.x - lowerLeft.x;
	float deltaY = upperLeft.y - lowerLeft.y;
	float deltaFx = lrCoef - llCoef;
	float deltaFy = ulCoef - llCoef;
	float deltaFxy = llCoef + urCoef - lrCoef - ulCoef;

	for (int i = 0; i < NB_BLOCKS_PER_CHUNK; i++) {
		Block& b = blocks[i];
		glm::vec2 curPos = glm::vec2(b.position.x, b.position.z);
		float dx = curPos.x - lowerLeft.x;
		float dy = curPos.y - lowerLeft.y;

		double biLinearInterpolation = (deltaFx * dx) / deltaX + (deltaFy * dy) / deltaY + (deltaFxy * dx * dy) / (deltaX * deltaY) + llCoef;
		b.position.y = (int)biLinearInterpolation;
	}

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

inline glm::vec4 Chunk::getHeightvalues() const{
	return glm::vec4(heightValuesS1[0], heightValuesS1[1], heightValuesS1[2], heightValuesS1[3]);
}

inline Chunk::~Chunk() {}