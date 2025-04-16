#pragma once

#include "RenderCore.h"
#include "RHITexture.h"

namespace rhi
{
	void Init();
	void Close();
}

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

	auto GetID() const { return m_id; }

private:
	GLuint m_id;
};

class IndexBuffer final
{
public:
	IndexBuffer(uint32_t count, const unsigned int* data);
	~IndexBuffer();

	void SetData(const void* data, unsigned int size, unsigned int offset = 0);

	auto GetID() const { return m_id; }
	auto GetCount() const { return m_count; }

private:
	GLuint m_id;
	uint32_t m_count;
};

class UniformBuffer final
{
public:
	UniformBuffer(uint32_t bindingPoint, uint32_t size);
	~UniformBuffer();

	void SetData(const void* data, uint32_t size = 0, uint32_t offset = 0);

	auto GetID() const { return m_id; }

private:
	GLuint   m_id;
	uint32_t m_size;
};

class VertexArray final 
{
public:
	VertexArray(std::shared_ptr<VertexBuffer> vb, std::shared_ptr<IndexBuffer> ib, const VertexBufferLayout& layout);
	~VertexArray();

	void Bind();

	auto GetID() const { return m_id; }

private:
	GLuint m_id;
};

class Texture2Do final
{
public:
	Texture2Do() = default;
	Texture2Do(GLuint rendererID) : m_id(rendererID) {}
	~Texture2Do();

	static std::shared_ptr<Texture2Do> LoadFromMemory(int width, int height, ImageFormat format, uint8_t* imageData);
	static std::shared_ptr<Texture2Do> LoadFromFile(const std::string& path, bool flipVertical = false);

	void Bind(unsigned int slot = 0) const;

	auto GetID() const { return m_id; }
	auto GetWidth() const { return m_width; }
	auto GetHeight() const { return m_height; }
	auto GetFormat() const { return m_format; }
	auto HasTransparency() const { return m_hasTransparency; }

private:
	GLuint      m_id{ 0 };
	int         m_width{ 0 };
	int         m_height{ 0 };
	ImageFormat m_format{ ImageFormat::RGBA };
	bool        m_hasTransparency{ false };
};

class TextureCubeo final
{
public:
	TextureCubeo() = default;
	TextureCubeo(GLuint rendererID) : m_id(rendererID) {}
	~TextureCubeo();

	static std::shared_ptr<TextureCubeo> LoadFromMemory(int width, int height, ImageFormat format, const std::vector<uint8_t*>& imageData);
	static std::shared_ptr<TextureCubeo> LoadFromFiles(const std::vector<std::string>& paths);

	void Bind(unsigned int slot = 0) const;

	auto GetID() const { return m_id; }
private:
	GLuint m_id{ 0 }; 
};


class FrameBuffero final
{
public:
	FrameBuffero(unsigned int width, unsigned int height);
	~FrameBuffero();

	void Bind() const;

	void Resize(unsigned int width, unsigned int height);

	void BindColorTexture(GLuint textureUnit) const;
	void BindВepthTexture(GLuint textureUnit) const;

private:
	GLuint m_id;
	GLuint m_colorAttachment;
	GLuint m_depthAttachment;
};

class ShaderProgram final
{
public:
	ShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	~ShaderProgram();

	void Bind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

	void FragmentSubRoutines(uint32_t subroutines);

	auto GetID() const { return m_id; }

	bool IsValid() const { return m_id > 0; }

private:
	GLuint compileShader(unsigned int type, const std::string& source);
	int getUniformLocation(const std::string& name);

	GLuint m_id{ 0 };
	std::unordered_map<std::string, int> m_UniformLocationCache;
};