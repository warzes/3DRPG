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