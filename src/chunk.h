#pragma once

#include "blocks/cubeModel.h"
#include "world.h"
#include "world-generator/heightGenerator.h"
#include "utils/constants.h"

#include <vector>

class Chunk {
public:
	char blockT[WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE];

	Chunk();
	Chunk(glm::vec3 position, CubeModel& cubeMod);
	~Chunk();

	void gen(HeightGenerator& heightGenerator);
	void pushMatrices();
	unsigned int getChunkVAO() const;
	glm::vec3 getPosition() const;

	void setBlock(glm::vec3 position, EMaterial mat);
	void setBlock(Block& block);
	Block getBlock(glm::vec3 pos);

	unsigned int getNbDrawableBlock();

private:
	glm::vec3 position;

	unsigned int chunkVAO;
	unsigned int chunkModelVBO;

	unsigned int nbDrawableBlock;

	glm::mat4 chunkModelMatrices[WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE];

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

inline void Chunk::gen(HeightGenerator& heightGenerator) {
	for (int i = 0; i < WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE; i++) {
		blockT[i] = -1;
	}

	// Maintenant ça rigole plus
	// --- Height intra stages ---
	

	// --- Height stages ---
	for (int x = position.x * WorldAttributes::CHUNK_SIZE; x < (position.x + 1) * WorldAttributes::CHUNK_SIZE; x++) {
		for (int z = position.z * WorldAttributes::CHUNK_SIZE; z < (position.z + 1) * WorldAttributes::CHUNK_SIZE; z++) {
			Block b = getBlock(glm::vec3(x, 0.0f, z));

			b.position.y = heightGenerator.getHeightOn(b.position) + 60;
			b.type = GRASS;

			if (b.position.y >= 0 && b.position.y < 16) {
				setBlock(b);
				
			}
		}
	}
}

inline void Chunk::pushMatrices() {
	// -- Fill modelMatrices --
	int j = 0;
	for (int i = 0; i < WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE; i++) {
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


inline unsigned int Chunk::getNbDrawableBlock(){
	return nbDrawableBlock;
}

inline glm::vec3 Chunk::getPositionFromIndex(int index) {
	int temp = index;
	int x = temp / (WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE);
	temp -= x * WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE;
	int y = temp / WorldAttributes::CHUNK_SIZE;
	temp -= y * WorldAttributes::CHUNK_SIZE;
	return glm::vec3(x + position.x * WorldAttributes::CHUNK_SIZE, y + position.y * WorldAttributes::CHUNK_SIZE, temp + position.z * WorldAttributes::CHUNK_SIZE);
}

inline int Chunk::getIndexFromPosition(glm::vec3 pos) {
	return (static_cast<int>(pos.z) % WorldAttributes::CHUNK_SIZE) + (static_cast<int>(pos.y) % WorldAttributes::CHUNK_SIZE) * WorldAttributes::CHUNK_SIZE + (static_cast<int>(pos.x) % WorldAttributes::CHUNK_SIZE) * WorldAttributes::CHUNK_SIZE * WorldAttributes::CHUNK_SIZE;
}

inline Chunk::~Chunk() {}