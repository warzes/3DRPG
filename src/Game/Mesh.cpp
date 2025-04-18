#include "stdafx.h"
#include "Mesh.h"
#include "Material.h"
#include "Core.h"
//=============================================================================
std::unordered_map<std::string, std::weak_ptr<Mesh>> Mesh::m_cache;
//=============================================================================
// Assimp texture enum lookup table.
static const aiTextureType kTextureTypes[] = 
{
	aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_AMBIENT,
	aiTextureType_EMISSIVE, aiTextureType_HEIGHT, aiTextureType_NORMALS,
	aiTextureType_SHININESS, aiTextureType_OPACITY, aiTextureType_DISPLACEMENT,
	aiTextureType_LIGHTMAP, aiTextureType_REFLECTION
};
//=============================================================================
// Assimp texture enum string table.
static std::string kTextureTypeStrings[] = 
{
	"aiTextureType_DIFFUSE", "aiTextureType_SPECULAR", "aiTextureType_AMBIENT",
	"aiTextureType_EMISSIVE", "aiTextureType_HEIGHT", "aiTextureType_NORMALS",
	"aiTextureType_SHININESS", "aiTextureType_OPACITY", "aiTextureType_DISPLACEMENT",
	"aiTextureType_LIGHTMAP", "aiTextureType_REFLECTION"
};
//=============================================================================
std::string getGltfBaseColorTexturePath(aiMaterial* material)
{
	aiString path;
	aiReturn result = material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &path);

	if (result == aiReturn_FAILURE)
		return "";
	else
	{
		std::string cppStr = std::string(path.C_Str());
		if (cppStr == "")
			return "";
		return cppStr;
	}
}
//=============================================================================
std::string getGltfMetallicRoughnessTexturePath(aiMaterial* material)
{
	aiString path;
	aiReturn result = material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &path);

	if (result == aiReturn_FAILURE)
		return "";
	else
	{
		std::string cppStr = std::string(path.C_Str());

		if (cppStr == "")
			return "";

		return cppStr;
	}
}
//=============================================================================
std::string resolveRelativePath(const std::string& meshPath, const std::string& path, bool isGltf)
{
	if (isGltf)
		return GetFileDirectory(meshPath) + "/" + path;
	else
		return path;
}
//=============================================================================
static uint32_t gLastMeshIdx = 0;
//=============================================================================
// Assimp loader helper method declarations.
std::string assimpGetTexturePath(aiMaterial* material, aiTextureType textureType);
//=============================================================================
bool assimpDoesMaterialExist(std::vector<unsigned int>& materials,
	unsigned int& currentMaterial);
//=============================================================================
Mesh::Ptr Mesh::Load(const std::string& path, bool loadMaterials, bool isOrcaMesh)
{
	std::filesystem::path absoluteFilePath = std::filesystem::path(path);

	if (!absoluteFilePath.is_absolute())
		absoluteFilePath = std::filesystem::path(std::filesystem::current_path().string() + "/" + path);

	std::string absoluteFilePathStr = absoluteFilePath.string();

	if (m_cache.find(absoluteFilePathStr) == m_cache.end() || m_cache[absoluteFilePathStr].expired())
	{
		auto mesh = std::shared_ptr<Mesh>(new Mesh(
			absoluteFilePathStr,
			loadMaterials,
			isOrcaMesh));
		m_cache[absoluteFilePathStr] = mesh;
		return mesh;
	}
	else
	{
		auto ptr = m_cache[absoluteFilePathStr];
		return ptr.lock();
	}
}
//=============================================================================
Mesh::Ptr Mesh::Load(
	const std::string& name,
	std::vector<Vertex>                    vertices,
	std::vector<uint32_t>                  indices,
	std::vector<SubMesh>                   subMeshes,
	std::vector<std::shared_ptr<Material>> materials,
	glm::vec3                              maxExtents,
	glm::vec3                              minExtents)
{
	if (m_cache.find(name) == m_cache.end() || m_cache[name].expired())
	{
		auto mesh = std::shared_ptr<Mesh>(new Mesh());

		// Manually assign properties...
		mesh->m_vertices = vertices;
		mesh->m_materials = materials;
		mesh->m_indices = indices;
		mesh->m_subMeshes = subMeshes;
		mesh->m_maxExtents = maxExtents;
		mesh->m_minExtents = minExtents;

		// ...then manually call the method to create GPU objects.
		mesh->createGpuObjects();

		m_cache[name] = mesh;
		return mesh;
	}
	else
	{
		auto ptr = m_cache[name];
		return ptr.lock();
	}
}
//=============================================================================
bool Mesh::IsLoaded(const std::string& name)
{
	return m_cache.find(name) != m_cache.end();
}
//=============================================================================
void Mesh::loadFromDisk(
	const std::string& path,
	bool               loadMaterials,
	bool               isOrcaMesh)
{
	const aiScene* Scene;
	Assimp::Importer importer;
	Scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	bool        isGltf = false;
	std::string extension = GetFileExtension(path);

	if (extension == "gltf" || extension == "glb")
		isGltf = true;

	m_subMeshes.resize(Scene->mNumMeshes);

	// Temporary variables
	aiMaterial* tempMaterial;
	std::vector<uint32_t>                       processedMatId;
	std::unordered_map<uint32_t, Material::Ptr> matIdMapping;
	std::unordered_map<uint32_t, uint32_t>      localMatIdxMapping;

	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;

	// Iterate over submeshes and find materials
	for (int i = 0; i < m_subMeshes.size(); i++)
	{
		bool hasLeastOneTexture = false;

		m_subMeshes[i].name = std::string(Scene->mMeshes[i]->mName.C_Str());
		m_subMeshes[i].indexCount = Scene->mMeshes[i]->mNumFaces * 3;
		m_subMeshes[i].baseIndex = indexCount;
		m_subMeshes[i].baseVertex = vertexCount;
		m_subMeshes[i].vertexCount = Scene->mMeshes[i]->mNumVertices;

		vertexCount += Scene->mMeshes[i]->mNumVertices;
		indexCount += m_subMeshes[i].indexCount;

		if (loadMaterials)
		{
			std::vector<std::string> texturePaths;

			int32_t    albedoIdx = -1;
			int32_t    normalIdx = -1;
			glm::ivec2 roughnessIdx = glm::ivec2(-1);
			glm::ivec2 metallicIdx = glm::ivec2(-1);
			int32_t    emissiveIdx = -1;

			glm::vec4 albedoValue = glm::vec4(1.0f);
			float     roughnessValue = 1.0f;
			float     metallicValue = 0.0f;
			glm::vec3 emissiveValue = glm::vec3(0.0f);

			if (matIdMapping.find(Scene->mMeshes[i]->mMaterialIndex) == matIdMapping.end())
			{
				std::string currentMatName;

				tempMaterial = Scene->mMaterials[Scene->mMeshes[i]->mMaterialIndex];
				currentMatName = path + std::to_string(i);

				aiColor3D diffuse = aiColor3D(1.0f, 1.0f, 1.0f);
				bool      hasDiifuseVal = false;

				// If this is a GLTF, try to find the base color texture path
				if (isGltf)
				{
					std::string texturePath = getGltfBaseColorTexturePath(tempMaterial);

					if (!texturePath.empty())
					{
						albedoIdx = texturePaths.size();
						texturePaths.push_back(resolveRelativePath(path, texturePath, isGltf));
					}
				}
				else
				{
					// If not, try to find the Diffuse texture path
					std::string texturePath = assimpGetTexturePath(tempMaterial, aiTextureType_DIFFUSE);

					// If that doesn't exist, try to find Diffuse texture
					if (texturePath.empty())
						texturePath = assimpGetTexturePath(tempMaterial, aiTextureType_BASE_COLOR);

					if (!texturePath.empty())
					{
						albedoIdx = texturePaths.size();
						texturePaths.push_back(resolveRelativePath(path, texturePath, isGltf));
					}
				}

				if (albedoIdx == -1)
				{
					aiColor3D diffuse = aiColor3D(1.0f, 1.0f, 1.0f);
					float     alpha = 1.0f;

					// Try loading in a Diffuse material property
					if (tempMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) != AI_SUCCESS)
						tempMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, diffuse);

					tempMaterial->Get(AI_MATKEY_OPACITY, alpha);
#if defined(MATERIAL_LOG)
					printf("Albedo Color: %f, %f, %f \n", diffuse.r, diffuse.g, diffuse.b);
#endif

					albedoValue = glm::vec4(diffuse.r, diffuse.g, diffuse.b, alpha);
				}
				else
				{
#if defined(MATERIAL_LOG)
					printf("Albedo Path: %s \n", albedo_path.c_str());
#endif
					std::string texturePath = texturePaths[albedoIdx];

					std::replace(texturePath.begin(), texturePath.end(), '\\', '/');

					texturePaths[albedoIdx] = texturePath;
				}

				if (isOrcaMesh)
				{
					std::string roughnessMetallicPath = assimpGetTexturePath(tempMaterial, aiTextureType_SPECULAR);

					if (!roughnessMetallicPath.empty())
					{
#if defined(MATERIAL_LOG)
						printf("Roughness Metallic Path: %s \n", roughness_metallic_path.c_str());
#endif
						std::replace(roughnessMetallicPath.begin(), roughnessMetallicPath.end(), '\\', '/');

						roughnessIdx.x = texturePaths.size();
						roughnessIdx.y = 1;

						metallicIdx.x = texturePaths.size();
						metallicIdx.y = 2;

						texturePaths.push_back(resolveRelativePath(path, roughnessMetallicPath, isGltf));
					}
				}
				else
				{
					// Try to find Roughness texture
					std::string roughnessPath = assimpGetTexturePath(tempMaterial, aiTextureType_SHININESS);

					if (roughnessPath.empty())
						roughnessPath = getGltfMetallicRoughnessTexturePath(tempMaterial);

					if (roughnessPath.empty())
					{
						// Try loading in a Diffuse material property
						tempMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, roughnessValue);
#if defined(MATERIAL_LOG)
						printf("Roughness Color: %f \n", roughness);
#endif
					}
					else
					{
#if defined(MATERIAL_LOG)
						printf("Roughness Path: %s \n", roughness_path.c_str());
#endif
						std::replace(roughnessPath.begin(), roughnessPath.end(), '\\', '/');

						roughnessIdx.x = texturePaths.size();
						roughnessIdx.y = isGltf ? 1 : 0;

						texturePaths.push_back(resolveRelativePath(path, roughnessPath, isGltf));
					}

					// Try to find Metallic texture
					std::string metallicPath = assimpGetTexturePath(tempMaterial, aiTextureType_AMBIENT);

					if (metallicPath.empty())
						metallicPath = getGltfMetallicRoughnessTexturePath(tempMaterial);

					if (metallicPath.empty())
					{
						// Try loading in a Diffuse material property
						tempMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, metallicValue);
#if defined(MATERIAL_LOG)
						printf("Metallic Color: %f \n", metallic);
#endif
					}
					else
					{
#if defined(MATERIAL_LOG)
						printf("Metallic Path: %s \n", metallic_path.c_str());
#endif
						std::replace(metallicPath.begin(), metallicPath.end(), '\\', '/');

						metallicIdx.x = texturePaths.size();
						metallicIdx.y = isGltf ? 2 : 0;

						texturePaths.push_back(resolveRelativePath(path, metallicPath, isGltf));
					}
				}

				// Try to find Emissive texture
				std::string emissivePath = assimpGetTexturePath(tempMaterial, aiTextureType_EMISSIVE);

				if (emissivePath.empty())
				{
					aiColor3D emissive;

					// Try loading in a Emissive material property
					if (tempMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive))
					{
#if defined(MATERIAL_LOG)
						printf("Emissive Color: %f, %f, %f \n", emissive.r, emissive.g, emissive.b);
#endif
						emissiveValue.x = emissive.r;
						emissiveValue.y = emissive.g;
						emissiveValue.z = emissive.b;
					}
				}
				else
				{
#if defined(MATERIAL_LOG)
					printf("Emissive Path: %s \n", emissive_path.c_str());
#endif
					std::replace(emissivePath.begin(), emissivePath.end(), '\\', '/');

					emissiveIdx = texturePaths.size();
					texturePaths.push_back(resolveRelativePath(path, emissivePath, isGltf));
				}

				// Try to find Normal texture
				std::string normalPath = assimpGetTexturePath(tempMaterial, aiTextureType_NORMALS);

				if (normalPath.empty())
					normalPath = assimpGetTexturePath(tempMaterial, aiTextureType_HEIGHT);

				if (!normalPath.empty())
				{
#if defined(MATERIAL_LOG)
					printf("Normal Path: %s \n", normal_path.c_str());
#endif
					std::replace(normalPath.begin(), normalPath.end(), '\\', '/');

					normalIdx = texturePaths.size();
					texturePaths.push_back(resolveRelativePath(path, normalPath, isGltf));
				}

				Material::Ptr mat = Material::Load(
					texturePaths,
					albedoIdx,
					normalIdx,
					roughnessIdx,
					metallicIdx,
					emissiveIdx);

				mat->SetAlbedo(albedoValue);
				mat->SetRoughness(roughnessValue);
				mat->SetMetallic(metallicValue);
				mat->SetEmissive(emissiveValue);

				matIdMapping[Scene->mMeshes[i]->mMaterialIndex] = mat;
				localMatIdxMapping[Scene->mMeshes[i]->mMaterialIndex] = m_materials.size();

				m_subMeshes[i].matIdx = m_materials.size();

				m_materials.push_back(mat);
			}
			else // if already exists, find the pointer.
				m_subMeshes[i].matIdx = localMatIdxMapping[Scene->mMeshes[i]->mMaterialIndex];
		}
	}

	m_vertices.resize(vertexCount);
	m_indices.resize(indexCount);

	std::vector<uint32_t> tempIndices(indexCount);

	aiMesh* tempMesh;
	int     idx = 0;
	int     vertexIndex = 0;

	// Iterate over submeshes...
	for (int i = 0; i < m_subMeshes.size(); i++)
	{
		tempMesh = Scene->mMeshes[i];
		m_subMeshes[i].maxExtents = glm::vec3(tempMesh->mVertices[0].x, tempMesh->mVertices[0].y, tempMesh->mVertices[0].z);
		m_subMeshes[i].minExtents = glm::vec3(tempMesh->mVertices[0].x, tempMesh->mVertices[0].y, tempMesh->mVertices[0].z);

		uint32_t matId = 0;

		if (matIdMapping[Scene->mMeshes[i]->mMaterialIndex])
			matId = matIdMapping[Scene->mMeshes[i]->mMaterialIndex]->Id();
		matId = m_subMeshes[i].matIdx;

		// Iterate over vertices in submesh...
		for (int k = 0; k < Scene->mMeshes[i]->mNumVertices; k++)
		{
			// Assign vertex values.
			m_vertices[vertexIndex].position = glm::vec4(tempMesh->mVertices[k].x, tempMesh->mVertices[k].y, tempMesh->mVertices[k].z, float(matId));
			glm::vec3 n = glm::vec3(tempMesh->mNormals[k].x, tempMesh->mNormals[k].y, tempMesh->mNormals[k].z);
			m_vertices[vertexIndex].normal = glm::vec4(n, 0.0f);

			if (tempMesh->mTangents && tempMesh->mBitangents)
			{
				glm::vec3 t = glm::vec3(tempMesh->mTangents[k].x, tempMesh->mTangents[k].y, tempMesh->mTangents[k].z);
				glm::vec3 b = glm::vec3(tempMesh->mBitangents[k].x, tempMesh->mBitangents[k].y, tempMesh->mBitangents[k].z);

				// Assuming right handed coordinate space
				if (glm::dot(glm::cross(n, t), b) < 0.0f)
					t *= -1.0f; // Flip tangent

				m_vertices[vertexIndex].tangent = glm::vec4(t, 0.0f);
				m_vertices[vertexIndex].bitangent = glm::vec4(b, 0.0f);
			}

			// Find submesh bounding box extents.
			if (m_vertices[vertexIndex].position.x > m_subMeshes[i].maxExtents.x)
				m_subMeshes[i].maxExtents.x = m_vertices[vertexIndex].position.x;
			if (m_vertices[vertexIndex].position.y > m_subMeshes[i].maxExtents.y)
				m_subMeshes[i].maxExtents.y = m_vertices[vertexIndex].position.y;
			if (m_vertices[vertexIndex].position.z > m_subMeshes[i].maxExtents.z)
				m_subMeshes[i].maxExtents.z = m_vertices[vertexIndex].position.z;

			if (m_vertices[vertexIndex].position.x < m_subMeshes[i].minExtents.x)
				m_subMeshes[i].minExtents.x = m_vertices[vertexIndex].position.x;
			if (m_vertices[vertexIndex].position.y < m_subMeshes[i].minExtents.y)
				m_subMeshes[i].minExtents.y = m_vertices[vertexIndex].position.y;
			if (m_vertices[vertexIndex].position.z < m_subMeshes[i].minExtents.z)
				m_subMeshes[i].minExtents.z = m_vertices[vertexIndex].position.z;

			// Assign texture coordinates if it has any. Only the first channel is considered.
			if (tempMesh->HasTextureCoords(0))
				m_vertices[vertexIndex].texCoord = glm::vec4(tempMesh->mTextureCoords[0][k].x, tempMesh->mTextureCoords[0][k].y, 0.0f, 0.0f);

			vertexIndex++;
		}

		// Assign indices.
		for (int j = 0; j < tempMesh->mNumFaces; j++)
		{
			tempIndices[idx] = tempMesh->mFaces[j].mIndices[0];
			idx++;
			tempIndices[idx] = tempMesh->mFaces[j].mIndices[1];
			idx++;
			tempIndices[idx] = tempMesh->mFaces[j].mIndices[2];
			idx++;
		}
	}

	int count = 0;

	for (int i = 0; i < m_subMeshes.size(); i++)
	{
		SubMesh& submesh = m_subMeshes[i];

		for (int idx = submesh.baseIndex; idx < (submesh.baseIndex + submesh.indexCount); idx++)
			m_indices[count++] = submesh.baseVertex + tempIndices[idx];

		submesh.baseVertex = 0;
	}

	m_maxExtents = m_subMeshes[0].maxExtents;
	m_minExtents = m_subMeshes[0].minExtents;

	// Find bounding box extents of entire mesh.
	for (int i = 0; i < m_subMeshes.size(); i++)
	{
		if (m_subMeshes[i].maxExtents.x > m_maxExtents.x)
			m_maxExtents.x = m_subMeshes[i].maxExtents.x;
		if (m_subMeshes[i].maxExtents.y > m_maxExtents.y)
			m_maxExtents.y = m_subMeshes[i].maxExtents.y;
		if (m_subMeshes[i].maxExtents.z > m_maxExtents.z)
			m_maxExtents.z = m_subMeshes[i].maxExtents.z;

		if (m_subMeshes[i].minExtents.x < m_minExtents.x)
			m_minExtents.x = m_subMeshes[i].minExtents.x;
		if (m_subMeshes[i].minExtents.y < m_minExtents.y)
			m_minExtents.y = m_subMeshes[i].minExtents.y;
		if (m_subMeshes[i].minExtents.z < m_minExtents.z)
			m_minExtents.z = m_subMeshes[i].minExtents.z;
	}
}
//=============================================================================
void Mesh::createGpuObjects()
{
	// Create vertex buffer.
	m_vbo = Buffer::Create(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_vertices.size(), m_vertices.data());

	if (!m_vbo)
		Error("Failed to create Vertex Buffer");

	// Create index buffer.
	m_ibo = Buffer::Create(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * m_indices.size(), m_indices.data());

	if (!m_ibo)
		Error("Failed to create Index Buffer");

	// Declare vertex attributes.
	VertexAttrib attribs[] = { 
		{ 4, GL_FLOAT, false, 0 },
		{ 4, GL_FLOAT, false, offsetof(Vertex, texCoord) },
		{ 4, GL_FLOAT, false, offsetof(Vertex, normal) },
		{ 4, GL_FLOAT, false, offsetof(Vertex, tangent) },
		{ 4, GL_FLOAT, false, offsetof(Vertex, bitangent) } 
	};

	// Create vertex array.
	m_vao = VertexArray::Create(m_vbo, m_ibo, sizeof(Vertex), 5, attribs);

	if (!m_vao)
		Error("Failed to create Vertex Array");
}
//=============================================================================
Mesh::Mesh()
{
	m_id = gLastMeshIdx++;
}
//=============================================================================
Mesh::Mesh(const std::string& path, bool loadMaterials, bool isOrcaMesh)
{
	m_id = gLastMeshIdx++;

	loadFromDisk(
		path,
		loadMaterials,
		isOrcaMesh);
	createGpuObjects();
}
//=============================================================================
Mesh::~Mesh()
{
	// Unload submesh materials.
	for (uint32_t i = 0; i < m_materials.size(); i++)
		m_materials[i].reset();

	m_ibo.reset();
	m_vbo.reset();
}
//=============================================================================
bool Mesh::SetSubmeshMaterial(std::string name, std::shared_ptr<Material> material)
{
	for (int i = 0; i < m_subMeshes.size(); i++)
	{
		if (name == m_subMeshes[i].name)
		{
			m_subMeshes[i].matIdx = m_materials.size();
			m_materials.push_back(material);

			return true;
		}
	}

	return false;
}
//=============================================================================
bool Mesh::SetSubmeshMaterial(uint32_t meshIdx, std::shared_ptr<Material> material)
{
	if (meshIdx >= m_subMeshes.size())
		return false;

	m_subMeshes[meshIdx].matIdx = m_materials.size();
	m_materials.push_back(material);

	return true;
}
//=============================================================================
void Mesh::SetGlobalMaterial(std::shared_ptr<Material> material)
{
	for (int i = 0; i < m_subMeshes.size(); i++)
		m_subMeshes[i].matIdx = m_materials.size();

	m_materials.push_back(material);
}
//=============================================================================
// Assimp loader helper method definitions
std::string assimpGetTexturePath(aiMaterial* material, aiTextureType textureType)
{
	aiString path;
	aiReturn result = material->GetTexture(textureType, 0, &path);

	if (result == aiReturn_FAILURE)
		return "";
	else
	{
		std::string cppStr = std::string(path.C_Str());

		if (cppStr == "")
			return "";

		return cppStr;
	}
}
//=============================================================================
bool assimpDoesMaterialExist(std::vector<unsigned int>& materials, unsigned int& currentMaterial)
{
	for (auto it : materials)
	{
		if (it == currentMaterial)
			return true;
	}

	return false;
}
//=============================================================================