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
			255, 150, 200, 255,
			255, 150, 200, 255,
			64,  64,  64,  255,
		};

		std::shared_ptr<Texture2D> texture = Texture2D::LoadFromMemory(2, 2, defColor);
		DefaultMeshMaterial = std::make_shared<Material>(texture);
	}

	return DefaultMeshMaterial;
}
//=============================================================================
Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material)
	: m_material(material)
{
	m_vertexBuffer = std::make_shared<VertexBuffer>(vertices.size() * sizeof(MeshVertex), vertices.data());
	m_indexBuffer = std::make_shared<IndexBuffer>(indices.size(), indices.data());
	m_VAO = std::make_shared<VertexArray>(m_vertexBuffer, m_indexBuffer, MeshVertex::GetLayout());
}
//=============================================================================
void Mesh::Draw()
{
	m_material->diffuse->Bind();
	m_VAO->Bind();
	glDrawElements(GL_TRIANGLES, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
}
//=============================================================================
Model::Model(const std::vector<Mesh>& meshes)
{
	m_meshes = meshes;
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
std::shared_ptr<Model> Model::CreateCube(float length)
{
	std::vector<MeshVertex> vertices = {
		// Передняя грань
		{glm::vec3(-0.5f, -0.5f,  0.5f) * length, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(0.5f, -0.5f,  0.5f) * length,  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(0.5f,  0.5f,  0.5f) * length,  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f) * length, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},

		// Задняя грань
		{glm::vec3(-0.5f, -0.5f, -0.5f) * length, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(0.5f, -0.5f, -0.5f) * length,  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(0.5f,  0.5f, -0.5f) * length,  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f) * length, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},

		// Верхняя грань
		{glm::vec3(-0.5f,  0.5f, -0.5f) * length, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(0.5f,  0.5f, -0.5f) * length,  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(0.5f,  0.5f,  0.5f) * length,  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f) * length, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

		// Нижняя грань
		{glm::vec3(-0.5f, -0.5f, -0.5f) * length, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(0.5f, -0.5f, -0.5f) * length,  glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(0.5f, -0.5f,  0.5f) * length,  glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f) * length, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

		// Левая грань
		{glm::vec3(-0.5f, -0.5f, -0.5f) * length, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f) * length, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f) * length, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f) * length, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},

		// Правая грань
		{glm::vec3(0.5f, -0.5f, -0.5f) * length,  glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		{glm::vec3(0.5f, -0.5f,  0.5f) * length,  glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
		{glm::vec3(0.5f,  0.5f,  0.5f) * length,  glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
		{glm::vec3(0.5f,  0.5f, -0.5f) * length,  glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)}
	};

	// Индексы для куба (по два треугольника на грань)
	std::vector<unsigned int> indices =
	{
		// Передняя грань
		0, 1, 2, 2, 3, 0,
		// Задняя грань
		6, 5, 4, 4, 7, 6,
		// Верхняя грань
		10, 9, 8, 8, 11, 10,
		// Нижняя грань
		12, 13, 14, 14, 15, 12,
		// Левая грань
		16, 17, 18, 18, 19, 16,
		// Правая грань
		22, 21, 20, 20, 23, 22
	};

	return std::make_shared<Model>(std::vector<Mesh>{ {vertices, indices, GetDefaultMeshMaterial()} });
}
//=============================================================================
std::shared_ptr<Model> Model::CreateSphere(float radius, int sectors, int stacks)
{
	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;

	float const R = 1.0f / (float)(sectors - 1);
	float const S = 1.0f / (float)(stacks - 1);

	for (int r = 0; r < stacks; r++)
	{
		for (int s = 0; s < sectors; s++)
		{
			float y = sin(-M_PI_2 + M_PI * r * S);
			float x = cos(2 * M_PI * s * R) * sin(M_PI * r * S);
			float z = sin(2 * M_PI * s * R) * sin(M_PI * r * S);

			glm::vec3 position = glm::vec3(x, y, z) * radius;
			glm::vec3 normal = glm::normalize(position);
			glm::vec2 texCoord = glm::vec2((float)s / sectors, (float)r / stacks);

			vertices.push_back({ position, normal, texCoord });
		}
	}

	// Индексы
	unsigned int k1, k2;
	for (int r = 0; r < stacks - 1; ++r)
	{
		for (int s = 0; s < sectors - 1; ++s)
		{
			k1 = r * sectors + s;
			k2 = k1 + sectors;

			if (r != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (r != (stacks - 2))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	return std::make_shared<Model>(std::vector<Mesh>{ {vertices, indices, GetDefaultMeshMaterial()} });
}
//=============================================================================
std::shared_ptr<Model> Model::CreatePlane(float width, float height, float texWidth, float texHeight)
{
	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;

	// Создаем четыре вершины для плоскости
	vertices.push_back({ { -width / 2.0f, 0.0f, -height / 2.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } });
	vertices.push_back({ {  width / 2.0f, 0.0f, -height / 2.0f }, { 0.0f, 1.0f, 0.0f }, { texWidth, 0.0f } });
	vertices.push_back({ {  width / 2.0f, 0.0f,  height / 2.0f }, { 0.0f, 1.0f, 0.0f }, { texWidth, texHeight } });
	vertices.push_back({ { -width / 2.0f, 0.0f,  height / 2.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, texHeight } });

	// Создаем два треугольника из этих четырех вершин
	indices = { 2, 1, 0, 0, 3, 2 };

	return std::make_shared<Model>(std::vector<Mesh>{ {vertices, indices, GetDefaultMeshMaterial()} });
}
//=============================================================================
void Model::loadModel(const std::string& path, std::shared_ptr<Material> customMainMaterial)
{
	std::string directory = path.substr(0, path.find_last_of('/'));

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), directory.c_str()))
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
				material = std::make_shared<Material>(Texture2D::LoadFromFile(directory + "/" + materials[shape.mesh.material_ids[0]].diffuse_texname));
			else
				material = GetDefaultMeshMaterial();
		}
		processMesh(shape.mesh, attrib, material);
	}
}
//=============================================================================
void Model::processMesh(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attrib, std::shared_ptr<Material> material)
{
	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh.indices.size(); i++)
	{
		const tinyobj::index_t& index = mesh.indices[i];
		
		MeshVertex vertex;
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