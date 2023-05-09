#include "src/main.h"
#include "src/shader.h"
#include "src/camera.h"
#include "src/material.h"
#include "src/color.h"
#include "src/blocks/cubeModel.h"
#include"src/blocks/block.h"
#include "src/light.h"
#include "src/world.h"
#include "src/world-generator/heightGenerator.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

GLFWwindow* loadGLFW(unsigned int wWidth, unsigned int wHeight);
void loadGLAD(unsigned int wWidth, unsigned int wHeight);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void update(GLFWwindow* window);

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

BlockLoader* blockLoader;
World* world;

float lastXMouse = WINDOW_WIDTH / 2.0f;
float lastYMouse = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {

	// --- init ---
	GLFWwindow* window = loadGLFW(WINDOW_WIDTH, WINDOW_HEIGHT);
	loadGLAD(WINDOW_WIDTH, WINDOW_HEIGHT);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	// --- shader ---
	Shader base_shader("./shaders/base_shader.vs", "./shaders/base_shader.fs");

	// --- Lights ---
	DirLight dirLight(getColorFrom(232, 221, 181), glm::vec3(-0.2f, -1.0f, -0.3f), 1.0f, 0.5f, 0.8f);
	base_shader.use();

	base_shader.setVec3("viewPos", camera.m_position);
	base_shader.setVec3("dirLight.direction", dirLight.direction);
	base_shader.setVec3("dirLight.ambient", dirLight.ambient);
	base_shader.setVec3("dirLight.diffuse", dirLight.diffuse);
	base_shader.setVec3("dirLight.specular", dirLight.specular);
	
	// --- World ---
	HeightGenerator heightGenerator(static_cast<unsigned int>(time(NULL)));
	heightGenerator.load();

	blockLoader = new BlockLoader();
	world = new World(blockLoader);
	world->gen(heightGenerator);

	blockLoader->pushMaterialOnShader(base_shader);

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// --- update ---
		update(window);

		glClearColor(0.3411f, 0.4588f, 0.5647f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// --- render ---
		base_shader.use();
		
		// - Projection matrix -
		glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 300.0f);
		base_shader.setMat4("projection", projection);

		// - View Matrix -
		glm::mat4 view = camera.getViewMatrix();
		base_shader.setMat4("view", view);

		int i = 0;
		for (Chunk* chunkPtr : world->getChunks()) {
			glBindVertexArray(chunkPtr->getChunkVAO());
			glDrawArraysInstanced(GL_TRIANGLES, 0, 36, chunkPtr->getNbDrawableBlock());
			glBindVertexArray(0);
			i++;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

GLFWwindow* loadGLFW(unsigned int wWidth, unsigned int wHeight) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, "Sphere", NULL, NULL);
	if (window == NULL) {
		std::cout << "<load> Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	std::cout << "[GLFW]<load> GLFW Window loaded with OpenGL-4.5" << std::endl;
	return window;
}

void loadGLAD(unsigned int wWidth, unsigned int wHeight) {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "<load> Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	std::cout << "[GLAD]<load> GLAD initialized" << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse) {
		lastXMouse = xpos;
		lastYMouse = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastXMouse;
	float yOffset = lastYMouse - ypos;

	lastXMouse = xpos;
	lastYMouse = ypos;

	camera.processMouseMovement(xOffset, yOffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {}

void update(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);
}