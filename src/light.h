#pragma once

#include <glm/glm/glm.hpp>

class Light {
public:

	enum LightType {
		LIGHT_DIRECTIONNAL
	};

	glm::vec3 color;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	virtual LightType getType() const = 0;
};

class DirLight : public Light{
public:
	glm::vec3 direction;

	DirLight(glm::vec3 color, glm::vec3 direction, float ambient, float diffuse, float specular);
	Light::LightType getType() const;
	~DirLight();
};

DirLight::DirLight(glm::vec3 color, glm::vec3 direction, float ambient, float diffuse, float specular) {
	this->color = color;
	this->direction = direction;

	this->ambient = color * glm::vec3(ambient);
	this->diffuse = color * glm::vec3(diffuse);
	this->specular = color * glm::vec3(specular);
}

Light::LightType DirLight::getType() const{
	return LIGHT_DIRECTIONNAL;
}

DirLight::~DirLight() {}



