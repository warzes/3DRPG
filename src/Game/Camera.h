#pragma once

#include "GeometryCore.h"
#include "Frustum.h"

enum class CameraType : uint8_t
{
	Perspective,
	Orthographic
};

class Camera final
{
public:
	Camera(CameraType type = CameraType::Perspective);

	void Init(float FOV, float width, float height, float nearplane, float farPlane);

	void SetPosition(const glm::vec3& position);
	void LookAt(const glm::vec3& position);
	void SetDirection(const glm::vec3& direction) { m_direction = direction; };

	const glm::vec3& GetPosition() const { return m_position; };
	const glm::vec3& GetDirection() const { return m_direction; };
	const glm::vec3& GetLookpoint() const { return m_lookPoint; };

	glm::vec3* GetPositionAddr() { return &m_position; };
	glm::vec3* GetDirectionAddr() { return &m_direction; };
	glm::vec3* GetLookpointAddr() { return &m_lookPoint; };

	const glm::mat4& GetViewMatrix() const { return m_viewMatrix; };
	const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; };
	const glm::mat4& GetProjectionViewMatrix() const { return m_projectionViewMatrix; };

	const glm::mat4* GetViewMatrixAddr() { return &m_viewMatrix; };
	const glm::mat4* GetProjectionMatrixAddr() { return &m_projectionMatrix; };
	const glm::mat4* GetProjectionViewMatrixAddr() { return &m_projectionViewMatrix; };

	void UpdateFrustum();
	Frustum GetFrustum();

	void UpdateViewMatrix();
	void UpdateProjectionMatrix();
	const glm::mat4 UpdateProjectionViewMatrix();
	const glm::mat4* GetProjectionViewMatrixAddr() const { return &m_projectionViewMatrix; };

	void ToggleMouseFollow() { m_followMouse = !m_followMouse; };
	void SetMouseFollow(bool enable) { m_followMouse = enable; };
	bool GetMouseFollow() { return m_followMouse; };

	void ToggleForceLookAtPoint() { m_forceLookAtpoint = !m_forceLookAtpoint; };
	void SetForceLookAtPoint(bool enable) { m_forceLookAtpoint = enable; };

	void Move(glm::vec3 velocity, double deltatime);

	void SetFrameBufferSize(float width, float height);

private:
	glm::mat4  m_projectionMatrix;
	glm::mat4  m_viewMatrix;
	glm::mat4  m_projectionViewMatrix;

	float      m_FOV{ 1.0f };
	float      m_nearPlane{ 0.1f };
	float      m_farPlane{ 1000.f };
	float      m_sensitivity{ 1.5f };
	bool       m_followMouse{ true };
	bool       m_forceLookAtpoint{ true };
	glm::vec3  m_position = glm::vec3(0.0f);
	glm::vec3  m_lookPoint = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3  m_direction;
	float      m_width{ 1600.0f };
	float      m_height{ 900.0f };

	Frustum    m_frustum;
	CameraType m_type = CameraType::Perspective;
};

struct Camerao2 final
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

	FrustumO2 frustum;

	Camerao2(float fov, float near, float far, float aspectRatio, glm::vec3 position, glm::vec3 forward);
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