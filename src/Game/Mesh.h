#pragma once

#include "RHIBuffer.h"
#include "RHIVertexArray.h"

class Material;

// Non-skeletal vertex structure.
struct Vertex final
{
	glm::vec4 position;
	glm::vec4 texCoord;
	glm::vec4 normal;
	glm::vec4 tangent;
	glm::vec4 bitangent;
};

// SubMesh structure. Currently limited to one Material.
struct SubMesh final
{
	std::string name;
	uint32_t    matIdx;
	uint32_t    indexCount;
	uint32_t    baseVertex;
	uint32_t    baseIndex;
	uint32_t    vertexCount;
	glm::vec3   maxExtents;
	glm::vec3   minExtents;
};

class Mesh final
{
public:
	using Ptr = std::shared_ptr<Mesh>;

	static bool IsLoaded(const std::string& name);

	// Static factory methods.
	static Mesh::Ptr Load(
		const std::string& path,
		bool               loadMaterials = true,
		bool               isOrcaMesh = false);
	// Custom factory method for creating a mesh from provided data.
	static Mesh::Ptr Load(
		const std::string&                     name,
		std::vector<Vertex>                    vertices,
		std::vector<uint32_t>                  indices,
		std::vector<SubMesh>                   subMeshes,
		std::vector<std::shared_ptr<Material>> materials,
		glm::vec3                              maxExtents,
		glm::vec3                              minExtents);

	bool SetSubmeshMaterial(std::string name, std::shared_ptr<Material> material);
	bool SetSubmeshMaterial(uint32_t meshIdx, std::shared_ptr<Material> material);
	void SetGlobalMaterial(std::shared_ptr<Material> material);

	auto VertexBuffer() { return m_vbo; }
	auto IndexBuffer() { return m_ibo; }
	auto MeshVertexArray() { return m_vao.get(); }

	auto Id() { return m_id; }
	const auto& Materials() { return m_materials; }
	const auto& SubMeshes() { return m_subMeshes; }
	const auto& Indices() { return m_indices; }
	const auto& Vertices() { return m_vertices; }
	auto& GetMaterial(uint32_t idx) { return m_materials[idx]; }
	const auto& MaxExtents() { return m_maxExtents; }
	const auto& MinExtents() { return m_minExtents; }
	~Mesh();

private:
	// Private constructor to prevent manual creation.
	Mesh();
	Mesh(
		const std::string& path,
		bool               loadMaterials,
		bool               isOrcaMesh);

	// Internal initialization methods.
	void createGpuObjects();

	void loadFromDisk(
		const std::string& path,
		bool               loadMaterials,
		bool               isOrcaMesh);

	// Mesh cache. Used to prevent multiple loads.
	static std::unordered_map<std::string, std::weak_ptr<Mesh>> m_cache;

	// Mesh geometry.
	uint32_t                               m_id = 0;
	std::vector<std::shared_ptr<Material>> m_materials;
	std::vector<Vertex>                    m_vertices;
	std::vector<uint32_t>                  m_indices;
	std::vector<SubMesh>                   m_subMeshes;
	glm::vec3                              m_maxExtents;
	glm::vec3                              m_minExtents;
	VertexArray::Ptr                       m_vao = nullptr;
	Buffer::Ptr                            m_vbo = nullptr;
	Buffer::Ptr                            m_ibo = nullptr;
};