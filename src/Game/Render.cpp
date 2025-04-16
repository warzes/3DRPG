#include "stdafx.h"
#include "Render.h"
#include "Core.h"
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
	: m_size(size)
{
	glCreateBuffers(1, &m_id);
	glNamedBufferStorage(m_id, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
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
	glNamedBufferSubData(m_id, offset, (size ? size : m_size), data);
}
//=============================================================================
VertexArray::VertexArray(std::shared_ptr<VertexBuffer> vb, std::shared_ptr<IndexBuffer> ib, const VertexBufferLayout& layout)
{
	glCreateVertexArrays(1, &m_id);

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
VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_id);
}
//=============================================================================
void VertexArray::Bind()
{
	glBindVertexArray(m_id);
}
//=============================================================================
Texture2Do::~Texture2Do()
{
	glDeleteTextures(1, &m_id);
}
//=============================================================================
std::shared_ptr<Texture2Do> Texture2Do::LoadFromMemory(int width, int height, ImageFormat format, uint8_t* imageData)
{
	GLenum internalFormat{ GL_RGBA8 }, dataFormat{ GL_RGBA };
	int channels{ 4 };
	switch (format)
	{
	case ImageFormat::Grey:
		internalFormat = GL_R8;
		dataFormat     = GL_RED;
		channels       = 1;
		break;
	case ImageFormat::GreyAlpha:
		internalFormat = GL_RG8;
		dataFormat     = GL_RG;
		channels       = 2;
		break;
	case ImageFormat::RGB:
		internalFormat = GL_RGB8;
		dataFormat     = GL_RGB;
		channels       = 3;
		break;
	case ImageFormat::RGBA:
		internalFormat = GL_RGBA8;
		dataFormat     = GL_RGBA;
		channels       = 4;
		break;
	default:
		break;
	}

	// Проверяем наличие прозрачных пикселей
	bool hasTransparency = false;
	if (channels == 2 || channels == 4)
	{
		for (int i = 0; i < width * height; ++i)
		{
			uint8_t alpha = imageData[i * channels + (channels - 1)]; // Альфа-канал
			if (alpha < 255)
			{
				hasTransparency = true;
				break;
			}
		}
	}

	GLuint id;
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glTextureStorage2D(id, 1, internalFormat, width, height);
	glTextureSubImage2D(id, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, imageData);

	glGenerateTextureMipmap(id);

	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	auto texture = std::make_shared<Texture2Do>(id);
	texture->m_width = width;
	texture->m_height = height;
	texture->m_format = format;
	texture->m_hasTransparency = hasTransparency;
	return texture;
}
//=============================================================================
std::shared_ptr<Texture2Do> Texture2Do::LoadFromFile(const std::string& path, bool flipVertical)
{
	Print("Texture load: " + path);

	std::string ext = GetFileExtension(path);
	if (ext.contains("ktx"))
	{
		// TODO: в raylib без ktx.h/lib - взять оттуда

		ktxTexture* kTexture;
		KTX_error_code result = ktxTexture_CreateFromNamedFile(path.c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &kTexture);
		if (result != KTX_SUCCESS)
		{
			ktxTexture_Destroy(kTexture);
			Error("Failed to load texture: " + path + "\nError: " + ktxErrorString(result));
			return nullptr;
		}

		GLuint oglTexture = 0;
		GLenum target, glerror;
		glCreateTextures(GL_TEXTURE_2D, 1, &oglTexture);
		result = ktxTexture_GLUpload(kTexture, &oglTexture, &target, &glerror);
		if (result != KTX_SUCCESS)
		{
			ktxTexture_Destroy(kTexture);
			Error("Failed to load texture: " + path + "\nError: " + ktxErrorString(result));
			return nullptr;
		}
		ktxTexture_Destroy(kTexture);

		glBindTexture(GL_TEXTURE_2D, oglTexture);
		GLint iAlpha = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &iAlpha);
		glBindTexture(GL_TEXTURE_2D, 0);

		auto texture = std::make_shared<Texture2Do>(oglTexture);
		//texture->m_width = width;   // TODO:
		//texture->m_height = height; // TODO:
		//texture->m_format = format; // TODO:
		texture->m_hasTransparency = iAlpha > 0;
		return texture;
	}
	else
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(flipVertical);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data)
		{
			Error("Failed to load texture: " + path);
			return nullptr;
		}

		ImageFormat format;
		switch (channels)
		{
		case STBI_grey:       format = ImageFormat::Grey; break;
		case STBI_grey_alpha: format = ImageFormat::GreyAlpha; break;
		case STBI_rgb:        format = ImageFormat::RGB; break;
		case STBI_rgb_alpha:  format = ImageFormat::RGBA; break;
		}
		
		auto resurce = LoadFromMemory(width, height, format, data);
		stbi_image_free(data);
		return resurce;
	}

	return nullptr;
}
//=============================================================================
void Texture2Do::Bind(unsigned int slot) const
{
	glBindTextureUnit(slot, m_id);
}
//=============================================================================
TextureCubeo::~TextureCubeo()
{
	glDeleteTextures(1, &m_id);
}
//=============================================================================
std::shared_ptr<TextureCubeo> TextureCubeo::LoadFromMemory(int width, int height, ImageFormat format, const std::vector<uint8_t*>& imageData)
{
	if (imageData.size() != 6)
	{
		Error("TextureCube requires exactly 6 image data pointers for the cube map faces.");
		return nullptr;
	}

	GLuint id;
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);

	for (int i = 0; i < 6; ++i)
	{
		GLenum internalFormat = (format == ImageFormat::RGBA) ? GL_RGBA8 : GL_RGB8;
		GLenum dataFormat = (format == ImageFormat::RGBA) ? GL_RGBA : GL_RGB;
		glTextureSubImage3D(id, 0, 0, 0, i, width, height, 1, dataFormat, GL_UNSIGNED_BYTE, imageData[i]);
	}

	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return std::make_shared<TextureCubeo>(id);
}
//=============================================================================
std::shared_ptr<TextureCubeo> TextureCubeo::LoadFromFiles(const std::vector<std::string>& paths)
{
	std::string ext = GetFileExtension(paths[0]);
	if (ext.contains("ktx"))
	{
		ktxTexture* kTexture;
		KTX_error_code result = ktxTexture_CreateFromNamedFile(paths[0].c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &kTexture);
		if (result != KTX_SUCCESS)
		{
			ktxTexture_Destroy(kTexture);
			Error("Failed to load texture: " + paths[0] + "\nError: " + ktxErrorString(result));
			return nullptr;
		}

		GLuint oglTexture = 0;
		GLenum target, glerror;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &oglTexture);
		result = ktxTexture_GLUpload(kTexture, &oglTexture, &target, &glerror);
		if (result != KTX_SUCCESS)
		{
			ktxTexture_Destroy(kTexture);
			Error("Failed to load texture: " + paths[0] + "\nError: " + ktxErrorString(result));
			return nullptr;
		}
		ktxTexture_Destroy(kTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		return std::make_shared<TextureCubeo>(oglTexture);
	}
	else
	{
		// TODO: доделать
		return nullptr;
	}

	return nullptr;
}
//=============================================================================
void TextureCubeo::Bind(unsigned int slot) const
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
ShaderProgram::ShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	const GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	if (vs == 0) [[unlikely]]
	{
		return;
	}
	const GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	if (fs == 0) [[unlikely]]
	{
		glDeleteShader(vs);
		return;
	}

	const GLuint id = glCreateProgram();
	glAttachShader(id, vs);
	glAttachShader(id, fs);
	glLinkProgram(id);

	GLint linkResult;
	glGetProgramiv(id, GL_LINK_STATUS, &linkResult);
	if (linkResult == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
		std::string message;
		message.resize(length);
		glGetProgramInfoLog(id, length, nullptr, &message[0]);
		Error("Failed to link shaders: " + message);
		glDeleteProgram(id);
		return;
	}
	//glValidateProgram(id);

	glDeleteShader(vs);
	glDeleteShader(fs);

	m_id = id;
}
//=============================================================================
ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_id);
}
//=============================================================================
void ShaderProgram::Bind() const
{
	if (!IsValid()) [[unlikely]]
	{
		Warning("Shader Program not valid");
	}
	glUseProgram(m_id);
}
//=============================================================================
void ShaderProgram::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}
//=============================================================================
void ShaderProgram::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(getUniformLocation(name), value);
}
//=============================================================================
void ShaderProgram::SetUniform2f(const std::string& name, float v0, float v1)
{
	glUniform2f(getUniformLocation(name), v0, v1);
}
//=============================================================================
void ShaderProgram::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}
//=============================================================================
void ShaderProgram::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}
//=============================================================================
void ShaderProgram::FragmentSubRoutines(uint32_t subroutines)
{
	Bind();
	GLuint subRoutines[] = { subroutines };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &subRoutines[0]);
}
//=============================================================================
GLuint ShaderProgram::compileShader(unsigned int type, const std::string& source)
{
	std::string shaderTypeStr;
	switch (type)
	{
	case GL_VERTEX_SHADER:          shaderTypeStr = "vertex"; break;
	case GL_FRAGMENT_SHADER:        shaderTypeStr = "fragment"; break;
	case GL_GEOMETRY_SHADER:        shaderTypeStr = "geometry"; break;
	case GL_COMPUTE_SHADER:         shaderTypeStr = "compute"; break;
	case GL_TESS_CONTROL_SHADER:    shaderTypeStr = "tessellation control"; break;
	case GL_TESS_EVALUATION_SHADER: shaderTypeStr = "tessellation evaluation"; break;
	default: 
		shaderTypeStr = "unknown";
		break;
	}

	const char* src = source.c_str();

	const GLuint id = glCreateShader(type);
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		std::string message;
		message.resize(length);
		glGetShaderInfoLog(id, length, &length, &message[0]);
		Error("Failed to compile " + shaderTypeStr + " shader!\n" + message);
		glDeleteShader(id);
		return 0;
	}

	return id;
}
//=============================================================================
int ShaderProgram::getUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_id, name.c_str());
	if (location == -1) Warning("uniform '" + name + "' doesn't exist!");

	m_UniformLocationCache[name] = location;
	return location;
}
//=============================================================================