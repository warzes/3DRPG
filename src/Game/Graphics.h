#pragma once

#include "Render.h"

void ClearDefaultGraphicsResource();

class Material final
{
public:
	Material(std::shared_ptr<Texture2D> DiffuseTexture, std::shared_ptr<Texture2D> SpecularTexture, std::shared_ptr<Texture2D> RoughnessTexture);

	// TODO: возможно слоты перенести в инициализацию материала
	void Bind(uint32_t diffuseTexSlot = 0, uint32_t specularTexSlot = 1, uint32_t roughnessTexSlot = 2);

	std::shared_ptr<Texture2D> diffuseTexture;
	std::shared_ptr<Texture2D> specularTexture;
	std::shared_ptr<Texture2D> roughnessTexture;
};

std::shared_ptr<Material> GetDefaultMeshMaterial();

struct MeshVertex final
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;

	inline static VertexBufferLayout GetLayout()
	{
		VertexBufferLayout layout;
		layout.Push<glm::vec3>("aPosition");
		layout.Push<glm::vec3>("aNormal");
		layout.Push<glm::vec2>("aTexCoords");
		return layout;
	}
};

class Mesh final
{
public:
	Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material);
	void Draw();

private:
	std::shared_ptr<VertexArray>  m_VAO;
	std::shared_ptr<VertexBuffer> m_vertexBuffer;
	std::shared_ptr<IndexBuffer>  m_indexBuffer;
	std::shared_ptr<Material>     m_material;
};

class Model final
{
public:
	Model(const std::vector<Mesh>& meshes);
	Model(const std::string& path, std::shared_ptr<Material> customMainMaterial = nullptr);
	void Draw();

	static std::shared_ptr<Model> CreateCube(float length = 1.0f, std::shared_ptr<Material> material = nullptr);
	static std::shared_ptr<Model> CreateSphere(float radius, uint32_t uiTessU, uint32_t uiTessV, std::shared_ptr<Material> material = nullptr);
	static std::shared_ptr<Model> CreatePlane(float width, float height, float texWidth, float texHeight, std::shared_ptr<Material> material = nullptr);

private:
	void loadModel(const std::string& path, std::shared_ptr<Material> customMainMaterial);

	void loadObjModel(const std::string& path, std::shared_ptr<Material> customMainMaterial);
	void processObjMesh(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attrib, std::shared_ptr<Material> material);

	void loadAssimpModel(const std::string& path, std::shared_ptr<Material> customMainMaterial);

	std::vector<Mesh> m_meshes;
};