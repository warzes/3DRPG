#pragma once

enum class ShaderDataType : uint8_t
{
	None = 0,
	Float,
	Float2,
	Float3,
	Float4,
	Mat3,
	Mat4,
	Int,
	Int2,
	Int3,
	Int4,
	Bool
};

unsigned int ShaderDataTypeSize(ShaderDataType type);
unsigned int GetComponentCount(ShaderDataType type);
GLenum GetShaderDataType(ShaderDataType type);

struct VertexBufferElement final
{
	VertexBufferElement() = default;
	VertexBufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		: type(type)
		, name(name)
		, size(ShaderDataTypeSize(type))
		, offset(0)
		, normalized(normalized)
	{
	}

	ShaderDataType type;
	std::string    name;
	unsigned int   size;
	unsigned int   offset;
	bool           normalized;
};

class VertexBufferLayout final
{
public:
	VertexBufferLayout() = default;

	template<typename T>
	void Push(const std::string& name) { static_assert(false); }

	template<>
	void Push<float>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Float, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<glm::vec2>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Float2, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<glm::vec3>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Float3, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<glm::vec4>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Float4, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<glm::mat3>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Mat3, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<glm::mat4>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Mat4, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<int>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Int, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<glm::ivec2>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Int2, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<glm::ivec3>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Int3, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<glm::ivec4>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Int4, name });
		calculateOffsetsAndStride();
	}

	template<>
	void Push<bool>(const std::string& name)
	{
		m_elements.push_back({ ShaderDataType::Bool, name });
		calculateOffsetsAndStride();
	}

	const std::vector<VertexBufferElement>& GetElements() const { return m_elements; }
	unsigned int GetStride() const { return m_stride; }

private:
	void calculateOffsetsAndStride()
	{
		unsigned int offset = 0;
		m_stride = 0;
		for (auto& element : m_elements)
		{
			element.offset = offset;
			offset += element.size;
			m_stride += element.size;
		}
	}

	std::vector<VertexBufferElement> m_elements;
	unsigned int m_stride = 0;
};

class VertexBuffer final
{
public:
	VertexBuffer(unsigned int size, const void* data = nullptr);
	~VertexBuffer();

	void SetData(const void* data, unsigned int size, unsigned int offset = 0);

	GLuint GetID() const { return m_id; }

private:
	GLuint m_id;
};

class IndexBuffer final
{
public:
	IndexBuffer(uint32_t count, const unsigned int* data);
	~IndexBuffer();

	void SetData(const void* data, unsigned int size, unsigned int offset = 0);

	GLuint GetID() const { return m_id; }
	uint32_t GetCount() const { return m_count; }

private:
	GLuint m_id;
	uint32_t m_count;
};

class UniformBuffer final
{
public:
	UniformBuffer(uint32_t bindingPoint, uint32_t size);
	~UniformBuffer();

	void SetData(const void* data, uint32_t size, uint32_t offset = 0);

	GLuint GetID() const { return m_id; }

private:
	GLuint m_id;
	uint32_t m_bindingPoint;
};

class VertexArray final 
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	GLuint GetID() const { return m_id; }

private:
	GLuint m_id;
};

class Texture2D final
{
public:
	static Texture2D LoadFromMemory(int width, int height, void* imageData);
	static Texture2D LoadFromFile(const std::string& path, bool flipVertical = false);

	void Bind(unsigned int slot = 0) const;

	unsigned int GetID() const { return m_id; }

private:
	Texture2D(GLuint rendererID) : m_id(rendererID) {}
	GLuint m_id{ 0 };
};

class FrameBuffer final
{
public:
	FrameBuffer(unsigned int width, unsigned int height);
	~FrameBuffer();

	void Bind() const;

	void Resize(unsigned int width, unsigned int height);

	void BindColorTexture(GLuint textureUnit) const;
	void BindВepthTexture(GLuint textureUnit) const;

private:
	GLuint m_id;
	GLuint m_colorAttachment;
	GLuint m_depthAttachment;
};

class Shader final
{
public:
	Shader(const std::string& vertexShader, const std::string& fragmentShader);
	~Shader();

	void Bind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

	GLuint GetID() const { return m_id; }

private:
	static GLuint compileShader(unsigned int type, const std::string& source);
	static GLuint createShader(const std::string& vertexShader, const std::string& fragmentShader);
	int getUniformLocation(const std::string& name);

	GLuint m_id;
	std::unordered_map<std::string, int> m_UniformLocationCache;
};