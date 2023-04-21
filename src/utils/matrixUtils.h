#pragma once

#include <glm/glm/glm.hpp>

inline float convolve3(glm::mat3 matrix1, glm::mat3 matrix2) {
	float result = matrix1[0].x * matrix2[2].z + matrix1[0].y * matrix2[2].y + matrix1[0].z * matrix2[2].x;
	result += matrix1[1].x * matrix2[1].z + matrix1[1].y * matrix2[1].y + matrix1[1].z * matrix2[1].x;
	result += matrix1[2].x * matrix2[0].z + matrix1[2].y * matrix2[0].y + matrix1[2].z * matrix2[0].x;
	return result;
}