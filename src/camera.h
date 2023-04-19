#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 30.0f;
const float SENSITIVITY = 0.04f;
const float ZOOM = 70.0f;
const float MAX_ZOOM = 90.0f;

class Camera
{
public:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;

	float m_yaw;
	float m_pitch;

	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;

	Camera(glm::vec3 postion = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: m_front(glm::vec3(0.0f, 0.0f, -1.0f))
		, m_movementSpeed(SPEED)
		, m_mouseSensitivity(SENSITIVITY)
		, m_zoom(ZOOM)
	{
		m_position = postion;
		m_worldUp = up;
		m_yaw = yaw;
		m_pitch = pitch;
		updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		: m_front(glm::vec3(0.0f, 0.0f, -1.0f))
		, m_movementSpeed(SPEED)
		, m_mouseSensitivity(SENSITIVITY)
		, m_zoom(ZOOM)
	{
		m_position = glm::vec3(posX, posY, posZ);
		m_worldUp = glm::vec3(upX, upY, upZ);
		m_yaw = yaw;
		m_pitch = pitch;
		updateCameraVectors();
	}
	~Camera() {}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void processKeyboard(CameraMovement direction, float deltaTime) {
		float velocity = m_movementSpeed * deltaTime;
		if (direction == FORWARD) {
			/*m_position.x += m_front.x * velocity;
			m_position.z += m_front.z * velocity;*/
			m_position += m_front * velocity;
		}
		if (direction == BACKWARD) {
			/*m_position.x -= m_front.x * velocity;
			m_position.z -= m_front.z * velocity;*/
			m_position -= m_front * velocity;
		}
		if (direction == LEFT)
			m_position -= m_right * velocity;
		if (direction == RIGHT)
			m_position += m_right * velocity;
	}

	void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true) {
		xOffset *= m_mouseSensitivity;
		yOffset *= m_mouseSensitivity;

		m_yaw += xOffset;
		m_pitch += yOffset;

		if (constrainPitch)
		{
			if (m_pitch > 89.0f)
				m_pitch = 89.0f;
			if (m_pitch < -89.0f)
				m_pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void processMouseScroll(float yOffset) {
		m_zoom -= (float)yOffset;
		if (m_zoom < 1.0f)
			m_zoom = 1.0f;
		if (m_zoom > MAX_ZOOM)
			m_zoom = MAX_ZOOM;
	}

private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(front);

		m_right = glm::normalize(glm::cross(m_front, m_worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}
};