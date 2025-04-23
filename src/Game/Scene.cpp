#include "stdafx.h"
#include "Scene.h"
#include "Core.h"
#include "SceneShader.h"
//=============================================================================
void Scene::Init()
{
	m_geometryShader = std::make_shared<ShaderProgram>(vertexShaderSource, fragmentShaderSource);

	m_uniformTransformBuffer = std::make_shared<UniformBuffer>(0, sizeof(TransformUniformData));
	m_uniformCameraBuffer = std::make_shared<UniformBuffer>(1, sizeof(CameraUniformData));
	m_uniformLightBuffer = std::make_shared<UniformBuffer>(2, sizeof(PointLightData) * MaxNumLight);

	m_tempSkyBoxMap = TextureCubeo::LoadFromFiles({ "data/Cathedral/textures/SkyBox.ktx" });
}
//=============================================================================
void Scene::Close()
{
	m_tempSkyBoxMap.reset();
	m_geometryShader.reset();
}
//=============================================================================
void Scene::AddNode(Node* node)
{
	m_nodes.push_back(node);
}
//=============================================================================
void Scene::Render(const Camerao& camera, float screenAspect)
{
	assert(m_uniformTransformBuffer);
	assert(m_uniformCameraBuffer);
	assert(m_uniformLightBuffer);

	m_geometryShader->Bind();
	m_geometryShader->SetUniform1i("iNumPointLights", MaxNumLight); // Set number of lights

	m_uniformLightBuffer->SetData(m_uniformLightData.data());

	m_uniformCameraData.projection = camera.GetProjectionMatrix(screenAspect);
	m_uniformCameraData.view = camera.GetViewMatrix();
	m_uniformCameraData.cameraPosition = camera.GetPosition();
	m_uniformCameraBuffer->SetData(&m_uniformCameraData);

	glm::mat4 viewProjectionMatrix = m_uniformCameraData.projection * m_uniformCameraData.view;

	for (auto node : m_nodes)
	{
		node->UpdateWorldMatrix();
		if (isVisible(node, viewProjectionMatrix))
		{
			auto model = node->GetModel();
			if (model)
			{
				for (size_t i = 0; i < model->GetNumMesh(); i++)
				{
					m_uniformTransformData.model = node->GetWorldMatrix() * model->GetMesh(i).GetLocalTransform();
					m_uniformTransformBuffer->SetData(&m_uniformTransformData);

					m_geometryShader->SetUniform1f("EmissivePower", model->GetMesh(i).GetMaterial()->emissivePower);

					m_geometryShader->FragmentSubRoutines(model->GetMesh(i).GetMaterial()->transparent ? 1 : 0);

					if (model->GetMesh(i).GetMaterial()->transparent)
						m_tempSkyBoxMap->Bind(3);
					else
						glBindTextureUnit(3, 0);

					model->DrawMesh(i);
				}
			}
		}
	}
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
void Scene::SetPointLight(size_t id, bool enable, const glm::vec3& position, const glm::vec3& colour, const glm::vec3& falloff)
{
	if (id < MaxNumLight)
	{
		if (enable)
		{
			m_uniformLightData[id].position = position;
			m_uniformLightData[id].colour = colour;
			m_uniformLightData[id].falloff = falloff;
			m_uniformLightData[id].enable = 1;
		}
		else
		{
			m_uniformLightData[id].enable = 0;
		}
	}
	else
	{
		Warning("Max Light 16 - " + std::to_string(id));
	}
}
//=============================================================================