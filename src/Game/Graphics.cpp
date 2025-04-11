#include "stdafx.h"
#include "Graphics.h"
#include "CoreApp.h"
#include "Utility.h"
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
		std::shared_ptr<Texture2D> diffuseTexture = Texture2D::LoadFromMemory(2, 2, defColor);

		uint8_t specColor[] =
		{
			255, 255, 255, 255,
			255, 255, 255, 255,
			255, 255, 255, 255,
			255, 255, 255, 255,
		};
		std::shared_ptr<Texture2D> specularTexture = Texture2D::LoadFromMemory(2, 2, specColor);

		uint8_t roughColor[] =
		{
			128, 128, 128, 255,
			128, 128, 128, 255,
			128, 128, 128, 255,
			128, 128, 128, 255,
		};
		std::shared_ptr<Texture2D> roughnessTexture = Texture2D::LoadFromMemory(2, 2, roughColor);

		DefaultMeshMaterial = std::make_shared<Material>(diffuseTexture, specularTexture, roughnessTexture);
	}

	return DefaultMeshMaterial;
}
//=============================================================================
Material::Material(
	std::shared_ptr<Texture2D> DiffuseTexture,
	std::shared_ptr<Texture2D> SpecularTexture,
	std::shared_ptr<Texture2D> RoughnessTexture)
	: diffuseTexture(DiffuseTexture)
	, specularTexture(SpecularTexture)
	, roughnessTexture(RoughnessTexture)
{
	// TODO: если нет нужных текстур, брать дефолтные
	if (!diffuseTexture || !specularTexture || !roughnessTexture)
	{
		auto defaultMat = GetDefaultMeshMaterial();
		if (!diffuseTexture) diffuseTexture = defaultMat->diffuseTexture;
		if (!specularTexture) specularTexture = defaultMat->specularTexture;
		if (!roughnessTexture) roughnessTexture = defaultMat->roughnessTexture;
	}
}
//=============================================================================
void Material::Bind(uint32_t diffuseTexSlot, uint32_t specularTexSlot, uint32_t roughnessTexSlot)
{
	diffuseTexture->Bind(diffuseTexSlot);
	specularTexture->Bind(specularTexSlot);
	roughnessTexture->Bind(roughnessTexSlot);
}
//=============================================================================
Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Material> material, const glm::mat4& localTransform)
	: m_material(material)
	, m_localTransform(localTransform)
{
	if (!m_material) m_material = GetDefaultMeshMaterial();
	m_vertexBuffer = std::make_shared<VertexBuffer>(vertices.size() * sizeof(MeshVertex), vertices.data());
	m_indexBuffer = std::make_shared<IndexBuffer>(indices.size(), indices.data());
	m_VAO = std::make_shared<VertexArray>(m_vertexBuffer, m_indexBuffer, MeshVertex::GetLayout());
}
//=============================================================================
void Mesh::Draw()
{
	m_material->Bind();
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
void Model::DrawMesh(size_t i)
{
	m_meshes[i].Draw();
}
//=============================================================================
std::shared_ptr<Model> Model::CreateCube(float length, std::shared_ptr<Material> material)
{
	std::vector<MeshVertex> vertices = {
		// Create back face
		{ glm::vec3( 0.5f,  0.5f, -0.5f) * length, glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3( 0.5f, -0.5f, -0.5f) * length, glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.5f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f) * length, glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.5f, 0.5f) },
		{ glm::vec3(-0.5f,  0.5f, -0.5f) * length, glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.5f, 1.0f) },
		// Create left face
		{ glm::vec3(-0.5f,  0.5f, -0.5f) * length, glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.5f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f) * length, glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.5f, 0.5f) },
		{ glm::vec3(-0.5f, -0.5f,  0.5f) * length, glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.5f) },
		{ glm::vec3(-0.5f,  0.5f,  0.5f) * length, glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
		// Create bottom face
		{ glm::vec3( 0.5f, -0.5f, -0.5f) * length, glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.5f, 0.0f) },
		{ glm::vec3( 0.5f, -0.5f,  0.5f) * length, glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.5f, 0.5f) },
		{ glm::vec3(-0.5f, -0.5f,  0.5f) * length, glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.5f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f) * length, glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
		// Create front face
		{ glm::vec3(-0.5f,  0.5f,  0.5f) * length, glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f,  0.5f) * length, glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.5f) },
		{ glm::vec3( 0.5f, -0.5f,  0.5f) * length, glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.5f, 0.5f) },
		{ glm::vec3( 0.5f,  0.5f,  0.5f) * length, glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.5f, 1.0f) },
		// Create right face
		{ glm::vec3( 0.5f,  0.5f,  0.5f) * length, glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.5f, 1.0f) },
		{ glm::vec3( 0.5f, -0.5f,  0.5f) * length, glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.5f, 0.5f) },
		{ glm::vec3( 0.5f, -0.5f, -0.5f) * length, glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.5f) },
		{ glm::vec3( 0.5f,  0.5f, -0.5f) * length, glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
		// Create top face
		{ glm::vec3( 0.5f,  0.5f,  0.5f) * length, glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.5f) },
		{ glm::vec3( 0.5f,  0.5f, -0.5f) * length, glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-0.5f,  0.5f, -0.5f) * length, glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.5f, 0.0f) },
		{ glm::vec3(-0.5f,  0.5f,  0.5f) * length, glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.5f, 0.5f) },
	};

	// Индексы для куба (по два треугольника на грань)
	std::vector<unsigned int> indices =
	{
		// Create back face
		0,  1,  3,  3,  1,  2,
		// Create left face
		4,  5,  7,  7,  5,  6,
		// Create bottom face
		8,  9, 11, 11,  9, 10,
		// Create front face
		12, 13, 15, 15, 13, 14,
		// Create right face
		16, 17, 19, 19, 17, 18,
		// Create top face
		20, 21, 23, 23, 21, 22
	};

	return std::make_shared<Model>(std::vector<Mesh>{ {vertices, indices, material, glm::mat4(1.0f)} });
}
//=============================================================================
std::shared_ptr<Model> Model::CreateSphere(float radius, uint32_t uiTessU, uint32_t uiTessV, std::shared_ptr<Material> material)
{
	// Init params
	float fDPhi = (float)M_PI / (float)uiTessV;
	float fDTheta = (float)(M_PI + M_PI) / (float)uiTessU;

	// Determine required parameters
	uiTessU = uiTessU + 1;
	uint32_t uiNumVertices = (uiTessU * (uiTessV + 1));
	uint32_t uiNumIndices = uiTessU * uiTessV * 6;

	std::vector<MeshVertex> vertices(uiNumVertices);
	std::vector<unsigned int> indices(uiNumIndices);

	auto* vBuffer = &vertices[0];

	float fPhi = 0.0f;
	for (uint32_t uiPhi = 0; uiPhi < uiTessV + 1; uiPhi++)
	{
		// Calculate initial value
		float fRSinPhi = sinf(fPhi);
		float fRCosPhi = cosf(fPhi);

		float fY = fRCosPhi;

		float fTheta = 0.0f;
		for (uint32_t uiTheta = 0; uiTheta < uiTessU; uiTheta++)
		{
			// Calculate positions
			float fCosTheta = cosf(fTheta);
			float fSinTheta = sinf(fTheta);

			// Determine position
			float fX = fRSinPhi * fCosTheta;
			float fZ = fRSinPhi * fSinTheta;

			// Create vertex
			vBuffer->Position = glm::vec3(fX, fY, fZ) * radius;
			vBuffer->Normal = glm::vec3(fX, fY, fZ);
			vBuffer->TexCoords= glm::vec2(1.0f - (fTheta / (float)(M_PI + M_PI)),
				1.0f - (fPhi / (float)M_PI));
			vBuffer++;
			fTheta += fDTheta;
		}
		fPhi += fDPhi;
	}

	auto* iBuffer = &indices[0];

	for (GLuint i = 0; i < uiTessV; i++)
	{
		for (GLuint j = 0; j < uiTessU; j++)
		{
			// Create indexes for each quad face (pair of triangles)
			*iBuffer++ = j + (i * uiTessU);
			unsigned int Index = j + 1;
			*iBuffer++ = Index + (i * uiTessU);
			*iBuffer++ = j + ((i + 1) * uiTessU);

			*iBuffer = *(iBuffer - 2);
			iBuffer++;
			*iBuffer++ = Index + ((i + 1) * uiTessU);
			*iBuffer = *(iBuffer - 3);
			iBuffer++;
		}
	}

	return std::make_shared<Model>(std::vector<Mesh>{ {vertices, indices, material, glm::mat4(1.0f)} });
}
//=============================================================================
std::shared_ptr<Model> Model::CreatePlane(float width, float height, float texWidth, float texHeight, std::shared_ptr<Material> material)
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

	return std::make_shared<Model>(std::vector<Mesh>{ {vertices, indices, material, glm::mat4(1.0f)} });
}
//=============================================================================
void Model::loadModel(const std::string& path, std::shared_ptr<Material> customMainMaterial)
{
	std::string ext = GetFileExtension(path);
	if (ext.contains("obj"))
	{
		loadObjModel(path, customMainMaterial);
	}
	else
	{
		// TODO: неизвестные форматы
		//Error("Unknown model format: " + path);

		loadAssimpModel(path, customMainMaterial);
	}
}
//=============================================================================
void Model::loadObjModel(const std::string& path, std::shared_ptr<Material> customMainMaterial)
{
	std::string directory = GetFileDirectory(path);

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
				material = std::make_shared<Material>(Texture2D::LoadFromFile(directory + materials[shape.mesh.material_ids[0]].diffuse_texname), nullptr, nullptr); // TODO: spec and rought textures
			else
				material = GetDefaultMeshMaterial();
		}
		processObjMesh(shape.mesh, attrib, material);
	}
}
//=============================================================================
void Model::processObjMesh(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attrib, std::shared_ptr<Material> material)
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

	m_meshes.push_back(Mesh(vertices, indices, material, glm::mat4(1.0f)));
}
//=============================================================================
void Model::loadAssimpModel(const std::string& path, std::shared_ptr<Material> material)
{
	// Load scene from file
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(),
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_ImproveCacheLocality |
		aiProcess_SortByPType |
		aiProcess_OptimizeMeshes); // TODO: aiProcess_FlipUVs?
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Error("Failed to open scene file: " + std::string(importer.GetErrorString()));
		return;
	}

	std::string directory = GetFileDirectory(path);

	// Обрабатываем корневой узел и все его потомки
	processAssimpNode(directory, scene->mRootNode, scene, material);
}
//=============================================================================
void Model::processAssimpNode(const std::string& directoryModel, aiNode* node, const aiScene* scene, std::shared_ptr<Material> material)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		glm::mat4 localMat = glm::transpose(*(glm::mat4*)&node->mTransformation);
		m_meshes.emplace_back(processAssimpMesh(directoryModel, localMat, mesh, scene, material));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processAssimpNode(directoryModel, node->mChildren[i], scene, material);
	}
}
//=============================================================================
Mesh Model::processAssimpMesh(const std::string& directoryModel, const glm::mat4& localMat, aiMesh* mesh, const aiScene* scene, std::shared_ptr<Material> defaultMaterial)
{
	std::vector<MeshVertex> vertices(mesh->mNumVertices);

	// Обрабатываем вершины
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		MeshVertex& vertex = vertices[i];
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		if (mesh->HasNormals())
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		else
			vertex.Normal = glm::vec3{ 0.0f, 1.0f, 0.0f };

		if (mesh->mTextureCoords[0])
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			vertex.TexCoords = glm::vec2{ 0.0f };
	}

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Обрабатываем материал
	auto material = defaultMaterial;
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];
		material = std::make_shared<Material>(
			loadAssimpTexture(directoryModel, aiMaterial, aiTextureType_DIFFUSE),
			loadAssimpTexture(directoryModel, aiMaterial, aiTextureType_SPECULAR),
			loadAssimpTexture(directoryModel, aiMaterial, aiTextureType_HEIGHT));
	}

	return { vertices, indices, material, localMat };
}
//=============================================================================
std::shared_ptr<Texture2D> Model::loadAssimpTexture(const std::string& directoryModel, aiMaterial* mat, aiTextureType type)
{
	if (mat->GetTextureCount(type) > 0)
	{
		aiString str;
		mat->GetTexture(type, 0, &str);
		std::string path = std::string(str.C_Str());
		return Texture2D::LoadFromFile(directoryModel + path);
	}

	return nullptr; // Если текстуры нет
}
//=============================================================================