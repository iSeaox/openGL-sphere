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

	inline Material();
	inline Material(std::string name, EMaterial type, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
	inline Material(std::string name, EMaterial type, glm::vec3 color, float ambient, float diffuse, float specular, float shininess);
	inline ~Material();

private:
	unsigned int VAO;
	unsigned int VBO;

	inline void load();
};

inline Material::Material() {
	name = "empty";
	type = EMPTY;

	color = glm::vec3(0.0f);
	
	ambient = glm::vec3(0.0f);
	diffuse = glm::vec3(0.0f);
	specular = glm::vec3(0.0f);

	shininess = 16.0f;
}

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
	std::cout << "[material]<load> Matrial: " << name << ":" << type << " loaded" << std::endl;
}

inline Material::~Material() {}