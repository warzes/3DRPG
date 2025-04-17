#pragma once

#include "Render.h"

void ClearDefaultGraphicsResource();

class Materialo final
{
public:
	Materialo(
		std::shared_ptr<Texture2Do> DiffuseTexture, 
		std::shared_ptr<Texture2Do> SpecularTexture, 
		std::shared_ptr<Texture2Do> RoughnessTexture,
		float EmissivePower = 0.0f);

	// TODO: возможно слоты перенести в инициализацию материала
	void Bind(uint32_t diffuseTexSlot = 0, uint32_t specularTexSlot = 1, uint32_t roughnessTexSlot = 2);

	std::shared_ptr<Texture2Do> diffuseTexture;
	std::shared_ptr<Texture2Do> specularTexture;
	std::shared_ptr<Texture2Do> roughnessTexture;
	float                      emissivePower;
	bool                       transparent;
	bool                       reflective{ false };
};

std::shared_ptr<Materialo> GetDefaultMeshMaterial();

struct MeshVertex0 final
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

class Mesh0 final
{
public:
	Mesh0(const std::vector<MeshVertex0>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Materialo> material, const glm::mat4& localTransform);
	void Draw();

	const glm::mat4& GetLocalTransform() const { return m_localTransform; }
	std::shared_ptr<Materialo> GetMaterial() const { return m_material; }

private:
	std::shared_ptr<VertexArrayo>  m_VAO;
	std::shared_ptr<VertexBuffer> m_vertexBuffer;
	std::shared_ptr<IndexBuffer>  m_indexBuffer;
	std::shared_ptr<Materialo>     m_material;
	glm::mat4                     m_localTransform = glm::mat4(1.0f);
};

class Model0 final
{
public:
	Model0(const std::vector<Mesh0>& meshes);
	Model0(const std::string& path, std::shared_ptr<Materialo> customMainMaterial = nullptr);
	void Draw();
	void DrawMesh(size_t i);

	size_t GetNumMesh() const { return m_meshes.size(); }
	const Mesh0& GetMesh(size_t i) const { return m_meshes[i]; }

	static std::shared_ptr<Model0> CreateCube(float length = 1.0f, std::shared_ptr<Materialo> material = nullptr);
	static std::shared_ptr<Model0> CreateSphere(float radius, uint32_t uiTessU, uint32_t uiTessV, std::shared_ptr<Materialo> material = nullptr);
	static std::shared_ptr<Model0> CreatePlane(float width, float height, float texWidth, float texHeight, std::shared_ptr<Materialo> material = nullptr);

private:
	void loadModel(const std::string& path, std::shared_ptr<Materialo> customMainMaterial);

	void loadObjModel(const std::string& path, std::shared_ptr<Materialo> customMainMaterial);
	void processObjMesh(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attrib, std::shared_ptr<Materialo> material);

	void loadAssimpModel(const std::string& path, std::shared_ptr<Materialo> customMainMaterial);
	void processAssimpNode(const std::string& directoryModel, aiNode* node, const aiScene* scene, std::shared_ptr<Materialo> material, std::vector<Mesh0>& transMesh, std::vector<Mesh0>& solidMesh);
	Mesh0 processAssimpMesh(const std::string& directoryModel, const glm::mat4& localMat, aiMesh* mesh, const aiScene* scene, std::shared_ptr<Materialo> material);
	std::shared_ptr<Texture2Do> loadAssimpTexture(const std::string& directoryModel, aiMaterial* mat, aiTextureType type);

	std::vector<Mesh0> m_meshes;
};