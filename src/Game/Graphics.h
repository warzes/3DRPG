#pragma once

#include "Render.h"

void ClearDefaultGraphicsResource();

struct Vertex final
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Material final
{
public:
	Material(std::shared_ptr<Texture2D> diffuseTexture) : diffuse(diffuseTexture) {}

	std::shared_ptr<Texture2D> diffuse;
};

std::shared_ptr<Material> GetDefaultMeshMaterial();

class Mesh final
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material);
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
	Model(const std::string& path, std::shared_ptr<Material> customMainMaterial = nullptr);
	void Draw();

private:
	void loadModel(const std::string& path, std::shared_ptr<Material> customMainMaterial);
	void processMesh(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attrib, std::shared_ptr<Material> material);

	std::vector<Mesh> m_meshes;
	std::string       m_directory;
};