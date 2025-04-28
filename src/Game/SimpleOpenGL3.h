#pragma once

#define GLCheckError() (glGetError() == GL_NO_ERROR)

#include "TempTextureGL3.h"

namespace gl3
{
	struct VertexAttribute final
	{
		GLuint  index;      // индекс атрибута (location в шейдере)
		GLint   size;       // Количество компонентов (например, 3 для vec3)
		GLenum  type;       // Тип данных (например, GL_FLOAT)
		bool    normalized; // Нормализовать ли данные 
		GLsizei offset;     // Смещение в байтах относительно начала структуры Vertex, например offsetof(Vertex, position)
	};

	/*
	std::vector<VertexAttribute> attributes = {
		{0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position)},
		{1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord)}
	};
	SetVertexFormat(sizeof(Vertex), attributes);
	*/
	void SetVertexFormat(size_t vertexSize, const std::vector<VertexAttribute>& attributes);

	GLuint CreateShader(GLenum type, const char* shaderSource);
	GLuint CreateShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
	GLuint CreateVertexBuffer(GLenum usage, size_t size, void* data);
	GLuint CreateIndexBuffer(GLenum usage, size_t size, void* data);
	GLuint CreateVertexArray(GLuint vbo, GLuint ibo, size_t vertexSize, const std::vector<VertexAttribute>& attributes);
	GLuint CreateTexture2D(GLenum internalFormat, GLenum format, int width, int height, void* data);
	GLuint LoadTexture2D(const char* filename, bool isFlip = false);
}

namespace utils
{
	class Transform final
	{
	public:
		void SetPosition(const glm::vec3& position);
		void SetScale(const glm::vec3& scale);
		void SetScale(float scale);
		void SetRotation(const glm::vec3& eulerAngles);
		void SetRotation(const glm::quat& rotation);

		void Translate(const glm::vec3 movePosition);
		void Rotate(const glm::vec3& anglesRadians);

		const glm::vec3& GetPosition() const { return m_position; }
		const glm::vec3& GetScale() const { return m_scale; }
		const glm::quat& GetRotation() const { return m_rotation; }

		glm::mat4 GetWorldMatrix() const;

	private:
		glm::vec3 m_position = glm::vec3(0.0f);
		glm::vec3 m_scale = glm::vec3(1.0f);
		glm::quat m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	};

	enum class CameraMovement : uint8_t
	{
		Forward,
		Backward,
		Left,
		Right
	};

	class Camera final
	{
	public:

		Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = 90.0f, float pitch = 0.0f);

		void Move(CameraMovement direction, float deltaTime);

		void Rotate(float xOffset, float yOffset);

		glm::mat4 GetViewMatrix() const;
	private:
		void updateVector();

		// camera Attributes
		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;
		glm::vec3 m_worldUp;
		// euler angles
		float m_yaw{ 90.0f };
		float m_pitch{ 0.0 };
		// camera options
		float m_speed{ 10.0f };
		float m_sensitivity{ 0.1f };
	};

	class CameraTemp
	{
	public:
		CameraTemp(int WindowWidth, int WindowHeight);
		CameraTemp(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);

		void SetPosition(float x, float y, float z);

		void OnKeyboard(unsigned char key);

		void OnMouse(int x, int y);

		void OnRender();

		Matrix4f GetMatrix();

	private:

		void init();
		void update();

		Vector3f m_pos;
		Vector3f m_target;
		Vector3f m_up;
		float m_speed = 1.0f;

		int m_windowWidth;
		int m_windowHeight;

		float m_AngleH;
		float m_AngleV;

		bool m_OnUpperEdge;
		bool m_OnLowerEdge;
		bool m_OnLeftEdge;
		bool m_OnRightEdge;

		Vector2i m_mousePos;
	};

	struct PBRMaterial final
	{
		float     roughness = 0.0f;
		bool      isMetal = false;
		glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
		Texture* pAlbedo = NULL;
		Texture* pRoughness = NULL;
		Texture* pMetallic = NULL;
		Texture* pNormalMap = NULL;
		Texture* pAO = NULL;
		Texture* pEmissive = NULL;
	};

	// TODO: удаление текстур
	// TODO: кеш текстур
	class Material final
	{
	public:
		~Material()
		{
			if (pDiffuse) {
				delete pDiffuse;
			}

			if (pSpecularExponent) {
				delete pSpecularExponent;
			}
		}

		std::string name;
		glm::vec4 ambientColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 diffuseColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 specularColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		PBRMaterial pbrMaterial;

		Texture* pDiffuse = NULL; // base color of the material
		Texture* pNormal = NULL;
		Texture* pSpecularExponent = NULL;

		float transparencyFactor{ 1.0f };
		float alphaTest{ 0.0f };
	};

	class WorldTrans
	{
	public:
		WorldTrans() {}

		void SetScale(float scale);
		void SetRotation(float x, float y, float z);
		void SetRotation(const Vector3f&);
		void SetPosition(float x, float y, float z);
		void SetPosition(const Vector3f& WorldPos);

		void Rotate(float x, float y, float z);

		Matrix4f GetMatrix() const;

		Vector3f WorldPosToLocalPos(const Vector3f& WorldPos) const;
		Vector3f WorldDirToLocalDir(const Vector3f& WorldDir) const;

		Matrix4f GetReversedTranslationMatrix() const;
		Matrix4f GetReversedRotationMatrix() const;

		float GetScale() const { return m_scale; }
		Vector3f GetPos() const { return m_pos; }
		Vector3f GetRotation() const { return m_rotation; }

	private:
		float    m_scale = 1.0f;
		Vector3f m_rotation = Vector3f(0.0f, 0.0f, 0.0f);
		Vector3f m_pos = Vector3f(0.0f, 0.0f, 0.0f);
	};

	class IRenderCallbacks
	{
	public:
		virtual void DrawStartCB(uint32_t DrawIndex) {}

		virtual void ControlSpecularExponent(bool IsEnabled) {}

		virtual void SetMaterial(const Material& material) {}

		virtual void DisableDiffuseTexture() {}
	};
	
	class Mesh final
	{
	public:
		Mesh(const std::string& filename, int assimpFlags = ASSIMP_LOAD_FLAGS);
		~Mesh();

		void Render(IRenderCallbacks* pRenderCallbacks = NULL);
		void Render(uint32_t DrawIndex, uint32_t PrimID);
		void Render(uint32_t NumInstances, const Matrix4f* WVPMats, const Matrix4f* WorldMats);

		const Material& GetMaterial();

		PBRMaterial& GetPBRMaterial() { return m_materials[0].pbrMaterial; };

		void GetLeadingVertex(uint32_t DrawIndex, uint32_t PrimID, Vector3f& Vertex);

		void SetPBR(bool IsPBR) { m_isPBR = IsPBR; }

		void SetScale(float Scale)
		{
			m_worldTransform.SetScale(Scale);
		}
		void SetRotation(float x, float y, float z)
		{
			m_worldTransform.SetRotation(x, y, z);
		}
		void SetPosition(float x, float y, float z)
		{
			m_worldTransform.SetPosition(x, y, z);
		}
		void SetPosition(const Vector3f& pos) { SetPosition(pos.x, pos.y, pos.z); }

		Vector3f GetPosition() const
		{
			return m_worldTransform.GetPos();
		}

		WorldTrans& GetWorldTransform() { return m_worldTransform; }

		Matrix4f GetWorldMatrix() { return m_worldTransform.GetMatrix(); }
	private:
		bool initFromScene(const aiScene* pScene, const std::string& Filename);
		void countVerticesAndIndices(const aiScene* pScene, uint32_t& NumVertices, uint32_t& NumIndices);
		void reserveSpace(uint32_t NumVertices, uint32_t NumIndices);
		void initAllMeshes(const aiScene* pScene);
		void initSingleMesh(uint32_t MeshIndex, const aiMesh* paiMesh);
		void initSingleMeshOpt(uint32_t MeshIndex, const aiMesh* paiMesh);
		bool initMaterials(const aiScene* pScene, const std::string& Filename);
		void loadTextures(const std::string& Dir, const aiMaterial* pMaterial, int index);

		void loadDiffuseTexture(const std::string& Dir, const aiMaterial* pMaterial, int index);
		void loadDiffuseTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex);
		void loadDiffuseTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex);

		void loadSpecularTexture(const std::string& Dir, const aiMaterial* pMaterial, int index);
		void loadSpecularTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex);
		void loadSpecularTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex);

		void loadAlbedoTexture(const std::string& Dir, const aiMaterial* pMaterial, int index);
		void loadAlbedoTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex);
		void loadAlbedoTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex);

		void loadMetalnessTexture(const std::string& Dir, const aiMaterial* pMaterial, int index);
		void loadMetalnessTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex);
		void loadMetalnessTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex);

		void loadRoughnessTexture(const std::string& Dir, const aiMaterial* pMaterial, int index);
		void loadRoughnessTextureEmbedded(const aiTexture* paiTexture, int MaterialIndex);
		void loadRoughnessTextureFromFile(const std::string& dir, const aiString& Path, int MaterialIndex);

		void loadColors(const aiMaterial* pMaterial, int index);

		void populateBuffers();
		void populateBuffersNonDSA();
		void populateBuffersDSA();

		void setupRenderMaterialsPhong(unsigned int MeshIndex, unsigned int MaterialIndex, IRenderCallbacks* pRenderCallbacks);
		void setupRenderMaterialsPBR();


		WorldTrans m_worldTransform;

		struct Vertex final
		{
			glm::vec3 Position;
			glm::vec2 TexCoords;
			glm::vec3 Normal;
		};
		struct BasicMeshEntry final
		{
			uint32_t NumIndices{ 0 };
			uint32_t BaseVertex{ 0 };
			uint32_t BaseIndex{ 0 };
			uint32_t MaterialIndex{ INVALID_MATERIAL };
		};
		std::vector<BasicMeshEntry> m_meshes;
		const aiScene* m_scene{ nullptr };
		Matrix4f m_globalInverseTransform;
		std::vector<uint32_t> m_indices;

		enum BUFFER_TYPE
		{
			INDEX_BUFFER = 0,
			VERTEX_BUFFER = 1,
			WVP_MAT_BUFFER = 2,  // required only for instancing
			WORLD_MAT_BUFFER = 3,  // required only for instancing
			NUM_BUFFERS = 4
		};

		GLuint m_vao{ 0 };
		GLuint m_buffers[NUM_BUFFERS] = { 0 };

		std::vector<Material> m_materials;

		// Temporary space for vertex stuff before we load them into the GPU
		std::vector<Vertex> m_vertices;

		Assimp::Importer m_importer;
		bool m_isPBR{ false };
	};
}
