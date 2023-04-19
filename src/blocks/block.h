#pragma once

#include "../material.h"

#include <./glm/glm/glm.hpp>

class Block {
public:

	glm::vec3 position;
	EMaterial type;

	Block();
	Block(EMaterial type, glm::vec3 pos);
	~Block();

private:
};

Block::Block() {
	position = glm::vec3(0.0f);
	type = EMaterial::EMPTY;
}

Block::Block(EMaterial type, glm::vec3 pos) {
	position = pos;
	this->type = type;
}

Block::~Block() {}