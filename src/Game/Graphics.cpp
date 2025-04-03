#include "stdafx.h"
#include "Graphics.h"
#include "CoreApp.h"
//=============================================================================
namespace
{
	std::shared_ptr<Material> DefaultMeshMaterial;
}
//=============================================================================
void ClearDefaultGraphicsResource()
{
	DefaultMeshMaterial.reset();
}
//=============================================================================
std::shared_ptr<Material> GetDefaultMeshMaterial()
{
	if (!DefaultMeshMaterial)
	{
		uint8_t defColor[] =
		{
			64,  64,  64,  255,
			200, 128, 128, 255,
			200, 128, 128, 255,
			64,  64,  64,  255,
		};

		std::shared_ptr<Texture2D> texture = Texture2D::LoadFromMemory(2, 2, defColor);
		DefaultMeshMaterial = std::make_shared<Material>(texture);
	}

	return DefaultMeshMaterial;
}
//=============================================================================
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material)
	: m_material(material)
{
	m_vertexBuffer = std::make_shared<VertexBuffer>(vertices.size() * sizeof(Vertex), vertices.data());
	m_indexBuffer = std::make_shared<IndexBuffer>(indices.size(), indices.data());

	VertexBufferLayout layout;
	layout.Push<glm::vec3>("aPosition");
	layout.Push<glm::vec3>("aNormal");
	layout.Push<glm::vec2>("aTexCoords");

	m_VAO = std::make_shared<VertexArray>();
	m_VAO->AddBuffer(m_vertexBuffer, m_indexBuffer, layout);
}
//=============================================================================
void Mesh::Draw()
{
	m_material->diffuse->Bind();
	glBindVertexArray(m_VAO->GetID());
	glDrawElements(GL_TRIANGLES, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
}
//=============================================================================
Model::Model(const std::string& path, std::shared_ptr<Material> customMainMaterial)
{
	loadModel(path, customMainMaterial);
}
//=============================================================================
void Model::Draw()
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Draw();
	}
}
//=============================================================================
void Model::loadModel(const std::string& path, std::shared_ptr<Material> customMainMaterial)
{
	m_directory = path.substr(0, path.find_last_of('/'));

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), m_directory.c_str()))
	{
		Fatal(warn + err);
		return;
	}

	for (const auto& shape : shapes)
	{
		std::shared_ptr<Material> material;
		if (customMainMaterial)
		{
			material = customMainMaterial;
		}
		else
		{
			if (materials.size() > 0)
				material = std::make_shared<Material>(Texture2D::LoadFromFile(m_directory + "/" + materials[shape.mesh.material_ids[0]].diffuse_texname));
			else
				material = GetDefaultMeshMaterial();
		}
		processMesh(shape.mesh, attrib, material);
	}
}
//=============================================================================
void Model::processMesh(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attrib, std::shared_ptr<Material> material)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh.indices.size(); i++)
	{
		const tinyobj::index_t& index = mesh.indices[i];
		
		Vertex vertex;
		vertex.Position = glm::vec3(
			attrib.vertices[3 * index.vertex_index + 0], 
			attrib.vertices[3 * index.vertex_index + 1], 
			attrib.vertices[3 * index.vertex_index + 2]);
		vertex.Normal = glm::vec3(
			attrib.normals[3 * index.normal_index + 0], 
			attrib.normals[3 * index.normal_index + 1], 
			attrib.normals[3 * index.normal_index + 2]);
		vertex.TexCoords = glm::vec2(
			attrib.texcoords[2 * index.texcoord_index + 0], 
			1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
		
		vertices.push_back(vertex);
		indices.push_back(i);
	}

	m_meshes.push_back(Mesh(vertices, indices, material));
}
//=============================================================================