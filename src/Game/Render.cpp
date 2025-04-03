#include "stdafx.h"
#include "Render.h"
#include "CoreApp.h"
//=============================================================================
unsigned int ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:  return sizeof(float);
	case ShaderDataType::Float2: return sizeof(float) * 2;
	case ShaderDataType::Float3: return sizeof(float) * 3;
	case ShaderDataType::Float4: return sizeof(float) * 4;
	case ShaderDataType::Mat3:   return sizeof(float) * 3 * 3;
	case ShaderDataType::Mat4:   return sizeof(float) * 4 * 4;
	case ShaderDataType::Int:    return sizeof(int);
	case ShaderDataType::Int2:   return sizeof(int) * 2;
	case ShaderDataType::Int3:   return sizeof(int) * 3;
	case ShaderDataType::Int4:   return sizeof(int) * 4;
	case ShaderDataType::Bool:   return 1;
	}
	return 0;
}
//=============================================================================
unsigned int GetComponentCount(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:  return 1;
	case ShaderDataType::Float2: return 2;
	case ShaderDataType::Float3: return 3;
	case ShaderDataType::Float4: return 4;
	case ShaderDataType::Mat3:   return 3 * 3;
	case ShaderDataType::Mat4:   return 4 * 4;
	case ShaderDataType::Int:    return 1;
	case ShaderDataType::Int2:   return 2;
	case ShaderDataType::Int3:   return 3;
	case ShaderDataType::Int4:   return 4;
	case ShaderDataType::Bool:   return 1;
	}
	return 0;
}
//=============================================================================
GLenum GetShaderDataType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:
	case ShaderDataType::Float2:
	case ShaderDataType::Float3:
	case ShaderDataType::Float4:
	case ShaderDataType::Mat3:
	case ShaderDataType::Mat4:
		return GL_FLOAT;
	case ShaderDataType::Int:
	case ShaderDataType::Int2:
	case ShaderDataType::Int3:
	case ShaderDataType::Int4:
		return GL_INT;
	case ShaderDataType::Bool:
		return GL_BOOL;
	case ShaderDataType::None:
	default:
		return 0;
	}
}
//=============================================================================
VertexBuffer::VertexBuffer(unsigned int size, const void* data)
{
	glCreateBuffers(1, &m_id);
	GLbitfield flags = data ? 0 : GL_DYNAMIC_STORAGE_BIT; // массив динамический, если нет данных
	glNamedBufferStorage(m_id, size, data, flags);
}
//=============================================================================
VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_id);
}
//=============================================================================
void VertexBuffer::SetData(const void* data, unsigned int size, unsigned int offset)
{
	glNamedBufferSubData(m_id, offset, size, data);
}
//=============================================================================
IndexBuffer::IndexBuffer(uint32_t count, const unsigned int* data)
	: m_count(count)
{
	glCreateBuffers(1, &m_id);
	GLbitfield flags = data ? 0 : GL_DYNAMIC_STORAGE_BIT; // массив динамический, если нет данных
	glNamedBufferStorage(m_id, count * sizeof(uint32_t), data, flags);
}
//=============================================================================
IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_id);
}
//=============================================================================
void IndexBuffer::SetData(const void* data, unsigned int size, unsigned int offset)
{
	glNamedBufferSubData(m_id, offset, size, data);
}
//=============================================================================
UniformBuffer::UniformBuffer(uint32_t bindingPoint, uint32_t size)
	: m_bindingPoint(bindingPoint)
{
	glCreateBuffers(1, &m_id);
	glNamedBufferStorage(m_id, size, nullptr, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_id);
}
//=============================================================================
UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &m_id);
}
//=============================================================================
void UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	glNamedBufferSubData(m_id, offset, size, data);
}
//=============================================================================
VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &m_id);
}
//=============================================================================
VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_id);
}
//=============================================================================
void VertexArray::AddBuffer(std::shared_ptr<VertexBuffer> vb, std::shared_ptr<IndexBuffer> ib, const VertexBufferLayout& layout)
{
	glVertexArrayVertexBuffer(m_id, 0, vb->GetID(), 0, layout.GetStride());
	glVertexArrayElementBuffer(m_id, ib->GetID());

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexArrayAttrib(m_id, i);
		glVertexArrayAttribFormat(m_id, i, GetComponentCount(element.type), GetShaderDataType(element.type), element.normalized, offset);
		glVertexArrayAttribBinding(m_id, i, 0);
		offset += element.size;
	}
}
//=============================================================================
std::shared_ptr<Texture2D> Texture2D::LoadFromMemory(int width, int height, void* imageData)
{
	GLuint id;
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureStorage2D(id, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateTextureMipmap(id);

	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return std::make_shared<Texture2D>(id);
}
//=============================================================================
std::shared_ptr<Texture2D> Texture2D::LoadFromFile(const std::string& path, bool flipVertical)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(flipVertical);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!data)
	{
		Fatal("Failed to load texture: " + path);
		return { 0 };
	}

	auto resurce = LoadFromMemory(width, height, data);
	stbi_image_free(data);
	return resurce;
}
//=============================================================================
void Texture2D::Bind(unsigned int slot) const
{
	glBindTextureUnit(slot, m_id);
}
//=============================================================================
FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
{
	glCreateFramebuffers(1, &m_id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
	glTextureStorage2D(m_colorAttachment, 1, GL_RGB8, width, height);
	glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0, m_colorAttachment, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
	glTextureStorage2D(m_depthAttachment, 1, GL_DEPTH_COMPONENT32F, width, height);
	glNamedFramebufferTexture(m_id, GL_DEPTH_ATTACHMENT, m_depthAttachment, 0);

	if (glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Fatal("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_id);
	glDeleteTextures(1, &m_colorAttachment);
	glDeleteTextures(1, &m_depthAttachment);
}
//=============================================================================
void FrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}
//=============================================================================
void FrameBuffer::Resize(unsigned int width, unsigned int height)
{
	glTextureStorage2D(m_colorAttachment, 1, GL_RGB8, width, height);
	glTextureStorage2D(m_depthAttachment, 1, GL_DEPTH_COMPONENT32F, width, height);

	glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0, m_colorAttachment, 0);
	glNamedFramebufferTexture(m_id, GL_DEPTH_ATTACHMENT, m_depthAttachment, 0);
}
//=============================================================================
void FrameBuffer::BindColorTexture(GLuint textureUnit) const
{
	glBindTextureUnit(textureUnit, m_colorAttachment);
}
//=============================================================================
void FrameBuffer::BindВepthTexture(GLuint textureUnit) const
{
	glBindTextureUnit(textureUnit, m_depthAttachment);
}
//=============================================================================
Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
{
	m_id = createShader(vertexShader, fragmentShader);
}
//=============================================================================
Shader::~Shader()
{
	glDeleteProgram(m_id);
}
//=============================================================================
void Shader::Bind() const
{
	glUseProgram(m_id);
}
//=============================================================================
void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}
//=============================================================================
void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	glUniform2f(getUniformLocation(name), v0, v1);
}
//=============================================================================
void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}
//=============================================================================
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}
//=============================================================================
GLuint Shader::compileShader(unsigned int type, const std::string& source)
{
	const char* src = source.c_str();

	GLuint id = glCreateShader(type);
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		std::string message;
		message.resize(length);
		glGetShaderInfoLog(id, length, &length, &message[0]);
		Error(std::string("Failed to compile ") + (type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader!\n" + message);
		glDeleteShader(id);
		return 0;
	}

	return id;
}
//=============================================================================
GLuint Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLuint program = glCreateProgram();
	GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}
//=============================================================================
int Shader::getUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_id, name.c_str());
	if (location == -1) Warning("uniform '" + name + "' doesn't exist!");

	m_UniformLocationCache[name] = location;
	return location;
}
//=============================================================================