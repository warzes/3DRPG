#pragma once

#include "Render.h"

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

class Mesh final
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material);
	void Draw();

private:
	VertexArray                   m_VAO;
	std::shared_ptr<VertexBuffer> m_vertexBuffer;
	std::shared_ptr<IndexBuffer>  m_indexBuffer;
	std::shared_ptr<Material>     m_material;
};

class Model final
{
public:
	Model(const std::string& path);
	void Draw();

private:
	void loadModel(const std::string& path);
	void processMesh(tinyobj::mesh_t mesh, const tinyobj::attrib_t& attrib, const tinyobj::material_t& mat);

	std::vector<Mesh> m_meshes;
	std::string       m_directory;
};