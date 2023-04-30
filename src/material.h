#pragma once

#include <glm/glm/glm.hpp>

#include <string>
#include <iostream>

enum EMaterial {
	EMPTY = -1, GRASS = 0, ROCK = 1, 
};

class Material {
public:

	std::string name;
	EMaterial type;

	glm::vec3 color;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Material(std::string name, EMaterial type, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
	Material(std::string name, EMaterial type, glm::vec3 color, float ambient, float diffuse, float specular, float shininess);
	~Material();

private:
	unsigned int VAO;
	unsigned int VBO;

	inline void load();
};

inline Material::Material(std::string name, EMaterial type, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess) {
	this->name = name;
	this->type = type;
	this->color = color;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;

	load();
}

inline Material::Material(std::string name, EMaterial type, glm::vec3 color, float ambient, float diffuse, float specular, float shininess) {
	this->name = name;
	this->type = type;
	this->color = color;
	this->ambient = glm::vec3(ambient);
	this->diffuse = glm::vec3(diffuse);
	this->specular = glm::vec3(specular);
	this->shininess = shininess;
	load();
}

inline void Material::load() {
	std::cout << "[material]<load> Material: " << name << ":" << type << " loaded" << std::endl;
}

inline Material::~Material() {}