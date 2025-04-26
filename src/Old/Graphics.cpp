#include "stdafx.h"
#include "Graphics.h"
#include "Core.h"
//=============================================================================
namespace
{
	std::shared_ptr<Materialo> DefaultMeshMaterial;
}
//=============================================================================
void ClearDefaultGraphicsResource()
{
	DefaultMeshMaterial.reset();
}
//=============================================================================
std::shared_ptr<Materialo> GetDefaultMeshMaterial()
{
	if (!DefaultMeshMaterial)
	{
		uint8_t defColor[] =
		{
			64,  64,  64,
			255, 150, 200,
			255, 150, 200,
			64,  64,  64,
		};
		std::shared_ptr<Texture2Do> diffuseTexture = Texture2Do::LoadFromMemory(2, 2, ImageFormat::RGB, defColor);

		uint8_t specColor[] =
		{
			255, 255, 255,
			255, 255, 255,
			255, 255, 255,
			255, 255, 255,
		};
		std::shared_ptr<Texture2Do> specularTexture = Texture2Do::LoadFromMemory(2, 2, ImageFormat::RGB, specColor);

		uint8_t roughColor[] =
		{
			128, 128, 128,
			128, 128, 128,
			128, 128, 128,
			128, 128, 128,
		};
		std::shared_ptr<Texture2Do> roughnessTexture = Texture2Do::LoadFromMemory(2, 2, ImageFormat::RGB, roughColor);

		DefaultMeshMaterial = std::make_shared<Materialo>(diffuseTexture, specularTexture, roughnessTexture);
	}

	return DefaultMeshMaterial;
}
//=============================================================================
Materialo::Materialo(
	std::shared_ptr<Texture2Do> DiffuseTexture,
	std::shared_ptr<Texture2Do> SpecularTexture,
	std::shared_ptr<Texture2Do> RoughnessTexture,
	float EmissivePower)
	: diffuseTexture(DiffuseTexture)
	, specularTexture(SpecularTexture)
	, roughnessTexture(RoughnessTexture)
	, emissivePower(EmissivePower)
{
	// TODO: если нет нужных текстур, брать дефолтные
	if (!diffuseTexture || !specularTexture || !roughnessTexture)
	{
		auto defaultMat = GetDefaultMeshMaterial();
		if (!diffuseTexture) diffuseTexture = defaultMat->diffuseTexture;
		if (!specularTexture) specularTexture = defaultMat->specularTexture;
		if (!roughnessTexture) roughnessTexture = defaultMat->roughnessTexture;
	}

	transparent = diffuseTexture->HasTransparency();
}
//=============================================================================
void Materialo::Bind(uint32_t diffuseTexSlot, uint32_t specularTexSlot, uint32_t roughnessTexSlot)
{
	diffuseTexture->Bind(diffuseTexSlot);
	specularTexture->Bind(specularTexSlot);
	roughnessTexture->Bind(roughnessTexSlot);
}
//=============================================================================
Mesh0::Mesh0(const std::vector<MeshVertex0>& vertices, const std::vector<uint32_t>& indices, std::shared_ptr<Materialo> material, const glm::mat4& localTransform)
	: m_material(material)
	, m_localTransform(localTransform)
{
	if (!m_material) m_material = GetDefaultMeshMaterial();
	m_vertexBuffer = std::make_shared<VertexBuffer>(vertices.size() * sizeof(MeshVertex0), vertices.data());
	m_indexBuffer = std::make_shared<IndexBuffer>(indices.size(), indices.data());
	m_VAO = std::make_shared<VertexArrayo>(m_vertexBuffer, m_indexBuffer, MeshVertex0::GetLayout());
}
//=============================================================================
void Mesh0::Draw()
{
	m_material->Bind();
	m_VAO->Bind();
	glDrawElements(GL_TRIANGLES, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
}
//=============================================================================
Model0::Model0(const std::vector<Mesh0>& meshes)
{
	m_meshes = meshes;
}
//=============================================================================
Model0::Model0(const std::string& path, std::shared_ptr<Materialo> customMainMaterial)
{
	loadModel(path, customMainMaterial);
}
//=============================================================================
void Model0::Draw()
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Draw();
	}
}
//=============================================================================
void Model0::DrawMesh(size_t i)
{
	m_meshes[i].Draw();
}
//=============================================================================
std::shared_ptr<Model0> Model0::CreateCube(float length, std::shared_ptr<Materialo> material)
{
	std::vector<MeshVertex0> vertices = {
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

	return std::make_shared<Model0>(std::vector<Mesh0>{ {vertices, indices, material, glm::mat4(1.0f)} });
}
//=============================================================================
std::shared_ptr<Model0> Model0::CreateSphere(float radius, uint32_t uiTessU, uint32_t uiTessV, std::shared_ptr<Materialo> material)
{
	// Init params
	float fDPhi = (float)M_PI / (float)uiTessV;
	float fDTheta = (float)(M_PI + M_PI) / (float)uiTessU;

	// Determine required parameters
	uiTessU = uiTessU + 1;
	uint32_t uiNumVertices = (uiTessU * (uiTessV + 1));
	uint32_t uiNumIndices = uiTessU * uiTessV * 6;

	std::vector<MeshVertex0> vertices(uiNumVertices);
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

	return std::make_shared<Model0>(std::vector<Mesh0>{ {vertices, indices, material, glm::mat4(1.0f)} });
}
//=============================================================================
std::shared_ptr<Model0> Model0::CreatePlane(float width, float height, float texWidth, float texHeight, std::shared_ptr<Materialo> material)
{
	std::vector<MeshVertex0> vertices;
	std::vector<unsigned int> indices;

	// Создаем четыре вершины для плоскости
	vertices.push_back({ { -width / 2.0f, 0.0f, -height / 2.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } });
	vertices.push_back({ {  width / 2.0f, 0.0f, -height / 2.0f }, { 0.0f, 1.0f, 0.0f }, { texWidth, 0.0f } });
	vertices.push_back({ {  width / 2.0f, 0.0f,  height / 2.0f }, { 0.0f, 1.0f, 0.0f }, { texWidth, texHeight } });
	vertices.push_back({ { -width / 2.0f, 0.0f,  height / 2.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, texHeight } });

	// Создаем два треугольника из этих четырех вершин
	indices = { 2, 1, 0, 0, 3, 2 };

	return std::make_shared<Model0>(std::vector<Mesh0>{ {vertices, indices, material, glm::mat4(1.0f)} });
}
//=============================================================================
void Model0::loadModel(const std::string& path, std::shared_ptr<Materialo> customMainMaterial)
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
void Model0::loadObjModel(const std::string& path, std::shared_ptr<Materialo> customMainMaterial)
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
		std::shared_ptr<Materialo> material;
		if (customMainMaterial)
		{
			material = customMainMaterial;
		}
		else
		{
			if (materials.size() > 0)
				material = std::make_shared<Materialo>(Texture2Do::LoadFromFile(directory + materials[shape.mesh.material_ids[0]].diffuse_texname), nullptr, nullptr); // TODO: spec and rought textures
			else
				material = GetDefaultMeshMaterial();
		}
		processObjMesh(shape.mesh, attrib, material);
	}
}
//=============================================================================
void Model0::processObjMesh(const tinyobj::mesh_t& mesh, const tinyobj::attrib_t& attrib, std::shared_ptr<Materialo> material)
{
	std::vector<MeshVertex0> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh.indices.size(); i++)
	{
		const tinyobj::index_t& index = mesh.indices[i];
		
		MeshVertex0 vertex;
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

	m_meshes.push_back(Mesh0(vertices, indices, material, glm::mat4(1.0f)));
}
//=============================================================================
void Model0::loadAssimpModel(const std::string& path, std::shared_ptr<Materialo> material)
{
	// Load scene from file
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(),
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		//aiProcess_ConvertToLeftHanded |       // TODO: левосторонняя система, пока не используется
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		//aiProcess_PreTransformVertices | // TODO: удаляет локальную матрицу трансформации - но также возможно удаляет анимацию
		aiProcess_ImproveCacheLocality |
		aiProcess_SortByPType |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph
	);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Error("Failed to open scene file: " + std::string(importer.GetErrorString()));
		return;
	}

	std::string directory = GetFileDirectory(path);

	std::vector<Mesh0> transMesh;
	std::vector<Mesh0> solidMesh;

	// Обрабатываем корневой узел и все его потомки
	processAssimpNode(directory, scene->mRootNode, scene, material, transMesh, solidMesh);

	m_meshes.reserve(transMesh.size() + solidMesh.size()); // Резервируем память для оптимизации
	m_meshes.insert(m_meshes.end(), solidMesh.begin(), solidMesh.end());
	m_meshes.insert(m_meshes.end(), transMesh.begin(), transMesh.end());

	Print("num mesh: " + std::to_string(m_meshes.size()));
}
//=============================================================================
void Model0::processAssimpNode(const std::string& directoryModel, aiNode* node, const aiScene* scene, std::shared_ptr<Materialo> material, std::vector<Mesh0>& transMesh, std::vector<Mesh0>& solidMesh)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		glm::mat4 localMat = glm::transpose(*(glm::mat4*)&node->mTransformation);

		Mesh0 mesh = processAssimpMesh(directoryModel, localMat, aimesh, scene, material);
		if (mesh.GetMaterial()->transparent)
			transMesh.emplace_back(mesh);
		else
			solidMesh.emplace_back(mesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processAssimpNode(directoryModel, node->mChildren[i], scene, material, transMesh, solidMesh);
	}
}
//=============================================================================
Mesh0 Model0::processAssimpMesh(const std::string& directoryModel, const glm::mat4& localMat, aiMesh* mesh, const aiScene* scene, std::shared_ptr<Materialo> defaultMaterial)
{
	std::vector<MeshVertex0> vertices(mesh->mNumVertices);

	// Обрабатываем вершины
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		MeshVertex0& vertex = vertices[i];
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

		// emissive
		aiColor4D EmissiveColour(0.f, 0.f, 0.f, 0.f);
		aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, EmissiveColour);
		aiColor4D DiffuseColour(1.f, 1.f, 1.f, 1.f);
		aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColour);
		float emissive = EmissiveColour.r / DiffuseColour.r;

		// reflective material
		float reflectivity = 0.0f;
		aiMaterial->Get(AI_MATKEY_REFLECTIVITY, reflectivity);

		material = std::make_shared<Materialo>(
			loadAssimpTexture(directoryModel, aiMaterial, aiTextureType_DIFFUSE),
			loadAssimpTexture(directoryModel, aiMaterial, aiTextureType_SPECULAR),
			loadAssimpTexture(directoryModel, aiMaterial, aiTextureType_SHININESS),
			emissive);
		material->reflective = (reflectivity > 0.0f);
	}

	return { vertices, indices, material, localMat };
}
//=============================================================================
std::shared_ptr<Texture2Do> Model0::loadAssimpTexture(const std::string& directoryModel, aiMaterial* mat, aiTextureType type)
{
	if (mat->GetTextureCount(type) > 0)
	{
		aiString str;
		mat->GetTexture(type, 0, &str);
		std::string path = std::string(str.C_Str());
		return Texture2Do::LoadFromFile(directoryModel + path);
	}

	return nullptr; // Если текстуры нет
}
//=============================================================================