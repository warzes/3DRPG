#pragma once

#include "Graphics.h"
#include "Camera.h"

struct TransformUniformData final
{
	glm::aligned_mat4 model;
};

struct CameraUniformData final
{
	glm::aligned_mat4 view;
	glm::aligned_mat4 projection;
	glm::aligned_vec3 cameraPosition;
};

struct PointLightData final
{
	glm::aligned_vec3 position;
	glm::aligned_vec3 colour;
	glm::aligned_vec3 falloff;
	glm::aligned_i32  enable{ 0 };
};

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

	void SetModel(std::shared_ptr<Model0> model) { m_model = model; }
	std::shared_ptr<Model0> GetModel() const { return m_model; }

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
	std::shared_ptr<Model0> m_model;
	mutable glm::mat4      m_worldMatrix;
	Node*                  m_parent = nullptr;
	std::vector<Node*>     m_children;
};

constexpr const size_t MaxNumLight = 16;

class Scene final 
{
public:
	void Init();
	void Close();

	void AddCamera(const Camerao& camera);
	void AddNode(Node* node);
	void Render(const Camerao& camera, float screenAspect);

	void SetPointLight(size_t id, bool enable,
		const glm::vec3& position = glm::vec3(0.0f),
		const glm::vec3& colour = glm::vec3(1.0f),
		const glm::vec3& falloff = glm::vec3(0.0f));
private:
	bool isVisible(Node* node, const glm::mat4& viewProjectionMatrix) const;
	bool isSphereVisible(Node* node, const glm::mat4& viewProjectionMatrix) const;
	bool isAABBVisible(Node* node, const glm::mat4& viewProjectionMatrix) const;

	std::shared_ptr<ShaderProgram> m_geometryShader;

	std::vector<Node*>             m_nodes;
	TransformUniformData           m_uniformTransformData;
	std::shared_ptr<UniformBuffer> m_uniformTransformBuffer;
	CameraUniformData              m_uniformCameraData;
	std::shared_ptr<UniformBuffer> m_uniformCameraBuffer;

	std::array<PointLightData, MaxNumLight> m_uniformLightData;
	std::shared_ptr<UniformBuffer> m_uniformLightBuffer;

	std::shared_ptr<TextureCubeo>   m_tempSkyBoxMap;
};