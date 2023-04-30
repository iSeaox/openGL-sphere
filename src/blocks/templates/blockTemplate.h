#pragma once

#include "../../material.h"

class BlockTemplate {

public:
	BlockTemplate(Material* material);
	~BlockTemplate();

private:
	Material* material;
	uint8_t metadata;
};

BlockTemplate ::BlockTemplate(Material* material) {
	this->material = material;
	metadata = 0;

	std::cout << "[blockTemplate]<load> blockTemplate: " << material->name << ":" << material->type << " | " << (int) metadata << " loaded" << std::endl;
}

BlockTemplate ::~BlockTemplate() {}