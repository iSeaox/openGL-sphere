#pragma once

#include <iostream>

#include "../material.h"
#include "templates/blockTemplate.h"

#include "../color.h"

class BlockLoader {
public:

	BlockLoader();
	~BlockLoader();

	Material* getMaterial(EMaterial material);

private:

	Material* materials[2] = { NULL };
	BlockTemplate* blockTemplate[2] = { NULL };

};

BlockLoader::BlockLoader() {
	materials[EMaterial::GRASS] = new Material("grass", EMaterial::GRASS, getColorFrom(144, 190, 109), 0.4f, 0.4f, 0.2f, 16.0f);
	materials[EMaterial::ROCK] = new Material("rock", EMaterial::ROCK, getColorFrom(112, 102, 91), 0.3f, 0.4f, 0.1f, 16.0f);

	int i = 0;
	for (Material* mat : materials) {
		blockTemplate[i] = new BlockTemplate(mat);
		i++;
	}
}

BlockLoader::~BlockLoader() {
	delete blockTemplate[1];
	delete blockTemplate[0];

	delete materials[1];
	delete materials[0];
}

inline Material* BlockLoader::getMaterial(EMaterial material) {
	return materials[material];
}
