#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in mat4 aInstanceModelMatrix;

out vec3 normal;
out vec3 fragPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
	gl_Position = projection * view * aInstanceModelMatrix * vec4(aPos, 1.0);
	fragPos = vec3(aInstanceModelMatrix * vec4(aPos, 1.0));
	normal = mat3(transpose(inverse(aInstanceModelMatrix))) * aNormal;
}