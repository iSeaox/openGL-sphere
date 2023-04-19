#pragma once

#include <glm/glm/glm.hpp>

inline glm::vec3 getColorFrom(uint8_t r, uint8_t g, uint8_t b) {
	return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}