#pragma once

#include "Render.h"

void ClearDefaultGraphicsResource();

class Material final
{
public:
	Material(std::shared_ptr<Texture2D> diffuseTexture) : diffuse(diffuseTexture) {}

	std::shared_ptr<Texture2D> diffuse;
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

	static std::shared_ptr<Model> CreateCube(float length = 1.0f);
	static std::shared_ptr<Model> CreateSphere(float radius, int sectors, int stacks);
	static std::shared_ptr<Model> CreatePlane(float width, float height, float texWidth, float texHeight);

private:
	void loadModel(const std::string& path, std::shared_ptr<Material> customMainMaterial);
	void processMesh(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attrib, std::shared_ptr<Material> material);

	std::vector<Mesh> m_meshes;
};