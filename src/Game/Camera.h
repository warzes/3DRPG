#pragma once

#include "GeometryCore.h"

struct Camera final
{
	float     fov;
	float     near;
	float     far;
	float     aspectRatio;
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	glm::quat orientation;
	float     yaw;
	float     pitch;
	float     roll;

	glm::mat4 view;
	glm::mat3 model;
	glm::mat4 projection;
	glm::mat4 viewProjection;
	glm::mat4 prevViewProjection;
	glm::mat4 rotate;
	glm::mat4 translate;

	Plane nearPlane;
	Plane farPlane;
	Plane leftPlane;
	Plane rightPlane;
	Plane topPlane;
	Plane bottomPlane;

	Frustum frustum;

	Camera(float fov, float near, float far, float aspectRatio, glm::vec3 position, glm::vec3 forward);
	void SetTranslationDelta(glm::vec3 direction, float amount);
	void SetRotationDelta(glm::vec3 angles);
	void SetPosition(glm::vec3 position);
	void Update();
	void UpdateFromFrame(glm::vec3 position, glm::vec3 forward, glm::vec3 right);
	void UpdateProjection(float fov, float near, float far, float aspectRatio);
};

enum class CameraMovement : uint8_t
{
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

constexpr const float YAW = -90.0f;
constexpr const float PITCH = 0.0f;

class Camerao final
{
	const float SPEED = 10.5f;
	const float SENSITIVITY = 0.25f;
	const float ZOOM = 65.0f;
public:
	Camerao(
		const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), 
		const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix(float aspect) const;
	glm::vec3 GetPosition() const { return m_position; }

	void SetSpeed(float val);

	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessKeyboard(CameraMovement direction, float deltaTime);

private:
	void updateCameraVectors();

	// Camera Attributes
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_worldUp;
	// Eular Angles
	float     m_yaw;
	float     m_pitch;
	// Camera options
	float     m_movementSpeed;
	float     m_mouseSensitivity;
	float     m_zoom;
};