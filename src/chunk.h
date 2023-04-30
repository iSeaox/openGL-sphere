#pragma once

#include "blocks/cubeModel.h"
#include "world.h"

#include <vector>

class Chunk {
public:
	const static int CHUNK_SIZE = 16;

	char blockT[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

	Chunk();
	Chunk(glm::vec3 position, CubeModel& cubeMod);
	~Chunk();

	void gen(glm::mat4 gradientMatrices[], int genStage, glm::mat4 intraGradientMatrices[][32][32], int intraGenStage, float IGMCoef[]);
	void pushMatrices();
	unsigned int getChunkVAO() const;
	glm::vec3 getPosition() const;

	void setBlock(glm::vec3 position, EMaterial mat);
	void setBlock(Block& block);
	Block getBlock(glm::vec3 pos);

	glm::mat4 getIntraStageMatrix(glm::vec3 bPos, float unitSize, glm::mat4 intraGradientMatrices[32][32]);

	unsigned int getNbDrawableBlock();

private:
	glm::vec3 position;

	unsigned int chunkVAO;
	unsigned int chunkModelVBO;

	unsigned int nbDrawableBlock;

	glm::mat4 chunkModelMatrices[CHUNK_SIZE * CHUNK_SIZE];

	float bilinearinterHeight(glm::vec3 bPos, glm::mat4 gradients);
	glm::vec3 getPositionFromIndex(int index);
	int getIndexFromPosition(glm::vec3 pos);
};

inline Chunk::Chunk() {}

inline Chunk::Chunk(glm::vec3 position, CubeModel& cubeMod) {
	this->position = position;
	glGenBuffers(1, &chunkModelVBO);
	glGenVertexArrays(1, &chunkVAO);
	cubeMod.bindVertexVBOTo(chunkVAO);
}

inline void Chunk::gen(glm::mat4 gradientMatrices[], int genStage, glm::mat4 intraGradientMatrices[][32][32], int intraGenStage, float IGMCoef[]) {
	for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
		blockT[i] = -1;
	}

	// Maintenant ça rigole plus
	// --- Height intra stages ---
	

	// --- Height stages ---
	for (int x = position.x * CHUNK_SIZE; x < (position.x + 1) * CHUNK_SIZE; x++) {
		for (int z = position.z * CHUNK_SIZE; z < (position.z + 1) * CHUNK_SIZE; z++) {
			Block b = getBlock(glm::vec3(x, 0.0f, z));
			float height = 0;

			for (int i = 0; i < genStage; i++) {
				height += bilinearinterHeight(b.position, gradientMatrices[i]);
			}

			for (int i = 0; i < intraGenStage; i++) {
				height += bilinearinterHeight(b.position, getIntraStageMatrix(b.position, IGMCoef[i], intraGradientMatrices[i]));
			}
			b.position.y = std::round(height) + 60;
			b.type = GRASS;

			if (std::round(height) + 60 >= 0 && std::round(height) + 60 < 16) {
				setBlock(b);
				
			}
		}
	}
}

inline void Chunk::pushMatrices() {
	// -- Fill modelMatrices --
	int j = 0;
	for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
		if (blockT[i] == -1) continue;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, getPositionFromIndex(i));
		chunkModelMatrices[j] = model;
		j++;
	}
	nbDrawableBlock = j;

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

inline void Chunk::setBlock(glm::vec3 bPos, EMaterial mat) {
	blockT[getIndexFromPosition(bPos)] = mat;
}

inline void Chunk::setBlock(Block& block) {
	blockT[getIndexFromPosition(block.position)] = block.type;
}

inline Block Chunk::getBlock(glm::vec3 bPos) {
	EMaterial type = (EMaterial)((int)blockT[getIndexFromPosition(bPos)]);
	Block b(type, bPos);
	return b;
}

inline glm::mat4 Chunk::getIntraStageMatrix(glm::vec3 bPos, float unitSize, glm::mat4 intraGradientMatrices[32][32]) {
	float x = bPos.x - position.x * CHUNK_SIZE;
	float z = bPos.z - position.z * CHUNK_SIZE;
	
	int indexX = static_cast<int>(x / (CHUNK_SIZE * unitSize));
	int indexZ = static_cast<int>(z / (CHUNK_SIZE * unitSize));
	glm::mat4 result = intraGradientMatrices[indexX][indexZ];

	return result;
}

inline unsigned int Chunk::getNbDrawableBlock(){
	return nbDrawableBlock;
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

inline glm::vec3 Chunk::getPositionFromIndex(int index) {
	int temp = index;
	int x = temp / (CHUNK_SIZE * CHUNK_SIZE);
	temp -= x * CHUNK_SIZE * CHUNK_SIZE;
	int y = temp / CHUNK_SIZE;
	temp -= y * CHUNK_SIZE;
	return glm::vec3(x + position.x * CHUNK_SIZE, y + position.y * CHUNK_SIZE, temp + position.z * CHUNK_SIZE);
}

inline int Chunk::getIndexFromPosition(glm::vec3 pos) {
	return (static_cast<int>(pos.z) % CHUNK_SIZE) + (static_cast<int>(pos.y) % CHUNK_SIZE) * CHUNK_SIZE + (static_cast<int>(pos.x) % CHUNK_SIZE) * CHUNK_SIZE * CHUNK_SIZE;
}



inline Chunk::~Chunk() {}