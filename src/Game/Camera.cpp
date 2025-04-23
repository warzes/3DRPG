#include "stdafx.h"
#include "Camera.h"
//=============================================================================
#define PI 3.14159265359
//=============================================================================
Camera::Camera(float Fov, float Near, float Far, float AspectRatio, glm::vec3 Position, glm::vec3 Forward)
	: fov(Fov)
	, near(Near)
	, far(Far)
	, aspectRatio(AspectRatio)
	, position(Position)
{
	forward = glm::normalize(Forward);
	worldUp = glm::vec3(0, 1, 0);
	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::normalize(glm::cross(right, forward));

	roll = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;

	orientation = glm::quat();
	projection = glm::perspective(glm::radians(fov), aspectRatio, near, far);
	prevViewProjection = glm::mat4(1.0f);

	Update();
}
//=============================================================================
void Camera::UpdateProjection(float Fov, float Near, float Far, float AspectRatio)
{
	projection = glm::perspective(glm::radians(Fov), AspectRatio, Near, Far);
}
//=============================================================================
void Camera::SetTranslationDelta(glm::vec3 Direction, float Amount)
{
	position += Direction * Amount;
}
//=============================================================================
void Camera::SetRotationDelta(glm::vec3 Angles)
{
	yaw = glm::radians(Angles.y);
	pitch = glm::radians(Angles.x);
	roll = glm::radians(Angles.z);
}
//=============================================================================
void Camera::SetPosition(glm::vec3 Position) { position = Position; }
//=============================================================================
void Camera::Update()
{
	glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qRoll = glm::angleAxis(roll, glm::vec3(0.0f, 0.0f, 1.0f));

	orientation = qPitch * orientation;
	orientation = orientation * qYaw;
	orientation = qRoll * orientation;
	orientation = glm::normalize(orientation);

	rotate = glm::mat4_cast(orientation);
	forward = glm::conjugate(orientation) * glm::vec3(0.0f, 0.0f, -1.0f);

	right = glm::normalize(glm::cross(forward, worldUp));

	up = glm::normalize(glm::cross(right, forward));

	translate = glm::translate(glm::mat4(1.0f), -position);
	view = rotate * translate;
	prevViewProjection = viewProjection;
	viewProjection = projection * view;

	FrustumFromMatrix(frustum, viewProjection);
}
//=============================================================================
void Camera::UpdateFromFrame(glm::vec3 Position, glm::vec3 Forward, glm::vec3 Right)
{
	position = Position;
	forward = Forward;
	right = Right;

	up = glm::normalize(glm::cross(right, forward));

	view = glm::lookAt(position, position + forward * 10.0f, up);
	prevViewProjection = viewProjection;
	viewProjection = projection * view;

	FrustumFromMatrix(frustum, viewProjection);
}
//=============================================================================
Camerao::Camerao(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
	: m_position(position)
	, m_front(glm::vec3(0.0f, 0.0f, -1.0f))
	, m_up(up)
	, m_right(glm::vec3(1.0f, 0.0f, 0.0f))
	, m_worldUp(up)
	, m_yaw(yaw)
	, m_pitch(pitch)
	, m_movementSpeed(SPEED)
	, m_mouseSensitivity(SENSITIVITY)
	, m_zoom(ZOOM)
{
	updateCameraVectors();
}
//=============================================================================
glm::mat4 Camerao::GetViewMatrix() const
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}
//=============================================================================
glm::mat4 Camerao::GetProjectionMatrix(float aspect) const
{
	return glm::perspective(glm::radians(m_zoom), aspect, 0.1f, 1000.0f);
}
//=============================================================================
void Camerao::SetSpeed(float val)
{
	m_movementSpeed = val;
}
//=============================================================================
void Camerao::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= m_mouseSensitivity;
	yoffset *= m_mouseSensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	if (m_pitch > 89.0f) m_pitch = 89.0f;
	if (m_pitch < -89.0f) m_pitch = -89.0f;

	updateCameraVectors();
}
//=============================================================================
void Camerao::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
	float velocity = m_movementSpeed * deltaTime;
	if (direction == CameraMovement::Forward)  m_position += m_front * velocity;
	if (direction == CameraMovement::Backward) m_position -= m_front * velocity;
	if (direction == CameraMovement::Left)     m_position -= m_right * velocity;
	if (direction == CameraMovement::Right)    m_position += m_right * velocity;

	if (direction == CameraMovement::Up)       m_position += m_up * velocity;
	if (direction == CameraMovement::Down)     m_position -= m_up * velocity;
}
//=============================================================================
void Camerao::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}
//=============================================================================