#include "stdafx.h"
#include "Scene.h"
//=============================================================================
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
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
glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}
//=============================================================================
glm::mat4 Camera::GetProjectionMatrix(float aspect) const
{
	return glm::perspective(glm::radians(m_zoom), aspect, 0.1f, 1000.0f);
}
//=============================================================================
void Camera::ProcessMouseMovement(float xoffset, float yoffset)
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
void Camera::ProcessKeyboard(Direction direction, float deltaTime)
{
	float velocity = m_movementSpeed * deltaTime;
	if (direction == Direction::Forward)  m_position += m_front * velocity;
	if (direction == Direction::Backward) m_position -= m_front * velocity;
	if (direction == Direction::Left)     m_position -= m_right * velocity;
	if (direction == Direction::Right)    m_position += m_right * velocity;
}
//=============================================================================
void Camera::updateCameraVectors()
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
void Scene::AddNode(Node* node)
{
	m_nodes.push_back(node);
}
//=============================================================================
void Scene::Render(Shader& shader, Camera& camera, float screenAspect)
{
	glm::mat4 viewProjectionMatrix = camera.GetProjectionMatrix(screenAspect) * camera.GetViewMatrix();
	shader.Bind();

	for (auto node : m_nodes)
	{
		node->UpdateWorldMatrix();
		if (isVisible(node, viewProjectionMatrix))
		{	
			glm::mat4 modelMatrix = node->GetWorldMatrix();
			glProgramUniformMatrix4fv(shader.GetID(), m_modelLocation, 1, GL_FALSE, &modelMatrix[0][0]);
			auto model = node->GetModel();
			if (model) model->Draw();
		}
	}
}
//=============================================================================
void Scene::SetModelLocation(unsigned int location)
{
	m_modelLocation = location;
}
//=============================================================================
bool Scene::isVisible(Node* node, const glm::mat4& viewProjectionMatrix) const
{
	if (!node->GetModel()) return false;
	return isSphereVisible(node, viewProjectionMatrix) || isAABBVisible(node, viewProjectionMatrix);
}
//=============================================================================
bool Scene::isSphereVisible(Node* node, const glm::mat4& viewProjectionMatrix) const
{
	return true; // TODO: пока не работает isSphereVisible и isAABBVisible

	// Предположим, что сфера охватывает всю модель
	glm::vec3 center = node->GetTransform().GetPosition();
	float radius = 1.0f; // Предположительный радиус модели

	glm::vec4 sphereCenter = viewProjectionMatrix * glm::vec4(center, 1.0f);
	if (std::abs(sphereCenter.x) > sphereCenter.w || std::abs(sphereCenter.y) > sphereCenter.w || std::abs(sphereCenter.z) > sphereCenter.w)
	{
		return false;
	}
	return true;
}
//=============================================================================
bool Scene::isAABBVisible(Node* node, const glm::mat4& viewProjectionMatrix) const
{
	return true; // TODO: пока не работает isSphereVisible и isAABBVisible

	// Предположим, что AABB охватывает всю модель
	glm::vec3 min = glm::vec3(-1.0f, -1.0f, -1.0f); // Минимальные координаты модели
	glm::vec3 max = glm::vec3(1.0f, 1.0f, 1.0f);   // Максимальные координаты модели

	glm::mat4 worldMatrix = node->GetWorldMatrix();
	glm::vec4 corners[] = 
	{
		worldMatrix * glm::vec4(min.x, min.y, min.z, 1.0f),
		worldMatrix * glm::vec4(max.x, min.y, min.z, 1.0f),
		worldMatrix * glm::vec4(min.x, max.y, min.z, 1.0f),
		worldMatrix * glm::vec4(max.x, max.y, min.z, 1.0f),
		worldMatrix * glm::vec4(min.x, min.y, max.z, 1.0f),
		worldMatrix * glm::vec4(max.x, min.y, max.z, 1.0f),
		worldMatrix * glm::vec4(min.x, max.y, max.z, 1.0f),
		worldMatrix * glm::vec4(max.x, max.y, max.z, 1.0f)
	};

	for (auto& corner : corners)
	{
		glm::vec4 transformedCorner = viewProjectionMatrix * corner;
		if (transformedCorner.x >= -transformedCorner.w && transformedCorner.x <= transformedCorner.w &&
			transformedCorner.y >= -transformedCorner.w && transformedCorner.y <= transformedCorner.w &&
			transformedCorner.z >= -transformedCorner.w && transformedCorner.z <= transformedCorner.w)
		{
			return true;
		}
	}
	return false;
}
//=============================================================================