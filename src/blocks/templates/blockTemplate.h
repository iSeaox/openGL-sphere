#pragma once

#include "../../material.h"

class BlockTemplate {

public:
	BlockTemplate(Material* material);
	~BlockTemplate();

private:
	Material* material;
	int metadata;
};

BlockTemplate ::BlockTemplate(Material* material) {
	this->material = material;
	metadata = 0;

	std::cout << "[blockTemplate]<load> blockTemplate: " << material->name << ":" << material->type << " | " << metadata << " loaded" << std::endl;
}

BlockTemplate ::~BlockTemplate() {}