#pragma once

#include "Graphics.h"

class Transform final
{
public:
	Transform() = default;
	glm::vec3 GetPosition() const { return m_position; }
	glm::quat GetRotation() const { return m_rotation; }
	glm::vec3 GetScale() const { return m_scale; }

	void SetPosition(const glm::vec3& position) { m_position = position; }
	void SetRotation(const glm::quat& rotation) { m_rotation = rotation; }
	void SetScale(const glm::vec3& scale) { m_scale = scale; }

	void Translate(const glm::vec3& translation) { m_position += translation; }

	// Вращение вокруг своей оси
	void Rotate(float angle, const glm::vec3& axis)
	{
		m_rotation = glm::normalize(glm::rotate(m_rotation, glm::radians(angle), axis));
	}

	// Вращение вокруг заданной точки
	void RotateAroundPoint(float angle, const glm::vec3& axis, const glm::vec3& point)
	{
		glm::quat rot = glm::angleAxis(glm::radians(angle), axis);
		m_position = point + rot * (m_position - point);
		m_rotation = glm::normalize(rot * m_rotation);
	}

	void Scale(const glm::vec3& scale) { m_scale *= scale; }

	glm::mat4 GetModelMatrix() const
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_position);
		model = model * glm::toMat4(m_rotation);
		model = glm::scale(model, m_scale);
		return model;
	}

private:
	glm::vec3 m_position = glm::vec3{ 0.0f };
	glm::quat m_rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 m_scale = glm::vec3{ 1.0f };
};

class Node final
{
public:
	Node() = default;

	void SetModel(std::shared_ptr<Model> model) { m_model = model; }
	std::shared_ptr<Model> GetModel() const { return m_model; }

	void AddChild(Node* child) { m_children.push_back(child); }
	const std::vector<Node*>& GetChildren() const { return m_children; }

	// TODO: не учитывается трасформа от предка
	Transform& GetTransform() { return m_transform; }
	const Transform& GetTransform() const { return m_transform; }

	glm::mat4 GetWorldMatrix() const
	{
		if (m_parent) return m_parent->GetWorldMatrix() * m_transform.GetModelMatrix();
		else return m_transform.GetModelMatrix();
	}

	void UpdateWorldMatrix()
	{
		m_worldMatrix = GetWorldMatrix();
		for (auto& child : m_children)
		{
			child->m_parent = this;
			child->UpdateWorldMatrix();
		}
	}

private:
	Transform              m_transform;
	std::shared_ptr<Model> m_model;
	mutable glm::mat4      m_worldMatrix;
	Node*                  m_parent = nullptr;
	std::vector<Node*>     m_children;
};

enum class Direction : uint8_t
{
	Forward,
	Backward,
	Left,
	Right
};

class Camera final
{
	const float SPEED = 10.5f;
	const float SENSITIVITY = 0.25f;
	const float ZOOM = 65.0f;
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix(float aspect) const;

	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessKeyboard(Direction direction, float deltaTime);

private:
	void updateCameraVectors();

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
};

class Scene final 
{
public:
	Scene() = default;

	void AddNode(Node* node);
	void Render(std::shared_ptr<ShaderProgram> shader, Camera& camera, float screenAspect);
	void SetModelLocation(unsigned int location);

private:
	bool isVisible(Node* node, const glm::mat4& viewProjectionMatrix) const;
	bool isSphereVisible(Node* node, const glm::mat4& viewProjectionMatrix) const;
	bool isAABBVisible(Node* node, const glm::mat4& viewProjectionMatrix) const;

	std::vector<Node*> m_nodes;
	unsigned int m_modelLocation;
};