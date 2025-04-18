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